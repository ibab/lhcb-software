---------------------------------------------
--PL/SQL routing and destination package
--author L.Abadie
--v2.0
------------------------------------
create or replace package body routingtable_pck
is
-----------------------------------------------------------
--Convention we always give sysname=sysname+'_': i.e. if it's VELO, we give VELO_
-----------------------------------------------------------
----------------------------------------------------------
----fct which creates the logical view of the system : we just keep link between devices (get rid of port nb)
----------------------------------------------------------
function createNodeLink_tab(sysname varchar2) return number is 
node_table varchar2(60):=sysname||'logical_links';
node_table_view varchar2(60):=sysname||'logical_links_view';

res_query number:=0;
exist_tab number:=0;
exist_tab1 number:=0;
exist_tab2 number:=0;
exist_tab3 number:=0;
v_error_message varchar2(4000);
conn_table varchar2(60):=sysname||'connectivity';
port_table varchar2(60):=sysname||'port_properties';
link_table varchar2(60):=sysname||'linkpairs';
link_table_view varchar2(60):=sysname||'linkpairs_view';

begin	
	
	execute immediate 'truncate table '||node_table;
	execute immediate 'truncate table '||link_table;
	select count(table_name) into exist_tab1 from user_tables where table_name=upper(port_table);
	select count(table_name) into exist_tab3 from user_tables where table_name=upper(conn_table);
	execute immediate 'insert  into '||node_table||' (linkid,node_from,pfrom,node_to,pto, link_weight,lktrunk, lktype,lkused,last_update) select t.linkid,t.node_from,t.port_nbrfrom,t.node_to,t.port_nbrto,t.link_weight, 0,t.link_type,t.lkused, sysdate from '||conn_table||' t where t.port_nbrfrom=(select min(port_nbrfrom) from '||conn_table||' where t.trunk_lkid=0 and t.node_from=node_from and t.node_to=node_to)';
	execute immediate 'insert into '||node_table||' select -t.linkid,t.node_to,t.pto, t.node_from,t.pfrom, decode(t.link_weight,1,2,2,1,0), 0 ,t.lktype,t.lkused,sysdate from '||node_table||' t, '||conn_table||' e where e.trunk_lkid=0 and t.linkid=e.linkid and  e.BIDIRECTIONAL_LINK_USED=1 ';
	if(exist_tab1>0) then
		execute immediate 'insert into '||node_table||' select t.linkid,t.node_from,t.port_nbrfrom,t.node_to,t.port_nbrto,t.link_weight, 1 ,t.link_type, t.lkused,sysdate from  '||conn_table||' t, '||sysname||'trunk_link e where t.trunk_lkid=e.LINK_TRUNKID and (t.node_from,t.port_nbrfrom)=(select deviceid,port_nbr from '||sysname||'port_properties where portid=(select portid from '||sysname||'ip_ethernet where ethernet_add=e.LKAGGREG_ADD))';
		execute immediate 'insert  into '||node_table||' select -t.linkid,t.node_to,t.pto,t.node_from,t.pfrom,t.link_weight, 1 ,t.lktype, t.lkused,sysdate from '||node_table||' t, '||conn_table||' e where t.linkid=e.linkid and t.lktrunk=1 and e.BIDIRECTIONAL_LINK_USED=1';
	end if;
	execute immediate 'insert  into '||link_table||' select  t.link_weight+s.link_weight,t.node_from,t.node_to, s.node_to, case when l.link_nbr > f.link_nbr then t.lktype else s.lktype end, t.lkused*s.lkused,sysdate from '||node_table||' t, '||node_table||' s ,'||sysname||'link_type l,'||sysname||'link_type f where t.link_weight=0   and t.node_to=s.node_from and t.node_from!=s.node_to and t.lktype=l.linktypeid and s.lktype=f.linktypeid and mod(f.link_nbr,l.link_nbr)*mod(l.link_nbr,f.link_nbr)=0';
	return 0;
exception
when NO_DATA_FOUND then
return -1;
when OTHERS then
v_error_message:=sqlerrm;
return sqlcode;
end createNodeLink_tab;
----------------------------------------------------------
----fct which creates or recreates the routing table of a device: 
-- the routing table is stored in the sysname||routingtable
--- only one version of routing table can be stored (no versioning)
--requires the existence of a MAC table
----------------------------------------------------------
-----------------------------------------------------------
function createRT_all(sysname varchar2,devname number,path_necessary number default 0,round_trip number default 10) return number is 
v_error_message varchar2(1000);
res_query number:=0;
res_query1 number:=1;
time_before number;
exist_tab number:=1;
connectivitytab varchar2(60):=sysname||'connectivity';
--connectivitytab varchar2(50);
node_table varchar2(60):=sysname||'logical_links';
maxval date;
minval date;

begin
time_before := DBMS_UTILITY.GET_TIME;
--check if the logical table is up to date
execute immediate 'select max(last_update) from '||connectivitytab into maxval;
execute immediate 'select min(last_update) from '||node_table into minval;
if(maxval>minval) then --need to update the logical
	res_query:=routingtable_pck.createNodeLink_tab(sysname);
end if;	
if(res_query=0) then
	res_query:=routingtable_pck.createPathTab(sysname,devname,1,round_trip);
else
	commit;
	return res_query;
end if;

if(res_query=0) then
	res_query1:=routingtable_pck.createRoutingTable_SP(sysname,devname);
else
	commit;
	return res_query;
end if;
if(res_query1=0) then
res_query:=routingtable_pck.Insert_firsttab(sysname ,devname,1,path_necessary);
end if;
commit;

--DBMS_OUTPUT.PUT_LINE ('time execution to create the routing table '|| (DBMS_UTILITY.GET_TIME-time_before)/100);
return res_query1;
exception
when NO_DATA_FOUND then
res_query:=-1;
	commit;
return res_query;
when OTHERS then
res_query:=sqlcode;
v_error_message:=sqlerrm;
commit;

return res_query;
end createRT_all;
----------------------------------------------------------
--fct which performs the final insert into the big ones
-- we delete the previous rows regarding the order as routingtable and destination table have foreign key to pathdetails
----------------------------------------------------------

-----------------------------------------------------------
function Insert_firsttab(sysname varchar2,devname number, rt_needed number,path_necessary number) return number is 
tabname varchar2(100):=sysname||'pathdetails';
tabname_temp varchar2(100):=sysname||'pathdetails_temporary';
tabname1 varchar2(100):=sysname||'routingtable';
tabname1_temp  varchar2(100):=sysname||'startend_temporary';
tabname2_temp varchar2(100):=sysname||'routingtab_temporary';
tabname3 varchar2(100):=sysname||'destinationtable';
tabname3_temp varchar2(100):=sysname||'destintab_temporary';
ck_existence number:=0;
res_query number;
rescount number;
date_update date :=sysdate;
v_error_message varchar2(4000);
begin
execute immediate 'select count(*) from user_tables where table_name=upper(:1)' into rescount using tabname1; 
if(rescount=1) then
execute immediate 'delete from '||tabname1||' where nodeid_start0=:1' using devname;
end if;
execute immediate 'delete from '||tabname3||' where nodeid_start0=:1' using devname;
execute immediate 'delete from '||tabname||' where node1=:1' using devname;

