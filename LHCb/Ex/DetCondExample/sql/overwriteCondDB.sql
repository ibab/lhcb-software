/* $Id: overwriteCondDB.sql,v 1.1 2002-03-01 16:54:24 andreav Exp $ */
select * from condition_dbs where condb_name='&1';
update condition_dbs set status=1 where condb_name='&1';
commit;
select * from condition_dbs where condb_name='&1';
quit;
