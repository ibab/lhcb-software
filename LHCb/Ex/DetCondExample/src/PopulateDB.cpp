//$Id: PopulateDB.cpp,v 1.2 2001-10-29 12:43:17 andreav Exp $
#include <stdio.h>

#include "PopulateDB.h"
#include "Temperature.h"

#include "DetCond/IConditionsDBCnvSvc.h"
#include "DetCond/IConditionsDBGate.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/TimePoint.h"

#include "ConditionsDB/CondDBObjFactory.h"
#include "ConditionsDB/ICondDBMgr.h"
#include "ConditionsDB/ICondDBTagMgr.h"
#include "ConditionsDB/ICondDBFolderMgr.h"
#include "ConditionsDB/ICondDBDataAccess.h"

/// Instantiation of a static factory to create instances of this algorithm
static const AlgFactory<PopulateDB> Factory;
const IAlgFactory& PopulateDBFactory = Factory;

//----------------------------------------------------------------------------

/// Constructor
PopulateDB::PopulateDB( const std::string&  name, 
				  ISvcLocator*        pSvcLocator )
  : Algorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------------

/// Initialization of the algorithm. 
/// It creates the environment needed for processing the events. 
/// For this class this is the main method, where we store data in the CondDB.

StatusCode PopulateDB::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize()" << endreq;

  // Locate the ConditionsDBCnvSvc
  // This is used to encode folder data to be written in the CondDB
  StatusCode sc = serviceLocator()->service 
    ( "ConditionsDBCnvSvc", m_conditionsDBCnvSvc );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't locate ConditionsDBCnvSvc" << endreq;
    return sc;
  } else {
    log << MSG::INFO << "Succesfully located ConditionDBCnvSvc" << endreq;
  }

  // Get a handle to the ConditionsDBGate
  m_conditionsDBGate = m_conditionsDBCnvSvc->conditionsDBGate();
  log << MSG::INFO << "Retrieved a handle to the ConditionDBGate" << endreq;

  // Store sample data if the database is empty
  log << MSG::INFO << "Store sample data in the database if empty" << endreq;
  sc = i_condDBStoreSampleData( );
  if ( !sc.isSuccess() ) return sc;

  // Dump the database to make sure it is not empty
  log << MSG::INFO << "Dump sample data from the database" << endreq;
  sc = i_condDBDumpSampleData( );
  if ( !sc.isSuccess() ) return sc;

  log << MSG::INFO << "Initialization completed" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

