#!/bin/sh

argv0="\$(LCG_SETTINGSROOT)/scripts/icc_setup.sh"

intelplat=$1
intel_home=$2
icc_c_home=$3
icc_f_home=$4


if [ "x${intelplat}" != "x" -a "x${intel_home}" != "x" -a "x${icc_c_home}" != "x" -a "x${icc_f_home}" != "x" ]; then

  echo "${argv0}: Processing setup for Intel Compiler Suite"

  if [ x`env | grep INTEL_LICENSE_FILE` = "x" ] ; then
    echo "${argv0}: Sourcing Intel lincensing script"
    . ${intel_home}/setup.sh
  else
    echo "${argv0}: Intel license already sourced, skipping this step ..."
  fi

  if [ x`echo $PATH | grep ${icc_f_home}` = "x" ] ; then
    if [ -e ${icc_f_home}/bin/ifortvars.sh ] ; then
      echo "${argv0}: Sourcing scripts for Intel Fortran compiler environment" 
      . ${icc_f_home}/bin/ifortvars.sh intel64
    else 
      echo "${argv0}: No script for ifort variables found" 
    fi
  else
    echo "${argv0}: ifort environment already setup, skipping this step ..." 
  fi

  if [ x`echo $PATH | grep ${icc_c_home}` = "x" ] ; then
    if [ -e ${icc_c_home}/bin/iccvars.sh ] ; then
      echo "${argv0}: Sourcing scripts for Intel C compiler environment" 
      . ${icc_c_home}/bin/iccvars.sh intel64
    else 
      echo "${argv0}: No script for icc variables found" 
    fi
  else
    echo "${argv0}: icc environment already setup, skipping this step ..." 
  fi

  echo "${argv0}: Setup for Intel Compiler Suite finished"

fi 

