#!/usr/bin/env bash
#
# Example grid submit script
#
# Short example to submit the dumpxAOD job to the grid. Uses prun to
# create a tarball, then loops over a list of datasets and submits one
# job for each.

# This script should not be sourced, we don't need anything in here to
# propigate to the surrounding environment.
if [[ $- == *i* ]] ; then
    echo "Don't source me!" >&2
    return 1
else
    # set the shell to exit if there's an error (-e), and to error if
    # there's an unset variable (-u)
    set -eu
fi


##########################
# Real things start here #
##########################

###################################################
# Part 1: variables you you _might_ need to change
###################################################
#
# Users's grid name
GRID_NAME=${RUCIO_ACCOUNT-${USER}}
#
# This job's tag (the current expression is something random)
BATCH_TAG=$(date +%F-T%H%M%S)-R${RANDOM}


######################################################
# Part 2: variables you probably don't have to change
######################################################
#
# Build a zip of the files we're going to submit
ZIP=job.tgz
#
# This is the subdirectory we submit from
SUBMIT_DIR=submit


###################################################
# Part 3: prep the submit area
###################################################
#
echo "preping submit area"
if [[ -d ${SUBMIT_DIR} ]]; then
    echo "removing old submit directory"
    rm -rf ${SUBMIT_DIR}
fi
mkdir ${SUBMIT_DIR}
cd ${SUBMIT_DIR}


###########################################
# Part 4: build a tarball of the job
###########################################
#
echo "making tarball of local files: ${ZIP}" >&2
#
# The --outTarBall, --noSubmit, and --useAthenaPackages arguments are
# important. The --outDS and --exec don't matter at all here, they are
# just placeholders to keep panda from complianing.
prun --outTarBall=${ZIP} --noSubmit --useAthenaPackages\
     --outDS user.${GRID_NAME}.x --exec "ls"


##########################################
# Part 5: loop over datasets and submit
##########################################
#
# Get a list of input datasets
INPUT_DATASETS=(
    mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_FTAG2.e6337_e5984_s3126_r9781_r9778_p3415/
)
#
# Loop over all inputs
for DS in ${INPUT_DATASETS[*]}
do
    # This regex extracts the DSID from the input dataset name, so
    # that we can give the output dataset a unique name. It's not
    # pretty: ideally we'd just suffix our input dataset name with
    # another tag. But thanks to insanely long job options names we
    # use in the generation stage we're running out of space for
    # everything else.
    DSID=$(sed -r 's/[^\.]*\.([0-9]{6,8})\..*/\1/' <<< ${DS})
    #
    # Build the full output dataset name
    OUT_DS=user.${GRID_NAME}.${DSID}.mlbnn.${BATCH_TAG}
    #
    # Now submit. The script we're running expects one argument per
    # input dataset, whereas %IN gives us comma separated files, so we
    # have to run it through `tr`.
    echo "Submitting for ${GRID_NAME} on ${DS} -> ${OUT_DS}"
    prun --exec "dump-xaod $(echo %IN | tr ',' ' ')"\
         --outDS ${OUT_DS} --inDS ${DS}\
         --outputs output.h5 --inTarBall=${ZIP}\
         --noEmail > ${OUT_DS}.log 2>&1
done

