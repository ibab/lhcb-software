/* $Id: showNLS.sql,v 1.1 2002-07-25 15:52:11 andreav Exp $ */
set pagesize 80;
SELECT parameter,value FROM NLS_DATABASE_PARAMETERS WHERE parameter like 'NLS%';
quit;
