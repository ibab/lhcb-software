#!/usr/local/bin/tcsh
# $Id: cleanupDB_Oracle.csh,v 1.1 2002-03-28 13:28:52 andreav Exp $

set out  = /dev/null

echo Cleanup Objectivity Federated Database for DetCondExample > $out

# Cleaner way to remove the federated database (w.r.t. brute force \rm -rf)
# Because of CMT features, the lockserver is killed too early: use brute force
#if ( { test -e ${DETCONDEXAMPLE_BOOTDIR}/${DETCONDEXAMPLE_BOOTFILE} } ) then
#  echo Deleting Federated Database > $out
#  ${OBJY_DIR}/bin/oodeletefd \
#    -f ${DETCONDEXAMPLE_BOOTDIR}/${DETCONDEXAMPLE_BOOTFILE} > $out
#endif

if ( { test -d ${DETCONDEXAMPLE_BOOTDIR} } ) then
  echo Deleting Database directory > $out
  \rm -rf ${DETCONDEXAMPLE_BOOTDIR}
endif

echo Objectivity DB for DetCondExample and its directory have been deleted
unset out