if(rt_needed=1) then
execute immediate 'insert into '||tabname||' select t.pathid ,t.path_weight,t.path_type ,t.pathused , t.node1 ,t.node2 ,t.node3 ,t.node4 ,t.node5,t.node6,t.node7,t.node8,t.node9,t.node10,g.nodeid_end1,sysdate,'||path_necessary||' from '||tabname_temp||' t,  '||tabname2_temp||' f ,'||tabname1_temp||' g where  t.pathid=f.pathid and t.pathid=g.pathid';

execute immediate 'insert into '||tabname1||' select t.pathid,t.pathused,t.nodeid_start0,t.nodeid_start1,t.nodeid_end0,t.nodeid_end1,s.port_nb,s.ethernet_add_destin,s.ethernet_add_nexthop,t.path_type,t.path_length , sysdate from '||tabname1_temp||' t,'||tabname2_temp||' s where t.pathid=s.pathid ';

else
execute immediate 'insert into '||tabname||' select t.pathid ,t.path_weight,t.path_type ,t.pathused , t.node1 ,t.node2 ,t.node3 ,t.node4 ,t.node5,t.node6,t.node7,t.node8,t.node9,t.node10,g.nodeid_end1 , sysdate,'||path_necessary||' from '||tabname_temp||' t,  '||tabname3_temp||' f ,'||tabname1_temp||' g where  t.pathid=f.pathid and t.pathid=g.pathid';

execute immediate 'insert into '||tabname3||' select t.pathid,t.pathused,t.nodeid_start0,t.nodeid_start1,t.nodeid_end0,t.nodeid_end1,s.port_nb,s.destination_name,s.port_nb_destin,t.path_type,t.path_length ,sysdate from '||tabname1_temp||' t,'||tabname3_temp||' s where t.pathid=s.pathid ';

end if;
return 0;
exception
when NO_DATA_FOUND then
res_query:=-1;
return res_query;
when OTHERS then
res_query:=sqlcode;
v_error_message:=sqlerrm;
return res_query;
end Insert_firsttab;
----------------------------------------------------------
--no longer used 
----------------------------------------------------------

function InsertRT_tab(sysname varchar2,devname number) return number is 
tabname1 varchar2(100):=sysname||'routingtable';
tabname1_temp  varchar2(100):=sysname||'startend_temporary';
tabname2_temp varchar2(100):=sysname||'routingtab_temporary';
ck_existence number:=0;
res_query number;
v_error_message varchar2(4000);
begin
execute immediate 'delete from '||tabname1||' where nodeid_start0=:1' using devname;
execute immediate 'insert into '||tabname1||' select t.pathid,t.pathused,t.nodeid_start0,t.nodeid_start1,t.nodeid_end0,t.nodeid_end1,s.port_nb,s.ethernet_add_destin,s.ethernet_add_nexthop,t.path_type,t.path_length from '||tabname1_temp||' t,'||tabname2_temp||' s where t.pathid=s.pathid and t.nodeid_start0=s.nodeid_start0';
return 0;
exception
when NO_DATA_FOUND then
res_query:=-1;
return res_query;
when OTHERS then
res_query:=sqlcode;
v_error_message:=sqlerrm;
return res_query;
end InsertRT_tab;
----------------------------------------------------------
--no longer used 
----------------------------------------------------------
function InsertDestin_tab(sysname varchar2,devname number) return number is 
tabname1 varchar2(100):=sysname||'destinationtable';
tabname1_temp  varchar2(100):=sysname||'startend_temporary';
tabname2_temp varchar2(100):=sysname||'destintab_temporary';
ck_existence number:=0;
v_error_message varchar2(4000);
res_query number;
begin

execute immediate 'delete from '||tabname1||' where nodeid_start0=:1' using devname;
execute immediate 'insert into '||tabname1||' select t.pathid,t.pathused,t.nodeid_start0,t.nodeid_start1,t.nodeid_end0,t.nodeid_end1,s.port_nb,s.destination_name,s.port_nb_destin,t.path_type,t.path_length from '||tabname1_temp||' t,'||tabname2_temp||' s where t.pathid=s.pathid and t.nodeid_start0=s.nodeid_start0';
return 0;
exception
when NO_DATA_FOUND then
res_query:=-1;
return res_query;
when OTHERS then
res_query:=sqlcode;
v_error_message:=sqlerrm;
return res_query;
end InsertDestin_tab;
----------------------------------------------------------
----fct which creates or recreates the routing table of a device: 
-- the routing table is stored in the sysname||routingtable
--- only one version of routing table can be stored (no versioning)
--requires the existence of a MAC table
-- you pass the devicename instead of the deviceid
----------------------------------------------------------
function createRT_all_devname(sysname varchar2,devname varchar2,path_necessary number default 0,round_trip number default 10) return number is 
v_error_message varchar2(1000);
res_query number:=0;
res_query1 number:=0;
time_before number;
devid number:=0;
node number:=0;
maxval date;
minval date;
connectivitytab varchar2(60):=sysname||'connectivity';
node_table varchar2(60):=sysname||'logical_links';
exist_tab  number;
begin
time_before := DBMS_UTILITY.GET_TIME;
execute immediate 'select max(last_update) from '||connectivitytab into maxval;
execute immediate 'select min(last_update) from '||node_table into minval;
if(maxval>minval) then --need to update the logical
	res_query:=routingtable_pck.createNodeLink_tab(sysname);
end if;	

execute immediate 'select deviceid, node from '||sysname||'device where devicename=:devname' into devid,node using devname;
if(node=0) then
	res_query:=routingtable_pck.createPathTab(sysname,devid,1,round_trip);
else
	res_query:=routingtable_pck.createPathTab_Host(sysname,devid,1,round_trip);
end if;
if(res_query=0) then
	res_query1:=routingtable_pck.createRoutingTable_SP(sysname,devid);
else
 return res_query;
end if;
if(res_query1=0) then
	res_query:=routingtable_pck.Insert_firsttab(sysname ,devid,1, path_necessary);
end if;
commit;


--DBMS_OUTPUT.PUT_LINE ('time execution to create the routing table '|| (DBMS_UTILITY.GET_TIME-time_before)/100);
return res_query1;
exception
when NO_DATA_FOUND then
res_query:=-1;

commit;

return res_query;
when OTHERS then
res_query:=sqlcode;
v_error_message:=sqlerrm;

	commit;

return res_query;
end createRT_all_devname;
-----------------------------------------------------------
-- find all the valid paths of a length less than 10 starting from that device
-----------------------------------------------------------
function createPathTab(sysname varchar2, devfrom number,rt_needed number, round_trip number default 10) return number is
v_error_message varchar2(500);
sql_statement varchar2(4000);
sql_state_temp1 long;
sql_var varchar2(4000);
sql_state_temp11 long;
sql_state_temp2 varchar2(4000);
stop number:=0;
i number:=1;
j number:=1;
l number :=0;
k number:=0;
tabname1 varchar2(100);
tabname2 varchar2(100);
count_var number:=3;
exist_tab number:=0;
pfrom number:=0;
pto number:=0;
seq_name varchar2(100);
res_create number:=0;
seq_created number:=0;
path_copy_created number:=0;
path_copy1_created number:=0;
res_temp number:=1;
time_before number;
time_after number;
tabname varchar2(100):=sysname||'pathdetails_temporary';
nodetab varchar2(100):=sysname||'logical_links';
linktab varchar2(100):=sysname||'linkpairs';
begin
time_before := DBMS_UTILITY.GET_TIME;
tabname1:=sysname||'startend_temporary';
seq_name:=sysname||'sp_'||devfrom;
DBMS_OUTPUT.ENABLE( 10000000);

