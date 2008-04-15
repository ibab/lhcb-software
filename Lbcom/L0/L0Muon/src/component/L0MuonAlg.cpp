// $Id: L0MuonAlg.cpp,v 1.10 2008-04-15 09:19:29 jucogan Exp $
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

// from Event
#include "Event/MuonDigit.h"
#include "Event/L0MuonData.h"

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
  
  std::vector<int> time_slots;
  //  for (int i=-7;i<=7;++i) time_slots.push_back(i);
  time_slots.push_back(0);
  declareProperty( "TimeSlots"  , m_time_slots = time_slots);

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

  declareProperty("StoreInBuffer"  , m_storeBank  = true);  
  declareProperty("WriteOnTES"     , m_writeOnTES = false);  
  declareProperty("WriteL0ProcData", m_writeL0ProcData  = true);  

  declareProperty("InputSource"    , m_inputSource = 0);  

  declareProperty("DebugMode"      , m_debug = false );
  declareProperty("BankVersion"    , m_bankVersion = 0 );
  declareProperty("ProcVersion"    , m_procVersion = 0 );
  declareProperty("OutputMode"     , m_outputMode = 0 );

  declareProperty("Extension"     , m_extension = ""  );
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
  if( msgLevel(MSG::DEBUG) ) debug() <<  "MuonTrigger build from xml "<< endmsg;

  // Set debug mode
  if (m_debug) {
    //m_muontriggerunit->dumpUnitTree();
    m_muontriggerunit->setDebugMode();
    info() <<  "MuonTrigger debug mode "<< endmsg;
  }
  
  // Set properties
  // Set the properties of the MuonTriggerUnit 
  m_muontriggerunit->setProperties(l0MuonProperties());

  // Initialize
  m_muontriggerunit->initialize();

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

  if( msgLevel(MSG::DEBUG) )  {
    debug() << "-----------------------------------------------------------------" << endreq;
    debug() << "-- Start execution:" << endreq;
  }
  IChronoStatSvc * svc = chronoSvc();
  svc->chronoStart("L0MuonTrigger Execute");

  //debug() << "Chrono service started" << endreq;
  StatusCode sc;

  //debug() << "Read data from digits ..." << endreq; 
  sc = fillOLsfromDigits();

  // Measure the time needed for loading OL's only
  //return StatusCode::SUCCESS;

  if( sc.isFailure() ) {
    error() << "Failed to load OLs" << endreq;
    return sc;
  }  

  // Preexecution phase: data exchange between PUs
  if( msgLevel(MSG::DEBUG) ) debug() << "Preexecute of MuonKernel units ..." << endreq;
  m_muontriggerunit->preexecute();

  // Execution phase: search for candidates and fill output registers
  if( msgLevel(MSG::DEBUG) ) debug() << "Execution of MuonKernel units ..." << endreq;
  m_muontriggerunit->execute();

  // Fill the Raw Event container 
  if ( m_storeBank ) {
    if( msgLevel(MSG::DEBUG) ) debug() << "Fill RawEvent ..." << endreq;
    sc = m_outputTool->writeRawBanks(m_outputMode,m_bankVersion);
    if ( sc.isFailure() ) return sc;  
  } 
  
  // Write on TES
  if ( m_writeOnTES) {
    if( msgLevel(MSG::DEBUG) ) debug() << "Write on TES ..." << endreq;
    sc = m_outputTool->writeOnTES(m_procVersion, m_extension);
    if ( sc.isFailure() ) return sc;  
  }

  // Fill the container for the L0DU (L0ProcessorData)
  if ( m_writeL0ProcData) {
    if( msgLevel(MSG::DEBUG) ) debug() << "Fill L0ProcessorData ..." << endreq;
    sc = m_outputTool->writeL0ProcessorData(m_extension);
    if ( sc.isFailure() ) return sc;  
  }
  
  // Postexecution phase: reset registers
  if( msgLevel(MSG::DEBUG) ) debug() << "Postexecution of MuonKernel units ..." << endreq;
  m_muontriggerunit->postexecute();

  svc->chronoStop("L0MuonTrigger Execute");
  svc->chronoDelta("L0MuonTrigger Execute", IChronoStatSvc::KERNEL);
  //if( MSG::DEBUG >= log.level() ) svc->chronoPrint("L0MuonTrigger Execute");

  ++m_totEvent;

  if( msgLevel(MSG::DEBUG) ){
    debug() << "-- Execution done." << endreq;
    debug() << "-----------------------------------------------------------------" << endreq;
  }
  
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
  //  debug() << "fillOLsfromDigits:  IN "<<endmsg;

  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){
    setProperty("RootInTes",timeSlot(*it_ts));
    //    info()<<"RootInTES "<<rootInTES()<<endmsg;

    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;

    std::vector<LHCb::MuonTileID> ddigits;

    if (m_inputSource>0) {
      // Read the data from the L0Muon itself (must have been already decoded from raw)
      if (  exist<LHCb::L0MuonDatas>( LHCb::L0MuonDataLocation::Default) ) {
      
        if( msgLevel(MSG::DEBUG) ) debug() << "fillOLsfromDigits:  Getting hits from L0Muon itself"<<endmsg;
      
        LHCb::L0MuonDatas* pdatas = get<LHCb::L0MuonDatas>( LHCb::L0MuonDataLocation::Default);
        LHCb::L0MuonDatas::const_iterator itdata;
        for (itdata = pdatas->begin() ; itdata!=pdatas->end() ; ++itdata){
          LHCb::MuonTileID mkey = (*itdata)->key();    
          std::vector<LHCb::MuonTileID> ols = (*itdata)->ols();
          if (ols.size()>0) {
            if( msgLevel(MSG::DEBUG) ) debug()  <<"fillOLsfromDigits:    PU: "
                                                <<mkey.toString()<<" => "<<ols.size()<<" hits found"<<endmsg;;
            for (std::vector<LHCb::MuonTileID>::iterator itol=ols.begin(); itol!=ols.end(); ++itol){
              //              if( msgLevel(MSG::DEBUG) )debug()  <<"fillOLsfromDigits:       "<<(*itol).toString()<<endmsg;
              ddigits.push_back(*itol);
            }
          }
        
        }

      }
    
    } 
    else if (exist<LHCb::MuonDigits>( LHCb::MuonDigitLocation::MuonDigit ) )
    {
      // Read the MuonDigits from the TES (Boole); 
      if( msgLevel(MSG::DEBUG) ) debug() << "fillOLsfromDigits:  Getting hits from muon digits"<<m_muonBuffer<<endmsg;
      LHCb::MuonDigits* digits = get<LHCb::MuonDigits>( LHCb::MuonDigitLocation::MuonDigit );  
      LHCb::MuonDigits::const_iterator did;
      for( did = digits->begin() ; did != digits->end() ; did++ ){  
        LHCb::MuonTileID mkey = (*did)->key();
        ddigits.push_back(mkey);    
      }
    } 
    else 
    {
      // Use the MuonRawBufer tool to produce the MuonDigits from the raw buffer (DaVinci)
      if( msgLevel(MSG::DEBUG) ) debug() << "Getting hits from Raw buffer "<<m_muonBuffer<<endmsg;
      if(!m_muonBuffer) {
        // First call: initialize the pointer to the Muon Raw Buffer Interface
        m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer","MuonRaw", this);
        if(!m_muonBuffer) {    
          error() << "No pointer to muon raw buffer interface tool"<<m_muonBuffer<<endmsg;
          return StatusCode::FAILURE;
        }
      }
      IProperty* prop = dynamic_cast<IProperty*>( m_muonBuffer );
      if( prop ) {
        StatusCode sc = prop->setProperty( "RootInTES", rootInTES() );
        if( sc.isFailure() )
          return Error( "Unable to set RootInTES property of MuonRawBuffer", sc );
      }
      else
        return Error( "Unable to locate IProperty interface of MuonRawBuffer" );

      m_muonBuffer->getTile(ddigits);
      m_muonBuffer->forceReset();
    }  

    if( msgLevel(MSG::DEBUG) ) debug() << "fillOLsfromDigits:  ddigits.size()= "<<ddigits.size()<<endmsg;

    std::vector<LHCb::MuonTileID>::const_iterator id;

    for( id = ddigits.begin() ; id != ddigits.end() ; id++ ){

      LHCb::MuonTileID mkey = *id;

      //       if( msgLevel(MSG::DEBUG) )debug() << "fillOLsfromDigits:     mkey: "<<mkey.toString()<<endmsg;

      int sta = mkey.station(); 
      LHCb::MuonTileID olID = m_ollayout.contains(mkey);

      //       if( msgLevel(MSG::DEBUG) )debug() << "fillOLsfromDigits:     olID: "<<olID.toString()<<endmsg;

      char bufnm[1024];
      sprintf(bufnm,"(Q%d,R%d,%d,%d)",
              olID.quarter(),olID.region(),olID.nX(),olID.nY());           
      char buf[4096];
      char* format = "OL_%d_%s";
      sprintf(buf,format,sta,bufnm);
    
      //       if( msgLevel(MSG::DEBUG) )debug() << "fillOLsfromDigits:     buf: "<<buf<<endmsg;

      if( msgLevel(MSG::DEBUG) ) {
        debug() << "fillOLsfromDigits:     mkey: "<<mkey.toString();
        debug() << " olID: "<<olID.toString();
        debug() << " buf: "<<buf<<endmsg;
      }
      

      L0Muon::TileRegister* pReg = rfactory->createTileRegister(buf,0);
      pReg->setTile(mkey);
    }

  } // End of loop over time slots

    
  if (m_debug) rfactory->dump();  
    
  return StatusCode::SUCCESS;
}
