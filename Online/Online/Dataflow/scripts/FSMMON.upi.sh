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
. ${DATAFLOWDIR}/scripts/cleanEnviron.sh DATAINTERFACE PYTHONPATH PYTHONHOME ROOTSYS PVSS_II PVSS00api
export UTGID=FSMMON_0
export TAN_NODE=${DATAINTERFACE}
export TAN_PORT=YES
export LD_PRELOAD=${PYTHONHOME}/lib/libpython2.4.so
#
echo exec -a ${UTGID} ${PVSS00api} $* \
    -NAME ${UTGID}               \
    -DLL PVSSInterface           \
    -FUN pvss_pymain             \
    -SCRIPT ../python/Online/Streaming/UPIDisplay.py
exec -a ${UTGID} ${PVSS00api} $* \
    -NAME ${UTGID}               \
    -DLL PVSSInterface           \
    -FUN pvss_pymain             \
    -SCRIPT ../python/Online/Streaming/UPIDisplay.py
