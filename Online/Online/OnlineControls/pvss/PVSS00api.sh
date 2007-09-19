#!/bin/bash
export HOME=/home/frankm
echo "Home is: $HOME"
. ./PVSS.setup
PVSS_system=`python <<EOF
args = '$*'.split(' ')
for i in xrange(len(args)):
  if args[i]=='-PROJ':
    print args[i+1]
    break
EOF`
export PVSS_II=/localdisk/pvss/${PVSS_system}/config/config
echo "PVSS_II : $PVSS_II "
export LD_PRELOAD=${PYTHONHOME}/lib/libpython2.4.so
exec -a $2 $PVSS00api $*
###exec -a $2 `which python` $installation/../python/StorageCluster.py
