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

function CheckPathTabCompleteAndUpdate(systemname in number)  return varchar2 is
tabname varchar2(100);
node number:=0;
resquery1 varchar2(100);
resquery number:=-1;
resquery3 varchar2(100):='NONE';
resquery2 varchar2(100):='NONE';
time_before number;
result_fct varchar2(100):='NO_NEED';
sysID number:=-1;
daq_sysid number;
tfc_sysid number;
begin
select systemid into daq_sysid from lhcb_subsystemlist where system_name='DAQ' ;
select systemid into tfc_sysid from lhcb_subsystemlist where system_name='TFC' ;

if(mod(systemname,daq_sysid)=0 or mod(systemname,tfc_sysid)=0) then

--first check if the path table is updated or not
time_before := DBMS_UTILITY.GET_TIME;

pathqueries_pck.checkUpdatePathTable(systemname,daq_sysid,tfc_sysid,sysID,resquery);
DBMS_OUTPUT.PUT_LINE ('time execution to check update tab '|| (DBMS_UTILITY.GET_TIME-time_before)/100 ||' and sysID='||sysID);
DBMS_OUTPUT.PUT_LINE ('value of '||resquery);
-- then if pathtable is complete
if resquery=0 then
	
		result_fct:=pathqueries_pck.generKeyDTab(sysID);
	else
		result_fct:='ERROR_'||resquery;
	end if;
end if;
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

----------------------------------------------------------------------------------------------------
--check if the pathtable is complete or not to find all paths which cross a given device.
--need also to know if we need to search for the first link or not
-------------------------------------------------------------------------
function checkCompletePathTable(systemname number) return varchar2 is
diff_lastupdate number;
endprog number:=0;
node1 number;
node2 number;
--need_tabupdate number;
v_error_message varchar2(100);
pathtab varchar2(100):='lhcb_pdetails';
conntab varchar2(100):='lhcb_macroscopic_connectivity';
begin
execute immediate ' select count(t.linkid)  from lhcb_macroscopic_connectivity t ,lhcb_port_properties p,lhcb_pdetails l where t.link_weight=1 and t.portidto=p.portid and p.deviceid=l.node1 and (mod(l.system_name,:sysid)=0 or mod(:sysid,l.system_name)=0) and rownum=1' into diff_lastupdate using systemname,systemname;
--DBMS_OUTPUT.PUT_LINE ('case NO_TO 1'||diff_lastupdate);
if diff_lastupdate=1 then
	v_error_message:='NODE_TO';
	--DBMS_OUTPUT.PUT_LINE ('case NO_TO 1');
	execute immediate ' select count(t.linkid) from lhcb_macroscopic_connectivity t,lhcb_port_properties p,lhcb_pdetails l where t.link_weight=2 and t.portidfrom=p.portid and p.deviceid=l.node1 and (mod(l.system_name,:sysid)=0 or mod(:sysid,l.system_name)=0) ' into node1 using systemname,systemname;
	if node1=1 then
		--DBMS_OUTPUT.PUT_LINE ('case NO_TO 2');
		v_error_message:='NODE_TO?NODE_FROM';
	else
		--DBMS_OUTPUT.PUT_LINE ('case NO_TO 3');
		execute immediate ' select count(t.linkid) from lhcb_macroscopic_connectivity t,lhcb_port_properties p,lhcb_pdetails l where t.link_weight=2 and t.portidto=p.portid and p.deviceid=l.node1 and (mod(l.system_name,:sysid)=0 or mod(:sysid,l.system_name)=0) and rownum=1' into node1 using systemname,systemname;
		if node1=0 then
			v_error_message:='NODE_TO?NONE';
		else
			v_error_message:='NODE_TO?NODE_TO';
		end if;
	end if;
	--DBMS_OUTPUT.PUT_LINE ('case NO_TO 4');
