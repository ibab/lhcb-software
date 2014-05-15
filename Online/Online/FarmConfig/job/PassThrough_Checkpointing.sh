#!/bin/bash
# =========================================================================
#
#  Default script to start the passthrough process on a farm node.
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
export CHECKPOINT_SETUP_OPTIONS=${FARMCONFIGROOT}/options/Checkpoint.opts;
export CHECKPOINT_RESTART_OPTIONS=${FARMCONFIGROOT}/options/CheckpointRestart.opts;
OPTIONS=-opts=../options/${TASK_TYPE}.opts;
if test "${TAE_PROCESSING}" = "TAE";
then
    OPTIONS=-opts=../options/${TASK_TYPE}_TAE.opts;
fi;
#
export LD_PRELOAD=${CHECKPOINTINGROOT}/${CMTCONFIG}/libCheckpointing.so;
export PYTHONPATH=${FARMCONFIGROOT}/job:${PYTHONPATH};
echo exec -a ${UTGID} ${Checkpoint_task} ${OPTIONS};
#gdb --args  ${Checkpoint_task} -opts=../options/${TASK_TYPE}.opts -auto
##exit 1;
echo ${MBM_SETUP_OPTIONS}
export CHECKPOINT_BIN=${CHECKPOINTINGROOT}/${CMTCONFIG};

echo "=============================================================================";
echo "== MOORESTARTUP_MODE = ${MOORESTARTUP_MODE} Directory:${CHECKPOINT_DIR} ";
echo "=============================================================================";

if test "${MOORESTARTUP_MODE}" = "NORMAL";         ## RunInfo flag=0
    then
    unset  MOORESTARTUP_MODE;
    unset  CHECKPOINT_DIR;
    unset  CHECKPOINT_FILE;
elif test "${MOORESTARTUP_MODE}" = "FORKING";      ## RunInfo flag=1
    then
    echo "=============================================================================";
    echo "== MOORESTARTUP_MODE = ${MOORESTARTUP_MODE} ";
    echo "=============================================================================";
    unset  MOORESTARTUP_MODE;
    unset  CHECKPOINT_DIR;
    unset  CHECKPOINT_FILE;
elif test "${MOORESTARTUP_MODE}" = "CREATE_CHECKPOINT";   ## Not handled by RunInfo. Used to create checkpoints.
    then
    export MBM_SETUP_OPTIONS=/group/online/dataflow/cmtuser/checkpoints/cmds/MBM_setup.opts;
    export CHECKPOINT_SETUP_OPTIONS=/group/online/dataflow/cmtuser/checkpoints/cmds/Checkpoint.opts;
    export PYTHONPATH=${CHECKPOINT_DIR}:${PYTHONPATH};
    export MOORESTARTUP_MODE=-auto;
    echo "=============================================================================";
    echo "== File:  ${CHECKPOINT_FILE}"
    echo "== Producing CHECKPOINT file......Please be patient.";
    echo "=============================================================================";
    python -c "import ConfigureShell as c; c.doIt()";
    echo "=============================================================================";
elif test "${MOORESTARTUP_MODE}" = "RESTORE";      ## RunInfo flag=2
    then
    unset  MOORESTARTUP_MODE;
    unset  LD_PRELOAD;
    export CHECKPOINT_FILE=`ls ${CHECKPOINT_DIR}/Checkpoint.*.gz.torrent`;
    cd /localdisk/checkpoints;
    CMDS=`python <<EOF
import os
base='/group/online/dataflow/cmtuser/checkpoints/'
fn  = os.environ['CHECKPOINT_FILE']
dir = os.environ['CHECKPOINT_DIR']
c_dir = '/localdisk/checkpoints/'+dir[len(base):]
print 'mkdir -p '+c_dir+';'
print 'cd '+c_dir+';'
print '/group/online/dataflow/cmtuser/checkpoints/cmds/copy_torrent '+os.environ['CHECKPOINT_FILE']+';'
EOF`
    echo ${CMDS} | tr ';' '\n';
    export CHECKPOINT_FILE=`ls ${CHECKPOINT_DIR}/Checkpoint.*.data`;
    if test -f "${CHECKPOINT_FILE}";
	then
        export ONLINETASKS=/group/online/dataflow/templates
        export PARTITIONOPTS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
	if test -n "${TEST_CHECKPOINT}";
	    then
	    echo "=============================================================================";
	    echo "== File:  ${CHECKPOINT_FILE}";
	    echo "== Testing CHECKPOINT file......Please be patient.";
	    echo "=============================================================================";
	    python -c "import ConfigureFromCheckpoint";
	    echo exec -a ${UTGID} ${CHECKPOINT_BIN}/restore.exe -p 4 -e -l /dev/shm/checkpoint -i ${CHECKPOINT_FILE};
	    echo "=============================================================================";
	fi;
	python -c "import ConfigureFromCheckpoint" | \
            exec -a ${UTGID} ${CHECKPOINT_BIN}/restore.exe -p 4 -e -l /dev/shm/checkpoint -i ${CHECKPOINT_FILE};
    else
	echo " [FATAL] =============================================================================";
	echo " [FATAL] == File:  ${CHECKPOINT_FILE}";
	echo " [FATAL] == CHECKPOINT FILE DOES NOT EXIST!";
	echo " [FATAL] =============================================================================";
    fi;
    exit 0;
fi;

exec -a ${UTGID} ${Checkpoint_task} ${OPTIONS} ${APP_STARTUP_OPTS};
