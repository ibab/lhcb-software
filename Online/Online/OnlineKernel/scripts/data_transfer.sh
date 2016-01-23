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
#
#
LIBRARY=$1;
TYPE=$2;
#
#
#
make_transfer_aliases()
{
    alias zmq_transfer='   exec_transfer_test libZMQTransfer.so  zmq';
    alias ams_transfer='   exec_transfer_test libOnlineKernel.so ams';
    alias ipc_transfer='   exec_transfer_test libOnlineKernel.so asio_ipc /dev/shm/';
    alias asio_transfer='  exec_transfer_test libOnlineKernel.so asio';
    alias socket_transfer='exec_transfer_test libOnlineKernel.so socket';
    alias zmq_bounce='     exec_transfer_test libZMQTransfer.so  zmq      -bounce';
    alias ams_bounce='     exec_transfer_test libOnlineKernel.so ams      -bounce';
    alias ipc_bounce='     exec_transfer_test libOnlineKernel.so asio_ipc  /dev/shm/ -bounce';
    alias asio_bounce='    exec_transfer_test libOnlineKernel.so asio     -bounce';
    alias socket_bounce='  exec_transfer_test libOnlineKernel.so socket   -bounce';
    alias kill_transfer='  pkill -9 gentest.exe;killall -9 gentest.exe';
    alias start_asio_send='start_sender libOnlineKernel.so asio';
    alias start_asio_recv='start_receiver libOnlineKernel.so asio';
    echo "Type:                                                                   ";
    echo "     ams_transfer    to use ALEPH Message system  as a transfer library.";
    echo "     zmq_transfer    to use ZeroMQ                as a transfer library.";
    echo "     ipc_transfer    to use Boost::asio IPC       as a transfer library.";
    echo "     asio_transfer   to use Boost::asio           as a transfer library.";
    echo "     socket_transfer to use home grown socket lib as a transfer library.";
    echo "                                                                        ";
    echo "     ams_bounce      to use ALEPH Message system  as a transfer library.";
    echo "     zmq_bounce      to use ZeroMQ                as a transfer library.";
    echo "     ipc_bounce      to use Boost::asio IPC       as a transfer library.";
    echo "     asio_bounce     to use Boost::asio           as a transfer library.";
    echo "     socket_bounce   to use home grown socket lib as a transfer library.";
    echo "                                                                        ";
    echo "     start_asio_send <target-node>::<target-process>                    ";
    echo "     start_asio_recv <receive-node>::<receive-process>                  ";
    echo "                                                                        ";
    echo "     kill_transfer   to KILL all processes and stop.                    ";
}
#
#
#
start_tan_server()
{
    LIBRARY=$1; 
    start_gentest ${LIBRARY} TanServer boost_asio_tan_server -d -v;
    sleep 1;
    start_gentest ${LIBRARY} TanMon tanmon -c;
    sleep 2;
}
#
#
#
start_receiver()
{
    . ${ONLINEKERNELROOT}/scripts/test_macros.sh;
    HOSTNAME=${HOST}-d1;
    TARGET=${HOSTNAME}::RCV_${PROCID};
    LIBRARY=$1;
    TYPE=$2;
    shift;
    shift;
    if test -n "$1"; then
	echo TARGET=$1;
	TARGET=$1;
	shift;
    fi;

    OPT_ARGS=$*;
    TURNS="-turns=1000000";
    if test -n "`echo BB $OPT_ARGS | grep bounce`"; then
	TURNS="-turns=1";
    fi;
    echo "+++ Executing data transfer test ${TYPE} using load-library ${LIBRARY} PROCID=${PROCID}";
    start_tan_server ${LIBRARY};
    export TAN_NODE=${HOSTNAME};
    start_gentest ${LIBRARY} Receiver test_${TYPE}_net_recv \
	-name=${TARGET} \
	-l=1000000 -threads=10 ${OPT_ARGS};
    sleep 2;
    ps -ef | grep -v xterm | grep Receiver;
}
#
#
#
start_sender()
{
    . ${ONLINEKERNELROOT}/scripts/test_macros.sh;
    HOSTNAME=${HOST}-d1;
    TARGET=${HOSTNAME}::RCV_${PROCID};
    LIBRARY=$1;
    TYPE=$2;
    shift;
    shift;
    if test -n "$1"; then
	echo TARGET=$1;
	TARGET=$1;
	shift;
    fi;
    OPT_ARGS=$*;
    TURNS="-turns=1000000";
    echo "+++ Executing data transfer test ${TYPE} using load-library ${LIBRARY} PROCID=${PROCID}";
    start_tan_server ${LIBRARY};
    export TAN_NODE=${HOSTNAME};
    echo TARGET=${TARGET};
    for ID in $(seq 0 6); do
	start_gentest ${LIBRARY} SND_${ID} test_${TYPE}_net_send \
	    -name=${HOSTNAME}::SND${PROCID}_${ID} \
	    -target=${TARGET} ${TURNS} \
	    -count=1 -len=48000 ${OPT_ARGS};
    done;
}
#
#
#
exec_transfer_test()
{
    . ${ONLINEKERNELROOT}/scripts/test_macros.sh;
    HOSTNAME="${HOST}::";
    LIBRARY=$1;
    TYPE=$2;
    shift;
    shift;
    if test -n "$1"; then
	export HOSTNAME=${1};
	echo export HOSTNAME=${HOSTNAME};
	shift;
    fi;

    if test -n "$1"; then
	echo export PROCID=$1;
	export PROCID=$1;
	shift;
    fi;
    TARGET=${HOSTNAME}RCV_${PROCID};
    OPT_ARGS=$*;
    TURNS="-turns=1000000";
    if test -n "`echo BB $OPT_ARGS | grep bounce`"; then
	TURNS="-turns=1";
    fi;
    echo "+++ Executing data transfer test ${TYPE} using load-library ${LIBRARY}";
    kill_all_tasks_of_type gentest.exe
    start_tan_server ${LIBRARY};
    export TAN_NODE=${HOST};
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
	    -name=${HOSTNAME}SND${PROCID}_${ID} \
	    -target=${TARGET} ${TURNS} \
	    -count=1 -len=48000 ${OPT_ARGS};
    done;
}
#
#
#
if test -z "${LIBRARY}";  then
    make_transfer_aliases;
else
    exec_transfer_test $*;
fi;
