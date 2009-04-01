############################# ProjectEnv.csh #############################
#-------------------------------------------------------------------------
# set project environment
# for a shell with a C-like syntax (C shell, tcsh)
# Usage: source ProjectEnv.csh <project> [v<version#>r<release#>]
#-------------------------------------------------------------------------
# Checks whether the version entered (if any) is available
# and, if needed, offers the choice of the versions available
# (with the default being the most recent version)
#-------------------------------------------------------------------------
# 050830 - remove : in front of CMTPATH if any
# 060106 - goto exec_end in case of q(uit)
#          remove check 0n existing directory to accept soft link to directory
#==========================================================================
#set echo on 

set nar = $#argv
#
if ( $?LHCBHOME ) then

set tmp_project=$1; shift
set tmp_PROJECT=`echo ${tmp_project} | tr '[a-z]' '[A-Z]'`

eval set tmp_isset='$?'${tmp_project}_release_area
if ( ! $tmp_isset ) then
switch ( $tmp_project )
case Gaudi:
  setenv ${tmp_project}_release_area $GAUDISOFT
  breaksw
case *:
  setenv ${tmp_project}_release_area $LHCBRELEASES
  breaksw
endsw
endif

# project version can be given as 2nd argument or/and "+" <project>
set tmp_choice = ""
set tmp_plus = "" 
set tmp_plus_choice = ""

if ($nar >= 2) then
   if ( "$1" == "+") then
     shift
     set tmp_plus = "$1" ; shift
   else
     set tmp_choice="$1" ; shift
     if ( "$1" == "+") then
       shift
       set tmp_plus = "$1" ; shift 
       if ($nar >= 5) then
         set tmp_plus_choice = "$1" ; shift
       endif
     endif 
   endif
endif

set patch = ""
set tmp_gaudi = ""
if ("$tmp_choice" != "") then
  set patch = `echo $tmp_choice | grep p`
  if ("$tmp_project" == "Gaudi") then
    set tmp_gaudi = `echo $tmp_choice | tr -d '[vr]'`
  endif
endif  
#unset echo
#

set tmp_currentdir=`pwd`

if ("$tmp_gaudi" != "" && "$tmp_gaudi" <= 151) then
  eval cd '$'${tmp_project}_release_area
else
  eval cd '$'${tmp_project}_release_area/${tmp_PROJECT}
endif

set tmp_versions=(`eval /bin/ls -ltr . | sed -e '/.*'${tmp_PROJECT}'_v[0-9][0-9]*r[0-9][0-9]*$/'\!\d -e 's/.*'${tmp_PROJECT}'_//' | tr '\012' '\011'`)

echo Setting ${tmp_project} environment

while (1)
  if ("$patch" != "") goto setup

  foreach tmp_version ($tmp_versions[*])
    if ( "$tmp_version" == "$tmp_choice" ) then
      goto setup
    endif
  end

   echo -n "Please enter your choice ("

   set tmp_npos=0
   foreach tmp_version ($tmp_versions[*])
     @ tmp_npos++
     /bin/ls ${tmp_PROJECT}_$tmp_version >&! /dev/null && echo -n "$tmp_version " && set tmp_nlast=$tmp_npos
   end
   set tmp_last=${tmp_versions[$tmp_nlast]}
   echo -n "q[uit] [$tmp_last]): "
   set tmp_choice="$<"

switch ( $tmp_choice )
case q:
  unset tmp_choice; goto exec_end
  breaksw
case "":
  set tmp_choice=$tmp_last; break
  breaksw
endsw
end

setup:
cd $tmp_currentdir
#unset echo

# set project environment ==============================================================
if ( $?tmp_choice ) then
  eval source \$${tmp_project}_release_area/${tmp_project}Env/$tmp_choice/cmt/setup.csh
  
# if a + <project> is required add it to CMTPATH =======================================
#  set echo on
  if ("$tmp_plus" != "") then
    set tmp_PLUS = `echo ${tmp_plus} | tr '[a-z]' '[A-Z]'`
    eval set isset='$?'${tmp_plus}_release_area
    if ( ! $isset ) then 
       setenv ${tmp_plus}_release_area $LHCBRELEASES
    endif   

