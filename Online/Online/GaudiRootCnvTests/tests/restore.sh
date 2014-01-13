#!/bin/sh
export DIM_DNS_NODE=blabla.cern.ch;
unset LD_PRELOAD;
unset LD_LIBRARY_PATH;
restore.exe $* 2>&1;
