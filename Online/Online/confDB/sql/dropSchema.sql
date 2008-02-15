create or replace function DropTableSchema(system_name1 varchar2) return number AUTHID CURRENT_USER is
resfct number;
tabname varchar2(100);
fdbk number;
tabname_cur varchar2(100);
nbtab_created number:=0;
nbtab_created1 number:=0;
nbtab_created2 number:=0;
i number;
source_cursor number:=dbms_sql.open_cursor;
system_name varchar2:='all_';
begin
tabname:=upper(system_name||'%booting');
execute immediate 'select count(table_name) from user_tables where table_name like upper(:tab1)' into i using tabname;
if(i=2) then
	execute immediate 'drop table '||system_name||'device_booting';
	execute immediate 'drop table '||system_name||'devicetype_booting';
else
	if(i=1) then
		execute immediate 'select table_name from user_tables where table_name like upper(:tab1)' into tabname_cur using tabname;
		execute immediate 'drop table '||tabname_cur;
	end if;
end if;

--tabname:=upper(system_name||'routing%');
--execute immediate 'select count(table_name) from user_tables where table_name like upper(:tab1)' into i using tabname;
--if(i=3) then
	--execute immediate 'drop table '||system_name||'routingtable';
	--execute immediate 'drop table '||system_name||'routingtab_temporary';
	--execute immediate 'drop table '||system_name||'routingtabcopy_temporary';
--else
	--	if(i>0) then
		--	execute immediate 'select table_name from user_tables where table_name like upper(:tab1) and rownum=1' into tabname_cur using tabname;
		--	execute immediate 'drop table '||tabname_cur;
		--	if(i=2) then
		--		execute immediate 'select table_name from user_tables where table_name like upper(:tab1) and rownum=1' into tabname_cur using tabname;
		--		execute immediate 'drop table '||tabname_cur;
		--	end if;
	--	end if;
--end if;

--tabname:=upper(system_name||'trunk_link');
--execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;
--if(i=1) then
--	execute immediate 'drop table '||tabname;
--	execute immediate 'drop sequence '||system_name||'trunk_link_LINK_TRUNKID';
--end if;
	



tabname:=upper(system_name||'logical_links');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;

if(i=1) then
	execute immediate 'drop table '||tabname;
end if;

tabname:=upper(system_name||'linkpairs');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;

if(i=1) then
	execute immediate 'drop table '||tabname;
end if;

tabname:=upper(system_name||'prime_number');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;
if(i=1) then
	execute immediate 'drop table '||tabname;
end if;

tabname:=upper(system_name||'path_temp');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;
if(i=1) then
	execute immediate 'drop table '||tabname;
end if;

tabname:=upper(system_name||'destinationtable');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;
if(i=1) then
	execute immediate 'drop table '||tabname;
end if;
tabname:=upper(system_name||'PATHDETAILSWITHPORTS');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;

if(i=1) then
	execute immediate 'drop table '||tabname;
end if;
tabname:=upper(system_name||'pathdetailswithlkids');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;

if(i=1) then
	execute immediate 'drop table '||tabname;
end if;

tabname:=upper(system_name||'pathdetails');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;

if(i=1) then
	execute immediate 'drop table '||tabname;
end if;

tabname:=upper(system_name||'pathdetails_temporary');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;
if(i=1) then
	execute immediate 'drop table '||tabname;
end if;
tabname:=upper(system_name||'destintab_temporary');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;
if(i=1) then
	execute immediate 'drop table '||tabname;
end if;

tabname:=upper(system_name||'startend_temporary');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;
if(i=1) then
	execute immediate 'drop table '||tabname;
end if;

tabname:=upper(system_name||'connectivity');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;

if(i=1) then
	execute immediate 'drop table '||tabname;
	execute immediate 'drop sequence '||system_name||'connectivity_linkid';
end if;


tabname:=upper(system_name||'port_properties');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;
if(i=1) then
	execute immediate 'drop table '||tabname;
	execute immediate 'drop sequence '||system_name||'port_properties_portid';
end if;

tabname:=upper(system_name||'ipinfo');
execute immediate 'select count(table_name) from user_tables where table_name = upper(:tab1)' into fdbk using tabname;
if(fdbk=1) then
	execute immediate 'drop table '||tabname;
end if;



tabname:=upper(system_name||'link_type');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;
if(i=1) then
	execute immediate 'drop table '||tabname;
	execute immediate 'drop sequence '||system_name||'lktype_linkid';

end if;
	
tabname:=upper(system_name||'device_history');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;
if(i=1) then
	execute immediate 'drop table '||tabname;
	execute immediate 'drop sequence '||system_name||'devicehistory_deviceid';
end if;

tabname:=upper(system_name||'device');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;
if(i=1) then
	execute immediate 'drop table '||tabname;
	execute immediate 'drop sequence '||system_name||'device_deviceid';

end if;
tabname:=upper(system_name||'connector_properties');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;
if(i=1) then
	execute immediate 'drop table '||tabname;
end if;
tabname:=upper(system_name||'device_type');
execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into i using tabname;
if(i=1) then
	execute immediate 'drop table '||tabname;
	execute immediate 'drop sequence '||system_name||'devtype_dtypeid';

end if;
	

resfct:=0;
return resfct;
exception
when NO_DATA_FOUND then
resfct:=-1;
return resfct;
when OTHERS then
resfct:=sqlcode;
return resfct;	
end DropTableSchema;



