#!/bin/sh
#killall test.exe
killall Gaudi.exe
#killall gentest.exe
#rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
#
export TAN_PORT=YES
export TAN_NODE=$HOSTNAME
if test -z "${DIM_DNS_NODE}";
then
    export DIM_DNS_NODE=$HOSTNAME;
fi;
#
export OPTS=$GAUDIONLINEROOT/options
export msg_svc=LHCb::FmcMessageSvc
export msg_svc=MessageSvc

export test_exe="$ONLINEKERNELROOT/$CMTCONFIG/test.exe "
export gaudi_run="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineStart "
export gaudi_exe="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=$msg_svc -auto "
export gaudi_exe2="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto "
export gaudi_task="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=$msg_svc -auto -main=$OPTS/Main.opts "
export Class0_task="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -tasktype=LHCb::Class0Task -msgsvc=$msg_svc -auto -main=$OPTS/Main.opts "
export Class1_task="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -msgsvc=$msg_svc -auto -main=$OPTS/Main.opts "
export Class2_task="$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -tasktype=LHCb::Class2Task -msgsvc=$msg_svc -auto -main=$OPTS/Main.opts "
export MINITERM='xterm  -ls -132 -geometry 132x12 -title '
export BIGTERM='xterm  -ls -132 -geometry 132x65 -title '
export WIDETERM='xterm  -ls -132 -geometry 160x50 -title '
#export MINITERM=echo
export NODENAME=`python -c "print '$HOST'.split('.')[0]"`
if test "$NODENAME" = "storeio01";
then
  ### Running on storeio01:
  export NODENAME=`python -c "print '$HOST'.split('.')[0]"`-d1;
  export DATAINTERFACE=$NODENAME;
  export TAN_NODE=${DATAINTERFACE};
  echo "DATAINTERFACE ${DATAINTERFACE}";
fi;
if test -z "${PYOPTS}";
  then
    export PYOPTS=${GAUDIONLINEROOT}/python
    export PYTHONPATH=${PYOPTS}:${PYTHONPATH}
fi;
#
start_py_task()
{
  $MINITERM ${1}@${HOST}   -e "export UTGID=${NODENAME}/${1};   exec -a \${UTGID} $Class1_task -opt=command=\"${2}\""&
  ##bash -c "export UTGID=${NODENAME}/${1};   exec -a \${UTGID} $Class1_task -opt=command=\"${2}\""&
}
start_gaudi_task()
{
  $MINITERM ${1}@${HOST}   -e "UTGID=${NODENAME}/${1}; `which python` -c \"${2}\";"&
}
start_python_prompt()
{
  $MINITERM ${1}@${HOST}   -e "UTGID=${NODENAME}/${1} `which python` "&
}
