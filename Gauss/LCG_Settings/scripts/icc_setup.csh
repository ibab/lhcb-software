#!/bin/csh

set argv0=\${LCG_SETTINGSROOT}/scripts/icc_setup.sh

setenv intelplat   ${1}
setenv intel_home  ${2}
setenv icc_c_home  ${3}
setenv icc_f_home  ${4} 

if ( ${intelplat} != "" && ${intel_home} != "" && ${icc_c_home} != "" && ${icc_f_home} != "" ) then
  echo "$argv0: Processing setup for Intel Compiler Suite"

  if ( `env | grep INTEL_LICENSE_FILE` == "" ) then
    echo "${argv0}: Sourcing Intel lincensing script"
    source ${intel_home}/setup.csh
  else
    echo "${argv0}: Intel license already sourced, skipping this step ..."
  endif

  if ( `echo $PATH | grep ${icc_f_home}` == "" ) then
    if ( -e ${icc_f_home}/bin/ifortvars.csh ) then
      echo "${argv0}: Sourcing scripts for Intel Fortran compiler environment" 
      source ${icc_f_home}/bin/ifortvars.csh 
    else 
      echo "${argv0}: No script for ifort variables found" 
    endif
  else
    echo "${argv0}: ifort environment already setup, skipping this step ..." 
  endif

  if ( `echo $PATH | grep ${icc_c_home}` == "x" ) then
    if ( -e ${icc_c_home}/bin/iccvars.csh ) then
      echo "${argv0}: Sourcing scripts for Intel C compiler environment" 
      source ${icc_c_home}/bin/iccvars.csh 
    else 
      echo "${argv0}: No script for icc variables found" 
    endif
  else
    echo "${argv0}: icc environment already setup, skipping this step ..." 
  endif

  echo "${argv0}: Setup for Intel Compiler Suite finished"

endif 

#cleanup of variables
unset argv0

