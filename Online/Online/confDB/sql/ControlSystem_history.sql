--don't forget to delete in the right order pathdetails last one...
create or replace function CreateTableSchema(system_name varchar2,ip_address number default 0,port_pp number default 0,pc_pp number default 0,generic_schema number default 0) return number AUTHID CURRENT_USER is
resfct number:=-1;
nbtab_created number:=0;
nbtab_created1 number:=0;
nbtab_created2 number:=0;
TYPE devid_list IS TABLE OF NUMBER;
prime_nbList devid_list:=devid_list(2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,197,199,211,223,227,229,233,239,241,251,257,263,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,487,491,499,503);

begin
select count(tablespace_name) into resfct from user_segments where tablespace_name='INDX01' and rownum=1;
if(generic_schema=1) then
	execute immediate 'create table '||system_name||'device_type_hist(devicetypeID number,DEVICETYPE varchar2(100),NBROFINPUT number default -1,NBROFoutPUT number default -1,DESCRIPTION varchar2(500),system_name varchar2(20),created date,author varchar2(200),terminal_name varchar2(200), last_update date, user_update varchar2(100),RGBcolor varchar2(12),constraint '||system_name||'devtype_pk primary key(devicetypeID))';
	execute immediate 'alter table '||system_name||'device_type_hist add constraint '||system_name||'DEVICEtype_UNI1 unique(devicetype)';
	--execute immediate 'create sequence '||system_name||'devtype_dtypeid minvalue 0 start with 1 increment by 1';

	nbtab_created:=nbtab_created+1;--1
	execute immediate 'create table '||system_name||'device_hist(DEVICENAME varchar2(100) NOT NULL,DEVICETYPEID number,ACTIVE number default 0,HOSTNAME varchar2(500),NODE number NOT NULL,nodeused number default 0,STATUS number default 0,PROMISCUOUS_MODE number default 1,BAR_CODE varchar2(100) default 0,UUID varchar2(100) default 0,DEVICEID number, created date,author varchar2(200),terminal_name varchar2(200),last_update date, user_update varchar2(100), constraint '||system_name||'device_pk primary key(deviceid))';
	execute immediate 'create sequence '||system_name||'device_deviceid minvalue 0 start with 1 increment by 1';

	nbtab_created:=nbtab_created+1;--2
	execute immediate 'alter table '||system_name||'device_hist add constraint '||system_name||'DEVICE_UNI1 unique(devicename)';
	execute immediate 'alter table '||system_name||'device_hist add constraint '||system_name||'device_devtype_fk foreign key(devicetypeID) references '||system_name||'device_type(devicetypeID)';
	execute immediate 'alter table '||system_name||'device_hist add constraint '||system_name||'device_barcode_uni unique(bar_code)';
	execute immediate 'alter table '||system_name||'device_hist add constraint '||system_name||'device_uuid_uni unique(uuid)';
	execute immediate 'alter table '||system_name||'device_hist add constraint '||system_name||'device_nodeused_ck check(active<nodeused+1)';

	if(resfct=1) then
		execute immediate 'CREATE INDEX '||system_name||'devtype_idx ON '||system_name||'device_hist (devicetypeID) tablespace INDX01';
	else
		execute immediate 'CREATE INDEX '||system_name||'devtype_idx ON '||system_name||'device_hist (devicetypeID)';
	end if;


	execute immediate 'create table '||system_name||'link_type_hist (linktypeID number,link_nbr number ,link_name varchar2(20) NOT NULL,created date,author varchar2(200),terminal_name varchar2(200),last_update date,user_update varchar2(100))';
	nbtab_created:=nbtab_created+1;--3
	execute immediate 'alter table '||system_name||'link_type_hist add constraint '||system_name||'lktype_pk primary key(linktypeID)';
	execute immediate 'alter table '||system_name||'link_type_hist add constraint '||system_name||'lktype_lknameuni unique(link_name)';
	execute immediate 'alter table '||system_name||'link_type_hist add constraint '||system_name||'lktype_lknbuni unique(link_nbr)';
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
	execute immediate 'create table '||system_name||'connectivity_hist(linkid number,node_from number,port_nbrfrom number,node_to number,port_nbrto number,link_type number,bidirectional_link_used number default 0,link_weight number default 1,TRUNK_LKID number, LKUSED number default 0,created date,author varchar2(200),terminal_name varchar2(200),last_update date,user_update varchar2(100))';

	execute immediate 'create sequence '||system_name||'connectivity_linkid minvalue 0 start with 1 increment by 1';
	execute immediate 'alter table '||system_name||'connectivity_hist add constraint '||system_name||'conn_pk primary key(linkid)';
	execute immediate 'alter table '||system_name||'connectivity_hist add constraint '||system_name||'conn_sfrom_fk foreign key(node_from) references '||system_name||'device(deviceid)';
	execute immediate 'alter table '||system_name||'connectivity_hist add constraint '||system_name||'conn_sto_fk foreign key(node_to) references '||system_name||'device(deviceid)';
	execute immediate 'alter table '||system_name||'connectivity_hist add constraint '||system_name||'conn_sfrom_uni unique(node_from,port_nbrfrom)';
	execute immediate 'alter table '||system_name||'connectivity_hist add constraint '||system_name||'conn_sto_uni unique(node_to,port_nbrto)';
	execute immediate 'alter table '||system_name||'connectivity_hist add constraint '||system_name||'conn_lktype_fk foreign key(link_type) references '||system_name||'link_type(linktypeID)';
	execute immediate 'alter table '||system_name||'connectivity_hist add constraint '||system_name||'conn_nfromnto_ck check (node_from!=node_to)';
	
	if(resfct=1) then

		execute immediate 'CREATE INDEX '||system_name||'nodefrom_idx ON '||system_name||'connectivity_hist (node_from) tablespace INDX01 ';
		execute immediate 'CREATE INDEX '||system_name||'nodeto_idx ON '||system_name||'connectivity_hist (node_to)  tablespace INDX01 ';
		execute immediate 'create index '||system_name||'connect_lastupdate on '||system_name||'CONNECTIVITY_hist(last_update) tablespace INDX01';

	else
		execute immediate 'CREATE INDEX '||system_name||'nodefrom_idx ON '||system_name||'connectivity_hist (node_from)  ';
		execute immediate 'CREATE INDEX '||system_name||'nodeto_idx ON '||system_name||'connectivity_hist (node_to)  ';
		execute immediate 'create index '||system_name||'connect_lastupdate on '||system_name||'CONNECTIVITY_hist(last_update)';
	end if;

