--don't forget to delete in the right order pathdetails last one...
create or replace function CreateTableSchema(dummy number) return number AUTHID CURRENT_USER is
resfct number:=-1;
nbtab_created number:=0;
nbtab_created1 number:=0;
nbtab_created2 number:=0;
TYPE devid_list IS TABLE OF NUMBER;
prime_nbList devid_list:=devid_list(2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,197,199,211,223,227,229,233,239,241,251,257,263,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,487,491,499,503);
i number :=0;
TYPE element_list IS TABLE OF VARCHAR2(50);
sysname1_list element_list:=element_list('TFC','DAQ','VELO','VELO_A','VELO_C','RICH','RICH1','RICH2','OT_A','OT','OT_C','ST','IT','TT','HCAL','ECAL','L0CALO','PRS','L0DU','MUON','L0MUON','MUON_A','MUON_C','L0TRIGGER','PUS');
sysname_list devid_list:=devid_list(3,5,7,77,91,17,323,391,899,29,1073,41,1763,1927,59,53,7031,61,7979,67,6557,4757,4891,79,7663);

--seqname_list element_list:=element_list('lhcb_devtype_seq','lhcb_lg_deviceseq','lhcb_history_deviceseq','lhcb_lg_cpnts_deviceid_seq','lhcb_cpnt_history_comptid_seq','lhcb_microconnseq','lhcb_lktypeseq','IT','TT','MUON','DAQ','TFC','ECS','PUS','L0MUON','L0CALO','L0DU');

begin
	execute immediate 'create table lhcb_subsystemList(system_name varchar2(50),systemID number, constraint systemID_pk primary key(systemID))';
	execute immediate 'alter table lhcb_subsystemList add constraint lhcb_subsystemList_sysname_uni unique(system_name)';
	for i in sysname1_list.first .. sysname1_list.last loop
		execute immediate '	insert into lhcb_subsystemList values (:1,:2)' using sysname1_list(i),sysname_list(i);
	end loop;
	


	select count(tablespace_name) into resfct from user_segments where tablespace_name='INDX01' and rownum=1;
	execute immediate 'create table lhcb_lg_device_types(devicetypeID number,DEVICETYPE varchar2(100),NBROFINPUT number default -1,NBROFOUTPUT number default -1,DESCRIPTION varchar2(500),system_name number,created date default sysdate,author varchar2(200),terminal_name varchar2(200), last_update date default sysdate, user_update varchar2(100),RGBcolor varchar2(12),constraint lhcb_devtype_pk primary key(devicetypeID))';
	execute immediate 'alter table lhcb_lg_device_types add constraint DEVICEtype_UNI1 unique(devicetype)';
	
	execute immediate 'create sequence lhcb_devtype_seq minvalue 0 start with 1 increment by 1';
	
	-- system_name is absent because hw view we put everything, even the spares one which can be common to all the subsystems such tell1
	execute immediate 'create table lhcb_hw_devices(serialnb varchar2(500),device_status number NOT NULL,responsible varchar2(100), user_comments varchar2(1000),hwname varchar2(100),hwtype varchar2(200),location varchar2(200),constraint lhcb_serialnb_pk primary key(serialnb))';

	execute immediate 'create table lhcb_lg_devices(DEVICENAME varchar2(100) NOT NULL,DEVICETYPEID number,ACTIVE number default 0,NODE number NOT NULL,nodeused number default 1,PROMISCUOUS_MODE number default 1,serialnb varchar2(500),location varchar2(200),DEVICEID number,functionID number default 0,system_name number,created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date default sysdate, user_update varchar2(100), constraint lhcb_lg_device_pk primary key(deviceid))';
	execute immediate 'create sequence lhcb_lg_deviceseq minvalue 0 start with 1 increment by 1';
    execute immediate 'insert into lhcb_lg_devices(devicename,node,deviceid) values (''none'',0,-1)';
	-- for the moment we presume that the lg devicename is unique in all the subsystems
	-- if it turns out, that it's not we will put a unique constraint on devicename,systemname
   execute immediate 'alter table lhcb_lg_devices add constraint lhcb_fctID_ck check (functionID>=0)';
