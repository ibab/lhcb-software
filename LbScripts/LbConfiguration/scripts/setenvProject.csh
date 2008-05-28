#!/bin/csh -f
#---------------------------------------------------------------------------
# setup Project environment
#===========================================================================
# P1 = Project name
# P2 if given = Project version number
#===========================================================================
#set echo on
unsetenv CMTUSERCONTEXT

set nargs   = $#argv
set hat     = `echo $1 | awk -F/ '{print $1}'`
set Project = `echo $1 | awk -F/ '{print $2}'` ; shift
if ( $Project == "") then
  set Project = $hat
  unset hat
endif

if ($Project == "Gaudi") then
  set Project_release_area = ${Gaudi_release_area}
else
  set Project_release_area = ${LHCb_release_area}
endif 
set PROJECT = `echo $Project | tr '[a-z]' '[A-z]'`

set Pvers = ""
if( $nargs >= 2 ) then  
  set Pvers = $1;shift
else
  echo "You must give a project version:"
  set tmp_versions=(`eval /bin/ls -ltr ${Project_release_area}'/'${PROJECT} | sed -e '/.*'${PROJECT}'_v[0-9][0-9]*r[0-9][0-9]*$/'\!\d -e 's/.*'${PROJECT}'_//' | tr '\012' '\011'`)

  echo -n "Please enter your choice ("

  set tmp_npos=0
  foreach tmp_version ($tmp_versions[*])
    @ tmp_npos++
    /bin/ls ${Project_release_area}'/'${PROJECT}'/'${PROJECT}_$tmp_version >&! /dev/null && echo -n "$tmp_version " && set tmp_nlast=$tmp_npos
  end
  set tmp_last=${tmp_versions[$tmp_nlast]}
  echo -n "q[uit] [$tmp_last]): "
  set tmp_choice="$<"

  switch ( $tmp_choice )
    case q:
      unset tmp_choice; exit
      breaksw
    case "":
      set tmp_choice=$tmp_last
      breaksw
   endsw
   set Pvers = $tmp_choice
endif

if( -d ${Project_release_area}/${Project}Env/${Pvers} ) then
    unsetenv CMTPROJECTPATH 
    if ( -f $PWD/cmt/project.cmt ) then  
      set use = `grep -c use $PWD/cmt/project.cmt`
      if ($use > 0 )cd $User_release_area
    endif
    if ( ! $?CMTPATH ) then
    	setenv CMTPATH $User_release_area
    endif
    source ${Project_release_area}/${Project}Env/${Pvers}/cmt/setup.csh
    echo " --------------------------------------------------------------------"
    echo " --- ${Project} environment is set for version $Pvers "
    echo " --- packages will be searched for in CMTPATH: "
    echo " $CMTPATH"
    echo " --------------------------------------------------------------------"
    goto exec_end
endif

if( ! $?CMTPROJECTPATH ) then
  setenv CMTPROJECTPATH ${User_release_area}:${LHCb_release_area}:${Gaudi_release_area}:${LCG_release_area}
endif
echo " --------------------------------------------------------------------"
echo " --- ${Project} environment is set for version $Pvers "
echo " --- packages will be searched for in CMTPROJECTPATH: "
echo " $CMTPROJECTPATH"
echo " --------------------------------------------------------------------"

#echo " using CMTPROJECTPATH $CMTPROJECTPATH"
unsetenv CMTPATH
if !(-d ${User_release_area}/${Project}_${Pvers}) then
   cd ${User_release_area}
   cmt create_project ${Project}_${Pvers} -use=${PROJECT}:${PROJECT}_${Pvers} 
endif
#unsetenv CMTPATH
cd ${User_release_area}/${Project}_${Pvers}/cmt
#cmt show projects
cd ..
#========================================================================
exec_end:
#unset echo
if (-d $LHCBHOME/project/logfiles) then
   touch $LHCBHOME/project/logfiles/${PROJECT}_${Pvers}_$USER
endif
unset Pvers Project PROJECT hat 
unset tmp_choice tmp_last tmp_nlast tmp_npos tmp_version tmp_versions
exit

