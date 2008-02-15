create or replace function GiveDestinationTableName(devfrom varchar2, devto varchar2,test number) return varchar2 AUTHID CURRENT_USER is
tabname varchar2(100):='lhcb_pdetails';
TYPE element_list IS TABLE OF VARCHAR2(500);
TYPE devid_list IS TABLE OF NUMBER;
rec_count number;
EltList1 element_list:=element_list('test','test'); --max 2 elements
DevList devid_list:=devid_list(1000,215);-- max 2 elements
NodeList devid_list:=devid_list(1000,215);-- max 2 elements
pathidList devid_list;-- max 2 elements
node1List devid_list;-- max 2 elements
pathidList1 devid_list;-- max 2 elements
node1List1 devid_list;-- max 2 elements

deviceid number:=0;
deviceid_first number:=0;
fdbk number:=0;
fdbk1 number:=0;
tabname_cur varchar2(100):='lhcb_pdetails';
tabname_cur1 varchar2(100):='lhcb_path_temp';
node1 number:=0;
node2 number:=0;
node number:=0;
sys_first number:=0;
rescount number:=0;
rescount_old number:=0;
i number:=1;
j number:=0;
k number:=0;
case_res number:=-5;
devname varchar2(200);

source_cursor number:=dbms_sql.open_cursor;
begin
--check that the 2 devices are part of the same subsystem: if not-> not implemented (we don't give path between a sub farm node and a L0 board in the VELO doesn't make sense!)
execute immediate 'select t.deviceid,t.system_name,f.deviceid,f.system_name from lhcb_logical_device t,lhcb_logical_device f,LHCB_SUBSYSTEMLIST g where t.devicename=:dfrom and f.devicename=:dto and mod(t.system_name,g.systemid)=0 and mod(f.system_name,g.systemid)=0' into DevList(1),NodeList(1),DevList(2),NodeList(2) using devfrom, devto;


sys_first:=1;

if test=1 then
execute immediate 'select pathid,node1 from lhcb_pdetails where (node1-:dev1)*(node2-:dev1)*(node3-:dev1)*(node4-:dev1)*(node5-:dev1)*(node6-:dev1)*(node7-:dev1)*(node8-:dev1)*(node9-:dev1)*(node10-:dev1)*(node11-:dev1)=0 and (node1-:dev2)*(node2-:dev2)*(node3-:dev2)*(node4-:dev2)*(node5-:dev2)*(node6-:dev2)*(node7-:dev2)*(node8-:dev2)*(node9-:dev2)*(node10-:dev2)*(node11-:dev2)=0 and path_necessary=1' bulk collect into pathidList,node1List using DevList(1),DevList(2);