--alter table lhcb_lg_devices modify (functionID number default 0);
	execute immediate 'create table lhcb_device_functions (functionID number, function_name varchar2(200), created date default sysdate, author varchar2(200),terminal_name varchar2(200),user_update varchar2(200), last_update date default sysdate, constraint lhcb_device_fct_pk primary key(functionID))';
	execute immediate 'alter table lhcb_device_functions add constraint lhcb_device_functions_UNI1 unique(function_name)';
	-- view to check that the snb is unique when not null
	execute immediate 'create view lhcb_snb_view as select serialnb from lhcb_lg_devices where serialnb is not null';
	execute immediate 'alter view lhcb_snb_view add constraint lhcb_snb_view_snb unique(serialnb)';
	execute immediate 'alter table lhcb_lg_devices add constraint lhcb_lg_DEVICE_UNI1 unique(devicename)';

	execute immediate 'alter table lhcb_lg_devices add constraint lhcb_lg_device_devtype_fk foreign key(devicetypeID) references lhcb_lg_device_types(devicetypeID)';
	--execute immediate 'alter table lhcb_lg_devices add constraint lhcb_lg_device_serialnb_uni unique(serialnb)';
	execute immediate 'alter table lhcb_lg_devices add constraint lhcb_lg_device_nodeused_ck check(active<nodeused+1)';
	execute immediate 'alter table lhcb_lg_devices add constraint lhcb_lg_device_serialnb_fk foreign key(serialnb) references lhcb_hw_devices(serialnb)';
	--for moment we don't partition this table: but in the future we will have to.
	execute immediate 'create table lhcb_device_history(historydeviceID number,serialnb varchar2(500),deviceid number,device_STATUS varchar2(50),user_comment varchar2(1000), location varchar2(200),status_change date,system_name varchar2(100),created date,author varchar2(200),terminal_name varchar2(200), constraint lhcb_devicehist_pk primary key(historydeviceid))';
	execute immediate 'create sequence lhcb_history_deviceseq minvalue 0 start with 1 increment by 1';

	execute immediate 'alter table lhcb_device_history add constraint lhcb_devicehist_dev_fk foreign key(deviceID) references lhcb_lg_devices(deviceID)';
	execute immediate 'alter table lhcb_device_history add constraint lhcb_devicehist_serialnb_fk foreign key(serialnb) references lhcb_hw_devices(serialnb)';

	if(resfct=1) then
		execute immediate 'CREATE INDEX lhcb_devupdatehist_idx ON lhcb_device_history (deviceID,status_change) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_devhist_idx ON lhcb_device_history (deviceID) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_serialupdatehist_idx ON lhcb_device_history (serialnb,status_change) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_serialhist_idx ON lhcb_device_history (serialnb) tablespace INDX01';
		execute immediate 'create index lhcb_dtypeID_idx on lhcb_lg_devices(devicetypeID) tablespace INDX01';
		execute immediate 'create index lhcb_lgdevsnb_idx on lhcb_lg_devices(serialnb) tablespace INDX01';
		execute immediate 'create index lhcb_lgdevloc_idx on lhcb_lg_devices(location) tablespace INDX01';
		execute immediate 'create index lhcb_snbloc_idx on lhcb_hw_devices(location) tablespace INDX01';
		execute immediate 'create index lhcb_snbhwtype_idx on lhcb_hw_devices(hwtype) tablespace INDX01';
		execute immediate 'create index lhcb_snbdstatus_idx on lhcb_hw_devices(device_status) tablespace INDX01';

	else
		execute immediate 'CREATE INDEX lhcb_devupdatehist_idx ON lhcb_device_history (deviceID,status_change)';
		execute immediate 'CREATE INDEX lhcb_devhist_idx ON lhcb_device_history (deviceID)';
		execute immediate 'CREATE INDEX lhcb_serialupdatehist_idx ON lhcb_device_history (serialnb,status_change)';
		execute immediate 'CREATE INDEX lhcb_serialhist_idx ON lhcb_device_history (serialnb)';
		execute immediate 'create index lhcb_dtypeID_idx on lhcb_lg_devices(devicetypeID)';
		execute immediate 'create index lhcb_lgdevsnb_idx on lhcb_lg_devices(serialnb) ';
		execute immediate 'create index lhcb_lgdevloc_idx on lhcb_lg_devices(location) ';
		execute immediate 'create index lhcb_snbloc_idx on lhcb_hw_devices(location) ';
		execute immediate 'create index lhcb_snbhwtype_idx on lhcb_hw_devices(hwtype) ';
		execute immediate 'create index lhcb_snbdstatus_idx on lhcb_hw_devices(device_status) '; 
	end if;


	
	execute immediate 'create table lhcb_hw_cpnts(snbid number,serialnb varchar2(500),cpnt_status number NOT NULL,replacable number,responsible varchar2(100), user_comment varchar2(1000),hwname varchar2(100) NOT NULL,hwtype varchar2(200),location varchar2(200),constraint lhcb_compserialnb_pk primary key(snbid))';
