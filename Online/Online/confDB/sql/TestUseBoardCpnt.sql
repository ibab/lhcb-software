create or replace function TestUseBoardCpnt(deviceid number,testboard_id number,status_datechange varchar2,username varchar2, user_comments varchar2,host varchar2,serialnb_replaced varchar2 default 'none',replace_date varchar2 default 'none') return number AUTHID CURRENT_USER is
rescode number;
new_device_status number:=3;
date_format varchar2(50):='HH24/MI/SS/MM/DD/YY';
dstatus varchar2(50):='TEST_USE';
snb varchar2(500);
in_use number:=1;
begin
execute immediate 'select t.location from lhcb_lg_cpnt e, lhcb_hw_cpnts t where rownum=1 and t.snbid=e.snbid and e.motherboard=:dname' into snb using deviceid;
execute immediate 'update lhcb_hw_cpnts set cpnt_status=:dstatus where location=:dname' using new_device_status,snb;
if SQL%ROWCOUNT>0 then 
	execute immediate 'update lhcb_hw_cpnts set snbid=(select t.snbid from lhcb_hw_cpnt t where t.hwname=cpntname and t.location=:snb) where motherboardid=:bname' using snb,testboard_id; 
	execute immediate 'insert all into lhcb_cpnt_history (historycpntid,snbid,cpntid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values (LHCB_CPNT_HISTORY_COMPTID_SEQ.nextval,snb2,cpntid,:dstatus,:usercomment,location,to_date(:statchange,:dateformat),sysdate,:login,:host) select snbid as snb2, cpntid,location from lhcb_lg_cpnts where motherboardid=:bname' using dstatus,user_comments,status_datechange,date_format,username,host,testboard_id;

	if serialnb_replaced ='none' then
		execute immediate 'update lhcb_hw_cpnts set snbid=NULL where motherboardid= (select deviceid from lhcb_lg_devices where devicename=:dname)' using deviceid; 
		execute immediate 'insert all into lhcb_cpnt_history (historycpntid,snbid,cpntid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values (LHCB_CPNT_HISTORY_COMPTID_SEQ.nextval,NULL,cpntid,''NO_BOARD'',:usercomment,location,to_date(:statchange,:dateformat),sysdate,:login,:host) select cpntid,location from lhcb_lg_cpnts where motherboardid=:dname' using user_comments,status_datechange,date_format,username,host,deviceid;

	else
		if to_date(status_datechange,date_format)<to_date(replace_date,date_format) then
			execute immediate 'update lhcb_hw_cpnts set snbid=NULL where motherboardid=:dname' using deviceid; 
			execute immediate 'insert all into lhcb_cpnt_history (historycpntid,snbid,cpntid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values (LHCB_CPNT_HISTORY_COMPTID_SEQ.nextval,NULL,cpntid,''NO_BOARD'',:usercomment,location,to_date(:statchange,:dateformat),sysdate,:login,:host) select cpntid,location from lhcb_lg_cpnts where motherboardid=:dname' using user_comments,status_datechange,date_format,username,host,deviceid;
		else
			if to_date(status_datechange,date_format)>to_date(replace_date,date_format) then
				return -15;
			else
				execute immediate 'update lhcb_lg_cpnts set snbid=(select t.snbid from lhcb_hw_cpnts t where t.hwname=cpntname and t.location=:snb) where motherboardid=:dname ' using serialnb_replaced,deviceid; 
				execute immediate 'insert all into lhcb_cpnt_history (historycpntid,snbid,cpntid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values (LHCB_CPNT_HISTORY_COMPTID_SEQ.nextval,snbid,cpntid,''IN_USE'',:usercomment,location,to_date(:statchange2,:dateformat),sysdate,:login,:host)  select snbid,cpntid,location from lhcb_lg_cpnts where motherboardid=:dname' using user_comments,replace_date,date_format,username, host,deviceid; 
				execute immediate 'update lhcb_hw_cpnts set cpnt_status=:dstatus where location=:snb' using new_device_status,serialnb_replaced;
			end if;
		end if;
	end if;
end if;
dbms_output.put_line('after second loop');
rescode:=0;
return rescode;
exception
when NO_DATA_FOUND then
--if end_mandatory_part=0 then
rescode:=-1;
--end if;
return rescode;
when OTHERS then
rescode:=sqlcode;
return rescode;
end TestUseBoardCpnt;
/