#!/usr/bin/env python3

"""
Simple script to draw some distributions.
"""

import argparse
import numpy as np
from matplotlib import pyplot as plt
import h5py
import os

def get_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input_file')
    parser.add_argument('-o','--output-dir', default='plots')
    return parser.parse_args()

# Start by defining the bounds of the histograms
BOUNDS = {
    'rnnip_log_ratio': (-10, 15),
    'jf_sig': (0, 40)
}


def run():
    args = get_args()

    # get the jets out of the input file. We convert them to a numpy
    # array right away because we're not worried about efficiency. If
    # we were doing something that took more memory we'd want to read
    # out the array in slices.
    with h5py.File(args.input_file, 'r') as infile:
        jets = np.asarray(infile['jets'])

    # Read in the jet labels. Right now we're only worried about three
    # classes: the b-quarks, charm-quarks, and the light quarks.  for
    # now we ignore ones with double labels (i.e. 44, 45, 55).
    labels = jets['HadronConeExclExtendedTruthLabelID']
    is_b_jet = (labels == 5)
    is_ligth_jet = (labels == 0)
    is_c_jet = (labels == 4)
    masks = [(is_b_jet, 'b'), (is_c_jet, 'c'), (is_ligth_jet, 'light')]
    colors = {'b':'red', 'c': 'green', 'light': 'blue'}

    # Next we plot a few distributions, for light and b-jets
    #
    bounds = BOUNDS
    # the histograms will be stored in this dictionary
    for varname, (lowbin, highbin) in bounds.items():
        var = jets[varname]
        # set NaN values to small value (should only show up in rnnip_ratio)
        var[np.isnan(var)] = -9
        # make an axis to draw some distributinos
        ax = plt.subplot(1,1,1)
        # define the bin bounds (note that we want overflow, thus the inf)
        edges = np.concatenate(
            [[-np.inf], np.linspace(lowbin, highbin, 20), [np.inf]])
        centers = (edges[1:-2] + edges[2:-1]) / 2
        # now loop over signal and background
        for mask, name in masks:
            yields, _ = np.histogram(var[mask], bins=edges)
            # normalize the yields
            yields = yields / yields.sum()
            ax.step(centers, yields[1:-1], label=name, where='mid',
                    color=colors[name])

        ax.set_yscale('log')
        ax.set_xlim((centers[0], centers[-1]))
        ax.set_xlabel(varname)
        ax.legend()
        # make the output directory
        if not os.path.isdir(args.output_dir):
            os.mkdir(args.output_dir)

        plt.savefig('{}/{}.pdf'.format(args.output_dir, varname))
        plt.close()

    # Let's make another histogram, just for fun, that shows the
    # correlation between our variables.
    #
    # This one will show (b, c, light) as rgb channels in a 2d plot
    channels = []
    for mask, name in masks:
        ax_names = ['rnnip_log_ratio', 'jf_sig']
        points = np.stack([jets[x][mask] for x in ax_names], axis=1)
        edges = [np.linspace(*bounds[v], 20) for v in ax_names]
        # we need to transform the counts to fit them in 0--1 range and
        # make everything visible
        hist = np.log1p(np.histogramdd(points, edges)[0])
        channels.append(hist.T / hist.max())
    merged = np.stack(channels,axis=2)
    ax = plt.subplot(1,1,1)
    ax.imshow(merged, origin='lower', aspect='auto',
               extent=(*bounds['rnnip_log_ratio'], *bounds['jf_sig']))
    ax.set_xlabel('rnnip ratio')
    ax.set_ylabel('JetFitter Sig')
    plt.savefig('{}/{}.pdf'.format(args.output_dir, '2d'))

if __name__ == '__main__':
    run()
