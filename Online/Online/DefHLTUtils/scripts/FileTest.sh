#!/bin/bash
#cases: 1. 2 arguments: adder adds processes in a subfarm. options created by this script. $1: parent, $2 UTGID
#       2. 2 arguments: calibration farm. Options to be edited by hand in options/AdderCalibrationfarm.opts
#       3. 3 arguments: adder adds adders in a partition. $1: parent, $2: UTGID. $3:. partition name
#       4. 4 arguments: onlineversion
#
#need to cd in case script is launched outside the taskmanager
#get online version

export CMTCONFIG=x86_64-slc6-gcc48-dbg
#export LD_LIBRARY_PATH=/home/beat/cmtuser/myOnline/Online/DefHLTUtils/$CMTCONFIG:$LD_LIBRARY_PATH
. /group/online/dataflow/cmtuser/Online_v5r8/MyOnline/cmt/setup.${CMTCONFIG}.vars
export LD_PRELOAD=/group/online/dataflow/cmtuser/Online_v5r8/InstallArea/${CMTCONFIG}/lib/libDefUtil.so
$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe libGaudiOnline.so OnlineTask -options=../options/FileTest.opts -main=$GAUDIONLINEROOT/options/Main.opts
