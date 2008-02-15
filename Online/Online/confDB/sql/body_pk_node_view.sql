---------------------------------------------
--PL/SQL routing and destination package
--author L.Abadie
--v3.0
------------------------------------
create or replace package body routingtable_pck
is
-----------------------------------------------------------
--Convention we always give sysname=sysname+'_': i.e. if it's VELO, we give VELO_
-----------------------------------------------------------
----------------------------------------------------------
----fct which creates the logical view of the system : we just keep link between devices (get rid of port nb)
----------------------------------------------------------


procedure createMicroNodeLink_tab(motherboardidList in out number ,recreate_table number, rescode in out number, cpntid in out number,cpntname varchar2 default 'none')  is
node_table varchar2(60):='lhcb_microlg_links';
--node_table varchar2(60):=sysname||'logical_links';

res_query number:=0;
exist_tab number:=0;
exist_tab1 number:=0;
exist_tab2 number:=0;
exist_tab3 number:=0;
v_error_message varchar2(4000);
conn_table varchar2(60):='lhcb_microscopic_connectivity';
link_table varchar2(60):='lhcb_microlinkpairs';
--link_table varchar2(60):=sysname||'linkpairs';
begin
	if motherboardidList=0 then
		execute immediate 'select motherboardid, cpntid  from lhcb_lg_cpnts where cpntname=:cpnt' into motherboardidList,cpntid using cpntname;
	else
		if motherboardidList>0 and cpntid=1 then --means we give a portid
			execute immediate 'select deviceid from lhcb_port_properties where portid=:pid ' into cpntid using motherboardidList;
			motherboardidList:=cpntid;
			cpntid:=0;
		else
		cpntid:=0;
		end if;
	end if;
	DBMS_OUTPUT.PUT_LINE ('motherboardidList'||motherboardidList);
	if recreate_table=1 then
		execute immediate 'truncate table '||node_table;
		execute immediate 'truncate table '||link_table;
	end if;
	execute immediate 'insert into lhcb_microlg_links (linkway,node_from,node_to, link_weight, lktype,lkused) select distinct 1,decode(t.cpntidfrom,-1,-t.portidfrom,t.cpntidfrom),decode(t.cpntidto,-1,-t.portidto,t.cpntidto),t.link_weight,t.link_type,t.lkused from lhcb_microscopic_connectivity t ,  lhcb_lg_cpnts e,  lhcb_lg_cpnts f where t.cpntidfrom=e.cpntid and t.cpntidto=f.cpntid and (mod(:sysIDlist,nvl(f.motherboardid,:sysIDlist))=0 or mod(:sysIDlist,e.motherboardid)=0)' using motherboardidList,motherboardidList,motherboardidList;
	
	if SQL%ROWCOUNT=0 then
		rescode:=-11;
	else
	--DBMS_OUTPUT.PUT_LINE ('first insert');
	execute immediate 'insert into lhcb_microlg_links (linkway,node_from,node_to, link_weight, lktype,lkused) select distinct -1,decode(t.cpntidto,-1,-t.portidto,t.cpntidto),decode(t.cpntidfrom,-1,-t.portidfrom,t.cpntidfrom),decode(t.link_weight,1,2,2,1,0),t.link_type,t.lkused from lhcb_microscopic_connectivity t ,lhcb_lg_cpnts e,  lhcb_lg_cpnts f  where t.cpntidfrom=e.cpntid and t.cpntidto=f.cpntid and t.bidirectional_link_used=1 and (mod(:sysIDlist,nvl(f.motherboardid,:sysIDlist))=0 or mod(:sysIDlist,e.motherboardid)=0)' using motherboardidList,motherboardidList,motherboardidList;
	--DBMS_OUTPUT.PUT_LINE ('second insert');

	insert into lhcb_microlinkpairs select t.link_weight+s.link_weight, t.node_from,t.node_to, s.node_to, case when l.link_nbr > f.link_nbr then t.lktype else s.lktype end, t.lkused*s.lkused from lhcb_microlg_links t, lhcb_microlg_links s ,lhcb_link_types l,lhcb_link_types f where t.link_weight=0   and t.node_to=s.node_from and t.node_from!=s.node_to and t.lktype=l.linktypeid and s.lktype=f.linktypeid and mod(f.link_nbr,l.link_nbr)*mod(l.link_nbr,f.link_nbr)=0;
	--DBMS_OUTPUT.PUT_LINE ('third insert');
	rescode:=0;
	end if;
exception
when NO_DATA_FOUND then
rescode:=-1;
when OTHERS then
v_error_message:=sqlerrm;
rescode:=sqlcode;
end createMicroNodeLink_tab;
----------------------------------------------------------
----fct which creates the logical view of the system : we just keep link between devices (get rid of port nb)
----------------------------------------------------------
function createNodeLink_tab(sysIDlist number,recreate_table number, bidi_needed number default 0) return number is
node_table varchar2(60):='lhcb_lg_links';
--node_table varchar2(60):=sysname||'logical_links';
time_before number;

res_query number:=0;
exist_tab number:=0;
exist_tab1 number:=0;
exist_tab2 number:=0;
exist_tab3 number:=0;
v_error_message varchar2(4000);
conn_table varchar2(60):='lhcb_macroscopic_connectivity';
port_table varchar2(60):='lhcb_port_properties';
link_table varchar2(60):='lhcb_linkpairs';
--link_table varchar2(60):=sysname||'linkpairs';

begin
	if recreate_table=1 then
		execute immediate 'truncate table '||node_table;
		execute immediate 'truncate table '||link_table;
	end if;
	time_before := DBMS_UTILITY.GET_TIME;

	execute immediate 'insert into lhcb_lg_links (linkway,node_from,node_to, link_weight, lktype,lkused,system_name,bidi_used) select distinct 1,f.deviceid,e.deviceid,t.link_weight,t.link_type,t.lkused, t.system_name,t.bidirectional_link_used from lhcb_macroscopic_connectivity t , lhcb_port_properties f ,lhcb_port_properties e where t.portidfrom=f.portid and t.portidto=e.portid and (mod(t.system_name,:sysIDlist)=0 or mod(:sysIDlist,t.system_name)=0) and t.lkused=(select max(h.lkused) from lhcb_macroscopic_connectivity h,lhcb_port_properties v,lhcb_port_properties k where h.portidfrom=v.portid  and h.portidto=k.portid and v.deviceid=f.deviceid and k.deviceid=e.deviceid)' using sysIDlist,sysIDlist;
	DBMS_OUTPUT.PUT_LINE ('time execution to create createnodetab first '|| (DBMS_UTILITY.GET_TIME-time_before)/100);	
	if(bidi_needed=0) then
			execute immediate 'insert into lhcb_lg_links (linkway,node_from,node_to, link_weight, lktype,lkused,system_name,bidi_used) select distinct -1,e.node_to,e.node_from,decode(e.link_weight,1,2,2,1,0),e.lktype,e.lkused, e.system_name, e.bidi_used from lhcb_lg_links e  where e.bidi_used=1' ;
			--DBMS_OUTPUT.PUT_LINE ('first insert sysid='||sysIDlist||' and SQLrowcount='||SQL%ROWCOUNT);
	else
			execute immediate 'insert into lhcb_lg_links (linkway,node_from,node_to, link_weight, lktype,lkused,system_name,bidi_used) select distinct -1,e.node_to,e.node_from,decode(e.link_weight,1,2,2,1,0),e.lktype,e.lkused, e.system_name,e.bidi_used from lhcb_lg_links e ' ;
	end if;
	DBMS_OUTPUT.PUT_LINE ('time execution to create createnodetab scd '|| (DBMS_UTILITY.GET_TIME-time_before)/100);	

	insert into lhcb_linkpairs select t.link_weight+s.link_weight, t.node_from,t.node_to, s.node_to, case when l.link_nbr > f.link_nbr then t.lktype else s.lktype end, t.lkused*s.lkused, t.system_name from lhcb_lg_links t, lhcb_lg_links s ,lhcb_link_types l,lhcb_link_types f where t.link_weight=0   and t.node_to=s.node_from and t.node_from!=s.node_to and t.lktype=l.linktypeid and s.lktype=f.linktypeid and mod(f.link_nbr,l.link_nbr)*mod(l.link_nbr,f.link_nbr)=0;
--	DBMS_OUTPUT.PUT_LINE ('scd insert sysid='||sysIDlist||' and SQLrowcount='||SQL%ROWCOUNT);

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
function createRT_all(devname number,path_necessary number default 0,round_trip number default 10,recreate_table number default 1) return number is
v_error_message varchar2(1000);
res_query number:=0;
res_query1 number:=1;
time_before number;
exist_tab number:=1;
sysname number:=1;
connectivitytab varchar2(60):='lhcb_macroscopic_connectivity';
--connectivitytab varchar2(50);
node_table varchar2(60):='lhcb_lg_links';
maxval date;
minval date;

begin
time_before := DBMS_UTILITY.GET_TIME;
execute immediate 'select system_name from lhcb_lg_devices where deviceid=:devid'into sysname using devname;
if(recreate_table=1) then
res_query:=routingtable_pck.createNodeLink_tab(sysname,recreate_table);
end if;
DBMS_OUTPUT.PUT_LINE ('time execution to create logicalink '|| (DBMS_UTILITY.GET_TIME-time_before)/100);

if(res_query=0) then
	res_query:=routingtable_pck.createPathTab(devname,1,round_trip);
else
	commit;
	return res_query;
end if;
--DBMS_OUTPUT.PUT_LINE ('time execution to create createpathtab '|| (DBMS_UTILITY.GET_TIME-time_before)/100);

if(res_query=0) then
	res_query1:=routingtable_pck.createRoutingTable_SP(devname);
else
	commit;
	return res_query;
end if;
--DBMS_OUTPUT.PUT_LINE ('time execution to create createRoutingTable_SP '|| (DBMS_UTILITY.GET_TIME-time_before)/100);

if(res_query1=0) then
	res_query:=routingtable_pck.Insert_firsttab(devname,1,path_necessary);
else
	commit;
	return res_query1;
end if;

commit;

--DBMS_OUTPUT.PUT_LINE ('time execution to create the routing table '|| (DBMS_UTILITY.GET_TIME-time_before)/100);
return res_query;
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
function Insert_firsttab(devname number, rt_needed number,path_necessary number) return number is
tabname varchar2(100):='lhcb_pdetails';
tabname_temp varchar2(100):='lhcb_pdet_temp';
tabname1 varchar2(100):='lhcb_routingtable';
tabname1_temp  varchar2(100):='lhcb_startend_temp';
tabname2_temp varchar2(100):='lhcb_routingtable_temp';
tabname3 varchar2(100):='lhcb_destinationtable';
tabname3_temp varchar2(100):='lhcb_destintab_temp';
ck_existence number:=0;
res_query number;
rescount number;
date_update date :=sysdate;
v_error_message varchar2(4000);
time_before number;
begin
time_before := DBMS_UTILITY.GET_TIME;

execute immediate 'delete  '||tabname1||' where nodeid_start0=:1' using devname;
execute immediate 'delete  '||tabname3||' where nodeid_start0=:1' using devname;
execute immediate 'delete  '||tabname||' where node1=:1' using devname;
--DBMS_OUTPUT.PUT_LINE ('time to delete old paths '|| (DBMS_UTILITY.GET_TIME-time_before)/100);

--	execute immediate 'create table lhcb_destinationtable(pathid number,pathused number,nodeid_start0 number,pfromid0 number,ptoid1,nodeid_end1,path_type number,system_name number,nodeid_end1 number,path_type number,system_name number, path_length number,last_update date, constraint lhcb_dest_name_pk primary key(nodeid_start0,pathid))';
--DBMS_OUTPUT.PUT_LINE ('after deletion ');
if(rt_needed=1) then
	if path_necessary=1 then
		insert into lhcb_pdetails select t.pathid ,t.path_weight,t.path_type ,t.pathused , t.node1 ,t.node2 ,t.node3 ,t.node4 ,t.node5,t.node6,t.node7,t.node8,t.node9,t.node10,f.nodeid_end1,sysdate,1,t.system_name from lhcb_pdet_temp t,  lhcb_startend_temp f where  t.pathid=f.pathid ;
		--DBMS_OUTPUT.PUT_LINE ('after deletion 2');
	else
		insert into lhcb_pdetails select t.pathid ,t.path_weight,t.path_type ,t.pathused , t.node1 ,t.node2 ,t.node3 ,t.node4 ,t.node5,t.node6,t.node7,t.node8,t.node9,t.node10,f.nodeid_end1,sysdate,0,t.system_name from lhcb_pdet_temp t,  lhcb_startend_temp f where  t.pathid=f.pathid ;

