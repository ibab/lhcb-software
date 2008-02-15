create or replace function GetAllPathes(devfrom varchar2,systemname number,nextquery number) return varchar2 AUTHID CURRENT_USER is
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
sqlstmt varchar2(1000);
--source_cursor number:=dbms_sql.open_cursor;
begin
--check that the 2 devices are part of the same subsystem: if not-> not implemented (we don't give path between a sub farm node and a L0 board in the VELO doesn't make sense!)
execute immediate 'select t.deviceid,t.system_name from lhcb_lg_devices t  where t.devicename=:dfrom  and mod(t.system_name,:sysID)=0 ' into DevList(1),NodeList(1) using devfrom, systemname;

if systemname=1 then
	sys_first:=NodeList(1);
else
	sys_first:=systemname;
end if;
sqlstmt:='select pathid||'':''||node1 from lhcb_pdetails where (node1-:dev1)*(node2-:dev1)*(node3-:dev1)*(node4-:dev1)*(node5-:dev1)*(node6-:dev1)*(node7-:dev1)*(node8-:dev1)*(node9-:dev1)*(node10-:dev1)*(node11-:dev1)=0 and path_necessary=1 and (mod(system_name,:sysname)=0 or mod(:sysname,system_name)=0)' ;
-- next_query==0 means no need to look for the first link
-- next_query==1 means we only need to look for the first link where lk_weight=1
-- next_query==2 means we only need to look for the first link where lk_weight=2 + bidirec -- we will get rid of
-- next_query==3 means we only need to look for the first link where lk_weight=2 + bidirec and link_weight=1  -- we will get rid of
case nextquery 
when 0 then

execute immediate 'insert into lhcb_path_temp select p.pathid,1,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node2,f.port_nbr ,p.system_name   from (select pathid,node1,node2,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' ))p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node1=f.deviceid and p.node2=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node2!=0 ' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,2,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node3,f.port_nbr ,p.system_name   from (select pathid,node1,node2,node3,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node2=f.deviceid and p.node3=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node3!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,3,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node4,f.port_nbr ,p.system_name   from (select pathid,node1,node3,node4 ,system_name from lhcb_pdetails  where (pathid||'':''||node1) in  ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node3=f.deviceid and p.node4=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node4!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,4,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node5,f.port_nbr ,p.system_name   from (select pathid,node1,node4,node5,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p,lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node4=f.deviceid and p.node5=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node5!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,5,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node6,f.port_nbr ,p.system_name   from (select pathid,node1,node5,node6,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node5=f.deviceid and p.node6=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node6!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,6,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node7,f.port_nbr ,p.system_name  from (select pathid,node1,node6,node7,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node6=f.deviceid and p.node7=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node7!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,7,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node8,f.port_nbr ,p.system_name  from (select pathid,node1,node7,node8,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node7=f.deviceid and p.node8=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node8!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,8,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node9,f.port_nbr ,p.system_name   from (select pathid,node1,node8, node9,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node8=f.deviceid and p.node9=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node9!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,9,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node10,f.port_nbr ,p.system_name   from (select pathid,node1,node9,node10,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' ) ) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node9=f.deviceid and p.node10=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,10,p.node1,g.port_nbr,decode(g.port_way,1,h.linkid,-h.linkid), p.node11,f.port_nbr ,p.system_name   from (select pathid,node1,node10,node11,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node10=f.deviceid and p.node11=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;



when 1 then

execute immediate 'insert into lhcb_path_temp select 0,1,p.node1,d.port_nbr,h.linkid,p.node1,f.port_nbr,p.system_name from (select pathid,node1,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p,lhcb_macroscopic_connectivity h,lhcb_port_properties d,lhcb_port_properties f where p.node1=f.deviceid and f.portid=h.portidto and h.portidfrom=d.portid and mod(h.system_name,p.system_name)=0 and h.link_weight=1' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;
if(SQL%rowcount>0) then
	execute immediate 'insert into lhcb_path_temp select p.pathid,2,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node2,f.port_nbr ,p.system_name   from (select pathid,node1,node2,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node1=f.deviceid and p.node2=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node2!=0 ' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;
else
return tabname_cur;	
end if;
if(sql%rowcount>0) then
execute immediate 'insert into lhcb_path_temp select p.pathid,3,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node3,f.port_nbr ,p.system_name   from (select pathid,node1,node2,node3,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node2=f.deviceid and p.node3=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node3!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first ,sys_first;
else
return tabname_cur;	
end if;
if(SQL%rowcount>0) then
execute immediate 'insert into lhcb_path_temp select p.pathid,4,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node4,f.port_nbr ,p.system_name   from (select pathid,node1,node3,node4 ,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node3=f.deviceid and p.node4=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node4!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;
else
return tabname_cur;	
end if;
if(SQL%rowcount>0) then
execute immediate 'insert into lhcb_path_temp select p.pathid,5,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node5,f.port_nbr ,p.system_name   from (select pathid,node1,node4,node5,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p,lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node4=f.deviceid and p.node5=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node5!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;
else
return tabname_cur;	
end if;
if(SQL%rowcount>0) then
execute immediate 'insert into lhcb_path_temp select p.pathid,6,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node6,f.port_nbr ,p.system_name   from (select pathid,node1,node5,node6,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node5=f.deviceid and p.node6=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node6!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;
else
return tabname_cur;	
end if;
if(SQL%rowcount>0) then
execute immediate 'insert into lhcb_path_temp select p.pathid,7,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node7,f.port_nbr ,p.system_name  from (select pathid,node1,node6,node7,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node6=f.deviceid and p.node7=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node7!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;
else
return tabname_cur;	
end if;
if(SQL%rowcount>0) then
execute immediate 'insert into lhcb_path_temp select p.pathid,8,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node8,f.port_nbr ,p.system_name  from (select pathid,node1,node7,node8,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node7=f.deviceid and p.node8=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node8!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;
else
return tabname_cur;	
end if;
if(SQL%rowcount>0) then
execute immediate 'insert into lhcb_path_temp select p.pathid,9,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node9,f.port_nbr ,p.system_name   from (select pathid,node1,node8, node9,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node8=f.deviceid and p.node9=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node9!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;
else
return tabname_cur;	
end if;
if(SQL%rowcount>0) then
execute immediate 'insert into lhcb_path_temp select p.pathid,10,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node10,f.port_nbr ,p.system_name   from (select pathid,node1,node9,node10,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' ) ) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node9=f.deviceid and p.node10=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;
else
return tabname_cur;	
end if;
if(SQL%rowcount>0) then
execute immediate 'insert into lhcb_path_temp select p.pathid,11,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node11,f.port_nbr ,p.system_name   from (select pathid,node1,node10,node11,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node10=f.deviceid and p.node11=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;
else
return tabname_cur;	
end if;