l:=routingtable_pck.CheckExistenceSeq(seq_name);
--DBMS_OUTPUT.PUT_LINE ('after existnce '||l);

if(l=1) then
	
	UpdateSequence(seq_name);
else
	execute immediate 'create sequence '||seq_name||' start with 0 increment by 1 MINVALUE 0';
	execute immediate 'select '||seq_name||'.nextval from dual' into seq_created;
end if;
seq_created:=1;
	
--DBMS_OUTPUT.PUT_LINE ('sequence created '||l);


execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused , node1,node2) select '||seq_name||'.nextval ,link_weight,lktype,lkused,node_from,node_to from '||nodetab||' where node_from=:dfrom and link_weight=2' using devfrom;
execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused , node1,node2,node3) select '||seq_name||'.nextval ,link_weight,lktype,lkused,node1,node2,node3 from '||linktab||' where node1=:dfrom  ' using devfrom;

i:=3;
execute immediate 'insert into '||tabname1||' select pathid,pathused,node1,node2,node1,node2,path_type,1 from '||tabname||' where path_weight=2 and node3=0';
execute immediate 'insert into '||tabname1||' select pathid,pathused, node1,node2,node2,node3,path_type,2 from '||tabname||' where path_weight=2 and node3!=0';

sql_state_temp1:='t.node1,t.node2,';
sql_state_temp2:='node1,node2,node3';
sql_state_temp11:='(t.node1-s.node3)*(t.node2-s.node3)';
while (stop=0 and count_var<round_trip) loop
	j:=i+1;
	k:=i-1;
	sql_state_temp1:=sql_state_temp1||'t.node'||i||',';
	sql_state_temp11:=sql_state_temp11||'*(t.node'||i||'-s.node3)';
	sql_state_temp2:=sql_state_temp2||',node'||j;
	execute immediate 'insert into '||tabname||'(pathid,path_weight,path_type,pathused, '||sql_state_temp2|| ') select '||seq_name||'.nextval, t.path_weight+s.link_weight,case when l.link_nbr < f.link_nbr then s.lktype else t.path_type end,t.pathused*s.lkused, '||sql_state_temp1||' s.node3 from '||tabname||' t, '||linktab||' s , '||sysname||'link_type l,'||sysname||'link_type f where l.linktypeid=t.path_type and f.linktypeid=s.lktype and t.node'||i||'=s.node2 and t.node'||k||'=s.node1  and mod(f.link_nbr,l.link_nbr)*mod(l.link_nbr,f.link_nbr)=0 and t.path_weight=0 and '||sql_state_temp11||'!=0' ;
	execute immediate 'delete from '||tabname||' where path_weight=0 and node'||j||'=0';
	if(rt_needed=1) then
		execute immediate 'insert  into '||tabname1||'  select pathid, pathused,node1,node2,node'||i||',node'||j||',path_type,'||i||' from '||tabname||' where path_weight=2 and node'||j||'!=0 and (node'||i||',node'||j||') not in (select nodeid_end0,nodeid_end1 from '||tabname1||' )';
	else
		execute immediate 'insert  into '||tabname1||'  select pathid, pathused,node1,node2,node'||i||',node'||j||',path_type,'||i||' from '||tabname||' where path_weight=2 and node'||j||'!=0 ';
	end if;
	execute immediate 'select count(pathid ) from '||tabname||' where path_weight=0 and rownum=1' into pfrom;
	if pfrom=0 then
		stop:=1;
	else
		i:=i+1;
		j:=i+1;
		end if;
	count_var:=count_var+1;
end loop;
--DBMS_OUTPUT.PUT_LINE ('time execution for finding all the path '||count_var);

--UpdateSequence(seq_name);
res_create:=0;
--DBMS_OUTPUT.PUT_LINE ('time execution for finding all the path '|| (DBMS_UTILITY.GET_TIME-time_before)/100);
return res_create;
exception
when NO_DATA_FOUND then
res_create:=-1;
if seq_created=1 then
	UpdateSequence(seq_name);
end if;
return res_create;
when OTHERS then
res_create:=sqlcode;
v_error_message:=sqlerrm;
if seq_created=1 then
	UpdateSequence(seq_name);
end if;
return res_create;	
end createPathTab;
-----------------------------------------------------------
-- mapping of the paths with the ethernetadd
-----------------------------------------------------------
function createRoutingTable_SP(sysname varchar2,devfrom number) return number is
v_error_message varchar2(500);
res_create number;
nb_lkid number;
exist_tab number:=0;
dist_lkid number;
tabname1 varchar2(100);
lkid number:=0;
routing_name varchar2(100);
routing_name_copy varchar2(100);
node_name varchar2(500):=sysname||'logical_links';
begin
tabname1:=sysname||'startend_temporary';
routing_name:=sysname||'routingtab_temporary';
routing_name_copy:=sysname||'routingtabcopy_temporary';
	
		
		
	execute immediate 'select count(nodeid_end1),count(distinct nodeid_end1||''|''||nodeid_end0) from '||tabname1||' ' into nb_lkid,dist_lkid;
	if  nb_lkid=dist_lkid then
		execute immediate 'insert  into '||routing_name_copy||' select  f.pathid,f.pathused,s.node_from,s.pfrom, e.node_to,e.pto,f.path_type from '||tabname1||'  f, '||node_name||' e, '||node_name||' s where e.node_from=f.nodeid_end0 and e.node_to=f.nodeid_end1 and s.node_from=f.nodeid_start0 and s.node_to=f.nodeid_start1';
		execute immediate 'insert  into '||routing_name||' select f.pid_tp,f.lk_pfrom,t.ethernet_add,k.ethernet_add from '||sysname||'ip_ethernet k,'||routing_name_copy||' f, '||sysname||'ip_ethernet t,'||sysname||'port_properties w, '||sysname||'port_properties z where k.portid=w.portid and t.portid=z.portid and w.deviceid=f.lk_node_from and w.port_nbr=f.lk_pfrom and z.deviceid=f.lk_node_to and z.port_nbr=f.lk_pto';
	else
		execute immediate 'insert  into '||routing_name_copy||' select f.pathid,f.pathused,s.node_from,s.pfrom, e.node_to,e.pto,f.path_type from '||tabname1||'  f, '||node_name||' e, '||node_name||' s where e.node_from=f.nodeid_end0 and e.node_to=f.nodeid_end1 and s.node_from=f.nodeid_start0 and s.node_to=f.nodeid_start1 and f.pathid=(select min(pathid) from '||tabname1||'  where f.nodeid_end1=nodeid_end1)';
		execute immediate 'insert  into '||routing_name||' select  f.pid_tp,f.lk_pfrom,t.ethernet_add,k.ethernet_add from '||sysname||'ip_ethernet k,'||routing_name_copy||' f, '||sysname||'ip_ethernet t,'||sysname||'port_properties w, '||sysname||'port_properties z where k.portid=w.portid and t.portid=z.portid and w.deviceid=f.lk_node_from and w.port_nbr=f.lk_pfrom and z.deviceid=f.lk_node_to and z.port_nbr=f.lk_pto';
	end if;
	return 0;

