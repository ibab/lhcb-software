rem $Id: setupDB_Oracle.bat,v 1.3 2002-05-06 16:35:26 andreav Exp $
rem Setup Oracle environment variables
rem Do not overwrite env variables on Windows (setup is called by run script)
rem #================================
rem # Use UPPERCASE for all fields!!
rem #================================
rem # Oracle database host
if "%DETCONDEXAMPLE_DBHOST%." == "." ( 
  set DETCONDEXAMPLE_DBHOST=DB9
)
rem # Oracle user on this host
if "%DETCONDEXAMPLE_DBUSER%." == "." ( 
  set DETCONDEXAMPLE_DBUSER=AVALASSI
)
rem # Condition database created in the account of this user
if "%DETCONDEXAMPLE_DBNAME%." == "." ( 
  set DETCONDEXAMPLE_DBNAME=MYCONDDB_WINDOWS
)
rem # Oracle password for this user on this host
if "%DETCONDEXAMPLE_DBPSWD%." == "." ( 
  echo Please set the correct value of DETCONDEXAMPLE_DBPSWD if not yet done
  set DETCONDEXAMPLE_DBPSWD=PASSWORD
)

