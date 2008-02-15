declare
 
t number; 
time_before number:=0;
i number:=0;
begin
time_before := DBMS_UTILITY.GET_TIME;
for cur_rec in (select  t.deviceid from lhcb_lg_devices t where node=0 and mod(t.system_name,5)=0 and t.devicetypeid in (81,82) and deviceid in (select deviceid from lhcb_port_properties)) loop
	if i=0 then
 		t:=routingtable_pck.createRT_all(cur_rec.deviceid,1,10,1);	
 	else
 	 	t:=routingtable_pck.createRT_all(cur_rec.deviceid,1,10,0);
	end if;
 DBMS_OUTPUT.PUT_LINE ('time execution to create routing table for device='||cur_rec.deviceid||' and time= '|| (DBMS_UTILITY.GET_TIME-time_before)/100);
i:=i+1;
end loop;

end;
/
