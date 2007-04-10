

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

static char DEVICEBOOTING_TABLE[31]="lhcb_device_booting";
static char DEVICETYPEBOOTING_TABLE[31]="lhcb_devicetype_booting";
static char PATHDETAILS_TABLE[31]="lhcb_pdetails";
static char DESTINATIONTABLE[31]="lhcb_destinationtable";
static char ROUTINGTABLE[31]="lhcb_routingtable";

static char SUBSYSTEM_TABLE[31]="lhcb_subsystemList";
static char PATH_TEMP[31]="lhcb_path_temp";
static char MICROPATH_TEMP[31]="lhcb_micropath_temp";
static char SQLROWCOUNT[20]="SQL%ROWCOUNT";

static char _date_format[30]="YY/MM/DD/HH24/MI/SS";


static char _UpdateBoardCpnt[80]="lhcb_configuration.UpdateBoardCpnt";
static char _TestUseBoardCpnt[80]="lhcb_configuration.TestUseBoardCpnt";
static char _InsertTestBoard[100]="lhcb_configuration.InsertTestBoard";
static char   _createschema[51]="lhcb_configuration.createtableschema";
static char   _dropschema[51]="lhcb_configuration.droptableschema";
static char   _createRT[71]="lhcb_configuration.routingtable_pck.createRT_all_devname";
static char   _createTD[71]="lhcb_configuration.routingtable_pck.createTD_all_devname";

static char  _CreateMicroLgLinks[80]="lhcb_configuration.routingtable_pck.CreateMicroNodeLink_tab";
static char  _CreateMicroPaths[80]="lhcb_configuration.routingtable_pck.CreateMicroPathTab";
static char  _CreateMicroPaths_Host[80]="lhcb_configuration.routingtable_pck.CreateMicroPathTab_Host";
static char  _InsertMicroPathUsingID[80]="lhcb_configuration.routingtable_pck.InsertIntoMicroPathUsingID";
static char  _InsertMicroPathFromPortID[80]="lhcb_configuration.routingtable_pck.InsertIntoMicroPathFromPortID";
static char  _InsertMicroPathCpntType[80]="lhcb_configuration.routingtable_pck.InsertIntoMicroPathCnptType";
static char  _UpdatePathUsed[80]="lhcb_configuration.routingtable_pck.updatepathused";
static char  _UpdateDynamicTables[80]="lhcb_configuration.routingtable_pck.CheckPathTabCompleteAndUpdate";
static char  _SwapTwoDevices[80]="lhcb_configuration.SwapTwoDevices";
static char _DeleteLinksInPaths[80]="lhcb_configuration.routingtable_pck.deleteLinksInPaths";
static char _updateIPaddress[80]="lhcb_configuration.updateIPaddress";
static char _insertIpaddress[80]="lhcb_configuration.insertIPaddress";
static char _GiveDestinTabName[51]="lhcb_configuration_dev.GiveDestinationTableName";
static char  _GiveDestinType[51]="lhcb_configuration_dev.GiveDestTabName_DTgiven";




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
