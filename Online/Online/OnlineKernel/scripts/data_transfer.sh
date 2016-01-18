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
LIBRARY=$1;
TYPE=$2;
#
make_transfer_aliases()
{
    alias zmq_transfer='bash ${ONLINEKERNELROOT}/scripts/data_transfer.sh    libZMQTransfer.so  zmq';
    alias ams_transfer='bash ${ONLINEKERNELROOT}/scripts/data_transfer.sh    libOnlineKernel.so ams';
    alias asio_transfer='bash ${ONLINEKERNELROOT}/scripts/data_transfer.sh   libOnlineKernel.so asio';
    alias socket_transfer='bash ${ONLINEKERNELROOT}/scripts/data_transfer.sh libOnlineKernel.so socket';
    alias zmq_bounce='bash ${ONLINEKERNELROOT}/scripts/data_transfer.sh      libZMQTransfer.so  zmq    -bounce';
    alias ams_bounce='bash ${ONLINEKERNELROOT}/scripts/data_transfer.sh      libOnlineKernel.so ams    -bounce';
    alias asio_bounce='bash ${ONLINEKERNELROOT}/scripts/data_transfer.sh     libOnlineKernel.so asio   -bounce';
    alias socket_bounce='bash ${ONLINEKERNELROOT}/scripts/data_transfer.sh   libOnlineKernel.so socket -bounce';
    alias kill_transfer='pkill -9 gentest.exe;killall -9 gentest.exe';
    echo "Type:                                                                   ";
    echo "     ams_transfer    to use ALEPH Message system  as a transfer library.";
    echo "     zmq_transfer    to use ZeroMQ                as a transfer library.";
    echo "     asio_transfer   to use Boost::asio           as a transfer library.";
    echo "     socket_transfer to use home grown socket lib as a transfer library.";
    echo "                                                                        ";
    echo "     ams_bounce      to use ALEPH Message system  as a transfer library.";
    echo "     zmq_bounce      to use ZeroMQ                as a transfer library.";
    echo "     asio_bounce     to use Boost::asio           as a transfer library.";
    echo "     socket_bounce   to use home grown socket lib as a transfer library.";
    echo "                                                                        ";
    echo "     kill_transfer   to KILL all processes and stop.                    ";
}
#
exec_transfer_test()
{
    echo "";
}
#
if test -z "${LIBRARY}";
then
    make_transfer_aliases;
else
    shift;shift;OPT_ARGS=$*;
    TURNS="-turns=1000000";
    if test -n "`echo BB $OPT_ARGS | grep bounce`"; then
	TURNS="-turns=1";
    fi;
    . ${ONLINEKERNELROOT}/scripts/test_macros.sh;
    kill_all_tasks_of_type gentest.exe
    echo "Using load-library ${LIBRARY} for test ${TYPE}";
    start_gentest ${LIBRARY} TanServer boost_asio_tan_server -d -v
    sleep 1;
    start_gentest ${LIBRARY} TanMon tanmon -c
    sleep 2;
    TARGET=${HOST}::RCV_${PROCID};
    echo PROCID=${PROCID};
    echo TARGET=${TARGET};
    start_gentest ${LIBRARY} Receiver test_${TYPE}_net_recv \
	-name=${TARGET} \
	-l=1000000 -threads=10 ${OPT_ARGS};
    sleep 2;

    ps -ef | grep -v xterm | grep Receiver;
    sleep 3;
    for ID in $(seq 0 6); do
	start_gentest ${LIBRARY} SND_${ID} test_${TYPE}_net_send \
	    -name=${HOST}::SND${PROCID}_${ID} \
	    -target=${TARGET} ${TURNS} \
	    -count=1 -len=48000 ${OPT_ARGS};
    done;
fi;
