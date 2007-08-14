#/bin/sh
#
#
#
if test -z $1;
    then
    echo "No Project name given!";
    exit 0;
fi;
export PVSS_PROJECT_NAME=$1
shift 1;
. ../cmt/setup.sh
#
export PVSS_PROJECT_BASE=/localdisk/pvss/${PVSS_PROJECT_NAME}
export PVSS_II=${PVSS_PROJECT_BASE}/config/config
cp --recursive --force --symbolic-link ${ONLINECONTROLSROOT}/pvss/* ${PVSS_PROJECT_BASE}
#
echo "Importing FSM types for the storage cluster...."
${PVSS_SYSTEM_ROOT}/bin/PVSS00ascii -proj ${PVSS_PROJECT_NAME} -in ${ONLINECONTROLSROOT}/pvss/dplist/${PVSS_PROJECT_NAME}.dpl
#
if test ${PVSS_PROJECT_NAME} = MONITORING;
    then
    echo "Executing PVSS setup controller";
    ${PVSS_SYSTEM_ROOT}/bin/PVSS00ctrl -proj ${PVSS_PROJECT_NAME} InstallMonitoring.cpp;
    #
    echo "Executing python setup";
    python -c "import Online.Streaming.MonitoringInstaller as IM; IM.install('Monitoring')";
    #
    echo "Executing PVSS setup controller";
    ${PVSS_SYSTEM_ROOT}/bin/PVSS00ctrl InstallMonitoring2.cpp;
    #
elif test ${PVSS_PROJECT_NAME} = STORAGE;
    then
    echo "Executing PVSS setup controller";
    ${PVSS_SYSTEM_ROOT}/bin/PVSS00ctrl InstallStorage.cpp;
    #
    echo "Executing python setup";
    python -c "import Online.Streaming.StorageInstaller as IS; IS.install('Storage')";
    #
    echo "Executing PVSS setup controller";
    ${PVSS_SYSTEM_ROOT}/bin/PVSS00ctrl InstallStorage2.cpp;
elif test ${PVSS_PROJECT_NAME} = LBECS;
    then
    echo "Executing PVSS setup controller";
    ${PVSS_SYSTEM_ROOT}/bin/PVSS00ctrl InstallJobOptions.cpp;
    #
    echo "Executing python setup";
    python ${ONLINECONTROLSROOT}/python/InstallLBECS.py;
fi;
