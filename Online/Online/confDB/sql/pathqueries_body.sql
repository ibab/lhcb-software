---------------------------------------------
--PL/SQL path queries packages
--author L.Abadie
--v2.0
------------------------------------
create or replace package body pathqueries_pck
is
-----------------------------------------------------------
--Convention we always give sysname=sysname+'_': i.e. if it's VELO, we give VELO_
-----------------------------------------------------------
----------------------------------------------------------
----fct which creates the logical view of the system : we just keep link between devices (get rid of port nb)
----------------------------------------------------------

function CheckPathTabCompleteAndUpdate(systemname in number,all_details number default 0)  return varchar2 is
tabname varchar2(100);
node number:=0;
resquery1 varchar2(100);
resquery number;
resquery3 varchar2(100):='NONE';
resquery2 varchar2(100):='NONE';
time_before number;
result_fct varchar2(100);
daq_sysid number;
tfc_sysid number;

begin
--first check if the path table is updated or not
time_before := DBMS_UTILITY.GET_TIME;
select systemid into daq_sysid from lhcb_subsystemlist where system_name='DAQ' ;
select systemid into tfc_sysid from lhcb_subsystemlist where system_name='TFC' ;

if(mod(systemname,daq_sysid)=0 or mod(systemname,tfc_sysid)=0) then
	resquery:=pathqueries_pck.checkUpdatePathTable(systemname);
	--DBMS_OUTPUT.PUT_LINE ('time execution to check update tab '|| (DBMS_UTILITY.GET_TIME-time_before)/100);

	-- then if pathtable is complete
	if(resquery=0) then
		resquery1:=pathqueries_pck.checkCompletePathTable(systemname);
		if (resquery1='NEED_TO_UPDATE') then
			node:=1;
			--savepoint KeyTab;
			result_fct:=pathqueries_pck.generKeyDTab(systemname);
			if(result_fct='NODE_TO' or result_fct='NODE_FROM') then
				--DBMS_OUTPUT.PUT_LINE ('time execution to check complete tab '|| (DBMS_UTILITY.GET_TIME-time_before)/100);
				resquery2:=pathqueries_pck.MapPathTabWithLkIDs(systemname);
				if (all_details=1) then
					resquery3:=pathqueries_pck.MapPathTabWithPorts(systemname);
				end if;
				if(instr(resquery2,'ERROR')>0 or instr(resquery3,'ERROR')>0) then
					result_fct:=resquery2;
					rollback ;
				else
					commit;
				end if;
			else
				rollback ;
			end if;	
		else
			result_fct:=resquery1;
		end if;
	else
		node:=1;
		--savepoint KeyTab;
		result_fct:=pathqueries_pck.generKeyDTab(systemname);
		if(result_fct='NODE_TO' or result_fct='NODE_FROM') then
			--DBMS_OUTPUT.PUT_LINE ('time execution to check update tab 1'|| (DBMS_UTILITY.GET_TIME-time_before)/100);
			resquery2:=pathqueries_pck.MapPathTabWithLkIDs(systemname);
			--if (all_details=1) then
			--	resquery3:=pathqueries_pck.MapPathTabWithPorts(systemname);
			--end if;
			if(instr(resquery2,'ERROR')>0 or instr(resquery3,'ERROR')>0) then
				result_fct:=resquery2;
				rollback ;
			else
				commit;
			end if;
		else
			rollback ;
		end if;		
	end if;
else
result_fct:='NO_NEED';
end if;
return result_fct;
exception
when NO_DATA_FOUND then
result_fct:='ERROR_NO_DATA_FOUND';
if node=1 then
rollback;
end if;

return result_fct;
when OTHERS then
--need_tabupdate:=sqlcode;
result_fct:='ERROR_'||sqlerrm;
if node=1 then
rollback ;
end if;
return result_fct;
end CheckPathTabCompleteAndUpdate;

---------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
--check if the pathtable is complete or not to find all paths which cross a given device.
--need also to know if we need to search for the first link or not
-------------------------------------------------------------------------
function checkCompletePathTable(systemname number) return varchar2 is
diff_lastupdate number;
--need_tabupdate number;
v_error_message varchar2(100);
pathtab varchar2(100):='lhcb_pdetails';
conntab varchar2(100):='lhcb_macro_connectivity';
begin
execute immediate ' select count(linkid) into diff_lastupdate from lhcb_macro_connectivity  where link_weight=1 and node_to not in (select node1 from lhcb_pdetails) and rownum=1 and mod(system_name,:sysid)=0' using systemname;
if(diff_lastupdate=0) then
	v_error_message:='NODE_TO';
