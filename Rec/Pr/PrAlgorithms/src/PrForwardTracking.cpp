// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"

// local
#include "PrForwardTracking.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrForwardTracking
//
// 2012-03-20 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PrForwardTracking )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrForwardTracking::PrForwardTracking( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName",          m_inputName       = LHCb::TrackLocation::Velo    );
  declareProperty( "OutputName",         m_outputName      = LHCb::TrackLocation::Forward );
  declareProperty( "HitManagerName",     m_hitManagerName  = "PrFTHitManager" );
  declareProperty( "ForwardToolName",    m_forwardToolName = "PrForwardTool" );
  // Parameters for debugging
  declareProperty( "DebugToolName"     , m_debugToolName  = ""        );
  declareProperty( "WantedKey"         , m_wantedKey      = -100      );
  declareProperty( "VeloKey"           , m_veloKey        = -100      );
  declareProperty( "TimingMeasurement" , m_doTiming       = false     );
}
//=============================================================================
// Destructor
//=============================================================================
PrForwardTracking::~PrForwardTracking() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrForwardTracking::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hitManager = tool<PrHitManager>( m_hitManagerName );
  m_hitManager->buildGeometry();
  
  m_forwardTool = tool<PrForwardTool>( m_forwardToolName );

  m_debugTool   = 0;
  if ( "" != m_debugToolName ) {
    m_debugTool = tool<IPrDebugTool>( m_debugToolName );
  } else {
    m_wantedKey = -100;  // no debug
  }
  m_forwardTool->setDebugParams( m_debugTool, m_wantedKey, m_veloKey );

  if ( m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool/Timer", this );
    m_timeTotal   = m_timerTool->addTimer( "PrForward total" );
    m_timerTool->increaseIndent();
    m_timePrepare = m_timerTool->addTimer( "PrForward prepare" );
    m_timeExtend  = m_timerTool->addTimer( "PrForward extend" );
    m_timeFinal   = m_timerTool->addTimer( "PrForward final" );
    m_timerTool->decreaseIndent();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrForwardTracking::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  if ( m_doTiming ) {
    m_timerTool->start( m_timeTotal );
    m_timerTool->start( m_timePrepare );
  }
    
  LHCb::Tracks* result = new LHCb::Tracks();
  put( result, m_outputName );

  m_hitManager->decodeData();

  //== If needed, debug the cluster associated to the requested MC particle.
  if ( 0 <= m_wantedKey ) {
    info() << "--- Looking for MCParticle " << m_wantedKey << endmsg;
    for ( unsigned int zone = 0; m_hitManager->nbZones() > zone; ++zone ) {
      for ( PrHits::const_iterator itH = m_hitManager->hits( zone ).begin();
            m_hitManager->hits( zone ).end() != itH; ++itH ) {
        if ( m_forwardTool->matchKey( *itH ) ) m_forwardTool->printHit( *itH, " " );
      }
    }
  }

  //============================================================
  //== Main processing: Extend selected tracks
  //============================================================
  if ( m_doTiming ) {
    m_timerTool->stop( m_timePrepare );
    m_timerTool->start( m_timeExtend );
  }

  LHCb::Tracks* velo = get<LHCb::Tracks>( m_inputName );
  for ( LHCb::Tracks::iterator itV = velo->begin(); velo->end() != itV; ++itV ) {
    m_forwardTool->extendTrack( *itV, result );
  }
  
  //============================================================
  //== Final processing: filtering of duplicates,...
  //============================================================
  if ( m_doTiming ) {
    m_timerTool->stop( m_timeExtend );
    m_timerTool->start( m_timeFinal );
  }

  if ( m_doTiming ) {
    m_timerTool->stop( m_timeFinal );
    m_timerTool->stop( m_timeTotal );
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrForwardTracking::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
