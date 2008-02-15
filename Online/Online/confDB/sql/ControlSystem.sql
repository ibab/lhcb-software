--don't forget to delete in the right order pathdetails last one...
create or replace function CreateTableSchema(systemname varchar2,ip_address number default 0,port_pp number default 0,pc_pp number default 0,generic_schema number default 0) return number AUTHID CURRENT_USER is
resfct number:=-1;
nbtab_created number:=0;
nbtab_created1 number:=0;
nbtab_created2 number:=0;
system_name varchar2(20):='all_';
TYPE devid_list IS TABLE OF NUMBER;
prime_nbList devid_list:=devid_list(2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,197,199,211,223,227,229,233,239,241,251,257,263,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,487,491,499,503);

TYPE element_list IS TABLE OF VARCHAR2(50);
sysname_list element_list:=element_list('ECAL','HCAL','PRS','RICH1','RICH2','VELO','OT','IT','TT','MUON','DAQ','TFC','CONTROL','PUS');

begin
	select count(tablespace_name) into resfct from user_segments where tablespace_name='INDX01' and rownum=1;
	create table device_types(devicetypeID number,DEVICETYPE varchar2(100),NBROFINPUT number default -1,NBROFOUTPUT number default -1,DESCRIPTION varchar2(500),system_name varchar2(20),created date default sysdate,author varchar2(200),terminal_name varchar2(200), last_update date default sysdate, user_update varchar2(100),RGBcolor varchar2(12),constraint devtype_pk primary key(devicetypeID));
	alter table device_types add constraint DEVICEtype_UNI1 unique(devicetype);
	execute immediate 'create sequence '||system_name||'devtype_dtypeid minvalue 0 start with 1 increment by 1';
	
	-- system_name is absent because hardware view we put everything, even the spares one which can be common to all the subsystems such tell1
	nbtab_created:=nbtab_created+1;--2
	execute immediate 'create table '||system_name||'hardware_devices(serial_number varchar2(500),status number,responsible varchar2(100), comment varchar2(1000),hardwarename varchar2(100),location varchar2(200),constraint '||system_name||'serialnb_pk primary key(serial_number))';

	nbtab_created:=nbtab_created+1;--1
	execute immediate 'create table '||system_name||'logical_devices(DEVICENAME varchar2(100) NOT NULL,DEVICETYPEID number,ACTIVE number default 0,NODE number NOT NULL,nodeused number default 1,PROMISCUOUS_MODE number default 1,SERIAL_NUMBER varchar2(500),location varchar2(200),DEVICEID number, part_tfc number,part_daq number,system_name varchar2(100),created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date default sysdate, user_update varchar2(100), constraint '||system_name||'logical_device_pk primary key(deviceid))';
	execute immediate 'create sequence '||system_name||'logical_device_deviceid minvalue 0 start with 1 increment by 1';

	-- for the moment we presume that the logical devicename is unique in all the subsystems
	-- if it turns out, that it's not we will put a unique constraint on devicename,systemname

	execute immediate 'alter table '||system_name||'logical_devices add constraint '||system_name||'logical_DEVICE_UNI1 unique(devicename)';
	execute immediate 'alter table '||system_name||'logical_devices add constraint '||system_name||'logical_device_devtype_fk foreign key(devicetypeID) references '||system_name||'device_types(devicetypeID)';
	execute immediate 'alter table '||system_name||'logical_devices add constraint '||system_name||'logical_device_serialnb_uni unique(serial_number)';
	execute immediate 'alter table '||system_name||'logical_devices add constraint '||system_name||'logical_device_nodeused_ck check(active<nodeused+1)';
	execute immediate 'alter table '||system_name||'logical_devices add constraint '||system_name||'logical_device_serialnb_fk foreign key(serial_number) references '||system_name||'hardware_devices(serial_number)';

	nbtab_created:=nbtab_created+1;--2
	--for moment we don't partition this table: but in the future we will have to.
	execute immediate 'create table '||system_name||'device_history(historydeviceID number,serial_number varchar2(500),deviceid number,STATUS varchar2(50),comment varchar2(1000), location varchar2(200),status_change date,subsystem_name varchar2(100),created date,author varchar2(200),terminal_name varchar2(200), constraint '||system_name||'devicehist_pk primary key(historydeviceid))';
	execute immediate 'alter table '||system_name||'device_history add constraint '||system_name||'devicehist_dev_fk foreign key(deviceID) references '||system_name||'logical_devices(deviceID)';
	execute immediate 'alter table '||system_name||'device_history add constraint '||system_name||'devicehist_serialnb_fk foreign key(serial_number) references '||system_name||'hardware_devices(serial_number)';

	if(resfct=1) then
		execute immediate 'CREATE INDEX '||system_name||'devupdatehist_idx ON '||system_name||'device_history (deviceID,status_change) tablespace INDX01';
		execute immediate 'CREATE INDEX '||system_name||'devhist_idx ON '||system_name||'device_history (deviceID) tablespace INDX01';
		execute immediate 'CREATE INDEX '||system_name||'serialupdatehist_idx ON '||system_name||'device_history (serial_number,status_change) tablespace INDX01';
		execute immediate 'CREATE INDEX '||system_name||'serialhist_idx ON '||system_name||'device_history (serial_number) tablespace INDX01';
	else
		execute immediate 'CREATE INDEX '||system_name||'devupdatehist_idx ON '||system_name||'device_history (deviceID,status_change) ';
		execute immediate 'CREATE INDEX '||system_name||'devhist_idx ON '||system_name||'device_history (deviceID) ';
		execute immediate 'CREATE INDEX '||system_name||'serialupdatehist_idx ON '||system_name||'device_history (serial_number,status_change) ';
		execute immediate 'CREATE INDEX '||system_name||'serialhist_idx ON '||system_name||'device_history (serial_number) ';
	end if;


	execute immediate 'create table '||system_name||'link_types(linktypeID number,link_nbr number ,link_name varchar2(20) NOT NULL,created date default 0,author varchar2(200),terminal_name varchar2(200),last_update date default 0,user_update varchar2(100))';
	nbtab_created:=nbtab_created+1;--3
	execute immediate 'alter table '||system_name||'link_types add constraint '||system_name||'lktype_pk primary key(linktypeID)';
	execute immediate 'alter table '||system_name||'link_types add constraint '||system_name||'lktype_lknameuni unique(link_name)';
	execute immediate 'alter table '||system_name||'link_types add constraint '||system_name||'lktype_lknbuni unique(link_nbr)';
	execute immediate 'create sequence '||system_name||'lktype_linkid minvalue 0 start with 1 increment by 1';

	--execute immediate 'alter table '||system_name||'_link_type add constraint '||system_name||'_lktype_ck check(link_name is not null)';
	--the table will stores intermediates paths one per subsystem
	execute immediate 'create global temporary table '||system_name||'path_temp (pathid number, link_pos number,nfrom number,pfrom number, nto number, pto number) on commit delete rows';
	--table used to get prime number list
	execute immediate 'create  table '||system_name||'prime_number (prime_nb number, prime_nb_position number,last_update date,user_update varchar2(100),constraint '||system_name||'primenb_pk primary key(prime_nb), constraint '||system_name||'primenb_uni unique(prime_nb_position)) ';
	--need to fill the table with prime numbers (93) -----------
	for i in 1..93 loop
		execute immediate 'insert into '||system_name||'prime_number values (:1,:2,sysdate,sysdate)' using prime_nbList(i),i;
	end loop;	
	
	
	execute immediate 'create table '||system_name||'port_properties (portid number,port_nbr varchar2(50),administrative_status number default 1,speed number,port_type varchar2(100),phy varchar2(3) default 'T',pxi_booting number,deviceid number,bia varchar2(18),macaddress varchar2(18),ipaddress varchar2(15),created date default sysdate,author varchar2(200),terminal_name varchar2(200),user_update  varchar2(200),last_update date default sysdate)';
	nbtab_created1:=nbtab_created1+1;
	execute immediate 'create sequence '||system_name||'port_properties_portid minvalue 0 start with 1 increment by 1';

	execute immediate 'alter table '||system_name||'port_properties add constraint '||system_name||'port_pk primary key(portid)';
	execute immediate 'alter table '||system_name||'port_properties add constraint '||system_name||'port_devid_fk foreign key(ipaddress) references '||system_name||'ipinfo(ipaddress)';

	execute immediate 'alter table '||system_name||'port_properties add constraint '||system_name||'port_devid_fk foreign key(deviceid) references '||system_name||'logical_devices(deviceid)';
	execute immediate 'alter table '||system_name||'port_properties add constraint '||system_name||'port_uni unique(deviceid,port_nbr,port_type)';
	execute immediate 'alter table '||system_name||'port_properties add constraint '||system_name||'port_phy_ck check(upper(phy) in (''SX'',''SL'',''T''))';
	if(resfct=1) then
		execute immediate 'CREATE INDEX '||system_name||'portppdevid_idx ON '||system_name||'port_properties (deviceid)  tablespace INDX01';
	else
		execute immediate 'CREATE INDEX '||system_name||'portppdevid_idx ON '||system_name||'port_properties (deviceid)  ';
	end if;
	
		
	execute immediate 'create table '||system_name||'connectivity(linkid number,portIDfrom number,portIDto number,link_type number,bidirectional_link_used number default 0,link_weight number default 1,LKUSED number default 0,created date,author varchar2(200),terminal_name varchar2(200),last_update date,user_update varchar2(100))';
	execute immediate 'create sequence '||system_name||'connectivity_linkid minvalue 0 start with 1 increment by 1';
	execute immediate 'alter table '||system_name||'connectivity add constraint '||system_name||'conn_pk primary key(linkid)';
	execute immediate 'alter table '||system_name||'connectivity add constraint '||system_name||'conn_ptidfrom_fk foreign key(portIDfrom) references '||system_name||'port_properties(portID)';
	execute immediate 'alter table '||system_name||'connectivity add constraint '||system_name||'conn_ptidto_fk foreign key(portIDto) references '||system_name||'port_properties(portID)';
	execute immediate 'alter table '||system_name||'connectivity add constraint '||system_name||'conn_sfrom_uni unique(portIDfrom)';
	execute immediate 'alter table '||system_name||'connectivity add constraint '||system_name||'conn_sto_uni unique(portIDto)';
	execute immediate 'alter table '||system_name||'connectivity add constraint '||system_name||'conn_lktype_fk foreign key(link_type) references '||system_name||'link_types(linktypeID)';
	execute immediate 'alter table '||system_name||'connectivity add constraint '||system_name||'conn_nfromnto_ck check (portIDfrom!=portIDto)';
	
	--automatic table are by subsystems:
	for i in sysname_list.first..sysname_list.last loop
		execute immediate 'create table '||sysname_list(i)||'linkpairs(link_weight number ,node1 number,node2 number,node3 number,lktype number ,lkused number,last_update date )'; 
		execute immediate 'create table '||sysname_list(i)||'logical_links (linkid number, node_from number,portIDfrom number,node_to number,portIDto number, link_weight number,lktrunk number,lktype number,lkused number, last_update date)';
		execute immediate 'alter table '||sysname_list(i)||'logical_links add constraint pk_'||sysname_list(i)|'logical_links primary key(node_from,node_to)';
		execute immediate 'alter table '||sysname_list(i)||'logical_links add constraint uni_'||sysname_list(i)||'logical_links unique(linkid) ';

		if(resfct=1) then
			execute immediate 'create index '||sysname_list(i)||'connect_lastupdate on '||system_name||'CONNECTIVITY(last_update) tablespace INDX01';
		else
			execute immediate 'create index '||sysname_list(i)||'connect_lastupdate on '||system_name||'CONNECTIVITY(last_update)';
		end if;

		execute immediate 'create table '||sysname_list(i)||'pathdetails (pathid number,path_weight number,path_type number,pathused number, node1 number default 0,node2 number default 0,node3 number default 0,node4 number default 0,node5 number default 0,node6 number default 0,node7 number default 0,node8 number default 0,node9 number default 0,node10 number default 0,node11 number default 0,last_update date,path_necessary number default 0,constraint '||system_name||'pathdetails_1_pk primary key(node1,pathid))';
		execute immediate 'alter table '||sysname_list(i)||'pathdetails add constraint '||sysname_list(i)||'pathnecessary_ck check (path_necessary in (0,1))';

		execute immediate 'create table '||sysname_list(i)||'pathdetailswithports(pathid number,link_pos number,nfrom number,pfrom number,nto number,pto number,node1 number)';
		execute immediate 'create table '||sysname_list(i)||'pathdetailswithlkids(pathid number,link_pos number,linkid number,node1 number)';
		execute immediate 'alter table  '||sysname_list(i)||'pathdetailswithports add constraint '||sysname_list(i)||'PDETAILSWITHPORTS_PK primary key(pathid, node1,nfrom,pfrom)';
		execute immediate 'alter table  '||sysname_list(i)||'pathdetailswithlkids add constraint '||sysname_list(i)||'PDETAILSWITHlkidS_PK primary key(pathid, node1,linkid)';
		execute immediate 'alter table  '||sysname_list(i)||'pathdetailswithports add constraint '||sysname_list(i)||'PDETAILSWITHPORTS_fk1 foreign key(node1,pathid) references '||sysname_list(i)||'pathdetails(node1,pathid)';
		execute immediate 'alter table  '||sysname_list(i)||'pathdetailswithlkids add constraint '||sysname_list(i)||'PDETAILSWITHLKIDS_fk1 foreign key(node1,pathid) references '||sysname_list(i)||'pathdetails(node1,pathid)';
	--execute immediate 'create view '||system_name||'pathdetailswithnode_view as select t.node1 as node1,t.pathid as pathid,t.link_pos as link_pos ,case when t.linkid >0 then g.devicename else f.devicename end as nfrom,case when t.linkid >0 then p.port_nbrfrom else p.port_nbrto end as pfrom,case when t.linkid >0 then f.devicename else g.devicename end as nto,case when t.linkid >0 then p.port_nbrto else p.port_nbrfrom end as pto from '||system_name||'pathdetailswithlkids t,'||system_name||'device g, '||system_name||'device f, '||system_name||'connectivity p where p.linkid=t.linkid and p.node_from=g.deviceid and p.node_to=f.deviceid';


		if(resfct=1) then
			execute immediate 'CREATE INDEX '||sysname_list(i)||'pathdevid1_idx ON '||sysname_list(i)||'pathdetails (node1) tablespace INDX01';
			execute immediate 'CREATE INDEX '||sysname_list(i)||'pathdevid11_idx ON '||sysname_list(i)||'pathdetails (node11) tablespace INDX01';
			execute immediate 'create index '||sysname_list(i)||'pathd_lastupdate on '||sysname_list(i)||'PATHDETAILS(last_update) tablespace INDX01';
			execute immediate 'create index '||sysname_list(i)||'pdetailswithlkids_i1 on '||sysname_list(i)||'PATHDETAILSWITHLKIDS(node1,pathid) tablespace INDX01';
			execute immediate 'create index '||sysname_list(i)||'pdetailswithlkids_i2 on '||sysname_list(i)||'PATHDETAILSWITHLKIDS(node1) tablespace INDX01';

			execute immediate 'create index '||sysname_list(i)||'pdetailswithports_i1 on '||sysname_list(i)||'PATHDETAILSWITHPORTS(node1,pathid) tablespace INDX01';
			execute immediate 'create index '||sysname_list(i)||'pdetailswithports_i2 on '||sysname_list(i)||'PATHDETAILSWITHPORTS(node1) tablespace INDX01';
		else
			execute immediate 'CREATE INDEX '||sysname_list(i)||'pathdevid1_idx ON '||sysname_list(i)||'pathdetails (node1) ';
			execute immediate 'CREATE INDEX '||sysname_list(i)||'pathdevid11_idx ON '||sysname_list(i)||'pathdetails (node11) ';
			execute immediate 'create index '||sysname_list(i)||'pathd_lastupdate on '||sysname_list(i)||'PATHDETAILS(last_update)';
			execute immediate 'create index '||sysname_list(i)||'pdetailswithlkids_i1 on '||sysname_list(i)||'PATHDETAILSWITHLKIDS(node1,pathid)';
			execute immediate 'create index '||sysname_list(i)||'pdetailswithlkids_i2 on '||sysname_list(i)||'PATHDETAILSWITHLKIDS(node1)';

			execute immediate 'create index '||sysname_list(i)||'pdetailswithports_i1 on '||sysname_list(i)||'PATHDETAILSWITHPORTS(node1,pathid)';
			execute immediate 'create index '||sysname_list(i)||'pdetailswithports_i2 on '||sysname_list(i)||'PATHDETAILSWITHPORTS(node1)';
		end if;
		execute immediate 'create table '||sysname_list(i)||'destinationtable(pathid number,pathused number,nodeid_start0 number,nodeid_start1 number,nodeid_end0 number,nodeid_end1 number,port_nb number,destination_name varchar2(500) ,port_nb_destin number,path_type number, path_length number,last_update date, constraint '||sysname_list(i)||'destination_name_pk primary key(nodeid_start0,pathid))';
		execute immediate 'alter table '||sysname_list(i)||'destinationtable add constraint '||sysname_list(i)||'destintab_pathfk foreign key(nodeid_start0,pathid) references '||sysname_list(i)||'pathdetails (node1,pathid)';
		execute immediate 'alter table '||sysname_list(i)||'destinationtable add constraint '||sysname_list(i)||'destintab_nids0fk foreign key(nodeid_start0) references '||sysname_list(i)||'device (deviceid)';
		execute immediate 'alter table '||sysname_list(i)||'destinationtable add constraint '||sysname_list(i)||'destintab_nids1fk foreign key(nodeid_start1) references '||sysname_list(i)||'device (deviceid)';
		execute immediate 'alter table '||sysname_list(i)||'destinationtable add constraint '||sysname_list(i)||'destintab_nide1fk foreign key(nodeid_end1) references '||sysname_list(i)||'device (deviceid)';
		execute immediate 'alter table '||sysname_list(i)||'destinationtable add constraint '||sysname_list(i)||'destintab_nide0fk foreign key(nodeid_end0) references '||sysname_list(i)||'device (deviceid)';
		if(resfct=1) then
			execute immediate 'CREATE INDEX '||sysname_list(i)||'destinname_idx ON '||sysname_list(i)||'destinationtable (destination_name)  tablespace INDX01';
			execute immediate 'CREATE INDEX '||sysname_list(i)||'destinid_idx ON '||sysname_list(i)||'destinationtable (nodeid_end1)  tablespace INDX01';
			execute immediate 'CREATE INDEX '||sysname_list(i)||'destindevid_idx ON '||sysname_list(i)||'destinationtable (nodeid_start0)  tablespace INDX01';
		else
			execute immediate 'CREATE INDEX '||sysname_list(i)||'destinname_idx ON '||sysname_list(i)||'destinationtable (destination_name)  ';
			execute immediate 'CREATE INDEX '||sysname_list(i)||'destinid_idx ON '||sysname_list(i)||'destinationtable (nodeid_end1)  ';
			execute immediate 'CREATE INDEX '||sysname_list(i)||'destindevid_idx ON '||sysname_list(i)||'destinationtable (nodeid_start0)  ';
		end if;
		execute immediate 'create global temporary table '||sysname_list(i)||'pathdetails_temporary(pathid number,path_weight number,path_type number,pathused number, node1 number default 0,node2 number default 0,node3 number default 0,node4 number default 0,node5 number default 0,node6 number default 0,node7 number default 0,node8 number default 0,node9 number default 0,node10 number default 0,node11 number default 0) on commit delete rows';
		execute immediate 'create global temporary table '||sysname_list(i)||'startend_temporary(pathid number,pathused number,nodeid_start0 number,nodeid_start1 number,nodeid_end0 number,nodeid_end1 number,path_type number, path_length number) on commit delete rows';
		execute immediate 'create global temporary table '||sysname_list(i)||'destintab_temporary(pathid number,port_nb number,destination_name varchar2(500),port_nb_destin number) on commit delete rows';
	end loop;
	nbtab_created:=nbtab_created+1;--4

	--end of first part of automatic tables
	

	execute immediate 'create table '||system_name||'ipinfo (ipaddress varchar2(15),SUBNET_info varchar2(15),ipname varchar2(100),created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date default sysdate)';
	execute immediate 'alter table '||system_name||'ipinfo add constraint '||system_name||'ipinfo_pk primary key(ipaddress)';
	execute immediate 'alter table '||system_name||'ipinfo add constraint '||system_name||'ipinfo_ipnameuni unique(ipname)';
	if(resfct=1) then
		execute immediate 'CREATE INDEX '||system_name||'subnetid_idx ON '||system_name||'ipinfo (subnet_info)  tablespace INDX01';
	else
		execute immediate 'CREATE INDEX '||system_name||'subnetid_idx ON '||system_name||'ipinfo (subnet_info)';
	end if;
		--execute immediate 'create table '||system_name||'trunk_link(LINK_TRUNKID number, LKAGGREG_ADD varchar2(17),created date default sysdate,author varchar2(200),terminal_name varchar2(200),last_update date default sysdate, user_update varchar2(100),constraint '||system_name||'tklkid_pk primary key(link_trunkid))';
		--nbtab_created1:=nbtab_created1+1;
		--execute immediate 'insert into '||system_name||'trunk_link(LINK_TRUNKID,created,last_update) values (0,sysdate,sysdate)';
		--execute immediate 'create sequence '||system_name||'trunk_link_LINK_TRUNKID minvalue 0 start with 1 increment by 1';
		--execute immediate 'alter table '||system_name||'trunk_link add constraint '||system_name||'tklk_lkaggreg_add_fk foreign key(lkaggreg_add) references '||system_name||'ip_ethernet(ethernet_add)';
		--execute immediate 'alter table '||system_name||'trunk_link add constraint '||system_name||'tklk_lkaggreg_add_uni unique(lkaggreg_add)';
		
	--	execute immediate 'create table '||system_name||'routingtable(pathid number,pathused number,nodeid_start0 number,nodeid_start1 number,nodeid_end0 number,nodeid_end1 number,port_nb number,portID_destin varchar2(17) ,portID_nexthop varchar2(17),path_type number, path_length number,last_update date,constraint '||system_name||'routing_name_pk primary key(nodeid_start0 ,pathid) )';
	--	execute immediate 'alter table '||system_name||'routingtable add constraint '||system_name||'routingtab_pathfk foreign key(nodeid_start0 ,pathid) references '||system_name||'pathdetails (node1 ,pathid)';
	--	execute immediate 'alter table '||system_name||'routingtable add constraint '||system_name||'routingtab_nids0fk foreign key(nodeid_start0) references '||system_name||'device (deviceid)';
	--	execute immediate 'alter table '||system_name||'routingtable add constraint '||system_name||'routingtab_nids1k foreign key(nodeid_start1) references '||system_name||'device (deviceid)';
	--	execute immediate 'alter table '||system_name||'routingtable add constraint '||system_name||'routingtab_nide1fk foreign key(nodeid_end1) references '||system_name||'device(deviceid)';
	--	execute immediate 'alter table '||system_name||'routingtable add constraint '||system_name||'routingtab_nide0fk foreign key(nodeid_end0) references '||system_name||'device(deviceid)';
	--	execute immediate 'alter table '||system_name||'routingtable add constraint '||system_name||'routingtab_etherdesfk foreign key(ethernet_add_destin) references '||system_name||'ip_ethernet(ethernet_add)';
	--	execute immediate 'alter table '||system_name||'routingtable add constraint '||system_name||'routingtab_ethernextfk foreign key(ethernet_add_nexthop) references '||system_name||'ip_ethernet(ethernet_add)';
	if(3=1) then
		execute immediate 'CREATE INDEX '||system_name||'routingdevid_idx ON '||system_name||'routingtable (nodeid_start0)  tablespace INDX01';
		execute immediate 'CREATE INDEX '||system_name||'routingdevid1_idx ON '||system_name||'routingtable (nodeid_end1) tablespace INDX01 ';
		execute immediate 'CREATE INDEX '||system_name||'routingetherdest_idx ON '||system_name||'routingtable (ethernet_add_destin) tablespace INDX01  ';
	else
		execute immediate 'CREATE INDEX '||system_name||'routingdevid_idx ON '||system_name||'routingtable (nodeid_start0)  ';
		execute immediate 'CREATE INDEX '||system_name||'routingdevid1_idx ON '||system_name||'routingtable (nodeid_end1)  ';
		execute immediate 'CREATE INDEX '||system_name||'routingetherdest_idx ON '||system_name||'routingtable (ethernet_add_destin)  ';
	end if;
		--execute immediate 'create global temporary table '||system_name||'routingtab_temporary(pathid number,port_nb number,ethernet_add_destin varchar2(17),ethernet_add_nexthop varchar2(17)) on commit delete rows';
		--execute immediate 'create global temporary table '||system_name||'routingtabcopy_temporary (pid_tp number,pathused number,lk_node_from number,lk_pfrom number,lk_node_to number,lk_pto number,path_type number) on commit delete rows';


