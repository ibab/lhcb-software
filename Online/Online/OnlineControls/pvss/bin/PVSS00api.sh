#!/bin/bash
installation=/group/online/dataflow/cmtuser/Gaudi_v19r2/Online/OnlineControls/cmt
#Tests only: installation=/home/frankm/cmtuser/Gaudi_v19r2/Online/OnlineControls/cmt
cd $installation
echo $0 `dirname $0`
#
. ./setup.vars
PVSS_system=`python <<EOF
args = '$*'.split(' ')
for i in xrange(len(args)):
  if args[i].upper()[:5]=='-PROJ':
    print args[i+1]
    break
EOF`
export PVSS_II=/localdisk/pvss/${PVSS_system}/config/config
echo "PVSS_II : $PVSS_II "
export LD_PRELOAD=${PYTHONHOME}/lib/libpython2.4.so
exec -a $2 $PVSS00api $*
