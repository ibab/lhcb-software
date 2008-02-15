create or replace function DecomposeFunctionID(funcID number) return varchar2 AUTHID CURRENT_USER is
fct_name  varchar2(200);
result_query varchar2(2000);
i number:=1;
rec_count number;
source_cursor number;
stmt varchar2(2000);
begin
if funcID>0 then
source_cursor:=dbms_sql.open_cursor;
dbms_sql.parse(source_cursor,'select function_name from lhcb_device_functions where mod(:fctID,functionID)=0 ',dbms_sql.native);
dbms_sql.bind_variable(source_cursor,':fctID',funcID);
rec_count:=dbms_sql.execute(source_cursor);
dbms_sql.define_column(source_cursor,1,fct_name,200);
loop
	EXIT WHEN DBMS_SQL.FETCH_ROWS (source_cursor)=0;
	dbms_output.put_line('in the loop');

	DBMS_SQL.COLUMN_VALUE (source_cursor, 1,fct_name);
	if (i=1) then
		result_query:=fct_name;
	else
		result_query:=result_query||','||fct_name;
	end if;
	i:=i+1;
end loop;
dbms_sql.close_cursor(source_cursor);
end if;
if i=1 then
result_query:='none';
end if;
return result_query;
exception
when NO_DATA_FOUND then
result_query:='none';
if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
dbms_sql.close_cursor(source_cursor);
end if;

return result_query;
when OTHERS then
result_query:='ERROR_'||sqlerrm;
if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
dbms_sql.close_cursor(source_cursor);
end if;

return result_query;

end DecomposeFunctionID;
/


create or replace function ComposeFunctionID(function_list varchar2) return number AUTHID CURRENT_USER is
TYPE devid_list IS TABLE OF number;
fctIDlist devid_list;
i number:=1;
fID number;
result_query number:=0;
fct_count number :=1;
functionName varchar2(2000):=','||function_list||',';
begin

loop
exit when instr(function_list,',',1,fct_count)=0;
fct_count:=fct_count+1;
end loop;

execute immediate 'select functionID from lhcb_device_functions where instr(:fctlist,'',''||function_name||'','')>0' bulk collect into fctIDlist using functionName;

if fct_count=fctIDlist.count() then
	for i in fctIDlist.first..fctIDlist.last loop
		if (i=1) then
			result_query:=fctIDlist(i);
		else
			fID:=fctIDlist(i);
			result_query:=result_query*fID;
		end if;
	end loop;
else
	result_query:=-2;
end if;

if  fctIDlist.count()=0 then
	result_query:=-1;
end if;
return result_query;

exception
when NO_DATA_FOUND then
result_query:=-1;
return result_query;
when OTHERS then
if(sqlcode>0) then
result_query:=-sqlcode;
end if;
return result_query;

end ComposeFunctionID;
/

CREATE or replace TRIGGER uni_snb_inuse_tg  before INSERT or UPDATE OF SERIALNB ON lhcb_lg_devices for each row 
DECLARE
count_snb number:=0;
count_prim number:=0;
begin
if :new.serialnb is not NULL then
	select count(*) into count_snb  from lhcb_device_history where serialnb=:new.serialnb and device_status='IN_USE' and status_change=(select max(status_change) from lhcb_device_history where serialnb=:new.serialnb);
	
	if (count_snb >0) then
		
    		raise_application_error( -20501, 'HW Device '||:new.serialnb||' already in use and'); 
	end if;
end if;
	

END uni_snb_inuse_tg;
/


CREATE or replace TRIGGER uni_ipalias_tg BEFORE INSERT or UPDATE OF ipalias ON lhcb_ipaliases for each row 
DECLARE
count_snb number:=0;
count_prim number:=0;
begin
if :new.ipalias is not NULL then
	select count(*) into count_snb  from lhcb_ipinfo where ipname=:new.ipalias ;
	
	if (count_snb >0) then
		
    		raise_application_error( -20501, 'IP alias already exists as an IP name'); 
	end if;
end if;
	
--dbms_output.put_line('value of '||count_bmc);

END uni_ipalias_tg;
/



