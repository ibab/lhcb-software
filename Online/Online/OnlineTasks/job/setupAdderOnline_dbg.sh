#!/bin/bash
#  Generic script to setup the environment for an Online Gaudi/Gaucho task
#  
# Todo: remove hard-coded DIM_DNS_NODE
#       parametrise setup.sh scripts
#       make GaudiEnv version variable
#modified 20/03/2007 by EvH for tests in B157
#         18/10/2007 for Online v4r0

echo Running as $(/usr/bin/whoami) with DIM_DNS_NODE $DIM_DNS_NODE

. pathsetup_dbg

export gaudi_exe3="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline"  

