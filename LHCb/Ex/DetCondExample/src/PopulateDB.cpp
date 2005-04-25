//$Id: PopulateDB.cpp,v 1.14 2005-04-25 11:54:09 marcocle Exp $
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "PopulateDB.h"

#include "DetDesc/Condition.h"

#include "DetCond/ICondDBAccessSvc.h"

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

//----------------------------------------------------------------------------

/// Constructor
PopulateDB::PopulateDB( const std::string&  name, 
			ISvcLocator*        pSvcLocator )
  : GaudiAlgorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------------

/// Initialization of the algorithm. 
/// It creates the environment needed for processing the events. 
/// For this class this is the main method, where we store data in the CondDB.

StatusCode PopulateDB::initialize() {

  info() << "Initialize()" << endreq;

  StatusCode sc;
  
  // Locate the Database Access Service
  sc = serviceLocator()->service("CondDBAccessSvc",m_dbAccSvc);
  if (  !sc.isSuccess() ) {
    error() << "Could not locate CondDBAccessSvc" << endreq;
    return sc;
  }

  // Store sample data if the database is empty
  info() << "Store sample data in the database if empty" << endreq;
  sc = i_condDBStoreSampleData( );
  if ( !sc.isSuccess() ) return sc;

  // Dump the database to make sure it is not empty
  info() << "Dump sample data from the database" << endreq;
  sc = i_condDBDumpSampleData( );
  if ( !sc.isSuccess() ) return sc;

  info() << "Initialization completed" << endreq;
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

  info() << "============= condDBStoreSampleData() starting =====================" << endreq;

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
    info() << "Signature folderSet already exists: no need to store sample data" << endreq; 
    info() << "============= condDBStoreSampleData() ending =======================" << endreq;
    return StatusCode::SUCCESS;
  } else {      
    info() << "Signature folderSet does not exist: create it" << endreq;
    try {
      m_dbAccSvc->createFolder(signature_folderset,
                               "Dummy folderset used to sign the prepared database",ICondDBAccessSvc::FOLDERSET);
    } catch (cool::Exception &e) {
      error() << e << endreq;
      return StatusCode::FAILURE;
    }
    info() << "Signature folderSet did not exist and was succesfully created" << endreq;
  }
  
  // Store sample data
  info() << "Now store sample data into the CondDB" << endreq;

  try {
    
    StatusCode sc;

    // Create new CondDBFolderSets

    typedef std::pair<std::string,std::string> str_pair;
    typedef std::vector<str_pair> vec_str_pair;
    vec_str_pair foldersets;
    foldersets.reserve(9);
    foldersets.push_back(str_pair("/SlowControl",     "Main SlowControl folderSet"));
    foldersets.push_back(str_pair("/SlowControl/LHCb","SlowControl folderSet for the LHCb detector"));
    foldersets.push_back(str_pair("/SlowControl/Hcal","SlowControl folderSet for the Hcal detector"));
    foldersets.push_back(str_pair("/ReadOut",         "Main ReadOut folderSet"));
    foldersets.push_back(str_pair("/Geometry",        "Main Geometry folderSet"));
    foldersets.push_back(str_pair("/Geometry2",       "Test Geometry folderSet"));
    foldersets.push_back(str_pair("/Alignment",       "Main Alignment folderSet"));
    foldersets.push_back(str_pair("/Alignment/Ecal",  "Ecal Alignment folderSet"));
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
    xmlfolders.reserve(8);
    xmlfolders.push_back(str_pair("/SlowControl/LHCb/scLHCb",""));
    xmlfolders.push_back(str_pair("/SlowControl/Hcal/scHcal",""));
    xmlfolders.push_back(str_pair("/Geometry/LHCb",""));
    xmlfolders.push_back(str_pair("/Geometry2/LHCb",""));
    xmlfolders.push_back(str_pair("/Geometry2/lvLHCb",""));
    xmlfolders.push_back(str_pair("/Alignment/Ecal/alEcal",""));
    xmlfolders.push_back(str_pair("/SlowControl/DummyDE",""));
    xmlfolders.push_back(str_pair("/ReadOut/DummyDE",""));
    
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
    debug() << "Create COLD objects in /SlowControl/LHCb/scLHCb and /SlowControl/Hcal/scHcal" << endreq;
    int i;
    for ( i=0; i<3; i++ ) {
      std::string s;

      // LHCb
      i_encodeXmlTemperature( (double)i*10+5, "scLHCb", s);
      m_dbAccSvc->storeXMLString("/SlowControl/LHCb/scLHCb",s,TimePoint(i*16), TimePoint((i+1)*16));
      // Hcal
      i_encodeXmlTemperature( (double)i*10.4+4.2, "scHcal", s);
      m_dbAccSvc->storeXMLString("/SlowControl/Hcal/scHcal",s,TimePoint(i*16), TimePoint((i+1)*16));
    }

    debug() << "Apply tag COLD" << endmsg;
    m_dbAccSvc->tagFolder("/SlowControl","COLD");
    debug() << "Apply tag PRODUCTION to /SlowControl/LHCb/scLHCb" << endmsg; 
    m_dbAccSvc->tagFolder("/SlowControl/LHCb/scLHCb","PRODUCTION");

    // Create new HOT temperature CondDBObjects
    debug() << "Create HOT objects in /SlowControl/LHCb/scLHCb"
            << " and /SlowControl/Hcal/scHcal" << endreq;
    for ( i=0; i<3; i++ ) {
      std::string s;

      // LHCb
      i_encodeXmlTemperature( (double)i*10+105, "scLHCb", s);
      m_dbAccSvc->storeXMLString("/SlowControl/LHCb/scLHCb",s,TimePoint(i*16), TimePoint((i+1)*16));
      // Hcal
      i_encodeXmlTemperature( (double)i*10+105, "scHcal", s);
      m_dbAccSvc->storeXMLString("/SlowControl/Hcal/scHcal",s,TimePoint(i*16), TimePoint((i+1)*16));
    }

    debug() << "Apply tag HOT" << endmsg;
    m_dbAccSvc->tagFolder("/SlowControl","HOT");
    debug() << "Apply tag PRODUCTION to /SlowControl/Hcal" << endmsg; 
    m_dbAccSvc->tagFolder("/SlowControl/Hcal","PRODUCTION");

    // Create new geometry CondDBObjects
    debug() << "Create objects in folder /Geometry/LHCb" << endreq;
    {
      // Copy XML data from a file to a string
      char* fileName;      
      fileName = "../XMLDDDB/LHCb/geometry.xml";
      debug() << "Reading XML data from file: " << endreq;
      debug() << fileName << endreq;
      std::ifstream inputFile ( fileName );
      std::string xmlString = ""; 
      char ch;
      while ( inputFile.get(ch) ) {
        xmlString = xmlString + ch;
      }     
      if( xmlString == "" ) {
        error() << "File is empty" << endreq;
        return StatusCode::FAILURE;
      }
      // Change the DTD relative path location to "file.dtd"
      // This can be correctly interpreted by the XmlCnvSvc
      unsigned int dtdPos = xmlString.find( ".dtd" );
      if( dtdPos < xmlString.length() ) {
        verbose() << "Remove DTD relative path in the XML" << endreq;
        unsigned int slashPos = xmlString.substr(0,dtdPos).rfind("/");
        if( slashPos < dtdPos ) {
          unsigned int quotePos;
          if( xmlString[dtdPos+4] == '\'' ) {
            quotePos = xmlString.substr(0,dtdPos).rfind("\'");
            verbose() << "DTD literal was: " 
                << xmlString.substr(quotePos,dtdPos+5-quotePos) << endreq;
            if( quotePos < slashPos ) 
              xmlString.replace( quotePos+1, slashPos-quotePos, "" );
            verbose() << "DTD literal is now: " 
                << xmlString.substr(quotePos,dtdPos+5-slashPos) << endreq;
          } else if( xmlString[dtdPos+4] == '\"' ) {
            quotePos = xmlString.substr(0,dtdPos).rfind("\"");
            verbose() << "DTD literal was: " 
                << xmlString.substr(quotePos,dtdPos+5-quotePos) << endreq;
            if( quotePos < slashPos ) 
              xmlString.replace( quotePos+1, slashPos-quotePos, "" );
            verbose() << "DTD literal is now: " 
                << xmlString.substr(quotePos,dtdPos+5-slashPos) << endreq;
          } else {
            error() << "Bad DTD literal in the string to be parsed" << endreq;
            return StatusCode::FAILURE;
          }
        }
      }
      // Now store the XML string in the CondDB
      verbose() << "XML data is:" << std::endl << xmlString << endreq;
      verbose() << "Store it in the database with [-inf,+inf] validity range" << endreq;
      verbose() << "Folder name: /Geometry/LHCb" << endreq;
      verbose() << "Create object with validity range: [" << cool::ValidityKeyMin
                << "(0x" << std::hex << cool::ValidityKeyMin << std::dec << ")" 
                << "," << cool::ValidityKeyMax << "(0x" << std::hex << cool::ValidityKeyMax << std::dec << ")]" << endreq;

      m_dbAccSvc->storeXMLString("/Geometry/LHCb",xmlString,time_absolutepast,time_absolutefuture);
    }

    debug() << "Apply tag PRODUCTION to /Geometry/LHCb" << endmsg;
    m_dbAccSvc->tagFolder("/Geometry/LHCb","PRODUCTION");

    debug() << "Put in a new HEAD" << endmsg;
    for ( i=0; i<2; i++ ) {
      std::string s;
      // LHCb
      i_encodeXmlTemperature( (double)i*5+9.2, "scLHCb", s);
      m_dbAccSvc->storeXMLString("/SlowControl/LHCb/scLHCb",s,TimePoint(i*24), TimePoint((i+1)*24));
    }
    for ( i=0; i<10; i++ ) {
      std::string s;
      // Hcal
      i_encodeXmlTemperature( (double)i*5.4+9, "scHcal", s);
      m_dbAccSvc->storeXMLString("/SlowControl/Hcal/scHcal",s,TimePoint(i*10), TimePoint((i+1)*10));
    }

    /// ------------ DummyDE conditions
    for ( i=0; i<3; ++i ) {
      std::string s;
      i_encodeXmlTemperature((double)i*4.17+25.16, "DummyDE", s);
      m_dbAccSvc->storeXMLString("/SlowControl/DummyDE",s,TimePoint(i*24), TimePoint((i+1)*24));
    }
    for ( i=0; i<3; ++i ) {
      Condition temp;
      std::vector<double> temps;
      std::vector<std::string> temps_str;
      for ( int j=0; j<10; ++j ) {
        temps.push_back((double)i*4.17+20.16-(double)j*0.92);
        std::ostringstream t ;
        t << (double)i*4.17+20.16-(double)j*0.92;
        temps_str.push_back(t.str());
      }
      temp.addParamVector("CrateTemps","double","",temps_str,temps);
      temp.addParam("NChannels","int","","10",10,10);
      std::vector<int> chnls;
      std::vector<double> chnls_dbl;
      std::vector<std::string> chnls_str;
      for ( int j=0; j<10; j += 1+i ) {
        chnls.push_back(j);
        chnls_dbl.push_back(j);
        std::ostringstream t ;
        t << j;
        chnls_str.push_back(t.str());
      }
      temp.addParamVector("Channels","int","",chnls_str,chnls_dbl,chnls);
      std::string s = i_conditionToXml("DummyDE",temp);
      verbose() << "/ReadOut/DummyDE: " << s << endmsg;
      m_dbAccSvc->storeXMLString("/ReadOut/DummyDE",s,TimePoint(i*24), TimePoint((i+1)*24));
    }

    /// test alignment (TODO: remove)
    double ecalpos[3][3] = { { 0,0,0 }, { 0,1,0 }, { 0,0,2 } };    
    double fallbackpos[3] = {-1.,-1.,-1.};
    
    //    for ( i=-1; i<3; i++ ) {
    for ( i=-1; i<3; i++ ) {
      std::string s;
      // alEcal
      if (i>=0){
        i_encodeXmlParamVector( ecalpos[i%3], "alEcal","Ecal position", s);
        m_dbAccSvc->storeXMLString("/Alignment/Ecal/alEcal",s,time_absolutepast,time_absolutefuture);
      } else {
        i_encodeXmlParamVector( fallbackpos, "alEcal","Ecal position", s);
        m_dbAccSvc->storeXMLString("/Alignment/Ecal/alEcal",s,time_absolutepast,time_absolutefuture);
      }
    }

    std::string s;

    s = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE DDDB SYSTEM \"geometry.dtd\">";
    s += "<DDDB><catalog name=\"LHCb\"><logvolref href=\"conddb:";
    s += "/Geometry2/lvLHCb#lvLHCb\"/></catalog></DDDB>";
    
    m_dbAccSvc->storeXMLString("/Geometry2/LHCb",s,time_absolutepast,time_absolutefuture);

    s = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE DDDB SYSTEM \"geometry.dtd\">";
    s += "<DDDB><logvol material=\"Vacuum\" name=\"lvLHCb\">";
    s += "<box name=\"caveBox\" sizeX=\"50000*mm\" sizeY=\"50000*mm\" sizeZ=\"50000*mm\"/>";
    s += "<physvol logvol=\"/dd/Geometry/Ecal/lvEcal\" name=\"EcalSubsystem\"><posXYZ x=\"0*mm\" y=\"0*mm\" z=\"12907.5*mm\"/>";
    s += "</physvol><physvol logvol=\"/dd/Geometry/Hcal/lvHcal\" name=\"HcalSubsystem\">";
    s += "<posXYZ x=\"0*mm\" y=\"0*mm\" z=\"14162.5*mm\"/></physvol></logvol></DDDB>";

    m_dbAccSvc->storeXMLString("/Geometry2/lvLHCb",s,time_absolutepast,time_absolutefuture);
    
    debug() << "Tagging /Geometry2" << endmsg;
    m_dbAccSvc->tagFolder("/Geometry2","COLD");
    m_dbAccSvc->tagFolder("/Geometry2","HOT");
    m_dbAccSvc->tagFolder("/Geometry2","PRODUCTION");

    debug() << "Tagging everything (FORFUN)" << endmsg;
    m_dbAccSvc->tagFolder("/","FORFUN");
    
  } catch (cool::Exception &e) {

    error() << "Error in storing sample data into the CondDB" << endreq;
    error() << "*** COOL exception caught: " << endreq;
    error() << "***   error message: " << e << endreq;
    return StatusCode::FAILURE;

  }
  info() << "============= condDBStoreSampleData() ending =======================" << endreq;
  return StatusCode::SUCCESS; 

}

