#!/bin/bash
#
export UTGID=Alarms_$$
. setup.slc4_amd64_gcc34_dbg.vars
#
export DIM_DNS_NODE=ecs03
#
#
xterm -geo 132x48+0+0 -title AlarmDisplay_$$ -132 -ls -e "export UTGID=AlarmDisplay_$$; `which gentest.exe` libROMon.so romon_alarmdisp -part=${1}" &
#
#
sleep 1
#
##xterm -geo 132x24+0+650 -title AlarmDisplay_$$ -132 -ls -e "export UTGID=AlarmLogger_$$; `which gentest.exe` libROMon.so romon_farmmon -part=${1}" &
xterm -geo 132x24+0+650 -title AlarmDisplay_$$ -132 -ls -e "export UTGID=AlarmLogger_$$; `which gentest.exe` libROMon.so romon_farmmon -match=* -taskmon -all" &
