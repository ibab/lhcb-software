// Include files 

 // from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/ToolFactory.h"

// local
#include "GenFSRLog.h"

// from Event
#include "Event/GenFSR.h"
#include "Event/GenCountersFSR.h"
#include "Event/CrossSectionsFSR.h"
#include "Event/GenHeader.h"

// to write a file                
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>

//-----------------------------------------------------------------------------
// Implementation file for class : GenFSRLog
//
// 2015-06-23 : Davide Fazzini
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GenFSRLog )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GenFSRLog::GenFSRLog( const std::string& name,
                          ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator ),
  m_fileRecordSvc(NULL),
  m_navigatorTool(NULL)
{
  // expect the data to be written at LHCb::GenFSRLocation::Default
  declareProperty( "FileRecordLocation" , m_fileRecordName = "/FileRecords"        );  
  declareProperty( "FSRName"            , m_FSRName        = "/GenFSR"             );
  declareProperty( "FileName"           , m_xmlFileName    = "GeneratorLogFSR.xml" );
}
//=============================================================================
// Destructor
//=============================================================================
GenFSRLog::~GenFSRLog() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GenFSRLog::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  // get the File Records service              
  m_fileRecordSvc = svc<IDataProviderSvc>("FileRecordDataSvc", true);
  
  // prepare navigator tool  
  m_navigatorTool = tool<IFSRNavigator>("FSRNavigator", "FSRNavigator");  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GenFSRLog::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GenFSRLog::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;

  GenFSRLog::printXmlFSR();

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

//=============================================================================      
//  Print the GenFSR in a file xml
//=============================================================================      

void GenFSRLog::printXmlFSR()
{
  if (msgLevel(MSG::DEBUG)) debug() << "write to file: " << m_xmlFileName << endmsg;

  // root of store                                                                                                                            
  std::string fileRecordRoot = m_fileRecordName;
  // make an inventory of the FileRecord store                                                                                                
  std::vector< std::string > addresses = m_navigatorTool->navigate(fileRecordRoot, m_FSRName);
  
  int nFSRs = 0, evtType = 0;  
  std::string genName = "", method = "";
  
  for(std::vector< std::string >::iterator iAddr=addresses.begin(); iAddr!=addresses.end(); iAddr++)
    nFSRs += 1;
    
  if(nFSRs != 0)
  {
    m_xmlFile.open(m_xmlFileName.c_str());
  
    if (m_xmlFile.is_open()) 
    {    
      if (msgLevel(MSG::DEBUG)) debug() << " Xml file: " << m_xmlFileName << " - opened" << endmsg;
      
      m_xmlFile << "<?xml version=\"1.0\"?>" << std::endl
                << "<generatorCounters>" << std::endl
                << "  <version>" << "1.1" << "</version>" << std::endl;
      
      for(std::vector< std::string >::iterator iAddr=addresses.begin(); iAddr!=addresses.end(); iAddr++)
      {
        std::string genRecordAddress = *iAddr;
        
        // read GenFSR                                                               
        LHCb::GenFSR* genFSR = getIfExists<LHCb::GenFSR>(m_fileRecordSvc, genRecordAddress);
        
        if (genFSR==NULL) 
        {
          Warning("A genFSR record was not found").ignore();
          
          if (msgLevel(MSG::DEBUG)) debug() << genRecordAddress << " not found" << endmsg;
        }
        else 
        {
          evtType = genFSR->getSimulationInfo("evtType", 0);
          genName = genFSR->getSimulationInfo("hardGenerator", "");
          method = genFSR->getSimulationInfo("generationMethod", "");
        
          m_xmlFile << "  <eventType> " << evtType << " </eventType>" << std::endl;
          
          // Add generator level counters 
          GenFSRLog::addGenCounters(genFSR);
          // Add generator level cut efficiencies
          GenFSRLog::addCutEfficiencies(genFSR);
          // Add generator level fractions
          GenFSRLog::addGenFractions(genFSR);
          // Add generator level cross-sections  
          GenFSRLog::addGenCrossSections(genFSR);

          m_xmlFile << "  <method> " << method << " </method>" << std::endl;
          m_xmlFile << "  <generator> " << genName << " </generator>" << std::endl;          

          // Add pythia cross-sections
          GenFSRLog::addGeneratorCrossSections(genFSR);
        }
      }
      
      m_xmlFile << "</generatorCounters>" << std::endl;
      m_xmlFile.close(); 
    }
    else
    if (msgLevel(MSG::DEBUG)) debug() << "The file was not opened correctly" << endmsg;
  }
  else
    if (msgLevel(MSG::DEBUG)) debug() << "There are not FSRs to write" << endmsg;
  
}
  
  
//=============================================================================     
//  Add generator level counters in the xml file
//=============================================================================    

