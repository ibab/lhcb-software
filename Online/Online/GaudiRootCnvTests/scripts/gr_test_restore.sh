#!/bin/sh
export DIM_DNS_NODE=`hostname -s`;
unset LD_PRELOAD;
unset LD_LIBRARY_PATH;
restore.exe $* 2>&1;
