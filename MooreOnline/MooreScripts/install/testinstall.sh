#!/bin/bash
echo "============================================="
echo "TEST SCRIPT: Faking the online deployment structure"
echo "============================================="

export EXPECTV="HEAD"
export EXPECTONV="v5r7"
#options to use the nightly slots... need two of those options which work together
export SPopts="--nightly lhcb-trigger-dev"
export useSPopts=1
echo "testing for MooreOnline "${EXPECTV}" with Online "${EXPECTONV}" and extra options: "${SPopts}
#whether to delete existing directories or not
export fresh=0

startdir=`pwd`

if [ "$TMPDIR" == "" ]; then export TMPDIR="/tmp/"${USER}; fi; if [ ! -d "$TMPDIR" ]; then mkdir "$TMPDIR"; fi; cd "$TMPDIR";


if [ ${fresh} -eq 1 ]; then
    if [ -d "satelite" ]; then
	rm -rf satelite
    fi;


    if [ -d "MOORE" ]; then
	rm -rf MOORE
    fi;
fi

if [ ! -d "satelite" ]; then
    mkdir satelite
fi;

if [ ! -d "MOORE" ]; then
    mkdir MOORE
fi;

cd satelite

if [ $useSPopts -ne 0 ]; then
    (export User_release_area=`pwd`; export CMTPATH=${CMTPATH}:`pwd`; . SetupProject.sh Online ${EXPECTONV} --build-env ${SPopts};)
else
    (export User_release_area=`pwd`; export CMTPATH=${CMTPATH}:`pwd`; . SetupProject.sh Online ${EXPECTONV} --build-env;)
fi

cd ..
echo "============================================="
echo "TEST SCRIPT: now trying to run install script"
echo "============================================="

a=""
if [ ${useSPopts} -eq 0 ]; then
  echo "Install command to be run"
  echo ${startdir}"/InstallMoore.sh" ${EXPECTV} "--install-location" `pwd`"/MOORE" "--online-satelite" `pwd`"/satelite"
  echo "---------------------------------------------"

  ${startdir}/InstallMoore.sh ${EXPECTV} --install-location `pwd`/MOORE --online-satelite `pwd`/satelite 

else

  echo "Install command to be run"
  echo ${startdir}"/InstallMoore.sh" ${EXPECTV} "--install-location" `pwd`"/MOORE" "--online-satelite" `pwd`"/satelite" "--sp-opts" ${SPopts}
  echo "---------------------------------------------"

  ${startdir}/InstallMoore.sh ${EXPECTV} --install-location `pwd`/MOORE --online-satelite `pwd`/satelite --sp-opts ${SPopts}

fi

echo "============================================="
echo "TEST SCRIPT: now trying to run Moore test job"
echo "============================================="
cd MOORE
(source ${TMPDIR}"/MOORE/MooreOnline_"${EXPECTV}"/InstallArea/x86_64-slc6-gcc48-opt/setupMoore.sh"; echo "TCK data at: "${HLTTCKROOT}; gaudirun.py ${MOOREROOT}"/tests/qmtest/moore.qms/physics.qms/silent.qmt" --option="from Configurables import Moore; Moore().EvtMax=1;" | grep -v "List of requested";)