end if;
insert into lhcb_routingtable (pathid,pathused,ROUTINGPATHUSED,NODEID_START0,pfromid0,ptoid0,ptoid1,NODEID_END1,path_type,system_name,path_length,last_update) select t.pathid,t.pathused,s.routingpathused,t.nodeid_start0,s.pfromid0,s.ptoid0,s.ptoid1,t.nodeid_end1,t.path_type,t.system_name,t.path_length , sysdate  from lhcb_startend_temp t,lhcb_routingtable_temp s where t.pathid=s.pathid;
--DBMS_OUTPUT.PUT_LINE ('after deletion 3');
else
if path_necessary=1 then
insert into lhcb_pdetails select t.pathid ,t.path_weight,t.path_type ,t.pathused , t.node1 ,t.node2 ,t.node3 ,t.node4 ,t.node5,t.node6,t.node7,t.node8,t.node9,t.node10,f.nodeid_end1 , sysdate,1,t.system_name  from lhcb_pdet_temp t,  lhcb_destintab_temp f where  t.pathid=f.pathid ;
else
insert into lhcb_pdetails select t.pathid ,t.path_weight,t.path_type ,t.pathused , t.node1 ,t.node2 ,t.node3 ,t.node4 ,t.node5,t.node6,t.node7,t.node8,t.node9,t.node10,f.nodeid_end1 , sysdate,0,t.system_name  from lhcb_pdet_temp t,  lhcb_destintab_temp f where  t.pathid=f.pathid ;

end if;
insert into lhcb_destinationtable select t.pathid,t.pathused,t.nodeid_start0,s.pfromid0,s.ptoid1,t.nodeid_end1,t.path_type,t.system_name,t.path_length ,sysdate  from lhcb_startend_temp t,lhcb_destintab_temp s where t.pathid=s.pathid ;

end if;
return 0;
exception
when NO_DATA_FOUND then
res_query:=-1;
DBMS_OUTPUT.PUT_LINE ('NOT FOUND');
return res_query;
when OTHERS then
res_query:=sqlcode;
DBMS_OUTPUT.PUT_LINE ('Errr='||sqlerrm);
v_error_message:=sqlerrm;
return res_query;
end Insert_firsttab;
----------------------------------------------------------
--no longer used
----------------------------------------------------------

----------------------------------------------------------
----fct which creates or recreates the routing table of a device:
-- the routing table is stored in the sysname||routingtable
--- only one version of routing table can be stored (no versioning)
--requires the existence of a MAC table
-- you pass the devicename instead of the deviceid
----------------------------------------------------------
function createRT_all_devname(devname varchar2,path_necessary number default 0,round_trip number default 10,recreate_tab number default 1) return number is
v_error_message varchar2(1000);
res_query number:=0;
res_query1 number:=0;
time_before number;
devid number:=0;
node number:=0;
maxval date;
minval date;
sysname number:=1;
connectivitytab varchar2(60):='lhcb_macroscopic_connectivity';
node_table varchar2(60):='lhcb_lg_links';
exist_tab  number;
begin
--time_before := DBMS_UTILITY.GET_TIME;

execute immediate 'select system_name,deviceid,node from lhcb_lg_devices where devicename=:devid'into sysname,devid,node using devname;

res_query:=routingtable_pck.createNodeLink_tab(sysname,recreate_tab);

if(node=0) then
	res_query:=routingtable_pck.createPathTab(devid,1,round_trip);
else
	res_query:=routingtable_pck.createPathTab_Host(devid,1,round_trip);
end if;
if(res_query=0) then
	res_query1:=routingtable_pck.createRoutingTable_SP(devid);
else
 return res_query;
end if;
if(res_query1=0) then
	res_query:=routingtable_pck.Insert_firsttab(devid,1, path_necessary);
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
function createPathTab(devfrom number,rt_needed number, round_trip number default 10) return number is
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
tabname varchar2(100):='lhcb_pdet_temp';
nodetab varchar2(100):='lhcb_lg_links';
linktab varchar2(100):='lhcb_linkpairs';
line_counter number:=1;
line_counter_bis number:=0;
begin
time_before := DBMS_UTILITY.GET_TIME;
tabname1:='lhcb_startend_temp';
--seq_name:='lhcb_sp_'||devfrom;
DBMS_OUTPUT.ENABLE( 10000000);

--l:=routingtable_pck.CheckExistenceSeq(seq_name);
--DBMS_OUTPUT.PUT_LINE ('after existnce '||l||' and the name of the sequence is seq_name='||seq_name);

--if(l=1) then
	--UpdateSequence(seq_name);
	--execute immediate 'select '||seq_name||'.nextval from dual' into seq_created;
--else
--	execute immediate 'create sequence '||seq_name||' start with 0 increment by 1 MINVALUE 0';
	--execute immediate 'select '||seq_name||'.nextval from dual' into seq_created;
--end if;
--seq_created:=1;

--DBMS_OUTPUT.PUT_LINE ('sequence created 1 '||l);


execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused , node1,node2,system_name) select rownum +'||line_counter||' as pathid,link_weight,lktype,lkused,node_from,node_to,system_name from '||nodetab||' where node_from=:dfrom and link_weight=2' using devfrom;
line_counter:=line_counter+ SQL%ROWCOUNT;
--DBMS_OUTPUT.PUT_LINE ('sequence created 2 '||l);

execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused , node1,node2,node3,system_name) select rownum +'||line_counter||' as pathid,link_weight,lktype,lkused,node1,node2,node3,system_name from '||linktab||' where node1=:dfrom  ' using devfrom;
line_counter:=line_counter+ SQL%ROWCOUNT;
--DBMS_OUTPUT.PUT_LINE ('sequence created 3 '||l);

i:=3;
execute immediate 'insert into '||tabname1||' select pathid,pathused,node1,node2,node1,node2,path_type,1,system_name from '||tabname||' where path_weight=2 and node3=0';
execute immediate 'insert into '||tabname1||' select pathid,pathused, node1,node2,node2,node3,path_type,2,system_name from '||tabname||' where path_weight=2 and node3!=0';

sql_state_temp1:='t.node1,t.node2,';
sql_state_temp2:='node1,node2,node3';
sql_state_temp11:='(t.node1-s.node3)*(t.node2-s.node3)';
DBMS_OUTPUT.PUT_LINE ('before starting the loop '|| (DBMS_UTILITY.GET_TIME-time_before)/100||' and line_counter='||line_counter);
while (stop=0 and count_var<round_trip) loop
	j:=i+1;
	k:=i-1;
	sql_state_temp1:=sql_state_temp1||'t.node'||i||',';
	sql_state_temp11:=sql_state_temp11||'*(t.node'||i||'-s.node3)';
	sql_state_temp2:=sql_state_temp2||',node'||j;
	execute immediate 'insert into '||tabname||'(pathid,path_weight,path_type,pathused, '||sql_state_temp2|| ',system_name) select  rownum +'||line_counter||' as pathid, t.path_weight+s.link_weight,s.lktype,t.pathused*s.lkused, '||sql_state_temp1||' s.node3,s.system_name from '||tabname||' t, '||linktab||' s , lhcb_link_types l,lhcb_link_types f where l.linktypeid=t.path_type and f.linktypeid=s.lktype and t.node'||i||'=s.node2 and t.node'||k||'=s.node1  and mod(f.link_nbr,l.link_nbr)*mod(l.link_nbr,f.link_nbr)=0 and t.path_weight=0 and '||sql_state_temp11||'!=0' ;
	line_counter:=line_counter+ SQL%ROWCOUNT;

	execute immediate 'delete from '||tabname||' where path_weight=0 and node'||j||'=0';
	--if(rt_needed=1) then
	--DBMS_OUTPUT.PUT_LINE ('sequence created='||seq_name||' and count_var='||count_var);
	--	execute immediate 'insert  into '||tabname1||'  select pathid, pathused,node1,node2,node'||i||',node'||j||',path_type,'||i||',system_name from '||tabname||' where path_weight=2 and node'||j||'!=0 and (node'||i||',node'||j||') not in (select nodeid_end0,nodeid_end1 from '||tabname1||' )';
	--else
		execute immediate 'insert  into '||tabname1||'  select pathid, pathused,node1,node2,node'||i||',node'||j||',path_type,'||i||',system_name from '||tabname||' where path_weight=2 and node'||j||'!=0 ';
		line_counter_bis:=line_counter_bis+ SQL%ROWCOUNT;
	--end if;
	select count(pathid ) into pfrom from lhcb_pdet_temp where path_weight=0 and rownum=1;
	if pfrom=0 then
		stop:=1;
	else
		i:=i+1;
		j:=i+1;
	end if;
	DBMS_OUTPUT.PUT_LINE ('in the loop iter='||count_var||' and time='|| (DBMS_UTILITY.GET_TIME-time_before)/100 ||' and rows inserted='||line_counter_bis);
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
--if seq_created=1 then
--	UpdateSequence(seq_name);
--end if;
return res_create;
when OTHERS then
res_create:=sqlcode;
v_error_message:=sqlerrm;
--if seq_created=1 then
--	UpdateSequence(seq_name);
--end if;
return res_create;
end createPathTab;
-----------------------------------------------------------
-- mapping of the paths with the ethernetadd
-----------------------------------------------------------
function createRoutingTable_SP(devfrom number) return number is
v_error_message varchar2(500);
res_create number;
nb_lkid number;

exist_tab number:=0;
dist_lkid number;
tabname1 varchar2(100);
lkid number:=0;
routing_name varchar2(100);
routing_name_copy varchar2(100);
node_name varchar2(500):='lhcb_lg_links';
TYPE devid_list IS TABLE OF number;
pathidList devid_list;
bidi_link number:=0;
devidtoList devid_list;
pathid_nb number;
deviceid_nb number;
i number:=0;
time_before number;
begin
tabname1:='lhcb_startend_temp';
routing_name:='lhcb_routingtab_temp';
time_before := DBMS_UTILITY.GET_TIME;


insert  into lhcb_routingtable_temp(pathid,pathused,ROUTINGPATHUSED,NODEID_START0,pfromid0,ptoid0,ptoid1,NODEID_END1) select f.pathid,f.pathused,0, s.deviceid,t.portidfrom,t.portidto,t.portidto,e.deviceid from  lhcb_macroscopic_connectivity t,lhcb_port_properties e, lhcb_port_properties  s,lhcb_startend_temp f where mod(t.link_type,f.path_type)=0 and f.path_length=1 and t.portidfrom=s.portid and t.portidto=e.portid and s.deviceid=f.nodeid_start0 and e.deviceid=f.nodeid_start1;


insert  into lhcb_routingtable_temp(pathid,pathused,ROUTINGPATHUSED,NODEID_START0,pfromid0,ptoid0,ptoid1,NODEID_END1) select f.pathid,f.pathused,0, s.deviceid,t.portidfrom,t.portidto,m.portidto,n.deviceid from  lhcb_macroscopic_connectivity t,lhcb_macroscopic_connectivity m,lhcb_port_properties e, lhcb_port_properties  s,lhcb_port_properties n, lhcb_port_properties  g ,lhcb_startend_temp f where mod(t.link_type,f.path_type)=0 and mod(m.link_type,f.path_type)=0 and f.path_length>=2 and t.portidfrom=s.portid and t.portidto=e.portid and g.portid=m.portidfrom and n.portid=m.portidto and s.deviceid=f.nodeid_start0 and e.deviceid=f.nodeid_start1 and g.deviceid=f.nodeid_end0 and n.deviceid=f.nodeid_end1;

		--DBMS_OUTPUT.PUT_LINE ('after the first insert');

select count(t.linkway) into bidi_link from lhcb_lg_links t where t.linkway=-1 and rownum=1;

if(bidi_link=1) then
insert  into lhcb_routingtable_temp(pathid,pathused,ROUTINGPATHUSED,NODEID_START0,pfromid0,ptoid0,ptoid1,NODEID_END1) select f.pathid,f.pathused, 0,s.deviceid,t.portidto,t.portidfrom,t.portidfrom,e.deviceid from  lhcb_macroscopic_connectivity t,lhcb_port_properties e, lhcb_port_properties  s,lhcb_startend_temp f where mod(t.link_type,f.path_type)=0 and f.path_length=1  and t.portidto=s.portid and t.portidfrom=e.portid and s.deviceid=f.nodeid_start0 and e.deviceid=f.nodeid_start1 and t.bidirectional_link_used=1;


insert  into lhcb_routingtable_temp(pathid,pathused,ROUTINGPATHUSED,NODEID_START0,pfromid0,ptoid0,ptoid1,NODEID_END1) select f.pathid,f.pathused, 0,s.deviceid,t.portidto,t.portidfrom,m.portidfrom,n.deviceid from  lhcb_macroscopic_connectivity t,lhcb_macroscopic_connectivity m,lhcb_port_properties e, lhcb_port_properties  s,lhcb_port_properties n, lhcb_port_properties  g ,lhcb_startend_temp f where mod(t.link_type,f.path_type)=0 and mod(m.link_type,f.path_type)=0 and f.path_length>=2  and t.bidirectional_link_used=1 and t.portidto=s.portid and t.portidfrom=e.portid and g.portid=m.portidto and n.portid=m.portidfrom and s.deviceid=f.nodeid_start0 and e.deviceid=f.nodeid_start1 and g.deviceid=f.nodeid_end0 and n.deviceid=f.nodeid_end1 and m.bidirectional_link_used=1;