else
execute immediate ' select count(linkid) into diff_lastupdate from lhcb_macro_connectivity  where link_weight=1 and node_from not in (select node1 from lhcb_pdetails) and rownum=1 and mod(system_name,:sysid)=0' using systemname;
	if(diff_lastupdate=0) then
		--need_tabupdate=0;
		v_error_message:='NODE_FROM';
	else
		v_error_message:='NEED_TO_UPDATE';
	end if;
end if;
return v_error_message;
exception
when NO_DATA_FOUND then
--need_tabupdate:=-1;
v_error_message:='ERROR_NO_DATA_FOUND';
return v_error_message;

when OTHERS then
--need_tabupdate:=sqlcode;
v_error_message:='ERROR_'||sqlerrm;
return v_error_message;
end checkCompletePathTable;


----------------------------------------------------------------------------------------------------
--check if the pathtable is updated or not compared to connectivity
-------------------------------------------------------------------------
function checkUpdatePathTable(systemname number) return number is
diff_lastupdate number;
need_tabupdate number:=-1;
v_error_message varchar2(100);
minval date;
maxval date;
pathtab varchar2(100):='lhcb_pdetails';
connectivitytab varchar2(100):='lhcb_macro_connectivity';
begin

execute immediate 'select max(last_update) from lhcb_macro_connectivity where mod(system_name,:sysID)=0' into maxval using systemname;

execute immediate 'select min(last_update) from  lhcb_pdetails where mod(system_name,:sysID)=0' into minval using systemname;

diff_lastupdate:=minval-maxval;

if(diff_lastupdate>0) then
	need_tabupdate:=0;
else
	need_tabupdate:=1;
end if;
return need_tabupdate;
exception
when NO_DATA_FOUND then
need_tabupdate:=-1;
return need_tabupdate;
when OTHERS then
need_tabupdate:=sqlcode;
v_error_message:='ERROR_'||sqlerrm;
return need_tabupdate;
end checkUpdatepathtable;
-------------------------------------------------------------------------------------------------------------
--generate and check existence of key tables
----------------------------------------------------------------------------------------------------
function generKeyDTab(systemname in number) return varchar2  is
tabname varchar2(100);
pathtab varchar2(100):='lhcb_pdetails'; 
destintab varchar2(100):='lhcb_destinationtable'; 
routingtab varchar2(100):='lhcb_routingtable';
devicetab varchar2(100):='lhcb_lg_device';
connectivitytab varchar2(100):='lhcb_macro_connectivity';
IPtab varchar2(100):='lhcb_port_properties';
source_cursor number:=dbms_sql.open_cursor;
node number;
nodeid number;
nodeto_count number;
nodefrom_count number;
i number:=0;
resquery number:=0;
exist_iptab number:=0;
limit_node number;
rec_count number;
rescreate number:=0;
v_error_message varchar2(100);
begin
--first check if it's a one-to-one connection or many-to-one
--it's to reduce the number of useless DTs 
--delete old rows if it exists...bevause if we enter this fct, means we need either to update or to populate
execute immediate 'delete from '||routingtab||' where mod(system_name,:sysID)=0 and (nodeid_start0,pathid) in (select distinct node1,pathid from '||pathtab||' where path_necessary=1 and mod(system_name,:sysID)=0)' using systemname,systemname;
execute immediate 'delete from '||destintab||' where mod(system_name,:sysID)=0 and (nodeid_start0,pathid) in (select distinct node1,pathid from '||pathtab||' where path_necessary=1 and mod(system_name,:sysID)=0)'using systemname,systemname;
execute immediate 'delete from lhcb_pdetwithlkids mod(system_name,:sysID)=0' using systemname;
--execute immediate 'truncate table '||systemname||'pathdetailswithports';

execute immediate 'delete from '||pathtab||' where path_necessary=1 and mod(system_name,:sysID)=0' using systemname;
execute immediate 'select count(distinct node_to), count(distinct node_from) from '||connectivitytab||' where link_weight=1' into nodeto_count,nodefrom_count ;
if(nodeto_count< nodefrom_count) then
	limit_node:=nodeto_count;
	v_error_message:='NODE_TO';
	execute immediate 'select count(table_name) from user_tables where table_name=upper(:1)' into exist_iptab using IPtab;
	dbms_sql.parse(source_cursor,'select distinct node_to from '||connectivitytab||' where link_weight=1 and node_to not in (select node1 from '||pathtab||')',dbms_sql.native);
