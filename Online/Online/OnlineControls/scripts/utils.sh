subfarmSetup()
{
    # PVSS 3.8
    cd `dirname $0`/../cmt;
    # PVSS 3.6
    #cd /home/frankm/cmtuser/Gaudi_v19r2/Online/OnlineControls/cmt
    source PVSS.setup.sh
    unset PVSS_II
    export PVSS_PROJECT_HOST=`hostname -s|tr a-z A-Z`
    export PVSS_PROJECT_FARM=`echo ${PVSS_PROJECT_HOST} | cut -b 1-4`
    export PVSS_PROJECT_NAME=REC${PVSS_PROJECT_HOST}
    export PVSS_PROJECT_NUMBER=`grep ${PVSS_PROJECT_NAME} ../doc/ProjectNumbers.txt | awk '{print $2}'`
    export PVSS_II=/localdisk/pvss/${PVSS_PROJECT_NAME}/config/config
}

execOnFarm()
{
    if test -z "${1}";
	then
	   echo "No command name given.";
    else
	for i in A B C D E;
	  do
	  for j in 01 02 03 04 05 06 07 08 09 10 11;
	    do 
	    ${1} HLT${i}${j};
	  done;
	done;
    fi;
}

startLocalSubfarm()
{
    subfarmSetup;
    chmod -R g+rwx /localdisk/pvss/${PVSS_PROJECT_NAME}/
    chmod -R o+rwx /localdisk/pvss/${PVSS_PROJECT_NAME}/
    chmod -R a+rwx /localdisk/pvss/${PVSS_PROJECT_NAME}/
    ${PVSS_SYSTEM_ROOT}/bin/PVSS00pmon -config /localdisk/pvss/${PVSS_PROJECT_NAME}/config/config -autoreg&
}

stopLocalSubfarm()
{
    subfarmSetup;
    ${PVSS_SYSTEM_ROOT}/bin/PVSS00pmon -proj ${PVSS_PROJECT_NAME} -stopWait
    echo "${PVSS_PROJECT_NAME} > Project stopped: ${PVSS_PROJECT_NAME} `date`"   
}

installLocalSubfarm()
{
    subfarmSetup;
    start=`date`;
    echo "${PVSS_PROJECT_NAME} > Installation starting: `date`";
    `which python` ${ONLINECONTROLSROOT}/scripts/Install.py COPY  -project ${PVSS_PROJECT_NAME} -number ${PVSS_PROJECT_NUMBER};
    `which python` ${ONLINECONTROLSROOT}/scripts/Install.py COPY2 -project ${PVSS_PROJECT_NAME} -number ${PVSS_PROJECT_NUMBER};
    chmod -R g+rwx /localdisk/pvss/${PVSS_PROJECT_NAME}/
    chmod -R o+rwx /localdisk/pvss/${PVSS_PROJECT_NAME}/
    chmod -R a+rwx /localdisk/pvss/${PVSS_PROJECT_NAME}/
    echo "${PVSS_PROJECT_NAME} > Installation started: ${start}   finished: `date`";
}

deleteLocalSubfarm()
{
    subfarmSetup;
    /usr/bin/pkill -9 PVSS00api.exe;
    ${PVSS_SYSTEM_ROOT}/bin/PVSS00pmon -proj ${PVSS_PROJECT_NAME} -stopWait
    echo "${PVSS_PROJECT_NAME} > Project stopped: ${PVSS_PROJECT_NAME} `date`"   
    /bin/sleep 3;
    rm -rf /localdisk/pvss/${PVSS_PROJECT_NAME};
    echo "${PVSS_PROJECT_NAME} > Project ${PVSS_PROJECT_NAME} deleted: `date`";
}

sshSubfarm() 
{
    if test -z "${1}";
	then
	echo "No node name given.";
    elif test -z "${2}";
	then
	echo "No command name given.";
    else
	echo "${PVSS_PROJECT_NAME} > Executing: ssh -X ${1} bash ${ONLINECONTROLSROOT}/scripts/utils.sh ${2}";
	ssh -q -f -Y ${1} "bash ${ONLINECONTROLSROOT}/scripts/utils.sh ${2}" &
	#ssh -q -f -Y online@${1} "bash ${ONLINECONTROLSROOT}/scripts/utils.sh ${2}" &
    fi;
}

startSubfarm()
{
    sshSubfarm ${1} startLocalSubfarm;
}

stopSubfarm()
{
    sshSubfarm ${1} stopLocalSubfarm;
}

installSubfarm()
{
    sshSubfarm ${1} installLocalSubfarm;
}

deleteSubfarm()
{
    sshSubfarm ${1} deleteLocalSubfarm;
}

stopFarm()
{
    execOnFarm stopSubfarm
}

startFarm()
{
    execOnFarm startSubfarm
}

installFarm()
{
    execOnFarm installSubfarm
}

deleteFarm()
{
    execOnFarm deleteSubfarm
}

if test -z "${1}";
    then
    echo "No command name given.";
else
    cmd=${1};
    shift;
    ${cmd} $*;
fi;
