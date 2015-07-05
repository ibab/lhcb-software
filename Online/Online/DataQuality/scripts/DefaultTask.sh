#!/bin/bash
#
export CMTCONFIG=x86_64-slc6-gcc48-dbg;
cd /group/online/dataflow/cmtuser/DataQuality;
. ./setup.${CMTCONFIG}.vars;
cd /group/online/dataflow/cmtuser/DataQuality/Online/DataQuality/scripts;
export INFO_OPTIONS=${DATAQUALITYROOT}/options/InfoOptions.opts;
export SUBFARM_OPTIONS=${FARMCONFIGROOT}/options/Empty.opts;
#
# Use the data network for the data transfers....
export TAN_PORT=YES;
export TAN_NODE=`hostname -s`;
export DATAINTEFACE=`hostname -s`;
export DATAINTERFACE=`python /group/online/dataflow/scripts/getDataInterface.py`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
#
export DQ_DataSink1="MONA1002::${PARTITION_NAME}_MONA1002_DQRcv_0";
export DQ_DataSink2="MONA1003::${PARTITION_NAME}_MONA1003_DQRcv_0";
export DQ_DataSink3="MONA1004::${PARTITION_NAME}_MONA1004_DQRcv_0";
#
export READER_NODE_OPTIONS=/group/online/dataflow/cmtuser/${PARTITION_NAME}/DATAQUALITY_OPT/ReaderInput.opts;
##ls -laf $READER_NODE_OPTIONS;
#
WORKERS=`echo ${DATAINTEFACE} | cut -b 1-7`;
if test "${PARTITION_NAME}" = "H2" -a "${DATAINTEFACE}" = "mona1001";then
    export MBM_CONFIG_OPTS=${FARMCONFIGROOT}/options/Empty.opts;
    export MBM_SETUP_OPTIONS=/group/online/dataflow/architectures/lbDataflowArch_H2Monitor_MBM_setup.opts;
elif test "${PARTITION_NAME}" = "DQ" -a "${DATAINTEFACE}" = "mona1001";then
    export MBM_CONFIG_OPTS=${DATAQUALITYROOT}/options/DQReaderMBM.opts;
    export MBM_SETUP_OPTIONS=/group/online/dataflow/architectures/lbDataflowArch_DQReader_MBM_setup.opts;
elif test "${PARTITION_NAME}" = "DQ" -a "${WORKERS}" = "mona100";then
    export MBM_CONFIG_OPTS=${DATAQUALITYROOT}/options/DQWorkerMBM.opts;
    export MBM_SETUP_OPTIONS=/group/online/dataflow/architectures/lbDataflowArch_DQWorker_MBM_setup.opts;
fi;
#
#
if test -f ./${TASK_TYPE}.sh;
then
    . ./${TASK_TYPE}.sh $*;
else
    exec -a ${UTGID} ${Class1_task} -opts=../options/${TASK_TYPE}.opts;
fi;
