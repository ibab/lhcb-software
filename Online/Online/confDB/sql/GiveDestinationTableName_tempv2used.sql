
create or replace function GiveFirstAndLinkDetails(devfrom varchar2, devto varchar2) return varchar2 AUTHID CURRENT_USER is
tabname varchar2(100):='lhcb_pdetails';
res_query number;

deviceid number:=0;
deviceid_first number:=0;
fdbk number:=0;
fdbk1 number:=0;
tabname_cur varchar2(100):='lhcb_pdetails';
tabname_cur1 varchar2(100):='lhcb_path_temp';
node1 number:=0;
node2 number:=0;
node number:=0;
sysid1 number;
sysid2 number;
sys_first number:=0;
rescount number:=0;
rescount_old number:=0;
i number:=1;
j number:=0;
k number:=0;
case_res number:=-5;
devname varchar2(200);
deviceid_destin number:=0;
source_cursor number:=dbms_sql.open_cursor;
begin
--check that the 2 devices are part of the same subsystem: if not-> not implemented (we don't give path between a sub farm node and a L0 board in the VELO doesn't make sense!)
execute immediate 'select t.deviceid,t.system_name,t.node,f.deviceid,f.system_name,f.node from lhcb_lg_devices t,lhcb_lg_devices f,LHCB_SUBSYSTEMLIST g where t.devicename=:dfrom and f.devicename=:dto and mod(t.system_name,g.systemid)=0 and mod(f.system_name,g.systemid)=0 and rownum=1' into deviceid,sysid1,node1,deviceid_first,sysid2,node2 using devfrom, devto;





res_query:=routingtable_pck.createNodeLink_tab(sysid1,1);

if(res_query=0) then
	if node1=0 then
	res_query:=routingtable_pck.createPathTab(deviceid,1,10);
	else
	res_query:=routingtable_pck.createPathTab_host(deviceid,1,10);
	end if;
else
tabname:='ERROR_IN_CREATING_PATHDETAILS';
return tabname;
end if;

if(res_query=0) then
	execute immediate 'select count(*) from lhcb_pdet_temp where pathused=1 and  (node2-:dev1)*(node3-:dev1)*(node4-:dev1)*(node5-:dev1)*(node6-:dev1)*(node7-:dev1)*(node8-:dev1)*(node9-:dev1)*(node10-:dev1)*(node11-:dev1)=0 and rownum=1' into i  using deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first;
	if i=1 then
		deviceid_destin:=deviceid_first;
		execute immediate 'update lhcb_pdet_temp set pathused=2 where pathid in (select pathid from lhcb_pdet_temp where pathused=1 and (node2-:dev1)*(node3-:dev1)*(node4-:dev1)*(node5-:dev1)*(node6-:dev1)*(node7-:dev1)*(node8-:dev1)*(node9-:dev1)*(node10-:dev1)*(node11-:dev1)=0)' using deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first;
	else
		commit;
		res_query:=routingtable_pck.createNodeLink_tab(sysid2,1);
		if res_query=0 then 
			if node2=0 then
			res_query:=routingtable_pck.createPathTab(deviceid_first,1,10);
			else
			res_query:=routingtable_pck.createPathTab_host(deviceid_first,1,10);
			end if;
		else
		tabname:='ERROR_IN_CREATING_LOGICALCONN';
		return tabname;
		end if;
		if res_query=0 then
			execute immediate 'select count(*) from lhcb_pdet_temp where pathused=1 and  (node2-:dev1)*(node3-:dev1)*(node4-:dev1)*(node5-:dev1)*(node6-:dev1)*(node7-:dev1)*(node8-:dev1)*(node9-:dev1)*(node10-:dev1)*(node11-:dev1)=0 and rownum=1' into i  using deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid;
			if i=1 then
			deviceid_destin:=deviceid;
			execute immediate 'update lhcb_pdet_temp set pathused=2 where pathid in (select pathid from lhcb_pdet_temp where pathused=1 and  (node2-:dev1)*(node3-:dev1)*(node4-:dev1)*(node5-:dev1)*(node6-:dev1)*(node7-:dev1)*(node8-:dev1)*(node9-:dev1)*(node10-:dev1)*(node11-:dev1)=0)' using deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid;

			else
			commit;
			end if;
		else
			tabname:='ERROR_IN_CREATING_PATHDETAILS';
			return tabname;
		end if;
	end if;
	if i=0 then 
		tabname:='ERROR_NO_CONNECTIVITY';
		return tabname;
	end if;
	
