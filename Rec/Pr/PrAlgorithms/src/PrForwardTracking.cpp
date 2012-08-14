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
  declareProperty( "DeltaQuality",       m_deltaQuality    = 3.              );
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
  
  m_forwardTool = tool<IPrForwardTool>( m_forwardToolName );

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

  for ( LHCb::Tracks::iterator itT1 = result->begin(); result->end() != itT1; ++itT1 ) {
    LHCb::State* state1 = (*itT1)->stateAt( LHCb::State::AtT );
    for ( LHCb::Tracks::iterator itT2 = itT1+1; result->end() != itT2; ++itT2 ) {
      LHCb::State* state2 = (*itT2)->stateAt( LHCb::State::AtT );
      double dx = state1->x() - state2->x();
      if ( fabs(dx) > 50. ) continue;
      double dy = state1->y() - state2->y();
      if ( fabs(dy) > 100. ) continue;
      
      std::vector<LHCb::LHCbID>::const_iterator itID1 = (*itT1)->lhcbIDs().begin();
      std::vector<LHCb::LHCbID>::const_iterator itID2 = (*itT2)->lhcbIDs().begin();
      while( !(*itID1).isFT() ) ++itID1;
      while( !(*itID2).isFT() ) ++itID2;
      int nCommon = 0;
      int n1 = (*itT1)->lhcbIDs().end() - itID1;
      int n2 = (*itT2)->lhcbIDs().end() - itID2;
      while( itID1 != (*itT1)->lhcbIDs().end() &&
             itID2 != (*itT2)->lhcbIDs().end() ){
        if ( (*itID1) == (*itID2) ) {
          ++nCommon;
          ++itID1;
          ++itID2;
        } else if ( (*itID1) < (*itID2) ) {
          ++itID1;
        } else {
          ++itID2;
        }
      }      
      if ( nCommon > .4 * (n1 + n2 ) ) {
        float q1 = (*itT1)->info( LHCb::Track::PatQuality, 0. );
        float q2 = (*itT2)->info( LHCb::Track::PatQuality, 0. );
        debug() << (*itT1)->key() << " (q=" << q1 << ") and " 
                << (*itT2)->key() << " (q=" << q2 << ") share " 
                << nCommon << " FT hits" << endmsg;
        if ( q1 + m_deltaQuality < q2 ) {
          debug() << "Erase " << (*itT2)->key() << endmsg;
          result->erase( itT2 );
          itT2 = itT1;
        } else if ( q2 + m_deltaQuality < q1 ) {
          debug() << "Erase " << (*itT1)->key() << endmsg;
          result->erase( itT1 );
          itT1 = result->begin();
          itT2 = itT1;
        }   
      }
    }
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
