#!/bin/bash
source startgauditest.sh HLTA0102_GauchoJob_2 &
#setenv  UTGID "HLTA0102_Adder_2";
export UTGID="HLTA0102_Adder_2";
source Adder.sh&
