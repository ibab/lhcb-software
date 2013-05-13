#!/bin/bash
#
export NODENAME=`python -c "print '$HOST'.split('.')[0]"`
export OPTS=$GAUDIONLINEROOT/options
export msg_svc=LHCb::FmcMessageSvc
export msg_svc=MessageSvc
export gaudi_run="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineStart "
export gaudi_task="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=$msg_svc "
export Class0_task="$gaudi_task -tasktype=LHCb::Class0Task "
export Class1_task="$gaudi_task -tasktype=LHCb::Class1Task -main=$OPTS/Main.opts "
export Class2_task="$gaudi_task -tasktype=LHCb::Class2Task -main=$OPTS/Main.opts "
export MINITERM='xterm  -ls -132 -geometry 132x12 -title '
export BIGTERM='xterm  -ls -132 -geometry 132x65 -title '
#
#
#
mbmmon()
{
  $BIGTERM MBMMon@${HOST} -e "export UTGID=${NODENAME}/MBMMon; exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
}
#
#
#
start_py_task()
{
  ##echo exec -a \${UTGID} ${Class1_task} -opt=command="${1}";
  ##echo ${1};
  exec -a ${UTGID} ${Class1_task} -opt=command="${1}";
}
start_Class2_task()
{
  ##echo exec -a \${UTGID} ${Class2_task} -opt=command="${1}";
  ##echo ${1};
  exec -a ${UTGID} ${Class1_task} -opt=command="${1}";
}
