#!/bin/bash
export UTGID
. /group/online/dataflow/cmtuser/Online_v4r5/MyOnline/cmt/setup.vars
#. /home/frankm/cmtuser/Online_v4r5/MyOnline/cmt/setup.vars

TASKCLASS=-tasktype=LHCb::${2}Task
TASKTYPE=${3}
export ONLINETASKS=/group/online/dataflow/templates
export PREAMBLE_OPTS=$ONLINETASKS/options/Preamble.opts
DYNAMIC_OPTS=/group/online/dataflow/options
OPTIONS=${DYNAMIC_OPTS}/${PARTITION}/${1}

export DATAINTERFACE=`python -c "import socket;print socket.gethostbyname(socket.gethostname().split('.')[0]+'-d1')"`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
# DATAINTERFACE

export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=LHCb::FmcMessageSvc "
DIMDNS=${4}
DIM_DNS_MODIFIED=NO
if test -n "${DIMDNS}"
then
  DIM_DNS_MODIFIED=YES
  export DIM_DNS_NODE=${DIMDNS};
fi;

unset LS_COLORS
unset XERCESCROOT
unset SEALROOT
unset UUIDROOT
unset ROOTROOT
unset POOLROOT
unset REFLEXROOT
unset RELAXROOT
unset ORACLEROOT
unset PCREROOT
unset CASTORROOT
unset AIDAROOT
unset CLHEPROOT
unset CLHEPROOT
unset ORACLEROOT
unset SEAL_PLUGINS
unset BINTYPE
unset GCCXMLROOT
unset GAUDIALGROOT
unset GAUDIPOLICYROOT
unset GAUDIAUDROOT
unset GAUDIUPIROOT
unset GAUDIPOOLDBROOT
unset MBMDUMPROOT
unset LHCBMATHROOT
unset DIMROOT

unset MINITERM
unset BIGTERM
unset WIDETERM
unset CMTPATH
unset CLASSPATH
unset CERN
unset CERN_ROOT
unset CERN_LEVEL
unset CMTROOT
unset CASTOR_HOME

unset GAUDIEXE
unset OLDPWD
unset GROUP_DIR

unset HSM_HOME
unset HPX_CELL
unset HPX_OS_MAJOR
unset HPX_OS_MINOR
unset HPX_HEPGROUP_DIR
unset HPX_INITIALE
unset _HPX_SEEN_SITE_ENV
unset HPSS_HOME

unset PYTHONPATH
unset QTDIR
unset KDEDIR
unset VISUAL
unset WRITERROOT
unset BOOSTROOT
unset MDFROOT
unset GAUCHOROOT
unset PAGER
unset TESTSROOT
unset NEWCMTCONFIG
unset PRINT_CMD
unset GSLROOT
unset XFILESEARCHPATH
unset CLUSTER_DIR
unset WWW_HOME
unset LESSOPEN
unset ONLINEHISTDBROOT
unset EDITOR
unset MANPATH
unset NNTPSERVER
unset SYSTYPE
unset SSH_ASKPASS
unset G_BROKEN_FILENAMES
unset MSGSVC
unset ROOTPATH
unset SHLVL
unset ENV
unset DAQEVENTROOT
unset GAUDIKERNELROOT
unset GAUDISVCROOT
unset INITIALISED
unset USERPATH

echo "+++ Starting task ${UTGID} of class ${TASKCLASS} with DNS:${DIM_DNS_NODE} (Modified:${DIM_DNS_MODIFIED})"
if [ -r ${OPTIONS} ]; then
  unset DIM_DNS_MODIFIED
  $gaudi_exe ${TASKCLASS} -opt=${OPTIONS} -main=$ONLINETASKS/options/Main.opts
  #exec -a ${UTGID} $gaudi_exe ${TASKCLASS} -opt=${OPTIONS} -main=$ONLINETASKS/options/Main.opts
  exit
fi
echo "The options file ${OPTIONS} does not exist!"
