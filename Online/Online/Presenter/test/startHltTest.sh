#!/bin/bash
. startgauditest.sh HLTA0101_GauchoJob_1 &
export UTGID="HLTA0101_Adder_1";
. Adder.sh&
