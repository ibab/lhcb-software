#!/bin/sh
export DIM_DNS_NODE=`hostname -s`;
export UTGID=CollRDCheckpoint;
export LD_PRELOAD=libCheckpointing.so;
GaudiCheckpoint.exe libGaudiKernel.so GaudiMain $* 2>&1
