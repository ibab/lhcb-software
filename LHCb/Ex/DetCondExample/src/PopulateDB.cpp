// $Id: PopulateDB.cpp,v 1.29 2006-04-25 17:26:07 marcocle Exp $
// Include files
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Time.h"
#include "GaudiKernel/DeclareFactoryEntries.h" 

#include "Kernel/SystemOfUnits.h"

#include "DetDesc/Condition.h"
#include "DetDesc/AlignmentCondition.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetCond/ICondDBAccessSvc.h"

// from COOL
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/types.h"

// from CORAL
#include "CoralBase/AttributeList.h"

// local
#include "PopulateDB.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PopulateDB
//
// 2005-06-22 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PopulateDB );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PopulateDB::PopulateDB( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
PopulateDB::~PopulateDB() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode PopulateDB::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  try {

    // Locate the Database Access Service
    m_dbAccSvc = svc<ICondDBAccessSvc>("CondDBAccessSvc",true);

    // Store sample data if the database is empty
    info() << "Store sample data in the database if empty" << endmsg;
    sc = i_condDBStoreSampleData();
    if ( !sc.isSuccess() ) return sc;
    
    // Dump the database to make sure it is not empty
    info() << "Dump sample data from the database" << endmsg;
    sc = i_condDBDumpSampleData();
    if ( !sc.isSuccess() ) return sc;
    
    info() << "Initialization completed" << endmsg;
    
  } catch (GaudiException &e) {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PopulateDB::execute() {

  //  debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PopulateDB::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  Populate the database 
//=========================================================================
StatusCode PopulateDB::i_condDBStoreSampleData() {
  info() << "============= condDBStoreSampleData() starting =====================" << endmsg;

  // Get a pointer to the DB to speed up a bit
  cool::IDatabasePtr &db = m_dbAccSvc->database();

  // Check for signature (a folderset called "/_"+name())
  std::string signature_folderset = "/_";
  signature_folderset += name();
  info() << "Checking if CondDB tree has been already filled" << endmsg;
  bool signatureFolderSetExists = db->existsFolderSet(signature_folderset);

  // If root folderSet exists already then return
  // If root folderSet does not exist then create it and store sample data
  if (signatureFolderSetExists) {
    info() << "Signature folderSet already exists: no need to store sample data" << endmsg; 
    info() << "============= condDBStoreSampleData() ending =======================" << endmsg;
    return StatusCode::SUCCESS;
  } else {      
    info() << "Signature folderSet does not exist: create it" << endmsg;
    try {
      m_dbAccSvc->createFolder(signature_folderset,
                               "Dummy folderset used to sign the prepared database",
                               ICondDBAccessSvc::FOLDERSET);
    } catch (cool::Exception &e) {
      error() << e.what() << endmsg;
      return StatusCode::FAILURE;
    }
    info() << "Signature folderSet did not exist and was succesfully created" << endmsg;
  }
  // Store sample data
  info() << "Now store sample data into the CondDB" << endmsg;

  try {
    
    StatusCode sc;

    // Create new CondDBFolderSets

    typedef std::pair<std::string,std::string> str_pair;
    typedef std::vector<str_pair> vec_str_pair;
    vec_str_pair foldersets;
    foldersets.reserve(10);
    foldersets.push_back(str_pair("/SlowControl",     "Main SlowControl folderSet"));
    foldersets.push_back(str_pair("/SlowControl/LHCb","SlowControl folderSet for the LHCb detector"));
    foldersets.push_back(str_pair("/SlowControl/Hcal","SlowControl folderSet for the Hcal detector"));
    foldersets.push_back(str_pair("/ReadOut",         "Main ReadOut folderSet"));
    foldersets.push_back(str_pair("/Properties",      "FolderSet for tabulated properties"));
    foldersets.push_back(str_pair("/Geometry",        "Main Geometry folderSet"));
    foldersets.push_back(str_pair("/Geometry2",       "Test Geometry folderSet"));
    foldersets.push_back(str_pair("/Alignment",       "Main Alignment folderSet"));
    foldersets.push_back(str_pair("/Alignment/Velo",  "Velo Alignment folderSet"));
    foldersets.push_back(str_pair("/OnLine/Cave",     "Measurements for the cave"));

    debug() << "Create foldersets" << endmsg;
    for(vec_str_pair::iterator f = foldersets.begin(); f != foldersets.end(); ++f ){
      sc = m_dbAccSvc->createFolder(f->first,f->second,ICondDBAccessSvc::FOLDERSET);
      if (!sc.isSuccess()){
        error() << "Unable to create folderset \"" << f->first << '"' << endmsg;
        return sc;
      }
    }

    vec_str_pair xmlfolders;
    xmlfolders.reserve(10);
    xmlfolders.push_back(str_pair("/SlowControl/LHCb/scLHCb",""));
    xmlfolders.push_back(str_pair("/SlowControl/Hcal/scHcal",""));
    xmlfolders.push_back(str_pair("/Geometry/LHCb",""));
    xmlfolders.push_back(str_pair("/Geometry2/LHCb",""));
    xmlfolders.push_back(str_pair("/Geometry2/lvLHCb",""));
    xmlfolders.push_back(str_pair("/Alignment/Velo/Modules",""));
    xmlfolders.push_back(str_pair("/SlowControl/DummyDE",""));
    xmlfolders.push_back(str_pair("/ReadOut/DummyDE",""));
    xmlfolders.push_back(str_pair("/Properties/TestFunction",""));

    xmlfolders.push_back(str_pair("/TestFolder",""));

    debug() << "Create folders (multi-version)" << endmsg;
    for(vec_str_pair::iterator f = xmlfolders.begin(); f != xmlfolders.end(); ++f ){
      sc = m_dbAccSvc->createFolder(f->first,f->second,ICondDBAccessSvc::XML);
      if (!sc.isSuccess()){
        error() << "Unable to create XML folder \"" << f->first << '"' << endmsg;
        return sc;
      }
    }

    vec_str_pair xmlfolders_online;
    xmlfolders_online.reserve(3);
    xmlfolders_online.push_back(str_pair("/OnLine/Cave/T1","Temperature 1"));
    xmlfolders_online.push_back(str_pair("/OnLine/Cave/T2","Temperature 2"));
    xmlfolders_online.push_back(str_pair("/OnLine/Cave/P1","Pressure 1"));
    
    debug() << "Create folders (single-version)" << endmsg;
    for(vec_str_pair::iterator f = xmlfolders_online.begin(); f != xmlfolders_online.end(); ++f ){
      sc = m_dbAccSvc->createFolder(f->first,f->second,ICondDBAccessSvc::XML,ICondDBAccessSvc::SINGLE);
      if (!sc.isSuccess()){
        error() << "Unable to create XML folder \"" << f->first << '"' << endmsg;
        return sc;
      }
    }
    
    // Create new COLD temperature CondDBObjects
    debug() << "Create COLD objects in /SlowControl/LHCb/scLHCb and /SlowControl/Hcal/scHcal" << endmsg;
    int i;
    for ( i=0; i<3; i++ ) {

      // LHCb
      m_dbAccSvc->storeXMLString("/SlowControl/LHCb/scLHCb",
                                 i_encodeXmlTemperature( (double)i*10+5, "scLHCb" ),
                                 Gaudi::Time(i*16), Gaudi::Time((i+1)*16));
      // Hcal
      m_dbAccSvc->storeXMLString("/SlowControl/Hcal/scHcal",
                                 i_encodeXmlTemperature( (double)i*10.4+4.2, "scHcal"),
                                 Gaudi::Time(i*16), Gaudi::Time((i+1)*16));
    }

    debug() << "Apply tag COLD" << endmsg;
    m_dbAccSvc->tagFolder("/SlowControl","COLD");
    debug() << "Apply tag PRODUCTION to /SlowControl/LHCb/scLHCb" << endmsg; 
    m_dbAccSvc->tagFolder("/SlowControl/LHCb/scLHCb","PRODUCTION");

    // Create new HOT temperature CondDBObjects
    debug() << "Create HOT objects in /SlowControl/LHCb/scLHCb"
            << " and /SlowControl/Hcal/scHcal" << endmsg;
    for ( i=0; i<3; i++ ) {

      // LHCb
      m_dbAccSvc->storeXMLString("/SlowControl/LHCb/scLHCb",
                                 i_encodeXmlTemperature( (double)i*10+105, "scLHCb" ),
                                 Gaudi::Time(i*16), Gaudi::Time((i+1)*16));
      // Hcal
      m_dbAccSvc->storeXMLString("/SlowControl/Hcal/scHcal",
                                 i_encodeXmlTemperature( (double)i*10+105, "scHcal"),
                                 Gaudi::Time(i*16), Gaudi::Time((i+1)*16));
    }

    debug() << "Apply tag HOT" << endmsg;
    m_dbAccSvc->tagFolder("/SlowControl","HOT");
    debug() << "Apply tag PRODUCTION to /SlowControl/Hcal" << endmsg; 
    m_dbAccSvc->tagFolder("/SlowControl/Hcal","PRODUCTION");

    // Create new geometry CondDBObjects
    debug() << "Create objects in folder /Geometry/LHCb" << endmsg;
    {
      // Copy XML data from a file to a string
      char* fileName;      
      fileName = "../XMLDDDB/LHCb/geometry.xml";
      debug() << "Reading XML data from file: " << endmsg;
      debug() << fileName << endmsg;
      std::ifstream inputFile ( fileName );
      std::string xmlString = ""; 
      char ch;
      while ( inputFile.get(ch) ) {
        xmlString = xmlString + ch;
      }     
      if( xmlString == "" ) {
        error() << "File is empty" << endmsg;
        return StatusCode::FAILURE;
      }
      // Change the DTD relative path location to "file.dtd"
      // This can be correctly interpreted by the XmlCnvSvc
      unsigned int dtdPos = xmlString.find( ".dtd" );
      if( dtdPos < xmlString.length() ) {
        verbose() << "Remove DTD relative path in the XML" << endmsg;
        unsigned int slashPos = xmlString.substr(0,dtdPos).rfind("/");
        if( slashPos < dtdPos ) {
          unsigned int quotePos;
          if( xmlString[dtdPos+4] == '\'' ) {
            quotePos = xmlString.substr(0,dtdPos).rfind("\'");
            verbose() << "DTD literal was: " 
                      << xmlString.substr(quotePos,dtdPos+5-quotePos) << endmsg;
            if( quotePos < slashPos ) 
              xmlString.replace( quotePos+1, slashPos-quotePos, "" );
            verbose() << "DTD literal is now: " 
                      << xmlString.substr(quotePos,dtdPos+5-slashPos) << endmsg;
          } else if( xmlString[dtdPos+4] == '\"' ) {
            quotePos = xmlString.substr(0,dtdPos).rfind("\"");
            verbose() << "DTD literal was: " 
                      << xmlString.substr(quotePos,dtdPos+5-quotePos) << endmsg;
            if( quotePos < slashPos ) 
              xmlString.replace( quotePos+1, slashPos-quotePos, "" );
            verbose() << "DTD literal is now: " 
                      << xmlString.substr(quotePos,dtdPos+5-slashPos) << endmsg;
          } else {
            error() << "Bad DTD literal in the string to be parsed" << endmsg;
            return StatusCode::FAILURE;
          }
        }
      }
      // Now store the XML string in the CondDB
      verbose() << "XML data is:" << std::endl << xmlString << endmsg;
      verbose() << "Store it in the database with [-inf,+inf] validity range" << endmsg;
      verbose() << "Folder name: /Geometry/LHCb" << endmsg;
      verbose() << "Create object with validity range: [" << cool::ValidityKeyMin
                << "(0x" << std::hex << cool::ValidityKeyMin << std::dec << ")" 
                << "," << cool::ValidityKeyMax << "(0x" << std::hex << cool::ValidityKeyMax << std::dec << ")]" << endmsg;

      m_dbAccSvc->storeXMLString("/Geometry/LHCb",xmlString,Gaudi::Time::epoch(),Gaudi::Time::max());
    }

    debug() << "Apply tag PRODUCTION to /Geometry/LHCb" << endmsg;
    m_dbAccSvc->tagFolder("/Geometry/LHCb","PRODUCTION");

    debug() << "Put in a new HEAD" << endmsg;
    for ( i=0; i<2; i++ ) {
      // LHCb
      m_dbAccSvc->storeXMLString("/SlowControl/LHCb/scLHCb",
                                 i_encodeXmlTemperature( (double)i*5+9.2, "scLHCb" ),
                                 Gaudi::Time(i*24), Gaudi::Time((i+1)*24));
    }
    for ( i=0; i<10; i++ ) {
      // Hcal
      m_dbAccSvc->storeXMLString("/SlowControl/Hcal/scHcal",
                                 i_encodeXmlTemperature( (double)i*5.4+9, "scHcal" ),
                                 Gaudi::Time(i*10), Gaudi::Time((i+1)*10));
    }

    /// ------------ DummyDE conditions
    for ( i=0; i<3; ++i ) {
      m_dbAccSvc->storeXMLString("/SlowControl/DummyDE",
                                 i_encodeXmlTemperature((double)i*4.17+25.16, "DummyDE" ),
                                 Gaudi::Time(i*24), Gaudi::Time((i+1)*24));
    }
    for ( i=0; i<3; ++i ) {
      Condition temp;
      std::vector<double> temps;
      std::vector<std::string> temps_str;
      for ( int j=0; j<10; ++j ) {
        temps.push_back((double)i*4.17+20.16-(double)j*0.92);
      }
      temp.addParam("CrateTemps",temps);
      temp.addParam("NChannels",(int)10);
      std::vector<int> chnls;
      for ( int j=0; j<10; j += 1+i ) {
        chnls.push_back(j);
      }
      temp.addParam("Channels",chnls);
      std::string s = temp.toXml("DummyDE");
      verbose() << "/ReadOut/DummyDE: " << s << endmsg;
      m_dbAccSvc->storeXMLString("/ReadOut/DummyDE",s,Gaudi::Time(i*24), Gaudi::Time((i+1)*24));
    }

    /// test alignment condition
    std::vector<double> pos(3);
    std::vector<double> rot(3);
    
    AlignmentCondition align(pos,rot);

    // store null alignment (for 42 modules)
    for ( i = 0; i < 42; ++i ) {
      std::ostringstream obj_name;
      obj_name << "Module";
      obj_name.width(2);
      obj_name.fill('0');
      obj_name << i;
      m_dbAccSvc->storeXMLString("/Alignment/Velo/Modules",
                                 align.toXml(obj_name.str()),
                                 Gaudi::Time::epoch(), Gaudi::Time::max(), i);
    }

    // change placement of one of the modules
    pos[2] = 2*cm;
    AlignmentCondition align2(pos,rot);
    m_dbAccSvc->storeXMLString("/Alignment/Velo/Modules",
                               align2.toXml("Module12"),
                               Gaudi::Time(30), Gaudi::Time(50), 12);

    std::string s;

    s = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE DDDB SYSTEM \"geometry.dtd\">";
    s += "<DDDB><catalog name=\"LHCb\"><logvolref href=\"conddb:";
    s += "/Geometry2/lvLHCb#lvLHCb\"/></catalog></DDDB>";
    
    m_dbAccSvc->storeXMLString("/Geometry2/LHCb",s,Gaudi::Time::epoch(),Gaudi::Time::max());

    s = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE DDDB SYSTEM \"geometry.dtd\">";
    s += "<DDDB><logvol material=\"Vacuum\" name=\"lvLHCb\">";
    s += "<box name=\"caveBox\" sizeX=\"50000*mm\" sizeY=\"50000*mm\" sizeZ=\"50000*mm\"/>";
    s += "<physvol logvol=\"/dd/Geometry/Ecal/lvEcal\" name=\"EcalSubsystem\"><posXYZ x=\"0*mm\" y=\"0*mm\" z=\"12907.5*mm\"/>";
    s += "</physvol><physvol logvol=\"/dd/Geometry/Hcal/lvHcal\" name=\"HcalSubsystem\">";
    s += "<posXYZ x=\"0*mm\" y=\"0*mm\" z=\"14162.5*mm\"/></physvol></logvol></DDDB>";

    m_dbAccSvc->storeXMLString("/Geometry2/lvLHCb",s,Gaudi::Time::epoch(),Gaudi::Time::max());
    
    debug() << "Tagging /Geometry2" << endmsg;
    m_dbAccSvc->tagFolder("/Geometry2","COLD");
    m_dbAccSvc->tagFolder("/Geometry2","HOT");
    m_dbAccSvc->tagFolder("/Geometry2","PRODUCTION");

    // ---- Add a tabulated property
    debug() << "Add a TabulatedProperty from 0 to 30" << endmsg;
    TabulatedProperty tp1;
    for(double x = 0. ; x < 2. ; x += 0.2){
      double y = x*x;
      tp1.table().push_back(TabulatedProperty::Entry(x,y));
    }
    m_dbAccSvc->storeXMLString("/Properties/TestFunction",tp1.toXml("TestFunction"),Gaudi::Time(0), Gaudi::Time(30));

    debug() << "Add a second TabulatedProperty from 30 to +inf" << endmsg;
    tp1.table().clear();
    for(double x = 0. ; x < 2. ; x += 0.2){
      double y = x*x*0.5;
      tp1.table().push_back(TabulatedProperty::Entry(x,y));
    }
    m_dbAccSvc->storeXMLString("/Properties/TestFunction",tp1.toXml("TestFunction"),Gaudi::Time(30), Gaudi::Time::max());
    
    // Let's try to put an XML string with self referencing
    std::string test_self_referenced_xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<!DOCTYPE DDDB SYSTEM \"structure.dtd\">"
      "<DDDB>"
      "<catalog name=\"TestFolder\">"
      "<catalog name=\"TestSubFolder\">"
      "<conditionref href=\"#TestCondition\"/>"
      "</catalog>"
      "</catalog>"
      "<condition name=\"TestCondition\">"
      "<paramVector name=\"ChannelMap\" type=\"int\">1 2 3 4</paramVector>"
      "</condition>"
      "</DDDB>";
    
    m_dbAccSvc->storeXMLString("/TestFolder",test_self_referenced_xml,Gaudi::Time(0), Gaudi::Time::max());

    debug() << "Tagging everything (FORFUN)" << endmsg;
    m_dbAccSvc->tagFolder("/","FORFUN");
    
  } catch (cool::Exception &e) {

    error() << "Error in storing sample data into the CondDB" << endmsg;
    error() << "*** COOL exception caught: " << endmsg;
    error() << "***   error message: " << e.what() << endmsg;
    return StatusCode::FAILURE;

  }
  info() << "============= condDBStoreSampleData() ending =======================" << endmsg;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Dumpt the content of the database
//=========================================================================
StatusCode PopulateDB::i_condDBDumpSampleData() {

  info() << "============= condDBDumpSampleData() starting ======================" << endmsg;

  cool::IDatabasePtr &db = m_dbAccSvc->database();

  std::vector<std::string> tags; tags.reserve(5);
  tags.push_back("COLD");
  tags.push_back("HOT");
  tags.push_back("PRODUCTION");
  tags.push_back("HEAD");
  tags.push_back("FORFUN");

  // List all stored Folders 
  std::vector<std::string> fldr_names = db->listAllNodes();
  info() << " --> List of CondDB Folders" << endmsg;
  for ( std::vector<std::string>::iterator fldr_name = fldr_names.begin();
        fldr_name != fldr_names.end(); fldr_name++ ){
    bool isFolderSet = db->existsFolderSet(*fldr_name);
    if (isFolderSet) {
      info() << " S "; // Flag FolderSets
    } else {
      info() << "   ";
    }
    info() << *fldr_name << "  " <<  endmsg;
    if ( !isFolderSet ) { // dump the content of the folder
      StatusCode status;
      for (std::vector<std::string>::const_iterator tag = tags.begin(); tag != tags.end(); ++tag ){
        status = i_dumpFolder( *fldr_name, *tag );
        if ( !status.isSuccess() ) return status;
      }
    }
  }

  info() << "============= condDBDumpSampleData() ending ========================" << endmsg;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Prepare the xml representation of a Condition containing only a temperature
//=========================================================================
std::string PopulateDB::i_encodeXmlTemperature( const double temperature,
                                                const std::string& objName ) {
  verbose() << "Encoding XML string for name=" << objName << " and temperature=" << temperature << endmsg;  

  Condition temp;
  temp.addParam("Temperature",temperature);
  std::string xmlString = temp.toXml(objName);

  verbose() << "Encoded XML string is:" << xmlString << endmsg;
  return xmlString;
}

//=========================================================================
//  Dump the contents of a CondDBFolder
//=========================================================================
StatusCode PopulateDB::i_dumpFolder( const std::string& folderName,
                                     const std::string& tagName ) {
  
  cool::IFolderPtr folder = m_dbAccSvc->database()->getFolder(folderName);

  if ( tagName != "" && tagName != "HEAD" ) {
    // with ON-LINE folders only HEAD tag makes sense
    if ( folder->versioningMode() == cool::FolderVersioning::SINGLE_VERSION )
      return StatusCode::SUCCESS;
    info() << "    | using tag =\"" << tagName << "\"" << endmsg;
  } else {
    info() << "    | using tag =\"HEAD\"" << endmsg;
  }
  

  //  std::vector<cool::IObjectPtr> objs = folder->browseObjectsInTag(tagName);
  //  cool::IObjectIteratorPtr objs = folder->browseObjectsInTag(tagName);
  cool::IObjectIteratorPtr objs;
  if ( folder->versioningMode() != cool::FolderVersioning::SINGLE_VERSION ) {
    if ( tagName != "" && tagName != "HEAD" ) {
      objs = folder->browseObjects(cool::ValidityKeyMin,cool::ValidityKeyMax,
                                   0,folder->fullPath()+"-"+tagName);
    } else {
      objs = folder->browseObjects(cool::ValidityKeyMin,cool::ValidityKeyMax,0,tagName);
    }
  } else
    objs = folder->browseObjects(cool::ValidityKeyMin,cool::ValidityKeyMax,0);
  if ( objs->isEmpty() ) {
    info() << "    | no objects in folder" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  info() << "    | " << objs->size() << " objects in folder" << endmsg;

  objs->goToStart();
  //  info() << "    | " << objs->size() << " objects in folder" << endmsg;

  while (objs->hasNext()){
    cool::IObjectPtr objp = objs->next();
    
    info() << "    | " << objp->since() << " -> " << objp->until() << endmsg;
    info() << "      size = " << objp->payload().size() << endmsg;

    std::ostringstream pl_data;
    objp->payload().toOutputStream(pl_data);
    info() << "      " << pl_data.str() << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Prepare the xml representation of a Contdition containing only a ParamVector
//=========================================================================
std::string PopulateDB::i_encodeXmlParamVector( const double pos[3],
                                                const std::string& objName,
                                                const std::string& parName ) {
  
  verbose()
    << "Encoding XML string for name=" << objName 
    << " and position=" << pos[0] << " " << pos[1] << " " << pos[2] << endmsg;
  Condition posCond;
  std::vector<double> p;
  p.push_back(pos[0]);
  p.push_back(pos[1]);
  p.push_back(pos[2]);
  posCond.addParam(parName,p);
  std::string xmlString = posCond.toXml(objName);
  
  verbose() << "Encoded XML string is:" << std::endl
            << xmlString << endmsg;
  return xmlString;
}

//=============================================================================
