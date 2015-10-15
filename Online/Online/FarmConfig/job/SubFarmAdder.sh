#!/bin/bash
# =========================================================================
#
#  Default script to start the event reader task on the HLT farm
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
if [ "${PARTITION_NAME}" = "LHCb2" ]; then
    ./Hlt2Adder.sh
#    ./DummyTask.sh
else
#    export victimnodes="hltb01(.*)"
    export PYTHONPATH=${ONLINE_ENV_DIR}:${PYTHONPATH}
    export AdderOptsFile=/tmp/${PARTITION_NAME}_AdderOpts.opts
    export AdderStaticOpts=../options/SubFarmAdderStatic.opts
    python ./Tasklist_from_architecture.py 2 ${AdderOptsFile}
    exec -a ${UTGID} ${Class1_task} -opts=../options/genAdder.opts
fi