//----------------------------------------------------------------------------

StatusCode PopulateDB::i_condDBDumpSampleData ( ) {
  MsgStream log(msgSvc(), name());
  info() << "============= condDBDumpSampleData() starting ======================" << endreq;

  cool::IDatabasePtr &db = m_dbAccSvc->database();

  std::vector<std::string> tags; tags.reserve(5);
  tags.push_back("COLD");
  tags.push_back("HOT");
  tags.push_back("PRODUCTION");
  tags.push_back("HEAD");
  tags.push_back("FORFUN");

  // List all stored Folders 
  std::vector<std::string> fldr_names = db->listFolders();
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

  info() << "============= condDBDumpSampleData() ending ========================" << endreq;

  return StatusCode::SUCCESS;  

}

//----------------------------------------------------------------------------

std::string PopulateDB::i_conditionToXml( const std::string &name, Condition &cond ) const {
  std::ostringstream xml;
  // XML header
  xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE DDDB SYSTEM \"structure.dtd\">\n";
  // DDDB open
  xml << "<DDDB>";
  // condition open
  xml << "<condition name=\"" << name << "\">";

  std::vector<std::string> pars;
  std::vector<std::string>::const_iterator i;
  // loop over parameters
  pars = cond.params();
  for ( i = pars.begin(); i != pars.end(); ++i ){
  	xml << "<param name=\"" << *i << "\" type=\"";
    switch (cond.paramKind(*i)){
    case IParamSet::DOUBLE : xml << "double"; break;
    case IParamSet::INT    : xml << "int"; break;
    default                : xml << "other";
    }
    xml << "\">" << cond.paramAsString(*i) << "</param>";
  }
  // loop over vector parameters
  pars = cond.paramVectors();
  for ( i = pars.begin(); i != pars.end(); ++i ){
  	xml << "<paramVector name=\"" << *i << "\" type=\"";
    switch (cond.paramVectorKind(*i)){
    case IParamSet::DOUBLE : xml << "double"; break;
    case IParamSet::INT    : xml << "int"; break;
    default                : xml << "other";
    }
    xml << "\">";
    std::vector<std::string> p_val = cond.paramVectorAsString(*i);
    for ( std::vector<std::string>::const_iterator s = p_val.begin(); s != p_val.end() ; ++s ){
      xml << " " << *s;
    }
    xml << "</paramVector>";
  }

  // condition close
  xml << "</condition>";
  // DDDB close
  xml << "</DDDB>\n";
  
  return xml.str();
}

