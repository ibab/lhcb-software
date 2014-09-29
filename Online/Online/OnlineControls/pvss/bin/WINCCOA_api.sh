#!/bin/bash
#. /group/online/dataflow/scripts/pvss_preamble.sh $*
export RELEASEDIR=/group/online/dataflow/cmtuser/WINCC_OA_dev;
cd ${RELEASEDIR}/Online/OnlineControls/cmt;
#
# Clean runtime environment
#
#. ${DATAFLOWDIR}/scripts/cleanEnviron.sh LD_PRELOAD DATAINTERFACE PYTHONPATH PYTHONHOME ROOTSYS PVSS_II PVSS00api UTGID
#
PVSSMGR_Num=0
eval `python <<EOF
args = '$*'.split(' ')
num='0'
for i in xrange(len(args)):
  if args[i].upper()[:5]=='-PROJ':
    print 'PVSS_system='+str(args[i+1])+';'
  elif args[i].upper()[:4]=='-NUM':
    print 'PVSSMGR_Num='+str(args[i+1])+';'
EOF`
# export UTGID=${2}${PVSSMGR_Num}
export PVSS_II=/localdisk/wincc/${PVSS_system}/config/config
. ./WCCOA.setup.${PVSS_system}
#
echo $PVSS_II -- exec -a ${2} ${PVSS00api} $*
exec ${PVSS00api} $*