exception
when NO_DATA_FOUND then
return -1;
when OTHERS then
res_create:=sqlcode;
v_error_message:='Pb: '||sqlerrm;
return sqlcode;
end createRoutingTable_SP;
-----------------------------------------------------------
--same as createRT_all except it generates destination tables case where there is no rt and we don't select the shortest path
---------------------------------------------------------------
function createTD_all_devname(sysname varchar2,devname varchar2, path_necessary number default 0, round_trip number default 10) return number is 
v_error_message varchar2(1000);
res_query number:=0;
res_query1 number:=0;
node_type number:=0;
devid number:=0;
time_before number;
time_after number;

exist_tab  number;
connectivitytab varchar2(60):=sysname||'connectivity';
--connectivitytab varchar2(50);
node_table varchar2(60):=sysname||'logical_links';
maxval date;
minval date;

begin
time_before := DBMS_UTILITY.GET_TIME;
--check if the logical table is up to date
execute immediate 'select max(last_update) from '||connectivitytab into maxval;
execute immediate 'select min(last_update) from '||node_table into minval;
if(maxval>minval) then --need to update the logical
	res_query:=routingtable_pck.createNodeLink_tab(sysname);
end if;	

execute immediate 'select deviceid, node from '||sysname||'device where upper(devicename)=:1' into devid,node_type using upper(devname);
if(node_type=0) then
	res_query:=routingtable_pck.createPathTab(sysname,devid,0,round_trip);
else
	res_query:=routingtable_pck.createPathTab_host(sysname,devid,0,round_trip);
end if;
if(res_query=0) then
	res_query1:=routingtable_pck.createTabDestin(sysname ,devid);
else
	return res_query;
end if;
if(res_query1=0) then
	res_query:=routingtable_pck.Insert_firsttab(sysname ,devid,0,path_necessary );
end if;
commit;
--DBMS_OUTPUT.PUT_LINE ('time execution to create the destination tables '|| (DBMS_UTILITY.GET_TIME-time_before)/100);
return res_query1;
exception
when NO_DATA_FOUND then
commit;
return sqlcode;
when OTHERS then
res_query:=sqlcode;
v_error_message:=sqlerrm;
commit;
return sqlcode;
end createTD_all_devname;
-----------------------------------------------------------
--same as createTD_all except we pass the devicename instead of the deviceid
---------------------------------------------------------------
function createTD_all(sysname varchar2,devname number,path_necessary number default 0,round_trip number default 10) return number is 
v_error_message varchar2(1000);
res_query number:=0;
res_query1 number:=0;
node_type number:=0;
time_before number;
time_after number;
exist_tab  number;
connectivitytab varchar2(60):=sysname||'connectivity';
--connectivitytab varchar2(50);
node_table varchar2(60):=sysname||'logical_links';
maxval date;
minval date;

begin
time_before := DBMS_UTILITY.GET_TIME;
--check if the logical table is up to date
execute immediate 'select max(last_update) from '||connectivitytab into maxval;
execute immediate 'select min(last_update) from '||node_table into minval;
if(maxval>minval) then --need to update the logical
	res_query:=routingtable_pck.createNodeLink_tab(sysname);
end if;	
	
execute immediate 'select node from '||sysname||'device where deviceid=:1' into node_type using devname;
--DBMS_OUTPUT.PUT_LINE ('node_type '|| node_type||'devname '|| devname);

if(node_type=0) then
	res_query:=routingtable_pck.createPathTab(sysname,devname,0,round_trip);
else
	res_query:=routingtable_pck.createPathTab_host(sysname,devname,0,round_trip);
end if;
--DBMS_OUTPUT.PUT_LINE ('Sqlrowcount 1'|| SQL%ROWCOUNT);
if(res_query=0) then
	res_query1:=routingtable_pck.createTabDestin(sysname ,devname);
else
	return res_query;
end if;
--DBMS_OUTPUT.PUT_LINE ('Sqlrowcount 2'|| SQL%ROWCOUNT ||'pathnecessary= '||path_necessary);
if(res_query1=0) then
res_query:=routingtable_pck.Insert_firsttab(sysname ,devname,0,path_necessary );
--DBMS_OUTPUT.PUT_LINE ('Sqlrowcount 3'|| SQL%ROWCOUNT);
end if;
commit;
--DBMS_OUTPUT.PUT_LINE ('time execution to create the destination tables '|| (DBMS_UTILITY.GET_TIME-time_before)/100);
return res_query;
exception
when NO_DATA_FOUND then
commit;
return sqlcode;
when OTHERS then
res_query:=sqlcode;
v_error_message:=sqlerrm;
commit;
return sqlcode;
end createTD_all;

-----------------------------------------------------------
--mapping with the destination name
---------------------------------------------------------------

function createTabDestin(sysname varchar2,devfrom number) return number is 
v_error_message varchar2(500);
exist_tab number:=0;
stop number:=0;
res_create number:=0;
--devfrom varchar2(100):=upper(devfrom1);
curr1_tabname varchar2(500):=sysname||'destintab_temporary';
curr2_tabname varchar2(500):=sysname||'startend_temporary';
curr3_tabname varchar2(500):=sysname||'logical_links';

begin

execute immediate 'insert into '||curr1_tabname||' select t.pathid, e.pfrom,k.devicename,g.pto from '||curr2_tabname||' t, '||curr3_tabname||' e,'||curr3_tabname||' g ,'||sysname||'device k where t.nodeid_start0=e.node_from and t.nodeid_start1=e.node_to and t.nodeid_end0=g.node_from and t.nodeid_end1=g.node_to and k.deviceid=g.node_to and t.pathid=(select min(pathid) from '||curr2_tabname||' where nodeid_end1=t.nodeid_end1 and nodeid_end0=t.nodeid_end0 and nodeid_start0=t.nodeid_start0 and nodeid_start1=t.nodeid_start1)';

return 0;
exception
when NO_DATA_FOUND then
return sqlcode;
when OTHERS then
v_error_message:=sqlerrm;
return sqlcode;
end createTabDestin;
-----------------------------------------------------------
function CheckExistenceSeq(seqname varchar2) return number is
res_query number:=0;
begin
	select count(sequence_name) into res_query from user_sequences where sequence_name=upper(seqname);
	return res_query;
