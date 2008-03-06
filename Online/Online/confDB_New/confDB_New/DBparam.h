

static char PRIME_NUMBER_TABLE[20]="lhcb_prime_number";
static char DEVICETYPE_TABLE[31]="lhcb_lg_device_types";
static char LOGICAL_DEVICE_TABLE[31]="lhcb_lg_devices";
static char HW_DEVICE_TABLE[31]="lhcb_hw_devices";
static char HISTORY_DEVICE_TABLE[31]="lhcb_device_history";
static char HW_CPNTS_TABLE[31]="lhcb_hw_cpnts";
static char LG_CPNTS_TABLE[31]="lhcb_lg_cpnts";
static char HISTORY_CPNT_TABLE[31]="lhcb_cpnt_history";
static char LINKTYPE_TABLE[31]="lhcb_link_types";
static char MICRO_CONNECTIVITY_TABLE[31]="lhcb_microscopic_connectivity";
static char MACRO_CONNECTIVITY_TABLE[31]="lhcb_macroscopic_connectivity";
static char IPINFO_TABLE[31]="lhcb_ipinfo";
static char PORT_TABLE[31]="lhcb_port_properties";
static char HWPORT_TABLE[31]="lhcb_hwport_properties";
static char IPALIAS_TABLE[31]="lhcb_ipaliases";

static char DEVICEBOOTING_TABLE[31]="lhcb_device_booting";
static char DEVICETYPEBOOTING_TABLE[31]="lhcb_devicetype_booting";
static char PATHDETAILS_TABLE[31]="lhcb_pdetails";
static char DESTINATIONTABLE[31]="lhcb_destinationtable";
static char ROUTINGTABLE[31]="lhcb_routingtable";
static char DEVICE_FUNCTION_TABLE[31]="lhcb_DEVICE_FUNCTIONS";

static char SUBSYSTEM_TABLE[31]="lhcb_subsystemList";
static char PATH_TEMP[31]="lhcb_path_temp";
static char MICROPATH_TEMP[31]="lhcb_micropath_temp";
static char MICRODETPATH_TEMP[31]="lhcb_micropdet_temp";
static char SQLROWCOUNT[20]="SQL%ROWCOUNT";

static char _date_format[30]="YY/MM/DD/HH24/MI/SS";

static char _UpdateHistory[31]="UpdateDeviceHistory";
static char _UpdateBoardCpnt[100]="UpdateBoardCpnt";
static char _TestUseBoardCpnt[100]="TestUseBoardCpnt";
static char _InsertTestBoard[100]="InsertTestBoard";
static char   _createschema[100]="createtableschema";
static char   _dropschema[100]="droptableschema";
static char   _createRT[100]="routingtable_pck.createRT_all_devname";
static char   _createTD[100]="routingtable_pck.createTD_all_devname";

static char  _CreateMicroLgLinks[100]="routingtable_pck.CreateMicroNodeLink_tab";
static char  _CreateMicroPaths[100]="routingtable_pck.CreateMicroPathTab";
static char  _CreateMicroPaths_Host[100]="routingtable_pck.CreateMicroPathTab_Host";
static char  _InsertMicroPathUsingID[100]="routingtable_pck.InsertIntoMicroPathUsingID";
static char  _InsertMicroPathFromPortID[100]="routingtable_pck.InsertIntoMicroPathFromPortID";
static char  _InsertMicroPathCpntType[100]="routingtable_pck.InsertIntoMicroPathCnptType";
static char  _UpdatePathUsed[100]="routingtable_pck.updatepathused";
static char  _UpdateDynamicTables[100]="routingtable_pck.CheckPathTabCompleteAndUpdate";
static char  _SwapTwoDevices[100]="SwapTwoDevices";
static char _DeleteLinksInPaths[100]="routingtable_pck.deleteLinksInPaths";
static char _updateIPaddress[100]="updateIPaddress";
static char _insertIpaddress[100]="insertIPaddress";
static char _GiveDestinTabName[100]="GiveDestinationTableName";
static char  _GiveDestinType[100]="GiveDestTabName_DTgiven";
static char  _DecomposeFctID[100]="DecomposeFunctionID";
static char  _ComposeFctID[100]="ComposeFunctionID";
static char  _InsertSubsystem[100]="InsertSubsystem";


//static char _systemNameList[20][10]={"VELO","DAQ","TFC","RICH1","RICH2","IT","TT","OT","ECAL","HCAL",
#define IN_USE 1
#define SPARE 2 //means hot spare
#define TEST 3 //means test use: but still in CERN
#define EXT_TEST 4 //means use externally
#define IN_REPAIR 5
#define DESTROYED 6
#define NOT_AVAILABLE 7
#define TIMEOUT_DEFINED 300

extern char  _list_of_subsystemname[50][10];
extern int _list_of_subsystemID[50];
extern int _nb_of_subsystem;
