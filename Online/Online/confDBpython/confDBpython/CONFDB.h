/********************************************************************************/
//Author: L.Abadie
// version:v3.1
// wrapper to interface it with Python (can also be used a C++ wrapper)
/********************************************************************************/

#include <string>
#include "cicDB.h"
#include "oci.h"
#include <iostream>
#include <vector>
using namespace std;
/**********************************************************************************************/
/**
       * CONFDB class .
       */
/**********************************************************************************************/

class CONFDB
{
private:
	string _dbname;
	string _login;
	string _passwd;

public:

  OCIEnv* ociEnv;
  OCIError* ociError;
  OCISvcCtx* ociHdbc;

	
	CONFDB(string dbname,string login,string passwd);
	~CONFDB();
	int PyDBConnexion()  ;
	//int PyDBConnexion(string dbname,string login,string passwd);
	int PyDBDeconnexion()  ;
	string  PyGetDeviceTypeRow(string devitype)  ;
	string  PyGetDeviceRow_devicename(string deviname)  ;
	string  PyGetDeviceRow_deviceid(int devid)  ;
	string PyGetPortRow_pid(int pID)  ;
	string PyGetPortRow_devid(int devid,string port_nb,int port_way,string port_type)  ;
	string PyGetMacroConnectivityRow_lkid(int lkid)  ;
	string PyGetMacroConnectivityRow_node(int nodeid,string port_nb,int way_given,string port_type)  ;
	string PyGetIPInfoRow(string IPadd)  ;
	string PyGetLkTypeRow_lkname(string lname)  ;
	string PyGetLkTypeRow_lknb(int lknb)  ;
	vector<int> PyGetPortIDPerDevID(int dID)  ;
	vector<string> PyGetDeviceNamesPerType(string dtype)  ;
	vector<string> PyGetMacIPAddPerDevID(int dID)  ;
	vector<int> PyGetLkToDevID(int nto)  ;
	vector<int> PyGetLkFromDevID(int nfrom)  ;
	vector<int> PyGetDeviceIDsPerType(string dtype)  ;
	string PyInsertPort(string devname,string ptnb,string port_type,int port_way,string bia,string ipname,string ipadd,string subnet,string macadd,vector<int> param_list, string phy,int last) ;
	string PyInsertMultiplePorts(string devname,string ptnb,int port_way,string port_type,string bia,string ipname,string ipadd,string subnet,string macadd,vector<int> param_list, string phy,int first,int last)  ;
	string PyInsertMultipleMacroLinks(string nfrom,string nto,string pfrom,string pto,string port_typefrom,string port_typeto,string lk_type ,int bidirectional,string link_info,int first,int last)   ;
	string PyInsertMultipleCompositeLinkTypes(string lktype,string lktype_list_concan,int first,int last)  ;
	string PyInsertMultipleSimpleLinkTypes(string lktype,int first,int last)  ;
	//string PyInsertMultipleFunctionalDevices(string sysname,string devname,string devtype,int dnode,int promismode,string serialnb,string hwtype,string responsible,string location,string comments,int first,int last)   ;
	string PyInsertMultipleDeviceTypes(string sysname,string devtype,int nb_in,int nb_out,string devdescr,string rgbcolor,int first,int last) ;
	string PyInsertMacroLink(string nfrom,string nto,string pfrom,string pto,string port_typefrom,string port_typeto,string lk_type ,int bidirectional,string link_info,int last) ;
	string PyInsertCompositeLinkType(string lktype,string lktype_list_concan,int last)  ;
	//string PyInsertFunctionalDevice(string sysname,string devname,string devtype,int dnode,int promismode,string serialnb,string hwtype,string responsible,string location,string comments,int last) ;
	string PyInsertDeviceType(string sysname,string devtype,int nb_in,int nb_out,string  devdescr,string rgbcolor,int last)  ;
	string PyInsertSimpleLinkType(string lktype,int last)  ;
	vector<string> PyGetConnOutBetweenDeviceDeviceType(string nfrom,string nto,int nto_dtype,int reload_connectivity,int delete_connectivity)  ;
	vector<string> PyGetDetailedConnectivityBetweenDevices(string nfrom,string nto,int reload_connectivity,int delete_connectivity)  ;
	int PyCreateTableSchema()  ;
	vector<string> PyGetConnectivityBetweenDevices(string nfrom,string nto,int reload_connectivity,int delete_connectivity)  ;
	vector<string> PyGetDestinationNamePerDevPort(string devtype,string  port_nb,string port_type)  ;
	int PyGetDeviceID_devicename(string  devname)  ;
	string  PyGetDeviceName_deviceid(int devID)  ;
	vector<string> PyGetDetailedConnBetweenDeviceDevType(string nfrom,string nto,int dtype_given,int reload_connectivity,int delete_connectivity)  ;
	int PyDropTableSchema()  ;
	vector<string> PyGetFreeDeviceNamesPerType(string dtype)  ;
	int  PyCreateDestinationTable(string dname,int roundtrip)  ;
	int  PyCreateRoutingTable(string  devname, int  roundtrip)  ;
	vector<string>  PyGetAllPathsPerDevice(string systemname,string devicename,int reload_connectivity,int delete_connectivity)  ;
	vector<string> PyMatchDeviceIDDeviceName (string systemname)  ;
	vector<string> PyLoadConnectivityTable (string systemname)  ;
	vector<string> PyLoadDeviceTypeTable (string systemname)  ;
	vector<string> PyLoadLinkTypeTable ()  ;
	string PyUpdateMultiplePorts(string devicename, string port_nb, int port_way,string port_type,int speed,string phy, int pxi_booting,int first_time1,int last_rows1);
	string PyUpdateMultipleAttributeMacIPs(string ip_add,string subnet_mask,string ipname,int first_time1,int last_rows1);
	string PyUpdateMultipleIPAddresses(string ip_add,string oldip_add,int first_time1,int last_rows1);
	string PyUpdateMultipleCompositeLinkTypes(string link_name,string simple_lk_list,int first_time1,int last_rows1);
	string PyUpdateMultipleLinkTypeNames(string old_link_name, string link_name,int first_time1,int last_rows1);
	string PyUpdateMultipleDeviceTypeAttributes(string devicetype,string description,int nbrofinput,int nbrofoutput,string rgbcolor,int first_time1,int last_rows1);
	string PyUpdateMultipleDeviceTypes(string devicetype_old,string devicetype,int first_time1,int last_rows1);
	//string PyUpdateMultipleAttributesDevices(string devicename,int node,int promiscuous_mode,string location,int first_time1,int last_rows1);
	string PyUpdateMultipleDevNamesDevices(string devicename_old,string devicename,int first_time1,int last_rows1);
	string PyUpdateMultipleBidirectionalLinks(string node_from,string node_to,string port_nbrfrom,string port_typefrom,string port_nbrto,string port_typeto,int bidirectional_link_used,int first_time1, int last_rows1);
	string PyUpdateMultipleLkUsedLinks(string node_from,string node_to,string port_nbrfrom,string port_nbrto,string port_typefrom,string port_typeto,int lkused,int first_time1, int last_rows1);
	string PyUpdateMultipleLkTypeLinks(string node_from,string node_to,string port_nbrfrom,string port_nbrto,string port_typefrom,string port_typeto,string link_type,int first_time1, int last_rows1);
	string  PyGetBoardCpntRow_cpntname(string cpntname);
	string  PyGetHWCpntRow_serialnb(string serialnb);  
	vector<string> PyGetDeviceNamesPerLocation(string location);  
	string  PyGetBoardCpntRow_cpntid(int cpntid);
	string   PyGetHWCpntRow_snbid(int snbid);
	string  PyGetMicroConnectivityRow_lkid(int lkid);
	string  PyGetMicroConnectivityRow_node(int nodeid,int port_nb,int way_given);
	vector<string>  PyGetBoardCpntPerType(string cpnttype);
	vector<string>  PyGetSpareHWPerLocation(string location);
	vector<string>  PyGetSpareHWCpntPerLocation(string location);
	vector<string>  PyGetSpareHWPerType(string hwtype);  
	vector<string>  PyGetCpntNamesPerBoard(string motherboardname);
	vector<string>  PyGetSpareHWCpntPerType(string hwtype);
	vector<int>  PyGetMicroLkFromCpntID(int cpntid_from, int motherboard); 
vector<int>  PyGetMicroLkToCpntID(int cpntid_from, int motherboard);
string  PyInsertTestFunctionalDevice(string location,vector<string> ipaddList,vector<string> ipnameList,vector<string> subnetList,vector<string> portnbList,vector<string> port_typeList, vector<int> port_wayList,int last);
string  PyInsertMultipleSpareDevices(string hwname,string hwtype,string serialnb,string responsible,string location,string comments,int first,int last);
string  PyInsertMultipleBoardCpnts(string cpntname,string cpnttype,int replacable,string motherboardname,string serialnb,string hwtype,string responsible,string location,string comments,int first,int last);
string  PyInsertMultipleSparePorts(string serialnb,string ptnb,int port_way,string port_type,string bia,string macadd, int first,int last);

string  PyInsertSparePort(string serialnb,string ptnb,int port_way,string port_type,string bia,string macadd, int last);

string  PyInsertSpareDevice(string hwname,string hwtype,string serialnb,string responsible,string location,string comments,int last);
string  PyInsertMultipleMicroLinks(string nfrom,string nto,int pfrom,int pto,string lk_type ,int bidirectional,int first,int last);
string   PyGetCpntName_cpntid(int cpntID);
int  PyGetCpntID_cpntname(string  cpntname);

string  PyUpdateMultipleDeviceSystemList(string devicename,string new_systemList,int first_time1, int last_rows1);
vector<string> PyGetMicroConnectFromPortid(int mboardportid_from,int mboardportid_to,string cpntname);
vector<string> PyGetMicroConnectToCpntType(int mboardportid_from,int cpnttype_given,string cpnttype);
vector<string> PyGetMicroConnectBetweenBoardCpntAndMotherBoard(string cpntname_from,int mboard_portid);
string PyDeleteLinkRow(int linkid,int macro_link);  
string PyDeletePortRow(int portid);  
vector<string> PyGetLkTypeDecomposition_lknb(int lktypeID);
string PyUpdateMultipleDTypeDevices(string devicename,string devicetype,int first_time1,int last_rows1);
string PyUpdateMultipleLkInfoLinks(string node_from,string node_to,string port_nbrfrom,string port_typefrom,string port_nbrto,string port_typeto,string link_info,int first_time1, int last_rows1);
string PyGetBootImageRow_devicename(string functionaldeviname);
string PyDeleteBootImage(string devicename);
string PyInsertMultipleBootImages(string devicename,string boot_image,string kernel_image_location,string initrd_image_location,string physical_location,string  boot_protocol,int first_rows,int last_rows);
string PyUpdateBootImage(string devicename,string boot_image,string kernel_image_location,string initrd_image_location,string physical_location,string  boot_protocol);
int PyGetPortID_portinfo(int deviceid, string port_nb, string port_type, int port_way);
string PyDeleteIPAlias(string ipalias);
string PyInsertSimpleDeviceFunction(string function,int last); 

string PyInsertMultipleFunctionalDevices(string sysname,string devname,string devtype,int dnode,int promismode,string serialnb,string hwtype,string responsible,string location,string comments,string function_list,int first,int last)   ;
string PyUpdateMultipleAttributesDevices(string devicename,int node,int promiscuous_mode,string location,string function_list,int first_time1,int last_rows1);
string PyInsertFunctionalDevice(string sysname,string devname,string devtype,int dnode,int promismode,string serialnb,string hwtype,string responsible,string location,string comments,string function_list,int last) ;

vector<int> PyGetLkIDsPerLkInfo(string lkinfo);  
string  PyGetIPAliasRow(string ipalias) ;
vector<string> PyGetIPAliasesPerIPName(string IPName);
string PyUpdateIPalias(string old_ipalias,string new_ipalias);
string PyUpdateHWSerialNB(string old_serialnb,string new_serialnb);
string PyUpdateDeviceFunctionName(string old_function_name,string new_function_name);
vector<string> PyGetDeviceNamesPerFunction(string function_name);
string PyInsertMultipleIPAliases(string ip_address,string ipname,string ipalias,int first,int last);

string PyUpdateMultipleDeviceTypeSystemList(string devicetype,string new_systemList,int first_time1, int last_rows1);
vector<string> PyGetAvailableFunctions();  
string PyDeleteHWDevice(string serialnb);
string PyDeleteFunctionalDeviceType(string devicetype); 
string PyDeleteFunctionalDevice(int deviceid);
string PyDeleteSystemName(string system_name);
vector<string> PyGetListOfSubsystems();
string PyInsertSubsystem(string system_name,string parent_name);
string PyUpdateSubsystemName(string old_sysname,string new_sysname);

string PySwapTwoDevices(string functional_devname1,string functional_devname2,string comments);
string PyGetFunctionalDeviceStatus(string dname);
string PyGetFunctionalBoardCpntStatus(string cpntname);
string PyGetHWLastFunctionalDevice(string serialnb);
string PyGetHWLastFunctionalBoardCpntName(string hw_serialnb,int hw_snbid);
string PyGetFunctionalDeviceLastHW(string functional_name);
string PyGetFunctionalBoardCpntNameLastHW(string functional_cpntname);
string PyGetHWDeviceStatus(string dname);
string PyGetHWBoardCpntStatus(string serialnb, int snbid);
vector<string> PyGetHWDeviceByStatus(string systemname,string device_status);
vector<string> PyGetHWBoardCpntByStatus(string cpnt_status);
vector<string> PyGetHistoryOfHWDevice(string serial_nb,string min_date,string max_date);
vector<string> PyGetHistoryOfHWBoardCpnt(string serial_nb,int hw_snbid,string min_date,string max_date);
vector<string> PyGetHistoryOfFunctionalDevice(string functional_name,string min_date,string max_date); 

vector<string> PyGetHistoryOfFunctionalBoardCpntName(string functional_cpntname,string min_date,string max_date);
vector<string> PyGetFunctionalDeviceByStatus(string systemname,string device_status);
vector<string> PyGetSpareHWTypeList();
string  PyGetHWDeviceRow_serialnb(string serialnb);  
vector<string> PyGetPortInfoPerSpare(string snb) ;
string PyGetSparePortRow_snb(string serialnb,string port_nb,int port_way,string port_type)  ;
 
  













};