exception
when NO_DATA_FOUND then
res_query:=0;
return res_query;
when OTHERS then
return res_query;
end CheckExistenceSeq;
-----------------------------------------------------------
function UpdatePathUsed1(systemname varchar2) return number is
res_query number:=0;
update_pathused date:=sysdate;
begin
	execute immediate 'update '||systemname||'logical_links e set e.last_update=:date_up, e.lkused=(select t.lkused from '||systemname||'connectivity t where t.linkid=abs(e.linkid))' using update_pathused;
	execute immediate 'update  '||systemname||'linkpairs s set s.last_update=:date_up, s.lkused=(select decode(t.lkused+e.lkused,2,1,0) from  '||systemname||'logical_links t, '||systemname||'logical_links e where s.node1=e.node_from and s.node2=e.node_to and e.node_to=t.node_from and s.node3=t.node_to) 'using update_pathused;
	
	execute immediate 'update  '||systemname||'pathdetails l set l.last_update=:date_up, l.pathused=(select p.lkused from '||systemname||'logical_links p where l.node1=p.node_from and l.node2=p.node_to and l.node3=0)' using update_pathused;
	execute immediate 'update  '||systemname||'pathdetails l set l.last_update=:date_up,l.pathused=(select decode(p.lkused+e.lkused,2,1,0) from '||systemname||'logical_links p,'||systemname||'logical_links e where l.node1=p.node_from and l.node2=p.node_to and l.node2=e.node_from and l.node3=e.node_to and l.node4=0)' using update_pathused;
	execute immediate 'update  '||systemname||'pathdetails l set l.last_update=:date_up,l.pathused=(select decode(p.lkused+e.lkused+f.lkused,3,1,0) from '||systemname||'logical_links p,'||systemname||'logical_links e,'||systemname||'logical_links f where l.node1=p.node_from and l.node2=p.node_to and l.node2=e.node_from and l.node3=e.node_to and l.node3=f.node_from and l.node4=f.node_to and l.node5=0)' using update_pathused;
	execute immediate 'update  '||systemname||'pathdetails l set l.last_update=:date_up,l.pathused=(select decode(p.lkused+e.lkused+f.lkused+g.lkused,4,1,0) from '||systemname||'logical_links p,'||systemname||'logical_links e,'||systemname||'logical_links f,'||systemname||'logical_links g where l.node1=p.node_from and l.node2=p.node_to and l.node2=e.node_from and l.node3=e.node_to and l.node3=f.node_from and l.node4=f.node_to and l.node4=g.node_from and l.node5=g.node_to and l.node6=0)' using update_pathused;
	execute immediate 'update  '||systemname||'pathdetails l set l.last_update=:date_up,l.pathused=(select decode(p.lkused+e.lkused+f.lkused+g.lkused+h.lkused,5,1,0) from '||systemname||'logical_links p,'||systemname||'logical_links e,'||systemname||'logical_links f,'||systemname||'logical_links g , '||systemname||'logical_links h where l.node1=p.node_from and l.node2=p.node_to and l.node2=e.node_from and l.node3=e.node_to and l.node3=f.node_from and l.node4=f.node_to and l.node4=g.node_from and l.node5=g.node_to and l.node5=h.node_from and h.node_to=l.node6 and l.node7=0)' using update_pathused;
	execute immediate 'update  '||systemname||'pathdetails l set l.last_update=:date_up,l.pathused=(select decode(p.lkused+e.lkused+f.lkused+g.lkused+h.lkused+j.lkused,6,1,0)  from '||systemname||'logical_links p,'||systemname||'logical_links e,'||systemname||'logical_links f,'||systemname||'logical_links g, '||systemname||'logical_links h, '||systemname||'logical_links j where l.node1=p.node_from and l.node2=p.node_to and l.node2=e.node_from and l.node3=e.node_to and l.node3=f.node_from and l.node4=f.node_to and l.node4=g.node_from and l.node5=g.node_to and l.node5=h.node_from and h.node_to=l.node6 and j.node_from=l.node6 and j.node_to=l.node7 and l.node8=0)' using update_pathused;
	execute immediate 'update  '||systemname||'pathdetails l set l.last_update=:date_up,l.pathused=(select decode(p.lkused+e.lkused+f.lkused+g.lkused+h.lkused+j.lkused+k.lkused,7,1,0) from '||systemname||'logical_links p,'||systemname||'logical_links e,'||systemname||'logical_links f,'||systemname||'logical_links g, '||systemname||'logical_links h, '||systemname||'logical_links j, '||systemname||'logical_links k where l.node1=p.node_from and l.node2=p.node_to and l.node2=e.node_from and l.node3=e.node_to and l.node3=f.node_from and l.node4=f.node_to and l.node4=g.node_from and l.node5=g.node_to and l.node5=h.node_from and h.node_to=l.node6 and j.node_from=l.node6 and j.node_to=l.node7 and k.node_from=l.node7 and l.node8=k.node_to and l.node9=0)' using update_pathused;
	execute immediate 'update  '||systemname||'pathdetails l set l.last_update=:date_up,l.pathused=(select  decode(p.lkused+e.lkused+f.lkused+g.lkused+h.lkused+j.lkused+k.lkused+m.lkused,8,1,0) from '||systemname||'logical_links p,'||systemname||'logical_links e,'||systemname||'logical_links f,'||systemname||'logical_links g, '||systemname||'logical_links h, '||systemname||'logical_links j, '||systemname||'logical_links k,'||systemname||'logical_links m where l.node1=p.node_from and l.node2=p.node_to and l.node2=e.node_from and l.node3=e.node_to and l.node3=f.node_from and l.node4=f.node_to and l.node4=g.node_from and l.node5=g.node_to and l.node5=h.node_from and h.node_to=l.node6 and j.node_from=l.node6 and j.node_to=l.node7 and k.node_from=l.node7 and l.node8=k.node_to and l.node8=m.node_from and l.node9=m.node_to and l.node10=0)' using update_pathused;
	execute immediate 'update  '||systemname||'pathdetails l set l.last_update=:date_up,l.pathused=(select decode(p.lkused+e.lkused+f.lkused+g.lkused+h.lkused+j.lkused+k.lkused+m.lkused+n.lkused,9,1,0) from '||systemname||'logical_links p,'||systemname||'logical_links e,'||systemname||'logical_links f,'||systemname||'logical_links g, '||systemname||'logical_links h, '||systemname||'logical_links j, '||systemname||'logical_links k,'||systemname||'logical_links m,'||systemname||'logical_links n where l.node1=p.node_from and l.node2=p.node_to and l.node2=e.node_from and l.node3=e.node_to and l.node3=f.node_from and l.node4=f.node_to and l.node4=g.node_from and l.node5=g.node_to and l.node5=h.node_from and h.node_to=l.node6 and j.node_from=l.node6 and j.node_to=l.node7 and k.node_from=l.node7 and l.node8=k.node_to and l.node8=m.node_from and l.node9=m.node_to and l.node9=n.node_from and l.node10=n.node_to and l.node11=0)' using update_pathused;
	execute immediate 'update  '||systemname||'pathdetails l set l.last_update=:date_up,l.pathused=(select decode(p.lkused+e.lkused+f.lkused+g.lkused+h.lkused+j.lkused+k.lkused+m.lkused+n.lkused+s.lkused,10,1,0) from '||systemname||'logical_links p,'||systemname||'logical_links e,'||systemname||'logical_links f,'||systemname||'logical_links g, '||systemname||'logical_links h, '||systemname||'logical_links j, '||systemname||'logical_links k,'||systemname||'logical_links m, '||systemname||'logical_links n, '||systemname||'logical_links s where l.node1=p.node_from and l.node2=p.node_to and l.node2=e.node_from and l.node3=e.node_to and l.node3=f.node_from and l.node4=f.node_to and l.node4=g.node_from and l.node5=g.node_to and l.node5=h.node_from and h.node_to=l.node6 and j.node_from=l.node6 and j.node_to=l.node7 and k.node_from=l.node7 and l.node8=k.node_to and l.node8=m.node_from and l.node9=m.node_to and l.node9=n.node_from and l.node10=n.node_to and l.node10=s.node_from and l.node11=s.node_to and l.node10!=0)' using update_pathused;
	
	execute immediate 'update  '||systemname||'destinationtable r set r.last_update=:date_up,r.pathused=(select pathused from '||systemname||'pathdetails p where p.pathid=r.pathid and p.node1=r.nodeid_start0)' using update_pathused;
	execute immediate 'update  '||systemname||'routingtabletable r set r.last_update=:date_up,r.pathused=(select pathused from '||systemname||'pathdetails p where p.pathid=r.pathid and p.node1=r.nodei_dstart0)' using update_pathused;

	return res_query;
