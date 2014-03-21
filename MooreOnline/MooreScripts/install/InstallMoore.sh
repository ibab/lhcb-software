#!/bin/bash
#
# Moore Installation script for online environment
#   uses SetupProject and getpack to configure and install MooreOnline
#   --help for usage
#
#######################################################
#              STEERING
#######################################################
mooreInstall="/group/hlt/MOORE" # where to put the install directory..., User_release_area
onlineInstall="/group/online/dataflow/cmtuser"
onlineSPOpts=''
#which cmtconfigs to create
thiscmtconfig="x86_64-slc6-gcc48-opt"

version=""

proj="MooreOnline"

locflag=0
onlineflag=0
cmtflag=0
spflag=0
vflag=0

usage="Usage, InstallMoore.sh <version> [options] \noptions: \n --cmtconfig <CMTCONFIG> (default "${thiscmtconfig}", switch CMTCONFIG)\n --install-location <location> (where to install to, default "${mooreInstall}" )\n --online-satelite (where to find the local installation of Online, default "${onlineInstall}" )\n --sp-opts <'options'> (extra options to pass to setup project, default none )"


for arg in $*; do

case "$arg" in
    --help)  echo -e $usage; exit 0 ;;
    --install-location) locflag=1; cmtflag=0; spflag=0 ; onlineflag=0 ;;
    --cmtconfig) cmtflag=1; let locflag=0; spflag=0 ; onlineflag=0 ;;
    --sp-opts) cmtflag=0; locflag=0; spflag=1 ; onlineflag=0 ;;
    --online-satelite) cmtflag=0; locflag=0; spflag=1 ; onlineflag=1 ;;
    *)      if [ $cmtflag -eq 1 ] ; then 
	thiscmtconfig=$arg;
	cmtflag=0
	elif [ $locflag -eq 1 ] ; then
	mooreInstall=$arg;
	locflag=0;
	elif [ $onlineflag -eq 1 ] ; then
	onlineInstall=$arg;
	locflag=0;
	elif [ $spflag -eq 1 ] ; then
	onlineSPOpts=${onlineSPOpts}" "${arg};
	else 
	  if [ $vflag -eq 0 ]; then
	  version=$arg
	  vflag=1
	  else
	      echo -e "# ERROR: Can only install one version, check options! "$version" "$arg >&2
	      echo -e $usage
	      exit 1
	  fi
	fi;;  
esac
done

export actionlist=""
export skippedlist=""

#######################################################
#              Check ARGS
#######################################################

#check...
if [ -z $version ]; then
echo "#ERROR: you must supply a version!" >&2
echo -e $usage
exit 1
fi
if [ ! -d ${mooreInstall} ]; then
echo "#ERROR: Directory does not exist! "${mooreInstall} >&2
echo -e $usage
exit 1
fi
if [ ! -d ${onlineInstall} ]; then
echo "#ERROR: Directory does not exist! "${onlineInstall} >&2
echo -e $usage
exit 1
fi

echo "# INFO: --------------------------------------------"
echo "# INFO: 1) Welcome to the Moore installer. Preamble."
echo "# INFO: --------------------------------------------"
export RUNSCRIPT_PATH=$(cd `dirname "${BASH_SOURCE[0]}"` && pwd)
echo "# INFO: Installer in: "${RUNSCRIPT_PATH}
echo "# INFO: installing "${proj}" "${version}" into "${mooreInstall}" with online satellite from "${onlineInstall}" for cmtconfig "${thiscmtconfig}" with extra sp options: "${onlineSPOpts}

#######################################################
#     Prepare environment and create directory
#######################################################


#set user release area
export User_release_area=$mooreInstall

#setup with cmtconfig
. LbLogin.sh -c $thiscmtconfig 2>&1 | grep -E -i "WARNING|ERROR|"${thiscmtconfig} | grep -v "using environment variable"

#keep paths, don't suppress empty directories
export LB_NO_STRIP_PATH=1
export GAUDI_NO_STRIP_PATH=1

#set user release area
export User_release_area=$mooreInstall

echo "# INFO: installing into" ${mooreInstall}"/"${proj}"_"${version}

needtomake=0

#if directory does exist
if [ -d ${mooreInstall}"/"${proj}"_"${version} ]; then
    echo "# WARNING: directory already exists!" ${mooreInstall}"/"${proj}"_"${version}
    echo "# WARNING: some steps will be skipped because of this..."
