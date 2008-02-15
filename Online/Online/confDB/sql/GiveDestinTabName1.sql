
-------------------------------------------------------case for devicetype-------------------------------------------------
create or replace function GiveDestinTabName1(sysname varchar2,devfrom varchar2, devtype varchar2, devtype_given number) return varchar2 AUTHID CURRENT_USER is
tabname varchar2(100);
system_name1 number;
system_name2 number;
port_way number;
rec_count varchar2(100);
deviceid number:=0;
fdbk number;
tabname_cur varchar2(100):='';
tabname_cur1 varchar2(100):='lhcb_path_temp';
rescount number:=0;
devname_like varchar2(500);
i number:=0;
j number:=0;
stmtres varchar2(100);
resquery number:=0;
case_res number:=-1;
source_cursor number:=dbms_sql.open_cursor;
begin
if(devtype_given=1) then
	devname_like:=devtype;
	case_res:=3;
else
	devname_like:=devtype||'%';
	case_res:=4;
end if;
--stmtres:=pathqueries_pck.CheckAndUpdateDynamicTables (sysname ,0);
--if(instr(stmtres,'ERROR')>0) then
--	return stmtres;
--end if;

execute immediate 'select t.deviceid,t.system_name from lhcb_logical_device t where t.devicename=:devfrom' into deviceid,system_name1 using devfrom;


if(mod(system_name,41)=0) then --check daq belonging if yes possibility in routing table
	dbms_sql.parse(source_cursor,'select distinct t.port_nbr,t.port_way from lhcb_routingtable p,lhcb_port_table t where p.nodeid_start0=:dID and t.portid=p.pfromid0 and  p.nodeid_end1 in (select deviceid from lhcb_logical_device where devicename like upper(:devtype))',dbms_sql.native);
end if;
dbms_sql.bind_variable(source_cursor,':devtype',devname_like);
dbms_sql.bind_variable(source_cursor,':dID',deviceid);
rec_count:=dbms_sql.execute(source_cursor);
dbms_sql.define_column(source_cursor,1,rec_count);
dbms_sql.define_column(source_cursor,1,port_way);
LOOP
	EXIT WHEN DBMS_SQL.FETCH_ROWS (source_cursor)=0;
	DBMS_SQL.COLUMN_VALUE (source_cursor, 1, rec_count);
	DBMS_SQL.COLUMN_VALUE (source_cursor, 1, port_way);
	if j=0 then
		tabname:='|'||rec_count||':'||port_way;
	else
		tabname:=tabname||'|'||rec_count;
	end if;
	j:=j+1;
END LOOP;
dbms_sql.close_cursor(source_cursor);	

if j=0 then --if j is not equal to 0 already sth found...
	dbms_sql.parse(source_cursor,'select distinct t.port_nbr,t.port_way from lhcb_destinationtable p,lhcb_port_table t where p.nodeid_start0=:dID and t.portid=p.pfromid0 and  p.nodeid_end1 in (select deviceid from lhcb_logical_device where devicename like upper(:devtype))',dbms_sql.native);
end if;
dbms_sql.bind_variable(source_cursor,':devtype',devname_like);
dbms_sql.bind_variable(source_cursor,':dID',deviceid);
rec_count:=dbms_sql.execute(source_cursor);
dbms_sql.define_column(source_cursor,1,rec_count);
dbms_sql.define_column(source_cursor,1,port_way);
LOOP
	EXIT WHEN DBMS_SQL.FETCH_ROWS (source_cursor)=0;
	DBMS_SQL.COLUMN_VALUE (source_cursor, 1, rec_count);
	DBMS_SQL.COLUMN_VALUE (source_cursor, 1, port_way);
	if j=0 then
		tabname:='|'||rec_count||':'||port_way;
	else
		tabname:=tabname||'|'||rec_count;
	end if;
	j:=j+1;
END LOOP;
dbms_sql.close_cursor(source_cursor);	


	
if(j=0) then
tabname:='ERROR_NO_CONNECTIVITY';
else
tabname:=tabname||'?';
end if;
return tabname;
		
exception
when NO_DATA_FOUND then
tabname:='ERROR_NO_DATA_FOUND';
if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
dbms_sql.close_cursor(source_cursor);
end if;
return tabname;
when OTHERS then
tabname:='ERROR_'||sqlerrm;
if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
dbms_sql.close_cursor(source_cursor);
end if;
return tabname;
end GiveDestinTabName1;

