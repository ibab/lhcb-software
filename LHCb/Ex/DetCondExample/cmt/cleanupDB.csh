#!/usr/local/bin/tcsh
# $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/cmt/cleanupDB.csh,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $

#set out = /dev/stdout
set out  = /dev/null

echo Cleanup Objectivity Federated Database for DetCondExample > $out

if ( { test -e ${DETCONDEXAMPLE_BOOTDIR}/${DETCONDEXAMPLE_BOOTFILE} } ) then
  echo Deleting Federated Database > $out
  ${OBJY_DIR}/bin/oodeletefd \
    -f ${DETCONDEXAMPLE_BOOTDIR}/${DETCONDEXAMPLE_BOOTFILE} > $out
endif
if ( { test -d ${DETCONDEXAMPLE_BOOTDIR} } ) then
  echo Deleting Database directory > $out
  \rm -rf ${DETCONDEXAMPLE_BOOTDIR}
endif

echo Objectivity DB for DetCondExample and its directory have been deleted
unset out




