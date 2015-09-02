#!/bin/sh
#killall test.exe
if test ! "${1}" = "NO_KILL";then
    echo "Killing leftovers from previous tests.....";
    killall GaudiOnlineExe;
fi;
#killall gentest.exe
#rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
#
export TAN_PORT=YES;
export TAN_NODE=`hostname -s`;
if test -z "${DIM_DNS_NODE}";
then
    export DIM_DNS_NODE=`hostname -s`;
fi;
#
export OPTS=$GAUDIONLINEROOT/options

export msg_svc=LHCb::FmcMessageSvc;
export msg_svc=MessageSvc;

export test_exe=`which test.exe`;
export gentest_exe=`which gentest.exe`;
export GaudiOnlineExe=`which GaudiOnlineExe.exe`;
#echo "======> $test_exe $gentest_exe $GaudiOnlineExe";
export gaudi_run="${GaudiOnlineExe} libGaudiOnline.so OnlineStart "
export gaudi_exe="${GaudiOnlineExe} libGaudiOnline.so OnlineTask -msgsvc=$msg_svc -auto "
export gaudi_exe2="${GaudiOnlineExe} libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto "
export gaudi_task="${GaudiOnlineExe} libGaudiOnline.so OnlineTask -msgsvc=$msg_svc -auto -main=$OPTS/Main.opts "

export Class0_task="${GaudiOnlineExe} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class0Task -msgsvc=$msg_svc -auto -main=$OPTS/Main.opts "
export Class1_task="${GaudiOnlineExe} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -msgsvc=$msg_svc -auto -main=$OPTS/Main.opts "
export Class2_task="${GaudiOnlineExe} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class2Task -msgsvc=$msg_svc -auto -main=$OPTS/Main.opts "
export MINITERM='xterm  -ls -132 -geometry 132x12 -title ';
export BIGTERM='xterm  -ls -132 -geometry 132x65 -title ';
export WIDETERM='xterm  -ls -132 -geometry 160x50 -title ';
export NORMALTERM='xterm -ls -132 -geometry 132x32 -title ';
export HOST=`hostname -s`;
export NODENAME=`python -c "print '$HOST'.split('.')[0]"`;
export NODENAME=${HOST%%.*};
export GDB="`which gdb`"
export GDB="/home/frankm/bin/debug2"
if test -f /afs/cern.ch/sw/lcg/external/gdb/7.6/x86_64-slc6-gcc48-opt/bin/gdb;
then 
    export GDB=/afs/cern.ch/sw/lcg/external/gdb/7.6/x86_64-slc6-gcc48-opt/bin/gdb;
fi;
#
#
#
if test "$NODENAME" = "storeio01";
then
  ### Running on storeio01:
  export NODENAME=`python -c "print '$HOST'.split('.')[0]"`-d1;
  export DATAINTERFACE=$NODENAME;
  export TAN_NODE=${DATAINTERFACE};
  echo "DATAINTERFACE ${DATAINTERFACE}";
fi;
#if test -z "${PYOPTS}";
#  then
#    export PYOPTS=${GAUDIONLINEROOT}/python
#    export PYTHONPATH=${PYOPTS}:${PYTHONPATH}
#fi;


#
# -------------------------------------------------------------------------------------
#
start_py_task()
{
    $MINITERM ${1} -e "export UTGID=${NODENAME}_${1}; exec -a \${UTGID} $Class1_task -opt=command=\"${2}\""&
}
#
#
gdb_py_task()
{
    $BIGTERM ${1} -e "export UTGID=${NODENAME}_${1}; exec -a \${UTGID} $GDB --args $Class1_task -opt=command=\"${2}\""&
}
#
#
py_task()
{
    bash -c "export UTGID=${NODENAME}_${1}; exec -a \${UTGID} $Class1_task -opt=command=\"${2}\""&
}
#
# -------------------------------------------------------------------------------------
#
start_opts_task()
{
    $MINITERM ${1} -e "export UTGID=${NODENAME}_${1}; exec -a \${UTGID} $Class1_task -opt=${2}"&
}
#
#
gdb_opts_task()
{
    $BIGTERM ${1} -e "export UTGID=${NODENAME}_${1}; exec -a \${UTGID} $GDB --args $Class1_task -opt=${2}"&
}
#
#
opts_task()
{
    bash -c "export UTGID=${NODENAME}_${1}; exec -a \${UTGID} $Class1_task -opt=${2}"&
}
#
# -------------------------------------------------------------------------------------
#
start_gaudi_task()
{
  $MINITERM ${1} -e "export UTGID=${NODENAME}_${1}; shift; exec -a \${UTGID} $gaudi_run $2 $3 $4 $5 $6 $7 $8 $9"&
}
#
# -------------------------------------------------------------------------------------
#
start_mbmmon()
{
  $BIGTERM MBMMon -e "export UTGID=${NODENAME}_MBMMon; exec -a \${UTGID} $gaudi_run  libOnlineKernel.so mbm_mon $*"&
}
#
# -------------------------------------------------------------------------------------
#
start_mbmdump()
{
  $BIGTERM MBMDump -e "export UTGID=${NODENAME}_MBMDump_\$$; exec -a \${UTGID} $gaudi_run libMBMDump.so mbmdump"&
}
#
# -------------------------------------------------------------------------------------
#
start_meps()
{
  bash -c "export UTGID=${NODENAME}_Prod_\$$; exec -a \${UTGID} $gaudi_run libGaudiOnline.so mep2mbm_producer -n=prod_0 -p=333 -s=500 -r=2 -f=./mepData_0.dat -i=Events_LHCb -e=1"
}
#
# -------------------------------------------------------------------------------------
#
start_tae()
{
  bash -c "export UTGID=${NODENAME}_Prod_\$$; exec -a \${UTGID} $gaudi_run libGaudiOnline.so mep2mbm_producer -n=prod_0 -p=333 -s=500 -r=2 -f=./taeData_0.dat -i=Events_LHCb"
}
#
# -------------------------------------------------------------------------------------
#
start_tan_server()
{
    start_gaudi_task TANServ libOnlineKernel.so tan_nameserver -a -tcp -d;
}
#
# -------------------------------------------------------------------------------------
#
start_tanmon()
{
  $NORMALTERM TANMon -e "export UTGID=${NODENAME}_TANMon_\$$; exec -a \${UTGID} $gaudi_run libOnlineKernel.so tanmon -c"&
}
#
# -------------------------------------------------------------------------------------
#
start_some_python_task()
{
  $MINITERM ${1} -e "UTGID=${NODENAME}_${1}; `which python` -c \"${2}\";"&
}
#
#
start_python_prompt()
{
  $MINITERM ${1} -e "export UTGID=${NODENAME}_${1}; `which python` "&
}
