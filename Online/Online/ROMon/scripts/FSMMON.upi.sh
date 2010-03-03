#!/bin/bash
. /group/online/dataflow/scripts/pvss_preamble.sh $*
cd $RELEASE_DIR/Online/OnlineControls/cmt
. setup.vars
mkdir -p /tmp/MONITORING/${CMTCONFIG}
cp $RELEASE_DIR/Online/UPIR/${CMTCONFIG}/libUPIR.so /tmp/MONITORING/${CMTCONFIG}/libUPI.so
export PVSS_II=/localdisk/pvss/${1}/config/config
export LD_LIBRARY_PATH=/tmp/MONITORING/${CMTCONFIG}:$LD_LIBRARY_PATH
export PYTHONPATH=$RELEASE_DIR/InstallArea/python:${PYTHONPATH}
#
# Clean runtime environment
#
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh DATAINTERFACE PYTHONPATH PYTHONHOME ROOTSYS PVSS_II PVSS00api UTGID
PVSSMGR_Num=0
eval `python <<EOF
args = '$*'.split(' ')
utgid=None
num='0'
for i in xrange(len(args)):
  if args[i].upper()[:5]=='-PROJ':
    print 'PVSS_system='+str(args[i+1])+';'
  elif args[i].upper()[:4]=='-NUM':
    print 'PVSSMGR_Num='+str(args[i+1])+';'
EOF`
export TAN_NODE=${DATAINTERFACE}
export TAN_PORT=YES
export LD_PRELOAD=${PYTHONHOME}/lib/libpython2.4.so
PVSSName=`basename $0`
export UTGID=FSMMON_0
#
echo exec -a ${UTGID} ${PVSS00api} $* \
    -NAME ${PVSSName}               \
    -DLL PVSSInterface           \
    -FUN pvss_pymain             \
    -SCRIPT ../python/Online/Streaming/UPIDisplay.py
exec -a ${UTGID} ${PVSS00api} $* \
    -NAME ${PVSSName}               \
    -DLL PVSSInterface           \
    -FUN pvss_pymain             \
    -SCRIPT ../python/Online/Streaming/UPIDisplay.py
