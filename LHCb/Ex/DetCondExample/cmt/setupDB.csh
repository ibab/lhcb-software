#!/usr/local/bin/tcsh
# $Id: setupDB.csh,v 1.3 2001-12-17 19:44:45 andreav Exp $

# Objectivity condition database location
# Moved here from requirements because OBJY_LS_HOST is undefined on Windows
setenv DETCONDEXAMPLE_FDID     30997
setenv DETCONDEXAMPLE_BOOTHOST ${OBJY_LS_HOST}
setenv DETCONDEXAMPLE_BOOTDIR  ${DETCONDEXAMPLEROOT}/DB
setenv DETCONDEXAMPLE_BOOTFILE condDBBoot

set out  = /dev/null

echo Setup Objectivity Federated Database for DetCondExample > $out
echo Federated Database number is ${DETCONDEXAMPLE_FDID} > $out
echo Federated Database Boot file is > $out
echo "   "${DETCONDEXAMPLE_BOOTDIR}/${DETCONDEXAMPLE_BOOTFILE} > $out

if ( { test -e ${DETCONDEXAMPLE_BOOTDIR}/${DETCONDEXAMPLE_BOOTFILE} } ) then
  echo Federated Database has already been installed > $out
else
  if ( ! { test -d ${DETCONDEXAMPLE_BOOTDIR} } ) then
    echo Create directory ${DETCONDEXAMPLE_BOOTDIR} > $out
    mkdir ${DETCONDEXAMPLE_BOOTDIR}
  endif
  echo Install Federated Database > $out
  echo Import schema from \
    ${CONDDB_BOOTDIR}/${CONDDB_BOOTFILE} > $out
  ${HEPODBMS_DIR}/etc/getdb \
    ${CONDDB_BOOTDIR}/${CONDDB_BOOTFILE} \
    ${DETCONDEXAMPLE_BOOTDIR}/${DETCONDEXAMPLE_BOOTFILE} \
    ${DETCONDEXAMPLE_FDID} > $out
endif
echo Objectivity Federated Database for DETCONDEXAMPLE is installed on
echo "   "${DETCONDEXAMPLE_BOOTDIR}/${DETCONDEXAMPLE_BOOTFILE}

unset out

