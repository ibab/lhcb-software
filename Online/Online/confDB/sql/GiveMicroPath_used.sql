create or replace function GiveMicroPath(cpntname varchar2) return varchar2 AUTHID CURRENT_USER is
tabname varchar2(100):='lhcb_pdetails';
res_query number;

cpntid number:=0;
deviceid_first number:=0;
fdbk number:=0;
fdbk1 number:=0;
tabname_cur varchar2(100):='lhcb_pdetails';
tabname_cur1 varchar2(100):='lhcb_path_temp';
node1 number:=0;
node2 number:=0;
node number:=0;
sysid1 number;
mboardid number;
sys_first number:=0;
rescount number:=0;
rescount_old number:=0;
i number:=1;
j number:=0;
k number:=0;
case_res number:=-5;
devname varchar2(200);
motherboardname varchar2(100);
source_cursor number:=dbms_sql.open_cursor;
begin
--check that the 2 devices are part of the same subsystem: if not-> not implemented (we don't give path between a sub farm node and a L0 board in the VELO doesn't make sense!)
execute immediate 'select t.cpntid,t.motherboardid,f.devicename from lhcb_lg_devices f,lhcb_lg_cpnts t where t.cpntname=:dfrom and f.deviceid=:t.motherboardid' into cpntid,mboardid,motherboardname using cpntfrom;





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
	execute immediate 'select count(*) from lhcb_pdet_temp where (node2-:dev1)*(node3-:dev1)*(node4-:dev1)*(node5-:dev1)*(node6-:dev1)*(node7-:dev1)*(node8-:dev1)*(node9-:dev1)*(node10-:dev1)*(node11-:dev1)=0 and rownum=1' into i  using deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first;
	if i=1 then
		update lhcb_pdet_temp set pathused=0;
		execute immediate 'update lhcb_pdet_temp set pathused=1 where pathid in (select pathid from lhcb_pdet_temp where (node2-:dev1)*(node3-:dev1)*(node4-:dev1)*(node5-:dev1)*(node6-:dev1)*(node7-:dev1)*(node8-:dev1)*(node9-:dev1)*(node10-:dev1)*(node11-:dev1)=0)' using deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first,deviceid_first;
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
			execute immediate 'select count(*) from lhcb_pdet_temp where (node2-:dev1)*(node3-:dev1)*(node4-:dev1)*(node5-:dev1)*(node6-:dev1)*(node7-:dev1)*(node8-:dev1)*(node9-:dev1)*(node10-:dev1)*(node11-:dev1)=0 and rownum=1' into i  using deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid;
			if i=1 then
			update lhcb_pdet_temp set pathused=0;
			execute immediate 'update lhcb_pdet_temp set pathused=1 where pathid in (select pathid from lhcb_pdet_temp where (node2-:dev1)*(node3-:dev1)*(node4-:dev1)*(node5-:dev1)*(node6-:dev1)*(node7-:dev1)*(node8-:dev1)*(node9-:dev1)*(node10-:dev1)*(node11-:dev1)=0)' using deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid,deviceid;

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
insert into lhcb_path_temp select p.pathid,1,p.node1,f.port_nbr||':'||f.port_type,f.port_way, p.node2,g.port_nbr||':'||g.port_type ,g.port_way   from (select pathid,node1,node2 from lhcb_pdet_temp  where pathused=1) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node1=f.deviceid and p.node2=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node2!=0 ;
insert into lhcb_path_temp select p.pathid,2,p.node2,f.port_nbr||':'||f.port_type,f.port_way, p.node3,g.port_nbr||':'||g.port_type ,g.port_way   from (select pathid,node1,node2,node3 from lhcb_pdet_temp where pathused=1) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node2=f.deviceid and p.node3=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node3!=0  ;

insert into lhcb_path_temp select p.pathid,3,p.node3,f.port_nbr||':'||f.port_type,f.port_way, p.node4,g.port_nbr||':'||g.port_type ,g.port_way   from (select pathid,node1,node3,node4 from lhcb_pdet_temp  where pathused=1) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node3=f.deviceid and p.node4=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node4!=0;

insert into lhcb_path_temp select p.pathid,4,p.node4,f.port_nbr||':'||f.port_type,f.port_way, p.node5,g.port_nbr||':'||g.port_type ,g.port_way   from (select pathid,node1,node4,node5 from lhcb_pdet_temp  where  pathused=1) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node4=f.deviceid and p.node5=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node5!=0;

insert into lhcb_path_temp select p.pathid,5,p.node5,f.port_nbr||':'||f.port_type,f.port_way, p.node6,g.port_nbr||':'||g.port_type ,g.port_way   from (select pathid,node1,node5,node6 from lhcb_pdet_temp  where pathused=1 ) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node5=f.deviceid and p.node6=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node6!=0;

insert into lhcb_path_temp select p.pathid,6,p.node6,f.port_nbr||':'||f.port_type,f.port_way, p.node7,g.port_nbr||':'||g.port_type ,g.port_way   from (select pathid,node1,node6,node7 from lhcb_pdet_temp  where  pathused=1) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node6=f.deviceid and p.node7=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node7!=0;

insert into lhcb_path_temp select p.pathid,7,p.node7,f.port_nbr||':'||f.port_type,f.port_way, p.node8,g.port_nbr||':'||g.port_type ,g.port_way   from (select pathid,node1,node7,node8 from lhcb_pdet_temp  where pathused=1) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node7=f.deviceid and p.node8=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node8!=0;

insert into lhcb_path_temp select p.pathid,8,p.node8,f.port_nbr||':'||f.port_type,f.port_way, p.node9,g.port_nbr||':'||g.port_type ,g.port_way   from (select pathid,node1,node8, node9 from lhcb_pdet_temp  where pathused=1) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node8=f.deviceid and p.node9=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node9!=0;

insert into lhcb_path_temp select p.pathid,9,p.node9,f.port_nbr||':'||f.port_type,f.port_way, p.node10,g.port_nbr||':'||g.port_type ,g.port_way   from (select pathid,node1,node9,node10 from lhcb_pdet_temp  where  pathused=1) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node9=f.deviceid and p.node10=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0;

insert into lhcb_path_temp select p.pathid,10,p.node10,f.port_nbr||':'||f.port_type,f.port_way, p.node11,g.port_nbr||':'||g.port_type ,g.port_way   from (select pathid,node1,node10,node11 from lhcb_pdet_temp  where pathused=1) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node10=f.deviceid and p.node11=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0;


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
end GiveMicroPath;
/


