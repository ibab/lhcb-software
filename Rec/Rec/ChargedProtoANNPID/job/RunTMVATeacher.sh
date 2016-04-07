#!/bin/bash

# Data files
export ROOTDIR=/r03/lhcb/jonesc/ANNPID/ProtoParticlePIDtuples/Test/V1/StripFiltered

export TRACK="Long"

#export NAME="2015DataMCDiff"
#export SIGNALFILE=${ROOTDIR}"/2015-S23r1-ANNPID-BHADRONCOMPLETEEVENT.root:ANNPID/DecayTree"
#export BCKGRDFILE=${ROOTDIR}"/Incb-DevMCJun2015-S23r1-ANNPID-Pythia8.root:ANNPID/DecayTree"

export NAME="2012DataMCDiff"
export SIGNALFILE=${ROOTDIR}"/2012-S20-ANNPID-BHADRONCOMPLETEEVENT.root:ANNPID/DecayTree"
export BCKGRDFILE=${ROOTDIR}"/Incb-MC2012-S20-ANNPID-Pythia8.root:ANNPID/DecayTree"

# Run the teacher
TMVATeacher.exe --Name ${NAME}"-"${TRACK} --SignalCuts TMVACuts"-"${TRACK}.txt --BckgCuts TMVACuts"-"${TRACK}.txt --TrainingParameters TMVATraining.txt --Inputs TMVAInputs.txt --SignalTrainingFile $SIGNALFILE --BckgTrainingFile $BCKGRDFILE 2>&1 | tee TMVATrain.log

exit 0
