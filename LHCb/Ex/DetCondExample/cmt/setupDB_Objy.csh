#!/usr/local/bin/tcsh
# $Id: setupDB_Objy.csh,v 1.1 2002-03-28 13:28:52 andreav Exp $

# Objectivity condition database location
# Moved here from requirements because OBJY_LS_HOST is undefined on Windows
setenv DETCONDEXAMPLE_FDID     30997
setenv DETCONDEXAMPLE_BOOTHOST ${OBJY_LS_HOST}
setenv DETCONDEXAMPLE_BOOTDIR  ${DETCONDEXAMPLEROOT}/DB
setenv DETCONDEXAMPLE_BOOTFILE condDBBoot

# The following four variables will be passed on to the job options
setenv DETCONDEXAMPLE_DBHOST   ${DETCONDEXAMPLE_BOOTHOST}
setenv DETCONDEXAMPLE_DBUSER   user
setenv DETCONDEXAMPLE_DBPSWD   password
setenv DETCONDEXAMPLE_DBNAME   \
       ${DETCONDEXAMPLE_BOOTDIR}/${DETCONDEXAMPLE_BOOTFILE}

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