else
	tabname:='ERROR_IN_CREATING_PATHDETAILS';
	return tabname;
end if;
execute immediate 'insert into lhcb_path_temp select p.pathid,1,p.node1,f.port_nbr||''|''||nvl(f.port_type,''none''),f.port_way, p.node2,g.port_nbr||''|''||nvl(g.port_type,''none'') ,g.port_way   from (select pathid,node1,node2 from lhcb_pdet_temp  where pathused=2 and node2=:devid) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node1=f.deviceid and p.node2=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node2!=0 and h.lkused=1' using deviceid_destin;
execute immediate 'insert into lhcb_path_temp select p.pathid,1,p.node1,l.port_nbr||''|''||nvl(l.port_type,''none''),l.port_way, p.node3,g.port_nbr||''|''||nvl(g.port_type,''none'') ,g.port_way   from (select pathid,node1,node2,node3,node4 from lhcb_pdet_temp where pathused=2 and node3=:devid) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h, lhcb_port_properties l,lhcb_port_properties v,lhcb_macroscopic_connectivity b where p.node1=l.deviceid and p.node2=v.deviceid and ((v.portid=b.portidto and l.portid=b.portidfrom) or (v.portid=b.portidfrom and l.portid=b.portidto)) and p.node2=f.deviceid and p.node3=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node3!=0 and h.lkused=1 and b.lkused=1 ' using deviceid_destin;

execute immediate 'insert into lhcb_path_temp select p.pathid,1,p.node1,l.port_nbr||''|''||nvl(l.port_type,''none''),l.port_way, p.node4,g.port_nbr||''|''||nvl(g.port_type,''none'') ,g.port_way   from (select pathid,node1,node2,node3,node4 from lhcb_pdet_temp  where pathused=2 and node4=:devid) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h,lhcb_port_properties l,lhcb_port_properties v,lhcb_macroscopic_connectivity b where p.node1=l.deviceid and p.node2=v.deviceid and ((v.portid=b.portidto and l.portid=b.portidfrom) or (v.portid=b.portidfrom and l.portid=b.portidto)) and p.node3=f.deviceid and p.node4=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node4!=0 and h.lkused=1 and b.lkused=1 ' using deviceid_destin;

execute immediate 'insert into lhcb_path_temp select p.pathid,1,p.node1,l.port_nbr||''|''||nvl(l.port_type,''none''),l.port_way, p.node5,g.port_nbr||''|''||nvl(g.port_type,''none'') ,g.port_way   from (select pathid,node1,node2,node4,node5 from lhcb_pdet_temp  where  pathused=2 and node5=:devid) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h,lhcb_port_properties l,lhcb_port_properties v,lhcb_macroscopic_connectivity b where p.node1=l.deviceid and p.node2=v.deviceid and ((v.portid=b.portidto and l.portid=b.portidfrom) or (v.portid=b.portidfrom and l.portid=b.portidto)) and p.node4=f.deviceid and p.node5=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node5!=0 and h.lkused=1 and b.lkused=1 ' using deviceid_destin;

execute immediate 'insert into lhcb_path_temp select p.pathid,1,p.node1,l.port_nbr||''|''||nvl(l.port_type,''none''),l.port_way, p.node6,g.port_nbr||''|''||nvl(g.port_type,''none'') ,g.port_way   from (select pathid,node1,node2,node5,node6 from lhcb_pdet_temp  where pathused=2 and node6=:devid) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h,lhcb_port_properties l,lhcb_port_properties v,lhcb_macroscopic_connectivity b where p.node1=l.deviceid and p.node2=v.deviceid and ((v.portid=b.portidto and l.portid=b.portidfrom) or (v.portid=b.portidfrom and l.portid=b.portidto)) and p.node5=f.deviceid and p.node6=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node6!=0 and h.lkused=1 and b.lkused=1 ' using deviceid_destin;

