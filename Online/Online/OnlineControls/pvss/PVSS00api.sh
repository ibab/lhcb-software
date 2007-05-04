#!/bin/bash
export HOME=/home/frankm
echo "Home is: $HOME"
. ./PVSS.setup
export LD_PRELOAD=${PYTHONHOME}/lib/libpython2.4.so
#echo $*
exec -a $2 $PVSS00api $*
