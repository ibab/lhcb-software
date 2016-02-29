#!/bin/sh
ulimit -v 3221225472
export UTGID;
export DIM_DNS_NODE=$1
export PARTITION_NAME=$2
export NBOFSLAVES=${3:-0}
# remove the args because they interfere with the cmt scripts
export HOME=/home/$(/usr/bin/whoami)
#
. /group/hlt/MOORE/${MOOREONLINE_VERSION}/InstallArea/${CMTCONFIG}/setupMoore.sh;
#
# Enable next line for Debug printout
#python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py -r ${RUNINFO} -s;
eval `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py -r ${RUNINFO} -s -t Moore2`;
renice -n 19 -p $$>>/dev/null
#
if test "${MOORESTARTUP_MODE}" = "RESTORE";      ## RunInfo flag=2
    then
    # Enable next line for Debug printout
    #python ${FARMCONFIGROOT}/job/ConfigureFromCheckpoint.py;
    #echo "RESORE command: ${RESTORE_CMD}";
    eval "python ${FARMCONFIGROOT}/job/ConfigureFromCheckpoint.py | ${RESTORE_CMD}";
else
    exec -a ${UTGID} GaudiCheckpoint.exe libGaudiOnline.so OnlineTask \
	-msgsvc=LHCb::FmcMessageSvc \
        -tasktype=LHCb::Class1Task \
	-main=/group/online/dataflow/templates/options/Main.opts \
	-opt=command="import MooreScripts.runOnline; MooreScripts.runOnline.start(NbOfSlaves = "${NBOFSLAVES}", Split = 'Hlt2', CheckOdin = False,  WriterRequires = [ 'Hlt2' ]  )" \
	${APP_STARTUP_OPTS};
fi;
