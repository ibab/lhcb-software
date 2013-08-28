
unalias cd
unalias cwd
# Get the ICC base environment from LCGCMT
set LCG_Interfaces_gcc = /afs/cern.ch/sw/lcg/app/releases/LCGCMT/LCGCMT-latest/LCG_Interfaces/gcc/cmt
set intelplat=`unalias cd ; unalias cwdcmd ; cd $LCG_Interfaces_gcc ; setenv CMTCONFIG x86_64-slc6-icc13-dbg ; cmt show set_value intelplat`
set intel_home=`unalias cd ; unalias cwdcmd ;  cd $LCG_Interfaces_gcc ; setenv CMTCONFIG x86_64-slc6-icc13-dbg ; cmt show set_value intel_home`
set icc_c_home=`unalias cd ; unalias cwdcmd ;  cd $LCG_Interfaces_gcc ; setenv CMTCONFIG x86_64-slc6-icc13-dbg ; cmt show set_value icc_c_home`
set icc_f_home=`unalias cd ; unalias cwdcmd ;  cd $LCG_Interfaces_gcc ; setenv CMTCONFIG x86_64-slc6-icc13-dbg ; cmt show set_value icc_f_home`

# source Intel scripts
if ( ! $?INTEL_LICENSE_FILE ) source ${intel_home}/setup.csh > /dev/null
if ( -e ${icc_c_home}/bin/iccvars.csh ) source ${icc_c_home}/bin/iccvars.csh $intelplat
if ( -e ${icc_f_home}/bin/ifortvars.csh ) source ${icc_c_home}/bin/ifortvars.csh $intelplat

unset intel_home icc_c_home icc_f_home LCG_Interfaces_gcc