void GenFSRLog::addGenCounters(LHCb::GenFSR* genFSR)
{
  
  LHCb::GenFSR::GeneratorCounter::iterator icounter;
  LHCb::GenFSR::GeneratorCounter genCounters = genFSR->getGenCounters();

  int key = 0;
  std::string name = "", fullName = "";

  LHCb::GenCountersFSR counterFSR;  
  std::map<std::string,std::string> mapCounter = counterFSR.getFullNames();

  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {
    key = icounter->first;
    
    if(key > 24) continue; 
    if(key == 6 || key == 7) continue;

    name = LHCb::GenCountersFSR::CounterKeyToString(key);
    fullName = mapCounter[name];
    longlong value = (icounter->second).second;
    
    m_xmlFile << "  <counter name = \"" << fullName << "\">" << std::endl
              << "    <value> " << value << " </value>" << std::endl
              << "  </counter>" << std::endl;
  }  
}


//=============================================================================   
//  Add generator level cut efficiencies in the xml file  
//=============================================================================    

void GenFSRLog::addCutEfficiencies(LHCb::GenFSR* genFSR)
{
  LHCb::GenFSR::GeneratorCounter::iterator icounter;
  LHCb::GenFSR::GeneratorCounter genCounters = genFSR->getGenCounters();

  int key = 0;  
  std::string name = "", fullName = "";

  LHCb::GenCountersFSR crossFSR;
  std::map<std::string,std::string> mapCross = crossFSR.getFullNames();  

  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {
    key = icounter->first;

    if(key == 28)
    {
      name = LHCb::GenCountersFSR::CounterKeyToString(key);      
      fullName = mapCross[name];
      longlong value = (icounter->second).second;

      m_xmlFile << "  <counter name = \"" << fullName << "\">" << std::endl
                << "    <value> " << value << " </value>" << std::endl
                << "  </counter>" << std::endl;
    }
    
    if(key !=  7 && key != 27 && key != 94 && key != 96) continue;

    name = LHCb::GenCountersFSR::CounterKeyToString(key);
    fullName = mapCross[name];
    
    longlong after = 0, before = 0;    
    double fraction = 0, error = 0;

    before = genFSR->getDenominator(key);

    if(before == 0) continue;

    after    = icounter->second.second;    
    fraction = genFSR->getEfficiency(after, before);
    error    = genFSR->getEfficiencyError(after, before);

    m_xmlFile << "  <efficiency name = \"" << fullName << "\">" << std::endl
              << "    <after> " << after << " </after>" << std::endl
              << "    <before> " << before << " </before>" << std::endl
              << "    <value> " << fraction << " </value>" << std::endl
              << "    <error> " << error << " </error>" << std::endl
              << "  </efficiency>" << std::endl;    
  }
}


//=============================================================================       
//  Add generator level fractions in the xml file       
//=============================================================================     

