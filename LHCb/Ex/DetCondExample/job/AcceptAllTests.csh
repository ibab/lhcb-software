#!/usr/local/bin/tcsh -f 
# $Id: AcceptAllTests.csh,v 1.1 2003-01-31 21:39:41 andreav Exp $
if ( 1 != ${?DETCONDEXAMPLEROOT} ) then
  echo "ERROR! You must 'source ../cmt/setup.csh -tag=$CMTDEB' first"
  exit 1
endif
cd ${DETCONDEXAMPLEROOT}/job

# Protection against unnecessary changes
echo "You must comment out this line!"; exit 1

# Create diff files first
./DiffAllTests.csh

# Set echo ON
set echo

# New versus old log files
foreach theConfig ( \
    Win32Debug.CondDBMySQL \
    Win32Debug.CondDBMySQL.dbLINUX \
    Win32Debug.CondDBOracle \
    Win32Debug.CondDBOracle.dbLINUX \
    rh61_gcc2952.CondDBObjy \
    rh73_gcc2952.CondDBMySQL \
    rh73_gcc2952.CondDBMySQL.dbWIN32 \
    rh73_gcc2952.CondDBOracle \
    rh73_gcc2952.CondDBOracle.dbWIN32 )
  set theOldDir = ../job/log.$theConfig
  set theNewDir = ../job/NEWlog.$theConfig
  if ( -d $theOldDir && -d $theNewDir ) then
    mv $theNewDir/log.* $theOldDir
  endif
  unset theNewDir
  unset theOldDir
end

# New versus old diff files
set theOldDir = ../job/log.Diff
set theNewDir = ../job/
if ( -d $theOldDir && -d $theNewDir ) then
  mv $theNewDir/diff.* $theOldDir
endif
unset theNewDir
unset theOldDir

# Set echo OFF
unset echo


