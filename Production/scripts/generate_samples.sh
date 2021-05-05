#!/bin/bash

# CMSSW area should be activated prior to running this script and the
# /cvmfs/cms.cern.ch/cmsset_default.sh should be sourced
#
# To run the scipt:
#   bash generate_ntuples.sh $SAMPLE $OUTPUT_DIR $NUM_EVENTS $SEED
# e.g
#   bash generate_samples.sh TTbar_14TeV_TuneCP5_cfi /afs/cern.ch/work/l/ltani/ 10 1
#
# cmsRun Production/python/Production.py sampleType=MC_Phase2_111X inputFiles=file:///afs/cern.ch/work/l/ltani/TTTo2L2Nu-noPU-run4.root maxEvents=3
set -e  # If the return code of one command is not 0 and the caller does not check it, the shell script will exit.
set -x  # prints shell command before executing it

# SAMPLE=$1  # For which sample the ntuples are generated
# OUTPUT_DIR=$2  # Directory where the ntuples and config files are saved
# NUM_EVENTS=$3  # Number of events to be analyzed
# SEED=$4  # Random seed needed for runTheMatrix.py, each workflow needs to have different

SEED=1

#!/bin/bash
set -x

# in: /home/joosep/reco/CMSSW_11_3_0_pre6 dryRun for 'cd 23434.0_TTbar_14TeV+2026D49PU+TTbar_14TeV_TuneCP5_GenSimHLBeamSpot14+DigiTriggerPU+RecoGlobalPU+HARVESTGlobalPU
 cmsDriver.py TTbar_14TeV_TuneCP5_cfi  \
 --conditions auto:phase2_realistic_T15 \
 -n 3 \
 --era Phase2C9 \
 --eventcontent FEVTDEBUG \
 --relval 9000,100 -s GEN,SIM \
 --datatier GEN-SIM \
 --beamspot HLLHC14TeV \
 --geometry Extended2026D49 \
 --fileout file:step1.root 

 # in: /home/joosep/reco/CMSSW_11_3_0_pre6 dryRun for 'cd 23434.0_TTbar_14TeV+2026D49PU+TTbar_14TeV_TuneCP5_GenSimHLBeamSpot14+DigiTriggerPU+RecoGlobalPU+HARVESTGlobalPU
  cmsDriver.py step2  \
  --conditions auto:phase2_realistic_T15 \
  --pileup_input filelist:pu_files.txt \
  --customise TauMLTools/Production/customize_pfanalysis.customize_step2 \
  -n 3 \
  --era Phase2C9 \
  --eventcontent FEVTDEBUGHLT \
  -s DIGI:pdigi_valid,L1TrackTrigger,L1,DIGI2RAW,HLT:@fake2 \
  --datatier GEN-SIM-DIGI-RAW \
  --pileup AVE_200_BX_25ns \
  --geometry Extended2026D49 \
  --filein  file:step1.root  \
  --fileout file:step2.root

  # in: /home/joosep/reco/CMSSW_11_3_0_pre6 dryRun for 'cd 23434.0_TTbar_14TeV+2026D49PU+TTbar_14TeV_TuneCP5_GenSimHLBeamSpot14+DigiTriggerPU+RecoGlobalPU+HARVESTGlobalPU
   cmsDriver.py step3  \
   --conditions auto:phase2_realistic_T15 \
   --pileup_input filelist:pu_files.txt \
   -n 3 \
   --customise TauMLTools/Production/customize_pfanalysis.customize_step3 \
   --era Phase2C9 \
   --eventcontent FEVTDEBUGHLT,MINIAODSIM,DQM \
   -s RAW2DIGI,L1Reco,RECO,RECOSIM,PAT,VALIDATION:@phase2Validation+@miniAODValidation,DQM:@phase2+@miniAODDQM \
   --datatier GEN-SIM-RECO,MINIAODSIM,DQMIO \
   --pileup AVE_200_BX_25ns \
   --geometry Extended2026D49 \
   --filein  file:step2.root  \
   --fileout file:step3.root