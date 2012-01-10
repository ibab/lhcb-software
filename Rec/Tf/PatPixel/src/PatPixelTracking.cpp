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
  declareProperty( "MaxYSlope",             m_maxYSlope         = 0.250 );
  declareProperty( "SeedTolerance",         m_seedTolerance     = 0.080 * Gaudi::Units::mm );
  declareProperty( "ExtraTol",              m_extraTol          = 0.150 * Gaudi::Units::mm );
  declareProperty( "MaxChi2ToAdd",          m_maxChi2ToAdd      = 100.  );
  declareProperty( "MaxChi2SameSensor",     m_maxChi2SameSensor = 16.   );
  declareProperty( "MaxMissed",             m_maxMissed         = 4     );
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
    m_timeDist4   = m_timerTool->addTimer( "Dist=4" );
    m_timeDist6   = m_timerTool->addTimer( "Dist=6" );
    m_timeDist7   = m_timerTool->addTimer( "Dist=7" );
    m_timeDist5   = m_timerTool->addTimer( "Dist=5" );
    m_timeDist3   = m_timerTool->addTimer( "Dist=3" );
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
  // Search for groups with given distance
  //==========================================================================
  if ( m_doTiming ) m_timerTool->start( m_timeDist4 );
  searchTracks( 4 );
  if ( m_doTiming ) m_timerTool->stop(  m_timeDist4 );
  
  if ( m_doTiming ) m_timerTool->start( m_timeDist6 );
  searchTracks( 6 );
  if ( m_doTiming ) m_timerTool->stop(  m_timeDist6 );

  if ( m_doTiming ) m_timerTool->start( m_timeDist7 );
  searchTracks( 7 );
  if ( m_doTiming ) m_timerTool->stop(  m_timeDist7 );

  if ( m_doTiming ) m_timerTool->start( m_timeDist5 );
  searchTracks( 5 );
  if ( m_doTiming ) m_timerTool->stop(  m_timeDist5 );

  if ( m_doTiming ) m_timerTool->start( m_timeDist3 );
  searchTracks( 3 );
  if ( m_doTiming ) m_timerTool->stop(  m_timeDist3 );
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
//  Search with a given distance in the seed part
//=========================================================================
void PatPixelTracking::searchTracks( int length ) {
  int lastSensor  = m_hitManager->lastSensor();
  int firstSensor = m_hitManager->firstSensor() + length;
  int step = 1;
  if ( (length%2) == 0 ) step = 2;
  for ( int sens0 = lastSensor; firstSensor <= sens0; sens0 -= 1 ) {
    if ( length > 6 && (sens0%2) == 0 ) continue;  // Long: Only one gap in the seed part -> sens0 must be odd
    if ( length < 4 && (sens0%2) != 0 ) continue;  // Short: only if sens0 is even, and sens1 odd.
    PatPixelSensor* sensor0 = m_hitManager->sensor( sens0 );
    int sens1 = sens0 - length;
    PatPixelSensor* sensor1 = m_hitManager->sensor( sens1 );
    double z0 = sensor0->z();
    double z1 = sensor1->z();
    double dxMax = m_maxXSlope * fabs( z1 - z0 );
    double dyMax = m_maxYSlope * fabs( z1 - z0 );

    std::vector<PatPixelHits::const_iterator> itBegin( sens0 );
    std::vector<PatPixelHits::const_iterator> itFirst( sens0 );
    for ( int sensor = sens0-1 ; sens1 < sensor ; --sensor ) {
      itBegin[sensor] = m_hitManager->sensor(sensor)->hits().begin();
      itFirst[sensor] = itBegin[sensor];
    }

    PatPixelHits::const_iterator itH0, itH1, itH, first1;
    first1 = sensor1->hits().begin();
    for ( itH0 = sensor0->hits().begin(); sensor0->hits().end() != itH0; ++itH0 ) {
      if ( (*itH0)->isUsed() ) continue;
      double x0 = (*itH0)->x();
      double y0 = (*itH0)->y();
      double zh0 = (*itH0)->z();
      double xMin = x0 - dxMax;
      double xMax = x0 + dxMax;

      if (  0 != m_debugTool && matchKey( *itH0 ) ) {
        info() << format( "L=%1d: s1%3d xMin%9.3f xMax%9.3f ", length, sens1, xMin, xMax );
        printHit ( *itH0,   "St0" );
      }
      itFirst = itBegin;
      for ( itH1 = first1; sensor1->hits().end() != itH1; ++itH1 ) {
        if ( (*itH1)->isUsed() ) continue;
        double x1 = (*itH1)->x();
        if ( x1 < xMin ) {
          first1 = itH1+1;
          continue;
        }
        if ( x1 > xMax ) break;
        double y1  = (*itH1)->y();
        double zh1 = (*itH1)->z();
        //== Check y compatibility...
        if ( fabs( (*itH1)->y() - (*itH0)->y() ) > dyMax ) continue;

        m_debug = m_isDebug;
        if ( 0 != m_debugTool ) {
          if ( matchKey( *itH0 ) && matchKey( *itH1 ) ) m_debug = true;
          if ( m_debug ) {
            info() << "                                          ";
            printHit( *itH1 );
          }
        }
        double slx = (x1-x0)/(zh1-zh0);
        double xAt0 = x0 - slx*zh0;
        double sly = (y1-y0)/(zh1-zh0);
        double yAt0 = y0 - sly*zh0;
        PatPixelHits temp;
        for ( int sensor = sens0-step ; sens1 < sensor ; sensor -= step ) {
          PatPixelSensor* testSensor = m_hitManager->sensor( sensor );
          for ( itH = itFirst[sensor]; testSensor->hits().end() != itH; ++itH ) {
            double zh = (*itH)->z();
            double xPred = xAt0 + zh * slx;
            double x = (*itH)->x();
            if ( x < xPred - m_seedTolerance ) {
              if ( itH1 == first1 ) itBegin[sensor] = itH + 1;
              itFirst[sensor] = itH+1;
              continue;
            }
            if ( x > xPred + m_seedTolerance ) break;
            double yPred = yAt0 + zh * sly;
            if ( fabs( (*itH)->y()-yPred ) > m_seedTolerance ) continue;
            if (  m_debug && matchKey( *itH ) ) {
              info() << format( "L=%1d: s %3d xPred%8.3f yPred%8.3f ", length, sensor, xPred, yPred );
              printHit( *itH, "   " );
            }
            temp.push_back( *itH );
          }
        }
        if ( temp.size() > 0 ) {
          PatPixelTrack tmp;
          tmp.addHit( *itH0 );
          tmp.addHit( *itH1 );
          for ( itH = temp.begin(); temp.end() != itH; ++itH ) {
            tmp.addHit( *itH );
          }
          if ( itH0+1 != sensor0->hits().end() ) {
            if ( tmp.chi2( *(itH0+1) ) < m_maxChi2SameSensor ) {
              ++itH0;
              tmp.addHit( *itH0 );
            }
          }
          if ( itH1+1 != sensor1->hits().end() ) {
            if ( tmp.chi2( *(itH1+1) ) < m_maxChi2SameSensor ) {
              ++itH1;
              tmp.addHit( *itH1 );
            }
          }

          bool hasChangedSide = (step == 1);

          int extraSens = sens1-1;
          if ( hasChangedSide ) --extraSens;
          int nbMissed = 0;
          while ( extraSens >= 0 ) {
            PatPixelSensor* extra = m_hitManager->sensor( extraSens );
            bool added = false;
            for ( PatPixelHits::const_iterator itH = extra->hits().begin();
                  extra->hits().end() != itH; ++itH ) {
              double xPred = tmp.xAtHit( *itH );
              if ( m_debug && matchKey( *itH ) ) {
                info() << format( "  sens %3d delta5%7.3f chi2 %8.3f ", extraSens, xPred-(*itH)->x(), tmp.chi2( *itH ) );
                printHit( *itH, "   " );
              }
              if ( (*itH)->x() + m_extraTol < xPred ) continue;
              if ( (*itH)->x() - m_extraTol > xPred ) break;
              if ( tmp.chi2( *itH ) < m_maxChi2ToAdd ) {
                tmp.addHit( *itH );
                added = true;
                nbMissed = 0;
                if ( m_debug ) printHit( *itH, "Add" );
              }
            }
            if ( !added ) {
              nbMissed++;
              if ( hasChangedSide ) nbMissed++;
            }
            if ( added  && !hasChangedSide ) {
              if ( extraSens%2 != sens0%2 ) hasChangedSide = true;
            }
            if ( m_maxMissed < nbMissed ) break;
            --extraSens;
            if ( hasChangedSide ) --extraSens;
          }
          if ( tmp.hits().size() == 3 ) {
            if ( tmp.nbUnused() != 3 ) {
              if ( m_debug ) info() << "  -- reject, only " << tmp.nbUnused() << " unused hits." << endmsg;
              continue;
            }
            if ( tmp.chi2() > m_maxChi2Short ) {
              if ( m_debug ) info() << "  -- reject, chi2 " << tmp.chi2() << " too high." << endmsg;
              continue;
            }
          } else {
            tmp.tagUsedHits();
          }
          m_tracks.push_back( tmp );
        }
      }
    }
  }
}
//=========================================================================
//  Convert the local tracks to LHCb tracks
//=========================================================================
void PatPixelTracking::makeLHCbTracks( LHCb::Tracks* outputTracks ) {
  for ( PatPixelTracks::iterator itT = m_tracks.begin(); m_tracks.end() != itT; ++itT ) {
    if ( (*itT).hits().size() == 3 && (*itT).nbUnused() != 3 ) continue;
    LHCb::Track *newTrack = new LHCb::Track();
    newTrack->setType( LHCb::Track::Velo );
    newTrack->setHistory( LHCb::Track::PatFastVelo );
    newTrack->setPatRecStatus( LHCb::Track::PatRecIDs );
    if ( m_debug ) {
      info() << "=== Store track Nb " << outputTracks->size() << " chi2 " << (*itT).chi2() << endmsg;
      printHits( (*itT).hits() );
    }

    double zMin = 1.e9;
    double zMax = -1.e9;

    for ( PatPixelHits::iterator itR = (*itT).hits().begin();
          (*itT).hits().end() != itR; ++itR ) {
      newTrack->addToLhcbIDs( (*itR)->id() );
      if ( (*itR)->z() > zMax ) zMax = (*itR)->z();
      if ( (*itR)->z() < zMin ) zMin = (*itR)->z();
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

    if ( !backward ) {
      state.setLocation( LHCb::State::EndVelo );
      state.setState( (*itT).state( zMax ) );
      state.setCovariance( (*itT).covariance( zMax ) );
      newTrack->addToStates( state );
    }

    outputTracks->insert( newTrack );
  }
  m_tracks.clear();
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
//
//=========================================================================
void PatPixelTracking::printHits ( PatPixelHits& hits, std::string title ) {
  if ( "" != title ) info() << "=== " << title << " ===" << endmsg;
  for ( PatPixelHits::const_iterator itH = hits.begin(); hits.end() != itH; ++itH ) {
    printHit( *itH );
  }
}
//=============================================================================
