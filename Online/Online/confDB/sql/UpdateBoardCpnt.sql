create or replace function UpdateBoardCpnt(deviceid number,new_location varchar2,new_device_status number,status_datechange varchar2,username varchar2, user_comments varchar2,host varchar2,serialnb_replaced varchar2 default 'none',replace_date varchar2 default 'none') return number AUTHID CURRENT_USER is

rescode number;
date_format varchar2(50):='HH24/MI/SS/MM/DD/YY';
dstatus varchar2(50);
in_use_status number:=1;
begin

 execute immediate 'update lhcb_hw_cpnts set cpnt_status=:dstatus where snbid in (select snbid from lhcb_lg_cpnts where MOTHERBOARDID=:dname)' using new_device_status,deviceid;
if SQL%ROWCOUNT>0 then 
	execute immediate 'insert all into lhcb_cpnt_history (historycpntid,snbid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values (LHCB_CPNT_HISTORY_COMPTID_SEQ.nextval,snb2,:dstatus,:usercomment,:dlocation,to_date(:statchange,:dateformat),sysdate,:login,:host) select snbid as snb2 from lhcb_lg_cpnts where motherboardid=:dname' using dstatus,user_comments,new_location,status_datechange,date_format,username,host,deviceid;
	if serialnb_replaced='none' or to_date(status_datechange,date_format)<to_date(replace_date,date_format) then
		execute immediate 'update lhcb_lg_cpnts set snbid=NULL,user_update=:login,last_update=sysdate where motherboardid=:dname ' using username,deviceid; 
		execute immediate 'insert all into lhcb_cpnt_history (historycpntid,cpntid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values (LHCB_CPNT_HISTORY_COMPTID_SEQ.nextval,cpntid,''NO_BAORD'',:usercomment,motherboardid,to_date(:statchange,:dateformat),sysdate,:login,:host) select cpntid,motherboardid from lhcb_lg_cpnts where motherboardid=:dname' using user_comments,status_datechange,date_format,username,host,deviceid;
	end if;	
	if serialnb_replaced not like 'none' then
		if to_date(status_datechange,date_format)>to_date(replace_date,date_format) then 
			return -15;
		end if;
	else
		execute immediate 'update lhcb_lg_cpnts set snbid=(select t.snbid from lhcb_hw_cpnts t where t.hwname=cpntname and t.location=:snb) where motherboardid=:dname ' using serialnb_replaced,deviceid; 
		execute immediate 'insert all into lhcb_cpnt_history (historycpntid,snbid,cpntid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values (LHCB_CPNT_HISTORY_COMPTID_SEQ.nextval,snbid,cpntid,''IN_USE'',:usercomment,location,to_date(:statchange2,:dateformat),sysdate,:login,:host)  select snbid,cpntid,location from lhcb_lg_cpnts where motherboardid=:dname' using user_comments,replace_date,date_format,username, host,deviceid; 
		execute immediate 'update lhcb_hw_cpnts set cpnt_status=:dstatus where location=:snb' using in_use_status,serialnb_replaced;
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
end UpdateBoardCpnt;
/