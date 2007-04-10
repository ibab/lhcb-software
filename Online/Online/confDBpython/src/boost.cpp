#include "../confDBpython/CONFDB.h"
#include "../confDBpython/CONFDBEXCEPTION.h"
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/exception_translator.hpp>

using namespace boost::python;
boost::python::list vec_list() {
  boost::python::list l;
  for (int i=0; i < 10; ++i)
    l.append(i);
  return l;
}


void translatorExc(CONFDBEXCEPTION x) {
    PyErr_SetString(PyExc_RuntimeError, x.what());
}

#if defined( _WIN32) ||defined(WIN32)
BOOST_PYTHON_MODULE(confDBpython)
#else
BOOST_PYTHON_MODULE(libconfDBpython)
#endif
{
    // Compile check only...
    class_<std::vector<int> >("IntVec")
        .def(vector_indexing_suite<std::vector<int> >());

    class_<std::vector<std::string> >("StringVec")
   .def(vector_indexing_suite<std::vector<std::string>,true >());
	
    //class_<CONFDBEXCEPTION>("CONFDBEXCEPTION")
		//.def(init<std::string>())
		//.def("getconfDBErrMess",&CONFDBEXCEPTION::getconfDBErrMess)
		//.def("what",&CONFDBEXCEPTION::what)
		//;
register_exception_translator<CONFDBEXCEPTION>(&translatorExc);

    class_<CONFDB>("CONFDB",init<std::string,std::string,std::string>())
		.def("DBConnexion",&CONFDB::PyDBConnexion)
		.def("DBDeconnexion",&CONFDB::PyDBDeconnexion)
		.def("GetDeviceTypeRow",&CONFDB::PyGetDeviceTypeRow)
		.def("GetDeviceRow_devicename",&CONFDB::PyGetDeviceRow_devicename)
		.def("GetDeviceRow_deviceid",&CONFDB::PyGetDeviceRow_deviceid)
		.def("GetPortRow_pid",&CONFDB::PyGetPortRow_pid)
		.def("GetPortRow_devid",&CONFDB::PyGetPortRow_devid)
		.def("GetMacroConnectivityRow_lkid",&CONFDB::PyGetMacroConnectivityRow_lkid)
		.def("GetMacroConnectivityRow_node",&CONFDB::PyGetMacroConnectivityRow_node)
		.def("GetIPInfoRow",&CONFDB::PyGetIPInfoRow)
		.def("GetLkTypeRow_lkname",&CONFDB::PyGetLkTypeRow_lkname)
		.def("GetLkTypeRow_lknb",&CONFDB::PyGetLkTypeRow_lknb)
		.def("GetPortIDPerDevID",&CONFDB::PyGetPortIDPerDevID)
		.def("GetConnOutBetweenDeviceDeviceType",&CONFDB::PyGetConnOutBetweenDeviceDeviceType)
		.def("CreateTableSchema",&CONFDB::PyCreateTableSchema)
		.def("GetMacIPAddPerDevID",&CONFDB::PyGetMacIPAddPerDevID)
		.def("GetLkToDevID",&CONFDB::PyGetLkToDevID)
		.def("GetLkFromDevID",&CONFDB::PyGetLkFromDevID)
		.def("GetDeviceIDsPerType",&CONFDB::PyGetDeviceIDsPerType)
		.def("GetDeviceNamesPerType",&CONFDB::PyGetDeviceNamesPerType)
		.def("InsertMultipleMacroLinks",&CONFDB::PyInsertMultipleMacroLinks)
		.def("InsertMultiplePorts",&CONFDB::PyInsertMultiplePorts)
		.def("InsertMultipleSimpleLinkTypes",&CONFDB::PyInsertMultipleSimpleLinkTypes)
		.def("InsertMultipleCompositeLinkTypes",&CONFDB::PyInsertMultipleCompositeLinkTypes) 
		.def("InsertMultipleFunctionalDevices",&CONFDB::PyInsertMultipleFunctionalDevices)
		.def("InsertMultipleDeviceTypes",&CONFDB::PyInsertMultipleDeviceTypes)
		.def("InsertMacroLink",&CONFDB::PyInsertMacroLink)
		.def("InsertPort",&CONFDB::PyInsertPort)
		.def("InsertSimpleLinkType",&CONFDB::PyInsertSimpleLinkType)
		.def("InsertCompositeLinkType",&CONFDB::PyInsertCompositeLinkType)
		.def("InsertFunctionalDevice",&CONFDB::PyInsertFunctionalDevice)
		.def("InsertDeviceType",&CONFDB::PyInsertDeviceType)
		.def("GetDetailedConnectivityBetweenDevices",&CONFDB::PyGetDetailedConnectivityBetweenDevices)
		.def("GetConnectivityBetweenDevices",&CONFDB::PyGetConnectivityBetweenDevices)
		.def("GetDetailedConnBetweenDeviceDevType",&CONFDB::PyGetDetailedConnBetweenDeviceDevType)
		.def("GetDestinationNamePerDevPort", &CONFDB::PyGetDestinationNamePerDevPort)
		.def("GetDeviceID_devicename",&CONFDB::PyGetDeviceID_devicename)
		.def("GetDeviceName_deviceid",&CONFDB::PyGetDeviceName_deviceid)
		.def("DropTableSchema", &CONFDB::PyDropTableSchema)
		.def("CreateRoutingTable", &CONFDB::PyCreateRoutingTable)
		.def("CreateDestinationTable", &CONFDB::PyCreateDestinationTable)
		.def("GetFreeDeviceNamesPerType",&CONFDB::PyGetFreeDeviceNamesPerType)
		.def("GetAllPathsPerDevice",  &CONFDB::PyGetAllPathsPerDevice)
		.def("LoadConnectivityTable",&CONFDB::PyLoadConnectivityTable)
		.def("MatchDeviceIDDeviceName",&CONFDB::PyMatchDeviceIDDeviceName)
		.def("LoadDeviceTypeTable",&CONFDB::PyLoadDeviceTypeTable)
		.def("LoadLinkTypeTable",&CONFDB::PyLoadLinkTypeTable)
		.def("UpdateMultipleLkTypeLinks",&CONFDB::PyUpdateMultipleLkTypeLinks)
		.def("UpdateMultipleLkUsedLinks",&CONFDB::PyUpdateMultipleLkUsedLinks)
		.def("UpdateMultipleBidirectionalLinks",&CONFDB::PyUpdateMultipleBidirectionalLinks)
		.def("UpdateMultipleDevNamesDevices",&CONFDB::PyUpdateMultipleDevNamesDevices)
		.def("UpdateMultipleAttributesDevices",&CONFDB::PyUpdateMultipleAttributesDevices)
		.def("UpdateMultipleDeviceTypes",&CONFDB::PyUpdateMultipleDeviceTypes)
		.def("UpdateMultipleDeviceTypeAttributes",&CONFDB::PyUpdateMultipleDeviceTypeAttributes)
		.def("UpdateMultipleLinkTypeNames",&CONFDB::PyUpdateMultipleLinkTypeNames)
		.def("UpdateMultipleCompositeLinkTypes",&CONFDB::PyUpdateMultipleCompositeLinkTypes)
		.def("UpdateMultipleIPAddresses",&CONFDB::PyUpdateMultipleIPAddresses)
		.def("UpdateMultipleAttributeMacIPs",&CONFDB::PyUpdateMultipleAttributeMacIPs)
		.def("UpdateMultiplePorts",&CONFDB::PyUpdateMultiplePorts)
		.def("UpdateMultipleDeviceSystemList",&CONFDB::PyUpdateMultipleDeviceSystemList)
		.def("GetCpntID_cpntname",&CONFDB::PyGetCpntID_cpntname)
		.def("GetCpntName_cpntid",&CONFDB::PyGetCpntName_cpntid)
		.def("InsertMultipleMicroLinks",&CONFDB::PyInsertMultipleMicroLinks)
		.def("InsertSpareDevice",&CONFDB::PyInsertSpareDevice)
		.def("InsertSparePort",&CONFDB::PyInsertSparePort)
		.def("InsertMultipleSparePorts",&CONFDB::PyInsertMultipleSparePorts)
		.def("InsertMultipleBoardCpnts",&CONFDB::PyInsertMultipleBoardCpnts)
		.def("InsertMultipleSpareDevices",&CONFDB::PyInsertMultipleSpareDevices)
		.def("InsertTestFunctionalDevice",&CONFDB::PyInsertTestFunctionalDevice)
		.def("GetMicroLkToCpntID",&CONFDB::PyGetMicroLkToCpntID)
		.def("GetMicroLkFromCpntID",&CONFDB::PyGetMicroLkFromCpntID)
		.def("GetSpareHWCpntPerType",&CONFDB::PyGetSpareHWCpntPerType)
		.def("GetCpntNamesPerBoard",&CONFDB::PyGetCpntNamesPerBoard)
		.def("GetSpareHWPerType",&CONFDB::PyGetSpareHWPerType)
		.def("GetSpareHWCpntPerLocation",&CONFDB::PyGetSpareHWCpntPerLocation)
		.def("GetSpareHWPerLocation",&CONFDB::PyGetSpareHWPerLocation)
		.def("GetBoardCpntPerType",&CONFDB::PyGetBoardCpntPerType)
		.def("GetMicroConnectivityRow_node",&CONFDB::PyGetMicroConnectivityRow_node)
		.def("GetMicroConnectivityRow_lkid",&CONFDB::PyGetMicroConnectivityRow_lkid)
		.def("GetHWCpntRow_snbid",&CONFDB::PyGetHWCpntRow_snbid)
		.def("GetBoardCpntRow_cpntid",&CONFDB::PyGetBoardCpntRow_cpntid)
		.def("GetBoardCpntPerType",&CONFDB::PyGetBoardCpntPerType)
		.def("GetBoardCpntRow_cpntname",&CONFDB::PyGetBoardCpntRow_cpntname)
		.def("GetHWCpntRow_serialnb",&CONFDB::PyGetHWCpntRow_serialnb)
		.def("GetDeviceNamesPerLocation",&CONFDB::PyGetDeviceNamesPerLocation)
		.def("GetMicroConnectBetweenBoardCpntAndMotherBoard",&CONFDB::PyGetMicroConnectBetweenBoardCpntAndMotherBoard)
		.def("GetMicroConnectFromPortid",&CONFDB::PyGetMicroConnectFromPortid)
		.def("GetMicroConnectToCpntType",&CONFDB::PyGetMicroConnectToCpntType)
		.def("DeleteLinkRow",&CONFDB::PyDeleteLinkRow)
		.def("DeletePortRow",&CONFDB::PyDeletePortRow)
		.def("GetLkTypeDecomposition_lknb",&CONFDB::PyGetLkTypeDecomposition_lknb)  
		.def("UpdateMultipleDTypeDevices",&CONFDB::PyUpdateMultipleDTypeDevices)
		.def("DeleteBootImage",&CONFDB::PyDeleteBootImage)
		.def("UpdateBootImage",&CONFDB::PyUpdateBootImage)
		.def("InsertMultipleBootImages",&CONFDB::PyInsertMultipleBootImages)
		.def("GetBootImageRow_devicename",&CONFDB::PyGetBootImageRow_devicename)
		.def("GetPortID_portinfo",&CONFDB::PyGetPortID_portinfo)
		.def("GetLkIDsPerLkInfo",&CONFDB::PyGetLkIDsPerLkInfo)
		.def("GetIPAliasRow",&CONFDB::PyGetIPAliasRow)
		.def("GetIPAliasesPerIPName",&CONFDB::PyGetIPAliasesPerIPName)
		.def("UpdateIPalias",&CONFDB::PyUpdateIPalias)
		.def("UpdateHWSerialNB",&CONFDB::PyUpdateHWSerialNB)
		.def("UpdateDeviceFunctionName",&CONFDB::PyUpdateDeviceFunctionName)
		.def("GetDeviceNamesPerFunction",&CONFDB::PyGetDeviceNamesPerFunction)
		.def("InsertMultipleIPAliases",&CONFDB::PyInsertMultipleIPAliases)
		.def("InsertSimpleDeviceFunction",&CONFDB::PyInsertSimpleDeviceFunction)
		.def("DeleteIPAlias",&CONFDB::PyDeleteIPAlias)
	    .def("UpdateMultipleLkInfoLinks",&CONFDB::PyUpdateMultipleLkInfoLinks)
		.def("UpdateMultipleDeviceTypeSystemList",&CONFDB::PyUpdateMultipleDeviceTypeSystemList)
		.def("GetAvailableFunctions",&CONFDB::PyGetAvailableFunctions)
		.def("DeleteFunctionalDevice",&CONFDB::PyDeleteFunctionalDevice)
		.def("DeleteHWDevice",&CONFDB::PyDeleteHWDevice)
		.def("DeleteFunctionalDeviceType",&CONFDB::PyDeleteFunctionalDeviceType)
		.def("DeleteSystemName",&CONFDB::PyDeleteSystemName)
		.def("GetListOfSubsystems",&CONFDB::PyGetListOfSubsystems)
		.def("InsertSubsystem",&CONFDB::PyInsertSubsystem)
	    .def("UpdateSubsystemName",&CONFDB::PyUpdateSubsystemName)
		.def("SwapTwoDevices",&CONFDB::PySwapTwoDevices)
		.def("GetFunctionalDeviceStatus",&CONFDB::PyGetFunctionalDeviceStatus)
	    .def("GetFunctionalBoardCpntStatus",&CONFDB::PyGetFunctionalBoardCpntStatus)
		.def("GetHWLastFunctionalDevice",&CONFDB::PyGetHWLastFunctionalDevice)
		.def("GetHWLastFunctionalBoardCpntName",&CONFDB::PyGetHWLastFunctionalBoardCpntName)
		.def("GetFunctionalDeviceLastHW",&CONFDB::PyGetFunctionalDeviceLastHW)
		.def("GetFunctionalBoardCpntNameLastHW",&CONFDB::PyGetFunctionalBoardCpntNameLastHW)
		.def("GetHWDeviceStatus",&CONFDB::PyGetHWDeviceStatus)
		.def("GetHWBoardCpntStatus",&CONFDB::PyGetHWBoardCpntStatus)
		.def("GetHWDeviceByStatus",&CONFDB::PyGetHWDeviceByStatus)
		.def("GetHWBoardCpntByStatus",&CONFDB::PyGetHWBoardCpntByStatus)
	    .def("GetHistoryOfHWDevice",&CONFDB::PyGetHistoryOfHWDevice)
		.def("GetHistoryOfHWBoardCpnt",&CONFDB::PyGetHistoryOfHWBoardCpnt)
		.def("GetHistoryOfFunctionalDevice",&CONFDB::PyGetHistoryOfFunctionalDevice)
		.def("GetHistoryOfFunctionalBoardCpntName",&CONFDB::PyGetHistoryOfFunctionalBoardCpntName)
		.def("GetFunctionalDeviceByStatus",&CONFDB::PyGetFunctionalDeviceByStatus)
	    .def("GetSpareHWTypeList",&CONFDB::PyGetSpareHWTypeList)
		.def("GetSparePortRow_snb",&CONFDB::PyGetSparePortRow_snb)
		.def("GetPortInfoPerSpare",&CONFDB::PyGetPortInfoPerSpare)  

		.def("GetHWDeviceRow_serialnb",&CONFDB::PyGetHWDeviceRow_serialnb);






}
