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

rucio get mc16_13TeV:mc16_13TeV.301324.Pythia8EvtGen_A14NNPDF23LO_zprime750_tt.deriv.DAOD_FTAG5.e4061_s3126_r9364_p3652/