--if(pc_pp=1) then
	execute immediate 'create table '||system_name||'device_booting(deviceid number,KERNEL_IMAGE_LOCATION varchar2(500), INITRD_IMAGE_LOCATION varchar2(500), PHYSICAL_LOCATION varchar2(500),boot_image_location varchar2(500),boot_protocol varchar2(100),created date,author varchar2(200),terminal_name varchar2(200),last_update date,user_update varchar2(100), constraint '||system_name||'pc_device_pk primary key(deviceid))';
	execute immediate 'create table '||system_name||'devicetype_booting(devicetypeID number,KERNEL_IMAGE_LOCATION varchar2(500), INITRD_IMAGE_LOCATION varchar2(500), PHYSICAL_LOCATION varchar2(500),boot_image_location varchar2(500),boot_protocol varchar2(100),created date,author varchar2(200),terminal_name varchar2(200),last_update date, user_update varchar2(100), constraint '||system_name||'pc_devtype_pk primary key(devicetypeID))';
	nbtab_created2:=nbtab_created2+1;
	execute immediate 'alter table '||system_name||'devicetype_booting add constraint '||system_name||'pc_devtype_fk foreign key(devicetypeID) references '||system_name||'device_type (devicetypeID)';
	execute immediate 'alter table '||system_name||'device_booting add constraint '||system_name||'pc_device_fk foreign key(deviceid) references '||system_name||'device (deviceid)';	
--end if;
-------------------------------------------------------------------------------------
		






-------------------------------------------------------------------------------------
commit;
resfct:=0;
return resfct;

exception
when NO_DATA_FOUND then
resfct:=-1;
resfct:=droptableschema(system_name);
return -1;
when OTHERS then
resfct:=sqlcode;
nbtab_created :=droptableschema(system_name);
return resfct;	
end CreateTableSchema;
/


