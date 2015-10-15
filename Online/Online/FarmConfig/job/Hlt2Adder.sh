#!/bin/bash
# =========================================================================
#
#  Script to start the HLT2 histogram adder on the farm
#
#  Author   R. Aaij
#  Version: 1.0
#  Date:    16/07/2015
#
# =========================================================================
#
export TAN_PORT=YES;
export TAN_NODE=${DATAINTERFACE};
export CMTCONFIG=x86_64-slc6-gcc48-opt;
read COMMAND <<EOF
import GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=3; from Hlt2Monitoring import Hlt2Adder; Hlt2Adder.configure()
EOF

exec -a ${UTGID} ${Class1_task} -opt=command="${COMMAND}";
