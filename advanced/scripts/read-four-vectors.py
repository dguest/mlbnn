#!/usr/bin/env python3

"""
Read in some 4 vectors (stored in HDF5), and build TLorentzVectors
"""

import numpy
from uproot_methods.classes import TLorentzVector
import awkward
import argparse
from h5py import File


def get_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input_file')
    return parser.parse_args()

def jetstring(jet):
    s = 'pt: {:5.1f} GeV, eta: {: .1f}, phi: {: .2f}, m: {:4.1f} GeV'.format(
        jet.pt/1000, jet.eta, jet.phi, jet.mass/1000)
    return s

def run():
    args = get_args()
    with File(args.input_file, 'r') as input_file:
        jets = input_file['jet']
        pt, eta, phi, m = [jets[x] for x in ['pt','eta','phi','m']]
        flat = TLorentzVector.TLorentzVectorArray.from_ptetaphim(
            pt, eta, phi, m)
        counts = input_file['event']['nJets']
    for tlv in flat:
        print(jetstring(tlv))

    JaggedTLorentzVectorArray = awkward.Methods.mixin(
        TLorentzVector.ArrayMethods, awkward.JaggedArray)

    jagged = JaggedTLorentzVectorArray.fromcounts(counts, flat)
    for evtnum, event in enumerate(jagged):
        print(f'event number {evtnum}')
        for jet in event:
            print(jetstring(jet))


if __name__ == '__main__':
    run()
