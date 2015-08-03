// Include files
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

// from Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Time.h"

#include "GaudiKernel/SystemOfUnits.h"

#include "DetDesc/Condition.h"
#include "DetDesc/AlignmentCondition.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetCond/ICondDBEditor.h"
#include "DetCond/ICondDBAccessSvc.h"

// from COOL
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/types.h"
#include "CoolKernel/Record.h"

// local
#include "PopulateDB.h"

using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : PopulateDB
//
// 2005-06-22 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PopulateDB )

static std::string fix_dtd(std::string orig)
{
  std::string::size_type pos = orig.find("structure.dtd");
  if ( pos == std::string::npos ) {
    pos = orig.find("geometry.dtd");
    if ( pos == std::string::npos ) return orig;
  }
  return orig.substr(0,pos) + "conddb:/DTD/" + orig.substr(pos);
}


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PopulateDB::PopulateDB( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_dbEditor(NULL)
  , m_dbAccSvc(NULL)
{

}
//=============================================================================
// Destructor
//=============================================================================
PopulateDB::~PopulateDB() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PopulateDB::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  try {

    // Locate the Database Access Service
    m_dbEditor = svc<ICondDBEditor>("CondDBAccessSvc",true);
    sc = m_dbEditor->queryInterface(ICondDBAccessSvc::interfaceID(),(void**)&m_dbAccSvc);
    if ( !sc.isSuccess() ) {
      error() << "Cannot get ICondDBAccessSvc interface from CondDBAccessSvc" << endmsg;
      return sc;
    }
    m_dbAccSvc->release(); // is already referenced as ICondDBEditor

    // Store sample data if the database is empty
    info() << "Store sample data in the database if empty" << endmsg;
    sc = i_condDBStoreSampleData();
    if ( !sc.isSuccess() ) return sc;
    
    // Dump the database to make sure it is not empty
    info() << "Dump sample data from the database" << endmsg;
    sc = i_condDBDumpSampleData();
    if ( !sc.isSuccess() ) return sc;
    
    info() << "Initialization completed" << endmsg;
    
  } catch (GaudiException &) {
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

//=========================================================================
//  Populate the database 
//=========================================================================
StatusCode PopulateDB::i_condDBStoreSampleData() {
  info() << "============= condDBStoreSampleData() starting =====================" << endmsg;

  // Get a pointer to the DB to speed up a bit
  cool::IDatabasePtr &db = m_dbAccSvc->database();

  // Check for signature (a folderset called "/_"+name())
  std::string signature_folderset = "/_" + name();
  info() << "Checking if CondDB tree has been already filled" << endmsg;
  bool signatureFolderSetExists = db->existsFolderSet(signature_folderset);

  // If signature folderSet exists already then return
  // If signature folderSet does not exist then create it and store sample data
  if (signatureFolderSetExists) {
    info() << "Signature folderSet already exists: no need to store sample data" << endmsg; 
    info() << "============= condDBStoreSampleData() ending =======================" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Store sample data
  info() << "Now store sample data into the CondDB" << endmsg;
  try {
    if ( i_createHierarchy().isSuccess() &&
         i_createCOLDVersion().isSuccess() &&
         i_createHOTVersion().isSuccess() &&
         i_createHEADVersion() )
    { // if everything went fine mark the DB as ready
      
      info() << "Creating signature folderSet" << endmsg;
      
      m_dbEditor->createNode(signature_folderset,
                             "Dummy folderset used to sign the prepared database",
                             ICondDBEditor::FOLDERSET);
      info() << "Signature folderSet created" << endmsg;

    } else {
      return StatusCode::FAILURE;  
    }

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
//  Prepare all folders and foldersets
//=========================================================================
StatusCode PopulateDB::i_createHierarchy() {

  StatusCode sc;
  
  // Create new CondDBFolderSets
  
  typedef std::pair<std::string,std::string> str_pair;
  typedef std::vector<str_pair> vec_str_pair;
  vec_str_pair foldersets;

  foldersets.push_back(str_pair("/Conditions/Online",          "On-line folderSet"));
  foldersets.push_back(str_pair("/Conditions/Online/Cave",     "Measurements for the cave"));
  
  debug() << "Create foldersets" << endmsg;
  for(vec_str_pair::iterator f = foldersets.begin(); f != foldersets.end(); ++f ){
    sc = m_dbEditor->createNode(f->first,f->second,ICondDBEditor::FOLDERSET);
    if (!sc.isSuccess()){
      error() << "Unable to create folderset \"" << f->first << '"' << endmsg;
      return sc;
    }
  }

  // Create folders
  vec_str_pair xmlfolders;

  xmlfolders.push_back(str_pair("/Conditions/LHCb/Environment/Temperature.xml",""));

  xmlfolders.push_back(str_pair("/Conditions/Hcal/EnvironmentCatalog.xml",""));
  xmlfolders.push_back(str_pair("/Conditions/Hcal/Environment/Temperature",""));

  xmlfolders.push_back(str_pair("/Conditions/DummyDE/EnvironmentCatalog.xml",""));
  xmlfolders.push_back(str_pair("/Conditions/DummyDE/Environment/Temperature",""));
  xmlfolders.push_back(str_pair("/Conditions/DummyDE/ReadoutConfCatalog.xml",""));
  xmlfolders.push_back(str_pair("/Conditions/DummyDE/ReadoutConf/Conf.xml",""));

  xmlfolders.push_back(str_pair("/Conditions/Velo/Alignment/Module",""));

  xmlfolders.push_back(str_pair("/Conditions/Tests/Properties/TestFunction.xml",""));

  xmlfolders.push_back(str_pair("/Conditions/TestFolder",""));
  
  debug() << "Create folders (multi-version)" << endmsg;
  for(vec_str_pair::iterator f = xmlfolders.begin(); f != xmlfolders.end(); ++f ){
    sc = m_dbEditor->createNode(f->first,f->second,ICondDBEditor::XML);
    if (!sc.isSuccess()){
      error() << "Unable to create XML folder \"" << f->first << '"' << endmsg;
      return sc;
    }
  }
  
  std::string xml_header =
    "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
    "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">\n"
    "<DDDB>\n";
  std::string xml_trailer = "</DDDB>\n";
  
  // Create mapping
  m_dbEditor->storeXMLData("/Conditions/MainCatalog.xml",
                           xml_header +
                           "<catalog name=\"Conditions\">\n"
                           "<catalogref href = \"#Alignment\"/>\n"
                           "<catalogref href = \"#Calibration\"/>\n"
                           "<catalogref href = \"#ChannelInfo\"/>\n"
                           "<catalogref href = \"#Environment\"/>\n"
                           "<catalogref href = \"#HardwareProperties\"/>\n"
                           "<catalogref href = \"#ReadoutConf\"/>\n"
                           "<catalogref href = \"Trigger/Catalog.xml#Trigger\"/>\n"
                           "<catalogref href = \"#Sim\"/>\n"
                           "<catalogref href = \"properties.xml#Properties\"/>\n" // <- new line
                           "<catalogref href = \"Online\"/>\n" // <- new line
                           "<catalogref href = \"TestFolder\"/>\n" // <- new line
                           "<catalog name=\"ManiFolderTest\">\n" // <- new catalog begin
                           "<catalog name=\"A\">\n"
                           "<conditionref href=\"A@ManiFolder#Object\" />\n"
                           "</catalog>\n"
                           "<catalog name=\"B\">\n"
                           "<conditionref href=\"B@ManiFolder#Object\" />\n"
                           "</catalog>\n"
                           "<catalog name=\"C\">\n"
                           "<conditionref href=\"C@ManiFolder#Object\" />\n"
                           "</catalog>\n"
                           "</catalog>\n" // <- new catalog end
                           "</catalog>\n"
                           "<catalog name=\"Alignment\">\n"
                           "<catalogref href = \"LHCb/AlignmentCatalog.xml#LHCb\"/>\n"
                           "<catalogref href = \"Velo/AlignmentCatalog.xml#Velo\"/>\n"
                           "<catalogref href = \"Ecal/AlignmentCatalog.xml#Ecal\"/>\n"
                           "<catalogref href = \"Hcal/AlignmentCatalog.xml#Hcal\"/>\n"
                           "</catalog>\n"
                           "<catalog name=\"Calibration\">\n"
                           "<catalogref href = \"Ecal/CalibrationCatalog.xml#Ecal\"/>\n"
                           "<catalogref href = \"Hcal/CalibrationCatalog.xml#Hcal\"/>\n"
                           "</catalog>\n"
                           "<catalog name=\"ChannelInfo\">\n"
                           "</catalog>\n"
                           "<catalog name=\"Environment\">\n"
                           "<catalogref href = \"LHCb/EnvironmentCatalog.xml#LHCb\"/>\n"
                           "<catalogref href = \"Hcal/EnvironmentCatalog.xml#Hcal\"/>\n" // <- new line
                           "<catalogref href = \"DummyDE/EnvironmentCatalog.xml#DummyDE\"/>\n" // <- new line
                           "</catalog>\n"
                           "<catalog name=\"HardwareProperties\">\n"
                           "</catalog>\n"
                           "<catalog name=\"ReadoutConf\">\n"
                           "<catalogref href = \"Ecal/ReadoutConfCatalog.xml#Ecal\"/>\n"
                           "<catalogref href = \"Hcal/ReadoutConfCatalog.xml#Hcal\"/>\n"
                           "<catalogref href = \"DummyDE/ReadoutConfCatalog.xml#DummyDE\"/>\n" // <- new line
                           "</catalog>\n"
                           "<catalog name=\"Sim\">\n"
                           "<condition name=\"TestCondition\">\n"
                           "<paramVector comment=\"List of layers\" name=\"layers\" "
                             "type=\"string\">TTaXMap TTaUMap TTbVMap TTbXMap</paramVector>\n"
                           "<paramVector name=\"vs\" type=\"string\">\n"
                           "str1\n"
                           "str2 str3 str4</paramVector>\n"
                           "</condition>\n"
                           "</catalog>\n"
                           + xml_trailer,
                           Gaudi::Time::epoch(), Gaudi::Time::max());
  m_dbEditor->storeXMLData("/Conditions/LHCb/EnvironmentCatalog.xml",
                           xml_header +
                           "<catalog name=\"LHCb\">\n"
                           "<conditionref href=\"Environment/Global.xml#IP8Pressure\"/>\n"
                           "<conditionref href=\"Environment/Temperature.xml#Temperature\"/>\n"
                           "</catalog>\n"
                           + xml_trailer,
                           Gaudi::Time::epoch(), Gaudi::Time::max());
  m_dbEditor->storeXMLData("/Conditions/Hcal/EnvironmentCatalog.xml",
                           xml_header +
                           "<catalog name=\"Hcal\">\n"
                           "<conditionref href=\"Environment/Temperature\"/>\n"
                           "</catalog>\n"
                           + xml_trailer,
                           Gaudi::Time::epoch(), Gaudi::Time::max());
  m_dbEditor->storeXMLData("/Conditions/DummyDE/EnvironmentCatalog.xml",
                           xml_header +
                           "<catalog name=\"DummyDE\">\n"
                           "<conditionref href=\"Environment/Temperature\"/>\n"
                           "</catalog>\n"
                           + xml_trailer,
                           Gaudi::Time::epoch(), Gaudi::Time::max());
  m_dbEditor->storeXMLData("/Conditions/DummyDE/ReadoutConfCatalog.xml",
                           xml_header +
                           "<catalog name=\"DummyDE\">\n"
                           "<conditionref href=\"ReadoutConf/Conf.xml#Configuration\"/>\n"
                           "</catalog>\n"
                           + xml_trailer,
                           Gaudi::Time::epoch(), Gaudi::Time::max());

  std::ostringstream velo_align_cat;
  velo_align_cat << xml_header
                 << "<catalog name=\"Velo\">\n"
                 << "<conditionref href=\"Alignment/Global.xml#VeloSystem\"/>\n"
                 << "<conditionref href=\"Alignment/Global.xml#VeloRight\"/>\n"
                 << "<conditionref href=\"Alignment/Global.xml#VeloLeft\"/>\n";
  
  for ( int i = 0; i < 42; ++i ) {
    velo_align_cat << "<conditionref href=\"Alignment/Module:";
    velo_align_cat.width(2);
    velo_align_cat.fill('0');
    velo_align_cat << i << "#Module";
    velo_align_cat.width(2);
    velo_align_cat.fill('0');
    velo_align_cat << i << "\"/>\n";
  }
  velo_align_cat << "<conditionref href=\"Alignment/Detectors.xml#DetectorPU00-00\"/>\n";
  velo_align_cat << "<conditionref href=\"Alignment/Detectors.xml#DetectorPU01-00\"/>\n";
  velo_align_cat << "<conditionref href=\"Alignment/Detectors.xml#DetectorPU02-00\"/>\n";
  velo_align_cat << "<conditionref href=\"Alignment/Detectors.xml#DetectorPU03-00\"/>\n";
  for ( int i = 0; i < 42; ++i ) {
    for ( int j = 0; j < 2; ++j ) {    
      velo_align_cat << "<conditionref href=\"Alignment/Detectors.xml#Detector";
      velo_align_cat.width(2);
      velo_align_cat.fill('0');
      velo_align_cat << i << '-';
      velo_align_cat.width(2);
      velo_align_cat.fill('0');
      velo_align_cat << j << "\"/>\n";
    }
  }
  velo_align_cat << "</catalog>\n" << xml_trailer;
  
  m_dbEditor->storeXMLData("/Conditions/Velo/AlignmentCatalog.xml",
                           velo_align_cat.str(),
                           Gaudi::Time::epoch(), Gaudi::Time::max());

  vec_str_pair xmlfolders_online;
  xmlfolders_online.push_back(str_pair("/Conditions/Online/Cave/T1","Temperature 1"));
  xmlfolders_online.push_back(str_pair("/Conditions/Online/Cave/T2","Temperature 2"));
  xmlfolders_online.push_back(str_pair("/Conditions/Online/Cave/P1","Pressure 1"));
  
  debug() << "Create folders (single-version)" << endmsg;
  for(vec_str_pair::iterator f = xmlfolders_online.begin(); f != xmlfolders_online.end(); ++f ){
    sc = m_dbEditor->createNode(f->first,f->second,ICondDBEditor::XML,ICondDBEditor::SINGLE);
    if (!sc.isSuccess()){
      error() << "Unable to create XML folder \"" << f->first << '"' << endmsg;
      return sc;
    }
  }

  debug() << "Create folder for many files" << endmsg;
  std::set<std::string> fields;
  fields.insert("A");
  fields.insert("B");
  fields.insert("C");
  sc = m_dbEditor->createNode("/Conditions/ManiFolder","Folders for many files",fields,ICondDBEditor::XML);

  return StatusCode::SUCCESS;
}

//=========================================================================
//  
//=========================================================================
StatusCode PopulateDB::i_createCOLDVersion ( ) {
  debug() << "Create COLD version data" << endmsg;

  // ---------- /SlowControl/LHCb/scLHCb --------------------
  // ---------- /SlowControl/Hcal/scHcal --------------------
  int i;
  for ( i=0; i<3; i++ ) {
    
    // LHCb
    m_dbEditor->storeXMLData("/Conditions/LHCb/Environment/Temperature.xml",
                             i_encodeXmlTemperature( (double)i*10+5, "Temperature" ),
                             Gaudi::Time(i*16), Gaudi::Time((i+1)*16));
    // Hcal
    m_dbEditor->storeXMLData("/Conditions/Hcal/Environment/Temperature",
                             i_encodeXmlTemperature( (double)i*10.4+4.2, "Temperature"),
                             Gaudi::Time(i*16), Gaudi::Time((i+1)*16));
  }

  // ---------- /SlowControl/DummyDE --------------------
  // ---------- /ReaadOut/DummyDE --------------------
  for ( i=0; i<3; ++i ) {
    m_dbEditor->storeXMLData("/Conditions/DummyDE/Environment/Temperature",
                             i_encodeXmlTemperature((double)i*4.17+25.16, "Temperature" ),
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
    std::string s = fix_dtd(temp.toXml("Configuration"));
    verbose() << "/Conditions/DummyDE/ReadoutConf/Conf.xml: " << s << endmsg;
    m_dbEditor->storeXMLData("/Conditions/DummyDE/ReadoutConf/Conf.xml",s,Gaudi::Time(i*24), Gaudi::Time((i+1)*24));
  }
  
  // ---------- /Alignment/Velo/Modules --------------------
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
    m_dbEditor->storeXMLData("/Conditions/Velo/Alignment/Module",
                             fix_dtd(align.toXml(obj_name.str())),
                             Gaudi::Time::epoch(), Gaudi::Time::max(), i);
  }
  
  // change placement of one of the modules
  pos[2] = 2*cm;
  AlignmentCondition align2(pos,rot);
  m_dbEditor->storeXMLData("/Conditions/Velo/Alignment/Module",
                           fix_dtd(align2.toXml("Module12")),
                           Gaudi::Time(30), Gaudi::Time(50), 12);

  std::string s;  
  
  // ---------- /Properties/TestFunction --------------------
  // ---- Add a tabulated property
  debug() << "Add a TabulatedProperty from 0 to 30" << endmsg;
  TabulatedProperty tp1;
  for(double x = 0. ; x < 2. ; x += 0.2){
    double y = x*x;
    tp1.table().push_back(TabulatedProperty::Entry(x,y));
  }
  m_dbEditor->storeXMLData("/Conditions/Tests/Properties/TestFunction.xml",
                           fix_dtd(tp1.toXml("TestFunction")),Gaudi::Time(0), Gaudi::Time(30));
  
  debug() << "Add a second TabulatedProperty from 30 to +inf" << endmsg;
  tp1.table().clear();
  for(double x = 0. ; x < 2. ; x += 0.2){
    double y = x*x*0.5;
    tp1.table().push_back(TabulatedProperty::Entry(x,y));
  }
  m_dbEditor->storeXMLData("/Conditions/Tests/Properties/TestFunction.xml",
                           fix_dtd(tp1.toXml("TestFunction")),Gaudi::Time(30), Gaudi::Time::max());

  // ---------- /TestFolder --------------------
  // Let's try to put an XML string with self referencing
  std::string test_self_referenced_xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">"
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
  
  m_dbEditor->storeXMLData("/Conditions/TestFolder",test_self_referenced_xml,Gaudi::Time(0), Gaudi::Time::max());

  // ---------- /ManiFolder --------------------
  {
    std::map<std::string,std::string> data;
    Condition tmp;
    tmp.addParam<std::string>("Datum","A");
    data["A"] = fix_dtd(tmp.toXml("Object"));
    tmp.param<std::string>("Datum") = "B";
    data["B"] = fix_dtd(tmp.toXml("Object"));
    tmp.param<std::string>("Datum") = "C";
    data["C"] = fix_dtd(tmp.toXml("Object"));
    
    m_dbEditor->storeXMLData("/Conditions/ManiFolder",data,Gaudi::Time(0), Gaudi::Time::max());
  }
  
  // ---------- /TestXmlObject --------------------
  {
    Condition tmp;
    tmp.addParam<std::string>("Datum","Hi there!");
    m_dbEditor->storeXMLData("/a_folder.xml",fix_dtd(tmp.toXml("TestXmlObject")),Gaudi::Time(0), Gaudi::Time::max());
  }

  // ---------- APPLY TAG COLD --------------------
  debug() << "Apply (global) tag \"COLD\"" << endmsg;
  return m_dbEditor->recursiveTag("/","COLD");
}

//=========================================================================
//  
//=========================================================================
StatusCode PopulateDB::i_createHOTVersion ( ) {
  debug() << "Create HOT version data" << endmsg;

  // ---------- /SlowControl/LHCb/scLHCb --------------------
  // ---------- /SlowControl/Hcal/scHcal --------------------
  int i;
  for ( i=0; i<3; i++ ) {
    
    // LHCb
    m_dbEditor->storeXMLData("/Conditions/LHCb/Environment/Temperature.xml",
                             i_encodeXmlTemperature( (double)i*10+105, "Temperature" ),
                             Gaudi::Time(i*16), Gaudi::Time((i+1)*16));
    // Hcal
    m_dbEditor->storeXMLData("/Conditions/Hcal/Environment/Temperature",
                             i_encodeXmlTemperature( (double)i*10+105, "Temperature"),
                             Gaudi::Time(i*16), Gaudi::Time((i+1)*16));
  }
  
  // ---------- APPLY TAG HOT --------------------
  debug() << "Apply (global) tag \"HOT\"" << endmsg;
  return m_dbEditor->recursiveTag("/","HOT");
}

//=========================================================================
//  
//=========================================================================
StatusCode PopulateDB::i_createHEADVersion ( ) {
  debug() << "Create new HEAD version data" << endmsg;

  // ---------- /SlowControl/LHCb/scLHCb --------------------
  // ---------- /SlowControl/Hcal/scHcal --------------------
  int i;
  debug() << "Put in a new HEAD" << endmsg;
  for ( i=0; i<2; i++ ) {
    // LHCb
    m_dbEditor->storeXMLData("/Conditions/LHCb/Environment/Temperature.xml",
                             i_encodeXmlTemperature( (double)i*5+9.2, "Temperature" ),
                             Gaudi::Time(i*24), Gaudi::Time((i+1)*24));
  }
  for ( i=0; i<10; i++ ) {
    // Hcal
    m_dbEditor->storeXMLData("/Conditions/Hcal/Environment/Temperature",
                             i_encodeXmlTemperature( (double)i*5.4+9, "Temperature" ),
                             Gaudi::Time(i*10), Gaudi::Time((i+1)*10));
  }
  
  // ---------- APPLY TAG FORFUN --------------------
  debug() << "Apply (global) tag \"FORFUN\"" << endmsg;
  return m_dbEditor->recursiveTag("/","FORFUN");
}

//=========================================================================
//  Dump the content of the database
//=========================================================================
StatusCode PopulateDB::i_condDBDumpSampleData() {

  info() << "============= condDBDumpSampleData() starting ======================" << endmsg;

  cool::IDatabasePtr &db = m_dbAccSvc->database();

  std::vector<std::string> tags; tags.reserve(5);
  tags.push_back("COLD");
  tags.push_back("HOT");
  //tags.push_back("PRODUCTION");
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
    if ( !isFolderSet && (fldr_name->find("/Conditions") == 0) ) { // dump the content of the folder
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
  std::string xmlString = fix_dtd(temp.toXml(objName));

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
                                   0,folder->resolveTag(tagName));
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

  while (objs->goToNext()){
    const cool::IObject & obj = objs->currentRef();
    
    info() << "    | " << obj.since() << " -> " << obj.until() << endmsg;
    info() << "      size = " << obj.payload().size() << endmsg;
    info() << "      " << obj.payload() << endmsg;
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
  std::string xmlString = fix_dtd(posCond.toXml(objName));
  
  verbose() << "Encoded XML string is:" << std::endl
            << xmlString << endmsg;
  return xmlString;
}

//=============================================================================
