// $Id: L0MuonCandidatesFromRaw.cpp,v 1.24 2010-01-21 08:35:35 jucogan Exp $
#include <algorithm>
#include <math.h>
#include <set>

#include "L0MuonCandidatesFromRaw.h"
#include "L0MuonKernel/L0MuonKernelFromXML.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "SubstituteEnvVarInPath.h"

/// Gaudi interfaces
#include "GaudiKernel/IChronoStatSvc.h"

/// Gaudi factories for algorithms 
#include "GaudiKernel/AlgFactory.h"

#include "Event/RawEvent.h"
#include "Event/ODIN.h"
#include "Kernel/MuonTileID.h"

#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"

DECLARE_ALGORITHM_FACTORY( L0MuonCandidatesFromRaw );

L0MuonCandidatesFromRaw::L0MuonCandidatesFromRaw(const std::string& name,
                                         ISvcLocator* pSvcLocator) 
  :L0FromRawBase(name, pSvcLocator)
 
{

  declareProperty( "ConfigFile"     , m_configfile= "$PARAMFILESROOT/data/L0MuonKernel.xml" );

  declareProperty( "DisableTAE"     , m_disableTAE = true  );

  // Default for HLT :
  declareProperty( "DAQMode"        , m_mode = 0 );
}


StatusCode L0MuonCandidatesFromRaw::initialize()
{

  StatusCode sc = L0FromRawBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  IChronoStatSvc * svc = chronoSvc();
  svc->chronoStart("L0MuonCandidatesFromRaw Initialize");
  
  L0Muon::RegisterFactory::selectInstance(1);

  // Instanciate the MuonTrigger registers
  std::string xmlFileName = L0MuonUtils::SubstituteEnvVarInPath(m_configfile);
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

  // TAE window
  std::vector<std::string> bunches;
  if (m_disableTAE) {
    bunches.push_back("");
  } else {
    sc=tae_bunches(bunches);
    if( msgLevel(MSG::VERBOSE) ) verbose() << "Looping over "<<bunches.size()<<" bunches" << endmsg;
  }

  // Loop over time slots of TAE window
  for (std::vector<std::string>::iterator itbunches=bunches.begin(); itbunches<bunches.end(); ++itbunches) {
    if (!m_disableTAE) {
      sc=setProperty("RootInTes",(*itbunches));
      if (sc.isFailure()) return Error("Can not set RootInTes ",StatusCode::SUCCESS,10);
    }
    
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
      sc = m_outputTool->writeOnTES(m_l0context);
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
    if ( m_writeProcData) {
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
  
  return L0FromRawBase::finalize();  // must be called after all other actions
}


StatusCode L0MuonCandidatesFromRaw::tae_bunches(std::vector<std::string> &bunches)
{
  
  bunches.clear();
  if( exist<LHCb::ODIN>( LHCb::ODINLocation::Default) ){
    LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
    int tae_window = odin->timeAlignmentEventWindow();
    for (int it = -1*tae_window ; it<=tae_window ; ++it){
      bunches.push_back( timeSlot(it) );
    }
  } else {
    return Warning("Fail to get ODIN - can not determine the tae window - no time slot will be decoded"
                   ,StatusCode::SUCCESS,10);
  }
  
    
  return StatusCode::SUCCESS;
  
}