else
	limit_node:=nodefrom_count;
	v_error_message:='NODE_FROM';
	dbms_sql.parse(source_cursor,'select distinct node_from from '||connectivitytab||' where link_weight=1 and node_from not in (select node1 from '||pathtab||')',dbms_sql.native);
end if;	
--dbms_sql.bind_variable(source_cursor,':dfrom',devfrom);
--dbms_sql.bind_variable(source_cursor,':dto',devto);
rec_count:=dbms_sql.execute(source_cursor);
dbms_sql.define_column(source_cursor,1,nodeid);
--dbms_sql.define_column(source_cursor,2,node1);

while (i<nodeto_count) LOOP
	EXIT WHEN DBMS_SQL.FETCH_ROWS (source_cursor)=0;
	DBMS_SQL.COLUMN_VALUE (source_cursor, 1, nodeid);
	if i=0 then
		rescreate:=1;
	end if;
	if(exist_iptab=1) then --case we have ip_ethernet tab
		resquery:=routingtable_pck.createRT_all(nodeid,1,10,rescreate);
		rescreate:=0;
	else
		resquery:=routingtable_pck.createTD_all(nodeid,1,10,rescreate);
	end if;
--DBMS_OUTPUT.PUT_LINE ('generate key tabs '|| nodeid||' and resqueyr '||resquery);


	if(resquery=0) then
		i:=i+1;
	else
		if(exist_iptab=1) then
			v_error_message:='ERROR_IN_RT_OF_'||nodeid;
		else
			v_error_message:='ERROR_IN_DT'||nodeid;
		end if;
		i:=nodeto_count+10;
	end if;
END LOOP;
dbms_sql.close_cursor(source_cursor);

return v_error_message;

exception
when NO_DATA_FOUND then
resquery:=-1;
v_error_message:='ERROR_NO_DATA_FOUND';
if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
	dbms_sql.close_cursor(source_cursor);
end if;
return v_error_message;
when OTHERS then
resquery:=sqlcode;
if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
dbms_sql.close_cursor(source_cursor);
end if;
v_error_message:='ERROR_'||sqlerrm;
return v_error_message;
end generKeyDTab;
------------------------------------------------------------------------------------------
function UpdatePathTables (systemname in number) return varchar2 is
tabname varchar2(100);

pathtab varchar2(100):='lhcb_pdetails'; 
destintab varchar2(100):='lhcb_destinationtable';
routingtab varchar2(100):='lhcb_routingtable';
connectivitytab varchar2(100):='lhcb_macro_connectivity';
sqlquery varchar2(1000);
i number:=0;
resquery number:=0;
j number :=0;
source_cursor number:=dbms_sql.open_cursor;
rec_count number:=0;
v_error_message varchar2(2000):='SUCCESSFUL_UPDATE';
nodeID number;
rescreate number;
begin
dbms_sql.parse(source_cursor,'select distinct NODEID_START0 from '||destintab||' t,'||pathtab||' e where mod(t.system_name,:sys)=0 and e.path_necessary=0 and e.node1=t.nodeid_start0 and e.pathid=t.pathid',dbms_sql.native);
rec_count:=dbms_sql.execute(source_cursor);
dbms_sql.define_column(source_cursor,1,nodeID);
LOOP
	EXIT WHEN DBMS_SQL.FETCH_ROWS (source_cursor)=0;
	DBMS_SQL.COLUMN_VALUE (source_cursor, 1, nodeID);
	if i=0 then
		rescreate:=1;
	else
		rescreate:=0	
	end if;
	j:=routingtable_pck.createTD_all(nodeID,0,10,rescreate);
	if(j!=0) then
		exit;
		v_error_message:='ERROR_createTD_all_'||nodeID||'_errnb_'||j;
		dbms_sql.close_cursor(source_cursor);
		return v_error_message;
	end if;
	i:=i+1;
END LOOP;
i:=0;
dbms_sql.parse(source_cursor,'select distinct NODEID_START0 from '||routingtab||' t,'||pathtab||' e where mod(e.system_name,:sys)=0 and e.path_necessary=0 and e.node1=t.nodeid_start0 and e.pathid=t.pathid',dbms_sql.native);
rec_count:=dbms_sql.execute(source_cursor);
dbms_sql.define_column(source_cursor,1,nodeID);
LOOP
	EXIT WHEN DBMS_SQL.FETCH_ROWS (source_cursor)=0;
	DBMS_SQL.COLUMN_VALUE (source_cursor, 1, nodeID);
	if i=0 then
		rescreate:=1;
	else
		rescreate:=0	
	end if;
	j:=routingtable_pck.createRT_all(nodeID,0,10,rescreate);
	if(j!=0) then
		exit;
		v_error_message:='ERROR_createRT_all_'||nodeID||'_errnb_'||j;
		dbms_sql.close_cursor(source_cursor);
		return v_error_message;
	end if;
	i:=i+1;