end if;
nbtab_created:=nbtab_created+1;--4

if(port_pp=1) then
	execute immediate 'create table '||system_name||'port_properties_hist (portid number,port_nbr number,rxfc number default 0,txfc number default 0, status number default 0,speed1 number default 0, speed2 number default 0,speed3 number default 0,port_management number default 0,wire_sense varchar2(100),phy varchar2(3),force_speed number default 0,bmc_booting number default -1, primary_interface number default -1,TRAFFIC_TYPE_SUPPORTED number  default -1, deviceid number,created date,author varchar2(200),terminal_name varchar2(200),user_update  varchar2(200),last_update date)';
	nbtab_created1:=nbtab_created1+1;
	execute immediate 'create sequence '||system_name||'port_properties_portid minvalue 0 start with 1 increment by 1';

	execute immediate 'alter table '||system_name||'port_properties_hist add constraint '||system_name||'port_pk primary key(portid)';
	execute immediate 'alter table '||system_name||'port_properties_hist add constraint '||system_name||'port_devid_fk foreign key(deviceid) references '||system_name||'device(deviceid)';
	execute immediate 'alter table '||system_name||'port_properties_hist add constraint '||system_name||'port_uni unique(deviceid,port_nbr,bmc_booting)';
	execute immediate 'alter table '||system_name||'port_properties_hist add constraint '||system_name||'port_wiresense_ck check(upper(wire_sense) in (''SWITCH/SWITCH'',''SWITCH/COMPUTER'',''AUTOSENSE''))';
	execute immediate 'alter table '||system_name||'port_properties_hist add constraint '||system_name||'port_phy_ck check(upper(phy) in (''SX'',''SL'',''T''))';
	if(resfct=1) then
		execute immediate 'CREATE INDEX '||system_name||'portppdevid_idx ON '||system_name||'port_properties_hist (deviceid)  tablespace INDX01';
	else
		execute immediate 'CREATE INDEX '||system_name||'portppdevid_idx ON '||system_name||'port_properties_hist (deviceid)  ';
	end if;
	if(ip_address=1) then
		execute immediate 'create table '||system_name||'ip_ethernet_hist (ethernet_add varchar2(17),ip_add varchar2(15),SUBNET_MASK varchar2(15),VLAN_PREFIX varchar2(11),LKAGGREG number default 0, portid number,ipname varchar2(100),created date,author varchar2(200),terminal_name varchar2(200),last_update date)';
		nbtab_created1:=nbtab_created1+1;
		execute immediate 'alter table '||system_name||'ip_ethernet_hist add constraint '||system_name||'ip_ether_pk primary key(ethernet_add)';
				execute immediate 'alter table '||system_name||'ip_ethernet_hist add constraint '||system_name||'ip_ether_uni unique(portid,lkaggreg)';
		execute immediate 'alter table '||system_name||'ip_ethernet_hist add constraint '||system_name||'ip_ether_portid_fk foreign key(portid) references '||system_name||'port_properties(portid)';
		execute immediate 'alter table '||system_name||'ip_ethernet_hist add constraint '||system_name||'ip_ether_etheradd_ck check(length(ethernet_add)=17)';
		execute immediate 'alter table '||system_name||'ip_ethernet_hist add constraint '||system_name||'ip_ether_lkaggreg_ck check(lkaggreg in (0,1))';
		if(resfct=1) then
			execute immediate 'CREATE INDEX '||system_name||'etherportid_idx ON '||system_name||'ip_ethernet_hist (portid)  tablespace INDX01';
			execute immediate 'CREATE INDEX '||system_name||'etheripadd_idx ON '||system_name||'ip_ethernet_hist (ip_add)  tablespace INDX01';
		else
			execute immediate 'CREATE INDEX '||system_name||'etherportid_idx ON '||system_name||'ip_ethernet_hist (portid)  ';
			execute immediate 'CREATE INDEX '||system_name||'etheripadd_idx ON '||system_name||'ip_ethernet_hist (ip_add)  ';

		end if;
		execute immediate 'create table '||system_name||'trunk_link_hist(LINK_TRUNKID number, LKAGGREG_ADD varchar2(17),created date,author varchar2(200),terminal_name varchar2(200),last_update date, user_update varchar2(100),constraint '||system_name||'tklkid_pk primary key(link_trunkid))';
		nbtab_created1:=nbtab_created1+1;
		execute immediate 'insert into '||system_name||'trunk_link_hist(LINK_TRUNKID,created,last_update) values (0,sysdate,sysdate)';
		execute immediate 'create sequence '||system_name||'trunk_link_LINK_TRUNKID minvalue 0 start with 1 increment by 1';
		execute immediate 'alter table '||system_name||'trunk_link_hist add constraint '||system_name||'tklk_lkaggreg_add_fk foreign key(lkaggreg_add) references '||system_name||'ip_ethernet(ethernet_add)';
		execute immediate 'alter table '||system_name||'trunk_link_hist add constraint '||system_name||'tklk_lkaggreg_add_uni unique(lkaggreg_add)';
		
	end if;
