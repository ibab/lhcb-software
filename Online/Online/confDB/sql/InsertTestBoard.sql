create or replace procedure InsertTestBoard(location varchar2,login varchar2, host varchar2,rescode in out number,devid in out number)  AUTHID CURRENT_USER is
deviceid number;
dname varchar2(100);
dname_next varchar2(100);
next_one number;

begin
--DBMS_OUTPUT.PUT_LINE ('entering the fct');
select nvl(max(devicename),'none')  into dname from lhcb_lg_devices where devicename like 'TEST_BOARD%';
--DBMS_OUTPUT.PUT_LINE ('value of dname ' ||dname);
if dname='none' then
execute immediate 'insert into lhcb_lg_devices(devicename,node,deviceid,location,terminal_name,author) values (''TEST_BOARD_1'',0,lhcb_lg_deviceseq.nextval,:location,:host,:username) returning deviceid into :devid'  using location,host,login returning into deviceid;
else
--DBMS_OUTPUT.PUT_LINE ('value of dname ' ||dname);
next_one:=to_number(substr(dname,instr(dname,'_',1,2)+1))+1;
dname_next:='TEST_BOARD_'||next_one;
--DBMS_OUTPUT.PUT_LINE ('value of dname_next ' ||dname_next);
execute immediate 'insert into lhcb_lg_devices(devicename,node,deviceid,location,terminal_name,author) values (:dname,0,lhcb_lg_deviceseq.nextval,:location,:host,:username) returning deviceid into :devid'  using dname_next,location,host,login returning into deviceid;
end if;
devid:=deviceid;
rescode:=0;

exception
when NO_DATA_FOUND then
rescode:=-1;
devid:=-1;
when OTHERS then
rescode:=sqlcode;
devid:=-1;
end InsertTestBoard;
/