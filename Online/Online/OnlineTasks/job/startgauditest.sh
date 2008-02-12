#!/bin/bash
#this script should allow running of generic gaudi jobs in the eff farm
# evh 15/01/2008
#toplevel is the name of the subfarm - for storage options
#partname is the partition name
#runtype is the activity (empty: use DummyRead.opts)


test -n "$1" ; export TOPLEVEL=$1
test -n "$2" ; export PARTNAME=$2
test -n "$3" ; export RUNTYPE=$3

# remove the args because they interfere with the cmt scripts
while [ $# -ne 0 ]; do
  shift 
done
cd /home/online/Online_v4r5/Online/OnlineTasks/v1r8/job

export INFOOPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
export OPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${TOPLEVEL}_HLT.opts

. /home/online/Online_v4r5/Online/OnlineTasks/v1r8/job/setupOnline.sh

if test -z "$RUNTYPE"; 
  then export USEROPTS=${HLTOPTS}/DummyRead.opts
  else 
    if test -f /group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${RUNTYPE}.opts;
      then echo "/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${RUNTYPE}.opts exists"
      else cp /group/online/dataflow/options/EFFdefault.opts /group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${RUNTYPE}.opts
    fi  
    export USEROPTS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${RUNTYPE}.opts
fi    


echo "Useropts "$USEROPTS


${CLASS1_TASK} -opt=$USEROPTS
