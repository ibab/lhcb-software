create or replace function GiveDestTabName_DTgiven_view(sysname varchar2,devfrom varchar2, devto_type_bis varchar2, devto_type_given number) return varchar2 AUTHID CURRENT_USER is
tabname varchar2(100):=sysname||'pathdetails';
TYPE element_list IS TABLE OF VARCHAR2(500);
TYPE devid_list IS TABLE OF NUMBER;
rec_count number;
EltList1 element_list:=element_list('test','test'); --max 2 elements
DevList devid_list:=devid_list(1000,215);-- max 2 elements
NodeList devid_list:=devid_list(1000,215);-- max 2 elements
deviceid number:=0;
deviceid_first number:=0;
fdbk number:=0;
fdbk1 number:=0;
stmt_end varchar2(1000);
tabname_cur1 varchar2(100):=sysname||'path_temp_view';
tabname_cur varchar2(100):=sysname||'path_temp_view';
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
begin
if(devto_type_given=1) then
devto_type:=upper(devto_type_bis);
stmt_end:=', '||sysname||'device_type_view f where t.devicetypeID=f.devicetypeID and f.devicetype=';
else
devto_type:=upper(devto_type_bis||'%');
stmt_end:=' where t.devicename like ';
end if;
time_before := DBMS_UTILITY.GET_TIME;
--check existence of the device and get deviceid, if not found be trapped in the exceptions...
execute immediate 'select deviceid, node from '||sysname||'device_view where upper(devicename)=upper(:dfrom)' into deviceid,node1 using devfrom;

--check the existence of the destination/or routing tables of the devto
execute immediate 'select count(*) from '||sysname||'destinationtable_view where nodeid_start0=:devfrom and rownum=1' into fdbk using deviceid;
if(fdbk=0) then 
	execute immediate 'select count(*) from '||sysname||'routingtable_view where nodeid_start0=:devfrom and rownum=1' into fdbk1 using deviceid;
	if(fdbk1=1) then
		tabname_cur:=sysname||'routingtable_view';
		case_res:=1;
	else --create the destinat table for that device..
		rescount:=routingtable_pck_view.createTD_all(sysname,deviceid);
		if(rescount=0)then
			case_res:=1;
			tabname_cur:=sysname||'destinationtable_view';
		else
			case_res:=-2;
		end if;
	end if;
else
	tabname_cur:=sysname||'destinationtable_view';
	case_res:=1;
end if;
--check existence of the devto_type as possible destinations...
-- 2 cases either devtype given or devicename_like
execute immediate 'select count(*) from '||tabname_cur||' where nodeid_start0=:devfrom and nodeid_end1 in (select t.deviceid from '||sysname||'device_view t '||stmt_end||':devto) and rownum=1' into rescount using deviceid, devto_type;
if(rescount=0) then 
	case_res:=-3; --not a possible destination...
else
	case_res:=2; --means we find the connect
end if;


	
DBMS_OUTPUT.PUT_LINE ('time execution to create the routing table '|| (DBMS_UTILITY.GET_TIME-time_before)/100);
dbms_output.put_line('case_res '||case_res||' devto_type '||devto_type||' device '||deviceid);

