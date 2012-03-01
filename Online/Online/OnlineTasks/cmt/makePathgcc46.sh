#!/bin/bash
#  create the pathsetup and pathsetup_dbg files in ../job

#setup online environment
export MYSITEROOT=/sw/lib
export LHCBHOME=/sw/lib
. $MYSITEROOT/scripts/ExtCMT.sh
export CMTPROJECTPATH=$LHCBPROJECTPATH
export CMTOPT=x86_64-slc5-gcc46-opt
echo "Create pathsetupslc5 using config '$CMTOPT'"
export CMTCONFIG=$CMTOPT
. ./setup.sh
${CMTROOT}/mgr/cmt setup -sh > ../job/pathsetupslc5

export CMTDEB=x86_64-slc5-gcc46-dbg
echo "Create pathsetupslc5_dbg using config '$CMTDEB'"
export CMTCONFIG=$CMTDEB
. ./setup.sh
${CMTROOT}/mgr/cmt setup -sh > ../job/pathsetupslc5_dbg

export CMTCONFIG=$CMTOPT

