#include <algorithm>
#include <math.h>
#include <set>

#include "L0MuonCandidatesFromRaw.h"
#include "L0MuonKernel/L0MuonKernelFromXML.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "SubstituteEnvVarInPath.h"

/// Gaudi interfaces
#include "GaudiKernel/IChronoStatSvc.h"


#include "Event/RawEvent.h"
#include "Event/ODIN.h"
#include "Kernel/MuonTileID.h"

#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"

DECLARE_ALGORITHM_FACTORY( L0MuonCandidatesFromRaw )

L0MuonCandidatesFromRaw::L0MuonCandidatesFromRaw(const std::string& name,
                                         ISvcLocator* pSvcLocator) 
  : L0FromRawBase(name, pSvcLocator)
  , m_outputTool(NULL)
{

  declareProperty( "ConfigFile"     , m_configfile= "$PARAMFILESROOT/data/L0MuonKernel.xml" );

  declareProperty( "DisableTAE"     , m_disableTAE = true  );

  // Default for HLT :
  declareProperty( "DAQMode"        , m_mode = 0 );

  m_totEvent = 0;
  m_totBx = 0;
  m_errorEvent = 0;
  m_enableTAE = false;
  m_procVersion = 0;

}

StatusCode L0MuonCandidatesFromRaw::initialize()
{

  StatusCode sc = L0FromRawBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  //   IChronoStatSvc * svc = chronoSvc();
  //   svc->chronoStart("L0MuonCandidatesFromRaw Initialize");
  
  L0Muon::RegisterFactory::selectInstance(1);
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  if ( ! rfactory->filledFromXML() ) {
    // Instanciate the MuonTrigger registers
    std::string xmlFileName = L0MuonUtils::SubstituteEnvVarInPath(m_configfile);
    info() <<  "XML config file = " << xmlFileName << endmsg;
    L0Muon::L0MuonKernelFromXML(xmlFileName,false);
    if( msgLevel(MSG::DEBUG) ) debug() <<  "MuonTrigger build from xml "<< endmsg;
  } else {
    info() << "MuonTrigger already build from xml"<< endmsg;
  }
  
  //   svc->chronoStop("L0MuonCandidatesFromRaw Initialize");
  //   svc->chronoDelta("L0MuonCandidatesFromRaw Initialize",IChronoStatSvc::KERNEL);
  //   svc->chronoPrint("L0MuonCandidatesFromRaw Initialize");
  
  // L0MuonOutputs tool
  m_outputTool =  tool<L0MuonOutputs>( "L0MuonOutputs"  , "OutputTool" , this );
  m_outputTool->setDecodingMode();
  
  m_totEvent = 0;
  m_totBx = 0;
  m_errorEvent = 0;

  m_enableTAE = !m_disableTAE;
  
  // TAE slots names
  if (m_enableTAE){
    m_tae_items[-7] = "Prev7/";
    m_tae_items[-6] = "Prev6/";
    m_tae_items[-5] = "Prev5/";
    m_tae_items[-4] = "Prev4/";
    m_tae_items[-3] = "Prev3/";
    m_tae_items[-2] = "Prev2/";
    m_tae_items[-1] = "Prev1/";
    m_tae_items[ 0] = "";
    m_tae_items[ 1] = "Next1/";
    m_tae_items[ 2] = "Next2/";
    m_tae_items[ 3] = "Next3/";
    m_tae_items[ 4] = "Next4/";
    m_tae_items[ 5] = "Next5/";
    m_tae_items[ 6] = "Next6/";
    m_tae_items[ 7] = "Next7/";
  }

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

  //   IChronoStatSvc * svc = chronoSvc();
  //   svc->chronoStart("L0MuonCandidatesFromRaw Execute");
  
  StatusCode sc;

  // Scan the list of input location and select the first existing one.
  std::string rawEventLocation;
  if ( selectRawEventLocation(rawEventLocation).isFailure() ) 
    return Error("No valid raw event location found",StatusCode::SUCCESS,50);
  
  // TAE mode
  int tae_size = 0;
  if (m_enableTAE) 
  {
    LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default,IgnoreRootInTES);
    if ( NULL != odin ) {
      // TAE size from odin
      tae_size = int(odin->timeAlignmentEventWindow());
    } 
    else 
    {
      Warning("ODIN not found at "+LHCb::ODINLocation::Default+", TAE mode requested but not used" ,
              StatusCode::FAILURE,50).ignore();
    }
  }

  int ntae = 0;
  for (int itae = -1*tae_size; itae<=tae_size; ++itae)
  {
    std::string taeInTes = m_tae_items[itae];

    // Decode Raw banks
    m_outputTool->setMode(m_mode);
    sc = m_outputTool->decodeRawBanks( taeInTes+rawEventLocation , m_useRootInTES , m_statusOnTES);
    if ( sc.isFailure() ) 
    { 
      Warning("Error from decodeRawBanks - skip this time slice"
              ,StatusCode::SUCCESS,50);
      sc = m_outputTool->releaseRegisters();
      if ( sc.isFailure() ) 
      {
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
      if( msgLevel(MSG::VERBOSE) ) verbose() << "Write on TES ..." << endmsg;
      sc = m_outputTool->writeOnTES(m_l0context , taeInTes);
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
      if( msgLevel(MSG::VERBOSE) ) verbose() << "Fill L0ProcessorData ..." << endmsg;
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

    ++ntae;
  } // End of loop over time slots
  if (ntae==0) return Error("No valid time slice found",StatusCode::SUCCESS,50);
  
  //   svc->chronoStop("L0MuonCandidatesFromRaw Execute");
  //   svc->chronoDelta("L0MuonCandidatesFromRaw Execute", IChronoStatSvc::KERNEL);

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