CREATE or replace TRIGGER existing_portid_tg BEFORE INSERT or UPDATE  ON lhcb_microscopic_connectivity for each row 
DECLARE
count_snb number:=0;
count_prim number:=0;
begin
if :new.cpntidfrom=-1 then
	select count(*) into count_snb  from lhcb_port_properties where portid=:new.portidfrom ;
	
	if (count_snb=0) then
		
    		raise_application_error( -20501, 'Portid from does not exist'); 
	else
		if :new.cpntidto=-1 then
			select deviceid into count_snb  from lhcb_port_properties where portid=:new.portidfrom ;
		end if;
	end if;
end if;
	
if :new.cpntidto=-1 then
	select count(*) into count_prim  from lhcb_port_properties where portid=:new.portidto ;
	
	if (count_prim=0) then
		raise_application_error( -20501, 'Portid to does not exist'); 
	else
		if :new.cpntidfrom=-1 then
			select deviceid into count_prim  from lhcb_port_properties where portid=:new.portidto ;
			if count_snb!=count_prim then
				raise_application_error( -20501, 'The two portids do not belong to the same device'); 
			end if;
		end if;
	end if;
end if;
	

--dbms_output.put_line('value of '||count_bmc);

END existing_portid_tg;
/

create or replace function UpdateDeviceHistory(old_serialnb varchar2, new_serialnb varchar2, deviceid number) return number AUTHID CURRENT_USER is
fct_name  varchar2(200);
result_query varchar2(2000);
i number:=1;
rec_count number:=0;
histID number:=0;
TYPE devid_list IS TABLE OF number;
histIDlist devid_list;
stmt varchar2(2000);
begin

execute immediate 'select historydeviceid from lhcb_device_history where serialnb=:snbd' bulk collect into histIDlist using old_serialnb;

execute immediate 'update lhcb_device_history set serialnb=NULL where serialnb=:snb' using old_serialnb;

execute immediate 'update lhcb_hw_devices set serialnb=:newsnb where serialnb=:oldsnb' using new_serialnb,old_serialnb;
if(deviceid>0) then
	execute immediate 'update lhcb_lg_devices set serialnb=:newsnb where deviceid=:devid' using new_serialnb,deviceid;
end if;
if(SQL%ROWCOUNT=1) then
	rec_count:=histIDlist.count();
	if rec_count>0 then
		for i in histIDlist.first..histIDlist.last loop
			histID:=histIDlist(i);
			execute immediate 'update lhcb_device_history set serialnb=:newsnb where historydeviceid=:hist' using new_serialnb,histID;
		end loop;
		result_query:=0;
	else
		rollback;
		dbms_output.put_line('value of recocunt'||rec_count);
		result_query:=-1;
	end if;
else
	rollback;
	dbms_output.put_line('value of sqlrount');
	result_query:=-1;
end if;

return result_query;
exception
when NO_DATA_FOUND then
rollback;
dbms_output.put_line('no data found');
result_query:=-1;

return result_query;
when OTHERS then
result_query:=sqlcode;
rollback;
return result_query;

end UpdateDeviceHistory;
/


create or replace function InsertSubsystem(sysname varchar2,parent_name varchar2) return varchar2 AUTHID CURRENT_USER is
next_prime number:=1;
sysID number;
parent_sysID number;
result_query varchar2(2000);
sysID_total number;
step number :=0;
begin
select prime_nb into sysID from lhcb_prime_number where rownum=1 and  prime_nb not in (select t.prime_nb from lhcb_prime_number t, lhcb_subsystemlist e where mod(e.systemid,t.prime_nb)=0); 

if parent_name='none' then --it's a simple system like DAQ
	step:=1;
	execute immediate 'insert into lhcb_subsystemlist(system_name,systemid) values(:sysname,:sysid)' using sysname,sysID;
else
	step:=1;
	execute immediate 'select systemid from lhcb_subsystemlist where system_name=:sysname' into parent_sysID using parent_name;
	sysID_total:=sysID*parent_sysID;
	execute immediate 'insert into lhcb_subsystemlist(system_name,systemid) values(:sysname,:sysid)' using sysname,sysID_total;
end if;

commit;
result_query:=' NO_ERROR';
return result_query;
exception
when NO_DATA_FOUND then
if step=1 then
result_query:='PARENT SYSTEM NOT FOUND';
else
result_query:='NO PRIME NUMBERS LEFT: PLEASE ADD SOME';

end if;

return result_query;
when OTHERS then
result_query:='ERROR_'||sqlerrm;

return result_query;

end InsertSubsystem;
/

