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

    # read all the required information from a file
    with File(args.input_file, 'r') as input_file:

        # this is a slight optimization: read all the variables from
        # the hdf5 file and into numpy at once. This avoids doing
        # multiple reads from the file.
        kin_vars = ['pt','eta','phi','m']
        jets = input_file['jet'][tuple(kin_vars) + ('btag',)]

        # now we split the jets into components and build a vector
        # array. This seems a bit clunky.
        #
        # See https://github.com/scikit-hep/uproot-methods/issues/41
        pt, eta, phi, m = [jets[x] for x in kin_vars]
        flat = TLorentzVector.TLorentzVectorArray.from_ptetaphim(
            pt, eta, phi, m)

        # we can add arbitrary information to the vector array
        flat['btag'] = jets['btag']

        # finally we need to read out the indices corresponding to the
        # event bountries
        counts = input_file['event']['nJets']

    # Now print out all the jets. The zip here is a bit of a hack
    # because iterations don't return an object with the assocaited
    # table.
    #
    # See https://github.com/scikit-hep/uproot-methods/issues/40
    #
    for tlv, btag in zip(flat, flat.content['btag']):
        print(jetstring(tlv, btag))

    # define a mixin class that combines JaggedArray with
    # LorentzVector arrays
    JaggedTLorentzVectorArray = awkward.Methods.mixin(
        TLorentzVector.ArrayMethods, awkward.JaggedArray)

    # finally build the array
    jagged = JaggedTLorentzVectorArray.fromcounts(counts, flat)

    # and loop over by event number
    for evtnum, event in enumerate(jagged):
        print(f'event number {evtnum}')
        for jet, btag in zip(event, event.content['btag']):
            print(jetstring(jet, btag))


if __name__ == '__main__':
    run()