execute immediate 'insert into lhcb_hw_cpnts (hwname,snbid,serialnb,cpnt_status) values (''none'',-1,''none'',-1)';

	execute immediate 'create table lhcb_lg_cpnts(cpntname varchar2(100) NOT NULL,cpntType varchar2(100),snbid number,motherboardID number,cpntID number,location varchar2(100),system_name number,created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date default sysdate, user_update varchar2(100), constraint lhcb_lg_cmpnt_pk primary key(cpntid))';
	execute immediate 'create sequence lhcb_lg_cpnts_deviceid_seq minvalue 0 start with 1 increment by 1';
	execute immediate 'create sequence lhcb_hw_cpnts_snbid_seq minvalue 0 start with 1 increment by 1';
	execute immediate 'alter table lhcb_lg_cpnts add constraint lhcb_lg_cpnt_dID_fk foreign key(motherboardID) references lhcb_lg_devices(deviceID)';
	execute immediate 'alter table lhcb_lg_cpnts add constraint lhcb_lg_cpnt_snb_fk foreign key(snbid) references lhcb_hw_cpnts(snbid)';
	execute immediate 'insert into lhcb_lg_cpnts(cpntname,cpntID) values (''motherboard'',-1)';

execute immediate '	create table lhcb_cpnt_history(historycpntID number,snbid number,cpntid number,cpnt_STATUS varchar2(50),user_comment varchar2(1000), location varchar2(200),status_change date default sysdate,system_name varchar2(100),created date default sysdate,author varchar2(200),terminal_name varchar2(200), constraint lhcb_cpnthist_pk primary key(historycpntid))';

execute immediate '	create sequence lhcb_cpnt_history_comptid_seq minvalue 0 start with 1 increment by 1';
execute immediate '	alter table lhcb_cpnt_history add constraint lhcb_comphist_dev_fk foreign key(cpntID) references lhcb_lg_cpnts(cpntID)';
execute immediate '	alter table lhcb_cpnt_history add constraint lhcb_comphist_snb_fk foreign key(snbid) references lhcb_hw_cpnts(snbid)';

execute immediate 'create table lhcb_link_types(linktypeID number,link_nbr number ,link_name varchar2(20) NOT NULL,created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date default sysdate,user_update varchar2(100))';
execute immediate '	alter table lhcb_link_types add constraint lktype_pk primary key(linktypeID)';
execute immediate '	alter table lhcb_link_types add constraint lktype_lknameuni unique(link_name)';
execute immediate '	alter table lhcb_link_types add constraint lktype_lknbuni unique(link_nbr)';
execute immediate '	create sequence lhcb_lktypeseq minvalue 0 start with 1 increment by 1';

		--table used to get prime number list
	execute immediate 'create  table lhcb_prime_number (prime_nb number, prime_nb_position number,last_update date default sysdate,user_update varchar2(100),constraint primenb_pk primary key(prime_nb), constraint lhcb_primenb_uni unique(prime_nb_position))';
	--need to fill the table with prime numbers (93) -----------
	for i in 1..93 loop
		execute immediate 'insert into lhcb_prime_number(prime_nb,prime_nb_position) values (:1,:2)' using prime_nbList(i),i;
	end loop;	

	
	execute immediate 'create table lhcb_microscopic_connectivity(linkid number,cpntIDfrom number,portidfrom number,cpntIDto number,portIDto number,link_type number,bidirectional_link_used number default 0,link_weight number default 1,lkused number,created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date default sysdate,user_update varchar2(100))';
	execute immediate 'create sequence lhcb_microconnseq minvalue 0 start with 1 increment by 1';
	execute immediate 'alter table lhcb_microscopic_connectivity add constraint lhcb_conn_pk primary key(linkid)';