fi

#SP
echo "# INFO: SetupProject (in this shell, create initial directories)"
. SetupProject.sh -q ${proj} ${version} ${onlineSPOpts}  --build-env 
rc=$?
if [  $rc -ne 0  ]; then
    exit $rc
fi

#######################################################
#     Find original Moore version
#######################################################

#echo "# INFO: after SP --build-env, now in " `pwd`
moorever=""
#SP without build env, run in a subshell, needed only to get project version
moorever=$(. SetupProject.sh -q ${proj} ${version} ${onlineSPOpts} ; cmt sh projects | grep -e " MOORE MOORE_" | awk '{ print $2 ; }' | awk -F '_' '{ print $2 ; }' )
rc=$?
if [  $rc -ne 0  ]; then
    exit $rc
fi
if [ -z $moorever ]; then
echo "# ERROR, could not detect moore version" >&2 
exit 1
fi

#######################################################
#     Find original online version
#######################################################

#echo "# INFO: after SP --build-env, now in " `pwd`
onver=""
#SP without build env, run in a subshell, needed only to get project version
onver=$(. SetupProject.sh -q ${proj} ${version} ${onlineSPOpts}  ; cmt sh projects | grep -e " ONLINE ONLINE_" | awk '{ print $2 ; }' | awk -F '_' '{ print $2 ; }' )
rc=$?
if [  $rc -ne 0  ]; then
    exit $rc
fi
if [ -z $onver ]; then
echo "# ERROR, (a) could not detect online version" >&2 
exit 1
fi

echo "# INFO: detected online version, "$onver
echo "# INFO: --------------------------------------------"
echo "# INFO: 2) Installation actions from (a) to (i)."
echo "# INFO: --------------------------------------------"
#check that online version exists locally
if [ -d ${mooreInstall}"/Online_"$onver  ]; then
    echo "# WARNING: (a)(1) Online version already exists as a full directory, not doing anything "${mooreInstall}"/Online_"$onver" delete if you wish to recreate it."
    skippedlist=`echo ${skippedlist}"(a) didn't create online softlink because it already existed as a directory"`
elif [ -L ${mooreInstall}"/Online_"$onver ]; then
    echo "# WARNING: (a)(1) Online version already exists as a softlink, not doing anything "${mooreInstall}"/Online_"$onver" delete if you wish to recreate it."
    skippedlist=`echo ${skippedlist}"(a) didn't create online softlink because it already existed as a softlink"`
elif [ ! -d ${onlineInstall}"/Online_"$onver ]; then
    echo "# ERROR (a) the online version does *not yet* have a local satelite area! " ${onlineInstall}"/Online_"$onver >&2
    exit 1
else
    echo "# INFO: (a)(1) Creating link to online satelite area"
    ln -s ${onlineInstall}"/Online_"$onver  ${mooreInstall}"/Online_"$onver
    rc=$?
    if [  $rc -ne 0  ]; then
	exit $rc
    fi
    actionlist=`echo ${actionlist}"(a) created online softlink"`
fi

if [ ${onver} == "HEAD" ]; then
onver=""
fi

#######################################################
#     Prepare with that online version.
#######################################################
echo "# INFO: (a)(2) SetupProject (in this shell with full online env) "
. SetupProject.sh -q ${proj} ${version} ${onlineSPOpts} --runtime-project Online $onver --build-env 
rc=$?
if [  $rc -ne 0  ]; then
    exit $rc
fi

#######################################################
#     Getpack
#######################################################
#detect MooreScripts version.

#Getpack
if  [ -d ${mooreInstall}"/"${proj}"_"${version}"/MooreScripts" ]; then
   echo "# WARNING: (b) MooreScripts already exists, not calling getpack. If you need a different version, do it yourself ;)"
    skippedlist=`echo ${skippedlist}" (b) didn't getpack MooreScripts because it already existed"`
else
   MooreScriptsV=`svnProjectDeps -f MooreScripts ${proj} ${version}  | grep MooreOnline | awk '{print $4}'`
   if [ -z "$MooreScriptsV" ]; then
      echo "# ERROR: (b) MooreScripts version was not detected properly by svnProjectDeps, so I cannot getpack it, try yourself with svnProjectDeps -f MooreScripts "${proj} ${version}  >&2
   fi
   echo "# INFO: (b) getpacking MooreScripts"
   getpack -p anonymous MooreScripts $MooreScriptsV | awk '{ print "# INFO: " $0}'
   rc=$?
   if [  $rc -ne 0  ]; then
       exit $rc
   fi
    actionlist=`echo ${actionlist}" (b) did getpack MooreScripts "${MooreScriptsV}`