END LOOP;
dbms_sql.close_cursor(source_cursor);
return v_error_message;

exception
when NO_DATA_FOUND then
j:=-1;
v_error_message:='ERROR_NO_DATA_FOUND';
if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
	dbms_sql.close_cursor(source_cursor);
end if;
return v_error_message;
when OTHERS then
j:=sqlcode;
if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
dbms_sql.close_cursor(source_cursor);
end if;
v_error_message:='ERROR_'||sqlerrm;
return v_error_message;	
end UpdatePathTables;
-------------------------------------------------
--Check and update all the dynamic tables :
-------------------------------------------------------------------------------------------------------------
function CheckAndUpdateDynamicTables (systemname in number,all_details number default 0) return varchar2 is
tabname varchar2(100);


i number:=0;
resquery number:=-1;
j number :=0;
rec_count number:=-1;
resstmt varchar2(1000);
v_error_message varchar2(2000):='SUCCESSFUL_UPDATE';
nodeID number;
daq_sysid number;
tfc_sysid number;
begin
select systemid into daq_sysid from lhcb_subsystemlist where system_name='DAQ' ;
select systemid into tfc_sysid from lhcb_subsystemlist where system_name='TFC' ;

if(mod(systemname,daq_sysid)=0 or mod(systemname,tfc_sysid)=0) then


resstmt:=pathqueries_pck.CheckPathTabCompleteAndUpdate(systemname,all_details);

if(instr(resstmt,'ERROR')=0) then
	v_error_message:=pathqueries_pck.UpdatePathTables(systemname);
else
	v_error_message:=resstmt;
end if;
else
v_error_message varchar2(2000):='SUCCESSFUL_UPDATE';
end if;
return v_error_message;

end CheckAndUpdateDynamicTables;
----------------------------------------------------------------------------------------------------
function MapPathTabWithPorts(systemname in number) return varchar2  is
tabname varchar2(100);
pathtab varchar2(100):='lhcb_pdetails'; 
pathportab varchar2(100):='lhcb_pdetailswithports';

connectivitytab varchar2(100):='lhcb_macro_connectivity';
sqlquery varchar2(1000);
source_cursor number:=dbms_sql.open_cursor;
i number:=0;
resquery number:=0;
j number :=0;

v_error_message varchar2(100):='SUCCESSFUL_CREATION';
begin
DBMS_OUTPUT.ENABLE( 10000000);
delete from lhcb_pdetails where mod(system_name,:sys)=0;

--first check if it's a one-to-one connection or many-to-one
--it's to reduce the number of useless DTs 
--savepoint PortMapping;
for i in 2..11 loop
	j:=i-1;
	--sqlquery:='select p.pathid,'||j||', p.node'||j||', case when l.linkid>0 then e.port_nbrfrom else e.port_nbrto end,p.node'||i||',case when l.linkid<0 then e.port_nbrfrom else e.port_nbrto end';
	--DBMS_OUTPUT.PUT_LINE('sql1 : '||sqlquery);
	--sqlquery:='from '||pathtab||' p, '||connectivitytab||' e ,'||systemname||'logical_links l ';
	--DBMS_OUTPUT.PUT_LINE('sql2 : '||sqlquery);
	--sqlquery:='where  l.node_from=p.node'||j||' and l.node_to=p.node'||i||' and (abs((p.node'||j||'-e.node_from)*(p.node'||i||'-e.node_from))+ abs((p.node'||i||'-e.node_to)*(p.node'||j||'-e.node_to)))=0  and p.path_necessary=1 and p.node'||i||'>0 and p.node'||j||'>0';
	--DBMS_OUTPUT.PUT_LINE('sql3 : '||sqlquery);
	
	--execute immediate 'insert into '||pathportab||'(pathid, link_pos , nfrom , pfrom, nto ,pto) select t.pathid,'||i||',p.node_from,p.port_nbrfrom,p.node_to,p.port_nbrto from '||pathtab||' t, '||connectivitytab||' p where t.node'||j||'=p.node_from and t.node'||i||'=p.node_to and t.node'||i||'>0'; 
	--execute immediate 'insert into '||pathportab||'(pathid, link_pos , nfrom , pfrom, nto ,pto,node1) select p.pathid,'||j||', p.node'||j||', case when l.linkid>0 then e.port_nbrfrom else e.port_nbrto end,p.node'||i||',case when l.linkid<0 then e.port_nbrfrom else e.port_nbrto end,p.node1 from '||pathtab||' p, '||connectivitytab||' e ,'||systemname||'logical_links l where  l.node_from=p.node'||j||' and l.node_to=p.node'||i||' and (abs((p.node'||j||'-e.node_from)*(p.node'||i||'-e.node_from))+ abs((p.node'||i||'-e.node_to)*(p.node'||j||'-e.node_to)))=0  and p.path_necessary=1 and p.node'||i||'>0 and p.node'||j||'>0';
	--execute immediate 'insert into '||pathportab||' select p.pathid,'||j||', case when l.linkid>0 then e.linkid else -e.linkid end from '||pathtab||' p, '||connectivitytab||' e ,'||systemname||'logical_links l where  l.node_from=p.node'||j||' and l.node_to=p.node'||i||' and (abs((p.node'||j||'-e.node_from)*(p.node'||i||'-e.node_from))+ abs((p.node'||i||'-e.node_to)*(p.node'||j||'-e.node_to)))=0  and p.path_necessary=1 and p.node'||i||'>0 and p.node'||j||'>0';