execute immediate '	alter table lhcb_microscopic_connectivity add constraint lhcb_conn_ptidfrom_fk foreign key(cpntIDfrom) references lhcb_lg_cpnts(cpntID)';
execute immediate '	alter table lhcb_microscopic_connectivity add constraint lhcb_conn_ptidto_fk foreign key(cpntIDto) references lhcb_lg_cpnts(cpntID)';
execute immediate '	alter table lhcb_microscopic_connectivity add constraint lhcb_conn_lktype_fk foreign key(link_type) references lhcb_link_types(linktypeID)';
execute immediate '	alter table lhcb_microscopic_connectivity add constraint lhcb_conn_nfromnto_ck check (decode(cpntIDfrom,-1,-2,cpntIDfrom)!=cpntIDto)';
execute immediate '	alter table lhcb_microscopic_connectivity add constraint lhcb_conn_unipfrom_ck unique(cpntIDfrom,portidfrom)';
execute immediate '	alter table lhcb_microscopic_connectivity add constraint lhcb_conn_unipto_ck unique(cpntIDto,portidto)';

dbms_output.put_line('before indexes ');

	if(resfct=1) then
		execute immediate 'CREATE INDEX lhcb_cpntupdatehist_idx ON lhcb_cpnt_history (cpntID,status_change) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_cpnthist_idx ON lhcb_cpnt_history (cpntID) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_serialupdatehist1_idx ON lhcb_cpnt_history (snbid,status_change) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_serialhist1_idx ON lhcb_cpnt_history (snbid) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_snbid_idx ON lhcb_hw_cpnts (serialnb) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_location_idx ON lhcb_hw_cpnts (location) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_mboard_idx ON lhcb_lg_cpnts (motherboardid) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_mboard_idx ON lhcb_lg_cpnts (snbid) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_microconncptidfrom_idx ON lhcb_microscopic_connectivity (cpntidfrom) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_microconncptidto_idx ON lhcb_microscopic_connectivity (cpntidto) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_microconncptidto_idx ON lhcb_microscopic_connectivity (link_type) tablespace INDX01';

	else
		execute immediate 'CREATE INDEX lhcb_cpntupdatehist_idx ON lhcb_cpnt_history (cpntID,status_change) ';
		execute immediate 'CREATE INDEX lhcb_cpnthist_idx ON lhcb_cpnt_history (cpntID) ';
		execute immediate 'CREATE INDEX lhcb_serialupdatehist1_idx ON lhcb_cpnt_history (snbid,status_change) ';
		execute immediate 'CREATE INDEX lhcb_serialhist1_idx ON lhcb_cpnt_history (snbid) ';	
		execute immediate 'CREATE INDEX lhcb_snbid_idx ON lhcb_hw_cpnts (serialnb) ';
		execute immediate 'CREATE INDEX lhcb_location_idx ON lhcb_hw_cpnts (location) ';
		execute immediate 'CREATE INDEX lhcb_mboard_idx ON lhcb_lg_cpnts (motherboardid) ';
		execute immediate 'CREATE INDEX lhcb_mboard_idx ON lhcb_lg_cpnts (snbid) ';
		execute immediate 'CREATE INDEX lhcb_microconncptidto_idx ON lhcb_microscopic_connectivity (link_type) ';
		execute immediate 'CREATE INDEX lhcb_microconncptidfrom_idx ON lhcb_microscopic_connectivity (cpntidfrom) ';
		execute immediate 'CREATE INDEX lhcb_microconncptidto_idx ON lhcb_microscopic_connectivity (cpntidto) ';
	

	end if;
dbms_output.put_line('before ipinfo ');

execute immediate '	create table lhcb_ipinfo (ipaddress varchar2(15),SUBNET_info varchar2(15),ipname varchar2(100),zone_name varchar2(100) not null, created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date default sysdate)';
execute immediate '	alter table lhcb_ipinfo add constraint ipinfo_pk primary key(ipaddress)';
execute immediate '	alter table lhcb_ipinfo add constraint ipinfo_ipnameuni unique(ipname)';
	if(resfct=1) then
execute immediate '	CREATE INDEX lhcb_subnetid_idx ON lhcb_ipinfo (subnet_info)  tablespace INDX01';
	else
execute immediate '	CREATE INDEX lhcb_subnetid_idx ON lhcb_ipinfo (subnet_info)';
	end if;

execute immediate '	create table lhcb_ipaliases (ipaddress varchar2(15) ,ipalias varchar2(100),created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date default sysdate, constraint lhcb_ipalias_pk primary key(ipalias))';
execute immediate '	alter table lhcb_ipaliases add constraint lhcb_ipaddress_fk foreign key(ipaddress) reference lhcb_ipinfo(ipaddress)';
execute immediate 'CREATE INDEX lhcb_ipaliases_ipadd_idx ON lhcb_ipaliases (ipaddress)  ';