insert  into lhcb_routingtable_temp(pathid,pathused,ROUTINGPATHUSED,NODEID_START0,pfromid0,ptoid0,ptoid1,NODEID_END1) select f.pathid,f.pathused, 0, s.deviceid,t.portidto,t.portidfrom,m.portidto,n.deviceid from  lhcb_macroscopic_connectivity t,lhcb_macroscopic_connectivity m,lhcb_port_properties e, lhcb_port_properties  s,lhcb_port_properties n, lhcb_port_properties  g ,lhcb_startend_temp f where mod(t.link_type,f.path_type)=0 and mod(m.link_type,f.path_type)=0 and t.bidirectional_link_used=1 and t.portidto=s.portid and t.portidfrom=e.portid and g.portid=m.portidfrom and n.portid=m.portidto and s.deviceid=f.nodeid_start0 and e.deviceid=f.nodeid_start1 and g.deviceid=f.nodeid_end0 and n.deviceid=f.nodeid_end1;


insert  into lhcb_routingtable_temp(pathid,pathused,ROUTINGPATHUSED,NODEID_START0,pfromid0,ptoid0,ptoid1,NODEID_END1) select  f.pathid,f.pathused,0,s.deviceid,t.portidfrom,t.portidto,m.portidfrom,n.deviceid from  lhcb_macroscopic_connectivity t,lhcb_macroscopic_connectivity m,lhcb_port_properties e, lhcb_port_properties  s,lhcb_port_properties n, lhcb_port_properties  g ,lhcb_startend_temp f where mod(t.link_type,f.path_type)=0 and mod(m.link_type,f.path_type)=0 and t.portidfrom=s.portid and t.portidto=e.portid and g.portid=m.portidto and n.portid=m.portidfrom and s.deviceid=f.nodeid_start0 and e.deviceid=f.nodeid_start1 and g.deviceid=f.nodeid_end0 and n.deviceid=f.nodeid_end1 and m.bidirectional_link_used=1;

end if;
--DBMS_OUTPUT.PUT_LINE ('time execution for inseting path in temp '|| (DBMS_UTILITY.GET_TIME-time_before)/100);
--for cur_rec in (select min(t.pathid||':'||t.pfromid0) as minpid,t.nodeid_end1 from lhcb_routingtable_temp t group by t.nodeid_end1) loop

--execute immediate 'update lhcb_routingtable_temp f set f.routingpathused=1 where 	f.pathid||'':''||f.pfromid0=:pdi and f.nodeid_end1=:node' using cur_rec.minpid,cur_rec.nodeid_end1;
--i:=i+1;

--end loop;
execute immediate 'update lhcb_routingtable_temp f set f.routingpathused=1 where (f.pathid+1/f.pfromid0)=(select min(t.pathid+1/t.pfromid0) from lhcb_routingtable_temp t where f.ptoid1=t.ptoid1)';

 --DBMS_OUTPUT.PUT_LINE ('after the loop '|| (DBMS_UTILITY.GET_TIME-time_before)/100);

--DBMS_OUTPUT.PUT_LINE ('before the last exceute');

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
function createTD_all_devname(devname varchar2, path_necessary number default 0, round_trip number default 10,recreate_tab number default 1) return number is
v_error_message varchar2(1000);
res_query number:=0;
res_query1 number:=0;
node_type number:=0;
devid number:=0;
time_before number;
time_after number;
sysname number:=1;
exist_tab  number;
connectivitytab varchar2(60):='lhcb_macroscopic_connectivity';
--connectivitytab varchar2(50);
node_table varchar2(60):='lhcb_lg_links';
maxval date;
minval date;

begin
time_before := DBMS_UTILITY.GET_TIME;
--check if the logical table is up to date
execute immediate 'select system_name,deviceid,node from lhcb_lg_devices where devicename=:devname' into sysname,devid,node_type using devname;

res_query:=routingtable_pck.createNodeLink_tab(sysname,recreate_tab);

if(node_type=0) then
	res_query:=routingtable_pck.createPathTab(devid,0,round_trip);
else
	res_query:=routingtable_pck.createPathTab_host(devid,0,round_trip);
end if;
if(res_query=0) then
	res_query1:=routingtable_pck.createTabDestin(devid);
else
	return res_query;
end if;
if(res_query1=0) then
	res_query:=routingtable_pck.Insert_firsttab(devid,0,path_necessary );
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
function createTD_all(devname number,path_necessary number default 0,round_trip number default 10,recreate_tab number default 1) return number is
v_error_message varchar2(1000);
res_query number:=0;
res_query1 number:=0;
node_type number:=0;
time_before number;
time_after number;
time_start number;
exist_tab  number;
connectivitytab varchar2(60):='lhcb_macroscopic_connectivity';
--connectivitytab varchar2(50);
node_table varchar2(60):='lhcb_lg_links';
maxval date;
minval date;
sysname number:=1;
begin
time_start := DBMS_UTILITY.GET_TIME;
time_before := DBMS_UTILITY.GET_TIME;
--check if the logical table is up to date
execute immediate 'select system_name,node from lhcb_lg_devices where deviceid=:devid' into sysname,node_type using devname;
DBMS_OUTPUT.PUT_LINE ('sysname ='|| sysname||' and recreate_tab='||recreate_tab);
res_query:=routingtable_pck.createNodeLink_tab(sysname,recreate_tab);
DBMS_OUTPUT.PUT_LINE ('time execution to create lg views '|| (DBMS_UTILITY.GET_TIME-time_before)/100);
time_after := DBMS_UTILITY.GET_TIME;
if(node_type=0) then
	res_query:=routingtable_pck.createPathTab(devname,0,round_trip);
else
	res_query:=routingtable_pck.createPathTab_host(devname,0,round_trip);
end if;
DBMS_OUTPUT.PUT_LINE ('time execution to create the path tab '|| (DBMS_UTILITY.GET_TIME-time_after)/100);
--DBMS_OUTPUT.PUT_LINE ('Sqlrowcount 1'|| SQL%ROWCOUNT);
time_before := DBMS_UTILITY.GET_TIME;
if(res_query=0) then
	res_query1:=routingtable_pck.createTabDestin(devname);
else
	return res_query;
end if;
DBMS_OUTPUT.PUT_LINE ('time execution to create the destin tab '|| (DBMS_UTILITY.GET_TIME-time_before)/100);
time_after := DBMS_UTILITY.GET_TIME;

--DBMS_OUTPUT.PUT_LINE ('Sqlrowcount 2'|| SQL%ROWCOUNT ||'pathnecessary= '||path_necessary);
if(res_query1=0) then
res_query:=routingtable_pck.Insert_firsttab(devname,0,path_necessary );
--DBMS_OUTPUT.PUT_LINE ('Sqlrowcount 3'|| SQL%ROWCOUNT);
end if;
commit;
DBMS_OUTPUT.PUT_LINE ('time execution to insert in the tab '|| (DBMS_UTILITY.GET_TIME-time_after)/100);

DBMS_OUTPUT.PUT_LINE ('time execution to create the destination tables '|| (DBMS_UTILITY.GET_TIME-time_start)/100);
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
--	execute immediate 'create global temporary table lhcb_startend_temp(pathid number,pathused number,nodeid_start0 number,nodeid_start1 number,nodeid_end0 number,nodeid_end1 number,path_type number, path_length number,system_name number) on commit delete rows';
--	create table lhcb_destin_temp (pathid number,pathused number,nodeid_start0 number,nodeid_end1 number,portIDstart0 number, portIDdest1 number,path_type number,system_name number, path_length number) on commit delete rows';
---------------------------------------------------------------

function createTabDestin(devfrom number) return number is
v_error_message varchar2(500);
exist_tab number:=0;
stop number:=0;
res_create number:=0;
bidi_link number:=0;
--devfrom varchar2(100):=upper(devfrom1);
curr1_tabname varchar2(500):='lhcb_destintab_temp';
curr2_tabname varchar2(500):='lhcb_startend_temp';
curr3_tabname varchar2(500):='lhcb_lg_links';

begin
	insert  into lhcb_destintab_temp
			select f.pathid,f.pathused,s.deviceid,t.portidfrom,m.portidto,n.deviceid
			from  lhcb_macroscopic_connectivity t,lhcb_macroscopic_connectivity m,lhcb_port_properties e, lhcb_port_properties  s,lhcb_port_properties n, lhcb_port_properties  g ,lhcb_startend_temp f
			where mod(t.link_type,f.path_type)=0 and mod(m.link_type,f.path_type)=0 and t.portidfrom=s.portid and t.portidto=e.portid and g.portid=m.portidfrom and n.portid=m.portidto and s.deviceid=f.nodeid_start0 and e.deviceid=f.nodeid_start1 and g.deviceid=f.nodeid_end0 and n.deviceid=f.nodeid_end1;
		
select count(t.linkway) into bidi_link from lhcb_lg_links t where t.linkway=-1 and rownum=1;

if(bidi_link=1) then
		insert  into lhcb_destintab_temp
			select f.pathid,f.pathused, s.deviceid,t.portidto,m.portidfrom,n.deviceid
			from  lhcb_macroscopic_connectivity t,lhcb_macroscopic_connectivity m,lhcb_port_properties e, lhcb_port_properties  s,lhcb_port_properties n, lhcb_port_properties  g ,lhcb_startend_temp f
			where mod(m.link_type,f.path_type)=0 and mod(t.link_type,f.path_type)=0 and m.bidirectional_link_used=1 and t.bidirectional_link_used=1 and t.portidto=s.portid and t.portidfrom=e.portid and g.portid=m.portidto and n.portid=m.portidfrom and s.deviceid=f.nodeid_start0 and e.deviceid=f.nodeid_start1 and g.deviceid=f.nodeid_end0 and n.deviceid=f.nodeid_end1;


		insert  into lhcb_destintab_temp
			select f.pathid,f.pathused, s.deviceid,t.portidto,m.portidto,n.deviceid
			from  lhcb_macroscopic_connectivity t,lhcb_macroscopic_connectivity m,lhcb_port_properties e, lhcb_port_properties  s,lhcb_port_properties n, lhcb_port_properties  g ,lhcb_startend_temp f
			where mod(m.link_type,f.path_type)=0 and mod(t.link_type,f.path_type)=0 and t.bidirectional_link_used=1 and t.portidto=s.portid and t.portidfrom=e.portid and g.portid=m.portidfrom and n.portid=m.portidto and s.deviceid=f.nodeid_start0 and e.deviceid=f.nodeid_start1 and g.deviceid=f.nodeid_end0 and n.deviceid=f.nodeid_end1;


		insert  into lhcb_destintab_temp
			select  f.pathid,f.pathused,s.deviceid,t.portidfrom,m.portidfrom,n.deviceid
			from  lhcb_macroscopic_connectivity t,lhcb_macroscopic_connectivity m,lhcb_port_properties e, lhcb_port_properties  s,lhcb_port_properties n, lhcb_port_properties  g ,lhcb_startend_temp f
			where mod(m.link_type,f.path_type)=0 and mod(t.link_type,f.path_type)=0 and m.bidirectional_link_used=1 and t.portidfrom=s.portid and t.portidto=e.portid and g.portid=m.portidto and n.portid=m.portidfrom and s.deviceid=f.nodeid_start0 and e.deviceid=f.nodeid_start1 and g.deviceid=f.nodeid_end0 and n.deviceid=f.nodeid_end1;
end if;

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
--need to optimize this fct a little: there must be a way to improve it.
--idea we select all the links which are disabled. we enable all the paths. then for each path we look if there is a disable link
-----------------------------------------------------------
function UpdatePathUsed(systemname number) return number is
res_query number:=0;
update_pathused date:=sysdate;
curr1_tabname varchar2(500):='lhcb_pdetails';
curr2_tabname varchar2(500):='lhcb_linkpairs';
curr3_tabname varchar2(500):='lhcb_lg_links';
curr4_tabname varchar2(500):='lhcb_macroscopic_connectivity';
TYPE devid_list IS TABLE OF number;
devidfromList devid_list;
devidtoList devid_list;
devidfromList1 devid_list;
devidtoList1 devid_list;
devfrom number;
devto number;
--cursor C is select  f.deviceid,g.deviceid from lhcb_port_properties f, lhcb_port_properties g,lhcb_macroscopic_connectivity x where f.portid=x.portidfrom and x.portidto=g.portid and x.lkused=0;
sysbis number;
tfc_sysid number;
daq_sysid number;
begin
select systemid into daq_sysid from lhcb_subsystemlist where system_name='DAQ' ;
select systemid into tfc_sysid from lhcb_subsystemlist where system_name='TFC' ;
DBMS_OUTPUT.PUT_LINE ('systemane='||systemname);
if(mod(systemname,daq_sysid)=0 or mod(systemname,tfc_sysid)=0) then



if(systemname>0) then --means it's a systemname we had been given

execute immediate 'select  f.deviceid,g.deviceid from lhcb_port_properties f, lhcb_port_properties g,lhcb_macroscopic_connectivity x where (mod(x.system_name,:sysid)=0 or mod(x.system_name,:sysid)=0) and f.portid=x.portidfrom and x.portidto=g.portid group by f.deviceid,g.deviceid having sum(x.lkused)=0' bulk collect into devidfromList,devidtoList using daq_sysid,tfc_sysid;

