#!/bin/bash

# Data files
export ROOTDIR=/r03/lhcb/jonesc/ANNPID/ProtoParticlePIDtuples/Test
export SIGNALFILE=${ROOTDIR}"/2012-S20-ANNPID-BHADRONCOMPLETEEVENT.root:ANNPID/DecayTree"
export BCKGRDFILE=${ROOTDIR}"/Incb-MC2012-S20-ANNPID-Pythia8.root:ANNPID/DecayTree"

# Run the teacher
TMVATeacher.exe --TrainingParameters TMVATraining.txt --Inputs TMVAInputs.txt --SignalTrainingFile $SIGNALFILE --BckgTrainingFile $BCKGRDFILE 2>&1 | tee TMVATrain.log

exit 0
