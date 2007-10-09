// $Id: PatForward.cpp,v 1.1.1.1 2007-10-09 18:23:10 smenzeme Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "PatForward.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatForward
//
// 2005-04-01 : Olivier Callot
//-----------------------------------------------------------------------------


DECLARE_ALGORITHM_FACTORY( PatForward );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatForward::PatForward( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{ 
  declareProperty( "InputTracksName",   m_inputTracksName  = LHCb::TrackLocation::Velo );
  declareProperty( "OutputTracksName",  m_outputTracksName = LHCb::TrackLocation::Forward);
  declareProperty( "TrackSelectorName", m_trackSelectorName = "None");

  if ( "Hlt" == context() ) {
    m_inputTracksName  =  LHCb::TrackLocation::HltVelo;
    m_outputTracksName =  LHCb::TrackLocation::HltForward;
  }
  declareProperty( "DeltaNumberInT",   m_deltaNumberInT  = 3 );
  declareProperty( "DeltaNumberInTT",  m_deltaNumberInTT = 1 );
  declareProperty( "DoCleanUp", m_doClean = true);
}
//=============================================================================
// Destructor
//=============================================================================
PatForward::~PatForward() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatForward::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;


  m_trackSelector = NULL;
  if (m_trackSelectorName != "None") {
    m_trackSelector = tool<ITrackSelector>(m_trackSelectorName, this);
  }

  m_forwardTool = tool<IPatForwardTool>( "PatForwardTool" );

  if ( msgLevel( MSG::DEBUG ) ) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timerTool->increaseIndent();
    m_fwdTime = m_timerTool->addTimer( "Internal PatForward" );
    m_timerTool->decreaseIndent();
  }

  m_tHitManager    = tool<Tf::TStationHitManager <PatForwardHit> >("PatTStationHitManager");

  return StatusCode::SUCCESS;
};

//=========================================================================
//  Check if a track should be processed. Default flags, and selector if defined
//=========================================================================
bool PatForward::acceptTrack(const LHCb::Track& track) {
  bool ok = !(track.checkFlag( LHCb::Track::Invalid) );
  ok = ok && (!(track.checkFlag( LHCb::Track::Backward) ));
  if (m_trackSelector) ok = ok && (m_trackSelector->accept(track));

  verbose() << "For track " << track.key() << " accept flag =" << ok << endreq;
  return ok;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatForward::execute() {

  m_tHitManager->prepareHits(); 
        
  //== Prepare tracks
 
  LHCb::Tracks* inputTracks   = get<LHCb::Tracks>( m_inputTracksName ); 
    
  LHCb::Tracks* outputTracks  = new LHCb::Tracks();
  outputTracks->reserve(200);
  put(outputTracks, m_outputTracksName);

  debug() << "==> Execute" << endmsg;

  if ( msgLevel( MSG::DEBUG ) ) m_timerTool->start( m_fwdTime );

  int oriSize = outputTracks->size();
  LHCb::Tracks::iterator itT;

  for ( itT = inputTracks->begin(); inputTracks->end() != itT;  ++itT ) {
    LHCb::Track* seed = (*itT);
    if ( acceptTrack(*seed) ) {
      int prevSize = outputTracks->size();
      m_forwardTool->forwardTrack(seed, outputTracks );
      debug()  << " track " << seed->key()
               << " position " << seed->position()
               << " slopes " << seed->slopes()  
	       << " cov \n" << seed->firstState().covariance() << "\n"
               << " produced " << outputTracks->size() - prevSize
               << " tracks " << endmsg;

    }
  }

  if (!m_doClean) {
    if ( msgLevel( MSG::DEBUG ) ) {
      double t = m_timerTool->stop( m_fwdTime );
      debug() << "=== In total, produced " 
	      << outputTracks->size() - oriSize  << " tracks from "
	      << inputTracks->size() << " Velo tracks in " << t << " ms" 
	      << endreq;
    }
    return StatusCode::SUCCESS;
  }


  //== Try to filter from T station part
 
  LHCb::Tracks::iterator itT1;
  std::vector<LHCb::LHCbID>::const_iterator itId0, itId1;

  for ( itT = outputTracks->begin(); outputTracks->end() != itT;  ++itT ) {
    LHCb::State& state0 = (*itT)->stateAt( LHCb::State::AtT );
    for ( itT1 = itT+1; outputTracks->end() > itT1;  ++itT1 ) {
      LHCb::State& state1 = (*itT1)->stateAt( LHCb::State::AtT );
      if ( 5. > fabs( state0.x() - state1.x() ) ) {
        int nbCommon = 0;
        int nb0      = 0;
        int nbTT0    = 0;
        int nbTT1    = 0;
        int nb1      = 0;
        for ( itId1 = (*itT1)->lhcbIDs().begin(); (*itT1)->lhcbIDs().end() != itId1; ++itId1 ) {
          if ( (*itId1).isVelo() ) continue;
          if ( (*itId1).isTT()   ) {
            ++nbTT1;
          } else {
            ++nb1;
          }
        }

        for ( itId0 = (*itT)->lhcbIDs().begin(); (*itT)->lhcbIDs().end() != itId0; ++itId0 ) {
          if ( (*itId0).isVelo() ) continue;
          if ( (*itId0).isTT()   ) {
            ++nbTT0;
            continue;
          }
          nb0++;
          for ( itId1 = (*itT1)->lhcbIDs().begin(); (*itT1)->lhcbIDs().end() != itId1; ++itId1 ) {
            if ( *itId0 == *itId1 ) ++nbCommon;
          }
        }
        if ( .7 * nb0 > nbCommon || .7 * nb1 > nbCommon ) continue;
        if ( msgLevel ( MSG::DEBUG ) ) {
          debug() << format( "Track %3d and %3d nT0 %2d nT1 %2d nCommon %2d nbTT : %2d, %2d  chi20 %8.3f 1 %8.3f",
                             (*itT)->key(), (*itT1)->key(), nb0, nb1, nbCommon, nbTT0, nbTT1,
                             (*itT)->chi2PerDoF(), (*itT1)->chi2PerDoF() ) << endreq;
        }
        if ( nb0   > nb1   + m_deltaNumberInT  ||
             nbTT0 > nbTT1 + m_deltaNumberInTT   ) {
          debug() << "    erase " << (*itT1)->key() << " and restart " << endreq;
          outputTracks->erase( itT1 );
          itT  = outputTracks->begin();
          itT1 = itT;
          break;
        }
        if ( nb1   > nb0   + m_deltaNumberInT  ||
             nbTT1 > nbTT0 + m_deltaNumberInTT    ) {
          debug() << "    erase " << (*itT)->key() << " and restart " << endreq;
          outputTracks->erase( itT );
          itT  = outputTracks->begin();
          itT1 = itT;
          break;
        }
      }
    }
  }

  if ( msgLevel( MSG::DEBUG ) ) {
    double t = m_timerTool->stop( m_fwdTime );
    debug() << "=== In total, produced " <<  outputTracks->size() - oriSize  << " tracks from "
            << inputTracks->size() << " Velo tracks in " << t << " ms"
            << endreq;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
