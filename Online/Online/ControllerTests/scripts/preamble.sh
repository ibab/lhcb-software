#!/bin/bash
#
export NODENAME=`python -c "print '$HOST'.split('.')[0]"`
export OPTS=/group/online/dataflow/templates/options
export msg_svc=MessageSvc
export msg_svc=LHCb::FmcMessageSvc
export gaudi_task="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=$msg_svc "
export Class0_task="$gaudi_task -tasktype=LHCb::Class0Task "
export Class1_task="$gaudi_task -tasktype=LHCb::Class1Task -main=$OPTS/Main.opts "
export Class2_task="$gaudi_task -tasktype=LHCb::Class2Task -main=$OPTS/Main.opts "
export Checkpoint_task="$GAUDIONLINEROOT/$CMTCONFIG/GaudiCheckpoint.exe libGaudiOnline.so OnlineTask -msgsvc=$msg_svc  -tasktype=LHCb::Class1Task -main=$OPTS/Main.opts "
##export MINITERM='xterm  -ls -132 -geometry 132x12 -title '
#
#
#
mbmmon()
{
  export BIGTERM='xterm  -ls -132 -geometry 132x65 -title ';
  export gaudi_run="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineStart ";
  $BIGTERM MBMMon@${HOST} -e "export UTGID=${NODENAME}/MBMMon; exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
}
