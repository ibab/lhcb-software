#!/usr/local/bin/tcsh
# $Id: setupDB_Oracle.csh,v 1.3 2002-05-06 16:35:26 andreav Exp $
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
if ( ${?DETCONDEXAMPLE_DBPSWD} != 1 ) then
  echo Please set the correct value of DETCONDEXAMPLE_DBPSWD if not yet done
  setenv DETCONDEXAMPLE_DBPSWD PASSWORD
endif
