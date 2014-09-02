if test "$1" != ""; then
    arg=`echo ${1} | tr a-z A-Z`;
    shift;
fi;
. ./setup.sh;
. ./preamble.sh;
export PARTITION_NAME=LHCb;
export TASK_TYPE=PassThrough;
export NBOFSLAVES=1;
export NODE=`echo $HOST | tr a-z A-Z`;
#
export CHECKPOINT_DIR=${GAUDIONLINEROOT}/options;
export CHECKPOINT_SETUP_OPTIONS=${FARMCONFIGROOT}/options/Checkpoint.opts;
export CHECKPOINT_RESTART_OPTIONS=../options/CheckpointTestRestart.opts;
#
export PYTHONPATH=${FARMCONFIGROOT}/job:${PYTHONPATH};
export UTGID=${PARTITION_NAME}_${NODE}_${TASK_TYPE}_00;
#
echo "+++ Checkpoint test mode: ${arg}";
if test "${arg}" = "RESTORE" -o "${arg}" = "TEST";
then 
    export NBOFSLAVES=3;
    export MOORESTARTUP_MODE=RESTORE;
    if test "${arg}" = "TEST";
	then
	export TEST_CHECKPOINT="yes";
    else
	unset TEST_CHECKPOINT;
	export NBOFSLAVES=3;
    fi;
    python -c "import ConfigureFromCheckpoint";
    python -c "import ConfigureFromCheckpoint" | `which restore.exe` -e -p 3 -i Checkpoint.data;

else
    pkill -9 GaudiCheckpoint;
    if test "${arg}" = "CREATE"; 
    then 
	export CHECKPOINT_FILE="Checkpoint.data";
	export UTGID="aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	export UTGID="${UTGID}${UTGID}"
    fi;
    export Checkpoint_task="`which GaudiCheckpoint.exe` libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -msgsvc=MessageSvc -auto -main=$OPTS/Main.opts "
    echo "pkill -9 GaudiCheckpoint; debug --args $Checkpoint_task -opt=../options/CheckpointThreads.opts";
    export LD_PRELOAD=${CHECKPOINTINGROOT}/${CMTCONFIG}/libCheckpointing.so;
    export LD_LIBRARY_PATH=/home/frankm/rpmbuild/BUILDROOT/glibc-2.12-1.7.el6.x86_64/lib64:$LD_LIBRARY_PATH;

    ${Checkpoint_task} -opt=../options/CheckpointTest.opts
fi;