else --means devcieid
sysbis:=-systemname;
execute immediate 'select  f.deviceid,g.deviceid from lhcb_port_properties f, lhcb_port_properties g,lhcb_macroscopic_connectivity x where f.portid=x.portidfrom and x.portidto=g.portid and (f.deviceid=:did2 or g.deviceid=:did1) group by f.deviceid,g.deviceid having sum(x.lkused)=0' bulk collect into devidfromList,devidtoList using sysbis,sysbis;
end if;
update lhcb_pdetails set pathused=1;
if devidfromList.count()>0 then
for i in devidfromList.first() ..devidfromList.last() loop
devfrom:=devidfromList(i);
devto:=devidtoList(i);
DBMS_OUTPUT.PUT_LINE ('devfrom= '||devfrom||' and devto='||devto);
execute immediate 'update lhcb_pdetails set last_update=sysdate,pathused=0 where (node1=:devfrom and node2=:devto) or (node2=:devfrom and node1=:devto) ' using devfrom,devto,devfrom,devto;
execute immediate 'update lhcb_pdetails set last_update=sysdate,pathused=0 where (node2=:devfrom and node3=:devto) or (node3=:devfrom and node2=:devto) and pathused=1 and node3!=0' using devfrom,devto,devfrom,devto;
execute immediate 'update lhcb_pdetails set last_update=sysdate,pathused=0 where (node3=:devfrom and node4=:devto) or (node4=:devfrom and node3=:devto) and pathused=1 and node4!=0' using devfrom,devto,devfrom,devto;
execute immediate 'update lhcb_pdetails set last_update=sysdate,pathused=0 where (node4=:devfrom and node5=:devto) or (node5=:devfrom and node4=:devto) and pathused=1 and node5!=0' using devfrom,devto,devfrom,devto;
execute immediate 'update lhcb_pdetails set last_update=sysdate,pathused=0 where (node5=:devfrom and node6=:devto) or (node6=:devfrom and node5=:devto) and pathused=1 and node6!=0' using devfrom,devto,devfrom,devto;
execute immediate 'update lhcb_pdetails set last_update=sysdate,pathused=0 where (node6=:devfrom and node7=:devto) or (node7=:devfrom and node6=:devto) and pathused=1 and node7!=0' using devfrom,devto,devfrom,devto;
execute immediate 'update lhcb_pdetails set last_update=sysdate,pathused=0 where (node7=:devfrom and node8=:devto) or (node8=:devfrom and node7=:devto) and pathused=1 and node8!=0' using devfrom,devto,devfrom,devto;
execute immediate 'update lhcb_pdetails set last_update=sysdate,pathused=0 where (node8=:devfrom and node9=:devto) or (node9=:devfrom and node8=:devto) and pathused=1 and node9!=0' using devfrom,devto,devfrom,devto;
execute immediate 'update lhcb_pdetails set last_update=sysdate,pathused=0 where (node9=:devfrom and node10=:devto) or (node10=:devfrom and node9=:devto) and pathused=1 and node10!=0' using devfrom,devto,devfrom,devto;
execute immediate 'update lhcb_pdetails set last_update=sysdate,pathused=0 where (node10=:devfrom and node11=:devto) or (node11=:devfrom and node10=:devto) and pathused=1 and node10!=0 and node11!=node10' using devfrom,devto,devfrom,devto;

end loop;
end if;
update  lhcb_destinationtable r set r.last_update=sysdate,r.pathused=(select p.pathused from lhcb_pdetails p where p.pathid=r.pathid and p.node1=r.nodeid_start0);
update  lhcb_routingtable r set r.last_update=sysdate,r.pathused=(select p.pathused from lhcb_pdetails p where p.pathid=r.pathid and p.node1=r.nodeid_start0);

update  lhcb_destinationtable r set r.last_update=sysdate,r.pathused=(select p.lkused from lhcb_macroscopic_connectivity p where p.portidfrom=r.pfromid0 or p.portidto=r.pfromid0) where r.pathused=1;
update  lhcb_destinationtable r set r.last_update=sysdate,r.pathused=(select p.lkused from lhcb_macroscopic_connectivity p where p.portidfrom=r.ptoid1 or p.portidto=r.ptoid1) where r.pathused=1;

update  lhcb_routingtable r set r.last_update=sysdate,r.pathused=(select p.lkused from lhcb_macroscopic_connectivity p where p.portidfrom=r.pfromid0 or p.portidto=r.pfromid0) where r.pathused=1;
update  lhcb_routingtable r set r.last_update=sysdate,r.pathused=(select p.lkused from lhcb_macroscopic_connectivity p where p.portidfrom=r.ptoid1 or p.portidto=r.ptoid1) where r.pathused=1;

update lhcb_routingtable f set f.routingpathused=0;

	update lhcb_routingtable f set f.routingpathused=1 where f.pathused=1  and f.pathid||':'||f.pfromid0=(select  min(t.pathid||':'||t.pfromid0) from lhcb_routingtable t where t.nodeid_start0=f.nodeid_start0 and t.ptoid1=f.ptoid1 ) ;

end if;

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
--- find valid path if the deviceid is a host node
-----------------------------------------------------------
function CreatePathTab_Host( devfrom number, rt_needed number, round_trip number default 10) return number is
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
tabname varchar2(100):='lhcb_pdet_temp';
tabname1 varchar2(100);
nodetab varchar2(100):='lhcb_lg_links';
linktab varchar2(100):='lhcb_linkpairs';
line_counter number:=1;
begin
time_before := DBMS_UTILITY.GET_TIME;
tabname1:='lhcb_startend_temp';
--seq_name:='lhcb_sp_'||devfrom;
DBMS_OUTPUT.ENABLE( 10000000);
--l:=routingtable_pck.CheckExistenceSeq(seq_name);

--	if(l=1) then
--		execute immediate 'select '||seq_name||'.nextval from dual' into seq_created;
--	else
--		execute immediate 'create sequence '||seq_name||' start with 0 increment by 1 MINVALUE 0';
--		execute immediate 'select '||seq_name||'.nextval from dual' into seq_created;
--	end if;


	execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused,node1,node2,system_name) select  rownum+'||line_counter||' ,link_weight,lktype,lkused,node_from,node_to,system_name from '||nodetab||' where node_from=:dfrom and link_weight=3' using devfrom;
	line_counter:=line_counter+SQL%ROWCOUNT;
	execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused,node1,node2,node3,system_name) select rownum+'||line_counter||' ,f.link_weight+t.link_weight,f.lktype,f.lkused*t.lkused,t.node_from,t.node_to,f.node_to,f.system_name from '||nodetab||' t,'||nodetab||' f ,lhcb_link_types l,lhcb_link_types m where l.linktypeid=f.lktype and m.linktypeid=t.lktype and mod(l.link_nbr,m.link_nbr)*mod(m.link_nbr,l.link_nbr)=0 and t.node_from=:dfrom and t.node_to=f.node_from and f.node_to!=t.node_from' using devfrom;
	line_counter:=line_counter+SQL%ROWCOUNT;

	i:=3;

	--DBMS_OUTPUT.PUT_LINE ('before tabname1 '|| tabname1 );


	execute immediate 'insert into '||tabname1||' select pathid, pathused,node1,node2,node1,node2,path_type,1,system_name from '||tabname||' where path_weight=3 and node3=0';
	execute immediate 'insert into '||tabname1||' select pathid, pathused,node1,node2,node2,node3,path_type,2,system_name from '||tabname||' where path_weight=3 and node3!=0';

	sql_state_temp1:='t.node1,t.node2,';
	sql_state_temp2:='node1,node2,node3';
	sql_state_temp11:='(t.node1-s.node3)*(t.node2-s.node3)';

	--DBMS_OUTPUT.PUT_LINE ('before entering the loop' );

	while (stop=0 and count_var<round_trip) loop
		j:=i+1;
		k:=i-1;
		sql_state_temp1:=sql_state_temp1||'t.node'||i||',';
		sql_state_temp11:=sql_state_temp11||'*(t.node'||i||'-s.node3)';
		sql_state_temp2:=sql_state_temp2||',node'||j;
		execute immediate 'insert  into '||tabname||'(pathid,path_weight,path_type,pathused, '||sql_state_temp2|| ',system_name) select rownum+'||line_counter||', t.path_weight+s.link_weight,case when l.link_nbr < f.link_nbr then s.lktype else t.path_type  end,t.pathused*s.lkused, '||sql_state_temp1||' s.node3,s.system_name from '||tabname||' t, '||linktab||' s ,lhcb_link_types f,lhcb_link_types l where f.linktypeid=t.path_type and l.linktypeid=s.lktype and t.node'||i||'=s.node2 and t.node'||k||'=s.node1  and mod(f.link_nbr,l.link_nbr)*mod(l.link_nbr,f.link_nbr)=0 and t.path_weight=1 and '||sql_state_temp11||'!=0' ;
		line_counter:=line_counter+SQL%ROWCOUNT;

		execute immediate 'delete from '||tabname||' where path_weight=1 and node'||j||'=0';
		if(rt_needed=1) then
			execute immediate 'insert  into '||tabname1||'  select pathid, pathused,node1,node2,node'||i||',node'||j||',path_type,'||i||', system_name from '||tabname||' where path_weight=3 and node'||j||'!=0 and (node'||i||',node'||j||') not in (select nodeid_end0,nodeid_end1 from '||tabname1||' )';
		else
			execute immediate 'insert  into '||tabname1||'  select pathid, pathused,node1,node2,node'||i||',node'||j||',path_type,'||i||', system_name  from '||tabname||' where path_weight=3 and node'||j||'!=0 ';
		end if;
		select count(pathid ) into pfrom from lhcb_pdet_temp where path_weight!=3 and rownum=1;
		if pfrom=0 then
			stop:=1;
		else
			i:=i+1;
			j:=i+1;
		end if;
		count_var:=count_var+1;
	end loop;
res_create:=0;
--DBMS_OUTPUT.PUT_LINE ('count_var '|| count_var);
return res_create;
exception
when NO_DATA_FOUND then
res_create:=-1;

return res_create;
when OTHERS then
res_create:=sqlcode;
v_error_message:=sqlerrm;

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
tabname varchar2(100):='lhcb_pdet_temp';
tabname1 varchar2(100);
nodetab varchar2(100):='lhcb_lg_links';
linktype varchar2(100):='lhcb_link_types';
line_counter number:=1;

begin
time_before := DBMS_UTILITY.GET_TIME;
tabname1:='lhcb_startend_temp';

seq_name:='lhcb_sp_'||devfrom;
DBMS_OUTPUT.ENABLE( 10000000);

l:=routingtable_pck.CheckExistenceSeq(seq_name);


	if(l=1) then
		--UpdateSequence(seq_name);
		execute immediate 'select '||seq_name||'.nextval from dual' into seq_created;
	else
		execute immediate 'create sequence '||seq_name||' start with 0 increment by 1 MINVALUE 0';
		execute immediate 'select '||seq_name||'.nextval from dual' into seq_created;
	end if;
	seq_created:=1;

execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused , node1,node2,system_name) select rownum+'||line_counter||' ,link_weight,lktype,lkused,node_from,node_to,system_name from '||nodetab||' where node_from=:dfrom and link_weight=2' using devfrom;
line_counter:=line_counter+SQL%ROWCOUNT;
execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused , node1,node2,node3,system_name) select rownum+-'||line_counter||' ,t.link_weight+f.link_weight,f.lktype,t.lkused*f.lkused,t.node_from,t.node_to,f.node_to ,f.system_name from '||nodetab||' t, '||nodetab||' f,'||linktype||' l , '||linktype||' m where l.linktypeid=f.lktype and m.linktypeid=t.lktype and mod(l.link_nbr,m.link_nbr)*mod(m.link_nbr,l.link_nbr)=0   and t.node_from=:dfrom and t.node_to=f.node_from  and f.node_to!=t.node_from and f.link_weight!=1' using devfrom;
line_counter:=line_counter+SQL%ROWCOUNT;

i:=3;
execute immediate 'insert into '||tabname1||' select  pathid, pathused,node1,node2,node1,node2,path_type,1,system_name from '||tabname||' where path_weight=2 and node3=0';
execute immediate 'insert into '||tabname1||' select  pathid, pathused,node1,node2,node2,node3,path_type,2,system_name from '||tabname||' where path_weight=2 and node3!=0';


