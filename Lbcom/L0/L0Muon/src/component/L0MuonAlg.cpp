// $Id: L0MuonAlg.cpp,v 1.3 2007-08-27 10:26:32 jucogan Exp $
#include <algorithm>
#include <math.h>
#include <set>
#include <boost/dynamic_bitset.hpp>

#include "L0MuonAlg.h"

/// Gaudi interfaces
#include "GaudiKernel/IChronoStatSvc.h"

/// Gaudi factories for algorithms 
#include "GaudiKernel/AlgFactory.h"

/// Utility classes
#include "GaudiKernel/IToolSvc.h"   

// from MuonEvent
#include "Event/MuonDigit.h"

// // from DAQEvent
// #include "Event/RawEvent.h"

// Registers (to fill optical links)
#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/TileRegister.h"

// Emulator
#include "L0MuonKernel/UnitFactory.h"
#include "L0MuonKernel/L0MuonKernelFromXML.h"

DECLARE_ALGORITHM_FACTORY( L0MuonAlg );

L0MuonAlg::L0MuonAlg(const std::string& name,
                                         ISvcLocator* pSvcLocator) 
  :GaudiAlgorithm(name, pSvcLocator)
 
{
  
  m_muonBuffer = 0;

  m_configfile="L0MuonKernel.xml";

  m_foiXSize.push_back(4); // 0-> Xfoi in M1
  m_foiXSize.push_back(5); // 1-> Xfoi in M2
  m_foiXSize.push_back(0); // 2-> Xfoi in M3
  m_foiXSize.push_back(2); // 3-> Xfoi in M4
  m_foiXSize.push_back(4); // 4-> Xfoi in M5

  m_foiYSize.push_back(0); // 0-> Yfoi in M1
  m_foiYSize.push_back(0); // 1-> Yfoi in M2
  m_foiYSize.push_back(0); // 2-> Yfoi in M3
  m_foiYSize.push_back(1); // 3-> Yfoi in M4
  m_foiYSize.push_back(1); // 4-> Yfoi in M5

  m_ptParameters.push_back(40.);   // Precision on PT (MeV)
  m_ptParameters.push_back(7.);    // Number of bins in PT
  
  declareProperty("PtParameters"   , m_ptParameters); 
  declareProperty("ConfigFile"     , m_configfile);
  declareProperty("IgnoreM1"       , m_ignoreM1 = false );

  declareProperty("FoiXSize"       , m_foiXSize);
  declareProperty("FoiYSize"       , m_foiYSize);

  declareProperty("StoreInBuffer"  , m_storeFlag = true);  

  declareProperty("DebugMode"      , m_debug = false );
  declareProperty("BankVersion"    , m_bankVersion = 0 );
  declareProperty("ProcVersion"    , m_procVersion = 0 );
  declareProperty("OutputMode"     , m_outputMode = 0 );
}


StatusCode L0MuonAlg::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  IChronoStatSvc * svc = chronoSvc();
  svc->chronoStart("L0MuonTrigger Initialize");

  // Set the layouts used to fill the OLs
  setLayouts();
  
  // Instanciate the MuonTrigger Units and Registers
  L0Muon::RegisterFactory::selectInstance(0);

  std::string xmlFileName = m_configfile;
  info() <<  "XML file = " << xmlFileName << endmsg;
  L0Muon::L0MuonKernelFromXML(xmlFileName);
  L0Muon::UnitFactory* ufactory = L0Muon::UnitFactory::instance();
  m_muontriggerunit = ufactory->topUnit();
  debug() <<  "MuonTrigger build from xml "<< endmsg;

  // Set debug mode
  if (m_debug) {
    //m_muontriggerunit->dumpUnitTree();
    m_muontriggerunit->setDebugMode();
    info() <<  "MuonTrigger debug mode "<< endmsg;
  }
  
  // Set properties
  // Set the properties of the MuonTriggerUnit 
  m_muontriggerunit->setProperties(l0MuonProperties());
  debug() <<  "MuonTrigger properties set "<< endmsg;

  // Initialize
  m_muontriggerunit->initialize();
  debug() <<  "MuonTrigger initialized"<< endmsg;

  svc->chronoStop("L0MuonTrigger Initialize");
  svc->chronoDelta("L0MuonTrigger Initialize",IChronoStatSvc::KERNEL);
  svc->chronoPrint("L0MuonTrigger Initialize");
  
  // L0MuonOutputs tool
  m_outputTool =  tool<L0MuonOutputs>( "L0MuonOutputs" , "OutputTool" , this );

  m_totEvent = 0;

  return StatusCode::SUCCESS;			  					  		      
}



