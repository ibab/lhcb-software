#!/bin/bash
. /group/online/dataflow/scripts/pvss_preamble.sh $*
cd $RELEASE_DIR/Online/OnlineControls/cmt
. setup.slc4_ia32_gcc34.vars
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
export PVSS_II=/localdisk/pvss/${PVSS_system}/config/config
echo exec -a ${2} ${PVSS00api} $*
exec ${PVSS00api} $*