when 2 then
execute immediate 'insert into lhcb_path_temp select 0,1,p.node1,d.port_nbr,-h.linkid,p.node1,f.port_nbr,p.system_name from (select pathid,node1,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p,lhcb_macroscopic_connectivity h,lhcb_port_properties d,lhcb_port_properties f where p.node1=f.deviceid and f.portid=h.portidfrom and h.portidto=d.portid and mod(h.system_name,p.system_name)=0 and h.link_weight=2 and h.bidirectiona_link_used=1' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;


execute immediate 'insert into lhcb_path_temp select p.pathid,2,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node2,f.port_nbr ,p.system_name   from (select pathid,node1,node2,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' ))p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node1=f.deviceid and p.node2=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node2!=0 ' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,3,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node3,f.port_nbr ,p.system_name   from (select pathid,node1,node2,node3,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node2=f.deviceid and p.node3=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node3!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first ;

execute immediate 'insert into lhcb_path_temp select p.pathid,4,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node4,f.port_nbr ,p.system_name   from (select pathid,node1,node3,node4 ,system_name from lhcb_pdetails  where (pathid||'':''||node1) in  ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node3=f.deviceid and p.node4=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node4!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,5,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node5,f.port_nbr ,p.system_name   from (select pathid,node1,node4,node5,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p,lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node4=f.deviceid and p.node5=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node5!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,6,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node6,f.port_nbr ,p.system_name   from (select pathid,node1,node5,node6,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node5=f.deviceid and p.node6=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node6!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,7,p.node16,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node7,f.port_nbr ,p.system_name  from (select pathid,node1,node6,node7,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node6=f.deviceid and p.node7=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node7!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,8,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node8,f.port_nbr ,p.system_name  from (select pathid,node1,node7,node8,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node7=f.deviceid and p.node8=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node8!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,9,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node9,f.port_nbr ,p.system_name   from (select pathid,node1,node8, node9,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node8=f.deviceid and p.node9=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node9!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,10,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node10,f.port_nbr ,p.system_name   from (select pathid,node1,node9,node10,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' ) ) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node9=f.deviceid and p.node10=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,11,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node11,f.port_nbr ,p.system_name   from (select pathid,node1,node10,node11,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node10=f.deviceid and p.node11=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

when 3 then
execute immediate 'insert into lhcb_path_temp select 0,1,p.node1,d.port_nbr,h.linkid,p.node1,f.port_nbr,p.system_name from (select pathid,node1,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p,lhcb_macroscopic_connectivity h,lhcb_port_properties d,lhcb_port_properties f where p.node1=f.deviceid and f.portid=h.portidto and h.portidfrom=d.portid and mod(h.system_name,p.system_name)=0 and h.link_weight=1' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select 0,1,p.node1,d.port_nbr,-h.linkid,p.node1,f.port_nbr,p.system_name from (select pathid,node1,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p,lhcb_macroscopic_connectivity h,lhcb_port_properties d,lhcb_port_properties f where p.node1=f.deviceid and f.portid=h.portidfrom and h.portidto=d.portid and mod(h.system_name,p.system_name)=0 and h.link_weight=2 and h.bidirectiona_link_used=1' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;


execute immediate 'insert into lhcb_path_temp select p.pathid,2,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node2,f.port_nbr ,p.system_name   from (select pathid,node1,node2,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' ))p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node1=f.deviceid and p.node2=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node2!=0 ' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,3,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node3,f.port_nbr ,p.system_name   from (select pathid,node1,node2,node3,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node2=f.deviceid and p.node3=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node3!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first ,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,4,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node4,f.port_nbr ,p.system_name   from (select pathid,node1,node3,node4 ,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node3=f.deviceid and p.node4=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node4!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,5,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node5,f.port_nbr ,p.system_name   from (select pathid,node1,node4,node5,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p,lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node4=f.deviceid and p.node5=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node5!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,6,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node6,f.port_nbr ,p.system_name   from (select pathid,node1,node5,node6,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node5=f.deviceid and p.node6=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node6!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,7,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node7,f.port_nbr ,p.system_name  from (select pathid,node1,node6,node7,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node6=f.deviceid and p.node7=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node7!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,8,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node8,f.port_nbr ,p.system_name  from (select pathid,node1,node7,node8,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node7=f.deviceid and p.node8=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node8!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,9,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node9,f.port_nbr ,p.system_name   from (select pathid,node1,node8, node9,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where p.node8=f.deviceid and p.node9=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node9!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,10,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node10,f.port_nbr ,p.system_name   from (select pathid,node1,node9,node10,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' ) ) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node9=f.deviceid and p.node10=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;

