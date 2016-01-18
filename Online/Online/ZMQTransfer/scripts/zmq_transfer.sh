#!/bin/bash
#==========================================================================
#  LHCb Online software suite
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see OnlineSys/LICENSE.
#
# Author     : M.Frank
#
#==========================================================================
#
. ${ONLINEKERNELROOT}/scripts/test_macros.sh;
kill_all_tasks_of_type gentest.exe
#
start_gentest libZMQTransfer.so TanServer boost_asio_tan_server -d -v
start_gentest libZMQTransfer.so TanMon tanmon -c
sleep 1;
TARGET=${HOST}::RCV_${PROCID};
echo PROCID=${PROCID};
echo TARGET=${TARGET};
start_gentest libZMQTransfer.so Receiver test_zmq_net_recv -name=${TARGET} -l=1000000 -threads=10;
sleep 2;
#
ps -ef | grep -v xterm | grep Receiver;
# sleep 20;
#
for ID in $(seq 0 6); do
    start_gentest libZMQTransfer.so SND_${ID} test_zmq_net_send -name=${HOST}::SND${PROCID}_${ID} -target=${TARGET} -threads=1 -turns=1000000 -count=1 -len=48000;
done;
