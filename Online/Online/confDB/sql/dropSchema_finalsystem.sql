create or replace function DropTableSchema(dummy number) return number AUTHID CURRENT_USER is
resfct number;
tabname varchar2(100);
fdbk number;
tabname_cur varchar2(100);
nbtab_dropd number:=0;
nbtab_dropd1 number:=0;
nbtab_dropd2 number:=0;
i number;
cursor mycur is select SEQUENCE_NAME from user_sequences where SEQUENCE_NAME like 'LHCB_%';
--source_cursor number:=dbms_sql.open_cursor;
--system_name varchar2:='all_';
g number :=0;
TYPE element_list IS TABLE OF VARCHAR2(50);
tabname_list element_list:=element_list('lhcb_subsystemList','lhcb_lg_device_types','lhcb_hw_devices','lhcb_lg_devices','lhcb_device_history','lhcb_hw_cpnts','lhcb_lg_cpnts','lhcb_cpnt_history','lhcb_link_types','lhcb_microscopic_connectivity','lhcb_prime_number','lhcb_ipinfo','lhcb_port_properties','lhcb_macroscopic_connectivity','LHCB_MACRO_CONNEC_TEMP','lhcb_device_booting','lhcb_devicetype_booting');
sysname_list element_list:=element_list('ECAL','HCAL','PRS','RICH1','RICH2','VELO','OT','IT','TT','MUON','DAQ','TFC','ECS','PUS','L0MUON','L0CALO','L0DU','CALO','MU_L0');

begin
	open mycur;
	loop
		fetch mycur into tabname;
		exit when mycur%notfound;
		execute immediate 'drop sequence '||tabname;
	end loop;

close mycur;


	for i in sysname_list.first..sysname_list.last loop
		tabname:=upper('lhcb_'||sysname_list(i)||'_path_temp');
		execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into g using tabname;
		if g=1 then
		execute immediate 'drop  table lhcb_'||sysname_list(i)||'_path_temp ';
		end if;
		


	tabname:=upper('lhcb_'||sysname_list(i)||'_pdetwithports');
		execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into g using tabname;
	
		if g=1 then
		execute immediate 'drop table lhcb_'||sysname_list(i)||'_pdetwithports';
		end if;


	tabname:=upper('lhcb_'||sysname_list(i)||'_pdetwithlkids');
		execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into g using tabname;

		if g=1 then
		execute immediate 'drop table lhcb_'||sysname_list(i)||'_pdetwithlkids';
		end if;


tabname:=upper('lhcb_'||sysname_list(i)||'_destinationtable');
		execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into g using tabname;
	
		if g=1 then
		execute immediate 'drop table lhcb_'||sysname_list(i)||'_destinationtable';
		end if;


tabname:=upper('lhcb_'||sysname_list(i)||'_pdet_temp');
		execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into g using tabname;

		if g=1 then
		execute immediate 'drop table lhcb_'||sysname_list(i)||'_pdet_temp';
		end if;


tabname:=upper('lhcb_'||sysname_list(i)||'_startend_temp');
		execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into g using tabname;

		if g=1 then
		execute immediate 'drop  table lhcb_'||sysname_list(i)||'_startend_temp';
		end if;



		tabname:=upper('lhcb_'||sysname_list(i)||'_destintab_temp');
		execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into g using tabname;

			if g=1 then
		execute immediate 'drop table lhcb_'||sysname_list(i)||'_destintab_temp';
		end if;

			tabname:=upper('lhcb_'||sysname_list(i)||'_linkpairs');
		execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into g using tabname;
		if g=1 then
		execute immediate 'drop table lhcb_'||sysname_list(i)||'_linkpairs';
		end if;


		tabname:=upper('lhcb_'||sysname_list(i)||'_lg_links');
		execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into g using tabname;
		if g=1 then 
		execute immediate 'drop table lhcb_'||sysname_list(i)||'_lg_links';
		end if;


		tabname:=upper('lhcb_'||sysname_list(i)||'_pdetails');
		execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into g using tabname;
		if g=1 then
		execute immediate 'drop table lhcb_'||sysname_list(i)||'_pdetails ';
		end if;
	end loop;

	for i in reverse tabname_list.first..tabname_list.last loop
		tabname:=upper(tabname_list(i));
		execute immediate 'select count(table_name) from user_tables where table_name=upper(:tab1)' into g using tabname;
		--dbms_output.put_line('value of tabname '||tabname);
		if g=1 then
		execute immediate 'drop table '||tabname;
		end if;
	end loop;

	

	

resfct:=0;
return resfct;
exception
when NO_DATA_FOUND then
resfct:=-1;
return resfct;
when OTHERS then
resfct:=sqlcode;
return resfct;	
end DropTableSchema;



