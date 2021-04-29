#!/bin/bash

# CMSSW area should be activated prior to running this script and the
# /cvmfs/cms.cern.ch/cmsset_default.sh should be sourced
#
# To run the scipt:
#   bash generate_ntuples.sh $SAMPLE $OUTPUT_DIR $NUM_EVENTS $SEED
# e.g
#   bash generate_samples.sh TTbar_14TeV_TuneCP5_cfi /afs/cern.ch/work/l/ltani/ 10 1

set -e  # If the return code of one command is not 0 and the caller does not check it, the shell script will exit.
set -x  # prints shell command before executing it

SAMPLE=$1  # For which sample the ntuples are generated
OUTPUT_DIR=$2  # Directory where the ntuples and config files are saved
NUM_EVENTS=$3  # Number of events to be analyzed
SEED=$4  # Random seed needed for runTheMatrix.py, each workflow needs to have different

cd $OUTPUT_DIR

cmsDriver.py DYToLL_M-50_14TeV_pythia8_cff \
    --conditions auto:phase2_realistic \
    -n 10 \
    --era Phase2C8 \
    --eventcontent FEVTDEBUG \
    --relval 9000,100 \
    -s GEN,SIM,DIGI,L1,DIGI2RAW,HLT \
    --datatier GEN-SIM \
    --beamspot HLLHC14TeV \
    --geometry DB:Extended \
    --fileout file:step2_phase1_new.root \
    --no_exec \
    --customise TauMLTools/Production/customize_pfanalysis.customize_step2 \
    --python_filename=step2_phase1_new.py


cmsDriver.py step3 \
    --conditions auto:phase2_realistic \
    -n 10 \
    --era Phase2C8 \
    --eventcontent FEVTDEBUGHLT,MINIAODSIM,DQM \
    -s RAW2DIGI,L1Reco,RECO,RECOSIM \
    --datatier GEN-SIM-RECO,MINIAODSIM,DQMIO \
    --pileup AVE_200_BX_25ns \
    --geometry DB:Extended \
    --customise TauMLTools/Production/customize_pfanalysis.customize_step3\
    --no_exec \
    --filein file:step2_phase1_new.root \
    --fileout file:step3_phase1_new.root \
    --python_filename=step3_phase1_new.py

echo "process.RandomNumberGeneratorService.generator.initialSeed = 1" >> step2_phase1_new.py
# cmsRun step2_phase1_new.py
# cmsRun step3_phase1_new.py
# cmsRun $PACKAGE_BASE/config/ntuple_config.py