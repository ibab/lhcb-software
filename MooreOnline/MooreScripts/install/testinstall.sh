#!/bin/bash
echo "============================================="
echo "TEST SCRIPT: Faking the online deployment structure"
echo "============================================="


export EXPECTV="v21r0" #"HEAD"
export EXPECTONV="v5r6"
export SPopts="" #"--nightly lhcb-trigger-dev"
echo "testing for MooreOnline "${EXPECTV}" with Online "${EXPECTONV}" and extra options: "${SPopts}

if [ "$TMPDIR" == "" ]; then export TMPDIR="/tmp/"${USER}; fi; if [ ! -d "$TMPDIR" ]; then mkdir "$TMPDIR"; fi; cd "$TMPDIR";


if [ -d "satelite" ]; then
  rm -rf satelite
fi;

if [ -d "MOORE" ]; then
  rm -rf MOORE
fi;

if [ ! -d "satelite" ]; then
  mkdir satelite
fi;

if [ ! -d "MOORE" ]; then
  mkdir MOORE
fi;

cd satelite

(export User_release_area=`pwd`; export CMTPATH=${CMTPATH}:`pwd`; . SetupProject.sh Online ${EXPECTONV} --build-env;)

cd ..
echo "============================================="
echo "TEST SCRIPT: now trying to run install script"
echo "============================================="

a=""
if [ -z ${SPopts} ]; then
  echo "Install command to be run"
  echo "./InstallMoore.sh" ${EXPECTV} "--install-location" `pwd`"/MOORE" "--online-satelite" `pwd`"/satelite"
  echo "---------------------------------------------"

  ./InstallMoore.sh ${EXPECTV} --install-location `pwd`/MOORE --online-satelite `pwd`/satelite

else

  echo "Install command to be run"
  echo "./InstallMoore.sh" ${EXPECTV} "--install-location" `pwd`"/MOORE" "--online-satelite" `pwd`"/satelite" "--sp-opts" ${SPopts}
  echo "---------------------------------------------"

  ./InstallMoore.sh ${EXPECTV} --install-location `pwd`/MOORE --online-satelite `pwd`/satelite --sp-opts ${SPopts}

fi

echo "============================================="
echo "TEST SCRIPT: now trying to run Moore test job"
echo "============================================="
cd MOORE
(source ${TMPDIR}"/MOOREONLINE/MooreOnline_"${EXPECTV}"/InstallArea/x86_64-slc6-gcc48-opt/setupMoore.sh"; echo "TCK data at: "${HLTTCKROOT}; gaudirun.py ${MOOREROOT}"/tests/qmtest/moore.qms/physics.qms/2012.qmt" --option="from Configurables import Moore; Moore().EvtMax=1; Moore().OutputLevel=4;")
