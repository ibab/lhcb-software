// $Id: L0MuonCandidatesFromRaw.cpp,v 1.19 2009-02-03 12:04:24 jucogan Exp $
#include <algorithm>
#include <math.h>
#include <set>

#include "L0MuonCandidatesFromRaw.h"
#include "L0MuonKernel/L0MuonKernelFromXML.h"
#include "ProcessorKernel/RegisterFactory.h"

/// Gaudi interfaces
#include "GaudiKernel/IChronoStatSvc.h"

/// Gaudi factories for algorithms 
#include "GaudiKernel/AlgFactory.h"

#include "Event/RawEvent.h"
#include "Kernel/MuonTileID.h"

#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"

DECLARE_ALGORITHM_FACTORY( L0MuonCandidatesFromRaw );

L0MuonCandidatesFromRaw::L0MuonCandidatesFromRaw(const std::string& name,
                                         ISvcLocator* pSvcLocator) 
  :GaudiAlgorithm(name, pSvcLocator)
 
{
  
  m_configfile="L0MuonKernel.xml";
  if( NULL != getenv("PARAMFILESROOT") )
  {
    m_configfile  = getenv( "PARAMFILESROOT" ) ;
    m_configfile += "/data/L0MuonKernel.xml"  ;
  }

  declareProperty( "ConfigFile"     , m_configfile      );

  declareProperty( "DisableTAE"     , m_disableTAE = false  );

  // Default for HLT :
  declareProperty( "WriteOnTES"     , m_writeOnTES       = true);  
  declareProperty( "WriteL0ProcData", m_writeL0ProcData  = false);  
  declareProperty( "DAQMode"        , m_mode = 0 );
}


StatusCode L0MuonCandidatesFromRaw::initialize()
{

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  IChronoStatSvc * svc = chronoSvc();
  svc->chronoStart("L0MuonCandidatesFromRaw Initialize");
  
  L0Muon::RegisterFactory::selectInstance(1);

  // Instanciate the MuonTrigger registers
  std::string xmlFileName = m_configfile;
  info() <<  "XML config file = " << xmlFileName << endmsg;
  L0Muon::L0MuonKernelFromXML(xmlFileName,false);
  if( msgLevel(MSG::DEBUG) ) debug() <<  "MuonTrigger build from xml "<< endmsg;

  svc->chronoStop("L0MuonCandidatesFromRaw Initialize");
  svc->chronoDelta("L0MuonCandidatesFromRaw Initialize",IChronoStatSvc::KERNEL);
  //   svc->chronoPrint("L0MuonCandidatesFromRaw Initialize");
  
  // L0MuonOutputs tool
  m_outputTool =  tool<L0MuonOutputs>( "L0MuonOutputs"  , "OutputTool" , this );
  m_outputTool->setDecodingMode();
  
  m_totEvent = 0;
  m_totBx = 0;
  m_errorEvent = 0;

  if( msgLevel(MSG::DEBUG) ) {
    debug()<<"Output mode = "<< m_mode;
    if (m_mode>0) {
      debug()<<"=> all L0Muon banks will be used : "<<endmsg;
      debug()<<"  - L0MuonCtrlCand-1st part : controller boards final candidates     - should always be there"<<endmsg;
      debug()<<"  - L0MuonCtrlCand-2nd part : controller boards input candidates     - if present"<<endmsg;
      debug()<<"  - L0MuonProcCand          : processing boards PU & BCSU candidates - if present"<<endmsg;
      debug()<<"  - L0MuonProcData-1st part : PU link error & optical link data      - if present"<<endmsg;
      debug()<<"  - L0MuonProcData-2nd part : PU neighbours data                     - if present"<<endmsg;
    } else {
      debug()<<"=> only the 1st part of the L0MuonCtrlCand bank will be used (suitable for HLT & MC)"<<endmsg;
    }
  }
  
  return StatusCode::SUCCESS;			  					  		      
}

