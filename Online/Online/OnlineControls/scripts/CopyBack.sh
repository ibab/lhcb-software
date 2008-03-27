#/bin/sh
checkFiles()
{
  for i in `ls $2/$3`; 
    do
    #if  test -n "`diff $1/$i $i`";
    if test -L $1/$i;
	then 
	echo "---> Unchanged [Link]: $i";
    elif test -z "`diff $1/$i $i`";
	then 
	echo "---> Unchanged [File]: $i";
	rm $1/$i
	ln -s ${ONLINECONTROLSROOT}/pvss/$i $1/$i
    elif test $1/$i -nt $i;
	then 
	echo "---> CHANGED:   $i";
	cp --preserve=context $1/$i $i; 
    else
	echo "---> Unchanged: $i";
    fi;
  done;
}
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
checkFiles ${PVSS_PROJECT_BASE} scripts *.cpp
checkFiles ${PVSS_PROJECT_BASE} scripts/libs *.cpp
checkFiles ${PVSS_PROJECT_BASE} panels/Reco *.pnl
checkFiles ${PVSS_PROJECT_BASE} panels/ProcessorFarm *.pnl
checkFiles ${PVSS_PROJECT_BASE} panels/JobOptions *.pnl
checkFiles ${PVSS_PROJECT_BASE} panels/StreamControl *.pnl
checkFiles ${PVSS_PROJECT_BASE} panels/visionUtils *.pnl
checkFiles ${PVSS_PROJECT_BASE} panels/fwFSMuser *.pnl
checkFiles ${PVSS_PROJECT_BASE} panels/fwFSM/ui *.pnl
#
if test "${PVSS_INSTALL_NAME}" = JOBOPTIONS;
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
elif test ${PVSS_PROJECT_NAME} = RECOTEST;
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
elif test ${PVSS_PROJECT_NAME} = LBECS;
    then
    echo "Extracting data for project ${PVSS_PROJECT_NAME}"
    ${PVSS_II_HOME}/bin/PVSS00ascii \
	-proj ${PVSS_PROJECT_NAME} \
	-out  ${ONLINECONTROLSROOT}/pvss/dplist/${PVSS_PROJECT_NAME}.dpl \
	-filter T \
	-filterDpType RunInfo;
fi;
