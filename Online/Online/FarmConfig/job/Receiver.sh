#!/bin/bash
#
export TAN_PORT=YES;
export TAN_NODE=${DATAINTERFACE};
exec -a ${UTGID} ${Class1_task} -opts=../options/RCV.opts
