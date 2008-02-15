create or replace function GiveDestTabName_DTgiven(devfrom varchar2, devto_type_bis varchar2, devto_type_given number) return varchar2 AUTHID CURRENT_USER is
tabname varchar2(100):='lhcb_pdetails';
TYPE devid_list IS TABLE OF NUMBER;
rec_count number;
lkposList devid_list;-- max 2 elements
pathidList devid_list;-- max 2 elements

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
tabname:='ERROR_IN_CREATING_NODETAB';
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
tabname:='ERROR_IN_CREATING_PATHDETAILS_'||res_query;
return tabname;
end if;	
	insert into lhcb_path_temp select p.pathid,1,p.node1,f.port_nbr||'|'||nvl(f.port_type,'none'),f.port_way, p.node2,g.port_nbr||'|'||nvl(g.port_type,'none') ,g.port_way   from (select pathid,node1,node2 from lhcb_pdet_temp  where pathused=2) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node1=f.deviceid and p.node2=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node2!=0 and h.lkused=1;
	insert into lhcb_path_temp select p.pathid,2,p.node2,f.port_nbr||'|'||nvl(f.port_type,'none'),f.port_way, p.node3,g.port_nbr||'|'||nvl(g.port_type,'none') ,g.port_way   from (select pathid,node1,node2,node3 from lhcb_pdet_temp where pathused=2) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node2=f.deviceid and p.node3=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node3!=0 and h.lkused=1;

	insert into lhcb_path_temp select p.pathid,3,p.node3,f.port_nbr||'|'||nvl(f.port_type,'none'),f.port_way, p.node4,g.port_nbr||'|'||nvl(g.port_type,'none') ,g.port_way   from (select pathid,node1,node3,node4 from lhcb_pdet_temp where pathused=2) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node3=f.deviceid and p.node4=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node4!=0 and h.lkused=1;

	insert into lhcb_path_temp select p.pathid,4,p.node4,f.port_nbr||'|'||nvl(f.port_type,'none'),f.port_way, p.node5,g.port_nbr||'|'||nvl(g.port_type,'none') ,g.port_way   from (select pathid,node1,node4,node5 from lhcb_pdet_temp where pathused=2) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node4=f.deviceid and p.node5=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node5!=0 and h.lkused=1;

insert into lhcb_path_temp select p.pathid,5,p.node5,f.port_nbr||'|'||nvl(f.port_type,'none'),f.port_way, p.node6,g.port_nbr||'|'||nvl(g.port_type,'none') ,g.port_way   from (select pathid,node1,node5,node6 from lhcb_pdet_temp  where pathused=2 ) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node5=f.deviceid and p.node6=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node6!=0 and h.lkused=1;

	insert into lhcb_path_temp select p.pathid,6,p.node6,f.port_nbr||'|'||nvl(f.port_type,'none'),f.port_way, p.node7,g.port_nbr||'|'||nvl(g.port_type,'none') ,g.port_way   from (select pathid,node1,node6,node7 from lhcb_pdet_temp  where pathused=2 ) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node6=f.deviceid and p.node7=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node7!=0 and h.lkused=1;

	insert into lhcb_path_temp select p.pathid,7,p.node7,f.port_nbr||'|'||nvl(f.port_type,'none'),f.port_way, p.node8,g.port_nbr||'|'||nvl(g.port_type,'none') ,g.port_way   from (select pathid,node1,node7,node8 from lhcb_pdet_temp  where pathused=2) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node7=f.deviceid and p.node8=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node8!=0 and h.lkused=1;

	insert into lhcb_path_temp select p.pathid,8,p.node8,f.port_nbr||'|'||nvl(f.port_type,'none'),f.port_way, p.node9,g.port_nbr||'|'||nvl(g.port_type,'none') ,g.port_way   from (select pathid,node1,node8, node9 from lhcb_pdet_temp  where  pathused=2) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node8=f.deviceid and p.node9=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node9!=0 and h.lkused=1;

insert into lhcb_path_temp select p.pathid,9,p.node9,f.port_nbr||'|'||nvl(f.port_type,'none'),f.port_way, p.node10,g.port_nbr||'|'||nvl(g.port_type,'none') ,g.port_way   from (select pathid,node1,node9,node10 from lhcb_pdet_temp  where pathused=2) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node9=f.deviceid and p.node10=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0 and h.lkused=1;

	insert into lhcb_path_temp select p.pathid,10,p.node10,f.port_nbr||'|'||nvl(f.port_type,'none'),f.port_way, p.node11,g.port_nbr||'|'||nvl(g.port_type,'none') ,g.port_way   from (select pathid,node1,node10,node11 from lhcb_pdet_temp  where pathused=2) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node10=f.deviceid and p.node11=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0 and h.lkused=1;

if(devto_type_given=1) then
	execute immediate 'select count(*) from lhcb_pdet_temp ,lhcb_lg_devices where  rownum=1 and node11=deviceid and devicetypeid=:dtypeid' into k using dtypeid;
else
	execute immediate 'select count(*) from lhcb_pdet_temp ,lhcb_lg_devices where  rownum=1 and node11=deviceid and devicename like :dtype' into k using devto_type;
end if;
DBMS_OUTPUT.PUT_LINE ('result of k= '|| k);
if k=0 then
	if(devto_type_given=1) then
	execute immediate 'select pathid,link_pos from lhcb_path_temp where nto in (select deviceid from lhcb_lg_devices where devicetypeid=:devid_destin) ' bulk collect into pathidList,lkposList using dtypeid;
	else
	execute immediate 'select pathid,link_pos from lhcb_path_temp where nto in (select deviceid from lhcb_lg_devices where devicename like :devid_destin) ' bulk collect into pathidList,lkposList using devto_type;
	end if;
	for i in pathidList.first..pathidList.last loop
		pathid:=pathidList(i);
		lkpos:=lkposList(i);
		--DBMS_OUTPUT.PUT_LINE ('result of pathid= '|| pathid||' and lkpos '||lkpos);
		execute immediate 'delete from lhcb_path_temp where pathid=:pid and link_pos>:lkpos' using pathid,lkpos;
	end loop;
end if;
tabname_cur1:='lhcb_path_temp';

return tabname_cur1;	
exception
when NO_DATA_FOUND then
tabname:='ERROR_NO_DATA_FOUND';
return tabname;
when OTHERS then
tabname:='ERROR_'||sqlerrm;
return tabname;
end GiveDestTabName_DTgiven;
/
