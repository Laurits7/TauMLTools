#!/bin/bash

# CMSSW area should be activated prior to running this script and the
# /cvmfs/cms.cern.ch/cmsset_default.sh should be sourced
#
# To run the scipt:
#   bash generate_ntuples.sh $SAMPLE $OUTPUT_DIR $NUM_EVENTS $SEED
# e.g
#   bash generate_ntuples.sh TTbar_14TeV_TuneCP5_cfi /afs/cern.ch/work/l/ltani/ 10 1

set -e  # If the return code of one command is not 0 and the caller does not check it, the shell script will exit.
set -x  # prints shell command before executing it

SAMPLE=$1  # For which sample the ntuples are generated
OUTPUT_DIR=$2  # Directory where the ntuples and config files are saved
NUM_EVENTS=$3  # Number of events to be analyzed
SEED=$4  # Random seed needed for runTheMatrix.py, each workflow needs to have different

cd $OUTPUT_DIR

# Command for generating configuration file the MC
cmsDriver.py $SAMPLE \
  --conditions auto:phase2_realistic_T15 \
  -n $NUM_EVENTS \
  --era Run3 \
  --eventcontent FEVTDEBUGHLT \
  -s GEN,SIM,DIGI,L1,DIGI2RAW,HLT \
  --datatier GEN-SIM \
  --geometry DB:Extended \
  --no_exec \
  --fileout step2_phase1_new.root \
  --customise Validation/RecoParticleFlow/customize_pfanalysis.customize_step2 \
  --python_filename=step2_phase1_new.py

# customize osa defineerida endal python/customize_steps.py failis

#Run the reco sequences
cmsDriver.py step3 \
  --conditions auto:phase1_2021_realistic \
  --era Run3 \
  -n -1 \
  --eventcontent FEVTDEBUGHLT \
  -s RAW2DIGI,L1Reco,RECO,RECOSIM \
  --datatier GEN-SIM-RECO \
  --geometry DB:Extended \
  --no_exec \
  --filein file:step2_phase1_new.root \
  --fileout step3_phase1_new.root \
  --customise Validation/RecoParticleFlow/customize_pfanalysis.customize_step3 \
  --python_filename=step3_phase1_new.py


echo "process.RandomNumberGeneratorService.generator.initialSeed = $SEED" >> step2_phase1_new.py
# cmsRun step2_phase1_new.py
# cmsRun step3_phase1_new.py
# cmsRun $PACKAGE_BASE/config/ntuple_config.py