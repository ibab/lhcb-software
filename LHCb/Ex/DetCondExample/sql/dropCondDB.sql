/* $Id: dropCondDB.sql,v 1.1 2002-03-01 16:54:24 andreav Exp $ */
/* Drop all views, tables, sequences and rows associated with a CondDB  */
/* (the order is important: a table may be referenced in a previos one) */
/* Drop also the code used to manage the database                       */

define USER=&1;
define CONDDB=&2;

/* Use "select * from cat" to list views, tables, sequences and rows    */

drop materialized view "&USER"."&CONDDB._FLPATHS";
drop materialized view "&USER"."&CONDDB._FSPATHS";
drop materialized view "&USER"."&CONDDB._HEADERS";

drop table    MLOG$_&CONDDB._CONOBJS;

drop sequence &CONDDB._CDATSEQ;
drop sequence &CONDDB._COBJSEQ;
drop sequence &CONDDB._FLDSEQ;
drop sequence &CONDDB._FSSEQ;
drop sequence &CONDDB._TAGSEQ;

drop table    &CONDDB._FLDTAGS;
drop table    &CONDDB._OBJTAGS;

drop table    &CONDDB._CONOBJS;
drop table    &CONDDB._CONTAGS;
drop table    &CONDDB._FOLDERS;
drop table    &CONDDB._INTDATS;

drop table    &CONDDB._FLDSETS;

delete from condition_dbs where condb_name='&CONDDB';

/* Use "select * from user_source" to list pl/sql user code             */

drop package  &CONDDB._PACKAGE;

quit;


