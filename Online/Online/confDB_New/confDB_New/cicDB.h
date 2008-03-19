#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined( _WIN32) || defined(WIN32)

#define EXTERN_CONFDB _declspec(dllexport)

#else
#define EXTERN_CONFDB 
#endif


	EXTERN_CONFDB void* get_ErrorHandle(void);
	EXTERN_CONFDB void* get_OCIEnv(void);
	EXTERN_CONFDB void* get_ContxtHdl(void);
	EXTERN_CONFDB void* get_ServerHandle(void);
	EXTERN_CONFDB void* get_SessionHandle(void);


	EXTERN_CONFDB
		int UpdateMultiplePorts(char* devicename,char* port_nb,int port_way,char* port_type,int speed,char* phy,int pxi_booting,int first_time1,int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int UpdateMultiplePortStatuses(char* devicename,char* port_nb,int port_way,char* port_type,int admin_status,int first_time1,int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int UpdateMultipleAttributeMacIPs(char* ip_add,char* subnet_mask,char* ipname,int first_time1,int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int UpdateMultipleIPAddresses( char* ip_add,char* oldip_add,int first_time1,int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int UpdateMultipleCompositeLinkTypes(char* link_name,char * simple_lk_list,int first_time1,int last_rows1,char* ErrMess);



	EXTERN_CONFDB
		int UpdateMultipleLinkTypeNames(char *old_link_name, char *link_name,int first_time1,int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int UpdateMultipleLkInfoLinks(char* node_from,char* node_to,char* port_nbrfrom,char* port_nbrto,char* port_typefrom,char* port_typeto,char* link_information,int first_time1, int last_rows1,char* ErrMess);


	EXTERN_CONFDB
		int UpdateMultipleDeviceTypes(char* devicetype_old,char* devicetype,int first_time1,int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int UpdateMultipleDeviceTypeAttributes(char* devicetype,char* description,int nbrofinput,int nbrofoutput,char* rgbcolor,int first_time1,int last_rows1,char* ErrMess);


	EXTERN_CONFDB
		int UpdateMultipleDTypeDevices(char* devicename,char* devicetype,int first_time1,int last_rows1,char* ErrMess);


	EXTERN_CONFDB
		int UpdateMultipleDevNamesDevices(char* devicename_old,char* devicename,int first_time1,int last_rows1,char* ErrMess);



	EXTERN_CONFDB int UpdateMultipleDeviceActive(char* devicename,int active,int first_time1,int last_rows1,char* ErrMess);

	EXTERN_CONFDB int UpdateMultipleDeviceActiveByType(char* devicetype,int active,int devtype_given,int last_rows,char* ErrMess);

	EXTERN_CONFDB int UpdateMultipleDeviceNodeUsedByType(char* devicetype,int nodeused,int devtype_given,int last_rows,char* ErrMess);

	EXTERN_CONFDB int UpdateMultipleDeviceNodeUsed(char* devicename,int nodeused,int first_time1,int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int UpdateMultipleAttributesDevices(char* devicename,int node,int promiscuous_mode,char* location,char* function_list,int first_time1,int last_rows1,char* ErrMess);


	EXTERN_CONFDB
		int UpdateMultipleLkTypeLinks(char* node_from,char* node_to,char* port_nbrfrom,char* port_nbrto,char* port_typefrom,char* port_typeto,char* link_type,int first_time1, int last_rows1,char* ErrMess);


	EXTERN_CONFDB
		int UpdateMultipleLkUsedLinks(char* node_from,char* node_to,char* port_nbrfrom,char* port_nbrto,char* port_typefrom,char* port_typeto,int lkused,int first_time1, int last_rows1,char* ErrMess);


	EXTERN_CONFDB
		int UpdateMultipleBidirectionalLinks(char* node_from,char* node_to,char* port_nbrfrom,char* port_typefrom,char* port_nbrto,char* port_typeto,int bidirectional_link_used,int first_time1, int last_rows1,char* ErrMess);


	EXTERN_CONFDB
		int DBDeconnexion(char* ErrorMessage);

	EXTERN_CONFDB
		int DBConnexion(char* server,char* usr,char* pwd,char* ErrorMessage);


	EXTERN_CONFDB
		int GetDeviceTypeRow(char* devitype,int &len_devtype,char* devtype_result,char* ErrorMessage);


	EXTERN_CONFDB
		int GetDeviceRow_devicename(char* functionaldeviname,int &len_device, char* device_result,char* ErrorMessage);


	EXTERN_CONFDB
		int GetDeviceRow_devid(int deviceID,int &len_device, char* device_result,char* ErrorMessage);


	EXTERN_CONFDB
		int GetPortRow_pid(int portID, int &len_port,char* port_row_result,char* ErrorMessage);

	EXTERN_CONFDB
		int GetPortRow_devname(char* devicename, char* port_nb,int port_way, char* port_type,int &len_port, char* port_row_result,char* ErrorMessage);


	EXTERN_CONFDB
		int GetPortRow_devid(int deviceid, char* port_nb,int port_way,char* port_type,int &len_port, char* port_row_result,char* ErrorMessage);


	EXTERN_CONFDB
		int GetMacroConnectivityRow_lkid(int lkID, int &len_conn,char* Conn_row,char* ErrorMessage);


	EXTERN_CONFDB
		int GetMacroConnectivityRow_node(int nodeID, char* port_nb, int port_way,char* port_type,int &len_conn, char* Conn_row,char* ErrorMessage);


	EXTERN_CONFDB
		int GetMacroConnectivityRow_nodename(char* node_name, char* port_nb, int port_way,char* port_type,int &len_conn, char* Conn_row,char* ErrorMessage);


	EXTERN_CONFDB
		int GetIPInfoRow(char* ip_address,int &len_ip, char*  IP_row,char* ErrorMessage);


	EXTERN_CONFDB
		int GetLkTypeRow_lkname(char* lktype_name,int &len_lktype,char* LkType_row,char* ErrorMessage);


	EXTERN_CONFDB
		int GetLkTypeRow_lknb(int lktype_nbr,int &len_lktype,char* LkType_row,char* ErrorMessage);


	EXTERN_CONFDB
		int GetLkTypeDecomposition_lknb(int lktype_nbr,int &len_array,char* LkType_row,char* ErrorMessage);



	EXTERN_CONFDB
		int GetDeviceNamesPerType(char* devitype, int &len_array, char* devIDs_list,char* ErrorMessage);


	EXTERN_CONFDB
		int GetDeviceIDsPerType(char* devitype, int &len_array, int* devIDs_list,char* ErrorMessage);


	EXTERN_CONFDB
		int GetLkFromDevID(int node_from, int &len_array, int* lkfrom_list,char* ErrorMessage);

	EXTERN_CONFDB
		int GetLkToDevID(int node_to, int &len_array, int* lkfrom_list,char* ErrorMessage);


	EXTERN_CONFDB
		int GetPortIDPerDevID(int devID, int &len_array, int* portID_list,char* ErrorMessage);


	EXTERN_CONFDB
		int GetMacIPAddPerDevID(int devID, int &len_mac,char* MacAdd_list,char* ErrorMessage);

	EXTERN_CONFDB
		int GetDestinationNamePerDevPort(char* devname,char* port_nb, char* port_type,int &len_Destin_list,char* Destin_list,char* ErrorMessage);

	EXTERN_CONFDB
		int GetFreeDeviceNamesPerType(char* devitype, int &len_array, char* devIDs_list,char* ErrorMessage);


	EXTERN_CONFDB
		int GetDeviceID_devicename(char* deviname,int &deviceID,char* ErrorMessage);

	EXTERN_CONFDB
		int GetDeviceName_deviceid(int deviceID,char* devicename,char* ErrorMessage);

	EXTERN_CONFDB
		int GetPortID_portinfo(int deviceID,char* port_nb,char* port_type,int port_way,int &portID,char* ErrorMessage);



	EXTERN_CONFDB
		int GetDetailedConnectivityBetweenDevices(char* dev_from, char* dev_to, char* nfrom_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen,char* pfrom_list,char* pto_list,int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess);



	EXTERN_CONFDB
		int GetDetailedConnBetweenDeviceDevType(char* dev_from, char* dev_to,int devto_type_given,  char* nfrom_list,char* pfrom_list,char* pto_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen, int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess);

	EXTERN_CONFDB
		int GetConnectivityBetweenDevices(char* dev_from, char* dev_to, int& len_array, int &pfrom1_arraylen,int &nfrom1_arraylen,int &pto1_arraylen,int &nto1_arraylen,int &lkinfo_arraylen,char* nfrom1_list,char* nto1_list,char* pfrom1_list,int* pfrom_list,char* pto1_list,int* pto_list,int* pathid_list,int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess);

	EXTERN_CONFDB
		int GetConnOutBetweenDeviceDeviceType(char* dev_from, char* devtype,int devtype_give, int& len_array, char* pfrom_arraylist, int reload_connectivity,int delete_connectivity,char* ErrMess);




	EXTERN_CONFDB
		int CreateDestinationTable(char* devicename, int round_trip_max,char* ErrMess);

	EXTERN_CONFDB
		int CreateRoutingTable(char* devicename, int round_trip_max,char* ErrMess);

	EXTERN_CONFDB
		int DropTableSchema(char* ErrMess);

	EXTERN_CONFDB
		int CreateTableSchema(char* ErrMess);


	EXTERN_CONFDB
		int GetBusySubsystems(int &len_array1,char* ListSubsystemUsed,char* ListSubsystemUsed1,int &len_array, int* ListSubsystemState, char* ErrorMessage);


	EXTERN_CONFDB int LoadLinkTypeTable(int &len_lkid, int* lkid_list,int& len_lkname,char* lktypename_list,char* ErrMess);


	EXTERN_CONFDB int LoadDeviceTypeTable(char* systemnameList,int &len_did, char* devtypename_list,int &len_array,int* devtypeID_list, char* ErrMess);


	EXTERN_CONFDB int MatchDeviceIDDeviceName(char* systemnameList,int &len_did, int* devid_list,int& len_dname,char* devname_list,char* ErrMess);


	EXTERN_CONFDB int LoadConnectivityTable(char* systemnameList,int &len_array, int* lkid_list, int * node_from_list,int* node_to_list,int &portfrom_len,int &portto_len,char* port_nbrfrom_list, char* port_nbrto_list, int* bidirectional_list, int* lkused_list, int* lktype_list,char* lkinfo_list,int & lkinfo_len,char* ErrMess);


	EXTERN_CONFDB
		int InsertPort(char* devicename,char* port_nb,int admin_status,int port_way,int speed,int pxi_booting,char* port_type,char* bia,char* ipadd1,char* ipname,char* subnet,char* macadd,char* phy,int last_rows,char* ErrMess);


	EXTERN_CONFDB
		int InsertCompositeLinkType( char* link_name,char * simple_lk_list,int last_rows,char* ErrMess);


	EXTERN_CONFDB
		int InsertSimpleLinkType(char *link_name,int last_rows,char* ErrMess);


	EXTERN_CONFDB
		int InsertMacroLink(char* node_from,char* node_to,char* port_nbfrom,char* port_nbto,char* port_typefrom,char* port_typeto,char* link_type,char* link_information,int bidirectional_link_used, int last_rows,char* ErrMess);


	EXTERN_CONFDB
		int InsertFunctionalDevice(char* systemnameList,char* devicename,char* devicetype,int node,int promiscuous_mode,char* serial_nb,char* hwtype,char* responsible,char* comments,char* location,char* function_list,int last_rows,char* ErrMess);


	EXTERN_CONFDB
		int InsertDeviceType(char* systemnameList,char* devicetype,int nbrofinput,int nbrofoutput, char* description, char* rgbcolor,int last_rows,char* ErrMess);

	EXTERN_CONFDB
		int InsertMultiplePorts(char* devicename,char* port_nb,int admin_status,int port_way,int speed,int pxi_booting,char* port_type,char* bia,char* ipadd1,char* ipname,char* subnet,char* macadd,char* phy,int first_time1,int last_rows1,char* ErrMess);


	EXTERN_CONFDB
		int InsertMultipleSimpleLinkTypes( char *link_name,int first_time1,int last_rows1,char* ErrMess);


	EXTERN_CONFDB
		int InsertMultipleCompositeLinkTypes( char* link_name,char * simple_lk_list,int first_time1,int last_rows1,char* ErrMess);


	EXTERN_CONFDB
		int InsertMultipleDeviceTypes(char* systemnameList,char* devicetype,int nbrofinput,int nbrofoutput, char* description,char* rgbcolor,int first_time1, int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int InsertMultipleFunctionalDevices(char* systemnameList,char* devicename,char* devicetype,int node,int promiscuous_mode,char* serial_nb,char* hwtype,char* responsible,char* comments,char* location,char* function_list,int first_time1,int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int InsertMultipleSpareDevices(char* hwname,char* hwtype,char* serial_nb,char* responsible,char* comments,char* location,int first_time1,int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int InsertSpareDevice(char* hwname,char* hwtype,char* serial_nb,char* responsible,char* comments,char* location,int last_rows1,char* ErrMess);


	EXTERN_CONFDB
		int InsertMultipleMacroLinks(char* node_from,char* node_to,char* port_nbfrom,char* port_nbto,char* port_typefrom,char* port_typeto,char* link_type,char* link_information,int bidirectional_link_used,int first_time1, int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int InsertMultipleSparePorts(char* serialnb,char* port_nb,int port_way,char* port_type,char* bia,char* macadd,int first_time1,int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int InsertSparePort(char* serialnb,char* port_nb,int port_way,char* port_type,char* bia,char* macadd,int last_rows,char* ErrMess);


	EXTERN_CONFDB
		int GetDeviceNamesPerLocation(char* location, int &len_array, char* devnames_list,char* ErrorMessage);

	EXTERN_CONFDB
		int GetSpareHWPerLocation(char* location, int &len_array, char* devnames_list,char* ErrorMessage);

	EXTERN_CONFDB
		int GetSpareHWPerType(char* hwtype, int &len_array, char* devnames_list,char* ErrorMessage);

	EXTERN_CONFDB
		int InsertTestFunctionalDevice(char* location,char* ipaddressList,int ipaddlen,char* ipnameList,int ipnamelen,char* subnetList,int subnetlen,char* portnbList,int portnblen,char* portypeList,int ptypelen,int* portwayList,int nb_ipadd,int last_rows,char* ErrMess);

	EXTERN_CONFDB
		int GetFunctionalDeviceStatus(char* devicename,char* device_status ,char* ErrMess);

	EXTERN_CONFDB
		int GetHWDeviceStatus(char* serialnb,char* device_status ,char* ErrMess);

	EXTERN_CONFDB
		int ReplaceFunctionalDevice(char* devicename,char* new_device_status,char* new_location,char* user_comments,char* status_datechange,char* serialnb_replaced,char* replace_date,char* ErrMess);

	EXTERN_CONFDB
		int SetToTestUseStatus(char* devicename,char* user_comments,char* status_change,char* serialnb_replaced,char* testboard_name,char* replace_date,char* ErrMess);

	EXTERN_CONFDB
		int GetHWDeviceRow_serialnb(char* serialnb,int &len_device, char* device_result,char* ErrorMessage);

	EXTERN_CONFDB
		int GetFunctionalDeviceByStatus(char* system_name, char* device_status, int &len_devlist , char* device_list,char* ErrMess);

	EXTERN_CONFDB
		int UpdateHWDeviceStatus(char* serialnb,char* new_device_status,char* new_location,char* user_comments,char* status_datechange,char* functional_devicename,char* ErrMess);

	EXTERN_CONFDB
		int GetHWLastFunctionalDevice(char* serialnb,char* functional_devicename,int &len_functionaldname ,char* ErrMess);

	EXTERN_CONFDB
		int GetFunctionalDeviceLastHW(char* functional_devicename,char* serialnb,int &len_serialnb ,char* ErrMess);

	EXTERN_CONFDB
		int GetHistoryOfHWDevice(char* serialnb,char* functionaldevice_history,int &len_history , char* min_date, char* max_date,char* ErrMess);

	EXTERN_CONFDB
		int GetHWDeviceByStatus(char* system_name, char* device_status,int &len_status ,char* functionaldevice_status,char* ErrMess);

	EXTERN_CONFDB
		int GetHistoryOfFunctionalDevice(char* functional_devicename,char* functionaldevice_history,int &len_history , char* min_date, char* max_date,char* ErrMess);

	EXTERN_CONFDB
		int GetBoardCpntRow_cpntname(char* functionalcpntname,int &len_cpnt, char* cpnt_result,char* ErrorMessage);

	EXTERN_CONFDB
		int GetBoardCpntRow_cpntid(int cpntID,int &len_cpnt, char* cpnt_result,char* ErrorMessage);

	EXTERN_CONFDB
		int GetMicroConnectivityRow_lkid(int lkID, int &len_conn,char* Conn_row,char* ErrorMessage);

	EXTERN_CONFDB
		int GetMicroConnectivityRow_node(int cpntID, int port_nb, int port_way,int &len_conn, char* Conn_row,char* ErrorMessage);

	EXTERN_CONFDB
		int GetMicroConnectivityRow_cpntname(char* cpnt_name, int port_nb, int port_way,int &len_conn, char* Conn_row,char* ErrorMessage);
	EXTERN_CONFDB
		int GetBoardCpntPerType(char* cpnttype, int &len_array, char* cpntIDs_list,char* ErrorMessage);

	EXTERN_CONFDB
		int GetMicroLkFromCpntID(int cpntid_from,int motherboardID, int &len_array, int* lkfrom_list,char* ErrorMessage);

	EXTERN_CONFDB
		int GetMicroLkToCpntID(int cpnt_to,int motherboardID, int &len_array, int* lkfrom_list,char* ErrorMessage);

	EXTERN_CONFDB
		int GetCpntID_cpntname(char* cpntname,int &cpntID,char* ErrorMessage);

	EXTERN_CONFDB
		int GetCpntName_cpntid(int cpntID,char* cpntname,char* ErrorMessage);

	EXTERN_CONFDB
		int GetCpntNamesPerBoard(char* motherboardname, int &len_array, char* devnames_list,char* ErrorMessage);

	EXTERN_CONFDB
		int GetSpareHWCpntPerLocation(char* location, int &len_array, char* devnames_list,char* ErrorMessage);

	EXTERN_CONFDB
		int GetSpareHWCpntPerType(char* hwtype, int &len_array, char* devnames_list,char* ErrorMessage);

	EXTERN_CONFDB
		int GetHWCpntRow_serialnb(char* serialnb,int &len_device, char* device_result,char* ErrorMessage);

	EXTERN_CONFDB
		int GetHWCpntRow_snbid(int snbid,int &len_device, char* device_result,char* ErrorMessage);

	EXTERN_CONFDB int UpdateMultipleDeviceSystemList(char* devicename,char* new_systemList,int first_time1,int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int GetFunctionalBoardCpntStatus(char* cpntname,char* cpnt_status ,char* ErrMess);

	EXTERN_CONFDB
		int GetHWBoardCpntStatus(char* serialnb,int snbid,char* cpnt_status ,char* ErrMess);


	EXTERN_CONFDB
		int ReplaceFunctionalBoardCpnt(char* cpntname,char* new_cpnt_status,char* new_location,char* user_comments,char* status_datechange,char* serialnb_replace,char* replace_date,char* ErrMess);


	EXTERN_CONFDB
		int UpdateHWBoardCpntStatus(char* serialnb,char* new_cpnt_status,char* new_location,char* user_comments,char* status_datechange,char* functional_cpntname,char* ErrMess);

	EXTERN_CONFDB
		int GetHWLastFunctionalBoardCpntName(char* serialnb,int snbid,char* functional_cpntname,int &len_functionaldname ,char* ErrMess);

	EXTERN_CONFDB
		int GetFunctionalBoardCpntNameLastHW(char* functional_cpntname,char* serialnb,int &len_serialnb ,char* ErrMess);

	EXTERN_CONFDB
		int GetHistoryOfFunctionalBoardCpntName(char* functional_cpntname,char* functionalcpnt_history,int &len_history , char* min_date, char* max_date,char* ErrMess);

	EXTERN_CONFDB
		int GetHistoryOfHWBoardCpnt(char* serialnb,int snbid,char* functionalcpnt_history,int &len_history , char* min_date, char* max_date,char* ErrMess);

	EXTERN_CONFDB
		int GetHWBoardCpntByStatus(char* cpnt_status,char* functionalcpnt_status,int &len_status , char* ErrMess);

	EXTERN_CONFDB
		int InsertMultipleBoardCpnts(char* cpntname,char* cpnttype,int replacable,char* motherboardname,char* serial_nb,char* hwtype,char* responsible,char* comments,char* location,int first_time1,int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int InsertMultipleMicroLinks(char* node_from,char* node_to,int port_nbfrom,int port_nbto,char* link_type,int bidirectional_link_used,int first_time1, int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int GetMicroConnectFromPortid(int mboardportid_from, int mboardportid_to, char* cpntname,int* pathid_list, int* link_pos_list,int &len_array,char* nfrom_list,int &nfrom_arraylen,char* nto_list,int &nto_arraylen,char* last_nfrominfo,int &len_lastnfrominfo,char* last_ntoinfo,int &len_lastntoinfo,char* ErrMess);


	EXTERN_CONFDB
		int GetMicroConnectToCpntType(int mboardportid_from, int cpnttype_given, char* cpnttype,int* pathid_list, int* link_pos_list,int &len_array,char* nfrom_list,int &nfrom_arraylen,char* nto_list,int &nto_arraylen,char* last_nfrominfo,int &len_lastnfrominfo,char* last_ntoinfo,int &len_lastntoinfo,char* ErrMess);


	EXTERN_CONFDB
		int GetMicroConnectBetweenBoardCpntAndMotherBoard(char* cpntname_from, int mboard_portid, int* pathid_list, int* link_pos_list,int &len_array,char* nfrom_list,int &nfrom_arraylen,char* nto_list,int &nto_arraylen,char* last_nfrominfo,int &len_lastnfrominfo,char* last_ntoinfo,int &len_lastntoinfo,char* ErrMess);


	EXTERN_CONFDB
		int DeleteLinkRow(int linkid,int macro_link,char* ErrorMessage);





	EXTERN_CONFDB
		int GetPortPerSubsystem(char* dev_from, char* subsystem_name, int& len_array, char* pfrom_list, char* ErrMess);


	EXTERN_CONFDB int SwapTwoDevices(char* functional_devicename1,char* functional_devicename2,char* comments,char* ErrMess);



	EXTERN_CONFDB
		int GetAllPathsPerDevice(char* systemnameList,char* devname,int& len_array, int* lkid_list5,int* pathid_list,int* link_pos_list,int reload_connectivity,int delete_connectivity,char* ErrMessage);


	EXTERN_CONFDB
		int LoadRoutingtable( char* switch_name,int &len_array_destinname,char* destinname_list, int &len_array_destinip,char* destinip_list, int &len_array_portnext,char* port_list,int &len_array_ipaddnext,char* ipaddnext_list,int &len_array_subnetnext,char* subnetnext_list,int &len_array_macaddnext,char* macaddnext_list,char* ErrMess);

	EXTERN_CONFDB
		int DeletePortRow(int portid,char* ErrMess);
	EXTERN_CONFDB
		int GetDetailedConnectivityBetweenDevices_plsql(char* dev_from, char* dev_to, char* nfrom_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen,char* pfrom_list,char* pto_list,int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list,char* ErrMess);
	EXTERN_CONFDB
		int GetDetailedConnBetweenDeviceDevType_plsql(char* dev_from, char* devto_type,int devto_type_given, char* nfrom_list,char* pfrom1_list,char* pto1_list,int& len_array,int & nfrom_arraylen,int & pfrom1_arraylen, int & nto_arraylen,int & pto1_arraylen, int &lkinfo_arraylen,int* pfrom_list,int* pto_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list,char* ErrMess);

	EXTERN_CONFDB
		int InsertMultipleBootImages(char* devicename,char* boot_image,char* kernel_image_location,char* initrd_image_location,char* physical_location,char*  boot_protocol,int first_time1, int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int DeleteBootImage(char* devicename,char* ErrMess);

	EXTERN_CONFDB
		int UpdateBootImage(char* devicename,char* boot_image,char* kernel_image_location,char* initrd_image_location,char* physical_location,char*  boot_protocol,char* ErrMess);

	EXTERN_CONFDB
		int GetBootImageRow_devicename(char* functionaldeviname,int &len_device, char* device_result,char* ErrorMessage);


	EXTERN_CONFDB
		int GetDevicesPerSystem(char* system_name,int method_used,int &len_array, int* ListSubsystemState, char* ErrorMessage);

	EXTERN_CONFDB
		int InsertSimpleDeviceFunction(char *function_name,int last_rows,char* ErrMess);

	EXTERN_CONFDB int UpdateDeviceFunctionName(char* old_function_name,char* new_function_name,char* ErrMess);

	EXTERN_CONFDB int UpdateHWSerialNB(char* old_serialnb,char* new_serialnb,char* ErrMess);

	EXTERN_CONFDB
		int GetDeviceNamesPerFunction(char* function, int &len_array, char* devnames_list,char* ErrorMessage);

	EXTERN_CONFDB
		int InsertMultipleIPAliases(char* ipaddress,char* ipname,char* ipalias,int first_time1,int last_rows1,char* ErrMess);

	EXTERN_CONFDB int UpdateIPalias(char* old_ipalias,char* new_ipalias,char* ErrMess);

	EXTERN_CONFDB
		int GetIPAliasesPerIPName(char* ipname, int &len_array, char* ipaliases_list,char* ErrorMessage);

	EXTERN_CONFDB
		int GetIPAliasRow(char* ipalias,int &len_ipalias,char* ipalias_result,char* ErrorMessage);

	EXTERN_CONFDB
		int GetLkIDsPerLkInfo(char* lkinfo, int &len_array, int* lkIDs_list,char* ErrorMessage);

	EXTERN_CONFDB
		int DeleteIPAlias(char* ipalias,char* ErrMess);
	EXTERN_CONFDB
		int GetAvailableFunctions(int &len_array, char* function_list,char* ErrorMessage);

	EXTERN_CONFDB int UpdateMultipleDeviceTypeSystemList(char* devicetype,char* new_systemList,int first_time1,int last_rows1,char* ErrMess);

	EXTERN_CONFDB
		int DeleteHWDevice(char* serialnb,char* ErrMess);

	EXTERN_CONFDB
		int DeleteFunctionalDeviceType(char* devicetype,char* ErrMess);

	EXTERN_CONFDB
		int DeleteFunctionalDevice(int deviceid,char* ErrMess);

	EXTERN_CONFDB
		int InsertSubsystem(char *system_name,char* parent_sysname,char* ErrMess);

	EXTERN_CONFDB int UpdateSubsystemName(char* old_systemname,char* new_systemname,char* ErrMess);

	EXTERN_CONFDB
		int DeleteSystemName(char* sysname,char* ErrMess);

	EXTERN_CONFDB
		int GetListOfSubsystems(int &len_array, char* sysname_list,char* ErrorMessage);

	EXTERN_CONFDB
		int GetSpareHWTypeList(int &len_array, char* hwtypes_list,char* ErrorMessage);

	EXTERN_CONFDB
		int GetPortInfoPerSpare(char* serialnb, int &len_array, char* port_list,char* ErrorMessage);

	EXTERN_CONFDB
		int GetSparePortRow_snb(char* serialnb, char* port_nb,int port_way, char* port_type,int &len_port, char* port_row_result,char* ErrorMessage);
	///////////////
	//////////////
	EXTERN_CONFDB	//DBConnexion
		int DBConn(char* server,char* usr,char* pwd,char* ErrorMessage);
	EXTERN_CONFDB	//
		int _GetConnectivityBetweenDevices(char* dev_from, char* dev_to, int& len_array, int &pfrom1_arraylen,int &nfrom1_arraylen,int &pto1_arraylen,int &nto1_arraylen,int &lkinfo_arraylen,char* nfrom1_list,char* nto1_list,char* pfrom1_list,int* pfrom_list,char* pto1_list,int* pto_list,int* pathid_list,int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess);
	EXTERN_CONFDB
		int _GetConnOutBetweenDeviceDeviceType(char* dev_from, char* devtype,int devtype_give, int& len_array, char* pfrom_arraylist, int reload_connectivity,int delete_connectivity,char* ErrMess);
	EXTERN_CONFDB
		int _GetDetailedConnectivityBetweenDevices(char* dev_from, char* dev_to, char* nfrom_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen,char* pfrom_list,char* pto_list,int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess);
	EXTERN_CONFDB
		int _GetDetailedConnBetweenDeviceDevType(char* dev_from, char* dev_to,int devto_type_given,  char* nfrom_list,char* pfrom_list,char* pto_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen, int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess);
	EXTERN_CONFDB
		int _GetAllPathsPerDevice(char* systemnameList,char* devname,int& len_array, int* lkid_list5,int* pathid_list,int* link_pos_list,int reload_connectivity,int delete_connectivity,char* ErrMessage);
////
	EXTERN_CONFDB
		int _GetDeviceTypeRow(char* devitype,int &len_devtype,char* devtype_result,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetDeviceRow_devicename(char* functionaldeviname,int &len_device, char* device_result,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetDeviceRow_devid(int deviceID,int &len_device, char* device_result,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetPortRow_pid(int portID, int &len_port,char* port_row_result,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetPortRow_devname(char* devicename, char* port_nb,int port_way, char* port_type,int &len_port, char* port_row_result,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetPortRow_devid(int deviceid, char* port_nb,int port_way,char* port_type,int &len_port, char* port_row_result,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetMacroConnectivityRow_lkid(int lkID, int &len_conn,char* Conn_row,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetMacroConnectivityRow_node(int nodeID, char* port_nb, int port_way,char* port_type,int &len_conn, char* Conn_row,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetMacroConnectivityRow_nodename(char* node_name, char* port_nb, int port_way,char* port_type,int &len_conn, char* Conn_row,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetIPInfoRow(char* ip_address,int &len_ip, char*  IP_row,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetLkTypeRow_lkname(char* lktype_name,int &len_lktype,char* LkType_row,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetLkTypeRow_lknb(int lktype_nbr,int &len_lktype,char* LkType_row,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetLkTypeDecomposition_lknb(int lktype_nbr,int &len_array,char* LkType_row,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetDeviceNamesPerType(char* devitype, int &len_array, char* devIDs_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetDeviceIDsPerType(char* devitype, int &len_array, int* devIDs_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetLkFromDevID(int node_from, int &len_array, int* lkfrom_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetLkToDevID(int node_to, int &len_array, int* lkfrom_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetPortIDPerDevID(int devID, int &len_array, int* portID_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetPortInfoPerSpare(char* serialnb, int &len_array, char* port_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetMacIPAddPerDevID(int devID, int &len_mac,char* MacAdd_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetDestinationNamePerDevPort(char* devname,char* port_nb,char* port_type, int &len_Destin_list,char* Destin_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetFreeDeviceNamesPerType(char* devitype, int &len_array, char* devIDs_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetDeviceID_devicename(char* deviname,int &deviceID,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetPortID_portinfo(int deviceID,char* port_nb,char* port_type,int port_way,int &portID,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetDeviceName_deviceid(int deviceID,char* devicename,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetDeviceNamesPerLocation(char* location, int &len_array, char* devnames_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetDeviceNamesPerFunction(char* function, int &len_array, char* devnames_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetSpareHWPerLocation(char* location, int &len_array, char* devnames_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetSpareHWPerType(char* hwtype, int &len_array, char* devnames_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetHWDeviceRow_serialnb(char* serialnb,int &len_device, char* device_result,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetIPAliasesPerIPName(char* ipname, int &len_array, char* ipaliases_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetAvailableFunctions(int &len_array, char* function_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetIPAliasRow(char* ipalias,int &len_ipalias,char* ipalias_result,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetLkIDsPerLkInfo(char* lkinfo, int &len_array, int* lkIDs_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetListOfSubsystems(int &len_array, char* sysname_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetSpareHWTypeList(int &len_array, char* hwtypes_list,char* ErrorMessage);
	EXTERN_CONFDB
		int _GetSparePortRow_snb(char* serialnb, char* port_nb,int port_way, char* port_type,int &len_port, char* port_row_result,char* ErrorMessage);


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

