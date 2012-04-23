#!/bin/bash
#cases: 1. 2 arguments: adder adds processes in a subfarm. options created by this script. $1: parent, $2 UTGID 
#       2. 2 arguments: calibration farm. Options to be edited by hand in options/AdderCalibrationfarm.opts
#       3. 3 arguments: adder adds adders in a partition. $1: parent, $2: UTGID. $3:. partition name 
#       4. 4 arguments: onlineversion
#       
#need to cd in case script is launched outside the taskmanager
#get online version

export CMTCONFIG=$CMTDEB
SetupOnline v4r56

export UTGID=Adder
export DIM_DNS_NODE=localhost
export OPTS=/home/beat/cmtuser/myOnline/Online/Gaucho
export LD_LIBRARY_PATH=/home/beat/cmtuser/myOnline/Online/Gaucho/$CMTCONFIG:$LD_LIBRARY_PATH
#$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so OnlineTask -options=../options/nodeAdder.opts -main=/sw/lib/lhcb/ONLINE/ONLINE_v4r53/Online/GaudiOnline/options/Main.opts 