else
	execute immediate ' select count(t.linkid)  from lhcb_macroscopic_connectivity t,lhcb_port_properties p,lhcb_pdetails l where t.link_weight=1 and t.portidfrom=p.portid and p.deviceid=l.node1 and (mod(l.system_name,:sysid)=0 or mod(:sysid,l.system_name)=0) and rownum=1 ' into diff_lastupdate using systemname,systemname;
	if diff_lastupdate=1 then
		--need_tabupdate=0;
		v_error_message:='NODE_FROM';
		--DBMS_OUTPUT.PUT_LINE ('case NO_from 1');
		execute immediate ' select count(t.linkid) from lhcb_macroscopic_connectivity t,lhcb_port_properties p,lhcb_pdetails l where t.link_weight=2 and t.portidfrom=p.portid and p.deviceid=node1 and (mod(l.system_name,:sysid)=0 or mod(:sysid,l.system_name)=0)  and rownum=1' into node1 using systemname,systemname;
		if node1=1 then
			v_error_message:='NODE_FROM?NODE_FROM';
		else
			--DBMS_OUTPUT.PUT_LINE ('case NO_from 3');
			execute immediate ' select count(t.linkid) from lhcb_macroscopic_connectivity t,lhcb_port_properties p,lhcb_pdetails l  where t.link_weight=2  and t.portidto=p.portid and p.deviceid=l.node1 and (mod(l.system_name,:sysid)=0 or mod(:sysid,l.system_name)=0) and rownum=1' into node1 using systemname,systemname;
			if node1=0 then
				v_error_message:='NODE_FROM?NONE';
			else
				v_error_message:='NODE_FROM?NODE_TO';
			end if;	
		end if;
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
procedure checkUpdatePathTable(systemname in  number,sysname1 in out number,need_tabupdate in out number)  is
diff_lastupdate number;
TYPE intList is table of number;
sysIDlist intList;
v_error_message varchar2(100);
minval date;
maxval date;
i number:=1;
j number;
pathtab varchar2(100):='lhcb_pdetails';
connectivitytab varchar2(100):='lhcb_macroscopic_connectivity';
begin
--need_tabupdate:=-1;
execute immediate 'select max(last_update) from lhcb_macroscopic_connectivity where mod(system_name,:sysID)=0 or mod(:sysID,system_name)=0' into maxval using systemname,systemname;

execute immediate 'select min(last_update) from lhcb_pdetails where mod(system_name,:sysID)=0 or mod(:sysID,system_name)=0' into minval using systemname,systemname;

diff_lastupdate:=ceil(minval-maxval);
--DBMS_OUTPUT.PUT_LINE ('difflastupdate :'||minval);
if diff_lastupdate>0 then -- 
	need_tabupdate:=0;
		if systemname=1 then
		sysname1:=1;
		else
		sysname1:=systemname;
		end if;
		
else
	if systemname=1 then
		execute immediate 'select distinct system_name from lhcb_macroscopic_connectivity where last_update>=:minval and (mod(system_name,:sysID)=0 or mod(:sysID,system_name)=0)' bulk collect into sysIDlist using minval;
		sysname1:=systemname;
		if sysIDlist.count()=0 then
			need_tabupdate:=1;
			sysname1:=1;
		else
			for i in sysIDlist.first..sysIDlist.last loop
				sysname1:=sysname1*sysIDlist(i);
			end loop;
			need_tabupdate:=1;
		end if;
	else
		need_tabupdate:=1;
		sysname1:=systemname;
	end if;
end if;

exception
when NO_DATA_FOUND then
need_tabupdate:=-1;
when OTHERS then
need_tabupdate:=sqlcode;
v_error_message:='ERROR_'||sqlerrm;
DBMS_OUTPUT.PUT_LINE ('error mess :'||v_error_message);
end checkUpdatepathtable;

