#!/bin/bash

export INFILE=/r03/lhcb/jonesc/ANNPID/ProtoParticlePIDtuples/Test/V1/StripFiltered/2015-S23r1-ANNPID-BHADRONCOMPLETEEVENT.root
export OUTFILE=/r03/lhcb/jonesc/ANNPID/ProtoParticlePIDtuples/Test/V1/StripFiltered/WithMVAs/V1/2015-S23r1-ANNPID-BHADRONCOMPLETEEVENT.root

TMVAReader.exe --TreeName ANNPID/DecayTree --InRootFiles ${INFILE} --OutRootFile ${OUTFILE} --WorkDir output-100k/weights --Inputs TMVAInputs.txt

exit 0