dbms_output.put_line('before port ppies ');
	
	execute immediate 'create table lhcb_port_properties (portid number,port_nbr varchar2(50),port_way number,administrative_status number default 1,speed number,port_type varchar2(100),phy varchar2(3) ,pxi_booting number,deviceid number,ipaddress varchar2(15),created date default sysdate,author varchar2(200),terminal_name varchar2(200),user_update  varchar2(200),last_update date default sysdate)';
	nbtab_created1:=nbtab_created1+1;
	dbms_output.put_line('before port ppies sequence');
	execute immediate 'create table lhcb_hwport_properties (serialnb varchar2(100),port_nbr varchar2(50),port_way number,port_type varchar2(100),bia varchar2(18),macaddress varchar2(18),created date default sysdate,author varchar2(200),terminal_name varchar2(200),user_update  varchar2(200),last_update date default sysdate)';
	execute immediate 'alter table lhcb_hwport_properties add constraint lhcb_port_snb_fk foreign key(serialnb) references lhcb_hw_devices(serialnb)';

	execute immediate 'create sequence lhcb_portseq minvalue 0 start with 1 increment by 1';
	execute immediate 'alter table lhcb_port_properties add constraint lhcb_port_pk primary key(portid)';
	execute immediate 'alter table lhcb_port_properties add constraint lhcb_port_ipadd_fk foreign key(ipaddress) references lhcb_ipinfo(ipaddress)';

	execute immediate 'alter table lhcb_port_properties add constraint lhcb_port_devid_fk foreign key(deviceid) references lhcb_lg_devices(deviceid)';
	execute immediate 'alter table lhcb_port_properties add constraint lhcb_port_uni unique(deviceid,port_nbr,port_type,port_way)';
	execute immediate 'alter table lhcb_port_properties add constraint lhcb_port_phy_ck check(upper(phy) in (''SX'',''SL'',''T''))';
	if(resfct=1) then
		execute immediate 'CREATE INDEX lhcb_portppdevid_idx ON lhcb_port_properties (deviceid)  tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_portppdipadd_idx ON lhcb_port_properties (ipaddress)  tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_hwportppsnb_idx ON lhcb_hwport_properties (serialnb)  tablespace INDX01';

	else
		execute immediate 'CREATE INDEX lhcb_portppdevid_idx ON lhcb_port_properties (deviceid)';
		execute immediate 'CREATE INDEX lhcb_portppdipadd_idx ON lhcb_port_properties (ipaddress)';
		execute immediate 'CREATE INDEX lhcb_hwportppsnb_idx ON lhcb_hwport_properties (serialnb)';

	end if;

execute immediate '	create table lhcb_macroscopic_connectivity(linkid number,portIDfrom number,portIDto number,link_type number,bidirectional_link_used number default 0,link_weight number,LKUSED number default 0,system_name number,link_info varchar2(1000),created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date default sysdate,user_update varchar2(100))';
execute immediate '	create sequence lhcb_macroconnseq minvalue 0 start with 1 increment by 1';
execute immediate '	alter table lhcb_macroscopic_connectivity add constraint lhcb_maconn_pk primary key(linkid)';
execute immediate '	alter table lhcb_macroscopic_connectivity add constraint lhcb_maconn_ptidfrom_fk foreign key(portIDfrom) references lhcb_port_properties(portID)';
execute immediate '	alter table lhcb_macroscopic_connectivity add constraint lhcb_maconn_ptidto_fk foreign key(portIDto) references lhcb_port_properties(portID)';
execute immediate '	alter table lhcb_macroscopic_connectivity add constraint lhcb_maconn_sfrom_uni unique(portIDfrom)';
execute immediate '	alter table lhcb_macroscopic_connectivity add constraint lhcb_maconn_sto_uni unique(portIDto)';
execute immediate '	alter table lhcb_macroscopic_connectivity add constraint lhcb_maconn_lktype_fk foreign key(link_type) references lhcb_link_types(linktypeID)';
execute immediate '	alter table lhcb_macroscopic_connectivity add constraint lhcb_maconn_nfromnto_ck check (portIDfrom!=portIDto)';
	
