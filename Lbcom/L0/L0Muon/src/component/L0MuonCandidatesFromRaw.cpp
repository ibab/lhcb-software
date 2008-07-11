// $Id: L0MuonCandidatesFromRaw.cpp,v 1.10 2008-07-11 15:33:20 jucogan Exp $
#include <algorithm>
#include <math.h>
#include <set>
#include <boost/dynamic_bitset.hpp>

#include "L0MuonCandidatesFromRaw.h"
#include "L0MuonKernel/L0MuonKernelFromXML.h"

/// Gaudi interfaces
#include "GaudiKernel/IChronoStatSvc.h"

/// Gaudi factories for algorithms 
#include "GaudiKernel/AlgFactory.h"

/// Utility classes
#include "GaudiKernel/IToolSvc.h" 
  
#include "Event/RawEvent.h"

#include "Kernel/MuonTileID.h"

DECLARE_ALGORITHM_FACTORY( L0MuonCandidatesFromRaw );

L0MuonCandidatesFromRaw::L0MuonCandidatesFromRaw(const std::string& name,
                                         ISvcLocator* pSvcLocator) 
  :GaudiAlgorithm(name, pSvcLocator)
 
{
  
  m_configfile="L0MuonKernel.xml";

  m_ptParameters.push_back(40.);   // Precision on PT (MeV)
  m_ptParameters.push_back(7.);    // Number of bins in PT
  
  declareProperty( "ConfigFile"     , m_configfile      );
  declareProperty( "PtParameters"   , m_ptParameters    ); 
  declareProperty( "ProcVersion"    , m_procVersion = 0 );
  declareProperty( "Extension"      , m_extension = ""  );
  declareProperty( "WriteOnTES"     , m_writeOnTES = true);  
  declareProperty( "WriteL0ProcData", m_writeL0ProcData  = false);  
  declareProperty( "OutputMode"     , m_mode = 0 );
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

  // Loop over time slots
  for (int bx=-7; bx<8; ++bx){

    setProperty("RootInTes",timeSlot(bx));  
    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;

    if( msgLevel(MSG::VERBOSE) ) verbose() << "decoding event ... "<<timeSlot(bx) << endmsg;

    sc = m_outputTool->setProperty( "RootInTES", rootInTES() );
    if ( sc.isFailure() ) return sc;// error printed already by GaudiAlgorithm

    // Decode Raw banks
    sc = m_outputTool->decodeRawBanks(m_mode);
    if ( sc.isFailure() ) continue; // error printed already by GaudiAlgorithm
    
    // Print Errors
    if (msgLevel(MSG::DEBUG)) {
      m_outputTool->errors(debug());
    }
  
    // Write on TES
    if ( m_writeOnTES) {
      if( msgLevel(MSG::VERBOSE) ) verbose() << "Write on TES ..." << endreq;
      sc = m_outputTool->writeOnTES(m_procVersion,m_extension);
      if ( sc.isFailure() ) continue;  
    }
  
    // Fill the container for the L0DU (L0ProcessorData)
    if ( m_writeL0ProcData) {
      if( msgLevel(MSG::VERBOSE) ) verbose() << "Fill L0ProcessorData ..." << endreq;
      sc = m_outputTool->writeL0ProcessorData(m_extension);
      if ( sc.isFailure() ) continue;  
    }

    // Release registers used by the converters
    sc = m_outputTool->releaseRegisters();
    if ( sc.isFailure() ) continue; 

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


