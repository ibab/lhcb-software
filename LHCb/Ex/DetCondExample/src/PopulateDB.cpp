//$Id: PopulateDB.cpp,v 1.5 2002-03-01 16:12:36 andreav Exp $
#include <stdio.h>
#include <fstream>

#include "PopulateDB.h"

#include "DetDesc/Condition.h"

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
#include "ConditionsDB/ICondDBDataIterator.h"

/// Instantiation of a static factory to create instances of this algorithm
static const AlgFactory<PopulateDB> Factory;
const IAlgFactory& PopulateDBFactory = Factory;

// Define the name of the root folderset
// This used to be "/" in Objectivity.
// In the Oracle implementation it is required to have a name "/XXX"
const std::string rootName = "/CONDDB";  

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
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode PopulateDB::finalize( ) {
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
      << "Checking if CondDB root folderSet `"
      << rootName << "' already exists" << endreq;
  log << MSG::INFO 
      << "" << endreq;
  bool rootFolderSetExists;
  try {
    condDBMgr->startRead();
    rootFolderSetExists = condDBFolderMgr->exist( rootName );
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

  // Define the attributes for all folders created in this example
  std::string folderAttributes = "";

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
	( rootName,
	  folderAttributes,
	  "Root folderSet for the ConditionsDB",
	  true); 
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
    
    // Encode the string storage type into the folder description
    unsigned char type;
    std::string description;
    StatusCode status;

    // Create new CondDBFolderSets
    log << MSG::DEBUG << "Create folder sets" << endreq;

    condDBMgr->startUpdate();

    condDBFolderMgr->createCondDBFolderSet
      ( rootName+"/SlowControl",
	folderAttributes,
	"Main SlowControl folderSet",
	true ); 

    condDBFolderMgr->createCondDBFolderSet
      ( rootName+"/SlowControl/LHCb", 
	folderAttributes,
	"SlowControl folderSet for the LHCb detector",
	true ); 

    condDBFolderMgr->createCondDBFolderSet
      ( rootName+"/SlowControl/Hcal", 
	folderAttributes,
	"SlowControl folderSet for the Hcal detector",
	true ); 

    condDBFolderMgr->createCondDBFolderSet
      ( rootName+"/Geometry",
	folderAttributes,
	"Main Geometry folderSet",
	true ); 

    // Encode description for CondDBFolders with XML data
    log << MSG::DEBUG 
	<< "Create folders "+rootName+"/SlowControl/LHCb/scLHCb"
	<< " and "+rootName+"/SlowControl/Hcal/scHcal" << endreq;
    type = XML_StorageType; 
    log << MSG::DEBUG 
	<< "Encode description for type=" << (unsigned int)type << endreq;
    status = m_conditionsDBCnvSvc->encodeDescription( type, description );
    if (!status.isSuccess() ) {
      log << MSG::ERROR << "Could not encode folder description" << endreq;
      return status;
    }

    // Create CondDBFolders with temperature data
    // For simplicity, create two folders with the same objects
    condDBFolderMgr->createCondDBFolder
      ( rootName+"/SlowControl/LHCb/scLHCb", 
	folderAttributes,
	description,
	true );
    condDBFolderMgr->createCondDBFolder
      ( rootName+"/SlowControl/Hcal/scHcal", 
	folderAttributes,
	description,
	true );

    // Encode description for CondDBFolders with XML data
    log << MSG::DEBUG << "Create folder "+rootName+"/Geometry/LHCb" << endreq;
    type = XML_StorageType; 
    log << MSG::DEBUG 
	<< "Encode description for type=" << (unsigned int)type << endreq;
    status = m_conditionsDBCnvSvc->encodeDescription( type, description );
    if (!status.isSuccess() ) {
      log << MSG::ERROR << "Could not encode folder description" << endreq;
      return status;
    }

    // Create CondDBFolders with XML catalogs
    condDBFolderMgr->createCondDBFolder
      ( rootName+"/Geometry/LHCb",
	folderAttributes,
	description,
	true );

    // It is not necessary to commit transactions one by one (as of v.0.1.9.0)
    condDBMgr->commit();    

    // Create new COLD temperature CondDBObjects
    log << MSG::DEBUG 
	<< "Create COLD objects in "+rootName+"/SlowControl/LHCb/scLHCb"
	<< " and "+rootName+"/SlowControl/Hcal/scHcal" << endreq;
    int i;
    condDBMgr->startUpdate();
    for ( i=0; i<3; i++ ) {
      std::string s;
      ICondDBObject* condObject;
      // LHCb
      i_encodeXmlTemperature( (double)i*10+5, "scLHCb", s);
      condObject = CondDBObjFactory::createCondDBObject
	( i*16, (i+1)*16, s, "LHCb temperature in the given time interval");
      condDBDataAccess->storeCondDBObject
	( rootName+"/SlowControl/LHCb/scLHCb", condObject );
      CondDBObjFactory::destroyCondDBObject(condObject);
      // Hcal
      i_encodeXmlTemperature( (double)i*10+5, "scHcal", s);
      condObject = CondDBObjFactory::createCondDBObject
	( i*16, (i+1)*16, s, "Hcal temperature in the given time interval");
      condDBDataAccess->storeCondDBObject
	( rootName+"/SlowControl/Hcal/scHcal", condObject );
      CondDBObjFactory::destroyCondDBObject(condObject);
    }
    log << MSG::DEBUG << "Create and apply tag COLD" << endreq;
    condDBTagMgr->createCondDBTag("COLD");
    condDBTagMgr->tag(rootName+"/SlowControl/LHCb/scLHCb","COLD");
    condDBTagMgr->tag(rootName+"/SlowControl/Hcal/scHcal","COLD");
    condDBMgr->commit();    

    // Create new HOT temperature CondDBObjects
    log << MSG::DEBUG 
	<< "Create HOT objects in "+rootName+"/SlowControl/LHCb/scLHCb"
	<< " and "+rootName+"/SlowControl/Hcal/scHcal" << endreq;
    condDBMgr->startUpdate();
    for ( i=0; i<3; i++ ) {
      std::string s;
      ICondDBObject* condObject;
      // LHCb
      i_encodeXmlTemperature( (double)i*10+105, "scLHCb", s);
      condObject = CondDBObjFactory::createCondDBObject
	( i*16, (i+1)*16, s, "LHCb temperature in the given time interval");
      condDBDataAccess->storeCondDBObject
	( rootName+"/SlowControl/LHCb/scLHCb", condObject );
      CondDBObjFactory::destroyCondDBObject(condObject);
      // Hcal
      i_encodeXmlTemperature( (double)i*10+105, "scHcal", s);
      condObject = CondDBObjFactory::createCondDBObject
	( i*16, (i+1)*16, s, "Hcal temperature in the given time interval");
      condDBDataAccess->storeCondDBObject
	( rootName+"/SlowControl/Hcal/scHcal", condObject );
      CondDBObjFactory::destroyCondDBObject(condObject);
    }
    log << MSG::DEBUG << "Create and apply tag HOT" << endreq;
    condDBTagMgr->createCondDBTag("HOT");
    condDBTagMgr->tag(rootName+"/SlowControl/LHCb/scLHCb","HOT");
    condDBTagMgr->tag(rootName+"/SlowControl/Hcal/scHcal","HOT");
    condDBMgr->commit();    

    // Create new geometry CondDBObjects
    log << MSG::DEBUG 
	<< "Create objects in folder "+rootName+"/Geometry/LHCb" << endreq;
    condDBMgr->startUpdate();
    {
      // Copy XML data from a file to a string
      char* fileName;      
      fileName = "../XMLDDDB/LHCb/geometry.xml";
      log << MSG::DEBUG << "Reading XML data from file: " << endreq;
      log << MSG::DEBUG << fileName << endreq;
      std::ifstream inputFile ( fileName );
      std::string xmlString = ""; 
      char ch;
      while ( inputFile.get(ch) ) {
	xmlString = xmlString + ch;
      }     
      if( xmlString == "" ) {
	log << MSG::ERROR << "File is empty" << endreq;
	return StatusCode::FAILURE;
      }
      // Change the DTD relative path location to "file.dtd"
      // This can be correctly interpreted by the XmlCnvSvc
      unsigned int dtdPos = xmlString.find( ".dtd" );
      if( dtdPos < xmlString.length() ) {
	log << MSG::VERBOSE 
	    << "Remove DTD relative path in the XML" << endreq;
	unsigned int slashPos = xmlString.substr(0,dtdPos).rfind("/");
	if( slashPos < dtdPos ) {
	  unsigned int quotePos;
	  if( xmlString[dtdPos+4] == '\'' ) {
	    quotePos = xmlString.substr(0,dtdPos).rfind("\'");
	    log << MSG::VERBOSE << "DTD literal was: " 
		<< xmlString.substr(quotePos,dtdPos+5-quotePos) << endreq;
	    if( quotePos < slashPos ) 
	      xmlString.replace( quotePos+1, slashPos-quotePos, "" );
	    log << MSG::VERBOSE << "DTD literal is now: " 
		<< xmlString.substr(quotePos,dtdPos+5-slashPos) << endreq;
	  } else if( xmlString[dtdPos+4] == '\"' ) {
	    quotePos = xmlString.substr(0,dtdPos).rfind("\"");
	    log << MSG::VERBOSE << "DTD literal was: " 
		<< xmlString.substr(quotePos,dtdPos+5-quotePos) << endreq;
	    if( quotePos < slashPos ) 
	      xmlString.replace( quotePos+1, slashPos-quotePos, "" );
	    log << MSG::VERBOSE << "DTD literal is now: " 
		<< xmlString.substr(quotePos,dtdPos+5-slashPos) << endreq;
	  } else {
	    log << MSG::ERROR
		<< "Bad DTD literal in the string to be parsed" << endreq;
	    return StatusCode::FAILURE;
	  }
	}
      }
      // Now store the XML string in the CondDB
      log << MSG::VERBOSE << "XML data is:" << std::endl 
	  << xmlString << endreq;
      log << MSG::VERBOSE 
	  << "Store it in the database with [-inf,+inf] validity range" 
	  << endreq;
      log << MSG::VERBOSE 
	  << "Folder name: " << rootName+"/Geometry/LHCb" << endreq;
      log << MSG::VERBOSE 
	  << "Create object with validity range: [" << CondDBminusInf
	  << "," << CondDBplusInf << "]" << endreq;
      ICondDBObject* condObject = CondDBObjFactory::createCondDBObject
	(CondDBminusInf, CondDBplusInf, xmlString, "LHCb geometry");
      log << MSG::VERBOSE 
	  << "Created object with validity range: [" 
	  << condObject->validSince() << "," 
	  << condObject->validTill() << "]" << endreq;
      condDBDataAccess->storeCondDBObject
        ( rootName+"/Geometry/LHCb", condObject );
      CondDBObjFactory::destroyCondDBObject(condObject);
    }
    condDBMgr->commit();    

    // Now define the production tag
    // Temperature data for Hcal is hot, for LHCb is cold
    // Geometry data is neither cold nor hot, it is simply production version
    condDBMgr->startUpdate();
    log << MSG::DEBUG << "Create and apply tag PRODUCTION" << endreq;
    condDBTagMgr->createCondDBTag("PRODUCTION");
    condDBTagMgr->tag(rootName+"/SlowControl/LHCb/scLHCb","PRODUCTION","COLD");
    condDBTagMgr->tag(rootName+"/SlowControl/Hcal/scHcal","PRODUCTION","HOT");
    condDBTagMgr->tag(rootName+"/Geometry/LHCb","PRODUCTION");
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
    std::vector<std::string> allCondDBFolderSet;
    std::vector<std::string> allCondDBFolder;
    condDBFolderMgr->getAllCondDBFolderSet ( allCondDBFolderSet );
    condDBFolderMgr->getAllCondDBFolder    ( allCondDBFolder    );
    std::vector<std::string>::const_iterator aName;
    log << MSG::INFO << "CondDB contains " << allCondDBFolderSet.size() 
	<< " CondDBFolderSet(s):";
    for ( aName  = allCondDBFolderSet.begin(); 
	  aName != allCondDBFolderSet.end(); 
	  aName++ ) {
      log << MSG::INFO << std::endl 
	  << "                                      " << *aName;
    }
    log << MSG::INFO << endreq;
    log << MSG::INFO << "CondDB contains " << allCondDBFolder.size() 
	<< " CondDBFolder(s):";
    for ( aName  = allCondDBFolder.begin(); 
	  aName != allCondDBFolder.end(); 
	  aName++ ) {
      log << MSG::INFO << std::endl 
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

  // Dump the content of CondDBFolder rootName+"/SlowControl/LHCb/scLHCb"
  std::string folderName;
  StatusCode status;
  folderName = rootName+"/SlowControl/LHCb/scLHCb";
  status = i_dumpFolder( folderName, "COLD" );
  if (! status.isSuccess() ) return status;
  status = i_dumpFolder( folderName, "HOT" );
  if (! status.isSuccess() ) return status;
  status = i_dumpFolder( folderName, "PRODUCTION" );
  if (! status.isSuccess() ) return status;

  // Dump the content of CondDBFolder rootName+"/SlowControl/Hcal/scHcal"
  folderName = rootName+"/SlowControl/Hcal/scHcal";
  status = i_dumpFolder( folderName, "COLD" );
  if (! status.isSuccess() ) return status;
  status = i_dumpFolder( folderName, "HOT" );
  if (! status.isSuccess() ) return status;
  status = i_dumpFolder( folderName, "PRODUCTION" );
  if (! status.isSuccess() ) return status;

  // Dump the content of CondDBFolder rootName+"/Geometry/LHCb"
  folderName = rootName+"/Geometry/LHCb";
  status = i_dumpFolder( folderName, "PRODUCTION" );
  if (! status.isSuccess() ) return status;

  log << MSG::INFO 
      << "============= condDBDumpSampleData() ending ========================"
      << endreq;
  return StatusCode::SUCCESS;  

}

//----------------------------------------------------------------------------

void PopulateDB::i_encodeXmlTemperature( const double temperature,
					 const std::string& objName,
					 std::string& xmlString   ) {
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE 
      << "Encoding XML string for name=" << objName 
      << " and temperature=" << temperature << endreq;
  char* cTemp = new char[15];
  sprintf(cTemp,"%-1.8e",temperature);
  std::string sTemp(cTemp);
  delete[] cTemp;
  xmlString  = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  xmlString += "<!DOCTYPE DDDB SYSTEM \"structure.dtd\">\n";
  xmlString += "<DDDB>\n";
  xmlString += "  <condition name=\"" + objName + "\">\n";
  xmlString += "    <param name=\"Temperature\" type=\"double\">\n";
  xmlString += "      " + sTemp + "\n";
  xmlString += "    </param>\n";
  xmlString += "  </condition>\n";
  xmlString += "</DDDB>\n";  
  log << MSG::VERBOSE << "Encoded XML string is:" << std::endl
      << xmlString << endreq;
  return;
};

//----------------------------------------------------------------------------

/// Dump the contents of a CondDBFolder
StatusCode PopulateDB::i_dumpFolder( const std::string& folderName,
				     const std::string& tagName ) 
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "--------------------------------------------------------------------"
      << endreq;
  log << MSG::INFO << "Dump the contents of folder " << folderName 
      << " inside tag " << tagName << endreq;

  // Data access to the CondDB
  ICondDBMgr*        condDBMgr        = m_conditionsDBGate->condDBManager();
  ICondDBDataAccess* condDBDataAccess = condDBMgr->getCondDBDataAccess();

  // Iterator over objects in the folder for this tag
  ICondDBDataIterator* it;
  try {
    condDBMgr->startRead();
    // Retrieve an iterator (positioned at minus infinity)
    condDBDataAccess->browseObjectsInTag( it, folderName, tagName );
    // Although this is not necessary, this cannot harm
    it->goToFirst();
    condDBMgr->commit();
  } catch (CondDBException &e) {
    log << MSG::ERROR
	<< "Could not browse objects in tag" << endreq;
    log << MSG::ERROR
	<< "*** ConditionsDB exception caught:"          << endreq;
    log << MSG::ERROR
	<< "***  error message: " << e.getMessage()      << endreq;
    log << MSG::ERROR
	<< "***  error code:    " << e.getErrorCode()    << endreq;
    return StatusCode::FAILURE;
  } 

  // Iterate over all objects
  condDBMgr->startRead();
  ICondDBObject* pObj;
  std::string theData;  
  pObj = it->current();
  do {
    if( pObj->isNullInterval() ) {
      theData = "NULL INTERVAL";
    } else {
      pObj->data( theData );
    }
    log << MSG::INFO 
	<< "Data since " << pObj->validSince() 
	<< " till " << pObj->validTill() << " is:" << std::endl
	<< theData << endreq;
    // In the Objy implementation, next() returns 0 on the last object
    // In the Oracle one, next() throws an exception on the last object
    // Portable solution: check with hasNext() if this is the last object
    if ( it->hasNext() ) {
      pObj = it->next();
    } else {
      pObj = 0;
    }
  } while ( pObj != 0 );
  condDBMgr->commit();
  return StatusCode::SUCCESS;
}
  
//----------------------------------------------------------------------------
