#!/bin/bash

export UTGID
export LOGFIFO
export PARTITION;
export NBOFSLAVES
#
cd `dirname $0`/../..
#
TASK_CLASS_TYPE=${2}
export TASK_TYPE=${3}
export DIM_DNS_NODE=${4}
#
export CMTCONFIG=x86_64-slc6-gcc48-dbg
export CMTCONFIG=x86_64-slc6-gcc48-opt
#
. setup.${CMTCONFIG}.vars
#
export PYTHONPATH=/group/online/dataflow/options/${PARTITION}/RECONSTRUCTION:/group/online/hlt/conditions/RunChangeHandler:${PYTHONPATH}
#
#
export DATAINTERFACE=`python /group/online/dataflow/scripts/getDataInterface.py`
export TAN_PORT=YES
export TAN_NODE=${DATAINTERFACE}
export ONLINETASKS=/group/online/dataflow/templates
export PARTITIONOPTS=/group/online/dataflow/options/${PARTITION}/${PARTITION}_Info.opts;
#
ulimit -d 2097152
ulimit -m 2097152
ulimit -v 2097152
#
exec_restart="restart";
exec_restart="normal";
#
make_line()
{
    if test "$*" = "";then
	echo "=============================================================================";
    else
	echo "== $* ";
    fi;
}
if test "${APP_STARTUP_OPTS}" = "-checkpoint";
then
    export CHECKPOINT_DIR;
    export CHECKPOINT_FILE;
    export PYTHONPATH=${CHECKPOINT_DIR}:${PYTHONPATH};
    export LD_PRELOAD=${CHECKPOINTING_BIN}/lib/libCheckpointing.so;
    OBS=${ONLINEBRUNELSYSROOT};
    make_line "[INFO] +++ Checkpoint BRUNEL APP_STARTUP_OPTS: ${APP_STARTUP_OPTS} ";
    make_line "[INFO] +++ Checkpoint BRUNEL CHECKPOINT_FILE:  ${CHECKPOINT_FILE}";
    make_line "[INFO] +++ Checkpoint BRUNEL ${UTGID} with DNS:${DIM_DNS_NODE} Checkpoint:${CHECKPOINT_FILE}";
    `which GaudiCheckpoint.exe` libGaudiOnline.so OnlineTask \
	-checkpoint \
	-msgsvc=LHCb::FmcMessageSvc \
	-tasktype=LHCb::Class1Task \
	-main=$GAUDIONLINEROOT/options/Main.opts \
	-opt=command="\
import Gaudi,GaudiKernel.ProcessJobOptions;\
from Gaudi.Configuration import importOptions;\
GaudiKernel.ProcessJobOptions.printing_level=999;\
importOptions('OnlineBrunelSys/python/BrunelOnline.py');"

    if test -f ${CHECKPOINT_FILE}; then
	make_line;
	make_line "Create torrent file with 3 MB pieces: ${CHECKPOINT_FILE}.torrent";
	make_line "Please wait for the completion message before proceeding.";
	make_line;
	cd `dirname ${CHECKPOINT_FILE}`;
	/group/online/dataflow/scripts/BitTorrentMake `basename ${CHECKPOINT_FILE}` \
	    -s 3145728 \
	    -o `basename ${CHECKPOINT_FILE}`.torrent;
	make_line;
	make_line "Torrent file created: ${CHECKPOINT_FILE}.torrent";
	make_line "All actions done. Now test the checkpoint....";
	make_line;
    else
	echo " [ERROR] ============================================================================="
	echo " [ERROR] == File:  ${CHECKPOINT_FILE}"
	echo " [ERROR] == CHECKPOINT FILE DOES NOT EXIST. Cannot create torrent!";
	echo " [ERROR] ============================================================================="
    fi;
elif test "${exec_restart}" = "restart" || test "${APP_STARTUP_OPTS}" = "-restart"; then
    OBS=${ONLINEBRUNELSYSROOT};
    eval `python ${OBS}/python/SetupCheckpointing.py ${CHECKPOINT_FILE}`
    if test -f "${CHECKPOINT_FILE}"
	then
	export CHECKPOINT_FILE;
	echo "[INFO] +++ Starting BRUNEL ${UTGID} with DNS:${DIM_DNS_NODE} Checkpoint:${CHECKPOINT_FILE}"
	unset LD_PRELOAD;
	unset LD_LIBRARY_PATH;
	if test -n  "${TEST_CHECKPOINT}";
	    then
	    export TEST_CHECKPOINT;
	    make_line;
	    make_line "File:  ${CHECKPOINT_FILE}";
	    make_line "Testing CHECKPOINT file......Please be patient.";
	    make_line;
	    `which python` ${OBS}/python/BrunelOnlineRestart.py;
	    echo exec -a ${UTGID} `which restore.exe` -p 3 -e -i ${CHECKPOINT_FILE};
	    make_line;
	    export PYTHONPATH=${CHECKPOINT_DIR}:${PYTHONPATH};
	    export PARTITIONOPTS=${CHECKPOINT_DIR}/OnlineEnv.opts;
	fi;
	`which python` ${OBS}/python/BrunelOnlineRestart.py | exec -a ${UTGID} `which restore.exe` -p 3 -e -i ${CHECKPOINT_FILE};
    else
	echo " [ERROR] =============================================================================";
	echo " [ERROR] == File:  ${CHECKPOINT_FILE}"
	echo " [ERROR] == CHECKPOINT FILE DOES NOT EXIST!";
	echo " [ERROR] =============================================================================";
    fi;
else
    ## Normal running, nothing special
    echo "[INFO] +++ Starting BRUNEL ${UTGID} of class ${TASKCLASS} ${TASK_TYPE} with DNS:${DIM_DNS_NODE} Version:${BRUNELROOT}";
    export LD_PRELOAD=${CHECKPOINTING_BIN}/lib/libCheckpointing.so;
    exec -a ${UTGID} `which GaudiCheckpoint.exe` libGaudiOnline.so OnlineTask \
	-msgsvc=LHCb::FmcMessageSvc \
	-tasktype=LHCb::${TASK_CLASS_TYPE}Task \
	-main=$ONLINETASKS/options/Main.opts \
	-opt=command="\
import Gaudi,GaudiKernel.ProcessJobOptions;\
from Gaudi.Configuration import importOptions;\
GaudiKernel.ProcessJobOptions.printing_level=999;\
importOptions('OnlineBrunelSys/python/BrunelOnline.py');"

fi;