------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------
--generate and check existence of key tables
----------------------------------------------------------------------------------------------------
function generKeyDTab(systemname in number) return varchar2  is
tabname varchar2(100);
pathtab varchar2(100):='lhcb_pdetails'; 
destintab varchar2(100):='lhcb_destinationtable'; 
routingtab varchar2(100):='lhcb_routingtable';
connectivitytab varchar2(100):='lhcb_macroscopic_connectivity';
IPtab varchar2(100):='lhcb_port_properties';
source_cursor number:=dbms_sql.open_cursor;
node number;
nodeid number;
nodeto_count number;
nodefrom_count number;
i number:=0;
deviceid number;
resquery number:=0;
exist_iptab number:=0;
limit_node number;
rec_count number;
rescreate number:=0;
v_error_message varchar2(100);
program_end number:=-1;
begin
--first check if it's a one-to-one connection or many-to-one
--it's to reduce the number of useless DTs 
--delete old rows if it exists...because if we enter this fct, means we need either to update or to populate
execute immediate 'delete from '||routingtab||' where (mod(system_name,:sysID)=0 or mod(:sysID,system_name)=0) and (nodeid_start0||'':''||pathid) in (select distinct node1||'':''||pathid from '||pathtab||' where path_necessary=1 and (mod(system_name,:sysID)=0 or mod(:sysID,system_name)=0))' using systemname,systemname,systemname,systemname;
DBMS_OUTPUT.PUT_LINE ('case NO_from 1');
execute immediate 'delete from '||destintab||' where (mod(system_name,:sysID)=0  or mod(:sysID,system_name)=0) and (nodeid_start0||'':''||pathid) in (select distinct node1||'':''||pathid from '||pathtab||' where path_necessary=1 and (mod(system_name,:sysID)=0 or mod(:sysID,system_name)=0))' using systemname,systemname,systemname,systemname;
DBMS_OUTPUT.PUT_LINE ('case NO_from 2');
--execute immediate 'delete from lhcb_pdetwithlkids where mod(system_name,:sysID)=0' using systemname;
--execute immediate 'truncate table '||systemname||'pathdetailswithports';

execute immediate 'delete from '||pathtab||' where path_necessary=1 and (mod(system_name,:sysID)=0 or mod(:sysID,system_name)=0)' using systemname,systemname;
DBMS_OUTPUT.PUT_LINE ('case NO_from 3');
execute immediate 'select count(distinct r.deviceid), count(distinct d.deviceid) from lhcb_macroscopic_connectivity t ,lhcb_port_properties r,lhcb_port_properties d where t.link_weight=1 and t.portidfrom=r.portid and t.portidto=d.portid and (mod(t.system_name,:sys)=0 or mod(:sysID,t.system_name)=0)' into nodefrom_count,nodeto_count using systemname,systemname ;
if nodeto_count< nodefrom_count then
	limit_node:=nodeto_count;
	v_error_message:='NODE_TO';
	dbms_sql.parse(source_cursor,'select distinct t.deviceid from lhcb_macroscopic_connectivity g,lhcb_port_properties t where g.link_weight=1 and g.portidto=t.portid and (mod(g.system_name,:sys)=0 or mod(:sys,g.system_name)=0)',dbms_sql.native);
else
	limit_node:=nodefrom_count;
	v_error_message:='NODE_FROM';
	dbms_sql.parse(source_cursor,'select distinct t.deviceid from lhcb_macroscopic_connectivity g, lhcb_port_properties t where g.link_weight=1 and g.portidfrom=t.portid and (mod(g.system_name,:sys)=0 or mod(:sys,g.system_name)=0)',dbms_sql.native);
end if;	
DBMS_OUTPUT.PUT_LINE ('case NO_from 5');
dbms_sql.bind_variable(source_cursor,':sys',systemname);
--dbms_sql.bind_variable(source_cursor,':dto',devto);
rec_count:=dbms_sql.execute(source_cursor);
dbms_sql.define_column(source_cursor,1,nodeid);
--dbms_sql.define_column(source_cursor,2,node1);
if mod(systemname,41)=0 and v_error_message='NODE_TO' then --sysID of the DAQ + check it's a switch--->we generate a routing table
exist_iptab:=1;
end if;
while (i<nodeto_count) LOOP
	EXIT WHEN DBMS_SQL.FETCH_ROWS (source_cursor)=0;
	DBMS_SQL.COLUMN_VALUE (source_cursor, 1, nodeid);
	if i=0 then
		rescreate:=1;
	end if;
	if exist_iptab=1 then --case we have ip_ethernet tab
		resquery:=routingtable_pck.createRT_all(nodeid,1,10,rescreate);
		rescreate:=0;
	else
		resquery:=routingtable_pck.createTD_all(nodeid,1,10,rescreate);
	end if;