exception
when NO_DATA_FOUND then
res_query:=-1;
return res_query;
when OTHERS then
res_query:=sqlcode;
return res_query;
end UpdatePathUsed1;
-----------------------------------------------------------
-----------------------------------------------------------
function UpdatePathUsed(systemname varchar2) return number is
res_query number:=0;
update_pathused date:=sysdate;
begin
	execute immediate 'update '||systemname||'logical_links e set e.last_update=:date_up, e.lkused=(select t.lkused from '||systemname||'connectivity t where t.linkid=abs(e.linkid))' using update_pathused;
	execute immediate 'update  '||systemname||'linkpairs s set s.last_update=:date_up, s.lkused=(select decode(t.lkused+e.lkused,2,1,0) from  '||systemname||'logical_links t, '||systemname||'logical_links e where s.node1=e.node_from and s.node2=e.node_to and e.node_to=t.node_from and s.node3=t.node_to) 'using update_pathused;
	
	execute immediate 'update  '||systemname||'pathdetails l set l.last_update=:date_up, l.pathused=(select p.lkused from '||systemname||'logical_links p where l.node1=p.node_from and l.node2=p.node_to and l.node2!=0) where l.node2!=0' using update_pathused;
	
	execute immediate 'update  '||systemname||'pathdetails l set l.pathused=(select decode(l.pathused+e.lkused,2,1,0) from '||systemname||'logical_links e where l.node2=e.node_from and l.node3=e.node_to and l.node3!=0) where l.node3!=0';
	
	execute immediate 'update  '||systemname||'pathdetails l set l.pathused=(select decode(l.pathused+f.lkused,2,1,0) from '||systemname||'logical_links f where  l.node3=f.node_from and l.node4=f.node_to and l.node4!=0) where l.node4!=0';
	
	
	execute immediate 'update  '||systemname||'pathdetails l set l.pathused=(select decode(l.pathused+g.lkused,2,1,0) from '||systemname||'logical_links g where  l.node4=g.node_from and l.node5=g.node_to and l.node5!=0) where l.node5!=0 ' ;
	execute immediate 'update  '||systemname||'pathdetails l set l.pathused=(select decode(l.pathused+h.lkused,2,1,0) from  '||systemname||'logical_links h where  l.node5=h.node_from and h.node_to=l.node6 and l.node6!=0) where l.node6!=0' ;
	execute immediate 'update  '||systemname||'pathdetails l set l.pathused=(select decode(l.pathused+j.lkused,2,1,0)  from   '||systemname||'logical_links j where  j.node_from=l.node6 and j.node_to=l.node7 and l.node7!=0) where l.node7!=0' ;
	
	execute immediate 'update  '||systemname||'pathdetails l set l.pathused=(select decode(l.pathused+k.lkused,2,1,0) from   '||systemname||'logical_links k where   k.node_from=l.node7 and l.node8=k.node_to and l.node8!=0) where l.node8!=0' ;
	execute immediate 'update  '||systemname||'pathdetails l set l.pathused=(select  decode(l.pathused+m.lkused,2,1,0) from   '||systemname||'logical_links m where  l.node8=m.node_from and l.node9=m.node_to and l.node9!=0) where l.node9!=0' ;
	execute immediate 'update  '||systemname||'pathdetails l set l.pathused=(select decode(l.pathused+n.lkused,2,1,0) from  '||systemname||'logical_links n where     l.node9=n.node_from and l.node10=n.node_to and l.node10!=0) where l.node10!=0';
	execute immediate 'update  '||systemname||'pathdetails l set l.pathused=(select decode(l.pathused+s.lkused,2,1,0) from  '||systemname||'logical_links s where    l.node10=s.node_from and l.node11=s.node_to and l.node10!=0) where l.node10!=0';
	execute immediate 'update  '||systemname||'destinationtable r set r.last_update=:date_up,r.pathused=(select p.pathused from '||systemname||'pathdetails p where p.pathid=r.pathid and p.node1=r.nodeid_start0)' using update_pathused;
	execute immediate 'update  '||systemname||'routingtable r set r.last_update=:date_up,r.pathused=(select p.pathused from '||systemname||'pathdetails p where p.pathid=r.pathid and p.node1=r.nodeid_start0)' using update_pathused;

	return res_query;
exception
when NO_DATA_FOUND then
res_query:=-1;
return res_query;
when OTHERS then
res_query:=sqlcode;
return res_query;
end UpdatePathUsed;
-----------------------------------------------------------
function CheckExistenceTabTemp(tabname varchar2) return number is
res_query number:=0;
begin
	select count(table_name) into res_query from user_tables where table_name=upper(tabname);
	return res_query;
exception
when NO_DATA_FOUND then
res_query:=0;
return res_query;
when OTHERS then
res_query:=sqlcode;
return res_query;
end CheckExistenceTabTemp;
-----------------------------------------------------------
--- find valid path if the deviceid is a host node
-----------------------------------------------------------
function CreatePathTab_Host(sysname varchar2, devfrom number, rt_needed number, round_trip number default 10) return number is
v_error_message varchar2(500);
sql_statement varchar2(4000);
sql_state_temp1 long;
sql_state_temp11 long;
sql_state_temp2 varchar2(4000);
sql_var varchar2(4000);
stop number:=0;
i number:=1;
j number:=1;
l number :=0;
k number:=0;
count_var number:=3;
exist_tab number:=0;
pfrom number:=0;
pto number:=0;
seq_name varchar2(100);
res_create number:=0;
seq_created number:=0;
path_copy_created number:=0;
path_copy1_created number:=0;
res_temp number:=1;
time_before number;
time_after number;
tabname varchar2(100):=sysname||'pathdetails_temporary';
tabname1 varchar2(100);
nodetab varchar2(100):=sysname||'logical_links';
linktab varchar2(100):=sysname||'linkpairs';
linktype varchar2(100):=sysname||'link_type';

