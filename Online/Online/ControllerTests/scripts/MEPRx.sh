#!/bin/bash
#
export PYTHONPATH=${HOME}/data:${PYTHONPATH};
start_Class2_task "import GaudiOnlineTests;GaudiOnlineTests.runMDF2MBM2(buffers=['Events'],partitionBuffers=False)"
