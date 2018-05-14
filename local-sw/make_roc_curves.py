#!/usr/bin/env python3

"""
Simple script to draw roc curves
"""

import argparse
import numpy as np
from matplotlib import pyplot as plt
import h5py
import os

# grab the same histogram bounds we used in the plotting scripts
from make_hists import BOUNDS

def get_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input_file')
    parser.add_argument('-o','--output-dir', default='plots')
    parser.add_argument('-n', '--nn', nargs=2,
                        help='nn architecture and weights')
    return parser.parse_args()

def run():
    args = get_args()

    # get the jets out of the input file.
    with h5py.File(args.input_file, 'r') as infile:
        jets = np.asarray(infile['jets'])

    make_roc(jets, args.output_dir, args.nn)

def make_roc(jets, output_dir, nn):

    # Read in the jet labels.
    labels = jets['HadronConeExclExtendedTruthLabelID']
    is_b_jet = (labels == 5)
    is_ligth_jet = (labels == 0)
    is_c_jet = (labels == 4)
    masks = [(is_b_jet, 'b'), (is_c_jet, 'c'), (is_ligth_jet, 'light')]

    # make an axis to draw some distributinos
    ax = plt.subplot(1,1,1)
    for varname, (lowbin, highbin) in BOUNDS.items():

        # this is a bunch of silly logic, but basically we compute
        # the NN score if we were given a network
        if varname == 'nn':
            if nn:
                from train_nn import get_discrim
                var = get_discrim(jets, *nn)
            else:
                continue
        # otherwise we just take the variable out of jets
        else:
            var = jets[varname]

        # set NaN values to small value (should only show up in rnnip_ratio)
        var[np.isnan(var)] = -9
        # define the bin bounds (note that we want overflow, thus the inf)
        edges = np.concatenate(
            [[-np.inf], np.linspace(lowbin, highbin, 500), [np.inf]])

        # now loop over signal and background to calculate efficiencies
        # for each working point
        all_eff = {}
        for mask, name in masks:
            discrim, _ = np.histogram(var[mask], bins=edges)
            # normalize the yields
            eff = np.cumsum(discrim[::-1])[::-1] / discrim.sum()
            all_eff[name] = eff

        # calculate efficinecy and background rejection
        light_eff = all_eff['light']
        # we don't want to divide by zero, so filter out points with
        # no background
        valid_rej = light_eff > 0
        valid_eff = (all_eff['b'] > 0.3) & (all_eff['b'] != 1.0)
        valid = valid_rej & valid_eff

        b_eff = all_eff['b'][valid]
        light_rejection = 1/light_eff[valid]

        # draw the plot
        ax.plot(b_eff, light_rejection, label=varname)

    # put things on the plot
    ax.set_yscale('log')
    ax.set_xlabel('b efficiency')
    ax.set_ylabel('light rejection')
    ax.legend()

    # make the output directory
    if not os.path.isdir(output_dir):
        os.mkdir(output_dir)

    plt.savefig('{}/roc.pdf'.format(output_dir))
    plt.close()


if __name__ == '__main__':
    run()
