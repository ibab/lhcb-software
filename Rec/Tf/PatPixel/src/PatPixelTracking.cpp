// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/Track.h"

// local
#include "PatPixelTracking.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatPixelTracking
//
// 2011-12-16 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PatPixelTracking )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PatPixelTracking::PatPixelTracking( const std::string& name,
                                      ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "OutputTracksName",      m_outputLocation    = LHCb::TrackLocation::VeloPix );
  declareProperty( "MaxXSlope",             m_maxXSlope         = 0.400 );
  declareProperty( "MaxYSlope",             m_maxYSlope         = 0.300 );
  declareProperty( "MaxZForRBeamCut",       m_maxZForRBeamCut   = 200.* Gaudi::Units::mm );
  declareProperty( "MaxR2Beam",             m_maxR2Beam         = 1. * Gaudi::Units::mm2 );
  declareProperty( "ExtraTol",              m_extraTol          = 0.150 * Gaudi::Units::mm );
  declareProperty( "MaxChi2ToAdd",          m_maxChi2ToAdd      = 100.  );
  declareProperty( "MaxChi2SameSensor",     m_maxChi2SameSensor = 16.   );
  declareProperty( "MaxMissed",             m_maxMissed         = 4     );
  declareProperty( "MaxChi2PerHit",         m_maxChi2PerHit     = 16.0  );
  declareProperty( "MaxChi2Short",          m_maxChi2Short      = 6.0   );

  // Parameters for debugging
  declareProperty( "DebugToolName",         m_debugToolName     = ""    );
  declareProperty( "WantedKey",             m_wantedKey         = -100  );
  declareProperty( "TimingMeasurement",     m_doTiming          = false );
}
//=============================================================================
// Destructor
//=============================================================================
PatPixelTracking::~PatPixelTracking() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatPixelTracking::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hitManager = tool<PatPixelHitManager>("PatPixelHitManager");

  m_debugTool   = 0;
  if ( "" != m_debugToolName ) m_debugTool = tool<IPatDebugTool>( m_debugToolName );

  if ( m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool/Timer", this );
    m_timeTotal   = m_timerTool->addTimer( "Total" );
    m_timerTool->increaseIndent();
    m_timePrepare = m_timerTool->addTimer( "Prepare" );
    m_timePairs   = m_timerTool->addTimer( "Find by pairs" );
    m_timeFinal   = m_timerTool->addTimer( "Store tracks" );
    m_timerTool->decreaseIndent();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatPixelTracking::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  m_isDebug   = msgLevel( MSG::DEBUG   );

  if ( m_doTiming ){
    m_timerTool->start( m_timeTotal );
    m_timerTool->start( m_timePrepare );
  }

  LHCb::Tracks* outputTracks = new LHCb::Tracks();
  put( outputTracks, m_outputLocation );

  m_hitManager->buildHits();
  m_hitManager->sortByX();

  if ( m_isDebug ) {
    for ( unsigned int sensorNb = m_hitManager->firstSensor(); m_hitManager->lastSensor() >= sensorNb; ++sensorNb ) {
      for ( PatPixelHits::const_iterator itH = m_hitManager->hits(sensorNb).begin();
            m_hitManager->hits(sensorNb).end() != itH; ++itH ) {
        printHit( *itH );
      }
    }
  }

  if ( 0 <= m_wantedKey ) {
    info() << "--- Looking for track " << m_wantedKey << endmsg;
    for ( unsigned int sensorNb = m_hitManager->firstSensor(); m_hitManager->lastSensor() >= sensorNb; ++sensorNb ) {
      for ( PatPixelHits::const_iterator itH = m_hitManager->hits(sensorNb).begin();
            m_hitManager->hits(sensorNb).end() != itH; ++itH ) {
        if ( matchKey( *itH ) ) printHit( *itH );
      }
    }
  }

  if ( m_doTiming ) m_timerTool->stop( m_timePrepare );
  //==========================================================================
  // Search by finding a pair, then extrapolating
  //==========================================================================
  if ( m_doTiming ) m_timerTool->start( m_timePairs );
  searchByPair();
  if ( m_doTiming ) m_timerTool->stop( m_timePairs );
  //==========================================================================
  // Final storage of tracks
  //==========================================================================
  if ( m_doTiming ) m_timerTool->start( m_timeFinal );

  makeLHCbTracks( outputTracks );

  if ( m_doTiming ) {
    m_timerTool->stop( m_timeFinal );
    m_timerTool->stop( m_timeTotal );
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatPixelTracking::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=========================================================================
//  Search starting with a pair of consecutive sensors.
//=========================================================================
void PatPixelTracking::searchByPair( ) {
  int lastSensor  = m_hitManager->lastSensor();
  int firstSensor = m_hitManager->firstSensor() + 2;
  for ( int sens0 = lastSensor; firstSensor <= sens0; sens0 -= 1 ) {
    int sens1 = sens0 - 2;
    PatPixelSensor* sensor0 = m_hitManager->sensor( sens0 );
    PatPixelSensor* sensor1 = m_hitManager->sensor( sens1 );
    double z0 = sensor0->z();
    double z1 = sensor1->z();
    double dxMax = m_maxXSlope * fabs( z1 - z0 );
    double dyMax = m_maxYSlope * fabs( z1 - z0 );
    
    PatPixelHits::const_iterator itH0, itH1, itH, first1;
    first1 = sensor1->hits().begin();
    for ( itH0 = sensor0->hits().begin(); sensor0->hits().end() != itH0; ++itH0 ) {
      if ( (*itH0)->isUsed() ) continue;
      double x0  = (*itH0)->x();
      double y0  = (*itH0)->y();
      double xMin = x0 - dxMax;
      double xMax = x0 + dxMax;
      
      if (  0 != m_debugTool && matchKey( *itH0 ) ) {
        info() << format( "s1%3d xMin%9.3f xMax%9.3f ", sens1, xMin, xMax );
        printHit ( *itH0,   "St0" );
      }
      for ( itH1 = first1; sensor1->hits().end() != itH1; ++itH1 ) {
        double x1 = (*itH1)->x();
        if ( x1 < xMin ) {
          first1 = itH1+1;
          continue;
        }
        if ( x1 > xMax ) break;
        if ( (*itH1)->isUsed() ) continue;

        //== Check y compatibility...
        double y1  = (*itH1)->y();
        if ( fabs( y1 - y0 ) > dyMax ) continue;

        m_debug = m_isDebug;
        if ( 0 != m_debugTool ) {
          if ( matchKey( *itH0 ) && matchKey( *itH1 ) ) m_debug = true;
          if ( m_debug ) {
            info() << format( "s1%3d dxRel %7.3f dyRel %7.3f    ", 
                              sens1, (x1-xMin)/(xMax-xMin), fabs((*itH1)->y()-y0)/dyMax );
            printHit( *itH1 );
          }
        }

        m_track.set( *itH0, *itH1 );

        //== Cut on R2Beam if needed : backward tracks, i.e zBeam > first hit
        if ( sensor0->z() < m_maxZForRBeamCut ) {
          double zBeam  = m_track.zBeam();
          if ( zBeam > sensor0->z() ) {
            double r2Beam = m_track.r2AtZ( zBeam ) ;
            if ( m_debug ) info() << format( "  zbeam %9.2f r2 %7.3f", zBeam, r2Beam ) << endmsg;
            if ( r2Beam > m_maxR2Beam ) continue;
          }
        }

        //== Extend downstream, on both sides of the detector as soon as one hit is missed
        int extraStep = 2;
        int extraSens = sens1-extraStep;
        int nbMissed = 0;
        double lastZ = sensor1->z();
        while ( extraSens >= 0 ) {
          PatPixelSensor* extra = m_hitManager->sensor( extraSens );
          double tol     =  m_extraTol;
          double maxChi2 = m_maxChi2ToAdd;
          if ( extra->z() < lastZ - 100. ) {
            tol     = 2 * tol;
            maxChi2 = 2 * maxChi2;
          }
          bool added = addHitsOnSensor( extra, tol, maxChi2 );
          if ( added ) {
            nbMissed = 0;
            lastZ = extra->z();
          } else {
            nbMissed += extraStep;
            extraStep = 1;
          }
          if ( m_maxMissed < nbMissed ) break;
          extraSens -= extraStep;
        }
          
        //== Try upstream if almost forward tracks
        if ( sensor0->z() > m_maxZForRBeamCut ) {
          extraStep = 1;
          extraSens = sens0 + 3;  // + 2 already tried...
          nbMissed = 2;
          while ( extraSens <= lastSensor ) {
            PatPixelSensor* extra = m_hitManager->sensor( extraSens );
            bool added = addHitsOnSensor( extra, m_extraTol, m_maxChi2ToAdd );
            if ( added ) {
              nbMissed = 0;
            } else {
              nbMissed += extraStep;
            }
            if ( m_maxMissed < nbMissed ) break;
            extraSens += extraStep;
          }
        }

        removeWorstHit( m_maxChi2PerHit );
        if ( m_track.hits().size() < 3 ) continue;

        //== Add compatible hits in sens0 and sens1.
        if ( itH0+1 != sensor0->hits().end() ) {
          if ( m_track.chi2( *(itH0+1) ) < m_maxChi2SameSensor ) {
            ++itH0;
            m_track.addHit( *itH0 );
          }
        }
        if ( itH1+1 != sensor1->hits().end() ) {
          if ( m_track.chi2( *(itH1+1) ) < m_maxChi2SameSensor ) {
            ++itH1;
            m_track.addHit( *itH1 );
          }
        }

        //== Final check: if only 3 hits, all should be unused and chi2 good.
        if ( m_track.hits().size() == 3 ) {
          if ( !m_track.all3SensorsAreDifferent() ) {
            if ( m_debug ) {
              info() << "  -- reject, not 3 different sensors." << endmsg;
              printTrack( m_track );
            }
            continue;
          }
          if ( m_track.nbUnused() != 3 ) {
            if ( m_debug ) {
              info() << "  -- reject, only " << m_track.nbUnused() << " unused hits." << endmsg;
              printTrack( m_track );
            }
            continue;
          }
          if ( m_track.chi2() > m_maxChi2Short ) {
            if ( m_debug ) {
              info() << "  -- reject, chi2 " << m_track.chi2() << " too high." << endmsg;
              printTrack( m_track );
            }
            continue;
          }
        } else {
          if ( m_track.nbUnused() < .6 * m_track.hits().size() ) {
            if ( m_debug ) {
              info() << "  -- reject, only " << m_track.nbUnused() << " unused hits." << endmsg;
              printTrack( m_track );
            }
            continue;
          }
        }
        
        m_tracks.push_back( m_track );
        if ( m_debug ) {
          info() << "=== Store track Nb " << m_tracks.size() << " chi2 " << m_track.chi2() << endmsg;
          printTrack( m_track );
        }
        if ( m_track.hits().size() > 3 ) {
          m_track.tagUsedHits();
          break;
        }
      } // itH1
    } // itH0
  } // sens0
}
//=========================================================================
//  Convert the local tracks to LHCb tracks
//=========================================================================
void PatPixelTracking::makeLHCbTracks( LHCb::Tracks* outputTracks ) {
  unsigned int key = 0;
  for ( PatPixelTracks::iterator itT = m_tracks.begin(); m_tracks.end() != itT; ++itT ) {
    if ( (*itT).hits().size() == 3 && (*itT).nbUnused() != 3 ) continue;
    LHCb::Track *newTrack = new LHCb::Track( key++ );
    newTrack->setType( LHCb::Track::Velo );
    newTrack->setHistory( LHCb::Track::PatFastVelo );
    newTrack->setPatRecStatus( LHCb::Track::PatRecIDs );
    if ( m_debug ) {
      info() << "=== Store track Nb " << outputTracks->size() << " chi2 " << (*itT).chi2() << endmsg;
      printTrack( *itT );
    }

    double zMax = -1.e9;

    for ( PatPixelHits::iterator itR = (*itT).hits().begin();
          (*itT).hits().end() != itR; ++itR ) {
      newTrack->addToLhcbIDs( (*itR)->id() );
      if ( (*itR)->z() > zMax ) zMax = (*itR)->z();
    }

    LHCb::State state;

    //== Define backward as z closest to beam downstream of hits
    double zBeam = (*itT).zBeam();
    bool backward = zBeam > zMax;
    newTrack->setFlag( LHCb::Track::Backward, backward );

    state.setLocation( LHCb::State::ClosestToBeam );
    state.setState( (*itT).state( zBeam ) );
    state.setCovariance( (*itT).covariance( zBeam ) );
    newTrack->addToStates( state );
    outputTracks->insert( newTrack );
  }
  m_tracks.clear();
}

//=========================================================================
//  Remove the worst hit until all chi2 are good.
//=========================================================================
void PatPixelTracking::removeWorstHit ( double maxChi2 ) {
  double topChi2 = 1.e9;
  while( topChi2 > maxChi2 ) {
    topChi2 = 0.;
    PatPixelHit* worst = NULL;
    for (  PatPixelHits::const_iterator itH = m_track.hits().begin(); m_track.hits().end() != itH; ++itH ) {
      double myChi2 = m_track.chi2( *itH );
      if ( myChi2 > topChi2 ) {
        topChi2 = myChi2;
        worst   = *itH;
      }
    }
    if ( topChi2 > maxChi2 ) {
      m_track.removeHit( worst );
      if ( m_debug ) {
        info() << "After worst hit removal" << endmsg;
        printTrack( m_track );
      }
    }  
  }
}

//=========================================================================
//  Add hits from the specified sensor to the track
//=========================================================================
bool PatPixelTracking::addHitsOnSensor( PatPixelSensor* sensor, double xTol, double maxChi2 ) {
  if ( 0 == sensor->hits().size() ) return false;
  double xGuess = m_track.xAtZ( sensor->z() ) - xTol - 1.;
  if ( sensor->hits().back()->x() < xGuess ) return false;
  PatPixelHits::const_iterator itStart = sensor->hits().begin();
  
  unsigned int step = sensor->hits().size();
  while ( 2 < step ) {
    step = step/2;
    if ( (*(itStart+step))->x() < xGuess ) itStart += step;
  }
  bool added = false;
  for ( PatPixelHits::const_iterator itH = itStart; sensor->hits().end() != itH; ++itH ) {
    double xPred = m_track.xAtHit( *itH );
    if ( m_debug ) printHitOnTrack( *itH );
    if ( (*itH)->x() + xTol < xPred ) continue;
    if ( (*itH)->x() - xTol > xPred ) break;
    if ( m_track.chi2( *itH ) < maxChi2 ) {
      if ( m_debug ) printHitOnTrack( *itH, false );
      m_track.addHit( *itH );
      added = true;
    }
  }
  return added;
}
//=========================================================================
//  Debug the content of a hit
//=========================================================================
void PatPixelTracking::printHit ( const PatPixelHit* hit, std::string title ) {
  if ( "" != title ) info() << title;
  info() << format( " sensor%3d x%8.3f y%8.3f z%8.2f used%2d",
                    hit->sensor(), hit->x(), hit->y(), hit->z(), hit->isUsed() );
  LHCb::LHCbID myId =  hit->id();
  if ( 0 != m_debugTool ) {
    info() << " MC: ";
    m_debugTool->printKey( info(), myId );
    if ( matchKey( hit ) ) info() << " ***";
  }
  info() << endreq;
}

//=========================================================================
//  Print a track, with distance and chi2
//=========================================================================
void PatPixelTracking::printTrack ( PatPixelTrack& track ) {
  for (  PatPixelHits::const_iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
    info() << format( "Dist%8.3f chi%7.3f ", track.distance( *itH ), track.chi2( *itH ) );
    printHit( *itH );
  }
}

//=========================================================================
//  Print a hit on a track, with its distance.
//=========================================================================
void PatPixelTracking::printHitOnTrack ( PatPixelHit* hit, bool ifMatch ) {
  bool isMatching = matchKey( hit );
  isMatching = (isMatching && ifMatch) || (!isMatching && !ifMatch );
  if ( isMatching ) {
    info() << format( "  sens %3d dx  %9.3f chi2 %8.3f ", hit->sensor(), m_track.xAtHit( hit )-hit->x(), m_track.chi2( hit ) );
    printHit( hit, "   " );
  }
}
//=============================================================================
