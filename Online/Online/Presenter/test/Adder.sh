# !/bin/sh

$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe \
$GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline \
-runable=LHCb::OnlineRunable \
-options=$PRESENTERROOT/test/Adder.opts \
-msgsvc=MessageSvc \
-loop -auto
