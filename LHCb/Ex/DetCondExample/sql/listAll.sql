/* $Id: listAll.sql,v 1.1 2002-03-01 16:54:24 andreav Exp $ */
set pagesize 80;
ttitle 'List all tables in the catalog:';
select * from cat;
quit;