insert into lhcb_path_temp select p.pathid,1,p.node1,t.port_nbr,t.port_way, p.node2,f.port_nbr ,f.port_way   from (select pathid,node1,node2 from lhcb_pdetails  where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node1=f.deviceid and p.node2=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node2!=0 ;
insert into lhcb_path_temp select p.pathid,2,p.node2,t.port_nbr,t.port_way, p.node3,f.port_nbr ,f.port_way   from (select pathid,node1,node2,node3 from lhcb_pdetails  where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node2=f.deviceid and p.node3=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node3!=0  ;

insert into lhcb_path_temp select p.pathid,3,p.node3,t.port_nbr,t.port_way, p.node4,f.port_nbr ,f.port_way   from (select pathid,node1,node3,node4 from lhcb_pdetails  where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node3=f.deviceid and p.node4=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node4!=0;

insert into lhcb_path_temp select p.pathid,4,p.node4,t.port_nbr,t.port_way, p.node5,f.port_nbr ,f.port_way   from (select pathid,node1,node4,node5 from lhcb_pdetails  where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node4=f.deviceid and p.node5=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node5!=0;

insert into lhcb_path_temp select p.pathid,5,p.node5,t.port_nbr,t.port_way, p.node6,f.port_nbr ,f.port_way   from (select pathid,node1,node5,node6 from lhcb_pdetails  where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node5=f.deviceid and p.node6=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node6!=0;

insert into lhcb_path_temp select p.pathid,6,p.node6,t.port_nbr,t.port_way, p.node7,f.port_nbr ,f.port_way   from (select pathid,node1,node6,node7 from lhcb_pdetails  where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node6=f.deviceid and p.node7=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node7!=0;

insert into lhcb_path_temp select p.pathid,7,p.node7,t.port_nbr,t.port_way, p.node8,f.port_nbr ,f.port_way   from (select pathid,node1,node7,node8 from lhcb_pdetails  where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node7=f.deviceid and p.node8=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node8!=0;

insert into lhcb_path_temp select p.pathid,8,p.node8,t.port_nbr,t.port_way, p.node9,f.port_nbr ,f.port_way   from (select pathid,node1,node8, node9 from lhcb_pdetails  where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node8=f.deviceid and p.node9=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node9!=0;

insert into lhcb_path_temp select p.pathid,9,p.node9,t.port_nbr,t.port_way, p.node10,f.port_nbr ,f.port_way   from (select pathid,node1,node9,node10 from lhcb_pdetails  where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node9=f.deviceid and p.node10=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node10!=0;

insert into lhcb_path_temp select p.pathid,10,p.node10,t.port_nbr,t.port_way, p.node11,f.port_nbr ,f.port_way   from (select pathid,node1,node10,node11 from lhcb_pdetails  where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node10=f.deviceid and p.node11=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node10!=0;



else

res_query:=routingtable_pck.createNodeLink_tab(NodeList(1),1);

if(res_query=0) then
	res_query:=routingtable_pck.createPathTab(DevList(1),1,10);
else
tabname:='ERROR_IN_CREATING_PATHDETAILS';
return tabname;
end if;

if(res_query=0) then
	execute immediate 'select count(*) from lhcb_pdet_temp where (node2-:dev1)*(node3-:dev1)*(node4-:dev1)*(node5-:dev1)*(node6-:dev1)*(node7-:dev1)*(node8-:dev1)*(node9-:dev1)*(node10-:dev1)*(node11-:dev1)=0 and rownum=1' into i  using DevList(2);
	if i=1 then
		execute immediate 'select pathid,node1 from lhcb_pdet_temp where (node2-:dev1)*(node3-:dev1)*(node4-:dev1)*(node5-:dev1)*(node6-:dev1)*(node7-:dev1)*(node8-:dev1)*(node9-:dev1)*(node10-:dev1)*(node11-:dev1)=0' bulk collect into pathidList,node1List using DevList(2);
	else
		commit;
		res_query:=routingtable_pck.createNodeLink_tab(NodeList(2),1);
		if res_query=0 then 
			execute immediate 'select count(*) from lhcb_pdet_temp where (node2-:dev1)*(node3-:dev1)*(node4-:dev1)*(node5-:dev1)*(node6-:dev1)*(node7-:dev1)*(node8-:dev1)*(node9-:dev1)*(node10-:dev1)*(node11-:dev1)=0 and (node1-:dev2)*(node2-:dev2)*(node3-:dev2)*(node4-:dev2)*(node5-:dev2)*(node6-:dev2)*(node7-:dev2)*(node8-:dev2)*(node9-:dev2)*(node10-:dev2)*(node11-:dev2)=0  and rownum=1' into i  using DevList(1);
			if i=1 then
				execute immediate 'select pathid,node1 from lhcb_pdet_temp where (node1-:dev1)*(node2-:dev1)*(node3-:dev1)*(node4-:dev1)*(node5-:dev1)*(node6-:dev1)*(node7-:dev1)*(node8-:dev1)*(node9-:dev1)*(node10-:dev1)*(node11-:dev1)=0  ' bulk collect into pathidList,node1List using DevList(1);
			
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
end if;
insert into lhcb_path_temp select p.pathid,1,p.node1,t.port_nbr,t.port_way, p.node2,f.port_nbr ,f.port_way   from (select pathid,node1,node2 lhcb_pdet_temp p where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node1=f.deviceid and p.node2=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node2!=0 ;
insert into lhcb_path_temp select p.pathid,2,p.node2,t.port_nbr,t.port_way, p.node3,f.port_nbr ,f.port_way   from (select pathid,node1,node2,node3 lhcb_pdet_temp p where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node2=f.deviceid and p.node3=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node3!=0  ;

insert into lhcb_path_temp select p.pathid,3,p.node3,t.port_nbr,t.port_way, p.node4,f.port_nbr ,f.port_way   from (select pathid,node1,node3,node4 lhcb_pdet_temp p where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node3=f.deviceid and p.node4=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node4!=0;

insert into lhcb_path_temp select p.pathid,4,p.node4,t.port_nbr,t.port_way, p.node5,f.port_nbr ,f.port_way   from (select pathid,node1,node4,node5 lhcb_pdet_temp p where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node4=f.deviceid and p.node5=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node5!=0;

insert into lhcb_path_temp select p.pathid,5,p.node5,t.port_nbr,t.port_way, p.node6,f.port_nbr ,f.port_way   from (select pathid,node1,node5,node6 lhcb_pdet_temp p where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node5=f.deviceid and p.node6=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node6!=0;

insert into lhcb_path_temp select p.pathid,6,p.node6,t.port_nbr,t.port_way, p.node7,f.port_nbr ,f.port_way   from (select pathid,node1,node6,node7 lhcb_pdet_temp p where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node6=f.deviceid and p.node7=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node7!=0;

insert into lhcb_path_temp select p.pathid,7,p.node7,t.port_nbr,t.port_way, p.node8,f.port_nbr ,f.port_way   from (select pathid,node1,node7,node8 lhcb_pdet_temp p where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node7=f.deviceid and p.node8=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node8!=0;

insert into lhcb_path_temp select p.pathid,8,p.node8,t.port_nbr,t.port_way, p.node9,f.port_nbr ,f.port_way   from (select pathid,node1,node8, node9 lhcb_pdet_temp p where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node8=f.deviceid and p.node9=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node9!=0;

insert into lhcb_path_temp select p.pathid,9,p.node9,t.port_nbr,t.port_way, p.node10,f.port_nbr ,f.port_way   from (select pathid,node1,node9,node10 lhcb_pdet_temp p where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node9=f.deviceid and p.node10=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node10!=0;

insert into lhcb_path_temp select p.pathid,10,p.node10,t.port_nbr,t.port_way, p.node11,f.port_nbr ,f.port_way   from (select pathid,node1,node10,node11 lhcb_pdet_temp p where (pathid,node1) in (select a.column_value,b.column_value from table(cast(pathidList as devid_list)) a, table(cast(node1List as devid_list)) b)) p, lhcb_port_properties t,lhcb_port_properties f ,lhcb_macro_connectivity h where p.pathid=:pathid and p.node1=:node1 and p.node10=f.deviceid and p.node11=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (h.portid=h.portidfrom and f.portidto=g.portid)) and p.node10!=0;

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
end GiveDestinationTableName;
/


