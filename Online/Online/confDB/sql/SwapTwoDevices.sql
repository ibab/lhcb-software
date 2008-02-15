create or replace function SwapTwoDevices(logicalName1 varchar2,logicalName2 varchar2, username varchar2, user_comments varchar2) return varchar2 AUTHID CURRENT_USER is
portid number;

rescode varchar2(100);
rec_count number:=0;
deviceid1 number;
deviceid2 number;
dtypeID1 varchar2(100);
dtypeID2 varchar2(100);
loc1 varchar2(100);
loc2 varchar2(100);
sys1 number;
sys2 number;
usercomments varchar2(1000);
update_date date;

port_ID1 number;
port_ID2 number;

TYPE numberList is table of number;
TYPE stringList is table of varchar2(100);
i number:=0;
count1 number:=0;
count2 number:=0;
portid1 number;
portid2 number;
snb1List stringList;
snb2List stringList;
cpnt1List numberList;
cpnt2List numberList;
end_mandatory_part number:=0;
serialnb1 varchar2(100);
serialnb2 varchar2(100);
cpnt1 number;
cpnt2 number;
serialnb11 varchar2(100);
serialnb22 varchar2(100);

begin
update_date:=sysdate;
if user_comments = 'none' then
usercomments:='Swapping '||logicalName1||' and '||logicalName2||'. ';
else
usercomments:='Swapping '||logicalName1||' and '||logicalName2||'. '||user_comments;
end if;
execute immediate 'select serialnb,deviceid,devicetypeID,location,system_name from lhcb_lg_devices where devicename=:dname1' into serialnb1,deviceid1,dtypeID1,loc1,sys1 using logicalName1;
execute immediate 'select serialnb,deviceid,devicetypeID,location,system_name from lhcb_lg_devices where devicename=:dname1' into serialnb2,deviceid2,dtypeID2,loc2,sys2 using logicalName2;

if dtypeID1=dtypeID2 then
	DBMS_OUTPUT.PUT_LINE ('first select insert'||dtypeID1);
	execute immediate 'select count(portid) from lhcb_port_properties where deviceid=:did1 order by port_way,port_nbr,port_type '  into port_ID1 using deviceid1;
	DBMS_OUTPUT.PUT_LINE ('first select insert bis');
	execute immediate 'select count(portid) from lhcb_port_properties where deviceid=:did1 order by port_way,port_nbr,port_type ' into port_ID2 using deviceid2;
	--need to check if the nb of ports is equal
	DBMS_OUTPUT.PUT_LINE ('first select insert ter');
	if port_ID1=port_ID2 then
				--end_mandatory_part:=1;
				execute immediate 'select serialnb,cpntid from lhcb_lg_cpnts where motherboardID=:dID1 ' bulk collect into snb1List,cpnt1List using deviceid1;
				execute immediate 'select serialnb,cpntid from lhcb_lg_cpnts where motherboardID=:dID2 ' bulk collect into snb2List,cpnt2List using deviceid2;
				DBMS_OUTPUT.PUT_LINE ('first select insert 4');
				if snb1List.count()>0 then
					DBMS_OUTPUT.PUT_LINE ('first select insert 44');
					if snb1List.count()=snb2List.count() then
						for i in snb1List.first..snb2List.last loop
						serialnb11:=snb1List(i);
						serialnb22:=snb2List(i);
						cpnt2:=cpnt2List(i);
						cpnt1:=cpnt1List(i);
						execute immediate 'update lhcb_lg_cpnts set last_update=:date1,user_update=:user,serialnb=decode(motherboardID,:dID1,:snb2,dID2,:snb1,serialnb)' using update_date,username, deviceid1,serialnb22,deviceid2,serialnb11;
						execute immediate 'insert into lhcb_cpnt_history(historycpntID,serialnb,cpntid,cpnt_status,user_comment,location,status_change,system_name,created,author,terminal_name) values (lhcb_cpnt_history_comptid_seq.nextval,:snb,''IN_USE'',:comments,:location,:statchange,:sysname,sysdate,:author)' using serialnb1,cpnt2,usercomments,deviceid2,update_date,sys2,username;
						execute immediate 'insert into lhcb_cpnt_history(historycpntID,serialnb,cpntid,cpnt_status,user_comment,location,status_change,system_name,created,author,terminal_name) values (lhcb_cpnt_history_comptid_seq.nextval,:snb,''IN_USE'',:comments,:location,:statchange,:sysname,sysdate,:author)' using serialnb2,cpnt1,usercomments,deviceid1,update_date,sys1,username;
						end loop;				
						rescode:='SUCCESSFUL_SWAPPING';
					else
						rescode:='ERROR_SWAPPING_NBOFCHIPS_ARE_DIFFERENT';
						return rescode;
					end if;
				end if;
				DBMS_OUTPUT.PUT_LINE ('first select insert 445');
				execute immediate 'update lhcb_lg_devices set last_update=:date1,user_update=:user1, serialnb=decode(deviceid,:dID1,:serialnb2,:dID2,:serialnb1,serialnb)' using update_date,username,deviceid1,serialnb2,deviceid2,serialnb1;
				DBMS_OUTPUT.PUT_LINE ('first select insert 5');
				execute immediate 'insert  into lhcb_device_history(historydeviceID,serialnb,deviceid,device_status,user_comment,location,status_change,system_name,author) values(lhcb_history_deviceseq.nextval,:serialnb,:deviceid,''IN_USE'', :comments,:location,:statuschange,:sysname,:author)' using serialnb1,deviceid2,usercomments,loc2,update_date,sys2,username;
				execute immediate 'insert  into lhcb_device_history(historydeviceID,serialnb,deviceid,device_status,user_comment,location,status_change,system_name,author) values(lhcb_history_deviceseq.nextval,:serialnb,:deviceid,''IN_USE'', :comments,:location,:statuschange,:sysname,:author)' using serialnb2,deviceid1,usercomments,loc1,update_date,sys1,username;
				DBMS_OUTPUT.PUT_LINE ('after loop');
				rescode:='SUCCESSFUL_SWAPPING';
	else
				rescode:='ERROR_SWAPPING_NBOFPORTS_ARE_DIFFERENT';
	end if;
else
	rescode:='ERROR_SWAPPING_DEVICETYPES_ARE_DIFFERENT';
end if;
commit;
dbms_output.put_line('after second loop');
return rescode;
exception
when NO_DATA_FOUND then
--if end_mandatory_part=0 then
rescode:='ERROR_NO_DATA_FOUND';
--end if;
return rescode;
when OTHERS then
rescode:='ERROR_'||sqlerrm;
return rescode;
end SwapTwoDevices;
/