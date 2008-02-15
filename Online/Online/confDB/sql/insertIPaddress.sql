create or replace function InsertIPaddress(ipadd varchar2,ipname varchar2, subnetname varchar2,username1 varchar2,hostname1 varchar2) return number AUTHID CURRENT_USER is

rescode number:=0;
rec_count number:=0;
begin
execute immediate 'select count(*) from lhcb_ipinfo where ipaddress=:ipaddress and rownum=1' into rec_count using ipadd;
if rec_count=0 then
execute immediate 'insert into lhcb_ipinfo(ipaddress,ipname,subnet_info,created,terminal_name,author) values (:ipadd,:ipname,:subnet,sysdate,:hostname1,:username1)' using ipadd,ipname,subnetname,hostname1,username1;
end if;

return 0;
exception
when NO_DATA_FOUND then
rescode:=-1;

return rescode;
when OTHERS then
rescode:=sqlcode;

return rescode;

end InsertIPaddress;
/