execute immediate 'insert into lhcb_path_temp select p.pathid,11,p.node1,g.port_nbr,decode(g.port_way,2,-h.linkid,h.linkid), p.node11,f.port_nbr ,p.system_name   from (select pathid,node1,node10,node11,system_name from lhcb_pdetails  where (pathid||'':''||node1) in ('||sqlstmt||' )) p, lhcb_port_properties g,lhcb_port_properties f ,lhcb_macroscopic_connectivity h where  p.node10=f.deviceid and p.node11=g.deviceid and ((f.portid=h.portidfrom and h.portidto=g.portid) or (g.portid=h.portidfrom and h.portidto=f.portid)) and p.node10!=0' using DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),DevList(1),sys_first,sys_first;


else
i:=0;
end case;


return tabname_cur;	
exception
when NO_DATA_FOUND then
if sys_first=1 then
tabname:='ERROR_DEVICE_NOT_IN_SYSTEM';
else
tabname:='ERROR_DEVICE_NOT_CONNECTED';
end if;
--if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
--dbms_sql.close_cursor(source_cursor);
--end if;
return tabname;
when OTHERS then
tabname:='ERROR_'||sqlerrm;
--if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
--dbms_sql.close_cursor(source_cursor);
--end if;
return tabname;
end GetAllPathes;
/


