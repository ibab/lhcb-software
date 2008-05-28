#!/bin/sh -f
#---------------------------------------------------------------------------
# setup Project environment
#===========================================================================
# P1 = Project name
# P2 if given = Project version number
#===========================================================================
#set -x

here=`pwd`
nar=$#
unset CMTUSERCONTEXT

hat=`echo $1 | awk -F/ '{print $1}'`
Project=`echo $1 | awk -F/ '{print $2}'` ; shift
if [ "${Project}" = "" ]; then
  Project=$hat
  unset hat
fi
PROJECT=`echo $Project | tr '[a-z]' '[A-z]'`

if [ "${Project}" = "Gaudi" ]; then
   Project_release_area=${Gaudi_release_area}
else
   Project_release_area=${LHCb_release_area}
fi

# get the project version
Pvers=''
if [ ${nar} = 2 ]; then
  Pvers=$1 ; shift
else
  echo "you must give a project version:"
  vlist=`/bin/ls -trd ${Project_release_area}/${PROJECT}/${PROJECT}_v*`
  tmp_npos=0
  echo -n "Please enter your choice ("
  for v in $vlist
  do
    tmp_version=`echo $v | awk -F_ '{print $NF}'`
    tmp_npos=`expr $tmp_npos + 1`
    /bin/ls  ${Project_release_area}'/'${PROJECT}'/'${PROJECT}_$tmp_version > /dev/null 2>&1 && echo -n "$tmp_version " && tmp_last=$tmp_version
  done
  echo -n "q[uit] [$tmp_last]): "
  read tmp_choice

  case $tmp_choice in
    q)
      unset tmp_choice; return 1
      ;;
    "")
      tmp_choice=$tmp_last
   esac
   Pvers=$tmp_choice
fi

# if <Project>Env/<version> exists then use it
if [ -d ${Project_release_area}/${Project}Env/${Pvers} ]; then
   unset CMTPROJECTPATH
   if [ -f $PWD/cmt/project.cmt ]; then
      use=`grep -c use $PWD/cmt/project.cmt`
      if [ $use -ge 0 ]; then
        cd $User_release_area
      fi
   fi
   if [ x"$CMTPATH" = x ]; then
      export CMTPATH=$User_release_area
   fi
   . ${Project_release_area}/${Project}Env/${Pvers}/cmt/setup.sh
   echo " --------------------------------------------------------------------"
   echo " --- ${Project} environment is set for version $Pvers "
   echo " --- packages will be searched for in CMTPATH: "
   echo " $CMTPATH"
   echo " --------------------------------------------------------------------"

else

# <Project>Env/<version> does not exist then use CMTPROJECTPATH
   unset CMTPATH
   if [ ! ${CMTPROJECTPATH} ]; then
      CMTPROJECTPATH="${User_release_area}:${LHCb_release_area}:${Gaudi_release_area}:${LCG_release_area}"; export CMTPROJECTPATH
   fi
   echo " --------------------------------------------------------------------"
   echo " --- ${Project} environment is set for version $Pvers "
   echo " --- packages will be searched for in CMTPROJECTPATH: "
   echo " $CMTPROJECTPATH"
   echo " --------------------------------------------------------------------"

   if [ ! -d ${User_release_area}/${Project}_${Pvers} ]; then
      cd ${User_release_area}
      cmt create_project ${Project}_${Pvers} -use=${PROJECT}:${PROJECT}_${Pvers} 
   fi
   cd ${User_release_area}/${Project}_${Pvers}/cmt
#   cmt show projects
   cd ..
fi

#========================================================================
if [ -d ${LHCBHOME}/project/logfiles ]; then
  touch $LHCBHOME/project/logfiles/${PROJECT}_${Pvers}_$USER
fi

unset Pvers
unset Project
unset PROJECT
unset hat
unset here
unset v
unset vlist
unset use
set -

