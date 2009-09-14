#!/bin/bash
. startgauditest.sh HLTA0102_GauchoJob_2 &
export UTGID="HLTA0102_Adder_2";
. Adder.sh&
