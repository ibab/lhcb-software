#!/bin/bash
#this script should allow running of generic gaudi jobs in the eff farm
# evh 15/01/2008
#parent is the name of the subfarm - for storage options
#partname is the partition name
#runtype is the activity (empty: use DummyRead.opts)

cd job
export DEBUGGING=YES

. ./setupOnline.sh $*

export USEROPTS=${HLTOPTS}/DummyRead${IS_TAE_RUN}.opts

# if test -f /group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${RUNTYPE}${IS_TAE_RUN}.opts;
#     then 
#     export USEROPTS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${RUNTYPE}${IS_TAE_RUN}.opts;
# fi

#echo "${UTGID} Script:  INFO  Useropts "$USEROPTS

exec -a ${UTGID} ${CLASS1_TASK} -opt=$USEROPTS
