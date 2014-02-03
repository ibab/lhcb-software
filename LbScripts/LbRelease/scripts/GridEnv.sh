#!/bin/sh
rh=`echo "${CMTCONFIG}" | awk -F- '{print $2}'`

if [ $# -eq 1 ]; then
     if [ $1='-v' ]; then
         echo "Setting up environment for the grid on a $rh type machine"
     fi
fi
# until further notice the script is valid on slc5 and slc6
if [ x"$rh" != "x" ]; then
  if [ x"$X509_USER_PROXY" != "x" ]; then
     SAVE_PROXY=$X509_USER_PROXY
  fi
  if [ "$rh" = "slc5" ]; then
      . /afs/cern.ch/project/gd/LCG-share/sl5/etc/profile.d/grid_env.sh
  fi
  if [ x"$SAVE_PROXY" != "x" ]; then
     X509_USER_PROXY=$SAVE_PROXY
     unset SAVE_PROXY
  fi
fi