begin
time_before := DBMS_UTILITY.GET_TIME;
tabname1:=sysname||'startend_temporary';
seq_name:=sysname||'sp_'||devfrom;
DBMS_OUTPUT.ENABLE( 10000000);
l:=routingtable_pck.CheckExistenceSeq(seq_name);

	if(l=1) then
		UpdateSequence(seq_name);
	else
		execute immediate 'create sequence '||seq_name||' start with 0 increment by 1 MINVALUE 0';
		execute immediate 'select '||seq_name||'.nextval from dual' into seq_created;
	end if;
	seq_created:=1;
	
	execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused,node1,node2) select  '||seq_name||'.nextval ,link_weight,lktype,lkused,node_from,node_to from '||nodetab||' where node_from=:dfrom and link_weight=3' using devfrom;
	execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused,node1,node2,node3) select '||seq_name||'.nextval ,f.link_weight+t.link_weight,f.lktype,f.lkused*t.lkused,t.node_from,t.node_to,f.node_to from '||nodetab||' t,'||nodetab||' f ,'||linktype||' l,'||linktype||' m where l.linktypeid=f.lktype and m.linktypeid=t.lktype and mod(l.link_nbr,m.link_nbr)*mod(m.link_nbr,l.link_nbr)=0 and t.node_from=:dfrom and t.node_to=f.node_from and f.node_to!=t.node_from' using devfrom;	
	i:=3;


		
	execute immediate 'insert into '||tabname1||' select pathid, pathused,node1,node2,node1,node2,path_type,1 from '||tabname||' where path_weight=3 and node3=0';
	execute immediate 'insert into '||tabname1||' select pathid, pathused,node1,node2,node2,node3,path_type,2 from '||tabname||' where path_weight=3 and node3!=0';

	sql_state_temp1:='t.node1,t.node2,';
	sql_state_temp2:='node1,node2,node3';
	sql_state_temp11:='(t.node1-s.node3)*(t.node2-s.node3)';
	while (stop=0 and count_var<round_trip) loop
		j:=i+1;
		k:=i-1;
		sql_state_temp1:=sql_state_temp1||'t.node'||i||',';	
		sql_state_temp11:=sql_state_temp11||'*(t.node'||i||'-s.node3)';
		sql_state_temp2:=sql_state_temp2||',node'||j;
		execute immediate 'insert  into '||tabname||'(pathid,path_weight,path_type,pathused, '||sql_state_temp2|| ') select '||seq_name||'.nextval, t.path_weight+s.link_weight,case when l.link_nbr < f.link_nbr then s.lktype else t.path_type  end,t.pathused*s.lkused, '||sql_state_temp1||' s.node3 from '||tabname||' t, '||linktab||' s ,'||sysname||'link_type f,'||sysname||'link_type l where f.linktypeid=t.path_type and l.linktypeid=s.lktype and t.node'||i||'=s.node2 and t.node'||k||'=s.node1  and mod(f.link_nbr,l.link_nbr)*mod(l.link_nbr,f.link_nbr)=0 and t.path_weight=1 and '||sql_state_temp11||'!=0' ;
		execute immediate 'delete from '||tabname||' where path_weight=1 and node'||j||'=0';
		if(rt_needed=1) then
			execute immediate 'insert  into '||tabname1||'  select pathid, pathused,node1,node2,node'||i||',node'||j||',path_type,'||i||' from '||tabname||' where path_weight=3 and node'||j||'!=0 and (node'||i||',node'||j||') not in (select nodeid_end0,nodeid_end1 from '||tabname1||' )';
		else
			execute immediate 'insert  into '||tabname1||'  select pathid, pathused,node1,node2,node'||i||',node'||j||',path_type,'||i||' from '||tabname||' where path_weight=3 and node'||j||'!=0 ';
		end if;
		execute immediate 'select count(pathid ) from '||tabname||' where path_weight!=3 and rownum=1' into pfrom;
		if pfrom=0 then
			stop:=1;
		else
			i:=i+1;
			j:=i+1;
		end if;
		count_var:=count_var+1;
	end loop;
res_create:=0;
DBMS_OUTPUT.PUT_LINE ('count_var '|| count_var);
return res_create;
exception
when NO_DATA_FOUND then
res_create:=-1;
if seq_created=1 then
	UpdateSequence(seq_name);
end if;
return res_create;
when OTHERS then
res_create:=sqlcode;
v_error_message:=sqlerrm;
if seq_created=1 then
	UpdateSequence(seq_name);
end if;
return res_create;	
end CreatePathTab_Host;
-----------------------------------------------------------
--- find valid path  without using the linkpairs table (interesting when the linkpairs table is too big)
-----------------------------------------------------------

-------------------------------------------------------------------------------------------------------------------------
function createPathTab_nolkpairs(sysname varchar2, devfrom number,rt_needed number, round_trip number default 10) return number is
v_error_message varchar2(500);
sql_statement varchar2(4000);
sql_state_temp1 long;
sql_var varchar2(4000);
sql_state_temp11 long;
sql_state_temp2 varchar2(4000);
stop number:=0;
i number:=1;
j number:=1;
l number :=0;
k number:=0;

count_var number:=3;
exist_tab number:=0;
pfrom number:=0;
pto number:=0;
seq_name varchar2(100);
res_create number:=0;
seq_created number:=0;
path_copy_created number:=0;
path_copy1_created number:=0;
res_temp number:=1;
time_before number;
time_after number;
tabname varchar2(100):=sysname||'pathdetails_temporary';
tabname1 varchar2(100);
nodetab varchar2(100):=sysname||'logical_links';
linktype varchar2(100):=sysname||'link_type';

begin
time_before := DBMS_UTILITY.GET_TIME;
tabname1:=sysname||'startend_temporary';

seq_name:=sysname||'sp_'||devfrom;
DBMS_OUTPUT.ENABLE( 10000000);

l:=routingtable_pck.CheckExistenceSeq(seq_name);


	if(l=1) then
		UpdateSequence(seq_name);
	else
		execute immediate 'create sequence '||seq_name||' start with 0 increment by 1 MINVALUE 0';
		execute immediate 'select '||seq_name||'.nextval from dual' into seq_created;
	end if;
	seq_created:=1;

execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused , node1,node2) select '||seq_name||'.nextval ,link_weight,lktype,lkused,node_from,node_to from '||nodetab||' where node_from=:dfrom and link_weight=2' using devfrom;
execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused , node1,node2,node3) select '||seq_name||'.nextval ,t.link_weight+f.link_weight, f.lktype ,t.lkused*f.lkused,t.node_from,t.node_to,f.node_to from '||nodetab||' t, '||nodetab||' f ,'||linktype||' l, '||linktype||' m where l.linktypeid=f.lktype and m.linktypeid=t.lktype and mod(l.link_nbr,m.link_nbr)*mod(m.link_nbr,l.link_nbr) and  t.node_from=:dfrom and t.node_to=f.node_from  and f.node_to!=t.node_from and f.link_weight!=1' using devfrom;

i:=3;
select count(table_name) into exist_tab from user_tables where table_name=upper(tabname1);
execute immediate 'insert into '||tabname1||' select  pathid, pathused,node1,node2,node1,node2,path_type,1 from '||tabname||' where path_weight=2 and node3=0';
execute immediate 'insert into '||tabname1||' select  pathid, pathused,node1,node2,node2,node3,path_type,2 from '||tabname||' where path_weight=2 and node3!=0';


sql_state_temp1:='t.node1,t.node2,';
sql_state_temp2:='node1,node2,node3';
sql_state_temp11:='(t.node1-s.node_to)*(t.node2-s.node_to)';
while (stop=0 and count_var<round_trip) loop
	j:=i+1;
	k:=i-1;
	sql_state_temp1:=sql_state_temp1||'t.node'||i||',';
	sql_state_temp11:=sql_state_temp11||'*(t.node'||i||'-s.node_to)';
	sql_state_temp2:=sql_state_temp2||',node'||j;
	execute immediate 'insert into '||tabname||'(pathid,path_weight,path_type,pathused, '||sql_state_temp2|| ') select '||seq_name||'.nextval, t.path_weight+s.link_weight,case when l.link_nbr > f.link_nbr then s.lktype else t.path_type end,t.pathused*s.lkused, '||sql_state_temp1||' s.node_to from '||tabname||' t, '||nodetab||' s, '||sysname||'link_type l,'||sysname||'link_type f where f.linktypeid=t.path_type and l.linktypeid=s.lktype and t.node'||i||'=s.node_from  and mod(l.link_nbr,f.link_nbr)*mod(f.link_nbr,l.link_nbr)=0 and t.path_weight=0 and '||sql_state_temp11||'!=0' ;
	execute immediate 'delete from '||tabname||' where path_weight=0 and node'||j||'=0';
	if(rt_needed=1) then
		execute immediate 'insert  into '||tabname1||'  select pathid,pathused, node1,node2,node'||i||',node'||j||',path_type,'||i||' from '||tabname||' where path_weight=2 and node'||j||'!=0 and (node'||i||',node'||j||') not in (select nodeid_end0,nodeid_end1 from '||tabname1||' )';
	else
		execute immediate 'insert  into '||tabname1||'  select pathid,pathused, node1,node2,node'||i||',node'||j||',path_type,'||i||' from '||tabname||' where path_weight=2 and node'||j||'!=0 ';
	end if;
	execute immediate 'select count(pathid ) from '||tabname||' where path_weight=0 and rownum=1' into pfrom;
	if pfrom=0 then
		stop:=1;
	else
		i:=i+1;
		j:=i+1;
		end if;
	count_var:=count_var+1;
