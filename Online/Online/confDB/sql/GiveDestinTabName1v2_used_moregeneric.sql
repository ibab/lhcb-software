
-------------------------------------------------------case for devicetype-------------------------------------------------
create or replace function GiveDestinTabName1v2(devfrom varchar2, devto_type_bis varchar2, devto_type_given number) return varchar2 AUTHID CURRENT_USER is
tabname varchar2(100):='lhcb_pdetails';
TYPE devid_list IS TABLE OF VARCHAR2(30);
rec_count number;
portList devid_list;-- max 2 elements

deviceid number:=0;
deviceid_first number:=0;
fdbk number:=0;
fdbk1 number:=0;
stmt_end varchar2(1000);
tabname_cur1 varchar2(100):='lhcb_path_temp';
node1 number:=0;
node2 number:=0;
node number:=0;
node_first number:=0;
rescount number:=0;
rescount_old number:=0;
tabname1 varchar2(100);
tabname2 varchar2(100);
tabname3 varchar2(100);
i number:=1;
j number:=0;
k number:=0;
case_res number:=-1;
devname varchar2(200);
time_before number;
devto_type varchar2(500);
res_query number;
sysname number;
dtypeid number:=0;
pathid number:=0;
lkpos number:=0;
begin
if(devto_type_given=1) then
execute immediate 'select devicetypeid from lhcb_lg_device_types where devicetype=:dtype' into dtypeid using  devto_type_bis;

devto_type:=devto_type_bis;
else
devto_type:=devto_type_bis||'%';
end if;

time_before := DBMS_UTILITY.GET_TIME;
--check existence of the device and get deviceid, if not found be trapped in the exceptions...
execute immediate 'select deviceid,system_name,node from lhcb_lg_devices where devicename=:dfrom' into deviceid,sysname,node using devfrom;
res_query:=routingtable_pck.createNodeLink_tab(sysname,1);

if(res_query=0) then
	if node=0 then
		res_query:=routingtable_pck.createPathTab(deviceid,1,10);
	else
		res_query:=routingtable_pck.createPathTab_Host(deviceid,1,10);
	end if;
else
tabname:='ERROR_IN_CREATING_PATHDETAILS';
return tabname;
end if;

if(res_query=0) then
	if(devto_type_given=1) then
		execute immediate 'select count(*) from lhcb_pdet_temp p,lhcb_lg_devices t where (p.node2-t.deviceid)*(p.node3-t.deviceid)*(p.node4-t.deviceid)*(p.node5-t.deviceid)*(p.node6-t.deviceid)*(p.node7-t.deviceid)*(node8-t.deviceid)*(node9-t.deviceid)*(node10-t.deviceid)*(node11-t.deviceid)=0 and rownum=1 and t.devicetypeID=:dtypeID and pathused=1' into i  using dtypeid;
	else
		execute immediate 'select count(*) from lhcb_pdet_temp p ,lhcb_lg_devices t where (p.node2-deviceid)*(p.node3-t.deviceid)*(p.node4-t.deviceid)*(p.node5-t.deviceid)*(p.node6-t.deviceid)*(p.node7-t.deviceid)*(p.node8-t.deviceid)*(p.node9-t.deviceid)*(p.node10-t.deviceid)*(p.node11-t.deviceid)=0 and rownum=1 and t.devicename like :dtypeID and pathused=1' into i  using devto_type;
	end if;
	if i=1 then
		
		if(devto_type_given=1) then
			execute immediate 'update lhcb_pdet_temp set  pathused=2 where pathid in (select p.pathid from lhcb_pdet_temp p,lhcb_lg_devices t where (p.node2-t.deviceid)*(p.node3-t.deviceid)*(p.node4-t.deviceid)*(p.node5-t.deviceid)*(p.node6-t.deviceid)*(p.node7-t.deviceid)*(p.node8-t.deviceid)*(p.node9-t.deviceid)*(p.node10-t.deviceid)*(p.node11-t.deviceid)=0 and t.devicetypeID=:dtypeID) and pathused=1' using dtypeid;
		else
			execute immediate 'update lhcb_pdet_temp set  pathused=2 where pathid in (select p.pathid from lhcb_pdet_temp p ,lhcb_lg_devices t where (p.node2-t.deviceid)*(p.node3-t.deviceid)*(p.node4-t.deviceid)*(p.node5-t.deviceid)*(p.node6-t.deviceid)*(p.node7-t.deviceid)*(p.node8-t.deviceid)*(p.node9-t.deviceid)*(p.node10-t.deviceid)*(p.node11-t.deviceid)=0 and t.devicename like :dtypeID) and pathused=1' using devto_type;
		end if;
	else
		commit;
		tabname:='ERROR_NO_CONNETCIVITY';
		return tabname;
	end if;
else
tabname:='ERROR_IN_CREATING_PATHDETAILS';
return tabname;
end if;	
stmt_end:='';
select distinct f.port_nbr||':'||f.port_way||':'||nvl(f.port_type,'none') bulk collect into portList   from (select pathid,node1,node2 from lhcb_pdet_temp  where pathused=2) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where h.lkused=1 and p.node1=f.deviceid and p.node2=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node2!=0 ;
DBMS_OUTPUT.PUT_LINE ('before the loop');
for i in portList.first .. portList.last loop

devname:=portList(i);
DBMS_OUTPUT.PUT_LINE ('in the loop '||devname);
stmt_end:=stmt_end||'|'||devname;

end loop;
DBMS_OUTPUT.PUT_LINE ('after the loop');
stmt_end:=stmt_end||'?';
return stmt_end;	
exception
when NO_DATA_FOUND then
tabname:='ERROR_NO_DATA_FOUND';
return tabname;
when OTHERS then
tabname:='ERROR_'||sqlerrm;
return tabname;
end GiveDestinTabName1v2;

