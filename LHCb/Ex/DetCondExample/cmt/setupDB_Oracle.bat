rem $Id: setupDB_Oracle.bat,v 1.2 2002-03-28 14:30:49 andreav Exp $
rem Setup Oracle environment variables
rem #================================
rem # Use UPPERCASE for all fields!!
rem #================================
rem # Oracle database host
set DETCONDEXAMPLE_DBHOST=DB9
rem # Oracle user on this host
set DETCONDEXAMPLE_DBUSER=AVALASSI
rem # Condition database created in the account of this user
set DETCONDEXAMPLE_DBNAME=MYCONDDB_WINDOWS
rem # Oracle password for this user on this host
call %DETCONDEXAMPLEROOT%\cmt\private\setupDBPSWD_Oracle.bat

