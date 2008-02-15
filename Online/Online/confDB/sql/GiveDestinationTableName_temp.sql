
------------------------------------------------------------------------
create or replace procedure GiveDestinTableName_temp(sysname in varchar2,devfrom in varchar2, devto in varchar2, cursorC1 out sys_refcursor )  AUTHID CURRENT_USER is
tabname varchar2(100);
TYPE element_list IS TABLE OF VARCHAR2(500);
TYPE devid_list IS TABLE OF NUMBER;

rec_count number;
EltList1 element_list:=element_list('test','test'); --max 4 elements
DevList devid_list:=devid_list(0,0);-- max 4 elements
NodeList devid_list:=devid_list(0,0);
deviceid number:=0;
deviceid_first number:=0;
fdbk number:=0;
tabname_cur varchar2(100);
tabname_cur1 varchar2(100):=sysname||'path_temp_view';
node1 number:=0;
fdbk1 number:=0;
node number:=0;
node_first number:=0;
rescount number:=0;
rescount_old number:=0;
tabname1 varchar2(100);
tabname2 varchar2(100);
tabname3 varchar2(100);
i number:=1;
j number:=0;
k number:=0;
case_res number:=0;
time_before number;
source_cursor number:=dbms_sql.open_cursor;
begin
time_before := DBMS_UTILITY.GET_TIME;
dbms_sql.parse(source_cursor,'select deviceid, node from '||sysname||'device_view where upper(devicename)=upper(:dfrom) or upper(devicename)=upper(:dto)',dbms_sql.native);
dbms_sql.bind_variable(source_cursor,':dfrom',devfrom);
dbms_sql.bind_variable(source_cursor,':dto',devto);
rec_count:=dbms_sql.execute(source_cursor);
dbms_sql.define_column(source_cursor,1,deviceid);
dbms_sql.define_column(source_cursor,2,node1);
while (i<4) LOOP
	EXIT WHEN DBMS_SQL.FETCH_ROWS (source_cursor)=0;
	DBMS_SQL.COLUMN_VALUE (source_cursor, 1, deviceid);
	
	DBMS_SQL.COLUMN_VALUE (source_cursor, 2, node1);
	DevList(i):=deviceid;
	
	NodeList(i):=node1;
	i:=i+1;
END LOOP;
dbms_sql.close_cursor(source_cursor);
dbms_output.put_line('DevList(1) '||DevList(1)||' i : '||i);

if(i!=3) then
	case_res:=10;
	open cursorC1 for select -1,-1,-3 from dual ;
end if;

if(NodeList(2)=0 and NodeList(1)=0) then
	case_res:=10;
	open cursorC1 for select -1,-1,-3 from dual ;
end if;

