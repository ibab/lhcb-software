/* $Id: listCondDBs.sql,v 1.1 2002-03-01 16:54:24 andreav Exp $ */
set pagesize 80;
ttitle 'List existing ConditionDBs:';
select * from condition_dbs;
quit;
