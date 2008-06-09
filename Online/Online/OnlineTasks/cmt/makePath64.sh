#!/bin/bash
#  create the pathsetup and pathsetup_dbg files in ../job

#setup online environment
export MYSITEROOT=/sw/lib
export LHCBHOME=/sw/lib
. $MYSITEROOT/scripts/ExtCMT.sh
export CMTPROJECTPATH=$LHCBPROJECTPATH

echo "Create pathsetup using config '$CMTOPT'"
export CMTCONFIG=$CMTOPT
. ./setup.sh
${CMTROOT}/mgr/cmt setup -sh > ../job/pathsetup64

echo "Create pathsetup_dbg using config '$CMTDEB'"
export CMTCONFIG=$CMTDEB
. ./setup.sh
${CMTROOT}/mgr/cmt setup -sh > ../job/pathsetup64_dbg

export CMTCONFIG=$CMTOPT