end loop;
res_create:=0;

--DBMS_OUTPUT.PUT_LINE ('time execution for finding all the path '|| (DBMS_UTILITY.GET_TIME-time_before)/100);
return res_create;
exception
when NO_DATA_FOUND then
res_create:=-1;
if seq_created=1 then
	UpdateSequence(seq_name);
end if;
return res_create;
when OTHERS then
res_create:=sqlcode;
v_error_message:=sqlerrm;
if seq_created=1 then
	UpdateSequence(seq_name);
end if;
return res_create;	
end createPathTab_nolkpairs;
-----------------------------------------------------------
--- find valid path  for host nodes without using the linkpairs table (interesting when the linkpairs table is too big)
-----------------------------------------------------------


------------------------------------------------------------------------------------------------------------------------------------
function CreatePathTab_Host_nolkpairs(sysname varchar2, devfrom number, rt_needed number, round_trip number default 10) return number is
v_error_message varchar2(500);
sql_statement varchar2(4000);
sql_state_temp1 long;
sql_state_temp11 long;
sql_state_temp2 varchar2(4000);
sql_var varchar2(4000);
stop number:=0;
i number:=1;
j number:=1;
l number :=0;
k number:=0;
count_var number:=3;
exist_tab number:=0;
pfrom number:=0;
pto number:=0;
seq_name varchar2(100);
res_create number:=0;
seq_created number:=0;
path_copy_created number:=0;
path_copy1_created number:=0;
res_temp number:=1;
time_before number;
time_after number;
tabname varchar2(100):=sysname||'pathdetails_temporary';
tabname1 varchar2(100);
nodetab varchar2(100):=sysname||'logical_links';
linktype varchar2(100):=sysname||'link_type';

begin
time_before := DBMS_UTILITY.GET_TIME;
tabname1:=sysname||'startend_temporary';
seq_name:=sysname||'sp_'||devfrom;
DBMS_OUTPUT.ENABLE( 10000000);
l:=routingtable_pck.CheckExistenceSeq(seq_name);
if(l=1) then
	UpdateSequence(seq_name);
else
	execute immediate 'create sequence '||seq_name||' start with 0 increment by 1 MINVALUE 0';
	execute immediate 'select '||seq_name||'.nextval from dual' into seq_created;
end if;
	seq_created:=1;

execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused , node1,node2) select '||seq_name||'.nextval ,link_weight,lktype,lkused,node_from,node_to from '||nodetab||' where node_from=:dfrom and link_weight=3' using devfrom;
execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused , node1,node2,node3) select '||seq_name||'.nextval ,t.link_weight+f.link_weight, f.lktype,t.lkused*f.lkused,t.node_from,t.node_to,f.node_to from '||nodetab||' t, '||nodetab||' f , '||linktype||' l,'||linktype||' m where l.linktypeid=f.lktype and t.lktype and m.linktypeid and mod(l.link_nbr,m.link_nbr)*mod(m.link_nbr,l.link_nbr)=0 and  t.node_from=:dfrom and t.node_to=f.node_from  and f.link_weight!=1 and t.node_from!=f.node_to' using devfrom;

i:=3;
	execute immediate 'insert into '||tabname1||' select pathid, pathused,node1,node2,node1,node2,path_type,1 from '||tabname||' where path_weight=3 and node3=0';
	execute immediate 'insert into '||tabname1||' select pathid, pathused,node1,node2,node2,node3,path_type,2 from '||tabname||' where path_weight=3 and node3!=0';

sql_state_temp1:='t.node1,t.node2,';
sql_state_temp2:='node1,node2,node3';
sql_state_temp11:='(t.node1-s.node_to)*(t.node2-s.node_to)';
while (stop=0 and count_var<round_trip) loop
	j:=i+1;
	k:=i-1;
	sql_state_temp1:=sql_state_temp1||'t.node'||i||',';
	sql_state_temp11:=sql_state_temp11||'*(t.node'||i||'-s.node_to)';
	sql_state_temp2:=sql_state_temp2||',node'||j;
	execute immediate 'insert into '||tabname||'(pathid,path_weight,path_type,pathused, '||sql_state_temp2|| ') select '||seq_name||'.nextval, t.path_weight+s.link_weight,case when f.link_nbr > l.link_nbr then s.lktype else t.path_type end,t.pathused*s.lkused, '||sql_state_temp1||' s.node_to from '||tabname||' t, '||nodetab||' s , '||sysname||'link_type l, '||sysname||'link_type f  where f.linktypeid=t.path_type and l.linktypeid=s.lktype  and t.node'||i||'=s.node_from  and mod(l.link_nbr,f.link_nbr)*mod(f.link_nbr,l.link_nbr)=0 and t.path_weight=1 and '||sql_state_temp11||'!=0' ;
	execute immediate 'delete from '||tabname||' where path_weight=0 and node'||j||'=0';
	if(rt_needed=1) then
		execute immediate 'insert  into '||tabname1||'  select pathid, pathused, node1,node2,node'||i||',node'||j||',path_type,'||i||' from '||tabname||' where path_weight=3 and node'||j||'!=0 and (node'||i||',node'||j||') not in (select nodeid_end0,nodeid_end1 from '||tabname1||' )';
	else
		execute immediate 'insert  into '||tabname1||'  select pathid, pathused, node1,node2,node'||i||',node'||j||',path_type,'||i||'  from '||tabname||' where path_weight=3 and node'||j||'!=0 ';
	end if;
	execute immediate 'select count(pathid ) from '||tabname||' where path_weight=1 and rownum=1' into pfrom;
	if pfrom=0 then
		stop:=1;
	else
		i:=i+1;
		j:=i+1;
		end if;
	count_var:=count_var+1;
end loop;
res_create:=0;

--DBMS_OUTPUT.PUT_LINE ('time execution for finding all the path '|| (DBMS_UTILITY.GET_TIME-time_before)/100);
return res_create;
exception
when NO_DATA_FOUND then
res_create:=-1;
if seq_created=1 then
	UpdateSequence(seq_name);
end if;
return res_create;
when OTHERS then
res_create:=sqlcode;
v_error_message:=sqlerrm;
if seq_created=1 then
	UpdateSequence(seq_name);
end if;
return res_create;	
end createPathTab_Host_nolkpairs;

end routingtable_pck;
/




