#!/usr/local/bin/tcsh
# $Id: setup_RedHat72.csh,v 1.2 2002-03-28 16:39:57 andreav Exp $

# Do you need to run this script? Are you on RedHat7.2?
if ( `fs sysname` != "Current sysname is 'i386_linux24'" ) then
  exit 0
endif

# Do you need to run this script (again)? Is CMTCONFIG not ok for RedHat7.2?
if ( ${?CMTCONFIG} != 1 ) then
  setenv CMTCONFIG undefined
endif
if ( "${CMTCONFIG}" == "rh72_gcc29521" ) then
  exit 0
endif

# Banner
echo "******************************************************"
echo "*       WELCOME to RedHat7.2 with gcc2.95.2          *"
echo "******************************************************"

# Clear PATH and LD_LIBRARY_PATH
setenv PATH $SAVEPATH
unsetenv LD_LIBRARY_PATH
unsetenv COMPILER_PATH

# Re-configure CMT to use v1r10p20011126
source $SITEROOT/sw/contrib/CMT/v1r10p20011126/mgr/setup.csh

# Define the compilation tags
setenv CMTCONFIG "rh72_gcc29521"
setenv CMTDEB    "${CMTCONFIG}dbx"
setenv CMTSTATIC "${CMTCONFIG}Static"

# The default compiler on RedHat7.2 is gcc 2.96
echo Previously using g++ compiler: `which g++`
g++ -v

# Use the gcc 2.95.2 compiler instead
setenv COMPILER_PATH "/usr/local/gcc-alt-2.95.2"
set path =(${COMPILER_PATH}/bin $path)
setenv LD_LIBRARY_PATH "${COMPILER_PATH}/lib"
echo " -------------------------------------------------------------------- "
echo Presently using g++ compiler: `which g++`
g++ -v

# Assume that ${HOME}/newmycmt is the private CMT directory
setenv CMTPATH ${HOME}/newmycmt

# Reconfigure the LHCXX external libraries
# The default setup in $GAUDISOFT/ExternalLibs/v3r3 does not foresee rh72
echo " -------------------------------------------------------------------- "
echo Redefine LHCXX_DIR as ${SWROOT}/lhcxx/specific/redhat72/gcc-2.95.2-from72
setenv LHCXX_DIR ${SWROOT}/lhcxx/specific/redhat72/gcc-2.95.2-from72

# Closing banner
echo " -------------------------------------------------------------------- "
echo " --- "\$CMTROOT " is set to $CMTROOT "
echo " --- "\$CMTCONFIG " is set to $CMTCONFIG "
echo " --- to compile and link in debug mode : gmake tag="\$CMTDEB
echo " --- to link in static mode : gmake tag="\$CMTSTATIC
echo " --- "\$CMTPATH " is set to $CMTPATH " 
echo " --- packages will be searched in "\$CMTPATH "and then in "\$LHCBNEW":"\$GAUDISOFT 
echo " -------------------------------------------------------------------- "


