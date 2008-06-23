#!/bin/sh -f

export CMTCONFIG=slc4_ia32_gcc34
export MYSITEROOT=/sw/lib
export User_release_area=$HOME/cmtuser
. $MYSITEROOT/scripts/ExtCMT.sh

unset CMTPATH
export CMTPROJECTPATH=$LHCBPROJECTPATH

export DIM_DNS_NODE=ecs01.lbdaq.cern.ch

cmt startServer
