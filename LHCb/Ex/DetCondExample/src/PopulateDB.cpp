//$Id: PopulateDB.cpp,v 1.11 2005-02-10 08:06:21 marcocle Exp $
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "PopulateDB.h"

#include "DetDesc/Condition.h"

#include "DetCond/IConditionsDBCnvSvc.h"
#include "DetCond/ICondDBAccessSvc.h"
//#include "DetCond/XmlRelyCnv.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/TimePoint.h"

#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/types.h"

// from POOL
#include "AttributeList/AttributeListSpecification.h"
#include "AttributeList/AttributeList.h"

/// Instantiation of a static factory to create instances of this algorithm
static const AlgFactory<PopulateDB> Factory;
const IAlgFactory& PopulateDBFactory = Factory;

// Define the name of the root folderset
// This used to be "/" in Objectivity.
// In the Oracle implementation it is required to have a name "/XXX"
const std::string rootName = "/CONDDB";  
//const std::string rootName = "/";  

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

  StatusCode sc;
  
  // Locate the ConditionsDBCnvSvc
  // This is used to encode folder data to be written in the CondDB
  /*
  sc = serviceLocator()->service 
    ( "ConditionsDBCnvSvc", m_conditionsDBCnvSvc );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't locate ConditionsDBCnvSvc" << endreq;
    return sc;
  } else {
    log << MSG::INFO << "Succesfully located ConditionDBCnvSvc" << endreq;
  }
  */

  // Locate the Database Access Service
  sc = serviceLocator()->getService("CondDBAccessSvc",
                                    ICondDBAccessSvc::interfaceID(),(IInterface*&)m_dbAccSvc);
  if (  !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not locate CondDBAccessSvc" << endreq;
    return sc;
  }

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

  // Get a pointer to the DB to speed up a bit
  cool::IDatabasePtr &db = m_dbAccSvc->database();
  
  // Check if root folderSet exists
  log << MSG::INFO 
      << "Checking if CondDB root folderSet `"
      << rootName << "' already exists" << endreq;
  log << MSG::INFO 
      << "" << endreq;
  bool rootFolderSetExists;
  rootFolderSetExists = db->existsFolderSet( rootName );
  
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
      db->createFolderSet(rootName);
    } catch (cool::Exception &e) {
      log << MSG::ERROR
          << e << endreq;
      return StatusCode::FAILURE;
    }
    log << MSG::INFO 
        << "Root folderSet did not exist and was succesfully created" 
        << endreq;
  }
  
  // Store sample data
  log << MSG::INFO << "Now store sample data into the CondDB" << endreq;

  try {
    
    // Prepare AttributeListSpecification
    
    //    log << MSG::DEBUG << "Get AttributeListSpecification" << endmsg;
    //    XmlRelyCnv::attrListSpec();
    //    log << MSG::DEBUG << "Get AttributeListSpecification again (testing cache)" << endmsg;
    //    pool::AttributeListSpecification &attListSpec = XmlRelyCnv::attrListSpec();
    log << MSG::DEBUG << "Prepare AttributeListSpecification" << endmsg;
    pool::AttributeListSpecification attListSpec;
    // attListSpec.push_back<long>("storage_type");
    attListSpec.push_back<std::string>("data");

    // Encode the string storage type into the folder description
    unsigned char type;
    std::string description = "<storage_type=";
    description += XML_StorageType;
    description += ">";
    StatusCode status;

    // Create new CondDBFolderSets
    log << MSG::DEBUG << "Create folder sets" << endreq;

    db->createFolderSet( rootName+"/SlowControl",
                         "Main SlowControl folderSet", true );
    db->createFolderSet( rootName+"/SlowControl/LHCb",
                         "SlowControl folderSet for the LHCb detector", true ); 
    db->createFolderSet( rootName+"/SlowControl/Hcal",
                         "SlowControl folderSet for the Hcal detector", true ); 
    db->createFolderSet( rootName+"/Geometry",
                         "Main Geometry folderSet", true ); 
    db->createFolderSet( rootName+"/Geometry2",
                         "Test Geometry folderSet", true ); 
    db->createFolderSet( rootName+"/Alignment",
                         "Main Alignment folderSet", true );
    db->createFolderSet( rootName+"/Alignment/Ecal",
                         "Ecal Alignment folderSet", true );

    log << MSG::DEBUG 
        << "Create folders "+rootName+"/SlowControl/LHCb/scLHCb"
        << " and "+rootName+"/SlowControl/Hcal/scHcal" << endreq;

    cool::IFolderPtr fldr;
    // Create CondDBFolders with temperature data
    // For simplicity, create two folders with the same objects
    db->createFolder( rootName+"/pippo", attListSpec, description,
                      cool::FolderVersioning::ONLINE, true );
    db->createFolder( rootName+"/scLHCb", attListSpec, description,
                      cool::FolderVersioning::ONLINE, true );
    db->createFolder( rootName+"/SlowControl/LHCb/scLHCb", attListSpec, description,
                      cool::FolderVersioning::ONLINE, true );
    db->createFolder( rootName+"/SlowControl/Hcal/scHcal", attListSpec, description,
                      cool::FolderVersioning::ONLINE, true );
        

    log << MSG::DEBUG << "Create folder "+rootName+"/Geometry/LHCb" << endreq;
    // Create CondDBFolders with XML catalogs
    db->createFolder( rootName+"/Geometry/LHCb", attListSpec, description,
                      cool::FolderVersioning::ONLINE, true );
    db->createFolder( rootName+"/Geometry2/LHCb", attListSpec, description,
                      cool::FolderVersioning::ONLINE, true );
    db->createFolder( rootName+"/Geometry2/lvLHCb", attListSpec, description,
                      cool::FolderVersioning::ONLINE, true );
    db->createFolder( rootName+"/Alignment/Ecal/alEcal", attListSpec, description,
                      cool::FolderVersioning::ONLINE, true );


    pool::AttributeList payload(attListSpec);
    // payload["storage_type"].setValue<long>(XML_StorageType);
    
    // Create new COLD temperature CondDBObjects
    log << MSG::DEBUG 
        << "Create COLD objects in "+rootName+"/SlowControl/LHCb/scLHCb"
        << " and "+rootName+"/SlowControl/Hcal/scHcal" << endreq;
    int i;
    for ( i=0; i<3; i++ ) {
      std::string s;

      // LHCb
      i_encodeXmlTemperature( (double)i*10+5, "scLHCb", s);
      payload["data"].setValue<std::string>(s);      
      db->getFolder(rootName+"/SlowControl/LHCb/scLHCb")->storeObject(i*16, (i+1)*16,
                                                                      payload);
      // Hcal
      i_encodeXmlTemperature( (double)i*10+5, "scHcal", s);
      payload["data"].setValue<std::string>(s);      
      db->getFolder(rootName+"/SlowControl/Hcal/scHcal")->storeObject(i*16, (i+1)*16,
                                                                      payload);
    }

    /* Tagging not yet implemented
    log << MSG::DEBUG << "Create and apply tag COLD" << endreq;
    condDBTagMgr->createCondDBTag("COLD");
    condDBTagMgr->tag(rootName+"/SlowControl/LHCb/scLHCb","COLD");
    condDBTagMgr->tag(rootName+"/SlowControl/Hcal/scHcal","COLD");
    condDBMgr->commit();    
    */

    /* Only ONLINE MODE... cannot add more versions
    // Create new HOT temperature CondDBObjects
    log << MSG::DEBUG 
        << "Create HOT objects in "+rootName+"/SlowControl/LHCb/scLHCb"
        << " and "+rootName+"/SlowControl/Hcal/scHcal" << endreq;
    for ( i=0; i<3; i++ ) {
      std::string s;

      // LHCb
      i_encodeXmlTemperature( (double)i*10+105, "scLHCb", s);
      payload["data"].setValue<std::string>(s);      
      db->getFolder(rootName+"/SlowControl/LHCb/scLHCb")->storeObject(i*16, (i+1)*16,
                                                                      payload);
      // Hcal
      i_encodeXmlTemperature( (double)i*10+105, "scHcal", s);
      payload["data"].setValue<std::string>(s);      
      db->getFolder(rootName+"/SlowControl/Hcal/scHcal")->storeObject(i*16, (i+1)*16,
                                                                      payload);
    }
    */
    /*
    log << MSG::DEBUG << "Create and apply tag HOT" << endreq;
    condDBTagMgr->createCondDBTag("HOT");
    condDBTagMgr->tag(rootName+"/SlowControl/LHCb/scLHCb","HOT");
    condDBTagMgr->tag(rootName+"/SlowControl/Hcal/scHcal","HOT");
    condDBMgr->commit();    
    */

    // Create new geometry CondDBObjects
    log << MSG::DEBUG 
        << "Create objects in folder "+rootName+"/Geometry/LHCb" << endreq;
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
          << "Create object with validity range: [" << cool::IValidityKeyMin
          << "(0x" << std::hex 
          << cool::IValidityKeyMin
          << std::dec << ")" 
          << "," << cool::IValidityKeyMax
          << "(0x" << std::hex 
          << cool::IValidityKeyMax
          << std::dec << ")" 
          << "]" << endreq;

      payload["data"].setValue<std::string>(xmlString);
      db->getFolder(rootName+"/Geometry/LHCb")->storeObject(cool::IValidityKeyMin,cool::IValidityKeyMax,
                                                            payload);
    }

    /*
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
    */

    // populate $root/Alignment/Ecal/alEcal folder for testing with XmlTestAlg

    double ecalpos[3][3] = { { 0,0,0 }, { 0,1,0 }, { 0,0,2 } };    
    double fallbackpos[3] = {-1.,-1.,-1.};
    
    //    for ( i=-1; i<3; i++ ) {
    for ( i=-1; i<3; i++ ) {
      std::string s;
      // alEcal
      if (i>=0){
        i_encodeXmlParamVector( ecalpos[i%3], "alEcal","Ecal position", s);
        payload["data"].setValue<std::string>(s);
        db->getFolder(rootName+"/Alignment/Ecal/alEcal")->storeObject(i*16,
                                                                      (i+1)*16,
                                                                      payload);
      } else {
        i_encodeXmlParamVector( fallbackpos, "alEcal","Ecal position", s);
        payload["data"].setValue<std::string>(s);
        db->getFolder(rootName+"/Alignment/Ecal/alEcal")->storeObject(cool::IValidityKeyMin,
                                                                      cool::IValidityKeyMax,
                                                                      payload);
      }
    }

    std::string s;

    s = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE DDDB SYSTEM \"geometry.dtd\">";
    s += "<DDDB><catalog name=\"LHCb\"><logvolref href=\"conddb:";
    s += rootName;
    s += "/Geometry2/lvLHCb#lvLHCb\"/></catalog></DDDB>";
    
    payload["data"].setValue<std::string>(s);
    db->getFolder(rootName+"/Geometry2/LHCb")->storeObject(cool::IValidityKeyMin,
                                                                  cool::IValidityKeyMax,
                                                                  payload);

    s = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE DDDB SYSTEM \"geometry.dtd\">";
    s += "<DDDB><logvol material=\"Vacuum\" name=\"lvLHCb\">";
    s += "<box name=\"caveBox\" sizeX=\"50000*mm\" sizeY=\"50000*mm\" sizeZ=\"50000*mm\"/>";
    s += "<physvol logvol=\"/dd/Geometry/Ecal/lvEcal\" name=\"EcalSubsystem\"><posXYZ x=\"0*mm\" y=\"0*mm\" z=\"12907.5*mm\"/>";
    s += "</physvol><physvol logvol=\"/dd/Geometry/Hcal/lvHcal\" name=\"HcalSubsystem\">";
    s += "<posXYZ x=\"0*mm\" y=\"0*mm\" z=\"14162.5*mm\"/></physvol></logvol></DDDB>";

    payload["data"].setValue<std::string>(s);
    db->getFolder(rootName+"/Geometry2/lvLHCb")->storeObject(cool::IValidityKeyMin,
                                                                  cool::IValidityKeyMax,
                                                                  payload);
  } catch (cool::Exception &e) {

    log << MSG::ERROR
        << "Error in storing sample data into the CondDB" << endreq;
    log << MSG::ERROR
        << "*** COOL exception caught: " << endreq;
    log << MSG::ERROR
    	<< "***   error message: " << e << endreq;
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

  cool::IDatabasePtr &db = m_dbAccSvc->database();

  // List all stored Folders 
  std::vector<std::string> fldr_names = db->listFolders();
  log << MSG::INFO << " --> List of CondDB Folders" << endmsg;
  for ( std::vector<std::string>::iterator fldr_name = fldr_names.begin();
        fldr_name != fldr_names.end(); fldr_name++ ){
    bool isFolderSet = db->existsFolderSet(*fldr_name);
    if (isFolderSet) {
      log << MSG::INFO << " S "; // Flag FolderSets
    } else {
      log << MSG::INFO << "   ";
    }
    log << MSG::INFO << *fldr_name << "  " <<  endmsg;
    if ( !isFolderSet ) { // dump the content of the folder
      i_dumpFolder( *fldr_name, "" );
    }    
  }
  
  

#ifdef NODEF
  ICondDBMgr*        condDBMgr        = m_conditionsDBGate->condDBManager();
  ICondDBBasicFolderMgr*  condDBFolderMgr  = condDBMgr->getCondDBBasicFolderMgr();

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
  log << MSG::INFO << "Iteratively list contents of CondDB" << endreq;
  std::vector<std::string> allCondDBFolderSet;
  std::vector<std::string> allCondDBFolder;
  try {
    condDBMgr->startRead();
    condDBFolderMgr->getAllCondDBFolderSet ( allCondDBFolderSet );
    condDBFolderMgr->getAllCondDBFolder    ( allCondDBFolder    );
    condDBMgr->commit();
  } catch (CondDBException &e) {
    log << MSG::ERROR << "Could not retrieve CondDBFolder/Set lists" << endreq;
    log << MSG::ERROR << "*** ConditionsDB exception caught: "       << endreq;
    log << MSG::ERROR << "***  error message: " << e.getMessage()    << endreq;
    log << MSG::ERROR << "***  error code:    " << e.getErrorCode()  << endreq;
    return StatusCode::FAILURE;
  }
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

#endif

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

  cool::IFolderPtr folder = m_dbAccSvc->database()->getFolder(folderName);

  //  std::vector<cool::IObjectPtr> objs = folder->browseObjectsInTag(tagName);
  cool::IObjectIteratorPtr objs = folder->browseObjectsInTag(tagName);
  if ( objs->isEmpty() ) {
    log << MSG::INFO << "    | no objects in folder" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  objs->goToStart();
  while (objs->hasNext()){
    cool::IObjectPtr objp = objs->next();
    
    log << MSG::INFO << "    | " << objp->since() << " -> " << objp->till() << endmsg;
    log << MSG::INFO << "      size = " << objp->payload().size() << endmsg;
    
    for (pool::AttributeList::const_iterator it = objp->payload().begin();
         it!=objp->payload().end(); it++){
      log << MSG::INFO << "      " << it->spec().name()  << " (" <<  it->spec().type_name() << ") =" << endmsg;
      log << MSG::INFO << "        " << it->getValueAsString() << endmsg;
    }
  }

  
#ifdef NODEF
  log << MSG::INFO 
      << "--------------------------------------------------------------------"
      << endreq;
  log << MSG::INFO << "Dump the contents of folder " << folderName 
      << " inside tag " << tagName << endreq;

  // #ifdef NODEF
  // Data access to the CondDB
  ICondDBMgr*        condDBMgr        = m_conditionsDBGate->condDBManager();
  ICondDBBasicDataAccess* condDBDataAccess = condDBMgr->getCondDBBasicDataAccess();

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
	<< "(0x" << std::hex 
	<< pObj->validSince() 
	<< std::dec << ")" 
	<< " till " << pObj->validTill() 
	<< "(0x" << std::hex 
	<< pObj->validTill() 
	<< std::dec << ")" 
	<< " is:" << std::endl
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
#endif
  return StatusCode::SUCCESS;
}
  
//----------------------------------------------------------------------------

void PopulateDB::i_encodeXmlParamVector( const double pos[3],
                                           const std::string& objName,
                                           const std::string& parName,
                                           std::string& xmlString   ) {
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE 
      << "Encoding XML string for name=" << objName 
      << " and position=" << pos[0] << " " << pos[1] << " " << pos[2] << endreq;
  std::ostringstream sTemp;
  sTemp << pos[0] << " " << pos[1] << " " << pos[2];
  xmlString  = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  xmlString += "<!DOCTYPE DDDB SYSTEM \"structure.dtd\">\n";
  xmlString += "<DDDB>\n";
  xmlString += "  <condition name=\"" + objName + "\">\n";
  xmlString += "    <paramVector name=\""+ parName +"\" type=\"double\">\n";
  xmlString += "      " + sTemp.str() + "\n";
  xmlString += "    </paramVector>\n";
  xmlString += "  </condition>\n";
  xmlString += "</DDDB>\n";  
  log << MSG::VERBOSE << "Encoded XML string is:" << std::endl
      << xmlString << endreq;
  return;
};

