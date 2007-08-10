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
#
if test -z $1;
    then
    echo "No Project name given!";
    exit 0;
fi;
export PVSS_PROJECT_NAME=$1
shift 1;
. ../cmt/setup.sh
export PVSS_II="/localdisk/pvss/${PVSS_PROJECT_NAME}/config/config"
export PVSS_PROJECT_BASE=/localdisk/pvss/${PVSS_PROJECT_NAME}
#
cd ${ONLINECONTROLSROOT}/pvss
#
checkFiles ${PVSS_PROJECT_BASE} scripts/libs *.cpp
checkFiles ${PVSS_PROJECT_BASE} panels/JobOptions *.pnl
checkFiles ${PVSS_PROJECT_BASE} panels/StreamControl *.pnl
#
if test ${PVSS_PROJECT_NAME} = STORAGE -o ${PVSS_PROJECT_NAME} = MONITORING;
    then
    echo "Extracting FSM types for project ${PVSS_PROJECT_NAME}"
    ${PVSS_II_HOME}/bin/PVSS00ascii \
	-proj ${PVSS_PROJECT_NAME} \
	-out  ${ONLINECONTROLSROOT}/pvss/dplist/${PVSS_PROJECT_NAME}.dpl \
	-filter DOP \
	-filterDp fwOT_StorageCluster.* \
	-filterDp fwOT_StreamControl.* \
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
