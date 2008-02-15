create or replace function GiveDestinTabName1_view(sysname varchar2,devfrom varchar2, devtype varchar2, devtype_given number) return varchar2 AUTHID CURRENT_USER is
tabname varchar2(100);

rec_count number;
deviceid number:=0;
fdbk number;
tabname_cur varchar2(100):='';
tabname_cur1 varchar2(100):=sysname||'path_temp_view';
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
	case_res:=1;
else
	devname_like:=devtype||'%';
	case_res:=0;
end if;
--stmtres:=pathqueries_pck.CheckAndUpdateDynamicTables (sysname ,0);
--if(instr(stmtres,'ERROR')>0) then
--	return stmtres;
--end if;

execute immediate 'select deviceid from '||sysname||'device_view where upper(devicename)=upper(:devfrom)' into deviceid using devfrom;
	
execute immediate 'select count(t.pathid) from '||sysname||'pathdetails_view t, '||sysname||'device_view e where t.node1=:devid and rownum=1' into rescount using deviceid;
if(rescount=1) then
	execute immediate 'select count(t.pathid) from '||sysname||'destinationtable_view t, '||sysname||'device_view e where t.nodeid_start0=:devid and rownum=1' into fdbk using deviceid;
	if fdbk=1 then
		case_res:=case_res+2; --means we have a destination table
	end if;	
else
	fdbk:=routingtable_pck_view.createTD_all(sysname,deviceid);
	if(fdbk=0) then
		case_res:=case_res+2; --means destination table
		
	else
		case_res:=-1;
	end if;
end if;
--DBMS_OUTPUT.PUT_LINE ('value of case_res '||case_res||' and devlike '||fdbk);
case case_res
	when 0 then --means routing table + devicename like
			dbms_sql.parse(source_cursor,'select distinct port_nb from '||sysname||'routingtable_view where nodeid_start0=:dID and  nodeid_end1 in (select deviceid from '||sysname||'device_view where upper(devicename) like upper(:devtype))',dbms_sql.native);
	when 1 then --means routing table + devicetype
			dbms_sql.parse(source_cursor,'select distinct port_nb from '||sysname||'routingtable_view where nodeid_start0=:dID and  nodeid_end1 in (select t.deviceid from '||sysname||'device_view t, '||sysname||'device_type_view f where t.devicetypeID=f.devicetypeID and f.devicetype=:devtype)',dbms_sql.native);
	when 2 then --means destinationtable + devicename like
			
			dbms_sql.parse(source_cursor,'select distinct port_nb from '||sysname||'destinationtable_view where nodeid_start0=:dID and   nodeid_end1 in (select deviceid from '||sysname||'device_view where upper(devicename) like upper(:devtype))',dbms_sql.native);
	when 3 then --means destinationtable + devicetype
			dbms_sql.parse(source_cursor,'select distinct port_nb from '||sysname||'destinationtable_view where nodeid_start0=:dID and   nodeid_end1 in (select t.deviceid from '||sysname||'device_view t ,'||sysname||'device_type_view f where  t.devicetypeID=f.devicetypeID and f.devicetype=:devtype)',dbms_sql.native);
	else
		tabname_cur:='ERROR_NO_CONNECTIVITY';
		j:=-1;
end case;
if(case_res>-1) then
	dbms_sql.bind_variable(source_cursor,':devtype',devname_like);
	dbms_sql.bind_variable(source_cursor,':dID',deviceid);
	rec_count:=dbms_sql.execute(source_cursor);
	dbms_sql.define_column(source_cursor,1,rec_count);
	LOOP
		EXIT WHEN DBMS_SQL.FETCH_ROWS (source_cursor)=0;
		DBMS_SQL.COLUMN_VALUE (source_cursor, 1, rec_count);
		tabname_cur:=tabname_cur||'|'||rec_count;
		j:=j+1;
	END LOOP;
	dbms_sql.close_cursor(source_cursor);	
end if;
	
if(j=0) then
tabname_cur:='ERROR_NO_CONNECTIVITY';
else
tabname_cur:=tabname_cur||'|';
end if;
return tabname_cur;
		
exception
when NO_DATA_FOUND then
tabname_cur:='ERROR_NO_DATA_FOUND';
if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
dbms_sql.close_cursor(source_cursor);
end if;
return tabname_cur;
when OTHERS then
tabname_cur:='ERROR_'||sqlerrm;
if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
dbms_sql.close_cursor(source_cursor);
end if;
return tabname_cur;
end GiveDestinTabName1_view;
