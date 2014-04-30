#!/bin/bash
#cases: 1. 2 arguments: adder adds processes in a subfarm. options created by this script. $1: parent, $2 UTGID
#       2. 2 arguments: calibration farm. Options to be edited by hand in options/AdderCalibrationfarm.opts
#       3. 3 arguments: adder adds adders in a partition. $1: parent, $2: UTGID. $3:. partition name
#       4. 4 arguments: onlineversion
#
#need to cd in case script is launched outside the taskmanager
#get online version

export CMTCONFIG=$CMTDEB
SetupOnline v5r8

export UTGID=FileTest
export DIM_DNS_NODE=localhost
export OPTS=/home/beat/cmtuser/myOnline/Online/Gaucho
cd /home/beat/cmtuser/myOnline/Online/DefHLTUtils/cmt
source setup.sh
#export LD_LIBRARY_PATH=/home/beat/cmtuser/myOnline/Online/DefHLTUtils/$CMTCONFIG:$LD_LIBRARY_PATH
#export LD_PRELOAD=/home/beat/cmtuser/myOnline/Online/DefHLTUtils/$CMTCONFIG/libDefUtil.so
$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -options=/home/beat/cmtuser/myOnline/Online/DefHLTUtils/options/FileTest.opts -main=$GAUDIONLINEROOT/options/Main.opts