void GenFSRLog::addGenFractions(LHCb::GenFSR* genFSR)
{
  
  LHCb::GenFSR::GeneratorCounter::iterator icounter;  
  LHCb::GenFSR::GeneratorCounter genCounters = genFSR->getGenCounters();

  int key = 0;  
  std::string name = "", fullName = "";
  
  LHCb::GenCountersFSR crossFSR;
  std::map<std::string,std::string> mapCross = crossFSR.getFullNames();

  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {
    key = icounter->first;

    if(key < 30) continue; 
    if(key > 99) continue;

    name = LHCb::GenCountersFSR::CounterKeyToString(key);
    fullName = mapCross[name];
    
    longlong after = 0, before = 0;    
    double fraction = 0, error = 0;

    before = genFSR->getDenominator(key);

    if(before == 0) continue;

    after    = icounter->second.second;    
    fraction = genFSR->getEfficiency(after, before);
    error    = genFSR->getEfficiencyError(after, before);

    m_xmlFile << "  <fraction name = \"" << fullName << "\">" << std::endl
              << "    <number> " << after << " </number>" << std::endl
              << "    <value> " << fraction << " </value>" << std::endl
              << "    <error> " << error << " </error>" << std::endl
              << "  </fraction>" << std::endl;
  }
}


//============================================================================= 
//  Add generator level cross-sections in the xml file  
//============================================================================= 

void GenFSRLog::addGenCrossSections(LHCb::GenFSR* genFSR)
{

  LHCb::GenFSR::GeneratorCounter::iterator icounter; 
  LHCb::GenFSR::GeneratorCounter genCounters = genFSR->getGenCounters();

  int key = 0;  
  std::string name = "", fullName = "";

  LHCb::GenCountersFSR counterFSR;
  std::map<std::string,std::string> mapCounter = counterFSR.getFullNames();

  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {
    key = icounter->first;

    if(key > 24) continue;    
    if(key == 6 || key == 7) continue;

    name = LHCb::GenCountersFSR::CounterKeyToString(key);
    fullName = mapCounter[name];
    
    longlong after = 0, before = 0;
    double fraction = 0, error = 0;
    bool flag = true;
    double C = 1;

    before = genFSR->getDenominator(key);

    if(before == 0) continue;

    if(key >= 10)
    {      
      int keycs = LHCb::CrossSectionsFSR::CrossSectionKeyToType("MBCrossSection"); 
      C = genFSR->getCrossSectionInfo(keycs).second;
    }

    if(key == 2) after = genFSR->getGenCounterInfo(key+1).second;
    else after = icounter->second.second;
    if(after == 0) continue;
    fraction = genFSR->getEfficiency(after, before, C);
    
    name =  LHCb::CrossSectionsFSR::CrossSectionKeyToString(key);
    if(name == "MeanNoZeroPUInt" || name == "MeanPUInt" || name == "MeanPUIntAcc") flag = false;
    else flag = true;

    error = genFSR->getEfficiencyError(after, before, C, flag);

    m_xmlFile << "  <crosssection name = \"" << fullName << "\">" << std::endl
              << "    <generated> " <<  after  << " </generated>" << std::endl
              << "    <value> " << fraction << " </value>" << std::endl
              << "    <error> " << error << " </error>" << std::endl
              << "  </crosssection>" << std::endl;    
  }  
}


//=============================================================================
//  Add pythia cross-sections in the xml file 
//=============================================================================

void GenFSRLog::addGeneratorCrossSections(LHCb::GenFSR* genFSR)
{

  LHCb::GenFSR::CrossSection::iterator icross; 
  LHCb::GenFSR::CrossSection crossSections = genFSR->getCrossSections();

  int key = 0;  
  std::string name = "", line = "";

  for(icross = crossSections.begin(); icross != crossSections.end(); icross++)
  {    
    key = icross->first;
    name = icross->second.first;
    
    longlong number = genFSR->getGenCounterInfo(key+100).second;    
    double value = icross->second.second;
    
    m_xmlFile << "  <crosssection id = \"" << key << "\">" << std::endl
              << "    <description> \"" << name << "\" </description>" << std::endl
              << "    <generated> " <<  number  << " </generated>" << std::endl
              << "    <value> " <<  value << " </value>" << std::endl
              << "  </crosssection>" << std::endl;
  }
}
  