StatusCode L0MuonCandidatesFromRaw::execute()
{
  L0Muon::RegisterFactory::selectInstance(1);

  IChronoStatSvc * svc = chronoSvc();
  svc->chronoStart("L0MuonCandidatesFromRaw Execute");

  StatusCode sc;

  std::vector<std::string> bunches;
  bunches.push_back("");
  if (!m_disableTAE) {
    sc=tae_bunches(bunches);
    if ( sc.isFailure() ) {
      return Error("Failed to get available bunches",StatusCode::SUCCESS,50);
    }
  }

  for (std::vector<std::string>::iterator itbunches=bunches.begin(); itbunches<bunches.end(); ++itbunches) {
    setProperty("RootInTes",(*itbunches));

    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;

    if( msgLevel(MSG::VERBOSE) ) verbose() << "decoding event ... "<<(*itbunches) << endmsg;

    sc = m_outputTool->setProperty( "RootInTES", rootInTES() );
    if ( sc.isFailure() ) continue;// error printed already by GaudiAlgorithm

    // Decode Raw banks
    m_outputTool->setMode(m_mode);
    sc = m_outputTool->decodeRawBanks();
    if ( sc.isFailure() ) { 
      Warning("Error from decodeRawBanks - skip this time slice"
                     ,StatusCode::SUCCESS,50);
      sc = m_outputTool->releaseRegisters();
      if ( sc.isFailure() ) {
        return Warning("Fail to release registers - skip the rest of event"
                       ,StatusCode::SUCCESS,50);
      }
      continue;
    }
    
    // Print Errors
    if (msgLevel(MSG::DEBUG)) {
      m_outputTool->errors(debug());
    }
  
    // Write on TES
    if ( m_writeOnTES) {
      if( msgLevel(MSG::VERBOSE) ) verbose() << "Write on TES ..." << endreq;
      sc = m_outputTool->writeOnTES();
      if ( sc.isFailure() ) { 
        Warning("Error from writeOnTES - skip this time slice"
                       ,StatusCode::SUCCESS,50);
        sc = m_outputTool->releaseRegisters();
        if ( sc.isFailure() ) {
          return Warning("Fail to release registers - skip the rest of event"
                         ,StatusCode::SUCCESS,50);
        }
        continue;
      }
    }
  
    // Fill the container for the L0DU (L0ProcessorData)
    if ( m_writeL0ProcData) {
      if( msgLevel(MSG::VERBOSE) ) verbose() << "Fill L0ProcessorData ..." << endreq;
      sc = m_outputTool->writeL0ProcessorData();
      if ( sc.isFailure() ) { 
        Warning("Error from writeL0ProcessorData - skip this time slice"
                       ,StatusCode::SUCCESS,50);
        sc = m_outputTool->releaseRegisters();
        if ( sc.isFailure() ) {
          return Warning("Fail to release registers - skip the rest of event"
                         ,StatusCode::SUCCESS,50);
        }
        continue;
      }
    }

    // Release registers used by the converters
    sc = m_outputTool->releaseRegisters();
    if ( sc.isFailure() ) {
      return Warning("Fail to release registers - skip the rest of event"
                     ,StatusCode::SUCCESS,50);
    }

    if( msgLevel(MSG::VERBOSE) ) verbose() << "... decoding done"<< endmsg;

    ++m_totBx;

  } // End of loop over time slots
  
  svc->chronoStop("L0MuonCandidatesFromRaw Execute");
  svc->chronoDelta("L0MuonCandidatesFromRaw Execute", IChronoStatSvc::KERNEL);

  ++m_totEvent;

  return StatusCode::SUCCESS;
  
}


StatusCode L0MuonCandidatesFromRaw::finalize()
{
  if (msgLevel(MSG::INFO)) {

    m_outputTool->errorSummary(info());

    info() << "- ------------------------------------------------------------------"<<endmsg;
    info() << "- ========> Final summary of the L0Muon trigger (from raw) <========"<<endmsg;
    info() << "- Total number of events processed           : "
           <<format("%8d",m_totEvent)<<endmsg;
    info() << "- Total number of bunch crossings processed  : "
           <<format("%8d",m_totBx)<<endmsg;
    m_outputTool->statTot(info());
    info() << "- ------------------------------------------------------------------"<<endmsg;
  }
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


StatusCode L0MuonCandidatesFromRaw::tae_bunches(std::vector<std::string> &bunches)
{
  
  DataObject * pObj=get<DataObject>("/Event");
  if (!pObj) return Error("tae_bunches : no DataObject found at /Event",StatusCode::FAILURE,50);
  
  SmartIF<IDataManagerSvc> mgr(eventSvc());
  if ( !mgr ) return Error("tae_bunches : failed to retrieve DataManagerSvc",StatusCode::FAILURE,50);

  typedef std::vector<IRegistry*> Leaves;
  Leaves leaves;
  StatusCode sc = mgr->objectLeaves(pObj, leaves);
  if (! sc.isSuccess() ) return Error("tae_bunches : failed to get leaves",StatusCode::FAILURE,50);
  if( msgLevel(MSG::VERBOSE) ) verbose() << " tae_bunches : nb of leaves =  "<<leaves.size()<< endmsg;
  
  for ( Leaves::const_iterator i=leaves.begin(); i !=leaves.end(); i++ ) {
    
    const std::string& id = (*i)->identifier();
    if( msgLevel(MSG::VERBOSE) ) verbose() << " tae_bunches : leaf found @ "<<id<< endmsg;

    unsigned int first=id.rfind("/");
    if (first>=id.size()) continue;
    
    std::string top=id.substr(first,id.size()-first);
    if ((id.find("/Event/Next")<id.size()) || (id.find("/Event/Prev")<id.size())) {
      std::string bunch=id.substr(first+1,id.size()-first);
      if (bunch.size()<5) continue;
      bunch.append("/");
      if( msgLevel(MSG::VERBOSE) ) verbose() << " tae_bunches : add "<<bunch<< endmsg;
      bunches.push_back(bunch);
    }
  }

  return StatusCode::SUCCESS;
  
}


