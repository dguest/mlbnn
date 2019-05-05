#!/usr/bin/env python3

"""
Simple script to train a neural network with keras
"""

import argparse
from itertools import product
import numpy as np
from matplotlib import pyplot as plt
import h5py
import os
import json

# grab the same histogram bounds we used in the plotting scripts
from make_hists import BOUNDS

def get_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input_file')
    parser.add_argument('-e','--epochs', type=int, default=1)
    parser.add_argument('-o','--output-dir', default='model')
    return parser.parse_args()

###############################################################
# Main Training Part
###############################################################

def run():
    args = get_args()

    # get the jets out of the input file.
    with h5py.File(args.input_file, 'r') as infile:
        jets = np.asarray(infile['jets'])
        tracks = np.asarray(infile['tracks'])

    # first, let's make the training dataset!
    mask_value = 999
    jet_inputs, track_inputs = preproc_inputs(jets, tracks, mask_value)
    targets = make_targets(jets)

    # now make the network
    from keras.layers import Input, TimeDistributed, Dense, Softmax, Masking
    from keras.layers import Concatenate
    from SumLayer import SumLayer
    from keras.models import Model

    track_node = Input(shape=track_inputs.shape[1:])
    tdd = Masking(mask_value=mask_value)(track_node)
    for x in range(2):
        tdd = TimeDistributed(Dense(20))(tdd)
    track_sum = SumLayer()(tdd)

    jet_node = Input(shape=jet_inputs.shape[1:])
    merged = Concatenate()([jet_node, track_sum])
    for x in range(2):
        merged = Dense(20)(merged)

    dense2 = Dense(3)(merged)
    pred = Softmax()(dense2)
    model = Model(inputs=[jet_node, track_node], outputs=pred)
    model.compile(optimizer='adam',
                  loss='categorical_crossentropy',
                  metrics=['accuracy'])

    # now fit this thing!
    model.fit([jet_inputs, track_inputs], targets, epochs=args.epochs)

    # finally, save the trained network
    odir = args.output_dir
    if not os.path.isdir(odir):
        os.mkdir(odir)
    with open(f'{odir}/architecture.json','w') as arch_file:
        arch_file.write(model.to_json(indent=2))
    model.save_weights(f'{odir}/weights.h5')

    # also write out the variable specification
    with open(f'{odir}/variables.json', 'w') as vars_file:
        json.dump(get_variables_json(), vars_file)


def make_targets(jets):
    labels = jets['HadronConeExclExtendedTruthLabelID']
    targets = np.stack([labels == x for x in [0, 4, 5]], axis=1)
    return np.asarray(targets, dtype=int)


def get_discrim(jets, model_path, weights_path):
    """
    Get the discriminant from a saved model
    """

    # this just silences some annoying warnings that I get from
    # tensorflow. They might be useful for training but in evaluation
    # they should be harmless.
    os.environ['TF_CPP_MIN_LOG_LEVEL']='2'

    # load the model
    from keras.models import model_from_json
    with open(model_path,'r') as model_file:
        model = model_from_json(model_file.read())
    model.load_weights(weights_path)

    # get the model inputs
    input_data = preproc_inputs(jets)
    outputs = model.predict(input_data)
    b_weight, light_weights = outputs[:,2], outputs[:,0]
    output_log_ratio = np.log(b_weight / light_weights)
    return output_log_ratio



####################################################################
# Preprocessing and saving the configuration
####################################################################
#

def track_vars():
    ip = [
        f'IP3D_signed_{x}0{y}' for x, y in product('dz',['','_significance'])]
    return ip + ['dr','ptfrac']

def jet_vars():
    ip = [f'IP{x}D_p{y}' for x, y in product([2,3],'bcu')]
    jf = [
        f'JetFitter_{x}' for x in ['significance3d', 'mass','energyFraction']]
    return ip + jf

def preproc_inputs(jets, tracks, mask_value):
    """
    We make some hardcoded transformations to normalize these inputs
    """

    ptfrac = tracks['ptfrac']
    invalid = np.isnan(tracks['ptfrac'])
    track_array = np.stack([tracks[x] for x in track_vars()], axis=2)
    track_array[invalid,:] = mask_value

    jn = {'JetFitter_mass': 0.001}
    rja = np.stack([jets[x]*jn.get(x,1) for x in jet_vars()], axis=1)
    jet_array = np.nan_to_num(rja)
    return jet_array, track_array

def get_variables_json():
    """
    Make a file that specifies the input variables and
    transformations as JSON, so that the network can be used with lwtnn
    """

    # This is a more 'traditional' network with one set of inputs so
    # we just have to name the variables in one input node. In more
    # advanced cases we could have multiple input nodes, some of which
    # might operate on sequences.
    btag_variables = [
        {
            # Note this is not the same name we use in the file! We'll
            # have to make the log1p transformation in the C++ code to
            # build this variable.
            'name': 'jf_sig_log1p',
            # 'offset': JF_OFFSET,
            # 'scale': JF_SCALE,
        },
        {
            'name': 'rnnip_log_ratio',
            # 'offset': RNN_OFFSET,
            # 'scale': RNN_SCALE,
            # 'default': RNN_DEFAULT,
        }
    ]

    # note that this is a list of output nodes, where each node can
    # have multiple output values. In principal we could also add a
    # regression output to the same network (i.e. the b-hadron pt) but
    # that's a more advanced subject.
    outputs = [
        {
            'name': 'classes',
            'labels': ['light', 'charm', 'bottom']
        }
    ]

    # lwtnn expects a specific format that allows multiple input and
    # output nodes, so we have to dress the above information a bit.
    final_dict = {
        'input_sequences': [],
        'inputs': [
            {
                'name': 'btag_variables',
                'variables': btag_variables
            }
        ],
        'outputs': outputs
    }

    return final_dict



if __name__ == '__main__':
    run()
