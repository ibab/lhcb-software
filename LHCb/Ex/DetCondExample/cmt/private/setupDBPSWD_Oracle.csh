#!/usr/local/bin/tcsh
# $Id: setupDBPSWD_Oracle.csh,v 1.1 2002-03-28 14:29:56 andreav Exp $
#===============
# Use UPPERCASE 
#===============
# Oracle password for user $DETCONDEXAMPLE_DBUSER on $DETCONDEXAMPLE_DBHOST
if ( ${?DETCONDEXAMPLE_DBPSWD} != 1 ) then
  setenv DETCONDEXAMPLE_DBPSWD PASSWORD
endif
