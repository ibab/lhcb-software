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
sysname1_list element_list:=element_list('ECAL','HCAL','PRS','L0CALO','MUON','L0MUON','RICH1','RICH2','VELO','OT','IT','TT','DAQ','TFC','ECS','PUS','L0DU');
sysname_list element_list:=element_list('CALO','MU_L0','RICH1','RICH2','VELO','OT','IT','TT','DAQ','TFC','ECS','PUS','L0DU');

--seqname_list element_list:=element_list('lhcb_devtype_seq','lhcb_lg_deviceseq','lhcb_history_deviceseq','lhcb_lg_cpnts_deviceid_seq','lhcb_cpnt_history_comptid_seq','lhcb_microconnseq','lhcb_lktypeseq','IT','TT','MUON','DAQ','TFC','ECS','PUS','L0MUON','L0CALO','L0DU');

begin
	execute immediate 'create table lhcb_subsystemList(system_name varchar2(50),systemID number, constraint systemID_pk primary key(systemID))';
	for i in sysname1_list.first .. sysname1_list.last loop
		execute immediate '	insert into lhcb_subsystemList values (:1,:2)' using sysname1_list(i),prime_nbList(i);
	end loop;
		execute immediate '	insert into lhcb_subsystemList values (''CALO'',210)';
		execute immediate '	insert into lhcb_subsystemList values (''MU_L0'',143)';



	select count(tablespace_name) into resfct from user_segments where tablespace_name='INDX01' and rownum=1;
	execute immediate 'create table lhcb_lg_device_types(devicetypeID number,DEVICETYPE varchar2(100),NBROFINPUT number default -1,NBROFOUTPUT number default -1,DESCRIPTION varchar2(500),system_name number,created date default sysdate,author varchar2(200),terminal_name varchar2(200), last_update date default sysdate, user_update varchar2(100),RGBcolor varchar2(12),constraint lhcb_devtype_pk primary key(devicetypeID))';
	execute immediate 'alter table lhcb_lg_device_types add constraint DEVICEtype_UNI1 unique(devicetype)';
	
	execute immediate 'create sequence lhcb_devtype_seq minvalue 0 start with 1 increment by 1';
	
	-- system_name is absent because hw view we put everything, even the spares one which can be common to all the subsystems such tell1
	execute immediate 'create table lhcb_hw_devices(serialnb varchar2(500),device_status number NOT NULL,responsible varchar2(100), user_comments varchar2(1000),hwname varchar2(100),hwtype varchar2(200),location varchar2(200),constraint lhcb_serialnb_pk primary key(serialnb))';

	execute immediate 'create table lhcb_lg_devices(DEVICENAME varchar2(100) NOT NULL,DEVICETYPEID number,ACTIVE number default 0,NODE number NOT NULL,nodeused number default 1,PROMISCUOUS_MODE number default 1,serialnb varchar2(500),location varchar2(200),DEVICEID number,system_name number,created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date default sysdate, user_update varchar2(100), constraint lhcb_lg_device_pk primary key(deviceid))';
	execute immediate 'create sequence lhcb_lg_deviceseq minvalue 0 start with 1 increment by 1';

	-- for the moment we presume that the lg devicename is unique in all the subsystems
	-- if it turns out, that it's not we will put a unique constraint on devicename,systemname

	execute immediate 'alter table lhcb_lg_devices add constraint lhcb_lg_DEVICE_UNI1 unique(devicename)';
	execute immediate 'alter table lhcb_lg_devices add constraint lhcb_lg_device_devtype_fk foreign key(devicetypeID) references lhcb_lg_device_types(devicetypeID)';
	execute immediate 'alter table lhcb_lg_devices add constraint lhcb_lg_device_serialnb_uni unique(serialnb)';
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

	else
		execute immediate 'CREATE INDEX lhcb_devupdatehist_idx ON lhcb_device_history (deviceID,status_change)';
		execute immediate 'CREATE INDEX lhcb_devhist_idx ON lhcb_device_history (deviceID)';
		execute immediate 'CREATE INDEX lhcb_serialupdatehist_idx ON lhcb_device_history (serialnb,status_change)';
		execute immediate 'CREATE INDEX lhcb_serialhist_idx ON lhcb_device_history (serialnb)';
		execute immediate 'create index lhcb_dtypeID_idx on lhcb_lg_devices(devicetypeID)';
	end if;


	
	execute immediate 'create table lhcb_hw_cpnts(serialnb varchar2(500),cpnt_status number,responsible varchar2(100), user_comment varchar2(1000),hwname varchar2(100),hwtype varchar2(200),location varchar2(200),constraint lhcb_compserialnb_pk primary key(serialnb))';


	execute immediate 'create table lhcb_lg_cpnts(cpntname varchar2(100) NOT NULL,cpntType varchar2(100),serialnb varchar2(500),motherboardID number,cpntID number,system_name number,created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date default sysdate, user_update varchar2(100), constraint lhcb_lg_cmpnt_pk primary key(cpntid))';
	execute immediate 'create sequence lhcb_lg_cpnts_deviceid_seq minvalue 0 start with 1 increment by 1';
	execute immediate 'alter table lhcb_lg_cpnts add constraint lhcb_lg_cpnt_dID_fk foreign key(motherboardID) references lhcb_lg_devices(deviceID)';
	execute immediate 'alter table lhcb_lg_cpnts add constraint lhcb_lg_cpnt_snb_fk foreign key(serialnb) references lhcb_hw_cpnts(serialnb)';

	execute immediate 'insert into lhcb_lg_cpnts(cpntname,cpntID) values (''motherboard'',-1)';

