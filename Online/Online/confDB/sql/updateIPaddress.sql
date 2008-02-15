create or replace function UpdateIPaddress(old_ipadd varchar2,new_ipadd varchar2, username varchar2) return number AUTHID CURRENT_USER is
TYPE devid_list IS TABLE OF NUMBER;
portIDlist devid_list;
ipAliaslist devid_list;
portid number;
i number:=1;
j number:=0;
source_cursor number:=dbms_sql.open_cursor;

rescode number:=0;
rec_count number:=0;
deviceid number;
ipalias varchar2(100);
begin

portIDlist:=devid_list();
ipAliaslist:=devid_list();
dbms_sql.parse(source_cursor,'select portid from lhcb_port_properties where ipaddress=:ipadd',dbms_sql.native);
dbms_sql.bind_variable(source_cursor,':ipadd',old_ipadd);
rec_count:=dbms_sql.execute(source_cursor);
dbms_sql.define_column(source_cursor,1,portid);
loop
EXIT WHEN DBMS_SQL.FETCH_ROWS (source_cursor)=0;
DBMS_SQL.COLUMN_VALUE (source_cursor, 1, portid);
portIDlist.extend();
portIDlist(i):=portid;
i:=i+1;
execute immediate 'update lhcb_port_properties set ipaddress=NULL where portid=:portid' using portid;
end loop;
dbms_sql.close_cursor(source_cursor);
dbms_output.put_line('after first loop i='||i);

dbms_sql.parse(source_cursor,'select ipalias from lhcb_ipaliases where ipaddress=:ipadd',dbms_sql.native);
dbms_sql.bind_variable(source_cursor,':ipadd',old_ipadd);
rec_count:=dbms_sql.execute(source_cursor);
dbms_sql.define_column(source_cursor,1,ipalias,100);
loop
EXIT WHEN DBMS_SQL.FETCH_ROWS (source_cursor)=0;
DBMS_SQL.COLUMN_VALUE (source_cursor, 1, ipalias);
ipAliaslist.extend();
ipAliaslist(j):=ipalias;
j:=j+1;
execute immediate 'update lhcb_ipaliases set ipaddress=NULL where ipalias=:ipalias' using ipalias;
end loop;
dbms_sql.close_cursor(source_cursor);
dbms_output.put_line('after second loop i='||i);


execute immediate 'update lhcb_ipinfo set ipaddress=:newaddress, last_update=sysdate,author=:username where ipaddress=:oldadd' using new_ipadd,username,old_ipadd;

if i> 1 then
	dbms_output.put_line('before second loop'||portIDlist(1));
	for i in portIDlist.first..portIDlist.last loop
		portid:=portIDlist(i);
		dbms_output.put_line('in the second loop : before update');
		execute immediate 'update lhcb_port_properties set ipaddress=:newaddress,last_update=sysdate,user_update=:username  where portid=:portid' using new_ipadd,username,portid;
	end loop;
end if;

if j> 1 then
	dbms_output.put_line('before second loop'||portIDlist(1));
	for j in ipAliaslist.first..ipAliaslist.last loop
		ipalias:=ipAliaslist(j);
		dbms_output.put_line('in the second loop : before update');
		execute immediate 'update lhcb_ipaliases set ipaddress=:newaddress,last_update=sysdate,user_update=:username  where ipalias=:ipalias' using new_ipadd,username,ipalias;
	end loop;
end if;

commit;
dbms_output.put_line('after second loop');
return 1;
exception
when NO_DATA_FOUND then
rescode:=-1;
if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
dbms_sql.close_cursor(source_cursor);
end if;
return rescode;
when OTHERS then
rescode:=sqlcode;
if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
dbms_sql.close_cursor(source_cursor);
end if;
return rescode;

end UpdateIPaddress;
/