execute immediate '	alter table lhcb_macroscopic_connectivity  add constraint macro_sysname_ck check(system_name!=-1)';

	
	if(resfct=1) then
		execute immediate '	create index lhcb_maconnect_lastupdate on lhcb_macroscopic_CONNECTIVITY(last_update) tablespace INDX01';
	execute immediate '	create index lhcb_LKTYPE_IDX on lhcb_macroscopic_CONNECTIVITY(link_type) tablespace INDX01';
	execute immediate '	create index lhcb_LKused_IDX on lhcb_macroscopic_CONNECTIVITY(lkused) tablespace INDX01';

	else
		execute immediate '	create index lhcb_maconnect_lastupdate on lhcb_macroscopic_CONNECTIVITY(last_update)';
		execute immediate '	create index lhcb_LKTYPE_IDX on lhcb_macroscopic_CONNECTIVITY(link_type) ';
		execute immediate '	create index lhcb_LKused_IDX on lhcb_macroscopic_CONNECTIVITY(lkused) ';


	end if;

dbms_output.put_line('before lhcb_macro_connec_temp ');

execute immediate 'create global temporary table lhcb_macro_connec_temp(nodeID_from number,port_nbrfrom number,nodeID_to number,port_nbrto number,link_type number,bidirectional_link_used number,link_weight number,lkused number,system_name number,created date, author varchar2(200),terminal_name varchar2(200),last_update date ,user_update varchar2(200)) on commit preserve rows';


dbms_output.put_line('before device booting ');

execute immediate '	create table lhcb_device_booting(deviceid number,KERNEL_IMAGE_LOCATION varchar2(500), INITRD_IMAGE_LOCATION varchar2(500), PHYSICAL_LOCATION varchar2(500),boot_image_location varchar2(500),boot_protocol varchar2(100),created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date,user_update varchar2(100), constraint lhcb_pc_device_pk primary key(deviceid))';
dbms_output.put_line('before device type booting ');