execute immediate '	create table lhcb_cpnt_history(historycpntID number,serialnb varchar2(500),cpntid number,cpnt_STATUS varchar2(50),user_comment varchar2(1000), location varchar2(200),status_change date default sysdate,system_name varchar2(100),created date default sysdate,author varchar2(200),terminal_name varchar2(200), constraint lhcb_cpnthist_pk primary key(historycpntid))';

execute immediate '	create sequence lhcb_cpnt_history_comptid_seq minvalue 0 start with 1 increment by 1';
execute immediate '	alter table lhcb_cpnt_history add constraint lhcb_comphist_dev_fk foreign key(cpntID) references lhcb_lg_cpnts(cpntID)';
execute immediate '	alter table lhcb_cpnt_history add constraint lhcb_comphist_snb_fk foreign key(serialnb) references lhcb_hw_cpnts(serialnb)';

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

	
	execute immediate 'create table lhcb_microscopic_connectivity(linkid number,cpntIDfrom number,portnb_from number,cpntIDto number,portIDto number,link_type number,bidirectional_link_used number default 0,link_weight number default 1,created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date default sysdate,user_update varchar2(100))';
	execute immediate 'create sequence lhcb_microconnseq minvalue 0 start with 1 increment by 1';
	execute immediate 'alter table lhcb_microscopic_connectivity add constraint lhcb_conn_pk primary key(linkid)';
execute immediate '	alter table lhcb_microscopic_connectivity add constraint lhcb_conn_ptidfrom_fk foreign key(cpntIDfrom) references lhcb_lg_cpnts(cpntID)';
execute immediate '	alter table lhcb_microscopic_connectivity add constraint lhcb_conn_ptidto_fk foreign key(cpntIDto) references lhcb_lg_cpnts(cpntID)';
execute immediate '	alter table lhcb_microscopic_connectivity add constraint lhcb_conn_lktype_fk foreign key(link_type) references lhcb_link_types(linktypeID)';
execute immediate '	alter table lhcb_microscopic_connectivity add constraint lhcb_conn_nfromnto_ck check (cpntIDfrom!=cpntIDto)';

