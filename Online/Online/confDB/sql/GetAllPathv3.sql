create or replace function GetAllPathesv2(devfrom varchar2,systemname number) return varchar2 AUTHID CURRENT_USER is
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
node number:=-1;
sysname number:=0;
rescount number:=0;
rescount_old number:=0;
i number:=1;
j number:=0;
k number:=0;
case_res number:=-5;
devname varchar2(200);
res_query number:=0;
begin


execute immediate 'select t.deviceid,t.system_name,t.node from lhcb_lg_devices t  where t.devicename=:dfrom  and mod(t.system_name,:sysID)=0 ' into deviceid,sysname,node using devfrom, systemname;

--execute immediate 'select deviceid,system_name,node from lhcb_lg_devices where devicename=upper(:dfrom)' into deviceid,sysname,node using devfrom;
res_query:=routingtable_pck.createNodeLink_tab(systemname,1,1);
select count(*) into rescount from lhcb_lg_links;
DBMS_OUTPUT.PUT_LINE ('rescount='||rescount );

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
--select count(*) into rescount from lhcb_lg_links;
--DBMS_OUTPUT.PUT_LINE ('rescount='||rescount );

insert into lhcb_path_temp select p.pathid,1,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node2,f.port_nbr ,p.system_name   from (select pathid,node1,node2,system_name from lhcb_pdet_temp)p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node1=f.deviceid and p.node2=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node2!=0 ;

insert into lhcb_path_temp select p.pathid,2,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node3,f.port_nbr ,p.system_name   from (select pathid,node1,node2,node3,system_name from lhcb_pdet_temp) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node2=f.deviceid and p.node3=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node3!=0;

insert into lhcb_path_temp select p.pathid,3,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node4,f.port_nbr ,p.system_name   from (select pathid,node1,node3,node4 ,system_name from lhcb_pdet_temp) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node3=f.deviceid and p.node4=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node4!=0;

insert into lhcb_path_temp select p.pathid,4,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node5,f.port_nbr ,p.system_name   from (select pathid,node1,node4,node5,system_name from lhcb_pdet_temp) p,lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node4=f.deviceid and p.node5=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node5!=0;

insert into lhcb_path_temp select p.pathid,5,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node6,f.port_nbr ,p.system_name   from (select pathid,node1,node5,node6,system_name from lhcb_pdet_temp) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node5=f.deviceid and p.node6=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node6!=0;

insert into lhcb_path_temp select p.pathid,6,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node7,f.port_nbr ,p.system_name  from (select pathid,node1,node6,node7,system_name from lhcb_pdet_temp) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node6=f.deviceid and p.node7=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node7!=0;

insert into lhcb_path_temp select p.pathid,7,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node8,f.port_nbr ,p.system_name  from (select pathid,node1,node7,node8,system_name from lhcb_pdet_temp) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node7=f.deviceid and p.node8=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node8!=0;

insert into lhcb_path_temp select p.pathid,8,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node9,f.port_nbr ,p.system_name   from (select pathid,node1,node8, node9,system_name from lhcb_pdet_temp) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node8=f.deviceid and p.node9=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node9!=0 ;

insert into lhcb_path_temp select p.pathid,9,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node10,f.port_nbr ,p.system_name   from (select pathid,node1,node9,node10,system_name from lhcb_pdet_temp) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node9=f.deviceid and p.node10=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0 ;
insert into lhcb_path_temp select p.pathid,10,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node11,f.port_nbr ,p.system_name   from (select pathid,node1,node10,node11,system_name from lhcb_pdet_temp) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node10=f.deviceid and p.node11=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0 ;



return tabname_cur;	
exception
when NO_DATA_FOUND then
if node=-1 then
tabname:='ERROR_DEVICE_NOT_IN_SYSTEM';
else
tabname:='ERROR_DEVICE_NOT_CONNECTED';
end if;
return tabname;
when OTHERS then
tabname:='ERROR_'||sqlerrm;
return tabname;
end GetAllPathesv2;
/


