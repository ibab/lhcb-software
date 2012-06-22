#!/bin/sh
export DIM_DNS_NODE=blabla.cern.ch;
export UTGID=CollRDCheckpoint;
export LD_PRELOAD=${CHECKPOINTINGROOT}/${CMTCONFIG}/libCheckpointing.so;
`which GaudiCheckpoint.exe` libGaudiKernel.so GaudiMain $* 2>&1
