#!/usr/local/bin/tcsh -f 
# $Id: DiffAllTests.csh,v 1.4 2002-12-06 11:57:17 andreav Exp $
if ( 1 != ${?DETCONDEXAMPLEROOT} ) then
  echo "ERROR! You must 'source ../cmt/setup.csh -tag=$CMTDEB' first"
  exit 1
endif
cd ${DETCONDEXAMPLEROOT}/job
dos2unix NEWlog.Win32Debug.*/log.* >& /dev/null

# New versus old logfile
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
  set theDiff = diff.$theConfig
  set theOldDir = ../job/log.$theConfig
  set theNewDir = ../job/NEWlog.$theConfig
  if ( -d $theOldDir && -d $theNewDir ) then
    rm -f $theDiff 
    touch $theDiff
    diff $theNewDir $theOldDir >> $theDiff
  endif
  unset theNewDir
  unset theOldDir
  unset theDiff
end

# Linux versus Windows on the new logfiles
foreach theImpl ( CondDBMySQL CondDBOracle )
  set theDiff = diff.Lin-vs-Win.$theImpl
  set theLinDir = ../job/NEWlog.rh73_gcc2952.$theImpl
  set theWinDir = ../job/NEWlog.Win32Debug.$theImpl
  if ( -d $theLinDir && -d $theWinDir ) then
    rm -f $theDiff 
    touch $theDiff
    diff $theLinDir $theWinDir >> $theDiff
  endif
  unset theWinDir
  unset theLinDir
  unset theDiff
end

# (Linux on Win32 DB) versus (Win32) on the new logfiles
foreach theImpl ( CondDBMySQL CondDBOracle )
  set theDiff = diff.mixDbWIN32.$theImpl
  set theLinDir = ../job/NEWlog.rh73_gcc2952.$theImpl.dbWIN32
  set theWinDir = ../job/NEWlog.Win32Debug.$theImpl
  if ( -d $theLinDir && -d $theWinDir ) then
    rm -f $theDiff 
    touch $theDiff
    diff $theLinDir $theWinDir >> $theDiff
  endif
  unset theWinDir
  unset theLinDir
  unset theDiff
end

# (Win32 on Linux DB) versus (Linux) on the new logfiles
foreach theImpl ( CondDBMySQL CondDBOracle )
  set theDiff = diff.mixDbLINUX.$theImpl
  set theLinDir = ../job/NEWlog.rh73_gcc2952.$theImpl
  set theWinDir = ../job/NEWlog.Win32Debug.$theImpl.dbLINUX
  if ( -d $theLinDir && -d $theWinDir ) then
    rm -f $theDiff 
    touch $theDiff
    diff $theLinDir $theWinDir >> $theDiff
  endif
  unset theWinDir
  unset theLinDir
  unset theDiff
end

# Oracle vs MySQL on the new logfiles
foreach theArch ( rh73_gcc2952 Win32Debug )
  set theDiff = diff.$theArch.Oracle-vs-MySQL
  set theOraDir = ../job/NEWlog.$theArch.CondDBMySQL
  set theMySDir = ../job/NEWlog.$theArch.CondDBOracle
  if ( -d $theOraDir && -d $theMySDir ) then
    rm -f $theDiff 
    touch $theDiff
    diff $theOraDir $theMySDir >> $theDiff
  endif
  unset theMySDir
  unset theOraDir
  unset theDiff
end