execute immediate '	create table lhcb_devicetype_booting(devicetypeID number,KERNEL_IMAGE_LOCATION varchar2(500), INITRD_IMAGE_LOCATION varchar2(500), PHYSICAL_LOCATION varchar2(500),boot_image_location varchar2(500),boot_protocol varchar2(100),created date  default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date, user_update varchar2(100), constraint lhcb_pc_devtype_pk primary key(devicetypeID))';
execute immediate '	alter table lhcb_devicetype_booting add constraint lhcb_pc_devtype_fk foreign key(devicetypeID) references lhcb_lg_device_types (devicetypeID)';
execute immediate '	alter table lhcb_device_booting add constraint lhcb_pc_device_fk foreign key(deviceid) references lhcb_lg_devices (deviceid)';	




		
	--automatic table are by subsystems:
	
		execute immediate 'create global temporary table lhcb_path_temp (pathid number, link_pos number,nfrom number,pfrom varchar2(100), port_way1 number,nto number, pto varchar2(100),port_way2 number) on commit delete rows';
		execute immediate 'create global temporary table lhcb_linkpairs(link_weight number ,node1 number,node2 number,node3 number,lktype number ,lkused number,system_name number ) on commit preserve rows'; 
		execute immediate 'create global temporary table lhcb_lg_links (linkway number, node_from number,node_to number,link_weight number,lktype number,lkused number,system_name number) on commit preserve rows';
	

		execute immediate 'create global temporary table lhcb_micropath_temp (pathid number, link_pos number,nfrom varchar2(500), nto varchar2(500)) on commit delete rows';
	
		execute immediate 'create global temporary table lhcb_microlinkpairs(link_weight number ,node1 number,node2 number,node3 number,lktype number ,lkused number ) on commit preserve rows'; 
		execute immediate 'create global temporary table lhcb_microlg_links (linkway number, node_from number,node_to number,link_weight number,lktype number,lkused number) on commit preserve rows';
	

		execute immediate 'create table lhcb_pdetails (pathid number,path_weight number,path_type number,pathused number, node1 number default 0,node2 number default 0,node3 number default 0,node4 number default 0,node5 number default 0,node6 number default 0,node7 number default 0,node8 number default 0,node9 number default 0,node10 number default 0,node11 number default 0,last_update date,path_necessary number default 0,system_name number,constraint lhcb_pathdetails_1_pk primary key(node1,pathid))';
		execute immediate 'create table lhcb_micropdetails (pathid number,path_weight number,path_type number,pathused number, node1 number default 0,node2 number default 0,node3 number default 0,node4 number default 0,node5 number default 0,node6 number default 0,node7 number default 0,node8 number default 0,node9 number default 0,node10 number default 0,node11 number default 0,last_update date,path_necessary number default 0,system_name number,constraint lhcb_micropathdetails_1_pk primary key(node1,pathid))';
		
		execute immediate 'alter table lhcb_pdetails add constraint lhcb_pneces_ck check (path_necessary in (0,1))';

		execute immediate 'create table lhcb_pdetwithports(pathid number,link_pos number,nfrom number,pfrom number,nto number,pto number,node1 number,system_name number)';
		execute immediate 'create table lhcb_pdetwithlkids(pathid number,link_pos number,linkid number,node1 number,system_name number)';
		execute immediate 'alter table  lhcb_pdetwithports add constraint lhcb_PDETWITHPORTS_PK primary key(pathid, node1,nfrom,pfrom)';
		execute immediate 'alter table  lhcb_pdetwithlkids add constraint lhcb_PDETWITHlkidS_PK primary key(pathid, node1,linkid)';
		execute immediate 'alter table  lhcb_pdetwithports add constraint lhcb_PDETWITHPORTS_fk1 foreign key(node1,pathid) references lhcb_pdetails(node1,pathid)';
		execute immediate 'alter table  lhcb_pdetwithlkids add constraint lhcb_PDETWITHLKIDS_fk1 foreign key(node1,pathid) references lhcb_pdetails(node1,pathid)';
	--execute immediate 'create view '||system_name||'pathdetailswithnode_view as select t.node1 as node1,t.pathid as pathid,t.link_pos as link_pos ,case when t.linkid >0 then g.devicename else f.devicename end as nfrom,case when t.linkid >0 then p.port_nbrfrom else p.port_nbrto end as pfrom,case when t.linkid >0 then f.devicename else g.devicename end as nto,case when t.linkid >0 then p.port_nbrto else p.port_nbrfrom end as pto from '||system_name||'pathdetailswithlkids t,'||system_name||'device g, '||system_name||'device f, '||system_name||'connectivity p where p.linkid=t.linkid and p.node_from=g.deviceid and p.node_to=f.deviceid';


		if(resfct=1) then
			execute immediate 'CREATE INDEX lhcb_pathdevid1_idx ON lhcb_pdetails (node1) tablespace INDX01';
			execute immediate 'CREATE INDEX lhcb_pathdevid11_idx ON lhcb_pdetails (node11) tablespace INDX01';
			execute immediate 'create index lhcb_pathd_lastup on lhcb_PDETAILs(last_update) tablespace INDX01';
			
			execute immediate 'CREATE INDEX lhcb_micropathdevid1_idx ON lhcb_micropdetails (node1) tablespace INDX01';
			execute immediate 'CREATE INDEX lhcb_micropathdevid11_idx ON lhcb_micropdetails (node11) tablespace INDX01';
			execute immediate 'create index lhcb_micropathd_lastup on lhcb_microPDETAILs(last_update) tablespace INDX01';
			
			execute immediate 'create index lhcb_pdetlkids_i1 on lhcb_PDETWITHLKIDS(node1,pathid) tablespace INDX01';
			execute immediate 'create index lhcb_pdetlkids_i2 on lhcb_PDETWITHLKIDS(node1) tablespace INDX01';

			execute immediate 'create index lhcb_pdetports_i1 on lhcb_PDETWITHPORTS(node1,pathid) tablespace INDX01';
			execute immediate 'create index lhcb_pdetports_i2 on lhcb_PDETWITHPORTS(node1) tablespace INDX01';
		else
					execute immediate 'CREATE INDEX lhcb_micropathdevid1_idx ON lhcb_micropdetails (node1) ';
			execute immediate 'CREATE INDEX lhcb_micropathdevid11_idx ON lhcb_micropdetails (node11) ';
			execute immediate 'create index lhcb_micropathd_lastup on lhcb_microPDETAILs(last_update) ';
	execute immediate 'create index lhcb_pdetlkids_i1 on lhcb_PDETWITHLKIDS(node1,pathid)';
			execute immediate 'create index lhcb_pdetlkids_i2 on lhcb_PDETWITHLKIDS(node1)';

			execute immediate 'create index lhcb_pdetports_i1 on lhcb_PDETWITHPORTS(node1,pathid)';
			execute immediate 'create index lhcb_pdetports_i2 on lhcb_PDETWITHPORTS(node1)';
		end if;
		execute immediate 'create table lhcb_destinationtable(pathid number,pathused number,nodeid_start0 number,pfromid0 number,ptoid1 number,nodeid_end1 number,path_type number,system_name number,path_length number,last_update date, constraint lhcb_dest_name_pk primary key(pfromid0,pathid))';
	--	execute immediate 'alter table lhcb_destinationtable add constraint lhcb_destab_pathfk foreign key(nodeid_start0,pathid) references lhcb_pdetails (node1,pathid)';
		execute immediate 'alter table lhcb_destinationtable add constraint lhcb_destab_nids0fk foreign key(nodeid_start0) references lhcb_lg_devices (deviceid)';
		execute immediate 'alter table lhcb_destinationtable add constraint lhcb_destab_nide1fk foreign key(nodeid_end1) references lhcb_lg_devices(deviceid)';
		
		execute immediate 'create table lhcb_routingtable(pathid number,pathused number,routingpathused number,nodeid_start0 number,pfromid0 number,ptoid0 number,ptoid1 number,nodeid_end1 number,path_type number,system_name number, path_length number,last_update date, constraint lhcb_routing_name_pk primary key(pfromid0,pathid))';
		execute immediate 'create global temporary table lhcb_routingtable_temp(pathid number,pathused number,routingpathused number,nodeid_start0 number,pfromid0 number,ptoid0 number,ptoid1 number,nodeid_end1 number) on commit delete rows';
	
	--	execute immediate 'alter table lhcb_routingtable add constraint lhcb_routingtab_pathfk foreign key(nodeid_start0,pathid) references lhcb_pdetails (node1,pathid)';
		execute immediate 'alter table lhcb_routingtable add constraint lhcb_routingtab_nids0fk foreign key(nodeid_start0) references lhcb_lg_devices (deviceid)';
		execute immediate 'alter table lhcb_routingtable add constraint lhcb_routingtab_nide1fk foreign key(nodeid_end1) references lhcb_lg_devices(deviceid)';
	
		if(resfct=1) then
			execute immediate 'CREATE INDEX lhcb_destid_idx ON lhcb_destinationtable (nodeid_end1)  tablespace INDX01';
			execute immediate 'CREATE INDEX lhcb_destdevid_idx ON lhcb_destinationtable (nodeid_start0)  tablespace INDX01';
			execute immediate 'CREATE INDEX lhcb_routingtid_idx ON lhcb_routingtable (nodeid_end1)  tablespace INDX01';
			execute immediate 'CREATE INDEX lhcb_routingdevid_idx ON lhcb_routingtable (nodeid_start0)  tablespace INDX01';

		else
			execute immediate 'CREATE INDEX lhcb_destid_idx ON lhcb_destinationtable (nodeid_end1)  ';
			execute immediate 'CREATE INDEX lhcb_destdevid_idx ON lhcb_destinationtable (nodeid_start0)  ';
			execute immediate 'CREATE INDEX lhcb_routingtid_idx ON lhcb_routingtable (nodeid_end1)  ';
			execute immediate 'CREATE INDEX lhcb_routingdevid_idx ON lhcb_routingtable (nodeid_start0) ';

		end if;
		execute immediate 'create global temporary table lhcb_pdet_temp(pathid number,path_weight number,path_type number,pathused number, node1 number default 0,node2 number default 0,node3 number default 0,node4 number default 0,node5 number default 0,node6 number default 0,node7 number default 0,node8 number default 0,node9 number default 0,node10 number default 0,node11 number default 0,system_name number) on commit delete rows';
		execute immediate 'create global temporary table lhcb_startend_temp(pathid number,pathused number,nodeid_start0 number,nodeid_start1 number,nodeid_end0 number,nodeid_end1 number,path_type number, path_length number,system_name number) on commit delete rows';
		execute immediate 'create global temporary table lhcb_destintab_temp (pathid number,pathused number,nodeid_start0 number,pfromid0 number,ptoid1 number,nodeid_end1 number) on commit delete rows';
		execute immediate 'create global temporary table lhcb_micropdet_temp(pathid number,path_weight number,path_type number,pathused number, node1 number default 0,node2 number default 0,node3 number default 0,node4 number default 0,node5 number default 0,node6 number default 0,node7 number default 0,node8 number default 0,node9 number default 0,node10 number default 0,node11 number default 0) on commit delete rows';
	
	--end of first part of automatic tables






-------------------------------------------------------------------------------------

resfct:=0;
return 0;
exception
when NO_DATA_FOUND then
resfct:=-1;
--resfct:=droptableschema();
return -1;
when OTHERS then
resfct:=sqlcode;
--nbtab_created :=droptableschema();
return resfct;	
end createtableschema;
/


