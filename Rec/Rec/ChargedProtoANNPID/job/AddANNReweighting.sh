#!/bin/bash

export ROOTDIR="/r03/lhcb/jonesc/ANNPID/ProtoParticlePIDtuples/Test/V1/StripFiltered/"

export WORKDIR="output-100k/weights"
export MVADIR=${ROOTDIR}"WithMVAs/V1/"

export FILE="2015-S23r1-ANNPID-BHADRONCOMPLETEEVENT.root"
TMVAReader.exe --TreeName ANNPID/DecayTree --InRootFiles ${ROOTDIR}${FILE} --OutRootFile ${MVADIR}${FILE} --WorkDir ${WORKDIR} --Inputs TMVAInputs.txt

export FILE="Incb-DevMCJun2015-S23r1-ANNPID-Pythia8.root"
TMVAReader.exe --TreeName ANNPID/DecayTree --InRootFiles ${ROOTDIR}${FILE} --OutRootFile ${MVADIR}${FILE} --WorkDir ${WORKDIR} --Inputs TMVAInputs.txt

exit 0
