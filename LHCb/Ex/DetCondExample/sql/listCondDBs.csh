#!/usr/local/bin/tcsh
# $Id: listCondDBs.csh,v 1.1 2002-03-28 13:23:19 andreav Exp $
${ORACLE9_DIR}/bin/sqlplus ${DETCONDEXAMPLE_DBUSER}/${DETCONDEXAMPLE_DBPSWD}@${DETCONDEXAMPLE_DBHOST} @${DETCONDEXAMPLEROOT}/sql/listCondDBs
