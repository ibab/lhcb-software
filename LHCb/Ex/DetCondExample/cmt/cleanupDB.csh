#!/usr/local/bin/tcsh
# $Id: cleanupDB.csh,v 1.2 2001-12-17 19:44:45 andreav Exp $

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




