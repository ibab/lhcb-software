rem $Id: dropDB_Oracle.bat,v 1.2 2004-12-08 17:22:37 marcocle Exp $
rem Remove the Oracle ConditionsDB
call %CONDDBROOT%\cmt\condDBOracle_dropCondDB.csh %CONDDBORACLE_DBUSER% %CONDDBORACLE_DBPSWD% %CONDDBORACLE_DBHOST% %CONDDBORACLE_DBNAME%
