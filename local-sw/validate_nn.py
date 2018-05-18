#!/usr/bin/env python3

"""
Simple script verify that Atlas-side NN is working
"""

import argparse
import numpy as np
import h5py
from train_nn import get_discrim

def get_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input_file')
    parser.add_argument('-n', '--nn', nargs=2, required=True,
                        help='nn architecture and weights')
    return parser.parse_args()

def run():
    args = get_args()

    # get the jets out of the input file. We only care about the first
    # ten thousand: if things match there they probably match everywhere.
    with h5py.File(args.input_file, 'r') as infile:
        jets = np.asarray(infile['jets'][0:10000])

    # calculate the discriminant with the local network
    local = get_discrim(jets, *args.nn)

    # now calculate it based on the upstream scores that we wrote
    upstream = np.log(jets['nn_bottom'] / jets['nn_light'])

    # now see how close they are
    close = np.isclose(local, upstream)
    diffs = local[~close] - upstream[~close]
    print('most significant differences: {}'.format(diffs))

if __name__ == '__main__':
    run()