fi

#######################################################
#     Sort HLTTCK if required
#######################################################
echo "# INFO: (c) Updating HltTCK"
whereto=${mooreInstall}"/"${proj}"_"${version}"/InstallArea/manifest"
cd ${mooreInstall}"/"${proj}"_"${version}
if [ ! -d ${mooreInstall}"/"${proj}"_"${version}"/TCK/HltTCK" ]; then
   getpack -p anonymous TCK/HltTCK head #| awk '{ print "# INFO: " $0}'
   actionlist=`echo ${actionlist}" (c) did getpack HltTCK"`
else
   svn update TCK/HltTCK #| awk '{ print "# INFO: " $0}'
   skippedlist=`echo ${skippedlist}" (c) didn't getpack HltTCK because it already existed"`

   actionlist=`echo ${actionlist}" (c) updated HltTCK"`
fi


#######################################################
#     make install directories
#######################################################
echo "# INFO: ------------- (d) Environment file --------------"
if [ ! -d ${mooreInstall}"/"${proj}"_"${version}"/InstallArea" ]; then
  echo "# INFO: (d)(1) Creating InstallArea directory"
  mkdir ${mooreInstall}"/"${proj}"_"${version}"/InstallArea"
  needtomake=1
fi
if [ ! -d ${mooreInstall}"/"${proj}"_"${version}"/InstallArea/"${thiscmtconfig} ]; then
  echo "# INFO: (d)(2) Creating cmtconfig directory"
  mkdir ${mooreInstall}"/"${proj}"_"${version}"/InstallArea/"${thiscmtconfig}
  needtomake=1
fi


#######################################################
#     Create environment file
#######################################################
#SP to create the initial environment file, needs editing.
#where to put it?
whereto=${mooreInstall}"/"${proj}"_"${version}"/InstallArea/"${thiscmtconfig}"/setupMoore.sh"
wheretoRun=${mooreInstall}"/"${proj}"_"${version}"/InstallArea/"
setupmoore=$whereto

if [ -e $whereto ]; then
   echo "# WARNING (d) environment file already exists! Delete if you want to overwrite! "${whereto}
   skippedlist=`echo ${skippedlist}" (d) didn't create env file because it already existed"`
else
   #use python module directly, same as SetupProject really.
   echo "# INFO: (d)(3) Create environment file "
   needtomake=1
   echo "# INFO: python -mLbConfiguration.SetupProject" ${proj} ${version} ${onlineSPOpts}  "--runtime-project Online"  ${onver} "--output="${whereto} "--shell=sh"
   (python -mLbConfiguration.SetupProject ${proj} ${version} ${onlineSPOpts}  --runtime-project Online ${onver} --output=${whereto} --shell=sh | awk '{ print "# INFO: " $0}')


   if [ ! -e $whereto ]; then
       echo "# ERROR (d) environment file was not created! "${whereto} >&2
       exit 1
   fi
   echo "# INFO: (d) Environment file exists as "${whereto}
   actionlist=`echo ${actionlist}" (d) created env file"`
   echo "# INFO: (d)(4) Edit the file I created "
   (source ${setupmoore} 2>&1 > /dev/null; python -c "execfile('"${mooreInstall}"/"${proj}"_"${version}"/MooreScripts/python/MooreScripts/CreateSetup.py'); FixSetupMoore('"${whereto}"');";)
   rc=$?
   if [  $rc -ne 0  ]; then
    echo "# ERROR (d) environment file was not edited, I am deleting it to protect against a broken file! "${whereto} >&2
    rm ${whereto}
    exit $rc
   fi
   actionlist=`echo ${actionlist}", edited env file"`
fi


#######################################################
#    Make and sort HltTCK
#######################################################
if  [ ! -d ${mooreInstall}"/"${proj}"_"${version}"/InstallArea/"${thiscmtconfig}"/python" ]; then
  needtomake=1
fi
if [ ! -d ${mooreInstall}"/"${proj}"_"${version}"/InstallArea/"${thiscmtconfig}"/lib" ]; then
  needtomake=1
