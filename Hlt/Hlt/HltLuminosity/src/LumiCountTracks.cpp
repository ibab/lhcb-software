// $Id: LumiCountTracks.cpp,v 1.3 2008-08-13 16:03:52 panmanj Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Kernel/IOnOffline.h"
// from LHCB
#include "Event/Track.h"
#include "Event/HltLumiSummary.h"

#include "HltBase/ANNSvc.h"

// local
#include "LumiCountTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiCountTracks
//
// 2008-07-21 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiCountTracks );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiCountTracks::LumiCountTracks( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputSelection" ,      m_InputSelectionName);
  declareProperty( "CounterName"    ,      m_CounterName);
  declareProperty( "OutputContainer",      m_OutputContainerName = LHCb::HltLumiSummaryLocation::Default );

}
//=============================================================================
// Destructor
//=============================================================================
LumiCountTracks::~LumiCountTracks() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiCountTracks::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  info() << "InputSelection         " << m_InputSelectionName  << endmsg;
  info() << "CounterName            " << m_CounterName         << endmsg;
  info() << "OutputContainer        " << m_OutputContainerName << endmsg;


  // ------------------------------------------
  IANNSvc* annSvc = svc<IANNSvc>("LumiANNSvc");

  boost::optional<IANNSvc::minor_value_type> 
    x = annSvc->value("LumiCounters", m_CounterName);

  if (!x) {
    warning() << "LumiCounter not found with name: " << m_CounterName <<  endmsg;
  } else {
    m_Counter = x->second;
    info() << "ExtraInfo key value: " << m_Counter << endmsg;
  }
  // ------------------------------------------
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiCountTracks::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // ------------------------------------------
  // get (existing) container
  LHCb::HltLumiSummary* hltLS;
  if ( !exist<LHCb::HltLumiSummarys>(m_OutputContainerName) ){
    // create output container on the TES
    m_HltLumiSummarys = new LHCb::HltLumiSummarys();
    // locate them in the TES
    put(m_HltLumiSummarys, m_OutputContainerName); 
    // initialize output class
    hltLS = new LHCb::HltLumiSummary();
    m_HltLumiSummarys->insert( hltLS );
    debug() << m_OutputContainerName << " not found, made a new one" << endmsg ;
  }
  else {
    m_HltLumiSummarys = get<LHCb::HltLumiSummarys>(m_OutputContainerName);
    hltLS = *m_HltLumiSummarys->begin();
    debug() << m_OutputContainerName << " found, used the old one" << endmsg ;
  }

  // ------------------------------------------
  // load the track objects
  int nCounter =  0;
  if ( !exist<LHCb::Tracks>(m_InputSelectionName) ){
    error() << m_InputSelectionName << " not found" << endmsg ;
  } else {
    // get the container
    m_InputContainer = get<LHCb::Tracks>(m_InputSelectionName);
  
    if ( !m_InputContainer ) { 
      err() << "Could not find location " 
	    <<  m_InputSelectionName << endreq;
      return StatusCode::FAILURE ;
    }
    nCounter = m_InputContainer->size() ;
    verbose() << "found " << nCounter << " tracks." << endreq ;
  }
  debug() << "There are " << nCounter << " tracks in " << m_InputSelectionName <<  endreq ;


  // ------------------------------------------
  // add track counter
  hltLS->addInfo( m_Counter, nCounter);

  setFilterPassed(true);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LumiCountTracks::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

