#/bin/sh
none=1
#
#
export PVSS_PROJECT_NAME=
export PVSS_INSTALL_NAME=
export PVSS_COMPONENTS_DIR=
#
#
vals=`python <<EE_EOF
args = '$*'.split(' ')
i=0
while(i<len(args)):
  a = str(args[i]).upper()[:2]
  if a=='-P':
    print 'export PVSS_PROJECT_NAME='+args[i+1]+'; shift 1; shift 1;'
    i = i + 2
  elif a=='-I':
    print 'export PVSS_INSTALL_NAME='+args[i+1]+'; shift 1; shift 1;'
    i = i + 2
  elif a=='-C':
    print 'export PVSS_COMPONENTS_DIR='+args[i+1]+'; shift 1; shift 1'
    i = i + 2
  else:
    print 'echo "Unknown option:'+args[i]+'";'
    print 'exit 1;'
    break
EE_EOF
`
echo $vals
eval $vals
if test -z "$PVSS_PROJECT_NAME";
    then
    echo "No Project name given!";
    echo "usage: Install.sh -project <project-name> <install_name>"
    echo "  -project <name>: Choose one of: STORAGE MONITORING LBECS"
    echo "  -install <name>: Choose one of: JOBOPTIONS"
    exit 0;
fi;
export PVSS_PROJECT_BASE=/localdisk/pvss/${PVSS_PROJECT_NAME}
export PVSS_II=${PVSS_PROJECT_BASE}/config/config
#
if test -n "$PVSS_INSTALL_NAME";
    then
    echo "PVSS Install name: ${PVSS_INSTALL_NAME}"
fi;
if test -n "$PVSS_COMPONENTS_DIR";
    then
    echo "Install files to directory: ${PVSS_COMPONENTS_DIR}"
else
    export PVSS_COMPONENTS_DIR=${PVSS_PROJECT_BASE}
fi;

. ../cmt/setup.sh
export PVSS_II="/localdisk/pvss/${PVSS_PROJECT_NAME}/config/config"
export PVSS_PROJECT_BASE=/localdisk/pvss/${PVSS_PROJECT_NAME}
#
cd ${ONLINECONTROLSROOT}/pvss
#

if test "${none}" = 0;
    then
    echo "No-work flag switched on....";
elif test "${PVSS_INSTALL_NAME}" = JOBOPTIONS;
    then
    echo "Executing PVSS setup controller";
    echo "Extracting FSM types for project ${PVSS_PROJECT_NAME}"
    ${PVSS_II_HOME}/bin/PVSS00ascii \
	-proj ${PVSS_PROJECT_NAME} \
	-out  ${ONLINECONTROLSROOT}/pvss/dplist/JobOptionsControl.dpl \
	-filter DOP -filterDp fwOT_JobOptionsControl.*
elif test ${PVSS_PROJECT_NAME} = STORAGE -o ${PVSS_PROJECT_NAME} = MONITORING;
    then
    echo "Extracting FSM types for project ${PVSS_PROJECT_NAME}"
    ${PVSS_II_HOME}/bin/PVSS00ascii \
	-proj ${PVSS_PROJECT_NAME} \
	-out  ${ONLINECONTROLSROOT}/pvss/dplist/${PVSS_PROJECT_NAME}.dpl \
	-filter DOP \
	-filterDp fwOT_StreamCluster.* \
	-filterDp fwOT_StreamControl.* \
	-filterDp fwOT_StreamConfigurator.* \
	-filterDp fwOT_JobOptionsControl.* \
	-filterDp fwOT_FSM_*.*;
    echo Omit: ${PVSS_II_HOME}/bin/PVSS00ascii \
	-proj ${PVSS_PROJECT_NAME} \
	-out  ${ONLINECONTROLSROOT}/pvss/dplist/ProcessorFarm.dpl \
	-filter DOP \
	-filterDp fwOT_StreamConfigurator.* \
	-filterDp fwOT_FSM_Slice.* \
	-filterDp fwOT_FSM_Tasks.* \
	-filterDp fwOT_FSM_DimTask.* ;
