#!/usr/local/bin/tcsh
# $Id: setup_RedHat72.csh,v 1.6 2002-04-17 16:09:02 andreav Exp $

# Re-configure CMT to use v1r12p20020412 if needed (normally this is default)
# Do this for both RedHat 6.1 and 7.2
set tmp_banner = 0
if ( ${?CMTROOT} != 1 ) then
  setenv CMTROOT undefined
endif
if ( ${CMTROOT} != $SITEROOT/sw/contrib/CMT/v1r12p20020412 ) then
  setenv CMTROOT $SITEROOT/sw/contrib/CMT/v1r12p20020412
  source $CMTROOT/mgr/setup.csh
  set tmp_banner = 1
endif

# Assume you are on RedHat6.1 if CMTCONFIG is undefined
if ( ${?CMTCONFIG} != 1 ) then
  setenv CMTCONFIG rh61_gcc2952
  setenv CMTDEB    "${CMTCONFIG}dbx"
  setenv CMTSTATIC "${CMTCONFIG}Static"
  set tmp_banner = 1
endif

# Assume ${HOME}/newmycmt is the private CMT directory if CMTPATH is undefined
if ( ${?CMTPATH} != 1 ) then
  setenv CMTPATH ${HOME}/newmycmt
endif

# Do you need to reconfigure the environment for RedHat7.2? 
# -> 1. Are you currenly on RedHat7.2?
# -> 2. Is CMTCONFIG not ok for RedHat7.2?
if ( `fs sysname` == "Current sysname is 'i386_linux24'" ) then
if ( "${CMTCONFIG}" != "rh72_gcc2952" ) then

# Banner
echo "******************************************************"
echo "*       WELCOME to RedHat7.2 with gcc2.95.2          *"
echo "******************************************************"
set tmp_banner = 1

# Clear PATH and LD_LIBRARY_PATH
setenv PATH $SAVEPATH
unsetenv LD_LIBRARY_PATH
unsetenv COMPILER_PATH

# Define the compilation tags
setenv CMTCONFIG "rh72_gcc2952"
setenv CMTDEB    "${CMTCONFIG}dbx"
setenv CMTSTATIC "${CMTCONFIG}Static"

# The default compiler on RedHat7.2 (not the LHCb one though!) is gcc 2.96
echo Default g++ compiler on RedHat7.2: `which g++`
g++ -v

# Use the gcc 2.95.2 compiler instead
setenv COMPILER_PATH "/usr/local/gcc-alt-2.95.2"
set path =(${COMPILER_PATH}/bin $path)
setenv LD_LIBRARY_PATH "${COMPILER_PATH}/lib"
echo " -------------------------------------------------------------------- "
echo Set new default g++ compiler: `which g++`
g++ -v

# Re-configure the external libraries
# The default setup in $GAUDISOFT/ExternalLibs/v3r6 does not foresee rh72
# The best is to modify PLATARCHdef in ExternalLibs and source its setup
echo " -------------------------------------------------------------------- "
setenv LHCXX_DIR ${SWROOT}/lhcxx/specific/redhat72/gcc-2.95.2
setenv CLHEP_DIR ${LHCXX_DIR}/CLHEP
setenv HTL_DIR   ${LHCXX_DIR}/HTL
setenv NAGC_DIR  ${LHCXX_DIR}/Nag_C
echo "Define LHCXX_DIR as ${LHCXX_DIR}"
echo "Define CLHEP_DIR as ${CLHEP_DIR}"
echo "Define HTL_DIR   as ${HTL_DIR}"
echo "Define NAGC_DIR  as ${NAGC_DIR}"

# Endif need to reconfigure the environment
endif
endif

# Closing banner
if ( $tmp_banner == 1 ) then
echo " -------------------------------------------------------------------- "
echo " --- "\$CMTROOT " is set to $CMTROOT "
echo " --- "\$CMTCONFIG " is set to $CMTCONFIG "
echo " --- to compile and link in debug mode : gmake tag="\$CMTDEB
echo " --- to link in static mode : gmake tag="\$CMTSTATIC
echo " --- "\$CMTPATH " is set to $CMTPATH " 
echo " --- packages will be searched in "\$CMTPATH "and then in "\$LHCBNEW":"\$GAUDISOFT 
echo " -------------------------------------------------------------------- "
unset tmp_banner
endif

# Echo system information
fs sysname
echo Kernel version is `uname -mrs`
echo Using g++ compiler `which g++`
g++ -v | & awk '{if ($1=="gcc") print $0}'
set tmp_lib = `g++ -print-file-name=libc.so.6`
echo Using glibc library `\ls -l $tmp_lib | awk '{print $NF}'`
set tmp_lib = `g++ -print-file-name=libstdc++.so`
echo Using stdc++ library `\ls -l $tmp_lib | awk '{print substr($NF,index($NF,"libstdc"))}'`
unset tmp_lib


