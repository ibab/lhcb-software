
def CreateRunMooreOnline_EFF( output, split='' ) :#setup, split='' ) :
    from os import makedirs,chmod
    from os.path import exists, dirname
    
    #print '# INFO: using ' + setup
    print '# INFO: generating '+ output
    target_dir = dirname( output )
    if not exists( target_dir ) : makedirs( target_dir )
    file = open(output,'w+')
    file.write( """#!/bin/sh
ulimit -v 3221225472
export PARENT=$1
export PARTNAME=$2
export NBOFSLAVES=${3:-0}
   
# remove the args because they interfere with the cmt scripts
export HOME=/home/$(/usr/bin/whoami)

. /group/hlt/MOORE/${MOOREONLINE_VERSION}/InstallArea/${CMTCONFIG}/setupMoore.sh;

####echo ${UTGID} Running as $(/usr/bin/whoami) with DIM_DNS_NODE $DIM_DNS_NODE and home $HOME , cmtconfig $CMTCONFIG

export LOGFIFO=/tmp/logGaudi.fifo

# pick up partition specific OnlineEnv module
export PYTHONPATH=/group/online/dataflow/options/${PARTNAME}/HLT:${PYTHONPATH}
# and go for it!
renice 19 -p $$>>/dev/null

# define the task
export gaudi_exe="exec -a ${UTGID} ${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe"

# Setup the environment to checkpoint, restore, run normally etc. by interpreting the vars from OnlineEnv
eval `python -c "import Moore.ConfigureShell as c; c.doIt()"`

export CHECKPOINT_BIN=${CHECKPOINTINGROOT}/${CMTCONFIG};

if test "${APP_STARTUP_OPTS}" = "-normal";         ## RunInfo flag=0
    then
    unset  APP_STARTUP_OPTS;
    unset  CHECKPOINT_DIR;
    unset  CHECKPOINT_FILE;
elif test "${APP_STARTUP_OPTS}" = "-forking";      ## RunInfo flag=1
    then
    unset  APP_STARTUP_OPTS;
    unset  CHECKPOINT_DIR;
    unset  CHECKPOINT_FILE;
    export gaudi_exe="exec -a ${UTGID} ${GAUDIONLINEROOT}/${CMTCONFIG}/GaudiCheckpoint.exe";
    ####echo   "FORKING: gaudi_exe: ${gaudi_exe}"
    export LD_PRELOAD=${CHECKPOINT_BIN}/libCheckpointing.so;
elif test "${APP_STARTUP_OPTS}" = "-checkpoint";   ## Not handled by RunInfo. Used to create checkpoints.
    then
    export gaudi_exe="exec -a ${UTGID} ${GAUDIONLINEROOT}/${CMTCONFIG}/GaudiCheckpoint.exe";
    export PYTHONPATH=${CHECKPOINT_DIR}:${PYTHONPATH};
    export LD_PRELOAD=${CHECKPOINT_BIN}/libCheckpointing.so;
    echo "=============================================================================";
    echo "== File:  ${CHECKPOINT_FILE}"
    echo "== Producing CHECKPOINT file......Please be patient.";
    echo "=============================================================================";
    python -c "import Moore.ConfigureShell as c; c.doIt()";
    echo "=============================================================================";
elif test "${APP_STARTUP_OPTS}" = "-restore";      ## RunInfo flag=2
    then
    unset  APP_STARTUP_OPTS;
    unset  LD_PRELOAD;
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
	    python ${MOOREROOT}/python/Moore/ConfigureFromCheckpoint.py;
	    echo exec -a ${UTGID} ${CHECKPOINT_BIN}/restore.exe -p 4 -e -l /dev/shm/checkpoint -i ${CHECKPOINT_FILE};
	    echo "=============================================================================";
	fi;
	python ${MOOREROOT}/python/Moore/ConfigureFromCheckpoint.py | \\
        exec -a ${UTGID} ${CHECKPOINT_BIN}/restore.exe -p 4 -e -l /dev/shm/checkpoint -i ${CHECKPOINT_FILE};
    else
	echo " [FATAL] =============================================================================";
	echo " [FATAL] == File:  ${CHECKPOINT_FILE}";
	echo " [FATAL] == CHECKPOINT FILE DOES NOT EXIST!";
	echo " [FATAL] =============================================================================";
    fi;
    exit 0;
fi;

${gaudi_exe} ${GAUDIONLINEROOT}/${CMTCONFIG}/libGaudiOnline.so \\
    OnlineTask  \\
    -msgsvc=LHCb::FmcMessageSvc \\
    -tasktype=LHCb::Class1Task  \\
    -main=/group/online/dataflow/templates/options/Main.opts \\
    -opt=command="import Moore.runOnline; Moore.runOnline.start(NbOfSlaves = "${NBOFSLAVES}", Split = '%(split)s', WriterRequires = %(WriterRequires)s )" \\
 ${APP_STARTUP_OPTS};

    """%({'split' : split, 'WriterRequires' : { 'Hlt1' : "[ 'Hlt1' ]" , 'Hlt2' : "[ 'Hlt2' ]" }.get( split, "[ 'HltDecisionSequence' ]" ) } ) )

    from stat import S_IRUSR, S_IRGRP, S_IROTH, S_IWUSR, S_IWGRP, S_IXUSR, S_IXGRP, S_IXOTH 
    import os
    orig = os.stat(output)[0]
    rwxrwxrx = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IXUSR | S_IXGRP | S_IXOTH 
    if orig|rwxrwxrx != orig :
        print '%s has permissions %d -- want %d instead' % (output,orig,orig|rwxrwxrx)
        try :
            chmod(output,rwxrwxrx)
            print 'updated permission of %s'%(output)
        except :
            print 'WARNING: could not update permissions of %s -- please make sure it is executable!' % output