sql_state_temp1:='t.node1,t.node2,';
sql_state_temp2:='node1,node2,node3';
sql_state_temp11:='(t.node1-s.node_to)*(t.node2-s.node_to)';
while (stop=0 and count_var<round_trip) loop
	j:=i+1;
	k:=i-1;
	sql_state_temp1:=sql_state_temp1||'t.node'||i||',';
	sql_state_temp11:=sql_state_temp11||'*(t.node'||i||'-s.node_to)';
	sql_state_temp2:=sql_state_temp2||',node'||j;
	execute immediate 'insert into '||tabname||'(pathid,path_weight,path_type,pathused, '||sql_state_temp2|| ',system_name) select rownum+'||line_counter||', t.path_weight+s.link_weight,case when l.link_nbr > f.link_nbr then s.lktype else t.path_type end,t.pathused*s.lkused, '||sql_state_temp1||' s.node_to,s.system_name from '||tabname||' t, '||nodetab||' s, lhcb_link_types l,lhcb_link_types f where f.linktypeid=t.path_type and l.linktypeid=s.lktype and t.node'||i||'=s.node_from  and mod(l.link_nbr,f.link_nbr)*mod(f.link_nbr,l.link_nbr)=0 and t.path_weight=0 and '||sql_state_temp11||'!=0' ;
	line_counter:=line_counter+SQL%ROWCOUNT;

	execute immediate 'delete from '||tabname||' where path_weight=0 and node'||j||'=0';
	if(rt_needed=1) then
		execute immediate 'insert  into '||tabname1||'  select pathid,pathused, node1,node2,node'||i||',node'||j||',path_type,'||i||',system_name from '||tabname||' where path_weight=2 and node'||j||'!=0 and (node'||i||',node'||j||') not in (select nodeid_end0,nodeid_end1 from '||tabname1||' )';
	else
		execute immediate 'insert  into '||tabname1||'  select pathid,pathused, node1,node2,node'||i||',node'||j||',path_type,'||i||',system_name from '||tabname||' where path_weight=2 and node'||j||'!=0 ';
	end if;
	select count(pathid ) into pfrom from lhcb_pdet_temp where path_weight=0 and rownum=1;
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
tabname varchar2(100):='lhcb_pdet_temp';
tabname1 varchar2(100);
linktype varchar2(100):='lhcb_link_type';
line_counter number:=1;
nodetab varchar2(100):='lhcb_lg_links';
begin
time_before := DBMS_UTILITY.GET_TIME;
tabname1:='lhcb_startend_temp';
seq_name:='lhcb_sp_'||devfrom;
DBMS_OUTPUT.ENABLE( 10000000);
--l:=routingtable_pck.CheckExistenceSeq(seq_name);
--if(l=1) then
--	--UpdateSequence(seq_name);
--	execute immediate 'select '||seq_name||'.nextval from dual' into seq_created;
--else
--	execute immediate 'create sequence '||seq_name||' start with 0 increment by 1 MINVALUE 0';
--	execute immediate 'select '||seq_name||'.nextval from dual' into seq_created;
--end if;
---	seq_created:=1;

execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused , node1,node2,system_name) select rownum+'||line_counter||' ,link_weight,lktype,lkused,node_from,node_to,system_name from '||nodetab||' where node_from=:dfrom and link_weight=3' using devfrom;
line_counter:=line_counter+SQL%ROWCOUNT;
execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused , node1,node2,node3,system_name) select rownum+'||line_counter||' ,t.link_weight+f.link_weight,f.lktype,t.lkused*f.lkused,t.node_from,t.node_to,f.node_to,f.system_name from '||nodetab||' t, '||nodetab||' f ,'||linktype||' l,'||linktype||' m where l.linktypeid=f.lktype and m.linktypeid=t.lktype and mod(l.link_nbr,m.link_nbr)*mod(m.link_nbr,l.link_nbr)=0 and t.node_from=:dfrom and t.node_to=f.node_from  and f.link_weight!=1 and t.node_from!=f.node_to' using devfrom;
line_counter:=line_counter+SQL%ROWCOUNT;

i:=3;
	execute immediate 'insert into '||tabname1||' select pathid, pathused,node1,node2,node1,node2,path_type,1,system_name from '||tabname||' where path_weight=3 and node3=0';
	execute immediate 'insert into '||tabname1||' select pathid, pathused,node1,node2,node2,node3,path_type,2,system_name from '||tabname||' where path_weight=3 and node3!=0';

sql_state_temp1:='t.node1,t.node2,';
sql_state_temp2:='node1,node2,node3';
sql_state_temp11:='(t.node1-s.node_to)*(t.node2-s.node_to)';
while (stop=0 and count_var<round_trip) loop
	j:=i+1;
	k:=i-1;
	sql_state_temp1:=sql_state_temp1||'t.node'||i||',';
	sql_state_temp11:=sql_state_temp11||'*(t.node'||i||'-s.node_to)';
	sql_state_temp2:=sql_state_temp2||',node'||j;
	execute immediate 'insert into '||tabname||'(pathid,path_weight,path_type,pathused, '||sql_state_temp2|| ',system_name) select rownum+'||line_counter||', t.path_weight+s.link_weight,case when f.link_nbr > l.link_nbr then s.lktype else t.path_type end,t.pathused*s.lkused, '||sql_state_temp1||' s.node_to,s.system_name from '||tabname||' t, '||nodetab||' s , lhcb_link_types l, lhcb_link_types f  where f.linktypeid=t.path_type and l.linktypeid=s.lktype  and t.node'||i||'=s.node_from  and mod(l.link_nbr,f.link_nbr)*mod(f.link_nbr,l.link_nbr)=0 and t.path_weight=1 and '||sql_state_temp11||'!=0' ;
	line_counter:=line_counter+SQL%ROWCOUNT;

	execute immediate 'delete from '||tabname||' where path_weight=0 and node'||j||'=0';
	if(rt_needed=1) then
		execute immediate 'insert  into '||tabname1||'  select pathid, pathused, node1,node2,node'||i||',node'||j||',path_type,'||i||',system_name from '||tabname||' where path_weight=3 and node'||j||'!=0 and (node'||i||',node'||j||') not in (select nodeid_end0,nodeid_end1 from '||tabname1||' )';
	else
		execute immediate 'insert  into '||tabname1||'  select pathid, pathused, node1,node2,node'||i||',node'||j||',path_type,'||i||' ,system_name from '||tabname||' where path_weight=3 and node'||j||'!=0 ';
	end if;
	select count(pathid ) into pfrom from lhcb_pdet_temp where path_weight=1 and rownum=1 ;
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
----------------------------------------------------------------------------------------------
-- find all the valid paths of a length less than 10 starting from that micro device: will be used only if it's a board cpnt
-----------------------------------------------------------
function createMicroPathTab(devfrom number, round_trip number default 10) return number is
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
tabname varchar2(100):='lhcb_micropdet_temp';
nodetab varchar2(100):='lhcb_microlg_links';
linktab varchar2(100):='lhcb_microlinkpairs';
line_counter number:=1;
begin
time_before := DBMS_UTILITY.GET_TIME;
--seq_name:='lhcb_spmicro_'||devfrom;
DBMS_OUTPUT.ENABLE( 10000000);

--l:=routingtable_pck.CheckExistenceSeq(seq_name);
--DBMS_OUTPUT.PUT_LINE ('after existnce '||l);

--if(l=1) then
--	execute immediate 'select '||seq_name||'.nextval from dual' into seq_created;
--	UpdateSequence(seq_name);
--else
--	execute immediate 'create sequence '||seq_name||' start with 0 increment by 1 MINVALUE 0';
--	execute immediate 'select '||seq_name||'.nextval from dual' into seq_created;
--end if;
--seq_created:=1;

--DBMS_OUTPUT.PUT_LINE ('sequence created '||l);


execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused , node1,node2) select rownum+'||line_counter||' ,link_weight,lktype,lkused,node_from,node_to from '||nodetab||' where node_from=:dfrom and link_weight=2' using devfrom;
line_counter:=line_counter+SQL%ROWCOUNT;
execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused , node1,node2,node3) select rownum+'||line_counter||'  ,link_weight,lktype,lkused,node1,node2,node3 from '||linktab||' where node1=:dfrom  and link_weight!=1' using devfrom; --case a cpntid is equal to the boardid
line_counter:=line_counter+SQL%ROWCOUNT;

i:=3;

sql_state_temp1:='t.node1,t.node2,';
sql_state_temp2:='node1,node2,node3';
sql_state_temp11:='(t.node1-s.node3)*(t.node2-s.node3)';
while (stop=0 and count_var<round_trip) loop
	j:=i+1;
	k:=i-1;
	sql_state_temp1:=sql_state_temp1||'t.node'||i||',';
	sql_state_temp11:=sql_state_temp11||'*(t.node'||i||'-s.node3)';
	sql_state_temp2:=sql_state_temp2||',node'||j;
	execute immediate 'insert into '||tabname||'(pathid,path_weight,path_type,pathused, '||sql_state_temp2|| ') select rownum+'||line_counter||', t.path_weight+s.link_weight,s.lktype,t.pathused*s.lkused, '||sql_state_temp1||' s.node3 from '||tabname||' t, '||linktab||' s , lhcb_link_types l,lhcb_link_types f where l.linktypeid=t.path_type and f.linktypeid=s.lktype and t.node'||i||'=s.node2 and t.node'||k||'=s.node1  and mod(f.link_nbr,l.link_nbr)*mod(l.link_nbr,f.link_nbr)=0 and t.path_weight=0 and '||sql_state_temp11||'!=0' ;
	line_counter:=line_counter+SQL%ROWCOUNT;

	execute immediate 'delete from '||tabname||' where path_weight=0 and node'||j||'=0';
	--end if;
	select count(pathid ) into pfrom from lhcb_micropdet_temp where path_weight=0 and rownum=1;
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
end createMicroPathTab;
-----------------------------------------------------------
--- find valid path if the deviceid is a host node: for a micro it is necessary the motherboardname
-----------------------------------------------------------
function CreateMicroPathTab_Host( devfrom_bis number,round_trip number default 10) return number is
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
tabname varchar2(100):='lhcb_micropdet_temp';
tabname1 varchar2(100);
nodetab varchar2(100):='lhcb_microlg_links';
linktab varchar2(100):='lhcb_microlinkpairs';
line_counter number:=1;

devfrom number;
begin

devfrom:=-devfrom_bis;

time_before := DBMS_UTILITY.GET_TIME;
--seq_name:='lhcb_microsphost_'||devfrom_bis;
--DBMS_OUTPUT.ENABLE( 10000000);
--l:=routingtable_pck.CheckExistenceSeq(seq_name);
--DBMS_OUTPUT.PUT_LINE ('just after CheckExistenceSeq'||devfrom );
--	if(l=1) then
--		UpdateSequence(seq_name);
--	else
--		execute immediate 'create sequence '||seq_name||' start with 0 increment by 1 MINVALUE 0';
--		execute immediate 'select '||seq_name||'.nextval from dual' into seq_created;
--	end if;
--	seq_created:=1;

	execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused,node1,node2,node11) select  rownum+'||line_counter||' ,link_weight,lktype,lkused,node_from,node_to,node_to from '||nodetab||' where node_from=:dfrom and link_weight=3' using devfrom;
	--DBMS_OUTPUT.PUT_LINE ('before first sql '|| devfrom );
	line_counter:=line_counter+SQL%ROWCOUNT;
	execute immediate 'insert into '||tabname||' (pathid,path_weight,path_type ,pathused,node1,node2,node3,node11) select rownum+'||line_counter||' ,f.link_weight+t.link_weight,f.lktype,f.lkused*t.lkused,t.node_from,t.node_to,f.node_to,f.node_to from '||nodetab||' t,'||nodetab||' f ,lhcb_link_types l,lhcb_link_types m where l.linktypeid=f.lktype and m.linktypeid=t.lktype and mod(l.link_nbr,m.link_nbr)*mod(m.link_nbr,l.link_nbr)=0 and t.node_from=:dfrom and t.node_to=f.node_from and f.node_to!=t.node_from and t.link_weight=1' using devfrom;
	line_counter:=line_counter+SQL%ROWCOUNT;
	i:=3;

	--DBMS_OUTPUT.PUT_LINE ('before tabname1 '|| tabname1 );



	sql_state_temp1:='t.node1,t.node2,';
	sql_state_temp2:='node1,node2,node3';
	sql_state_temp11:='(t.node1-s.node3)*(t.node2-s.node3)';

	--DBMS_OUTPUT.PUT_LINE ('before entering the loop' ||devfrom);

	while (stop=0 and count_var<round_trip) loop
		j:=i+1;
		k:=i-1;
		sql_state_temp1:=sql_state_temp1||'t.node'||i||',';
		sql_state_temp11:=sql_state_temp11||'*(t.node'||i||'-s.node3)';
		sql_state_temp2:=sql_state_temp2||',node'||j;
		execute immediate 'insert  into '||tabname||'(pathid,path_weight,path_type,pathused, '||sql_state_temp2|| ',node11) select rownum+'||line_counter||', t.path_weight+s.link_weight,case when l.link_nbr < f.link_nbr then s.lktype else t.path_type  end,t.pathused*s.lkused, '||sql_state_temp1||' s.node3 , s.node3 from '||tabname||' t, '||linktab||' s ,lhcb_link_types f,lhcb_link_types l where f.linktypeid=t.path_type and l.linktypeid=s.lktype and t.node'||i||'=s.node2 and t.node'||k||'=s.node1  and mod(f.link_nbr,l.link_nbr)*mod(l.link_nbr,f.link_nbr)=0 and t.path_weight=1 and '||sql_state_temp11||'!=0' ;
		line_counter:=line_counter+SQL%ROWCOUNT;
		execute immediate 'delete from '||tabname||' where path_weight=1 and node'||j||'=0';

		select count(pathid ) into pfrom from lhcb_micropdet_temp where path_weight!=3 and rownum=1;
		if pfrom=0 then
			stop:=1;
		else
			i:=i+1;
			j:=i+1;
		end if;
		count_var:=count_var+1;
	end loop;

