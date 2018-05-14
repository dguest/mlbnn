#!/usr/bin/env python3

"""
Simple script to draw roc curves
"""

import argparse
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
    parser.add_argument('-o','--output-dir', default='model')
    return parser.parse_args()

def run():
    args = get_args()

    # get the jets out of the input file.
    with h5py.File(args.input_file, 'r') as infile:
        jets = np.asarray(infile['jets'])

    # first, let's make the training dataset!
    input_data = preproc_inputs(jets)
    targets = make_targets(jets)

    # now make the network
    from keras.layers import Input, Dense, Softmax
    from keras.models import Model

    input_node = Input(shape=(2,))
    dense = Dense(3)(input_node)
    pred = Softmax()(dense)
    model = Model(inputs=input_node, outputs=pred)
    model.compile(optimizer='rmsprop',
                  loss='categorical_crossentropy',
                  metrics=['accuracy'])

    # now fit this thing!
    model.fit(input_data, targets)

    # finally, save the trained network
    odir = args.output_dir
    if not os.path.isdir(odir):
        os.mkdir(odir)
    with open(f'{odir}/architecture.json','w') as arch_file:
        arch_file.write(model.to_json(indent=2))
    model.save_weights(f'{odir}/weights.h5')

def get_discrim(jets, model_path, weights_path):
    """
    Get the discriminant from a saved model
    """

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

def preproc_inputs(jets):
    """
    We make some hardcoded transformations to normalize these inputs
    they don't have to be exact.
    """
    jf = (np.log1p(jets['jf_sig']) - 0.7 ) * 0.75
    print('jf mean: {}, std: {}'.format(np.mean(jf), np.std(jf)))
    rnnip = jets['rnnip_log_ratio']
    rnnip[np.isnan(rnnip)] = -9
    rnnip = (rnnip + 3.0) * 0.25
    print('rnnip mean: {}, std: {}'.format(np.mean(rnnip), np.std(rnnip)))
    return np.stack([jf, rnnip], axis=1)

def make_targets(jets):
    labels = jets['HadronConeExclExtendedTruthLabelID']
    targets = np.stack([labels == x for x in [0, 4, 5]], axis=1)
    return np.asarray(targets, dtype=int)



if __name__ == '__main__':
    run()
