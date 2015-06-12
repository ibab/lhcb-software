#!/bin/bash
#
cd /group/online/dataflow/cmtuser/DataQuality;
. ./setup.${CMTCONFIG}.vars;
cd /group/online/dataflow/cmtuser/DataQuality/Online/DataQuality/scripts;
export INFO_OPTIONS=${DATAQUALITYROOT}/options/DQ_Info.opts;
export SUBFARM_OPTIONS=${FARMCONFIGROOT}/options/Empty.opts;
export DQ_Buffer=Events_${PARTITION_NAME};
export DQ_READER_NODE=plus11;
export DQ_WORKER_NODE=plus12;
export DATAINTEFACE=`hostname -s`;
export TAN_PORT=YES;
export TAN_NODE=`hostname -s`;
#
export DQ_DataSink1="MONA1002::${PARTITION_NAME}_MONA1002_DQRcv_0";
export DQ_DataSink2="MONA1003::${PARTITION_NAME}_MONA1003_DQRcv_0";
export DQ_DataSink3="MONA1004::${PARTITION_NAME}_MONA1004_DQRcv_0";
#
export DQREADER_NODE_OPTIONS=/group/online/dataflow/cmtuser/DQ/DATAQUALITY_OPT/ReaderInput.opts;
#
WORKERS=`echo ${DATAINTEFACE} | cut -b 1-7`;
if test "${DATAINTEFACE}" = "mona1001";
then
    export MBM_SETUP_OPTIONS=/group/online/dataflow/architectures/lbDataflowArch_DQReader_MBM_setup.opts;
elif test "${WORKERS}" = "mona100"
then
    export MBM_SETUP_OPTIONS=/group/online/dataflow/architectures/lbDataflowArch_DQWorker_MBM_setup.opts;
fi;
#
#
if test -f ./${TASK_TYPE}.sh;
then
    . ./${TASK_TYPE}.sh;
else
    exec -a ${UTGID} ${Class1_task} -opts=../options/${TASK_TYPE}.opts;
fi;