res_create:=0;
--DBMS_OUTPUT.PUT_LINE ('count_var '|| count_var);
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
end CreateMicroPathTab_Host;
------------------------------------------------------------------------------------
-- fct return the path details with portnb from a cpnt board to a portid (<0 and =deviceid)
-- it means thta the first node is board cpnt
------------------------------------------------------------------------------------
function InsertIntoMicroPathUsingID( cpntid number,portid number) return number is
v_error_message varchar2(500);
res_temp number:=0 ;
portid1 number;
devfrom number;
begin
if portid>0 then
portid1:=-portid;
	execute immediate 'update lhcb_micropdet_temp set pathused=2 where node1=:pid and pathused=1 and pathid in (select p.pathid from lhcb_micropdet_temp p where p.node1=:pid and (p.node2-:dev1)*(p.node3-:dev1)*(p.node4-:dev1)*(p.node5-:dev1)*(p.node6-:dev1)*(p.node7-:dev1)*(p.node8-:dev1)*(p.node9-:dev1)*(p.node10-:dev1)*(p.node11-:dev1)=0)' using cpntid,cpntid,portid1,portid1,portid1,portid1,portid1,portid1,portid1,portid1,portid1,portid1;
else
	update lhcb_micropdet_temp set pathused=2 where pathused=1;