execute immediate 'insert into lhcb_path_temp select p.pathid,1,p.node1,l.port_nbr||''|''||nvl(l.port_type,''none''),l.port_way, p.node7,g.port_nbr||''|''||nvl(g.port_type,''none'') ,g.port_way   from (select pathid,node1,node2,node6,node7 from lhcb_pdet_temp  where  pathused=2 and node7=:devid) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h,lhcb_port_properties l,lhcb_port_properties v,lhcb_macroscopic_connectivity b where p.node1=l.deviceid and p.node2=v.deviceid and ((v.portid=b.portidto and l.portid=b.portidfrom) or (v.portid=b.portidfrom and l.portid=b.portidto)) and p.node6=f.deviceid and p.node7=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node7!=0 and h.lkused=1 and b.lkused=1 ' using deviceid_destin;

execute immediate 'insert into lhcb_path_temp select p.pathid,1,p.node1,l.port_nbr||''|''||nvl(l.port_type,''none''),l.port_way, p.node8,g.port_nbr||''|''||nvl(g.port_type,''none'') ,g.port_way   from (select pathid,node1,node2,node7,node8 from lhcb_pdet_temp  where pathused=2 and node8=:devid) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h,lhcb_port_properties l,lhcb_port_properties v,lhcb_macroscopic_connectivity b where p.node1=l.deviceid and p.node2=v.deviceid and ((v.portid=b.portidto and l.portid=b.portidfrom) or (v.portid=b.portidfrom and l.portid=b.portidto)) and p.node7=f.deviceid and p.node8=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node8!=0 and h.lkused=1 and b.lkused=1 ' using deviceid_destin;

execute immediate 'insert into lhcb_path_temp select p.pathid,1,p.node1,l.port_nbr||''|''||nvl(l.port_type,''none''),l.port_way, p.node9,g.port_nbr||''|''||nvl(g.port_type,''none'') ,g.port_way   from (select pathid,node1,node2,node8, node9 from lhcb_pdet_temp  where pathused=2 and node9=:devid) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h,lhcb_port_properties l,lhcb_port_properties v,lhcb_macroscopic_connectivity b where p.node1=l.deviceid and p.node2=v.deviceid and ((v.portid=b.portidto and l.portid=b.portidfrom) or (v.portid=b.portidfrom and l.portid=b.portidto)) and p.node8=f.deviceid and p.node9=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node9!=0 and h.lkused=1 and b.lkused=1 ' using deviceid_destin;

execute immediate 'insert into lhcb_path_temp select p.pathid,1,p.node1,l.port_nbr||''|''||nvl(l.port_type,''none''),l.port_way, p.node10,g.port_nbr||''|''||nvl(g.port_type,''none'') ,g.port_way   from (select pathid,node1,node2,node9,node10 from lhcb_pdet_temp  where  pathused=2 and node10=:devid) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h, lhcb_port_properties l,lhcb_port_properties v,lhcb_macroscopic_connectivity b where p.node1=l.deviceid and p.node2=v.deviceid and ((v.portid=b.portidto and l.portid=b.portidfrom) or (v.portid=b.portidfrom and l.portid=b.portidto)) and p.node9=f.deviceid and p.node10=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0 and h.lkused=1 and b.lkused=1 ' using deviceid_destin;

execute immediate 'insert into lhcb_path_temp select p.pathid,1,p.node1,l.port_nbr||''|''||nvl(l.port_type,''none''),l.port_way, p.node11,g.port_nbr||''|''||nvl(g.port_type,''none'') ,g.port_way   from (select pathid,node1,node2,node10,node11 from lhcb_pdet_temp  where pathused=2 and node10!=node11 and node11=:devid ) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h, lhcb_port_properties l,lhcb_port_properties v,lhcb_macroscopic_connectivity b where p.node1=l.deviceid and p.node2=v.deviceid and ((v.portid=b.portidto and l.portid=b.portidfrom) or (v.portid=b.portidfrom and l.portid=b.portidto)) and p.node10=f.deviceid and p.node11=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0 and h.lkused=1 and b.lkused=1 ' using deviceid_destin;


-- need to delete some links not necessarily all the paths are interesting
tabname_cur:='lhcb_path_temp';


return tabname_cur;	
exception
when NO_DATA_FOUND then
if sys_first=1 then
tabname:='ERROR_NO_PATH_FOUND';
else
tabname:='ERROR_NOT_IN_SAME_SUBSYSTEM';
end if;
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
end GiveFirstAndLinkDetails;
/