# get the LHCb version used by the main <project> and get the compatible versions of the + <project>
    eval cd '$'${tmp_project}_release_area/${tmp_project}Env/$tmp_choice/cmt  
    set lvers = `$CMT show macro_value LHCBVERS`
    if ("$tmp_plus_choice" != "") then
      eval cd '$'${tmp_plus}_release_area/${tmp_plus}Env/$tmp_plus_choice/cmt
      set pluslvers =  `$CMT show macro_value LHCBVERS`
      if ("$pluslvers" != "$lvers" ) then
        echo "$tmp_project $tmp_choice is not compatible with $tmp_plus $tmp_plus_choice - EXIT"
        goto exec_end
      else
        set v = $tmp_plus_choice
        eval cd '$'${tmp_plus}_release_area
        set pluspath = $PWD 
        goto addplus
      endif
    else
      eval cd '$'${tmp_plus}_release_area
      set pluspath = $PWD
      set vlist = `grep -l "${lvers}" ${tmp_plus}Env/v*/cmt/requirements | awk -F"/" '{print $2}'`
      if ("$vlist" == "") then
        echo " there is no $tmp_plus version compatible with $tmp_project version chosen - exit"
        goto cmtpath
      endif
    endif 

# choose the version of the + <project>
choose:  
    echo -n "Please enter the ${tmp_plus} version ("
    echo -n "$vlist q[uit] [${vlist[$#vlist]}]): "
    set v = "$<"
    if ("$v" == "") then 
      set v = "${vlist[$#vlist]}"
    else 
      if ("$v" == "q") goto cmtpath
      foreach w ($vlist[*])
        if ("$w" == "$v") goto addplus
      end
      goto choose
    endif
    
# introduce the + <project> path after the main <project> path and before LHCb path
addplus: 
    set fields = `echo $CMTPATH | tr ':' ' '`
    set mypath
    set found
    foreach field ($fields)
      set mypath = ${mypath}:${field}
      if ( $?found ) then
        echo $field | grep "${tmp_PROJECT}" >&! /dev/null
        if ($status == 0) then
          set mypath = ${mypath}:${pluspath}/${tmp_PLUS}/${tmp_PLUS}_$v
          unset found
        endif
      endif
    end

   setenv CMTPATH `echo $mypath | sed -e 's/^://'`

  endif
endif

#===== Check for versions only available on slc3
echo $CMTCONFIG | grep "slc3_ia32_gcc323" >&! /dev/null
if( $status != 0 ) then 
  set fields = `echo $CMTPATH | tr ':' ' '`
  foreach field ($fields)
     echo $field | grep "GAUDI" >&! /dev/null
     if ($status == 0) then
        set tmp_version = `echo ${field} | tr -d '[:alpha:]' | tr -d '[:punct:]'`
        if ($tmp_version < 189) then
           echo "==============================================================================="
           echo "WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING"
           echo "This version is not available for $CMTCONFIG. Please go to an slc3 machine"
           echo "WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING"
           echo "==============================================================================="
           goto exec_end
        endif
     endif
  end
endif

#==================================================================================================
cmtpath:
setenv CMTPATH `echo $CMTPATH | sed -e 's/^://'`
cd $tmp_currentdir
echo " -------------------------------------------------------------------- "
echo " --- ${tmp_project} environment is set for version $tmp_choice "
echo " --- packages will be searched for in $CMTPATH "
echo " -------------------------------------------------------------------- "

endif

if (-d $LHCBHOME/project/logfiles) then
   touch $LHCBHOME/project/logfiles/${tmp_PROJECT}_${tmp_choice}_$USER
endif

exec_end:
# Unconditionally set an LD_LIBRARY_PATH to please python (MC 2005-01-31)
if ( ! $?LD_LIBRARY_PATH ) setenv LD_LIBRARY_PATH
cd $tmp_currentdir
# unset temporary variables
unset tmp_project; unset tmp_isset; unset tmp_version; unset tmp_versions
unset tmp_PROJECT
#if ($?tmp_choice) unset tmp_choice
if ($?tmp_last) unset tmp_last
if ($?tmp_PLUS) then
  unset tmp_plus; unset tmp_PLUS; unset pluspath; unset isset
  unset v; unset w; unset n; unset m
  unset field; unset found; unset fields
  unset mypath
endif
unset tmp_npos; unset tmp_nlast

else
  echo LHCBHOME: parameter not set
endif
unset tmp_currentdir
unset echo 

exit:

