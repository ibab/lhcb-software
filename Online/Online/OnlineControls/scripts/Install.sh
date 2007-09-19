#/bin/sh
#
#
. ../scripts/InstallFiles.sh $*
#
if test -e ${ONLINECONTROLSROOT}/pvss/dplist/${PVSS_PROJECT_NAME}.dpl;
then
  echo "Importing FSM types ...."
  ${PVSS_SYSTEM_ROOT}/bin/PVSS00ascii -proj ${PVSS_PROJECT_NAME} -in ${ONLINECONTROLSROOT}/pvss/dplist/${PVSS_PROJECT_NAME}.dpl
else
  echo "No datapoints to be imported ...."
fi;
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
elif test ${PVSS_INSTALL_NAME} = JOBOPTIONS;
    then
    ${PVSS_SYSTEM_ROOT}/bin/PVSS00ascii -proj ${PVSS_PROJECT_NAME} -in ${ONLINECONTROLSROOT}/pvss/dplist/JobOptionsControl.dpl
    echo "Executing PVSS setup controller";
    ${PVSS_SYSTEM_ROOT}/bin/PVSS00ctrl InstallJobOptionsControl.cpp;
fi;
