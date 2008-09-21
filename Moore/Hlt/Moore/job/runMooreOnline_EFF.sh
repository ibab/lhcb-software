#!/bin/bash

# Scripts for testing on offline machine the initialisation of MooreOnline
echo 'running ' $0
#to get online joboptions for buffermanager etc
cd job
#. /group/online/hlt/tests/setupOnline.sh $*

. ./setupOnline.sh $*

oldpwd=$PWD
#for cmt
while [ $# -ne 0 ];do
   shift
done   

MOOREROOT=/group/hlt/devel-Moore_v5r3/Moore_v5r3/Hlt/Moore/v5r3
cd $MOOREROOT/job 
. ./pathsetup

export USEROPTS=${MOOREOPTS}/DEFAULT${IS_TAE_RUN}.opts

echo exec -a ${UTGID} ${CLASS1_TASK} -opt=${USEROPTS}
exec -a ${UTGID} ${CLASS1_TASK} -opt=${USEROPTS}


