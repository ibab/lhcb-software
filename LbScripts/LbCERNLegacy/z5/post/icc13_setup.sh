#!/bin/sh

# Get the ICC base environment from LCGCMT
LCG_Interfaces_gcc=/afs/cern.ch/sw/lcg/app/releases/LCGCMT/LCGCMT-latest/LCG_Interfaces/gcc/cmt
intelplat=`cd $LCG_Interfaces_gcc ; CMTCONFIG=x86_64-slc6-icc13-dbg cmt show set_value intelplat`
intel_home=`cd $LCG_Interfaces_gcc ; CMTCONFIG=x86_64-slc6-icc13-dbg cmt show set_value intel_home`
icc_c_home=`cd $LCG_Interfaces_gcc ; CMTCONFIG=x86_64-slc6-icc13-dbg cmt show set_value icc_c_home`
icc_f_home=`cd $LCG_Interfaces_gcc ; CMTCONFIG=x86_64-slc6-icc13-dbg cmt show set_value icc_f_home`

# source Intel scripts
[ -z "$INTEL_LICENSE_FILE" ] && . ${intel_home}/setup.sh > /dev/null
[ -e ${icc_c_home}/bin/iccvars.sh ] && . ${icc_c_home}/bin/iccvars.sh $intelplat
[ -e ${icc_f_home}/bin/ifortvars.sh ] && . ${icc_c_home}/bin/ifortvars.sh $intelplat

unset intel_home icc_c_home icc_f_home LCG_Interfaces_gcc
