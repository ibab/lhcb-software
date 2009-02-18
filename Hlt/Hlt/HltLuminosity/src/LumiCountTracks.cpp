// $Id: LumiCountTracks.cpp,v 1.7 2009-02-18 13:11:13 panmanj Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

#include "Event/Track.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"

// local
#include "LumiCountTracks.h"

using namespace LHCb;

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
  m_Counter = LHCb::LumiCounters::counterKeyToType(m_CounterName);
  if ( m_Counter == LHCb::LumiCounters::Unknown ) {
    info() << "LumiCounter not found with name: " << m_CounterName <<  endmsg;
  } else {
    info() << m_CounterName << " key value: " << m_Counter << endmsg;
  }
  // ------------------------------------------
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiCountTracks::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // load the track objects
  int nCounter =  0;
  if ( !exist<LHCb::Tracks>(m_InputSelectionName) ){
    warning() << m_InputSelectionName << " not found" << endmsg ;
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


  // get container
  LHCb::HltLumiSummary* sums = getOrCreate<HltLumiSummary,HltLumiSummary>(m_OutputContainerName);
  // add track counter
  sums->addInfo( m_Counter, nCounter);

  setFilterPassed(true);

  return StatusCode::SUCCESS;
}

