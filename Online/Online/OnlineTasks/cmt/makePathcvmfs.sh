#!/bin/bash
#  create the pathsetup and pathsetup_dbg files in ../job

/cvmfs/lhcb.cern.ch/lib/LbLogin.sh
export CMTOPT=x86_64-slc5-gcc46-opt
echo "Create pathsetupslc5 using config '$CMTOPT'"
runpy LbConfiguration.SetupProject --shell=sh --output=../job/pathsetupslc5 Online v5r3

export CMTDEB=x86_64-slc5-gcc46-dbg
echo "Create pathsetupslc5_dbg using config '$CMTDEB'"
export CMTCONFIG=$CMTDEB
runpy LbConfiguration.SetupProject --shell=sh --output=../job/pathsetupslc5_dbg Online v5r3

export CMTCONFIG=$CMTOPT