--DBMS_OUTPUT.PUT_LINE ('generate key tabs '|| nodeid||' and resqueyr '||resquery);


	if resquery=0 then
		i:=i+1;
	else
		if exist_iptab=1 then
			v_error_message:='ERROR_IN_RT_OF_'||nodeid;
		else
			v_error_message:='ERROR_IN_DT'||nodeid;
		end if;
		i:=nodeto_count+10;
	end if;
END LOOP;
dbms_sql.close_cursor(source_cursor);
program_end:=1;
i:=0;
--check that all the possible host nodes have been reached (one case for the storage path, doesn't go through the Big Switch...)	
execute immediate 'select count(distinct t.deviceid), count(distinct l.deviceid) from lhcb_macroscopic_connectivity g, lhcb_port_properties t,lhcb_port_properties l where g.link_weight=2 and g.portidfrom=t.portid and g.portidto=l.portid and (mod(g.system_name,:sys)=0 or mod(:sys,g.system_name)=0) and l.deviceid not in (select node11 from lhcb_pdetails where mod(system_name,:sys)=0 or mod(:sys,system_name)=0)' into nodeid,deviceid using systemname,systemname,systemname,systemname;
DBMS_OUTPUT.PUT_LINE ('case NO_from 6');

if nodeid=0 then
	v_error_message:=v_error_message||'?NONE';
	return v_error_message;
end if;
if nodeid<deviceid then 
	dbms_sql.parse(source_cursor,'select distinct t.deviceid from lhcb_macroscopic_connectivity g, lhcb_port_properties t where g.link_weight=2  and g.portidfrom=t.portid  and (mod(t.system_name,:sys)=0 or mod(:sys,t.system_name)=0) and g.bidirectional_link_used=1',dbms_sql.native);
	v_error_message:=v_error_message||'?NODE_FROM';
else
	dbms_sql.parse(source_cursor,'select distinct l.deviceid from lhcb_macroscopic_connectivity g, lhcb_port_properties l where g.link_weight=2   and g.portidto=l.portid and (mod(t.system_name,:sys)=0 or mod(:sys,t.system_name)=0) and g.bidirectional_link_used=1',dbms_sql.native);
	v_error_message:=v_error_message||'?NODE_TO';
end if;
dbms_sql.bind_variable(source_cursor,':sys',systemname);
	--dbms_sql.bind_variable(source_cursor,':dto',devto);
rec_count:=dbms_sql.execute(source_cursor);
dbms_sql.define_column(source_cursor,1,deviceid);
DBMS_OUTPUT.PUT_LINE ('case NO_from 7');

while i<nodeto_count LOOP
	EXIT WHEN DBMS_SQL.FETCH_ROWS (source_cursor)=0;
	DBMS_SQL.COLUMN_VALUE (source_cursor, 1, deviceid);
	if exist_iptab=1 then --case we have ip_ethernet tab
		resquery:=routingtable_pck.createRT_all(deviceid,1,10,rescreate);
		rescreate:=0;
	else
		resquery:=routingtable_pck.createTD_all(deviceid,1,10,rescreate);
	end if;
--DBMS_OUTPUT.PUT_LINE ('generate key tabs '|| nodeid||' and resqueyr '||resquery);
	if resquery=0 then
		i:=i+1;
	else
		if exist_iptab=1 then
			v_error_message:='ERROR_IN_RT_OF_'||deviceid;
		else
			v_error_message:='ERROR_IN_DT'||deviceid;
		end if;
		i:=nodeto_count+10;
	end if;
END LOOP;
dbms_sql.close_cursor(source_cursor);

return v_error_message;

exception
when NO_DATA_FOUND then
if program_end=-1 then
resquery:=-1;
v_error_message:='ERROR_NO_DATA_FOUND';
else
resquery:=1;
end if;
if DBMS_SQL.IS_OPEN(source_cursor)=TRUE then
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
--------------------------------------------------------------------
function UpdatePathTables (systemname in number) return varchar2 is
tabname varchar2(100);

pathtab varchar2(100):='lhcb_pdetails'; 
destintab varchar2(100):='lhcb_destinationtable';
routingtab varchar2(100):='lhcb_routingtable';
connectivitytab varchar2(100):='lhcb_macroscopic_connectivity';
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
dbms_sql.parse(source_cursor,'select distinct NODEID_START0 from '||destintab||' t,'||pathtab||' e where (mod(e.system_name,:sys)=0 or mod(:sys,e.system_name)=0)and e.path_necessary=1 and e.node1=t.nodeid_start0 and e.pathid=t.pathid',dbms_sql.native);
dbms_sql.bind_variable(source_cursor,':sys',systemname);
rec_count:=dbms_sql.execute(source_cursor);
dbms_sql.define_column(source_cursor,1,nodeID);
LOOP
	EXIT WHEN DBMS_SQL.FETCH_ROWS (source_cursor)=0;
	DBMS_SQL.COLUMN_VALUE (source_cursor, 1, nodeID);
	if i=0 then
		rescreate:=1;
	else
		rescreate:=0;	
	end if;
	j:=routingtable_pck.createTD_all(nodeID,0,10,rescreate);
	if j!=0 then
		v_error_message:='ERROR_createTD_all_'||nodeID||'_errnb_'||j;
		dbms_sql.close_cursor(source_cursor);
		return v_error_message;
	end if;
	i:=i+1;
END LOOP;
i:=0;
dbms_sql.parse(source_cursor,'select distinct NODEID_START0 from '||routingtab||' t,'||pathtab||' e where 9mod(e.system_name,:sys)=0 and or mod(:sys,e.system_name)=0)e.path_necessary=1 and e.node1=t.nodeid_start0 and e.pathid=t.pathid',dbms_sql.native);
dbms_sql.bind_variable(source_cursor,':sys',systemname);

rec_count:=dbms_sql.execute(source_cursor);
dbms_sql.define_column(source_cursor,1,nodeID);
LOOP
	EXIT WHEN DBMS_SQL.FETCH_ROWS (source_cursor)=0;
	DBMS_SQL.COLUMN_VALUE (source_cursor, 1, nodeID);
	if i=0 then
		rescreate:=1;
	else
		rescreate:=0;	
	end if;
	j:=routingtable_pck.createRT_all(nodeID,0,10,rescreate);
	if j!=0 then
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
if DBMS_SQL.IS_OPEN(source_cursor)=TRUE then
	dbms_sql.close_cursor(source_cursor);
end if;
return v_error_message;
when OTHERS then
j:=sqlcode;
if DBMS_SQL.IS_OPEN(source_cursor)=TRUE then
	dbms_sql.close_cursor(source_cursor);
end if;
v_error_message:='ERROR_'||sqlerrm;
return v_error_message;	
end UpdatePathTables;
-------------------------------------------------
--Check and update all the dynamic tables :
-------------------------------------------------------------------------------------------------------------
function CheckAndUpdateDynamicTables (systemname in number) return varchar2 is
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


resstmt:=pathqueries_pck.CheckPathTabCompleteAndUpdate(systemname);

if instr(resstmt,'ERROR')=0 then
	j:=9;
	--v_error_message:=pathqueries_pck.UpdatePathTables(systemname);
else
	v_error_message:=resstmt;
end if;
end if;
return v_error_message;
end CheckAndUpdateDynamicTables;


end pathqueries_pck;
/
