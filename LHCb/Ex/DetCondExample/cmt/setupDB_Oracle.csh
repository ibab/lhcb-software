#!/usr/local/bin/tcsh
# $Id: setupDB_Oracle.csh,v 1.2 2002-03-28 14:30:49 andreav Exp $
#================================
# Use UPPERCASE for all fields!!
#================================
# Oracle database host
setenv DETCONDEXAMPLE_DBHOST DB9
# Oracle user on this host
setenv DETCONDEXAMPLE_DBUSER AVALASSI
# Condition database created in the account of this user
setenv DETCONDEXAMPLE_DBNAME MYCONDDB_LINUX
# Oracle password for this user on this host 
source ${DETCONDEXAMPLEROOT}/cmt/private/setupDBPSWD_Oracle.csh
