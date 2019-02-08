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

def jetstring(jet, btag):
    s = ('pt: {:5.1f} GeV, eta: {: .1f}, phi: {: .2f}, m: {:4.1f} GeV, '
         'btag score: {:0.2f}').format(
             jet.pt/1000, jet.eta, jet.phi, jet.mass/1000, btag)
    return s

def run():
    args = get_args()
    with File(args.input_file, 'r') as input_file:
        kin_vars = ['pt','eta','phi','m']
        # this is a slight optimization, read all the variables from
        # the hdf5 file and into numpy at once
        jets = input_file['jet'][tuple(kin_vars) + ('btag',)]
        pt, eta, phi, m = [jets[x] for x in kin_vars]
        flat = TLorentzVector.TLorentzVectorArray.from_ptetaphim(
            pt, eta, phi, m)
        flat['btag'] = jets['btag']
        counts = input_file['event']['nJets']
    for tlv, btag in zip(flat, flat.content['btag']):
        print(jetstring(tlv, btag))

    JaggedTLorentzVectorArray = awkward.Methods.mixin(
        TLorentzVector.ArrayMethods, awkward.JaggedArray)

    jagged = JaggedTLorentzVectorArray.fromcounts(counts, flat)
    for evtnum, event in enumerate(jagged):
        print(f'event number {evtnum}')
        for jet, btag in zip(event, event.content['btag']):
            print(jetstring(jet, btag))


if __name__ == '__main__':
    run()
