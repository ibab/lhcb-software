rem $Id: setupDBPSWD_Oracle.bat,v 1.2 2002-04-05 16:26:06 andreav Exp $
rem #===============
rem # Use UPPERCASE
rem #===============
rem # Oracle password for user $DETCONDEXAMPLE_DBUSER on $DETCONDEXAMPLE_DBHOST
if "%DETCONDEXAMPLE_DBPSWD%." == "." ( 
  echo Please set the correct value of DETCONDEXAMPLE_DBPSWD if not yet done
  set DETCONDEXAMPLE_DBPSWD=PASSWORD
)