end if;

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,1,l.cpntname||'|'||decode(p.node1,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node2,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node2>0 and p.node1=l.cpntid and p.node2=h.cpntid and ((p.node1=t.cpntidfrom and p.node2=t.cpntidto) or (p.node2=t.cpntidfrom and p.node1=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,1,l.cpntname||'|'||decode(p.node1,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node2<0 and p.node1=l.cpntid and g.deviceid=h.deviceid and p.node2=-h.portid and ((p.node1=t.cpntidfrom and p.node2=-t.portidto) or  (p.node1=t.cpntidto and p.node2=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,2,l.cpntname||'|'||decode(p.node2,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node3,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node3>0 and p.node2=l.cpntid and p.node3=h.cpntid and ((p.node2=t.cpntidfrom and p.node3=t.cpntidto) or (p.node3=t.cpntidfrom and p.node2=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,2,l.cpntname||'|'||decode(p.node2,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node3<0 and p.node2=l.cpntid and g.deviceid=h.deviceid and p.node3=-h.portid and ((p.node2=t.cpntidfrom and p.node3=-t.portidto) or  (p.node2=t.cpntidto and p.node3=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,3,l.cpntname||'|'||decode(p.node3,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node4,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node4>0 and p.node3=l.cpntid and p.node4=h.cpntid and ((p.node3=t.cpntidfrom and p.node4=t.cpntidto) or (p.node4=t.cpntidfrom and p.node3=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,3,l.cpntname||'|'||decode(p.node3,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node4<0 and p.node3=l.cpntid and g.deviceid=h.deviceid and p.node4=-h.portid and ((p.node3=t.cpntidfrom and p.node4=-t.portidto) or  (p.node3=t.cpntidto and p.node4=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,4,l.cpntname||'|'||decode(p.node4,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node5,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node5>0 and p.node4=l.cpntid and p.node5=h.cpntid and ((p.node4=t.cpntidfrom and p.node5=t.cpntidto) or (p.node5=t.cpntidfrom and p.node4=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,4,l.cpntname||'|'||decode(p.node4,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node5<0 and p.node4=l.cpntid and g.deviceid=h.deviceid and p.node5=-h.portid and ((p.node4=t.cpntidfrom and p.node5=-t.portidto) or  (p.node4=t.cpntidto and p.node5=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,5,l.cpntname||'|'||decode(p.node5,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node6,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node6>0 and p.node5=l.cpntid and p.node6=h.cpntid and ((p.node5=t.cpntidfrom and p.node6=t.cpntidto) or (p.node6=t.cpntidfrom and p.node5=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,5,l.cpntname||'|'||decode(p.node5,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node6<0 and p.node5=l.cpntid and g.deviceid=h.deviceid and p.node6=-h.portid and ((p.node5=t.cpntidfrom and p.node6=-t.portidto) or  (p.node5=t.cpntidto and p.node6=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,6,l.cpntname||'|'||decode(p.node6,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node7,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node7>0 and p.node6=l.cpntid and p.node7=h.cpntid and ((p.node6=t.cpntidfrom and p.node7=t.cpntidto) or (p.node7=t.cpntidfrom and p.node6=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,6,l.cpntname||'|'||decode(p.node6,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node7<0 and p.node6=l.cpntid and g.deviceid=h.deviceid and p.node7=-h.portid and ((p.node6=t.cpntidfrom and p.node7=-t.portidto) or  (p.node6=t.cpntidto and p.node7=-t.portidfrom));



insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,7,l.cpntname||'|'||decode(p.node7,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node8,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node8>0 and p.node7=l.cpntid and p.node8=h.cpntid and ((p.node7=t.cpntidfrom and p.node8=t.cpntidto) or (p.node8=t.cpntidfrom and p.node7=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,7,l.cpntname||'|'||decode(p.node7,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node8<0 and p.node7=l.cpntid and g.deviceid=h.deviceid and p.node8=-h.portid and ((p.node7=t.cpntidfrom and p.node8=-t.portidto) or  (p.node7=t.cpntidto and p.node8=-t.portidfrom));



insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,8,l.cpntname||'|'||decode(p.node8,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node9,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node9>0 and p.node8=l.cpntid and p.node9=h.cpntid and ((p.node8=t.cpntidfrom and p.node9=t.cpntidto) or (p.node9=t.cpntidfrom and p.node8=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,8,l.cpntname||'|'||decode(p.node8,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node9<0 and p.node8=l.cpntid and g.deviceid=h.deviceid and p.node9=-h.portid and ((p.node9=t.cpntidfrom and p.node9=-t.portidto) or  (p.node8=t.cpntidto and p.node9=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,9,l.cpntname||'|'||decode(p.node9,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node10,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node10>0 and p.node9=l.cpntid and p.node10=h.cpntid and ((p.node9=t.cpntidfrom and p.node10=t.cpntidto) or (p.node10=t.cpntidfrom and p.node9=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,9,l.cpntname||'|'||decode(p.node9,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node10<0 and p.node9=l.cpntid and g.deviceid=h.deviceid and p.node10=-h.portid and ((p.node9=t.cpntidfrom and p.node10=-t.portidto) or  (p.node9=t.cpntidto and p.node10=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,10,l.cpntname||'|'||decode(p.node10,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node11<0 and p.node10=l.cpntid and g.deviceid=h.deviceid and p.node11=-h.portid and ((p.node10=t.cpntidfrom and p.node11=-t.portidto) or  (p.node10=t.cpntidto and p.node11=-t.portidfrom));


return res_temp;
exception
when NO_DATA_FOUND then
res_temp:=-1;

return res_temp;
when OTHERS then
res_temp:=sqlcode;
v_error_message:=sqlerrm;

return res_temp;

end InsertIntoMicroPathUsingID;
------------------------------------------------------------------------------------
-- fct return the path details with portnb from a portid (<0 and =deviceid) to a board cpnt
-- it means thta the first node is board cpnt
------------------------------------------------------------------------------------
function InsertIntoMicroPathFromPortID( portid_from number,portid number, cpntname varchar2) return number is
v_error_message varchar2(500);

i number:=1;
pathid number;
lkpos number;
TYPE devid_list IS TABLE OF NUMBER;
pathidList devid_list;
lkposList devid_list;
count_var number:=3;
exist_tab number:=0;
pfrom number:=0;
mboard number:=0;
seq_name varchar2(100);
res_create number:=0;
cpntid number:=0;
path_copy_created number:=0;
path_copy1_created number:=0;
res_temp number:=1;
time_before number;
time_after number;
tabname varchar2(100):='lhcb_micropdet_temp';
tabname1 varchar2(100);
nodetab varchar2(100):='lhcb_microlg_links';
linktab varchar2(100):='lhcb_microlinkpairs';
devfrom number;
cptname varchar2(100):=cpntname||'%';
portid_from1 number;
portid1 number;
begin
portid_from1:=-portid_from;

if portid=0 then
execute immediate 'select cpntid,motherboardid from lhcb_lg_cpnts where cpntname=:cpname' into cpntid,mboard using cpntname;
execute immediate 'update lhcb_micropdet_temp set pathused=2 where node1=:pid and pathused=1 and pathid in (select p.pathid from lhcb_micropdet_temp p where p.node1=:pid and (p.node2-:dev1)*(p.node3-:dev1)*(p.node4-:dev1)*(p.node5-:dev1)*(p.node6-:dev1)*(p.node7-:dev1)*(p.node8-:dev1)*(p.node9-:dev1)*(p.node10-:dev1)*(p.node11-:dev1)=0)' using portid_from1,portid_from1,cpntid,cpntid,cpntid,cpntid,cpntid,cpntid,cpntid,cpntid,cpntid,cpntid;
else
portid1:=-portid;
execute immediate 'update lhcb_micropdet_temp set pathused=2 where node1=:pid and pathused=1 and pathid in (select p.pathid from lhcb_micropdet_temp p where p.node1=:pid and (p.node2-:dev1)*(p.node3-:dev1)*(p.node4-:dev1)*(p.node5-:dev1)*(p.node6-:dev1)*(p.node7-:dev1)*(p.node8-:dev1)*(p.node9-:dev1)*(p.node10-:dev1)*(p.node11-:dev1)=0)' using portid_from1,portid_from1,portid1,portid1,portid1,portid1,portid1,portid1,portid1,portid1,portid1,portid1;

end if;


insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,1,l.devicename||'|'||v.port_nbr||'|'||nvl(v.port_type,'none')||'|'||v.port_way, h.cpntname||'|'||decode(p.node2,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_devices l,lhcb_port_properties v,lhcb_lg_cpnts h where p.pathused=2 and p.node2>0 and p.node1=-v.portid and v.deviceid=l.deviceid and p.node2=h.cpntid and ((p.node1=-t.portidfrom and p.node2=t.cpntidto) or (p.node2=t.cpntidfrom and p.node1=-t.portidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,1,l.devicename||'|'||v.port_nbr||'|'||nvl(v.port_type,'none')||'|'||v.port_way, g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_port_properties h,lhcb_port_properties v,lhcb_lg_devices g,lhcb_lg_devices l where p.pathused=2 and p.node2<0 and p.node1=-v.portid and v.deviceid=l.deviceid and g.deviceid=h.deviceid and p.node2=-h.portid;

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,2,l.cpntname||'|'||decode(p.node2,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node3,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node3>0 and p.node2=l.cpntid and p.node3=h.cpntid and ((p.node2=t.cpntidfrom and p.node3=t.cpntidto) or (p.node3=t.cpntidfrom and p.node2=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,2,l.cpntname||'|'||decode(p.node2,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node3<0 and p.node2=l.cpntid and g.deviceid=h.deviceid and p.node3=-h.portid and ((p.node2=t.cpntidfrom and p.node3=-t.portidto) or  (p.node2=t.cpntidto and p.node3=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,3,l.cpntname||'|'||decode(p.node3,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node4,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node4>0 and p.node3=l.cpntid and p.node4=h.cpntid and ((p.node3=t.cpntidfrom and p.node4=t.cpntidto) or (p.node4=t.cpntidfrom and p.node3=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,3,l.cpntname||'|'||decode(p.node3,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node4<0 and p.node3=l.cpntid and g.deviceid=h.deviceid and p.node4=-h.portid and ((p.node3=t.cpntidfrom and p.node4=-t.portidto) or  (p.node3=t.cpntidto and p.node4=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,4,l.cpntname||'|'||decode(p.node4,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node5,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node5>0 and p.node4=l.cpntid and p.node5=h.cpntid and ((p.node4=t.cpntidfrom and p.node5=t.cpntidto) or (p.node5=t.cpntidfrom and p.node4=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,4,l.cpntname||'|'||decode(p.node4,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node5<0 and p.node4=l.cpntid and g.deviceid=h.deviceid and p.node5=-h.portid and ((p.node4=t.cpntidfrom and p.node5=-t.portidto) or  (p.node4=t.cpntidto and p.node5=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,5,l.cpntname||'|'||decode(p.node5,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node6,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node6>0 and p.node5=l.cpntid and p.node6=h.cpntid and ((p.node5=t.cpntidfrom and p.node6=t.cpntidto) or (p.node6=t.cpntidfrom and p.node5=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,5,l.cpntname||'|'||decode(p.node5,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node6<0 and p.node5=l.cpntid and g.deviceid=h.deviceid and p.node6=-h.portid and ((p.node5=t.cpntidfrom and p.node6=-t.portidto) or  (p.node5=t.cpntidto and p.node6=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,6,l.cpntname||'|'||decode(p.node6,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node7,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node7>0 and p.node6=l.cpntid and p.node7=h.cpntid and ((p.node6=t.cpntidfrom and p.node7=t.cpntidto) or (p.node7=t.cpntidfrom and p.node6=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,6,l.cpntname||'|'||decode(p.node6,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node7<0 and p.node6=l.cpntid and g.deviceid=h.deviceid and p.node7=-h.portid and ((p.node6=t.cpntidfrom and p.node7=-t.portidto) or  (p.node6=t.cpntidto and p.node7=-t.portidfrom));



insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,7,l.cpntname||'|'||decode(p.node7,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node8,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node8>0 and p.node7=l.cpntid and p.node8=h.cpntid and ((p.node7=t.cpntidfrom and p.node8=t.cpntidto) or (p.node8=t.cpntidfrom and p.node7=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,7,l.cpntname||'|'||decode(p.node7,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node8<0 and p.node7=l.cpntid and g.deviceid=h.deviceid and p.node8=-h.portid and ((p.node7=t.cpntidfrom and p.node8=-t.portidto) or  (p.node7=t.cpntidto and p.node8=-t.portidfrom));



insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,8,l.cpntname||'|'||decode(p.node8,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node9,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node9>0 and p.node8=l.cpntid and p.node9=h.cpntid and ((p.node8=t.cpntidfrom and p.node9=t.cpntidto) or (p.node9=t.cpntidfrom and p.node8=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,8,l.cpntname||'|'||decode(p.node8,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node9<0 and p.node8=l.cpntid and g.deviceid=h.deviceid and p.node9=-h.portid and ((p.node9=t.cpntidfrom and p.node9=-t.portidto) or  (p.node8=t.cpntidto and p.node9=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,9,l.cpntname||'|'||decode(p.node9,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node10,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node10>0 and p.node9=l.cpntid and p.node10=h.cpntid and ((p.node9=t.cpntidfrom and p.node10=t.cpntidto) or (p.node10=t.cpntidfrom and p.node9=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,9,l.cpntname||'|'||decode(p.node9,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node10<0 and p.node9=l.cpntid and g.deviceid=h.deviceid and p.node10=-h.portid and ((p.node9=t.cpntidfrom and p.node10=-t.portidto) or  (p.node9=t.cpntidto and p.node10=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,10,l.cpntname||'|'||decode(p.node10,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node11<0 and p.node10=l.cpntid and g.deviceid=h.deviceid and p.node11=-h.portid and ((p.node10=t.cpntidfrom and p.node11=-t.portidto) or  (p.node10=t.cpntidto and p.node11=-t.portidfrom));


if portid=0 then

execute immediate 'select pathid,link_pos from lhcb_micropath_temp where nto like :devid_destin' bulk collect into pathidList,lkposList using cptname;
for i in pathidList.first..pathidList.last loop
	pathid:=pathidList(i);
	lkpos:=lkposList(i);
	--DBMS_OUTPUT.PUT_LINE ('result of pathid= '|| pathid||' and lkpos '||lkpos);
	execute immediate 'delete from lhcb_micropath_temp where pathid=:pid and link_pos>:lkpos' using pathid,lkpos;
end loop;

end if;
return res_create;
exception
when NO_DATA_FOUND then
res_create:=-1;

return res_create;
when OTHERS then
res_create:=sqlcode;
v_error_message:=sqlerrm;

return res_create;

end InsertIntoMicroPathFromPortID;
------------------------------------------------------------------------------------
-- fct return the path details with portnb from a portid (<0 and =deviceid) to a board cpnt
-- it means thta the first node is board cpnt
------------------------------------------------------------------------------------
function InsertIntoMicroPathCnptType(portid_from number, cpnttype varchar2,cpnttype_given number) return number is
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
pathid number;
lkpos number;
TYPE devid_list IS TABLE OF NUMBER;
pathidList devid_list;
lkposList devid_list;
count_var number:=3;
exist_tab number:=0;
pfrom number:=0;
mboard number:=0;
cptype1 varchar2(100);
res_create number:=0;
cpntid number:=0;
devfrom number;
portid_from1 number;
begin
portid_from1:=-portid_from;
if cpnttype_given=1 then
execute immediate 'update lhcb_micropdet_temp set pathused=2 where node1=:pid and pathused=1 and pathid in (select p.pathid from lhcb_micropdet_temp p,lhcb_lg_cpnts e  where p.node1=:pid and (p.node2-e.cpntid)*(p.node3-e.cpntid)*(p.node4-e.cpntid)*(p.node5-e.cpntid)*(p.node6-e.cpntid)*(p.node7-e.cpntid)*(p.node8-e.cpntid)*(p.node9-e.cpntid)*(p.node10-e.cpntid)*(p.node11-e.cpntid)=0  and e.cpnttype=:ctype)' using portid_from1,portid_from1,cpnttype;
else
cptype1:=cpnttype||'%';
execute immediate 'update lhcb_micropdet_temp set pathused=2 where node1=:pid and pathused=1 and pathid in (select p.pathid from lhcb_micropdet_temp p,lhcb_lg_cpnts e where p.node1=:pid and (p.node2-e.cpntid)*(p.node3-e.cpntid)*(p.node4-e.cpntid)*(p.node5-e.cpntid)*(p.node6-e.cpntid)*(p.node7-e.cpntid)*(p.node8-e.cpntid)*(p.node9-e.cpntid)*(p.node10-e.cpntid)*(p.node11-e.cpntid)=0 and e.cpnttype like :ctype)' using portid_from1,portid_from1,cptype1;
end if;

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,1,l.devicename||'|'||v.port_nbr||'|'||nvl(v.port_type,'none')||'|'||v.port_way, h.cpntname||'|'||decode(p.node2,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_devices l,lhcb_port_properties v,lhcb_lg_cpnts h where p.pathused=2 and p.node2>0 and p.node1=-v.portid and v.deviceid=l.deviceid and p.node2=h.cpntid and ((p.node1=-t.portidfrom and p.node2=t.cpntidto) or (p.node2=t.cpntidfrom and p.node1=-t.portidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,1,l.devicename||'|'||v.port_nbr||'|'||nvl(v.port_type,'none')||'|'||v.port_way, g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_port_properties h,lhcb_port_properties v,lhcb_lg_devices g,lhcb_lg_devices l where p.pathused=2 and p.node2<0 and p.node1=-v.portid and v.deviceid=l.deviceid and g.deviceid=h.deviceid and p.node2=-h.portid;

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,2,l.cpntname||'|'||decode(p.node2,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node3,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node3>0 and p.node2=l.cpntid and p.node3=h.cpntid and ((p.node2=t.cpntidfrom and p.node3=t.cpntidto) or (p.node3=t.cpntidfrom and p.node2=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,2,l.cpntname||'|'||decode(p.node2,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node3<0 and p.node2=l.cpntid and g.deviceid=h.deviceid and p.node3=-h.portid and ((p.node2=t.cpntidfrom and p.node3=-t.portidto) or  (p.node2=t.cpntidto and p.node3=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,3,l.cpntname||'|'||decode(p.node3,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node4,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node4>0 and p.node3=l.cpntid and p.node4=h.cpntid and ((p.node3=t.cpntidfrom and p.node4=t.cpntidto) or (p.node4=t.cpntidfrom and p.node3=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,3,l.cpntname||'|'||decode(p.node3,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node4<0 and p.node3=l.cpntid and g.deviceid=h.deviceid and p.node4=-h.portid and ((p.node3=t.cpntidfrom and p.node4=-t.portidto) or  (p.node3=t.cpntidto and p.node4=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,4,l.cpntname||'|'||decode(p.node4,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node5,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node5>0 and p.node4=l.cpntid and p.node5=h.cpntid and ((p.node4=t.cpntidfrom and p.node5=t.cpntidto) or (p.node5=t.cpntidfrom and p.node4=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,4,l.cpntname||'|'||decode(p.node4,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node5<0 and p.node4=l.cpntid and g.deviceid=h.deviceid and p.node5=-h.portid and ((p.node4=t.cpntidfrom and p.node5=-t.portidto) or  (p.node4=t.cpntidto and p.node5=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,5,l.cpntname||'|'||decode(p.node5,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node6,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node6>0 and p.node5=l.cpntid and p.node6=h.cpntid and ((p.node5=t.cpntidfrom and p.node6=t.cpntidto) or (p.node6=t.cpntidfrom and p.node5=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,5,l.cpntname||'|'||decode(p.node5,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node6<0 and p.node5=l.cpntid and g.deviceid=h.deviceid and p.node6=-h.portid and ((p.node5=t.cpntidfrom and p.node6=-t.portidto) or  (p.node5=t.cpntidto and p.node6=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,6,l.cpntname||'|'||decode(p.node6,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node7,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node7>0 and p.node6=l.cpntid and p.node7=h.cpntid and ((p.node6=t.cpntidfrom and p.node7=t.cpntidto) or (p.node7=t.cpntidfrom and p.node6=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,6,l.cpntname||'|'||decode(p.node6,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node7<0 and p.node6=l.cpntid and g.deviceid=h.deviceid and p.node7=-h.portid and ((p.node6=t.cpntidfrom and p.node7=-t.portidto) or  (p.node6=t.cpntidto and p.node7=-t.portidfrom));



insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,7,l.cpntname||'|'||decode(p.node7,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node8,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node8>0 and p.node7=l.cpntid and p.node8=h.cpntid and ((p.node7=t.cpntidfrom and p.node8=t.cpntidto) or (p.node8=t.cpntidfrom and p.node7=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,7,l.cpntname||'|'||decode(p.node7,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node8<0 and p.node7=l.cpntid and g.deviceid=h.deviceid and p.node8=-h.portid and ((p.node7=t.cpntidfrom and p.node8=-t.portidto) or  (p.node7=t.cpntidto and p.node8=-t.portidfrom));



insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,8,l.cpntname||'|'||decode(p.node8,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node9,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node9>0 and p.node8=l.cpntid and p.node9=h.cpntid and ((p.node8=t.cpntidfrom and p.node9=t.cpntidto) or (p.node9=t.cpntidfrom and p.node8=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,8,l.cpntname||'|'||decode(p.node8,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node9<0 and p.node8=l.cpntid and g.deviceid=h.deviceid and p.node9=-h.portid and ((p.node9=t.cpntidfrom and p.node9=-t.portidto) or  (p.node8=t.cpntidto and p.node9=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,9,l.cpntname||'|'||decode(p.node9,t.cpntidfrom,t.portidfrom,t.portidto), h.cpntname||'|'||decode(p.node10,t.cpntidto,t.portidto,t.portidfrom)  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_lg_cpnts l,lhcb_lg_cpnts h where p.pathused=2 and p.node10>0 and p.node9=l.cpntid and p.node10=h.cpntid and ((p.node9=t.cpntidfrom and p.node10=t.cpntidto) or (p.node10=t.cpntidfrom and p.node9=t.cpntidto));
insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,9,l.cpntname||'|'||decode(p.node9,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node10<0 and p.node9=l.cpntid and g.deviceid=h.deviceid and p.node10=-h.portid and ((p.node9=t.cpntidfrom and p.node10=-t.portidto) or  (p.node9=t.cpntidto and p.node10=-t.portidfrom));

insert into lhcb_micropath_temp(pathid,link_pos,nfrom,nto) select p.pathid,10,l.cpntname||'|'||decode(p.node10,t.cpntidfrom,t.portidfrom,t.portidto), g.devicename||'|'||h.port_nbr||'|'||nvl(h.port_type,'none')||'|'||h.port_way  from lhcb_micropdet_temp p, lhcb_microscopic_connectivity t,lhcb_port_properties h,lhcb_lg_devices g,lhcb_lg_cpnts l where p.pathused=2 and p.node11<0 and p.node10=l.cpntid and g.deviceid=h.deviceid and p.node11=-h.portid and ((p.node10=t.cpntidfrom and p.node11=-t.portidto) or  (p.node10=t.cpntidto and p.node11=-t.portidfrom));

if cpnttype_given=1 then
	execute immediate 'select pathid,link_pos from lhcb_micropath_temp,lhcb_lg_cpnts where instr(nto,cpntname)>0 and cpnttype=:cptype' bulk collect into pathidList,lkposList using cpnttype;
else

	execute immediate 'select pathid,link_pos from lhcb_micropath_temp where nto like :cptype' bulk collect into pathidList,lkposList using cptype1;
end if;

for i in pathidList.first..pathidList.last loop
	pathid:=pathidList(i);
	lkpos:=lkposList(i);
	execute immediate 'delete from lhcb_micropath_temp where pathid=:pid and link_pos>:lkpos' using pathid,lkpos;
end loop;


return res_create;
exception
when NO_DATA_FOUND then
res_create:=-1;

return res_create;
when OTHERS then
res_create:=sqlcode;
v_error_message:=sqlerrm;

return res_create;

end InsertIntoMicroPathCnptType;
------------------------------------------------------------------------------------
-----------------------------------------------------------
--need to optimize this fct a little: there must be a way to improve it.
-----------------------------------------------------------
function DeleteLinksInPaths(pfromid number,ptoid number) return number is
res_query number:=0;
update_pathused date:=sysdate;
curr1_tabname varchar2(500):='lhcb_pdetails';
curr2_tabname varchar2(500):='lhcb_linkpairs';
curr3_tabname varchar2(500):='lhcb_lg_links';
curr4_tabname varchar2(500):='lhcb_macroscopic_connectivity';
nb_links number:=0;
devfrom number:=0;
devto number:=0;
--cursor C is select  f.deviceid,g.deviceid from lhcb_port_properties f, lhcb_port_properties g,lhcb_macroscopic_connectivity x where f.portid=x.portidfrom and x.portidto=g.portid and x.lkused=0;
sysbis number;
sqlstmt1 varchar2(1000):='select  min(pathid)||'':''||nodeid_end1 from lhcb_routingtable_temp,lhcb_macroscopic_connectivity where (pfromid0=portidfrom or pfromid0=portidto) and lkused=1 and pathused=1 group by nodeid_end1';
daq_sysid number;
tfc_sysid number;

begin
select systemid into daq_sysid from lhcb_subsystemlist where system_name='DAQ' ;
select systemid into tfc_sysid from lhcb_subsystemlist where system_name='TFC' ;
--DBMS_OUTPUT.PUT_LINE ('after the first select daq='||daq_sysid);
execute immediate 'select  1 from lhcb_port_properties f, lhcb_port_properties g,lhcb_port_properties d,lhcb_port_properties e,lhcb_macroscopic_connectivity t where f.portid=:pfrom and g.portid=:pto and f.deviceid=e.deviceid and g.deviceid=d.deviceid and (e.portid=t.portidfrom and d.deviceid=t.portidto) and rownum=1 and mod(g.deviceid,:tfc)*mod(g.deviceid,:daq)=0'  into nb_links using pfromid,ptoid,daq_sysid,tfc_sysid;
--DBMS_OUTPUT.PUT_LINE ('after third select '||nb_links);
if nb_links=1 then --need to delete links in paths
	execute immediate 'select  f.deviceid,g.deviceid from lhcb_port_properties f, lhcb_port_properties g where f.portid=:pfrom and g.portid=:pto'  into devfrom,devto using pfromid,ptoid;
--DBMS_OUTPUT.PUT_LINE ('before update'||nb_links);

	execute immediate 'update lhcb_pdetails  set pathused=-10 where (node1=:devfrom and node2=:devto) or (node2=:devfrom and node1=:devto) ' using devfrom,devto,devfrom,devto;
	execute immediate 'update lhcb_pdetails  set pathused=-10  where (node2=:devfrom and node3=:devto) or (node3=:devfrom and node2=:devto) and node3!=0' using devfrom,devto,devfrom,devto;
	execute immediate 'update lhcb_pdetails  set pathused=-10  where (node3=:devfrom and node4=:devto) or (node4=:devfrom and node3=:devto) and node4!=0' using devfrom,devto,devfrom,devto;
	execute immediate 'update lhcb_pdetails  set pathused=-10  where (node4=:devfrom and node5=:devto) or (node5=:devfrom and node4=:devto)  and node5!=0' using devfrom,devto,devfrom,devto;
	execute immediate 'update lhcb_pdetails  set pathused=-10  where (node5=:devfrom and node6=:devto) or (node6=:devfrom and node5=:devto)  and node6!=0' using devfrom,devto,devfrom,devto;
	execute immediate 'update lhcb_pdetails  set pathused=-10  where (node6=:devfrom and node7=:devto) or (node7=:devfrom and node6=:devto)  and node7!=0' using devfrom,devto,devfrom,devto;
	execute immediate 'update lhcb_pdetails  set pathused=-10  where (node7=:devfrom and node8=:devto) or (node8=:devfrom and node7=:devto)  and node8!=0' using devfrom,devto,devfrom,devto;
	execute immediate 'update lhcb_pdetails  set pathused=-10  where (node8=:devfrom and node9=:devto) or (node9=:devfrom and node8=:devto)  and node9!=0' using devfrom,devto,devfrom,devto;
	execute immediate 'update lhcb_pdetails  set pathused=-10  where (node9=:devfrom and node10=:devto) or (node10=:devfrom and node9=:devto)  and node10!=0' using devfrom,devto,devfrom,devto;
	execute immediate 'update lhcb_pdetails  set pathused=-10  where (node10=:devfrom and node11=:devto) or (node11=:devfrom and node10=:devto) and node10!=0 and node11!=node10' using devfrom,devto,devfrom,devto;
--DBMS_OUTPUT.PUT_LINE ('before delete'||nb_links);

	execute immediate 'delete  from lhcb_destinationtable r where r.pathid||''|''||r.nodeid_start0 in (select pathid||''|''||node1 from lhcb_pdetails where pathused=-10)';
	execute immediate 'delete  from lhcb_destinationtable r where r.pfromid0=:pfromid or r.pfromid0=:ptoid' using pfromid,ptoid;
	execute immediate 'delete  from lhcb_destinationtable r where r.ptoid1=:pfromid or r.ptoid1=:ptoid' using pfromid,ptoid;

	execute immediate 'delete  from  lhcb_routingtable r where r.pathid||''|''||r.nodeid_start0 in  (select pathid||''|''||node1 from lhcb_pdetails where pathused=-10)';
	execute immediate 'delete  from lhcb_routingtable r where r.pfromid0=:pfromid or r.pfromid0=:ptoid' using pfromid,ptoid;
	execute immediate 'delete  from lhcb_routingtable r where r.ptoid1=:pfromid or r.ptoid1=:ptoid' using pfromid,ptoid;

	if SQL%ROWCOUNT>0 then
		update lhcb_routingtable f set f.routingpathused=0;
	update lhcb_routingtable f set f.routingpathused=1 where f.pathused=1  and f.pathid||':'||f.pfromid0=(select  min(t.pathid||':'||t.pfromid0) from lhcb_routingtable t where t.nodeid_start0=f.nodeid_start0 and t.ptoid1=f.ptoid1 ) ;
	end if;

	delete from lhcb_pdetails where pathused=-10;
	else
	execute immediate 'delete  from lhcb_destinationtable r where r.pfromid0=:pfromid or r.pfromid0=:ptoid' using pfromid,ptoid;
	execute immediate 'delete  from lhcb_routingtable r where r.pfromid0=:pfromid or r.pfromid0=:ptoid' using pfromid,ptoid;
	execute immediate 'delete  from lhcb_destinationtable r where r.ptoid1=:pfromid or r.ptoid1=:ptoid' using pfromid,ptoid;
	execute immediate 'delete  from lhcb_routingtable r where r.ptoid1=:pfromid or r.ptoid1=:ptoid' using pfromid,ptoid;
	execute immediate 'update lhcb_routingtable f set f.routingpathused=-1  where  (nodeid_start0=:dfrom and nodeid_start1=:dto) or (nodeid_start0=:dto and nodeid_start1=:dfrom)' using devfrom,devto,devto,devfrom;
	if SQL%ROWCOUNT>0 then
		update lhcb_routingtable f set f.routingpathused=1 where f.routingpathused=-1 and f.pathused=1  and f.pathid||':'||f.pfromid0=(select  min(t.pathid||':'||t.pfromid0) from lhcb_routingtable t where t.nodeid_start0=f.nodeid_start0 and t.ptoid1=f.ptoid1 ) ;
	end if;
end if;
return res_query;
exception
when NO_DATA_FOUND then

if nb_links=0 then
res_query:=0;
else
res_query:=-1;
end if;
return res_query;
when OTHERS then

res_query:=sqlcode;
return res_query;
end DeleteLinksInPaths;

function CheckPathTabCompleteAndUpdate(systemname in number)  return varchar2 is
rescode number;
time_before number;
result_fct varchar2(100):='NO_ERROR';
sysID number:=-1;
daq_sysid number;
tfc_sysid number;
begin
select systemid into daq_sysid from lhcb_subsystemlist where system_name='DAQ' ;
select systemid into tfc_sysid from lhcb_subsystemlist where system_name='TFC' ;

if(mod(systemname,daq_sysid)=0 or mod(systemname,tfc_sysid)=0) then

--first check if the path table is updated or not

rescode:=routingtable_pck.checkUpdatePathTable(daq_sysid,tfc_sysid);
-- then if pathtable is complete

end if;
if(rescode=0) then
result_fct:='NO_ERROR';
else
result_fct:='Error when recreating table and Ora Err='||result_fct;
end if;
return result_fct;
exception
when NO_DATA_FOUND then
result_fct:='ERROR_NO_DATA_FOUND';


return result_fct;
when OTHERS then
--need_tabupdate:=sqlcode;
result_fct:='ERROR_'||sqlerrm;

return result_fct;
end CheckPathTabCompleteAndUpdate;

------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------
--generate and check existence of key tables
----------------------------------------------------------------------------------------------------
function generKeyDTab(systemname in number) return number  is
tabname varchar2(100);

TYPE intList is table of number;
devIDlist intList;
i number:=1;
resquery number:=0;
exist_iptab number:=0;
limit_node number;
devid number;
rescreate number:=0;
v_error_message varchar2(100);
program_end number:=-1;
j number:=0;
begin
--it's to reduce the number of useless DTs 
--delete old rows if it exists...because if we enter this fct, means we need either to update or to populate
i:=1;
--regenerate all the RT for the DAQ switches
execute immediate 'select t.deviceid from lhcb_lg_devices t where t.node=0 and t.deviceid in (select e.deviceid from lhcb_port_properties e, lhcb_macroscopic_connectivity f where e.portid=f.portidto and rownum=1 and mod(f.system_name,:sysID)=0)' bulk collect into devIDlist using systemname;
if devIDlist.count()>0 then
	for i in devIDlist.first()..devIDlist.last() loop
		devid:=devIDlist(i);
		if(j=1) then 
			rescreate:=rescreate+routingtable_pck.createRT_all(devid,1,10,1);
		else
			rescreate:=rescreate+routingtable_pck.createRT_all(devid,1,10,0);
		end if;
		j:=j+1;
	end loop;
end if;
i:=1;
--regenerate the dhcp and tfc if necessary
execute immediate ' select distinct t.nodeid_start0 from lhcb_destinationtable t where mod(t.system_name,:sysID)=0' bulk collect into devIDlist using systemname;
if devIDlist.count()>0 then
	for i in devIDlist.first()..devIDlist.last() loop
		devid:=devIDlist(i);
		rescreate:=rescreate+routingtable_pck.createTD_all(devid,1,10,0);
	end loop;
end if;


return rescreate;

exception
when NO_DATA_FOUND then

resquery:=1;
return resquery;

when OTHERS then
resquery:=sqlcode;

return resquery;
end generKeyDTab;
--------------------------------------------------------------------
----------------------------------------------------------------------------------------------------
--check if the pathtable is updated or not compared to connectivity
-------------------------------------------------------------------------
function checkUpdatePathTable(daq_sysid in number, tfc_sysid number) return number is
diff_lastupdate number;
TYPE intList is table of number;
sysIDlist intList;
sysname1 number;
result_fct number;
res_query number;
v_error_message varchar2(100);
minval date;
maxval date;
i number:=1;
j number;

begin
--need_tabupdate:=-1;

execute immediate 'select max(last_update) from lhcb_macroscopic_connectivity where mod(system_name,:sysID)=0 or mod(system_name,:sysID)=0' into maxval using tfc_sysid,daq_sysid;

execute immediate 'select min(last_update) from lhcb_pdetails where mod(system_name,:sysID)=0 or mod(system_name,:sysID)=0' into minval using tfc_sysid,daq_sysid;

diff_lastupdate:=ceil(minval-maxval);
--DBMS_OUTPUT.PUT_LINE ('difflastupdate :'||minval);
if diff_lastupdate>0 then -- 
	return 0;
else
	
		execute immediate 'select distinct t.systemid from lhcb_macroscopic_connectivity e, lhcb_subsystemlist t where e.last_update>=:minval and mod(e.system_name,t.systemid)=0  and (t.systemid=:sysID or t.systemid=:sysID1)' bulk collect into sysIDlist using minval,tfc_sysid,daq_sysid;
		
		if sysIDlist.count()=0 then
			
			return 0;
		else
			for i in sysIDlist.first()..sysIDlist.last() loop
				sysname1:=sysIDlist(i);
				result_fct:=routingtable_pck.generKeyDTab(sysname1);
				if(result_fct!=0) then
					res_query:=result_fct;
				else
					res_query:=0;
				end if;
			end loop;
			
		end if;
	
end if;
return res_query;
exception
when NO_DATA_FOUND then
return -1;
when OTHERS then
return sqlcode;
end checkUpdatepathtable;
end routingtable_pck;
/




