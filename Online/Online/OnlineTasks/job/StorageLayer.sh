#
#  Script to start tasks on a storage layer node
#  RECV_0 receives events from HLT farm and declares them to the event buffer
#  MDFWriter takes events from the Event buffer and writes them to disk
#
#
. $GAUDIONLINEROOT/tests/cmt/preamble.sh
export DIM_DNS_NODE=pclhcb79.cern.ch
$MINITERM InputBuffer@${HOST} -e "/bin/sh -c 'export UTGID=InputBuffer;$gaudi_exe -main=$OPTS/MBMfarm.opts -opt=$OPTS/Daemon.opts'" &
#
$WIDETERM ErrorLogger@${HOST}    -e "setenv UTGID ErrLog; $gaudi_exe2 -opts=$GAUDIONLINEROOT/options/ErrorLogger.opts -main=$GAUDIONLINEROOT/options/Remap.opts " &
sleep 3
$MINITERM ErrServ@${HOST} -e "setenv UTGID ErrServ   ; $gaudi_exe -main=$GAUDIONLINEROOT/options/ErrorSrv.opts -opt=$GAUDIONLINEROOT/options/Daemon.opts " &
#
sleep 8
#
$BIGTERM MBMMon@${HOST} -e "/bin/sh -c 'export UTGID=MBMMon;$gaudi_run libOnlineKernel.so mbm_mon'" &
#
export UTGID=RECV_0
$MINITERM Recv_0@${HOST} -e "/bin/sh -c '$gaudi_exe -opt=$ONLINETASKSROOT/options/RecvFromFarm.opts -main=$GAUDIONLINEROOT/options/Main.opts -auto'" &
#ddd --command=gaudi.gdb $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe &
#$MINITERM MDFWriter@${HOST} -e "export UTGID=MDFWriter ; ${gaudi_exe} -opt=$ONLINETASKSROOT/options/MDFWriterNet.opts -main=$GAUDIONLINEROOT/options/Main.opts " &
export UTGID=MDFWriter
${gaudi_exe} -opt=$ONLINETASKSROOT/options/StorageLayerRecv.opts -main=$GAUDIONLINEROOT/options/Main.opts &