void PopulateDB::i_encodeXmlTemperature( const double temperature,
					 const std::string& objName,
					 std::string& xmlString   ) {
  MsgStream log(msgSvc(), name());
  verbose() << "Encoding XML string for name=" << objName << " and temperature=" << temperature << endmsg;  

  Condition temp;
  std::ostringstream tmp;
  tmp << temperature;
  temp.addParam("Temperature", "double", "", tmp.str(), temperature);
  xmlString = i_conditionToXml(objName,temp);

  verbose() << "Encoded XML string is:" << xmlString << endmsg;
}

//----------------------------------------------------------------------------

/// Dump the contents of a CondDBFolder
StatusCode PopulateDB::i_dumpFolder( const std::string& folderName,
				     const std::string& tagName ) 
{
  MsgStream log(msgSvc(), name());

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
    
    for (pool::AttributeList::const_iterator it = objp->payload().begin();
         it!=objp->payload().end(); it++){
      info() << "      " << it->spec().name()  << " (" <<  it->spec().type_name() << ") =" << endmsg;
      info() << "        " << it->getValueAsString() << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}
  
//----------------------------------------------------------------------------

void PopulateDB::i_encodeXmlParamVector( const double pos[3],
                                           const std::string& objName,
                                           const std::string& parName,
                                           std::string& xmlString   ) {
  MsgStream log(msgSvc(), name());
  verbose()
      << "Encoding XML string for name=" << objName 
      << " and position=" << pos[0] << " " << pos[1] << " " << pos[2] << endreq;
  Condition posCond;
  std::vector<double> p;
  std::vector<std::string> ps;
  std::ostringstream t0,t1,t2;
  p.push_back(pos[0]); t0 << pos[0]; ps.push_back(t0.str());
  p.push_back(pos[1]); t1 << pos[1]; ps.push_back(t1.str());
  p.push_back(pos[2]); t2 << pos[2]; ps.push_back(t2.str());
  posCond.addParamVector(parName,"double","",ps,p);
  xmlString = i_conditionToXml(objName,posCond);
  
  verbose() << "Encoded XML string is:" << std::endl
      << xmlString << endreq;
  return;
};


StatusCode PopulateDB::i_tagDB(const std::string& tagName){
  cool::IDatabasePtr &db = m_dbAccSvc->database();
  
  std::vector<std::string> fldr_names = db->listFolders();
  std::vector<std::string>::const_iterator fn;
  for ( fn = fldr_names.begin() ; fn != fldr_names.end() ; ++fn ){
  	if (db->existsFolder(*fn)){
  	  cool::IFolderPtr folder = db->getFolder(*fn);
  	  folder->tag(folder->fullPath()+"-"+tagName,"");
  	}
  }
  return StatusCode::SUCCESS;
}