fi

if  [ $needtomake -ne 1 ]; then
   echo "# WARNING: (e) InstallArea/cmtconfig already exists, not calling a full make. Do it manually if you want ;)"
   skippedlist=`echo ${skippedlist}" (e) didn't make because InstallArea/cmtconfig already existed"`
   #echo "# INFO: --------- MAKE HltTCK ONLY --------"
   #cd TCK/HltTCK/cmt
   #(source ${setupmoore}; cmt make;)
   #cd ../../..
   #actionlist=`echo ${actionlist}", made HltTCK only,"`
else
   echo "# INFO: --------- (e) MAKE EVERYTHING ---------"
   (source ${setupmoore};  make;)
   actionlist=`echo ${actionlist}" (e) make"`
   #rc=$?
   #if [  $rc -ne 0  ]; then
   #    exit $rc
   #fi
fi

#######################################################
#     update permissions
#######################################################
cd TCK/HltTCK/cmt
(source ${setupmoore}; cmt fixPermissions;)
cd ../../..
#######################################################
#     Make the manifest only if required!
#######################################################
ls -l TCK/HltTCK/config.tar TCK/HltTCK/scripts/* > 'InstallArea/checknewmanifest'
needtodomanifest="True"
if [ ! -d TCK/HltTCK/manifest ]; then
    needtodomanifest="True"
    #echo "no manifest dir"
elif [ -f "InstallArea/lastsawmanifest" ]; then
  #test that everything apart from the manifest directory is the same!
  needtodomanifest=`python -c "f1=open('InstallArea/lastsawmanifest'); f2=open('InstallArea/checknewmanifest'); f1l=f1.readlines(); f2l=f2.readlines(); f1.close(); f2.close(); print (f1l!=f2l);"`
    #echo "after the check..."${needtodomanifest}"..."
fi

if [ ${needtodomanifest} = "False" ]; then
    echo "# INFO: (f) no need to re-make manifest"
    skippedlist=`echo ${skippedlist}" (f) no need to remake manifest"`
else
    echo "# INFO: (f) make manifest"
    actionlist=`echo ${actionlist}" (f) made manifest"`
    cd TCK/HltTCK/cmt
    (source ${setupmoore}; cmt createManifest;)
    cd ../../..
    ls -l TCK/HltTCK/config.tar TCK/HltTCK/scripts/* > 'InstallArea/lastsawmanifest'
fi
#######################################################
#     copy the manifest
#######################################################
#parse minor version with simple bit of python
mooremin=`python -c "print '"${moorever}"'.split('p')[0]"`
whereto=${mooreInstall}"/"${proj}"_"${version}"/InstallArea/manifest"
if [ -e ${mooreInstall}"/"${proj}"_"${version}"/TCK/HltTCK/manifest/MOORE_"${mooremin} ]; then
   if [ -e ${whereto} ]; then
     echo "# INFO: (g) manifest already exists, replacing."
     rm $whereto
   else
     echo "# INFO: (g) installing manifest"
   fi
   cp ${mooreInstall}"/"${proj}"_"${version}"/TCK/HltTCK/manifest/MOORE_"${mooremin} ${whereto}
   actionlist=`echo ${actionlist}" (g) copied manifest"`
else
   echo "# WARNING: (g) could not install manifest, because no TCKs available for "${mooremin}
   skippedlist=`echo ${skippedlist}" (g) didn't copy manifest because it did not exist for this Moore version"`
fi
cd $mooreInstall

#######################################################
#     Dump the project information into a file 
#######################################################

whereto=$mooreInstall"/"${proj}"_"${version}"/InstallArea/project_versions.txt"
if [ -e $whereto ]; then
   echo "# WARNING (h) project_versions.txt linking file already exists! Delete if you want to overwrite"
    skippedlist=`echo ${skippedlist}" (h) didn't create project_versions.txt because it already existed"`
else
   echo "# INFO: (h) creating project_versions.txt"
   echo -e "Moore "${moorever}"\nOnline "${onver}"\n" > ${whereto}
   actionlist=`echo ${actionlist}" (h) created project_versions.txt"`
fi
if [ ! -e $whereto ]; then
   echo "# ERROR (h) failed to create project/version file"
   exit 1
fi
#make sure it is only readable by the user
chmod 444 $whereto


echo "# INFO: Final action, (i) PostInstall script"
cd ${mooreInstall}"/"${proj}"_"${version}
(source ${setupmoore} ; ./MooreScripts/scripts/PostInstall.py ${wheretoRun})
actionlist=`echo ${actionlist}" (i) ran post install script"`

echo "# INFO: ---------------------------------------------"
echo "# INFO: 3) Summary and Checks"
echo "# INFO: ---------------------------------------------"
echo "# INFO: Actions I did: "${actionlist}
if [ -n "$skippedlist" ] ; then
  echo "# WARNING: Actions I skipped: "${skippedlist}
fi
#check project
if [ -d ${mooreInstall}"/"${proj}"_"${version} ]; then
  echo "# INFO: MooreOnline exists as "${mooreInstall}"/"${proj}"_"${version}
else
  echo "# ERROR: No installation found at "${mooreInstall}/${proj}_${version}
fi
#check cmt
if [ -d ${mooreInstall}"/"${proj}"_"${version}"/InstallArea/"${thiscmtconfig} ]; then
  echo "# INFO: MooreOnline compiled for "${thiscmtconfig}
else
  echo "# ERROR: No compilation for "${thiscmtconfig}
fi
#check versions file
echo "# INFO: detected Online "${onver}" Moore "${moorever}" ("${mooremin}")"
if [ -e ${mooreInstall}"/"${proj}"_"${version}"/InstallArea/project_versions.txt" ]; then
  echo "# INFO:  stored in "${mooreInstall}"/"${proj}"_"${version}"/InstallArea/project_versions.txt, reading"
  cat ${mooreInstall}"/"${proj}"_"${version}"/InstallArea/project_versions.txt" | grep -v "^$"
else
  echo "# ERROR: No project_versions.txt found at "${mooreInstall}"/"${proj}"_"${version}"/InstallArea"
fi
# check online link
if [ -L ${mooreInstall}"/Online_"${onver} ]; then
  echo "# INFO:  Online version is configured via symlink:"
  ls -lh ${mooreInstall}"/Online_"${onver}
  if [ ! -h ${mooreInstall}"/Online_"${onver} ]; then
    echo "# ERROR: the Online symlink does not point to a valid location"
  fi
elif [ -d ${mooreInstall}/"Online"${onver} ]; then
  echo "# WARNING: local online version is a directory, should be a symlink"
else
  echo "# ERROR: No local online project exists."
fi
#check HLTTCK
if [ -d ${mooreInstall}"/"${proj}"_"${version}"/TCK/HltTCK" ]; then
  echo "# INFO:  HltTCK exists as "${mooreInstall}"/"${proj}"_"${version}"/TCK/HltTCK"
else
  echo "# ERROR: No HltTCK found at "${mooreInstall}"/"${proj}"_"${version}"/TCK/HltTCK"
fi
#check environment file
if [ -e ${mooreInstall}"/"${proj}"_"${version}"/InstallArea/"${thiscmtconfig}"/setupMoore.sh" ]; then
  echo "# INFO: environment file created at: "${mooreInstall}"/"${proj}"_"${version}"/InstallArea/"${thiscmtconfig}"/setupMoore.sh"
  #example path
  (source ${mooreInstall}"/"${proj}"_"${version}"/InstallArea/"${thiscmtconfig}"/setupMoore.sh" 2>&1 > /dev/null; python $RUNSCRIPT_PATH/checkPath.py LD_LIBRARY_PATH ;  python $RUNSCRIPT_PATH/checkPath.py PYTHONPATH ; echo "# INFO: HLTTCKROOT="$HLTTCKROOT;)
else
  echo "# ERROR: No environment file at "${mooreInstall}"/"${proj}"_"${version}"/InstallArea/"${thiscmtconfig}"/setupMoore.sh"
fi
echo "# INFO: The following 'online running' files exist:"
ls ${mooreInstall}"/"${proj}"_"${version}"/InstallArea/run"*
#check manifest
if [ -e ${mooreInstall}"/"${proj}"_"${version}"/InstallArea/manifest" ]; then
  echo "# INFO:  TCK manifest exists as "${mooreInstall}"/"${proj}"_"${version}"/InstallArea/manifest"
else
  echo "# ERROR: No TCK manifest created, probably there were no TCKs defined yet for this version, run this script again after TCKs have been defined "${mooreInstall}"/"${proj}"_"${version}"/InstallArea/manifest"
fi