end if;

if(pc_pp=1) then
	execute immediate 'create table '||system_name||'device_booting_hist(deviceid number,KERNEL_IMAGE_LOCATION varchar2(500), INITRD_IMAGE_LOCATION varchar2(500), PHYSICAL_LOCATION varchar2(500),boot_image_location varchar2(500),boot_protocol varchar2(100),created date,author varchar2(200),terminal_name varchar2(200),last_update date,user_update varchar2(100), constraint '||system_name||'pc_device_pk primary key(deviceid))';
	execute immediate 'create table '||system_name||'devicetype_booting_hist(devicetypeID number,KERNEL_IMAGE_LOCATION varchar2(500), INITRD_IMAGE_LOCATION varchar2(500), PHYSICAL_LOCATION varchar2(500),boot_image_location varchar2(500),boot_protocol varchar2(100),created date,author varchar2(200),terminal_name varchar2(200),last_update date, user_update varchar2(100), constraint '||system_name||'pc_devtype_pk primary key(devicetypeID))';
	nbtab_created2:=nbtab_created2+1;
	execute immediate 'alter table '||system_name||'devicetype_booting_hist add constraint '||system_name||'pc_devtype__histfk foreign key(devicetypeID) references '||system_name||'device_type_hist (devicetypeID)';
	execute immediate 'alter table '||system_name||'device_booting_hist add constraint '||system_name||'pc_device__histfk foreign key(deviceid) references '||system_name||'device_hist (deviceid)';	
end if;
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