end loop;

return v_error_message;

exception
when NO_DATA_FOUND then
resquery:=-1;
v_error_message:='ERROR_NO_DATA_FOUND';
return v_error_message;
when OTHERS then
resquery:=sqlcode;
v_error_message:='ERROR_'||sqlerrm;
return v_error_message;
end MapPathTabWithPorts;

/***********************************************************************/
function MapPathTabWithLkIDs(systemname in varchar2) return varchar2  is
tabname varchar2(100);
pathtab varchar2(100):='lhcb_pdetails'; 
pathportab varchar2(100):='lhcb_pdetailswithlkIDs';

connectivitytab varchar2(100):='lhcb_macro_connectivity';
sqlquery varchar2(1000);
source_cursor number:=dbms_sql.open_cursor;
i number:=0;
resquery number:=0;
j number :=0;
--create table daq_pathdetailswithlkIDs (pathid number, link_pos number,linkid number);
v_error_message varchar2(100):='SUCCESSFUL_CREATION';
begin
DBMS_OUTPUT.ENABLE( 10000000);
--first check if it's a one-to-one connection or many-to-one
--it's to reduce the number of useless DTs 
--savepoint PortMapping;

for i in 2..11 loop
	j:=i-1;
		
	--execute immediate 'insert into '||pathportab||'(pathid, link_pos , nfrom , pfrom, nto ,pto) select t.pathid,'||i||',p.node_from,p.port_nbrfrom,p.node_to,p.port_nbrto from '||pathtab||' t, '||connectivitytab||' p where t.node'||j||'=p.node_from and t.node'||i||'=p.node_to and t.node'||i||'>0'; 
	--execute immediate 'insert into '||pathportab||' select p.pathid,'||j||', p.node'||j||', case when l.linkid>0 then e.port_nbrfrom else e.port_nbrto end,p.node'||i||',case when l.linkid<0 then e.port_nbrfrom else e.port_nbrto end from '||pathtab||' p, '||connectivitytab||' e ,'||systemname||'logical_links l where  l.node_from=p.node'||j||' and l.node_to=p.node'||i||' and (abs((p.node'||j||'-e.node_from)*(p.node'||i||'-e.node_from))+ abs((p.node'||i||'-e.node_to)*(p.node'||j||'-e.node_to)))=0  and p.path_necessary=1 and p.node'||i||'>0 and p.node'||j||'>0';
	execute immediate 'insert into lhcb_pdetailswithlkIDs(pathid,link_pos,linkid,node1) select p.pathid,'||j||', case when l.linkid>0 then e.linkid else -e.linkid end, p.node1 from '||pathtab||' p, '||connectivitytab||' e ,'||systemname||'logical_links l where  l.node_from=p.node'||j||' and l.node_to=p.node'||i||' and (abs((p.node'||j||'-e.node_from)*(p.node'||i||'-e.node_from))+ abs((p.node'||i||'-e.node_to)*(p.node'||j||'-e.node_to)))=0  and p.path_necessary=1 and p.node'||i||'>0 and p.node'||j||'>0';

end loop;

return v_error_message;

exception
when NO_DATA_FOUND then
resquery:=-1;
v_error_message:='ERROR_NO_DATA_FOUND';
return v_error_message;
when OTHERS then
resquery:=sqlcode;
v_error_message:='ERROR_'||sqlerrm;
return v_error_message;
end MapPathTabWithLkIDs;

end pathqueries_pck;
/
