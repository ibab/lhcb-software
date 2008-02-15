---------------------------------------------
--PL/SQL path queries packages
--author L.Abadie
--v2.0
------------------------------------
create or replace package body pathqueries_pck
is
-----------------------------------------------------------
--Convention we always give sysname=sysname+'_': i.e. if it's VELO, we give VELO_
-----------------------------------------------------------
----------------------------------------------------------
----fct which creates the logical view of the system : we just keep link between devices (get rid of port nb)
----------------------------------------------------------

function CheckPathTabCompleteAndUpdate(systemname in number)  return varchar2 is

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

result_fct:=pathqueries_pck.checkUpdatePathTable(daq_sysid,tfc_sysid);
-- then if pathtable is complete

end if;
if(result_fct=0) then
return result_fct;
else
result_fct:='Error when recreating table and Ora Err='||result_fct;
end if;
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


i number:=0;
resquery number:=0;
exist_iptab number:=0;
limit_node number;
rec_count number;
rescreate number:=0;
v_error_message varchar2(100);
program_end number:=-1;
begin
--it's to reduce the number of useless DTs 
--delete old rows if it exists...because if we enter this fct, means we need either to update or to populate
i:=1;
for cur_rec in (select distinct t.nodeid_start0 from lhcb_routingtable t) loop
	if(i=1) then 
		rescreate:=rescreate+routingtable_pck.createRT_all(cur_rec.nodeid_start0,1,10,1);
	else
		rescreate:=rescreate+routingtable_pck.createRT_all(cur_rec.nodeid_start0,1,10,0);
	end if;
	i:=i+1;
end loop;

for cur_rec1 in (select distinct t.nodeid_start0 from lhcb_destinationtable t) loop
	rescreate:=rescreate+routingtable_pck.createTD_all(cur_rec1.nodeid_start0,1,10,0);
	
end loop;



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
	
		execute immediate 'select distinct system_name from lhcb_macroscopic_connectivity where last_update>=:minval and (mod(system_name,:sysID)=0 or mod(system_name,:sysID)=0)' bulk collect into sysIDlist using minval,tfc_sysid,daq_sysid;
		
		if sysIDlist.count()=0 then
			
			return 0;
		else
			for i in sysIDlist.first()..sysIDlist.last() loop
				sysname1:=sysIDlist(i);
				result_fct:=pathqueries_pck.generKeyDTab(sysname1);
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



end pathqueries_pck;
/