if(case_res=2) then
	tabname2:=tabname_cur;
	dbms_output.put_line('tabname_cur1 '||tabname_cur1||' tabname2 '||tabname2||' stmt_end '||stmt_end);

	--execute immediate 'insert into '||tabname_cur1||' select p.pathid,1, t.devicename, e.port_nbrfrom,g.devicename,e.port_nbrto from (select pathid,node1,node2 from '||tabname||' where node1=:1 and node2!=0) p, '||sysname||'device g, '||sysname||'device t,'||sysname||'connectivity e  where t.deviceid=e.node_from and g.deviceid=e.node_to and ((p.node1=e.node_from and p.node2=e.node_to) or (p.node2=e.node_from and p.node1=e.node_to))  and p.pathid in (select pathid from '||tabname2||' where nodeid_start0=:2 and nodeid_end1 in (select deviceid from '||sysname||'device where '||stmt_end||':3))' using deviceid,deviceid,devto_type;
	--execute immediate 'insert into '||tabname_cur1||' select p.pathid,2, t.devicename, e.port_nbrfrom,g.devicename,e.port_nbrto from (select pathid,node2, node3 from '||tabname||' where node1=:1 and node3!=0) p, '||sysname||'device g, '||sysname||'device t,'||sysname||'connectivity e  where t.deviceid=e.node_from and g.deviceid=e.node_to and ((p.node2=e.node_from and p.node3=e.node_to) or (p.node3=e.node_from and p.node2=e.node_to))  and p.pathid in (select pathid from '||tabname2||' where nodeid_start0=:2 and nodeid_end1 in (select deviceid from '||sysname||'device where '||stmt_end||':3))' using deviceid,deviceid,devto_type;
	--execute immediate 'insert into '||tabname_cur1||' select p.pathid,3, t.devicename, e.port_nbrfrom,g.devicename,e.port_nbrto from (select pathid,node3, node4 from '||tabname||' where node1=:1 and node4!=0) p, '||sysname||'device g, '||sysname||'device t,'||sysname||'connectivity e  where t.deviceid=e.node_from and g.deviceid=e.node_to and ((p.node3=e.node_from and p.node4=e.node_to) or (p.node4=e.node_from and p.node3=e.node_to))  and p.pathid in (select pathid from '||tabname2||' where nodeid_start0=:2 and nodeid_end1 in (select deviceid from '||sysname||'device where '||stmt_end||':3) )' using deviceid,deviceid,devto_type;
	--execute immediate 'insert into '||tabname_cur1||' select p.pathid,4, t.devicename, e.port_nbrfrom,g.devicename,e.port_nbrto from (select pathid, node4, node5 from '||tabname||' where node1=:1 and node5!=0) p, '||sysname||'device g, '||sysname||'device t,'||sysname||'connectivity e  where t.deviceid=e.node_from and g.deviceid=e.node_to and ((p.node4=e.node_from and p.node5=e.node_to) or (p.node5=e.node_from and p.node4=e.node_to))  and p.pathid in (select pathid from '||tabname2||' where nodeid_start0=:2 and nodeid_end1 in (select deviceid from '||sysname||'device where '||stmt_end||':3) )' using deviceid,deviceid,devto_type;
	--execute immediate 'insert into '||tabname_cur1||' select p.pathid,5, t.devicename, e.port_nbrfrom,g.devicename,e.port_nbrto from (select pathid, node5, node6 from '||tabname||' where node1=:1 and node6!=0) p, '||sysname||'device g, '||sysname||'device t,'||sysname||'connectivity e  where t.deviceid=e.node_from and g.deviceid=e.node_to and ((p.node5=e.node_from and p.node6=e.node_to) or (p.node6=e.node_from and p.node5=e.node_to))  and p.pathid in (select pathid from '||tabname2||' where nodeid_start0=:2 and nodeid_end1 in (select deviceid from '||sysname||'device where '||stmt_end||':3) )' using deviceid,deviceid,devto_type;
	--execute immediate 'insert into '||tabname_cur1||' select p.pathid,6, t.devicename, e.port_nbrfrom,g.devicename,e.port_nbrto from (select pathid, node6, node7 from '||tabname||' where node1=:1 and node7!=0) p, '||sysname||'device g, '||sysname||'device t,'||sysname||'connectivity e  where t.deviceid=e.node_from and g.deviceid=e.node_to and ((p.node6=e.node_from and p.node7=e.node_to) or (p.node7=e.node_from and p.node6=e.node_to))  and p.pathid in (select pathid from '||tabname2||' where nodeid_start0=:2 and nodeid_end1 in (select deviceid from '||sysname||'device where '||stmt_end||':3) )' using deviceid,deviceid,devto_type;
	--execute immediate 'insert into '||tabname_cur1||' select p.pathid,7, t.devicename, e.port_nbrfrom,g.devicename,e.port_nbrto from (select pathid, node7,node8 from '||tabname||' where node1=:1 and node8!=0) p, '||sysname||'device g, '||sysname||'device t,'||sysname||'connectivity e  where t.deviceid=e.node_from and g.deviceid=e.node_to and ((p.node7=e.node_from and p.node8=e.node_to) or (p.node8=e.node_from and p.node7=e.node_to))  and p.pathid in (select pathid from '||tabname2||' where nodeid_start0=:2 and nodeid_end1 in (select deviceid from '||sysname||'device where '||stmt_end||':3) )' using deviceid,deviceid,devto_type;
	--execute immediate 'insert into '||tabname_cur1||' select p.pathid,8, t.devicename, e.port_nbrfrom,g.devicename,e.port_nbrto from (select pathid, node8, node9 from '||tabname||' where node1=:1 and node9!=0) p, '||sysname||'device g, '||sysname||'device t,'||sysname||'connectivity e  where t.deviceid=e.node_from and g.deviceid=e.node_to and ((p.node8=e.node_from and p.node9=e.node_to) or (p.node9=e.node_from and p.node8=e.node_to))  and p.pathid in (select pathid from '||tabname2||' where nodeid_start0=:2 and nodeid_end1 in (select deviceid from '||sysname||'device where '||stmt_end||':3))' using deviceid,deviceid,devto_type;
	--execute immediate 'insert into '||tabname_cur1||' select p.pathid,9, t.devicename, e.port_nbrfrom,g.devicename,e.port_nbrto from (select pathid, node9, node10 from '||tabname||' where node1=:1 and node10!=0) p, '||sysname||'device g, '||sysname||'device t,'||sysname||'connectivity e  where t.deviceid=e.node_from and g.deviceid=e.node_to and ((p.node9=e.node_from and p.node10=e.node_to ) or (p.node10=e.node_from and p.node9=e.node_to )) and p.pathid in (select pathid from '||tabname2||' where nodeid_start0=:2 and nodeid_end1 in (select deviceid from '||sysname||'device where '||stmt_end||':3) )' using deviceid,deviceid,devto_type;
	--execute immediate 'insert into '||tabname_cur1||' select p.pathid,10,t.devicename, e.port_nbrfrom,g.devicename,e.port_nbrto from (select pathid, node10, node11 from '||tabname||' where node1=:1 and node11!=0) p, '||sysname||'device g, '||sysname||'device t,'||sysname||'connectivity e  where t.deviceid=e.node_from and g.deviceid=e.node_to and ((p.node10=e.node_from and p.node11=e.node_to) or (p.node11=e.node_from and p.node10=e.node_to))  and p.pathid in (select pathid from '||tabname2||' where  nodeid_start0=:2 and nodeid_end1 in (select deviceid from '||sysname||'device where '||stmt_end||':3) )' using deviceid,deviceid,devto_type;
	for i in 2..11 loop
		j:=i-1;
		execute immediate 'insert into '||tabname_cur1||' select p.pathid,'||j||',  case when l.linkid>0 then e.node_from else e.node_to end, case when l.linkid>0 then e.port_nbrfrom else e.port_nbrto end, case when l.linkid>0 then e.node_to else e.node_from end,case when l.linkid<0 then e.port_nbrfrom else e.port_nbrto end from  '||tabname||' p, '||sysname||'connectivity_view e  ,'||sysname||'logical_links_view l where  l.node_from=p.node'||j||' and l.node_to=p.node'||i||' and (abs((p.node'||j||'-e.node_from)*(p.node'||i||'-e.node_from))+ abs((p.node'||i||'-e.node_to)*(p.node'||j||'-e.node_to)))=0  and p.node'||i||'>0 and p.node'||j||'>0 and p.node1=:1 and p.node11 in (select deviceid from '||sysname||'device_view where '||stmt_end||':2)' using deviceid,devto_type;
	end loop;
	
	
	
	tabname_cur:=sysname||'path_temp_view';
end if;
return tabname_cur1;	
exception
when NO_DATA_FOUND then
tabname:='ERROR_NO_DATA_FOUND';
return tabname;
when OTHERS then
tabname:='ERROR_'||sqlerrm;
return tabname;
end GiveDestTabName_DTgiven_view;
/