StatusCode L0MuonAlg::execute()
{
  L0Muon::RegisterFactory::selectInstance(0);

  debug() << "-----------------------------------------------------------------" << endreq;
  debug() << "-- Start execution:" << endreq;
  IChronoStatSvc * svc = chronoSvc();
  svc->chronoStart("L0MuonTrigger Execute");

  debug() << "Chrono service started" << endreq;
  StatusCode sc;

  debug() << "Read data from digits ..." << endreq; 
  sc = fillOLsfromDigits();

  // Measure the time needed for loading OL's only
  //return StatusCode::SUCCESS;

  if( sc.isFailure() ) {
    error() << "Failed to load OLs" << endreq;
    return sc;
  }  

  // Preexecution phase: data exchange between PUs
  debug() << "Preexecute of MuonKernel units ..." << endreq;
  m_muontriggerunit->preexecute();

  // Execution phase: search for candidates and fill output registers
  debug() << "Execution of MuonKernel units ..." << endreq;
  m_muontriggerunit->execute();

  // Fill the Raw Event container or write on TES
  if ( m_storeFlag ) {
    debug() << "Fill RawEvent ..." << endreq;
    sc = m_outputTool->writeRawBanks(m_outputMode,m_bankVersion);
    if ( sc.isFailure() ) return sc;  
  } else {
    debug() << "Write on TES ..." << endreq;
    sc = m_outputTool->writeOnTES(m_procVersion,"");
    if ( sc.isFailure() ) return sc;  
  }

  // Fill the container for the L0DU (L0ProcessorData)
  debug() << "Fill L0ProcessorData ..." << endreq;
  sc = m_outputTool->writeL0ProcessorData();
  if ( sc.isFailure() ) return sc;  

  // Postexecution phase: reset registers
  debug() << "Postexecution of MuonKernel units ..." << endreq;
  m_muontriggerunit->postexecute();

  svc->chronoStop("L0MuonTrigger Execute");
  svc->chronoDelta("L0MuonTrigger Execute", IChronoStatSvc::KERNEL);
  //if( MSG::DEBUG >= log.level() ) svc->chronoPrint("L0MuonTrigger Execute");

  ++m_totEvent;

  debug() << "-- Execution done." << endreq;
  debug() << "-----------------------------------------------------------------" << endreq;

  return StatusCode::SUCCESS;
  
}


