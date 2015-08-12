// Include files

#include <vector>

// from Gaudi
#include "OTDet/DeOTDetector.h"
// from TrackEvent
#include "Event/StateParameters.h"
#include "Event/Track.h"

// local
#include "PatSeeding.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatSeeding
//
// 2006-10-13 : Olivier Callot
//-----------------------------------------------------------------------------


// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PatSeeding )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatSeeding::PatSeeding( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_seedingTool(NULL)
  , m_timerTool(NULL)
  , m_seedTime(0)
{
  declareProperty( "OutputTracksName",  m_outputTracksName = LHCb::TrackLocation::Seed     );
  declareProperty( "TimingMeasurement", m_doTiming = false);
}
//=============================================================================
// Destructor
//=============================================================================
PatSeeding::~PatSeeding() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatSeeding::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_seedingTool = tool<IPatSeedingTool>(
		  "PatSeedingTool", "PatSeedingTool", this );

  if ( m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timerTool->increaseIndent();
    m_seedTime = m_timerTool->addTimer( "Internal PatSeeding" );
    m_timerTool->decreaseIndent();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatSeeding::execute()
{
  if ( m_doTiming ) m_timerTool->start( m_seedTime );

  LHCb::Tracks* outputTracks = new LHCb::Tracks();


  m_seedingTool->prepareHits();
  StatusCode sc = m_seedingTool->performTracking(outputTracks);

  put(outputTracks, m_outputTracksName);

  if ( m_doTiming ) m_timerTool->stop( m_seedTime );

  return sc;
}
   
//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatSeeding::finalize()
{
  if( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
