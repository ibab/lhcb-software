#!/bin/bash
killall gentest.exe
pkill gentest.exe
HOST=`hostname -s`;
EXEC="`which gentest.exe` libZMQTransfer.so";
MINITERM='xterm -ls -132 -geometry 132x32 -e';
PROCID=$$;
#
$MINITERM "$EXEC boost_asio_tan_server -d -v" &
sleep 2;
TARGET=${HOST}::RCV_${PROCID};
$MINITERM "$EXEC test_zmq_net_recv -name=${TARGET} -l=1000000 -bounce -threads=20" &
sleep 2;
#
$MINITERM "$EXEC test_zmq_net_send -name=${HOST}::SND${PROCID}_0 -target=${TARGET} -turns=1 -count=1 -len=48000 -bounce" &
$MINITERM "$EXEC test_zmq_net_send -name=${HOST}::SND${PROCID}_1 -target=${TARGET} -turns=1 -count=1 -len=48000 -bounce" &
$MINITERM "$EXEC test_zmq_net_send -name=${HOST}::SND${PROCID}_2 -target=${TARGET} -turns=1 -count=1 -len=48000 -bounce" &
$MINITERM "$EXEC test_zmq_net_send -name=${HOST}::SND${PROCID}_3 -target=${TARGET} -turns=1 -count=1 -len=48000 -bounce" &
