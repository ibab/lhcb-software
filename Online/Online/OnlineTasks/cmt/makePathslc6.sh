#!/bin/bash
#  create the pathsetup and pathsetup_dbg files in ../job

/cvmfs/lhcb.cern.ch/lib/LbLogin.sh
export CMTOPT=x86_64-slc6-gcc46-opt
echo "Create pathsetupslc6 using config '$CMTOPT'"
runpy LbConfiguration.SetupProject --shell=sh --output=../job/pathsetupslc6 Online v5r6

export CMTDEB=x86_64-slc6-gcc46-dbg
echo "Create pathsetupslc6_dbg using config '$CMTDEB'"
export CMTCONFIG=$CMTDEB
runpy LbConfiguration.SetupProject --shell=sh --output=../job/pathsetupslc6_dbg Online v5r6

export CMTCONFIG=$CMTOPT