StatusCode L0MuonAlg::finalize()
{

  L0Muon::UnitFactory* ufactory = L0Muon::UnitFactory::instance();
  delete(ufactory);

  info() << "- ------------------------------------------------------------------"<<endmsg;
  info() << "- ========> Final summary of the L0Muon trigger (emulator) <========"<<endmsg;
  info() << "- Total number of events processed           : "
         <<format("%8d",m_totEvent)<<endmsg;
  m_outputTool->statTot(info());
  info() << "- ------------------------------------------------------------------"<<endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


void L0MuonAlg::setLayouts()
{
  m_layout=MuonSystemLayout(MuonStationLayout(MuonLayout(24,8)),
                            MuonStationLayout(MuonLayout(48,8)),
                            MuonStationLayout(MuonLayout(48,8)),
                            MuonStationLayout(MuonLayout(12,8)),
                            MuonStationLayout(MuonLayout(12,8)));

  m_ollayout = MuonSystemLayout(MuonStationLayout(MuonLayout(2,4)),
                                MuonStationLayout(MuonLayout(4,1),
                                                  MuonLayout(4,2),
                                                  MuonLayout(2,2),
                                                  MuonLayout(2,2)),
                                MuonStationLayout(MuonLayout(4,1),
                                                  MuonLayout(4,2),
                                                  MuonLayout(2,2),
                                                  MuonLayout(2,2)),
                                MuonStationLayout(MuonLayout(2,2)),
                                MuonStationLayout(MuonLayout(2,2)));


  m_lulayout=MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                              MuonStationLayout(MuonLayout(8,1),
                                                MuonLayout(4,2),
                                                MuonLayout(2,2),
                                                MuonLayout(2,2)),
                              MuonStationLayout(MuonLayout(8,1),
                                                MuonLayout(4,2),
                                                MuonLayout(2,2),
                                                MuonLayout(2,2)),
                              MuonStationLayout(MuonLayout(0,0),
                                                MuonLayout(4,2),
                                                MuonLayout(2,2),
                                                MuonLayout(2,2)),
                              MuonStationLayout(MuonLayout(0,0),
                                                MuonLayout(4,2),
                                                MuonLayout(2,2),
                                                MuonLayout(2,2)));	


  m_stripH =MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                             MuonStationLayout(MuonLayout(8,8),
                                               MuonLayout(4,8),
                                               MuonLayout(2,8),
                                               MuonLayout(2,8)),
                             MuonStationLayout(MuonLayout(8,8),
                                               MuonLayout(4,8),
                                               MuonLayout(2,8),
                                               MuonLayout(2,8)),
                             MuonStationLayout(MuonLayout(0,0),
                                               MuonLayout(4,8),
                                               MuonLayout(2,8),
                                               MuonLayout(2,8)),
                             MuonStationLayout(MuonLayout(0,0),
                                               MuonLayout(4,8),
                                               MuonLayout(2,8),
                                               MuonLayout(2,8)));

  m_stripV =MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                             MuonStationLayout(MuonLayout(48,1),
                                               MuonLayout(48,2),
                                               MuonLayout(48,2),
                                               MuonLayout(48,2)),
                             MuonStationLayout(MuonLayout(48,1),
                                               MuonLayout(48,2),
                                               MuonLayout(48,2),
                                               MuonLayout(48,2)),
                             MuonStationLayout(MuonLayout(0,0),
                                               MuonLayout(12,2),
                                               MuonLayout(12,2),
                                               MuonLayout(12,2)),
                             MuonStationLayout(MuonLayout(0,0),
                                               MuonLayout(12,2),
                                               MuonLayout(12,2),
                                               MuonLayout(12,2)));
}


std::map<std::string,L0Muon::Property>  L0MuonAlg::l0MuonProperties()
{

  // Translate the gaudi Algorithms properties into the L0MuonKernel properties:

  std::map<std::string,L0Muon::Property> properties;
  std::string prop;
  char buf[128];

  prop="";
  for (std::vector<double>::iterator iparam =  m_ptParameters.begin();iparam!= m_ptParameters.end();iparam++) {
    sprintf(buf,"%f",(*iparam));
    prop+=buf;
    iparam++;
    if( iparam!=  m_ptParameters.end()) prop+=",";
    iparam--;
  }
  properties["ptParameters"]   = L0Muon::Property(prop);  

  prop="";
  for (std::vector<int>::iterator ifoi =  m_foiXSize.begin();ifoi!= m_foiXSize.end();ifoi++) {
    sprintf(buf,"%d",(*ifoi));
    prop+=buf;
    ifoi++;
    if( ifoi!=  m_foiXSize.end()) prop+=",";
    ifoi--;
  }
  properties["foiXSize"]       = L0Muon::Property(prop);

  prop="";
  for (std::vector<int>::iterator ifoi =  m_foiYSize.begin();ifoi!= m_foiYSize.end();ifoi++) {
    sprintf(buf,"%d",(*ifoi));
    prop+=buf;
    ifoi++;
    if( ifoi!=  m_foiYSize.end()) prop+=",";
    ifoi--;
  }
  properties["foiYSize"]       = L0Muon::Property(prop);

  properties["ignoreM1"]       = m_ignoreM1 ? L0Muon::Property("1") : L0Muon::Property("0");

  
  sprintf(buf,"%d",m_procVersion);
  prop=buf;
  properties["procVersion"]    = L0Muon::Property(prop);;

  info() << "MuonTriggerUnit properties are:"<<endreq;
  for (std::map<std::string,L0Muon::Property>::iterator ip= properties.begin(); ip!=properties.end();ip++){
    info() << " "<< (*ip).first << " = "<< ((*ip).second).value() <<endreq;
  }

  return properties;
}

