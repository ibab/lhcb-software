#!/bin/bash
killall gentest.exe
pkill gentest.exe
HOST=`hostname -s`;
EXEC="`which gentest.exe` libZMQTransfer.so";
MINITERM='xterm -ls -132 -geometry 132x32 -e';
#
$MINITERM "$EXEC zmq_pull_recv -sink=tcp://*:5555" &
sleep 2;
#
$MINITERM "export UTGID=${HOST}::ZMQ_Send_0; $EXEC zmq_push_send -target=tcp://localhost:5555" &
$MINITERM "export UTGID=${HOST}::ZMQ_Send_1; $EXEC zmq_push_send -target=tcp://localhost:5555" &
$MINITERM "export UTGID=${HOST}::ZMQ_Send_2; $EXEC zmq_push_send -target=tcp://localhost:5555" &
$MINITERM "export UTGID=${HOST}::ZMQ_Send_3; $EXEC zmq_push_send -target=tcp://localhost:5555" &
