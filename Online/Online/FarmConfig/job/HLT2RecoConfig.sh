#!/bin/bash
#
export INFO_OPTIONS=${FARMCONFIGROOT}/options/HLT2Reco_Info.opts;
export SUBFARM_OPTIONS=${FARMCONFIGROOT}/options/Empty.opts;
export HLT2RECO_Buffer=Events_${PARTITION_NAME};
export HLT2RECO_READER_NODE=plus11;
export HLT2RECO_WORKER_NODE=plus12;
export DATAINTEFACE=`hostname -s`;
export TAN_PORT=YES;
export TAN_NODE=`hostname -s`;

export HLT2Reco_DataSink1="MONA1002::${PARTITION_NAME}_MONA1002_HLT2RecoRcv_0";
export HLT2Reco_DataSink2="MONA1003::${PARTITION_NAME}_MONA1003_HLT2RecoRcv_0";
export HLT2Reco_DataSink3="MONA1004::${PARTITION_NAME}_MONA1004_HLT2RecoRcv_0";
#
WORKERS=`echo ${DATAINTEFACE} | cut -b 1-7`;
if test "${DATAINTEFACE}" = "mona1001";
then
    export MBM_SETUP_OPTIONS=/group/online/dataflow/architectures/lbDataflowArch_HLT2RecoReader_MBM_setup.opts;
elif test "${WORKERS}" = "mona100"
then
    export MBM_SETUP_OPTIONS=/group/online/dataflow/architectures/lbDataflowArch_HLT2RecoWorker_MBM_setup.opts;
fi;
###echo $MBM_SETUP_OPTIONS;
