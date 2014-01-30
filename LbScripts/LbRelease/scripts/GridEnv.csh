#!/bin/csh
set rh = `echo $CMTCONFIG | awk -F_ '{print $1 "_" $2}'`
if ( $# == 1 ) then
     if ( $1 == "-v" ) then
         echo "Setting up environment for the grid on a $rh type machine"
     endif
endif

# until further notice the script is valid for slc4 and slc3!!!
if ( $?rh ) then
  if ( $?X509_USER_PROXY ) then
     setenv SAVE_PROXY $X509_USER_PROXY
  endif
  if ( $rh == "x86_64-slc5-gcc43-opt" ) then
      source /afs/cern.ch/project/gd/LCG-share/sl5/etc/profile.d/grid_env.csh
  else
      echo "$CMTCONFIG not known, use the current UI settings"
      source /afs/cern.ch/project/gd/LCG-share/sl5/etc/profile.d/grid_env.csh
  endif
  if ( $?SAVE_PROXY ) then
     setenv X509_USER_PROXY $SAVE_PROXY
     unsetenv SAVE_PROXY
  endif
endif