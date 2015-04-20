#!/bin/bash
#
export INFO_OPTIONS=${FARMCONFIGROOT}/options/HLT2Reco_Info.opts;
export SUBFARM_OPTIONS=${FARMCONFIGROOT}/options/Empty.opts;
export HLT2RECO_Buffer=Events_HLT2Reco;
export HLT2RECO_READER_NODE=plus11;
export HLT2RECO_WORKER_NODE=plus12;
export DATAINTEFACE=`hostname -s`;
export TAN_PORT=YES;
export TAN_NODE=`hostname -s`;

export HLT2Reco_DataSink1="PLUS12::HLT2Reco_PLUS12_HLT2RecoRcv_0";
export HLT2Reco_DataSink2="PLUS12::HLT2Reco_PLUS12_HLT2RecoRcv_0";
export HLT2Reco_DataSink3="PLUS12::HLT2Reco_PLUS12_HLT2RecoRcv_0";
#
if test "`hostname -s`" = "plus11";
then
    export MBM_SETUP_OPTIONS=/group/online/dataflow/architectures/lbDataflowArch_HLT2RecoReader_MBM_setup.opts;
elif test "`hostname -s`" = "plus12";
then
    export MBM_SETUP_OPTIONS=/group/online/dataflow/architectures/lbDataflowArch_HLT2RecoWorker_MBM_setup.opts;
fi;

echo $MBM_SETUP_OPTIONS;
