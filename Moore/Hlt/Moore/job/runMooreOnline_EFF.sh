#!/bin/bash

# Scripts for testing on offline machine the initialisation of MooreOnline
echo 'running ' $0
#to get online joboptions for buffermanager etc

#case $0 in 
#   ./*)  echo `dirname $PWD/$0`
#   * )   echo `dirname $0`
#esac
##TODO: strip leading ./ in first case...

cd job
#. /group/online/hlt/tests/setupOnline.sh $*

. ./setupOnline.sh $*

oldpwd=$PWD
#for cmt
while [ $# -ne 0 ];do
   shift
done   

# TODO: use cmt filter, driven by pattern in GaudiPolicy,
# to generate 'v5r3'
#source ${LHCBSCRIPTS}/SetupMoore_v5r3.sh
source ${LHCBSCRIPTS}/SetupMoore_${MOOREVERSION}.sh
export USEROPTS=${MOOREROOT}/options/DEFAULT${IS_TAE_RUN}.opts
echo exec -a ${UTGID} ${CLASS1_TASK} -opt=${USEROPTS}
exec -a ${UTGID} ${CLASS1_TASK} -opt=${USEROPTS}