StatusCode L0MuonAlg::fillOLsfromDigits()
{
  debug() << "fillOLsfromDigits:  IN "<<endmsg;

  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  
  //===============================
  // get Muon digitisations
  //===============================

  std::vector<LHCb::MuonTileID> ddigits;
  if (exist<LHCb::MuonDigits>( LHCb::MuonDigitLocation::MuonDigit ) )
  {
    // The MuonDigits are on the TES (Boole); 
    // get them and fill MuonTileID list
    debug() << "fillOLsfromDigits:  Getting hits from muon digits"<<m_muonBuffer<<endmsg;
    LHCb::MuonDigits* digits = get<LHCb::MuonDigits>( LHCb::MuonDigitLocation::MuonDigit );  
//     debug() << "fillOLsfromDigits:   digits="<< digits <<endmsg;
    LHCb::MuonDigits::const_iterator did;
    for( did = digits->begin() ; did != digits->end() ; did++ ){  
      LHCb::MuonTileID mkey = (*did)->key();
      ddigits.push_back(mkey);    
    }

  } 
  else 
  {
    // The MuonDigits are not on the TES (DaVinci); 
    // use the MuonRawBufer tool to produce them from the raw buffer
    debug() << "Getting hits from Raw buffer "<<m_muonBuffer<<endmsg;
    if(!m_muonBuffer) {
    // First call: initialize the pointer to the Muon Raw Buffer Interface
      m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer");
      if(!m_muonBuffer) {    
        error() << "No pointer to muon raw buffer interface tool"<<m_muonBuffer<<endmsg;
        return StatusCode::FAILURE;
      }
    }
//     debug() << "fillOLsfromDigits:  m_muonBuffer "<<m_muonBuffer<<endmsg;
    ddigits=m_muonBuffer->getTile();
  }  

  debug() << "fillOLsfromDigits:  ddigits.size()= "<<ddigits.size()<<endmsg;

  std::vector<LHCb::MuonTileID>::const_iterator id;

  for( id = ddigits.begin() ; id != ddigits.end() ; id++ ){
    LHCb::MuonTileID mkey = *id;
    int sta = mkey.station(); 
    LHCb::MuonTileID olID = m_ollayout.contains(mkey);
    char bufnm[1024];
    //sprintf(bufnm,"(R%d,%d,%d)",olID.region(),olID.nX(),olID.nY());
    sprintf(bufnm,"(Q%d,R%d,%d,%d)",
            olID.quarter(),olID.region(),olID.nX(),olID.nY());           
    char buf[4096];
    char* format = "OL_%d_%s";
    sprintf(buf,format,sta,bufnm);
    
//     debug() << "fillOLsfromDigits:  pad= "<<mkey.toString()<<endmsg;
    if (m_debug) std::cout << "fillOLsfromDigits:     "
                           <<" mkey: "<<mkey.toString()
                           <<" ; buf: "<<buf<<"\n";

    L0Muon::TileRegister* pReg = rfactory->createTileRegister(buf,0);
    pReg->setTile(mkey);
  }

    
  if (m_debug) rfactory->dump();  
    
  return StatusCode::SUCCESS;
}