dbms_output.put_line('before indexes ');

	if(resfct=1) then
		execute immediate 'CREATE INDEX lhcb_cpntupdatehist_idx ON lhcb_cpnt_history (cpntID,status_change) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_cpnthist_idx ON lhcb_cpnt_history (cpntID) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_serialupdatehist1_idx ON lhcb_cpnt_history (serialnb,status_change) tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_serialhist1_idx ON lhcb_cpnt_history (serialnb) tablespace INDX01';
		
	else
		execute immediate 'CREATE INDEX lhcb_devupdatehist_idx ON lhcb_cpnt_history (cpntID,status_change) ';
		execute immediate 'CREATE INDEX lhcb_cpnthist_idx ON lhcb_cpnt_history (cpntID) ';
		execute immediate 'CREATE INDEX lhcb_serialupdatehist1_idx ON lhcb_cpnt_history (serialnb,status_change) ';
		execute immediate 'CREATE INDEX lhcb_serialhist1_idx ON lhcb_cpnt_history (serialnb) ';		
	end if;
dbms_output.put_line('before ipinfo ');

execute immediate '	create table lhcb_ipinfo (ipaddress varchar2(15),SUBNET_info varchar2(15),ipname varchar2(100),created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date default sysdate)';
execute immediate '	alter table lhcb_ipinfo add constraint ipinfo_pk primary key(ipaddress)';
execute immediate '	alter table lhcb_ipinfo add constraint ipinfo_ipnameuni unique(ipname)';
	if(resfct=1) then
execute immediate '	CREATE INDEX lhcb_subnetid_idx ON lhcb_ipinfo (subnet_info)  tablespace INDX01';
	else
execute immediate '	CREATE INDEX lhcb_subnetid_idx ON lhcb_ipinfo (subnet_info)';
	end if;


dbms_output.put_line('before port ppies ');
	
	execute immediate 'create table lhcb_port_properties (portid number,port_nbr varchar2(50),port_way number,administrative_status number default 1,speed number,port_type varchar2(100),phy varchar2(3) default ''T'',pxi_booting number,deviceid number,bia varchar2(18),macaddress varchar2(18),ipaddress varchar2(15),created date default sysdate,author varchar2(200),terminal_name varchar2(200),user_update  varchar2(200),last_update date default sysdate)';
	nbtab_created1:=nbtab_created1+1;
	dbms_output.put_line('before port ppies sequence');

	execute immediate 'create sequence lhcb_portseq minvalue 0 start with 1 increment by 1';
	execute immediate 'alter table lhcb_port_properties add constraint lhcb_port_pk primary key(portid)';
	execute immediate 'alter table lhcb_port_properties add constraint lhcb_port_ipadd_fk foreign key(ipaddress) references lhcb_ipinfo(ipaddress)';

	execute immediate 'alter table lhcb_port_properties add constraint lhcb_port_devid_fk foreign key(deviceid) references lhcb_lg_devices(deviceid)';
	execute immediate 'alter table lhcb_port_properties add constraint lhcb_port_uni unique(deviceid,port_nbr,port_type,port_way)';
	execute immediate 'alter table lhcb_port_properties add constraint lhcb_port_phy_ck check(upper(phy) in (''SX'',''SL'',''T''))';
	if(resfct=1) then
		execute immediate 'CREATE INDEX lhcb_portppdevid_idx ON lhcb_port_properties (deviceid)  tablespace INDX01';
		execute immediate 'CREATE INDEX lhcb_portppdipadd_idx ON lhcb_port_properties (ipaddress)  tablespace INDX01';
	else
		execute immediate 'CREATE INDEX lhcb_portppdevid_idx ON lhcb_port_properties (deviceid)';
		execute immediate 'CREATE INDEX lhcb_portppdipadd_idx ON lhcb_port_properties (ipaddress)  ';
	end if;

