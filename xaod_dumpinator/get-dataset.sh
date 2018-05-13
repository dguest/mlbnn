#!/usr/bin/env bash

if [[ $- == *i* ]] ; then
    echo "Don't source me!" >&2
    return 1
else
    set -eu
fi

mkdir -p data
cd data
if ! type rucio; then
    echo "Please set up rucio! Quitting" >&2
    exit 1
fi

rucio get --nrandom 1 mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_FTAG2.e6337_e5984_s3126_r9781_r9778_p3415/