elif test ${PVSS_PROJECT_NAME} = RECHLTA01 -o ${PVSS_PROJECT_NAME} = RECHLTB01;
    then
    echo "Extracting FSM types for project ${PVSS_PROJECT_NAME}"
    ${PVSS_II_HOME}/bin/PVSS00ascii \
	-proj ${PVSS_PROJECT_NAME} \
	-out  ${ONLINECONTROLSROOT}/pvss/dplist/ProcessorFarm.dpl \
	-filter DOP \
	-filterDp fwOT_StreamConfigurator.* \
	-filterDp fwOT_FSM_Slice.* \
	-filterDp fwOT_FSM_Tasks.* \
	-filterDp fwOT_FSM_DimTask.* ;
elif test ${PVSS_PROJECT_NAME} = RECOTEST -o ${PVSS_PROJECT_NAME} = RECFARM;
    then
    echo "Extracting FSM types for project ${PVSS_PROJECT_NAME}"
    ${PVSS_II_HOME}/bin/PVSS00ascii \
	-proj ${PVSS_PROJECT_NAME} \
	-out  ${ONLINECONTROLSROOT}/pvss/dplist/${PVSS_PROJECT_NAME}.dpl \
	-filter DOP \
	-filterDp fwOT_StreamCluster.* \
	-filterDp fwOT_StreamControl.* \
	-filterDp fwOT_StreamConfigurator.* \
	-filterDp fwOT_JobOptionsControl.* \
	-filterDp fwOT_FSM_*.*;
elif test ${PVSS_PROJECT_NAME} = RECMONA08;
    then
    echo "Extracting FSM types for project ${PVSS_PROJECT_NAME}"
    ${PVSS_II_HOME}/bin/PVSS00ascii \
	-proj ${PVSS_PROJECT_NAME} \
	-out  ${ONLINECONTROLSROOT}/pvss/dplist/ProcessorFarm.dpl \
	-filter DOP \
	-filterDp fwOT_StreamConfigurator.* \
	-filterDp fwOT_FSM_Slice.* \
	-filterDp fwOT_FSM_Tasks.* \
	-filterDp fwOT_FSM_DimTask.* ;
elif test ${PVSS_PROJECT_NAME} = RECCTRL;
    then
    echo "Extracting FSM types for project ${PVSS_PROJECT_NAME}"
    ${PVSS_II_HOME}/bin/PVSS00ascii \
	-proj ${PVSS_PROJECT_NAME} \
	-out  ${ONLINECONTROLSROOT}/pvss/dplist/${PVSS_PROJECT_NAME}.dpl \
	-filter DOP \
	-filterDp fwOT_StreamCluster.* \
	-filterDp fwOT_StreamControl.* \
	-filterDp fwOT_StreamConfigurator.* \
	-filterDp fwOT_JobOptionsControl.* \
	-filterDp fwOT_FSM_*.*;
    ${PVSS_II_HOME}/bin/PVSS00ascii \
	-proj ${PVSS_PROJECT_NAME} \
	-out  ${ONLINECONTROLSROOT}/pvss/dplist/${PVSS_PROJECT_NAME}.Types.dpl \
        -filter T \
	-filterDpType FSM_Alloc \
	-filterDpType FSM_FarmSimu \
	-filterDpType AllocFarmInfo \
	-filterDpType AllocInfo ;
elif test ${PVSS_PROJECT_NAME} = LBECS;
    then
    echo "Extracting data for project ${PVSS_PROJECT_NAME}"
    ${PVSS_II_HOME}/bin/PVSS00ascii \
	-proj ${PVSS_PROJECT_NAME} \
	-out  ${ONLINECONTROLSROOT}/pvss/dplist/${PVSS_PROJECT_NAME}.dpl \
	-filter T \
	-filterDpType RunInfo;
fi;
