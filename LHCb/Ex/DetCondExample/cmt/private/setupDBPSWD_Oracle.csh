#!/usr/local/bin/tcsh
# $Id: setupDBPSWD_Oracle.csh,v 1.2 2002-04-05 16:26:06 andreav Exp $
#===============
# Use UPPERCASE 
#===============
# Oracle password for user $DETCONDEXAMPLE_DBUSER on $DETCONDEXAMPLE_DBHOST
if ( ${?DETCONDEXAMPLE_DBPSWD} != 1 ) then
  echo Please set the correct value of DETCONDEXAMPLE_DBPSWD if not yet done
  setenv DETCONDEXAMPLE_DBPSWD PASSWORD
endif