StatusCode PopulateDB::execute( ) {
  MsgStream log(msgSvc(), name());
  //log << MSG::INFO << "Execute()" << endreq;
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode PopulateDB::finalize( ) {

  MsgStream log(msgSvc(), name());
  //log << MSG::INFO << "Finalize()" << endreq;
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode PopulateDB::i_condDBStoreSampleData ( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "============= condDBStoreSampleData() starting ====================="
      << endreq;

  ICondDBMgr*        condDBMgr        = m_conditionsDBGate->condDBManager();
  ICondDBDataAccess* condDBDataAccess = condDBMgr->getCondDBDataAccess();
  ICondDBFolderMgr*  condDBFolderMgr  = condDBMgr->getCondDBFolderMgr();
  ICondDBTagMgr*     condDBTagMgr     = condDBMgr->getCondDBTagMgr();

  // Check if root folderSet exists
  log << MSG::INFO 
      << "Checking if CondDB root folderSet `/' already exists" << endreq;
  log << MSG::INFO 
      << "" << endreq;
  bool rootFolderSetExists;
  try {
    condDBMgr->startRead();
    rootFolderSetExists = condDBFolderMgr->exist( "/" );
    condDBMgr->commit();
  } catch (CondDBException &e) {
    log << MSG::ERROR
	<< "Could not check existence of root folderSet" << endreq;
    log << MSG::ERROR
	<< "*** ConditionsDB exception caught:"          << endreq;
    log << MSG::ERROR
	<< "***  error message: " << e.getMessage()      << endreq;
    log << MSG::ERROR
	<< "***  error code:    " << e.getErrorCode()    << endreq;
    return StatusCode::FAILURE;
  } 

  // If root folderSet exists already then return
  // If root folderSet does not exist then create it and store sample data
  if ( rootFolderSetExists ) {
    log << MSG::INFO 
	<< "Root folderSet already exists: no need to store sample data" 
	<< endreq;
    log << MSG::INFO 
	<< "============= condDBStoreSampleData() ending "
	<< "=======================" 
	<< endreq;
    return StatusCode::SUCCESS;
  } else {      
    log << MSG::INFO 
	<< "Root folderSet does not exist: create it"
	<< endreq;
    try {
      condDBMgr->startUpdate();
      condDBFolderMgr->createCondDBFolderSet
	( "/",
	  "",
	  "Root folderSet for the ConditionsDB" ); 
      condDBMgr->commit();    
    } catch (CondDBException &e) {
      log << MSG::ERROR
	  << "Error in creating root folderSet in the CondDB" << endreq;
      log << MSG::ERROR
	  << "*** ConditionsDB exception caught:"             << endreq;
      log << MSG::ERROR
	  << "***  error message: " << e.getMessage()         << endreq;
      log << MSG::ERROR
	  << "***  error code:    " << e.getErrorCode()       << endreq;
      return StatusCode::FAILURE;
    }
    log << MSG::INFO 
	<< "Root folderSet did not exist and was succesfully created" 
	<< endreq;
  }

  // Store sample data
  log << MSG::INFO << "Now store sample data into the CondDB" << endreq;

  try {
    
    // Encode classID and storage type into folder description
    CLID classID;
    unsigned char type;
    std::string description;
    StatusCode status;

    // Create new CondDBFolderSets and CondDBFolders
    condDBMgr->startUpdate();

    condDBFolderMgr->createCondDBFolderSet
      ( "/Conditions",
	"",
	"Generic Condition folderSet" ); 

    condDBFolderMgr->createCondDBFolderSet
      ( "/Conditions/LHCb", 
	"",
	"Condition folderSet for the LHCb detector" ); 

    condDBFolderMgr->createCondDBFolderSet
      ( "/Conditions/LHCb/Slow",
	"",
	"Slow control Condition folderSet for the LHCb detector" ); 

    condDBFolderMgr->createCondDBFolderSet
      ( "/Conditions/LHCb/Ecal",
	"",
	"Condition folderSet for the Ecal detector" ); 

    condDBFolderMgr->createCondDBFolderSet
      ( "/Conditions/LHCb/Ecal/Slow",
	"",
	"Slow control Condition folderSet for the Ecal detector" ); 

    classID = Temperature::classID();
    type    = XMLSTRING_StorageType; 
    log << MSG::DEBUG 
	<< "Encode description for type=" << (unsigned int)type 
	<< " classID=" << (unsigned int)classID << endreq;
    status  = m_conditionsDBCnvSvc->encodeDescription( classID,
						       type,
						       description );
    if (!status.isSuccess() ) {
      log << MSG::ERROR << "Could not encode folder description" << endreq;
      return status;
    }

    // Create two folders with the same objects for simplicity
    condDBFolderMgr->createCondDBFolder
      ( "/Conditions/LHCb/Slow/temp",
	"",
	description );
    condDBFolderMgr->createCondDBFolder
      ( "/Conditions/LHCb/Ecal/Slow/temp",
	"",
	description );

    // It is not necessary to commit transactions one by one (as of v.0.1.9.0)
    condDBMgr->commit();    

    // Create new COLD CondDBObjects
    int i;
    condDBMgr->startUpdate();
    for (i=10; i<50; i=i+10) {
      char* c = new char[3];
      sprintf(c,"%3.3i",i+5);
      std::string s(c);
      delete[] c;
      s = "<temperature value=" + s + ">" ;
      ICondDBObject* condObject = CondDBObjFactory::createCondDBObject
	(i, i+10, s, "LHCb temperature in the given time interval");
      condDBDataAccess->storeCondDBObject
	( "/Conditions/LHCb/Slow/temp", condObject );
      condDBDataAccess->storeCondDBObject
	( "/Conditions/LHCb/Ecal/Slow/temp", condObject );
      CondDBObjFactory::destroyCondDBObject(condObject);
    }
    log << MSG::DEBUG << "Create and apply tag COLD" << endreq;
    condDBTagMgr->createCondDBTag("COLD");
    condDBTagMgr->tag("/Conditions/LHCb/Slow/temp","COLD");
    condDBTagMgr->tag("/Conditions/LHCb/Ecal/Slow/temp","COLD");
    condDBMgr->commit();    

    // Create new HOT CondDBObjects
    condDBMgr->startUpdate();
    for (i=10; i<50; i=i+10) {
      char* c = new char[3];
      sprintf(c,"%3.3i",i+105);
      std::string s(c);
      delete[] c;
      s = "<temperature value=" + s + ">" ;
      ICondDBObject* condObject = CondDBObjFactory::createCondDBObject
	(i, i+10, s, "LHCb temperature in the given time interval");
      condDBDataAccess->storeCondDBObject
	( "/Conditions/LHCb/Slow/temp", condObject );
      condDBDataAccess->storeCondDBObject
	( "/Conditions/LHCb/Ecal/Slow/temp", condObject );
      CondDBObjFactory::destroyCondDBObject(condObject);
    }
    log << MSG::DEBUG << "Create and apply tag HOT" << endreq;
    condDBTagMgr->createCondDBTag("HOT");
    condDBTagMgr->tag("/Conditions/LHCb/Slow/temp","HOT");
    condDBTagMgr->tag("/Conditions/LHCb/Ecal/Slow/temp","HOT");
    condDBMgr->commit();    

    // Now define the production tag: Ecal is hot, LHCb is cold
    condDBMgr->startUpdate();
    log << MSG::DEBUG << "Create and apply tag PRODUCTION" << endreq;
    condDBTagMgr->createCondDBTag("PRODUCTION");
    condDBTagMgr->tag("/Conditions/LHCb/Slow/temp","PRODUCTION","COLD");
    condDBTagMgr->tag("/Conditions/LHCb/Ecal/Slow/temp","PRODUCTION","HOT");
    condDBMgr->commit();    

  } catch (CondDBException &e) {

    log << MSG::ERROR
	<< "Error in storing sample data into the CondDB" << endreq;
    log << MSG::ERROR
    	<< "*** ConditionsDB exception caught: "       << endreq;
    log << MSG::ERROR
    	<< "***   error message: " << e.getMessage()   << endreq;
    log << MSG::ERROR
	<< "***   error code:    " << e.getErrorCode() << endreq;
    return StatusCode::FAILURE;

  }
  log << MSG::INFO 
      << "============= condDBStoreSampleData() ending ======================="
      << endreq;
  return StatusCode::SUCCESS; 

}

//----------------------------------------------------------------------------

StatusCode PopulateDB::i_condDBDumpSampleData ( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "============= condDBDumpSampleData() starting ======================"
      << endreq;

  ICondDBMgr*        condDBMgr        = m_conditionsDBGate->condDBManager();
  ICondDBDataAccess* condDBDataAccess = condDBMgr->getCondDBDataAccess();
  ICondDBFolderMgr*  condDBFolderMgr  = condDBMgr->getCondDBFolderMgr();

  // List all stored CondDBFolders 
  // Use options: "l" = long, "R" = Recursive 
  try {
    condDBMgr->startRead();
    log << MSG::INFO << "Describe contents of CondDB" << endreq;
    condDBFolderMgr->describe("/","lR");
    condDBMgr->commit();
  } catch (CondDBException &e) {
    log << MSG::ERROR << "Could not list CondDBFolders from CondDB" << endreq;
    log << MSG::ERROR << "*** ConditionsDB exception caught: "      << endreq;
    log << MSG::ERROR << "***  error message: " << e.getMessage()   << endreq;
    log << MSG::ERROR << "***  error code:    " << e.getErrorCode() << endreq;
    return StatusCode::FAILURE;
  }

  // Iteratively print all known CondDBFolderSets and CondDBFolders
  try {
    condDBMgr->startRead();
    log << MSG::INFO << "Iteratively list contents of CondDB" << endreq;
    std::vector<string> allCondDBFolderSet;
    std::vector<string> allCondDBFolder;
    condDBFolderMgr->getAllCondDBFolderSet ( allCondDBFolderSet );
    condDBFolderMgr->getAllCondDBFolder    ( allCondDBFolder    );
    std::vector<string>::const_iterator aName;
    log << MSG::INFO << "CondDB contains " << allCondDBFolderSet.size() 
	<< " CondDBFolderSet(s):";
    for ( aName  = allCondDBFolderSet.begin(); 
	  aName != allCondDBFolderSet.end(); 
	  aName++ ) {
      log << MSG::INFO << endl 
	  << "                                      " << *aName;
    }
    log << MSG::INFO << endreq;
    log << MSG::INFO << "CondDB contains " << allCondDBFolder.size() 
	<< " CondDBFolder(s):";
    for ( aName  = allCondDBFolder.begin(); 
	  aName != allCondDBFolder.end(); 
	  aName++ ) {
      log << MSG::INFO << endl 
	  << "                                      " << *aName;
    }
    log << MSG::INFO << endreq;
    condDBMgr->commit();
  } catch (CondDBException &e) {
    log << MSG::ERROR << "Could not retrieve CondDBFolder/Set lists" << endreq;
    log << MSG::ERROR << "*** ConditionsDB exception caught: "       << endreq;
    log << MSG::ERROR << "***  error message: " << e.getMessage()    << endreq;
    log << MSG::ERROR << "***  error code:    " << e.getErrorCode()  << endreq;
    return StatusCode::FAILURE;
  }

  // Dump the content of CondDBFolder "/Conditions/LHCb/Slow/temp"
  try {
    condDBMgr->startRead();
    log << MSG::INFO 
	<< "Dumping the content of /Conditions/LHCb/Slow/temp" << endreq;
    condDBDataAccess->dump("/Conditions/LHCb/Slow/temp");
    condDBMgr->commit();
  } catch (CondDBException &e) {
    log << MSG::ERROR << "Could not dump CondDBFolder contents"     << endreq;
    log << MSG::ERROR << "*** ConditionsDB exception caught: "      << endreq;
    log << MSG::ERROR << "***  error message: " << e.getMessage()   << endreq;
    log << MSG::ERROR << "***  error code:    " << e.getErrorCode() << endreq;
    return StatusCode::FAILURE;
  }

  // Now retrieve inside the folder the CondDBObject valid at a given time
  // Uses internally the findCondDBObject method (slower, but need to test it)
  TimePoint when;
  std::string folderName = "/Conditions/LHCb/Slow/temp";
  std::string theData;
  TimePoint since;
  TimePoint till;
  int i;
  for ( i=15; i<50; i=i+10) {
    when = i;
    log << MSG::INFO 
	<< "Looking for the COLD temperature valid at time " << i << endreq;
    StatusCode status = m_conditionsDBGate->readCondDBObject
      ( since, till, theData, folderName, "COLD", when );
    if (! status.isSuccess() ) return status;
    log << MSG::INFO 
	<< "Temperature since " << since.absoluteTime() 
	<< " till " << till.absoluteTime() << " is:" << endreq;
    log << MSG::INFO 
	<< theData << endreq;
    log << MSG::INFO 
	<< "Looking for the HOT temperature valid at time " << i << endreq;
    status = m_conditionsDBGate->readCondDBObject
      ( since, till, theData, folderName, "HOT", when );
    if (! status.isSuccess() ) return status;
    log << MSG::INFO 
	<< "Temperature since " << since.absoluteTime() 
	<< " till " << till.absoluteTime() << " is:" << endreq;
    log << MSG::INFO 
	<< theData << endreq;
    log << MSG::INFO 
	<< "Looking for the PRODUCTION temperature valid at time " 
	<< i << endreq;
    status = m_conditionsDBGate->readCondDBObject
      ( since, till, theData, folderName, "PRODUCTION", when );
    if (! status.isSuccess() ) return status;
    log << MSG::INFO 
	<< "Temperature since " << since.absoluteTime() 
	<< " till " << till.absoluteTime() << " is:" << endreq;
    log << MSG::INFO 
	<< theData << endreq;
  }
  
  // Dump the content of CondDBFolder "/Conditions/LHCb/Ecal/Slow/temp"
  try {
    condDBMgr->startRead();
    log << MSG::INFO 
	<< "Dumping the content of /Conditions/LHCb/Ecal/Slow/temp" << endreq;
    condDBDataAccess->dump("/Conditions/LHCb/Ecal/Slow/temp");
    condDBMgr->commit();
  } catch (CondDBException &e) {
    log << MSG::ERROR << "Could not dump CondDBFolder contents"     << endreq;
    log << MSG::ERROR << "*** ConditionsDB exception caught: "      << endreq;
    log << MSG::ERROR << "***  error message: " << e.getMessage()   << endreq;
    log << MSG::ERROR << "***  error code:    " << e.getErrorCode() << endreq;
    return StatusCode::FAILURE;
  }

  // Now retrieve inside the folder the CondDBObject valid at a given time
  // Uses internally the findCondDBObject method (slower, but need to test it)
  folderName = "/Conditions/LHCb/Ecal/Slow/temp";
  for ( i=15; i<50; i=i+10) {
    when = i;
    log << MSG::INFO 
	<< "Looking for the COLD temperature valid at time " << i << endreq;
    StatusCode status = m_conditionsDBGate->readCondDBObject
      ( since, till, theData, folderName, "COLD", when );
    if (! status.isSuccess() ) return status;
    log << MSG::INFO 
	<< "Temperature since " << since.absoluteTime() 
	<< " till " << till.absoluteTime() << " is:" << endreq;
    log << MSG::INFO 
	<< theData << endreq;
    log << MSG::INFO 
	<< "Looking for the HOT temperature valid at time " << i << endreq;
    status = m_conditionsDBGate->readCondDBObject
      ( since, till, theData, folderName, "HOT", when );
    if (! status.isSuccess() ) return status;
    log << MSG::INFO 
	<< "Temperature since " << since.absoluteTime() 
	<< " till " << till.absoluteTime() << " is:" << endreq;
    log << MSG::INFO 
	<< theData << endreq;
    log << MSG::INFO 
	<< "Looking for the PRODUCTION temperature valid at time " 
	<< i << endreq;
    status = m_conditionsDBGate->readCondDBObject
      ( since, till, theData, folderName, "PRODUCTION", when );
    if (! status.isSuccess() ) return status;
    log << MSG::INFO 
	<< "Temperature since " << since.absoluteTime() 
	<< " till " << till.absoluteTime() << " is:" << endreq;
    log << MSG::INFO 
	<< theData << endreq;
  }
  
  log << MSG::INFO 
      << "============= condDBDumpSampleData() ending ========================"
      << endreq;
  return StatusCode::SUCCESS;  

}

//----------------------------------------------------------------------------

