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

# SAMPLE=$1  # For which sample the ntuples are generated
# OUTPUT_DIR=$2  # Directory where the ntuples and config files are saved
# NUM_EVENTS=$3  # Number of events to be analyzed
# SEED=$4  # Random seed needed for runTheMatrix.py, each workflow needs to have different

SEED=1

cmsDriver.py TTbar_14TeV_TuneCP5_cfi \
    --conditions auto:phase2_realistic_T15 \
    -n 10 \
    --era Phase2C9 \
    --eventcontent FEVTDEBUGHLT \
    --relval 9000,100 \
    -s GEN,SIM,DIGI,L1,DIGI2RAW,HLT \
    --datatier GEN-SIM \
    --beamspot HLLHC14TeV \
    --geometry Extended2026D49 \
    --no_exec \
    --customise TauMLTools/Production/customize_pfanalysis.customize_step2 \
    --python_filename=step2_phase1_new.py \
    --fileout step2_phase1_new.root 


cmsDriver.py step3 \
    --conditions auto:phase2_realistic_T15 \
    -n 10 \
    --era Phase2C9 \
    --eventcontent FEVTDEBUGHLT \
    -s RAW2DIGI,L1Reco,RECO,RECOSIM \
    --datatier GEN-SIM-RECO \
    --geometry Extended2026D49 \
    --no_exec \
    --filein file:step2_phase1_new.root \
    --fileout step3_phase1_new.root \
    --customise TauMLTools/Production/customize_pfanalysis.customize_step3 \
    --python_filename=step3_phase1_new.py
    # --pileup AVE_200_BX_25ns \


echo "process.RandomNumberGeneratorService.generator.initialSeed = $SEED" >> step2_phase1_new.py
cmsRun step2_phase1_new.py
cmsRun step3_phase1_new.py
# cmsRun $PACKAGE_BASE/config/ntuple_config.py


# PILEUP for step3 could be different