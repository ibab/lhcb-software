rem $Id: setupDBPSWD_Oracle.bat,v 1.1 2002-03-28 14:29:56 andreav Exp $
rem #===============
rem # Use UPPERCASE
rem #===============
rem # Oracle password for user $DETCONDEXAMPLE_DBUSER on $DETCONDEXAMPLE_DBHOST
if "%DETCONDEXAMPLE_DBPSWD%." == "." ( 
  set DETCONDEXAMPLE_DBPSWD=PASSWORD
)