if(case_res!=10) then
	deviceid:=0;
	if(Nodelist(2)=1) then
		execute immediate 'select count(*) from '||sysname||'destinationtable_view where nodeid_start0=:devfrom and rownum=1' into fdbk using DevList(1);
		if(fdbk=0) then 
			execute immediate 'select count(*) from '||sysname||'routingtable_view where nodeid_start0=:devfrom and rownum=1' into fdbk1 using DevList(1);
			if(fdbk1=1) then
				tabname_cur:=sysname||'routingtable';
				case_res:=1;
			else
				case_res:=-1;
			end if;
		else
			tabname_cur:=sysname||'destinationtable';
			case_res:=1;
		end if;
	end if;
	if(case_res=1) then
		execute immediate 'select count(*) from '||tabname_cur||' where nodeid_start0=:devfrom and nodeid_end1=:devto and rownum=1' into rescount using DevList(1), DevList(2);
		if(rescount=0) then 
			case_res:=-3; --need to look in the other tables
		else
			case_res:=2; --means we find the connect
			deviceid_first:=DevList(1);
			deviceid:=DevList(2);
		end if;
	end if;

	if(case_res!=2) then
		if(Nodelist(1)=1 ) then
			execute immediate 'select count(*) from '||sysname||'destinationtable_view where nodeid_start0=:devfrom and rownum=1' into fdbk using DevList(2);
			if(fdbk=0) then 
				execute immediate 'select count(*) from '||sysname||'routingtable_view where nodeid_start0=:devfrom and rownum=1' into fdbk1 using DevList(2);
				if(fdbk1=1) then
					tabname_cur:=sysname||'routingtable';
					case_res:=1;
				else
					rescount:=routingtable_pck_view.createTD_all(sysname,DevList(2));
					if(rescount=0) then
						tabname_cur:=sysname||'destinationtable';
						case_res:=1;
					else
						if(case_res=-3) then
							rescount:=routingtable_pck_view.createTD_all(sysname,DevList(1));
							if(rescount=0) then
								tabname_cur:=sysname||'destinationtable';
								case_res:=1;
								deviceid_first:=DevList(1);
								deviceid:=DevList(2);
							else
								case_res:=10;
								tabname_cur:='ERROR_IN_CREATING_DESTINTABLE';
							end if;
						else
							case_res:=10;
							tabname_cur:='ERROR_IN_CREATING_DESTINTABLE';
						end if;
					end if;
				end if;
			else
				tabname_cur:=sysname||'destinationtable';
				case_res:=1;
			end if;
		else
			if(case_res=-3) then
				rescount:=routingtable_pck_view.createTD_all(sysname,DevList(1));
				if(rescount=0) then
					tabname_cur:=sysname||'destinationtable';
					case_res:=1;
					deviceid_first:=DevList(1);
					deviceid:=DevList(2);
				else
					case_res:=10;
					tabname_cur:='ERROR_IN_CREATING_DESTINTABLE';
				end if;
			end if;
		end if;
	end if;
	if(case_res=1) then
		if(deviceid=0) then
			execute immediate 'select count(*) from '||tabname_cur||' where nodeid_start0=:devfrom and nodeid_end1=:devto and rownum=1' into rescount using DevList(2), DevList(1);
		else
			execute immediate 'select count(*) from '||tabname_cur||' where nodeid_start0=:devfrom and nodeid_end1=:devto and rownum=1' into rescount using DevList(1), DevList(2);
		end if;
		if(rescount=0) then 
			case_res:=-10; 
			tabname_cur:='ERROR_NO_CONNECTIVITY';
		else
			case_res:=2; --means we find the connect
			if(deviceid=0) then
				deviceid_first:=DevList(2);
				deviceid:=DevList(1);
			else
				deviceid_first:=DevList(1);
				deviceid:=DevList(2);
			end if;
		end if;
	end if;
	DBMS_OUTPUT.PUT_LINE ('time execution to create the routing table '|| (DBMS_UTILITY.GET_TIME-time_before)/100);
	if(case_res=2) then
		if(instr(tabname_cur,'destination')>0) then
			open cursorC1 for 'select pathid,port_nb,port_nb_destin from '||tabname_cur||'  where nodeid_start0=:1 and nodeid_end1=:2' using deviceid_first,deviceid;
		else
			open cursorC1 for 'select f.pathid,f.port_nb,t.port_nbr from '||tabname_cur||' f,'||sysname||'ip_ethernet_view r,'||sysname||'port_properties_view t where f.nodeid_start0=:1 and f.nodeid_end1=:2 and f.ETHERNET_ADD_DESTIN=r.ethernet_add and r.portid=t.portid' using deviceid_first,deviceid;
		end if;
	else
		open cursorC1 for select -1,-1,-3 from dual ;
	end if;
end if;

exception
when NO_DATA_FOUND then
if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
dbms_sql.close_cursor(source_cursor);
end if;
close cursorC1;
open cursorC1 for select -1,-1,-4 from dual ;
when OTHERS then
if DBMS_SQL.IS_OPEN (source_cursor)=TRUE then
dbms_sql.close_cursor(source_cursor);
end if;
close cursorC1;
open cursorC1 for 'select -1,-1,'||sqlcode||' from dual' ;
end GiveDestinTableName_temp_view;
/