execute immediate '	create table lhcb_macroscopic_connectivity(linkid number,portIDfrom number,portIDto number,link_type number,bidirectional_link_used number default 0,link_weight number,LKUSED number default 0,system_name number,created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date default sysdate,user_update varchar2(100))';
execute immediate '	create sequence lhcb_macroconnseq minvalue 0 start with 1 increment by 1';
execute immediate '	alter table lhcb_macroscopic_connectivity add constraint lhcb_maconn_pk primary key(linkid)';
execute immediate '	alter table lhcb_macroscopic_connectivity add constraint lhcb_maconn_ptidfrom_fk foreign key(portIDfrom) references lhcb_port_properties(portID)';
execute immediate '	alter table lhcb_macroscopic_connectivity add constraint lhcb_maconn_ptidto_fk foreign key(portIDto) references lhcb_port_properties(portID)';
execute immediate '	alter table lhcb_macroscopic_connectivity add constraint lhcb_maconn_sfrom_uni unique(portIDfrom)';
execute immediate '	alter table lhcb_macroscopic_connectivity add constraint lhcb_maconn_sto_uni unique(portIDto)';
execute immediate '	alter table lhcb_macroscopic_connectivity add constraint lhcb_maconn_lktype_fk foreign key(link_type) references lhcb_link_types(linktypeID)';
execute immediate '	alter table lhcb_macroscopic_connectivity add constraint lhcb_maconn_nfromnto_ck check (portIDfrom!=portIDto)';
	

	
	if(resfct=1) then
		execute immediate '	create index lhcb_maconnect_lastupdate on lhcb_macroscopic_CONNECTIVITY(last_update) tablespace INDX01';
		execute immediate '	create index lhcb_maconnect_sysname on lhcb_macroscopic_CONNECTIVITY(system_name) tablespace INDX01';

	else
		execute immediate '	create index lhcb_maconnect_lastupdate on lhcb_macroscopic_CONNECTIVITY(last_update)';
		execute immediate '	create index lhcb_maconnect_sysname on lhcb_macroscopic_CONNECTIVITY(system_name) ';

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
	for i in sysname_list.first..sysname_list.last loop
		execute immediate 'create global temporary table lhcb_'||sysname_list(i)||'_path_temp (pathid number, link_pos number,nfrom number,pfrom number, nto number, pto number) on commit delete rows';
		execute immediate 'create table lhcb_'||sysname_list(i)||'_linkpairs(link_weight number ,node1 number,node2 number,node3 number,lktype number ,lkused number,last_update date default sysdate )'; 
		execute immediate 'create table lhcb_'||sysname_list(i)||'_lg_links (linkid number, node_from number,portIDfrom number,node_to number,portIDto number, link_weight number,lktype number,lkused number, last_update date)';
		execute immediate 'alter table lhcb_'||sysname_list(i)||'_lg_links add constraint pk_'||sysname_list(i)||'_lg_links primary key(node_from,node_to)';
		execute immediate 'alter table lhcb_'||sysname_list(i)||'_lg_links add constraint uni_'||sysname_list(i)||'_lg_links unique(linkid) ';

		execute immediate 'create table lhcb_'||sysname_list(i)||'_pdetails (pathid number,path_weight number,path_type number,pathused number, node1 number default 0,node2 number default 0,node3 number default 0,node4 number default 0,node5 number default 0,node6 number default 0,node7 number default 0,node8 number default 0,node9 number default 0,node10 number default 0,node11 number default 0,last_update date,path_necessary number default 0,constraint lhcb_'||sysname_list(i)||'_pathdetails_1_pk primary key(node1,pathid))';
		execute immediate 'alter table lhcb_'||sysname_list(i)||'_pdetails add constraint lhcb_'||sysname_list(i)||'_pneces_ck check (path_necessary in (0,1))';

		execute immediate 'create table lhcb_'||sysname_list(i)||'_pdetwithports(pathid number,link_pos number,nfrom number,pfrom number,nto number,pto number,node1 number)';
		execute immediate 'create table lhcb_'||sysname_list(i)||'_pdetwithlkids(pathid number,link_pos number,linkid number,node1 number)';
		execute immediate 'alter table  lhcb_'||sysname_list(i)||'_pdetwithports add constraint lhcb_'||sysname_list(i)||'_PDETWITHPORTS_PK primary key(pathid, node1,nfrom,pfrom)';
		execute immediate 'alter table  lhcb_'||sysname_list(i)||'_pdetwithlkids add constraint lhcb_'||sysname_list(i)||'_PDETWITHlkidS_PK primary key(pathid, node1,linkid)';
		execute immediate 'alter table  lhcb_'||sysname_list(i)||'_pdetwithports add constraint lhcb_'||sysname_list(i)||'_PDETWITHPORTS_fk1 foreign key(node1,pathid) references lhcb_'||sysname_list(i)||'_pdetails(node1,pathid)';
		execute immediate 'alter table  lhcb_'||sysname_list(i)||'_pdetwithlkids add constraint lhcb_'||sysname_list(i)||'_PDETWITHLKIDS_fk1 foreign key(node1,pathid) references lhcb_'||sysname_list(i)||'_pdetails(node1,pathid)';
	--execute immediate 'create view '||system_name||'pathdetailswithnode_view as select t.node1 as node1,t.pathid as pathid,t.link_pos as link_pos ,case when t.linkid >0 then g.devicename else f.devicename end as nfrom,case when t.linkid >0 then p.port_nbrfrom else p.port_nbrto end as pfrom,case when t.linkid >0 then f.devicename else g.devicename end as nto,case when t.linkid >0 then p.port_nbrto else p.port_nbrfrom end as pto from '||system_name||'pathdetailswithlkids t,'||system_name||'device g, '||system_name||'device f, '||system_name||'connectivity p where p.linkid=t.linkid and p.node_from=g.deviceid and p.node_to=f.deviceid';


		if(resfct=1) then
			execute immediate 'CREATE INDEX lhcb_'||sysname_list(i)||'_pathdevid1_idx ON lhcb_'||sysname_list(i)||'_pdetails (node1) tablespace INDX01';
			execute immediate 'CREATE INDEX lhcb_'||sysname_list(i)||'_pathdevid11_idx ON lhcb_'||sysname_list(i)||'_pdetails (node11) tablespace INDX01';
			execute immediate 'create index lhcb_'||sysname_list(i)||'_pathd_lastup on lhcb_'||sysname_list(i)||'_PDETAILs(last_update) tablespace INDX01';
			execute immediate 'create index lhcb_'||sysname_list(i)||'_pdetlkids_i1 on lhcb_'||sysname_list(i)||'_PDETWITHLKIDS(node1,pathid) tablespace INDX01';
			execute immediate 'create index lhcb_'||sysname_list(i)||'_pdetlkids_i2 on lhcb_'||sysname_list(i)||'_PDETWITHLKIDS(node1) tablespace INDX01';

			execute immediate 'create index lhcb_'||sysname_list(i)||'_pdetports_i1 on lhcb_'||sysname_list(i)||'_PDETWITHPORTS(node1,pathid) tablespace INDX01';
			execute immediate 'create index lhcb_'||sysname_list(i)||'_pdetports_i2 on lhcb_'||sysname_list(i)||'_PDETWITHPORTS(node1) tablespace INDX01';
		else
			execute immediate 'CREATE INDEX lhcb_'||sysname_list(i)||'_pathdevid1_idx ON lhcb_'||sysname_list(i)||'_pdetails (node1) ';
			execute immediate 'CREATE INDEX lhcb_'||sysname_list(i)||'_pathdevid11_idx ON lhcb_'||sysname_list(i)||'_pdetails (node11) ';
			execute immediate 'create index lhcb_'||sysname_list(i)||'_pathd_lastupdate on lhcb_'||sysname_list(i)||'_PDETAILS(last_update)';
			execute immediate 'create index lhcb_'||sysname_list(i)||'_pdetlkids_i1 on lhcb_'||sysname_list(i)||'_PDETWITHLKIDS(node1,pathid)';
			execute immediate 'create index lhcb_'||sysname_list(i)||'_pdetlkids_i2 on lhcb_'||sysname_list(i)||'_PDETWITHLKIDS(node1)';

			execute immediate 'create index lhcb_'||sysname_list(i)||'_pdetports_i1 on lhcb_'||sysname_list(i)||'_PDETWITHPORTS(node1,pathid)';
			execute immediate 'create index lhcb_'||sysname_list(i)||'_pdetports_i2 on lhcb_'||sysname_list(i)||'_PDETWITHPORTS(node1)';
		end if;
		execute immediate 'create table lhcb_'||sysname_list(i)||'_destinationtable(pathid number,pathused number,nodeid_start0 number,nodeid_start1 number,nodeid_end0 number,nodeid_end1 number,portIDstart0 number,destination_name varchar2(500) ,portIDdest1 number,path_type number, path_length number,last_update date, constraint lhcb_'||sysname_list(i)||'_dest_name_pk primary key(nodeid_start0,pathid))';
		execute immediate 'alter table lhcb_'||sysname_list(i)||'_destinationtable add constraint lhcb_'||sysname_list(i)||'_destab_pathfk foreign key(nodeid_start0,pathid) references lhcb_'||sysname_list(i)||'_pdetails (node1,pathid)';
		execute immediate 'alter table lhcb_'||sysname_list(i)||'_destinationtable add constraint lhcb_'||sysname_list(i)||'_destab_nids0fk foreign key(nodeid_start0) references lhcb_lg_devices (deviceid)';
		execute immediate 'alter table lhcb_'||sysname_list(i)||'_destinationtable add constraint lhcb_'||sysname_list(i)||'_destab_nids1fk foreign key(nodeid_start1) references lhcb_lg_devices (deviceid)';
		execute immediate 'alter table lhcb_'||sysname_list(i)||'_destinationtable add constraint lhcb_'||sysname_list(i)||'_destab_nide1fk foreign key(nodeid_end1) references lhcb_lg_devices(deviceid)';
		execute immediate 'alter table lhcb_'||sysname_list(i)||'_destinationtable add constraint lhcb_'||sysname_list(i)||'_destab_nide0fk foreign key(nodeid_end0) references lhcb_lg_devices (deviceid)';
		if(resfct=1) then
			execute immediate 'CREATE INDEX lhcb_'||sysname_list(i)||'_destname_idx ON lhcb_'||sysname_list(i)||'_destinationtable (destination_name)  tablespace INDX01';
			execute immediate 'CREATE INDEX lhcb_'||sysname_list(i)||'_destid_idx ON lhcb_'||sysname_list(i)||'_destinationtable (nodeid_end1)  tablespace INDX01';
			execute immediate 'CREATE INDEX lhcb_'||sysname_list(i)||'_destdevid_idx ON lhcb_'||sysname_list(i)||'_destinationtable (nodeid_start0)  tablespace INDX01';
		else
			execute immediate 'CREATE INDEX lhcb_'||sysname_list(i)||'_destname_idx ON lhcb_'||sysname_list(i)||'_destinationtable (destination_name)  ';
			execute immediate 'CREATE INDEX lhcb_'||sysname_list(i)||'_destid_idx ON lhcb_'||sysname_list(i)||'_destinationtable (nodeid_end1)  ';
			execute immediate 'CREATE INDEX lhcb_'||sysname_list(i)||'_destdevid_idx ON lhcb_'||sysname_list(i)||'_destinationtable (nodeid_start0)  ';
		end if;
		execute immediate 'create global temporary table lhcb_'||sysname_list(i)||'_pdet_temp(pathid number,path_weight number,path_type number,pathused number, node1 number default 0,node2 number default 0,node3 number default 0,node4 number default 0,node5 number default 0,node6 number default 0,node7 number default 0,node8 number default 0,node9 number default 0,node10 number default 0,node11 number default 0) on commit delete rows';
		execute immediate 'create global temporary table lhcb_'||sysname_list(i)||'_startend_temp(pathid number,pathused number,nodeid_start0 number,nodeid_start1 number,nodeid_end0 number,nodeid_end1 number,path_type number, path_length number) on commit delete rows';
		execute immediate 'create global temporary table lhcb_'||sysname_list(i)||'_destintab_temp(pathid number,port_nb number,destination_name varchar2(500),port_nb_destin number) on commit delete rows';
	end loop;

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


