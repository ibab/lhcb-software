// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/Track.h"
#include "Event/StateParameters.h"

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
#ifdef DEBUG_HISTO
    : GaudiTupleAlg(name, pSvcLocator)
#else
    : GaudiAlgorithm ( name , pSvcLocator )
#endif

{
  declareProperty( "OutputTracksName",      m_outputLocation    = LHCb::TrackLocation::Velo );
  declareProperty( "MaxXSlope",             m_maxXSlope         = 0.400 );                     // 0.400
  declareProperty( "MaxYSlope",             m_maxYSlope         = 0.400 );                     // 0.300
  declareProperty( "MaxZForRBeamCut",       m_maxZForRBeamCut   = 200.0 * Gaudi::Units::mm );  // 
  declareProperty( "MaxR2Beam",             m_maxR2Beam         =   1.0 * Gaudi::Units::mm2 ); // 1.0 // limit on squared distance to the beam axis
  declareProperty( "ExtraTol",              m_extraTol          = 0.200 * Gaudi::Units::mm );  // 0.150 // (initial) extrapolation tolerance when adding hits
  declareProperty( "MaxChi2ToAdd",          m_maxChi2ToAdd      = 60.0 );  // 100.0 // (initial) limit on track chi2 when adding new hits
  declareProperty( "MaxChi2SameSensor",     m_maxChi2SameSensor = 12.0  ); // 16.0 // limit when adding hits from sensor that already has hits ?
  declareProperty( "MaxMissed",             m_maxMissed         = 4     );
  declareProperty( "MaxChi2PerHit",         m_maxChi2PerHit     = 12.0  ); // 16.0 // limit when removing worst hits
  declareProperty( "MaxChi2Short",          m_maxChi2Short      =  6.0  ); //  6.0 // short (3-hit) tracks are removed with this limit
  declareProperty( "ClearHits",             m_clearHits         = false ); // Clear hits if needed, for a rerun in same event
  // Parameters for debugging
  declareProperty( "DebugToolName",         m_debugToolName     = ""    );
  declareProperty( "WantedKey",             m_wantedKey         = -100  );
  declareProperty( "TimingMeasurement",     m_doTiming          = false );
  declareProperty( "ClosestToBeamKalmanFit",  m_closestToBeamKalmanFit  = true ) ;
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
#ifdef DEBUG_HISTO
  setHistoTopDir("VP/");
#endif
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatPixelTracking::execute() {

  // printf("PatPixelTracking::execute() =>\n");

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  m_isDebug   = msgLevel( MSG::DEBUG );
  // m_isDebug = 1;

  if ( m_doTiming ){
    m_timerTool->start( m_timeTotal );
    m_timerTool->start( m_timePrepare );
  }

  LHCb::Tracks* outputTracks = new LHCb::Tracks();
  put( outputTracks, m_outputLocation );

  if ( m_clearHits ) m_hitManager->clearHits();
  m_hitManager->buildHits();                                        // import hits from VeloLite Clusters
  m_hitManager->sortByX();                                          // sort by X-pos. within each sensor for faster search
  // printf(" m_hitManager->nbHits() => %d\n", m_hitManager->nbHits());
#ifdef DEBUG_HISTO
  plot(m_hitManager->nbHits(), "HitsPerEvent", "PatPixelTracking: Number of hits per event", 0.0, 4000.0, 50);
#endif

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
  searchByPair();                                                    // search by attempting to extrapolate every pair (withtn slope limits)
  if ( m_doTiming ) m_timerTool->stop( m_timePairs );
  //==========================================================================
  // Final storage of tracks
  //==========================================================================
  if ( m_doTiming ) m_timerTool->start( m_timeFinal );

  makeLHCbTracks( outputTracks );                                    // convert out tracks to LHCb tracks

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
void PatPixelTracking::searchByPair() {
  // printf("PatPixelTracking::searchByPair() =>\n");
  int lastSensor  = m_hitManager->lastSensor();
  int firstSensor = m_hitManager->firstSensor() + 2;
  for ( int sens0 = lastSensor; firstSensor <= sens0; sens0 -= 1 ) {  // loop over sensors, start with the last sensor
    int sens1 = sens0 - 2;                                            // pick-up the "paired" sensor as one full station backwards
    PatPixelSensor* sensor0 = m_hitManager->sensor( sens0 );          // the sensor pair
    PatPixelSensor* sensor1 = m_hitManager->sensor( sens1 );
    double z0 = sensor0->z();                                         // Zet's of the sensor pair
    double z1 = sensor1->z();
    double dxMax = m_maxXSlope * fabs( z1 - z0 );                     // limits on dX/dY from the slope limit
    double dyMax = m_maxYSlope * fabs( z1 - z0 );

    PatPixelHits::const_iterator itH0, itH1, itH, first1;
    first1 = sensor1->hits().begin();
    for ( itH0 = sensor0->hits().begin(); sensor0->hits().end() != itH0; ++itH0 )   // loop over hits in the first sensor in the pair
    { if ( (*itH0)->isUsed() ) continue;                                            // skip hits already in use by other tracks
      double x0  = (*itH0)->x();
      double y0  = (*itH0)->y();
      double xMin = x0 - dxMax;                                                     // X-pos. limits for the other sensor
      double xMax = x0 + dxMax;

      if (  0 != m_debugTool && matchKey( *itH0 ) )
      { info() << format( "s1%3d xMin%9.3f xMax%9.3f ", sens1, xMin, xMax );
        printHit ( *itH0,   "St0" ); }

      for ( itH1 = first1; sensor1->hits().end() != itH1; ++itH1 ) {                 // loop over hits in the second sensor in the pair
        double x1 = (*itH1)->x();
        if ( x1 < xMin ) { first1 = itH1+1; continue; }                              // skip hits below the X-pos. limit
        if ( x1 > xMax ) break;                                                      // stop search when above the X-pos. limit
        if ( (*itH1)->isUsed() ) continue;                                           // skip hits already assigned to tracks

        //== Check y compatibility...
        double y1  = (*itH1)->y();
        if ( fabs( y1 - y0 ) > dyMax ) continue;                                     // skip hits out of Y-pos. limit

        m_debug = m_isDebug;
        if ( 0 != m_debugTool ) {
          if ( matchKey( *itH0 ) && matchKey( *itH1 ) ) m_debug = true;
          if ( m_debug ) {
            info() << format( "s1%3d dxRel %7.3f dyRel %7.3f    ", 
                              sens1, (x1-xMin)/(xMax-xMin), fabs((*itH1)->y()-y0)/dyMax );
            printHit( *itH1 );
          }
        }

        m_track.set( *itH0, *itH1 );  // make a track out of the two hits that pass slope criteria and use it as a seed

        //== Cut on R2Beam if needed : backward tracks, i.e zBeam > first hit
        if ( sensor0->z() < m_maxZForRBeamCut )                // if 1st sensor below the Z-limit
        { double zBeam  = m_track.zBeam();                     // where (which Z) the track passes closest to the beam
          if ( zBeam > sensor0->z() )                          // 
          { double r2Beam = m_track.r2AtZ( zBeam ) ;           // how close the track passes to the beam ?
            if ( m_debug ) info() << format( "  zbeam %9.2f r2 %7.3f", zBeam, r2Beam ) << endmsg;
            if ( r2Beam > m_maxR2Beam ) continue;
          }
        }

        //== Extend downstream, on both sides of the detector as soon as one hit is missed
        int extraStep = 2;                                              // scan every 2nd sensor to stay on the same side (right or left)
        int extraSens = sens1-extraStep;                                // start two sensors behind the 2nd seed sensor
        int nbMissed = 0;
        double lastZ = sensor1->z();                                    // last-Z on a track
        while ( extraSens >= 0 )                                        // loop over sensors before the seed
        { PatPixelSensor* extra = m_hitManager->sensor( extraSens );    // 
          double tol     = m_extraTol;
          double maxChi2 = m_maxChi2ToAdd;
          if ( extra->z() < (lastZ - 100.0) )                           // double the tolerances if sensor more than 100 mm away
          { tol     = 2 * tol;
            maxChi2 = 2 * maxChi2; }
          bool added = addHitsOnSensor( extra, tol, maxChi2 );          // attempt to add new hits from this sensor
          if ( added ) { nbMissed = 0; lastZ = extra->z(); }            // if some hits added: 
                  else { nbMissed += extraStep; extraStep = 1; }        // if none added: count missed sensors
          if ( m_maxMissed < nbMissed ) break;
          extraSens -= extraStep; }

        //== Try upstream if almost forward tracks
        if ( sensor0->z() > m_maxZForRBeamCut )
        { extraStep = 1;
          extraSens = sens0 + 3;  // + 2 already tried...
          nbMissed = 2;
          while ( extraSens <= lastSensor )
          { PatPixelSensor* extra = m_hitManager->sensor( extraSens );
            bool added = addHitsOnSensor( extra, m_extraTol, m_maxChi2ToAdd );
            if ( added ) { nbMissed = 0; }
                    else { nbMissed += extraStep; }
            if ( m_maxMissed < nbMissed ) break;
            extraSens += extraStep;
          }
        }

        removeWorstHit( m_maxChi2PerHit );
        if ( m_track.hits().size() < 3 ) continue;
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

        // printf(" Track #%3d: %2d/%2d hists, Chi2=%3.1f zBeam=%+6.1f\n",
        //       m_tracks.size(), m_track.hits().size(), m_track.nbUnused(), m_track.chi2(), m_track.zBeam() );
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

  // printf("PatPixelTracking::searchByPair() => %3d tracks\n", m_tracks.size());
}

//=========================================================================
//  Convert the local tracks to LHCb tracks
//=========================================================================
void PatPixelTracking::makeLHCbTracks( LHCb::Tracks* outputTracks ) {
  // printf("PatPixelTracking::makeLHCbTracks() =>\n");
#ifdef DEBUG_HISTO
  int Fwd_Count=0; int Bwd_Count=0;
#endif
  unsigned int key = 0;
  for ( PatPixelTracks::iterator itT = m_tracks.begin(); m_tracks.end() != itT; ++itT ) {
    if ( (*itT).hits().size() == 3 && (*itT).nbUnused() != 3 ) continue;          // avoid 3-hit tracks with double-used hits ?
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
          (*itT).hits().end() != itR; ++itR ) {               // loop over hits
      newTrack->addToLhcbIDs( (*itR)->id() );
      if ( (*itR)->z() > zMax ) zMax = (*itR)->z();           // find highest Z for all hits of this track
    }

    LHCb::State state;

    //== Define backward as z closest to beam downstream of hits
    double zBeam = (*itT).zBeam();                            // Z where the track passes closest to the beam
    bool backward = zBeam > zMax;                             // decide: forward or backward track
    newTrack->setFlag( LHCb::Track::Backward, backward );

    // set the state closest to beam
    state.setLocation( LHCb::State::ClosestToBeam );
    state.setState( (*itT).state( zBeam ) );
    if( m_closestToBeamKalmanFit ) {
      // run a K-filter with scattering to improve IP resolution
      double tx = state.tx() ; double ty = state.ty() ;
      // calibrated on MC, shamelessly hardcoded:
      double scat2 = 1e-8 + 7e-6*(tx*tx+ty*ty) ;
      (*itT).fitKalman( state, backward ? 1 : -1 , scat2 ) ;
    } else {
      state.setCovariance( (*itT).covariance( zBeam ) );
    }
    newTrack->addToStates( state );
    
    // set state at the end of the velo from unweighted straight line fit
    if( !backward ) {
      state.setLocation( LHCb::State::EndVelo ) ;
      state.setState( (*itT).state( StateParameters::ZEndVelo ) ) ;
      state.setCovariance( (*itT).covariance( state.z() ) );
      newTrack->addToStates( state );
    }
    
    // set the chi2/dof
    newTrack->setNDoF(2*((*itT).hits().size()-2)); newTrack->setChi2PerDoF((*itT).chi2());
    outputTracks->insert( newTrack );

    // printf(" pseudoRapidity=%3.1f Chi2/DoF=%3.1f nDoF=%d\n",
    //  newTrack->pseudoRapidity(), newTrack->chi2PerDoF(), newTrack->nDoF() );

#ifdef DEBUG_HISTO
    int HitsPerTrack = (*itT).hits().size();
    if(backward)
    {
    plot(HitsPerTrack,                               "Bwd_HitsPerTrack", "PatPixelTracking: Number of hits per backward track",
         0.5, 40.5, 40);
    plot(newTrack->chi2PerDoF(),                     "Bwd_Chi2PerTrack", "PatPixelTracking: Chi2/DoF of backward tracks",
         0.0, 10.0, 50);
    plot(newTrack->pseudoRapidity(),                 "Bwd_EtaOfTracks", "PatPixelTracking: pseudoRapidity of backward tracks",
         1.0, 6.0, 50);
    plot(newTrack->phi()*(180.0/M_PI),               "Bwd_PhiOfTracks", "PatPixelTracking: Phi-angle of backward tracks",
         -180.0, 180.0, 60);
    plot2D(newTrack->pseudoRapidity(), HitsPerTrack, "Bwd_HitsPerTrackVsEta", "PatPixelTracking: hits/track vs pseudoRapidity of backward tracks",
           1.0, 6.0, 0.5, 15.5, 50, 15);
    plot2D(newTrack->pseudoRapidity(), newTrack->chi2PerDoF(), "Bwd_Chi2VsEta", "PatPixelTracking: Chi2/DoF vs pseudoRapidity of backward tracks",
           1.0, 6.0, 0.0, 10.0, 50, 20);
    plot2D(HitsPerTrack, newTrack->chi2PerDoF(),     "Bwd_Chi2VsHitsPerTrack", "PatPixelTracking: Chi2/DoF vs hits/backward track",
           0.5, 15.5, 0.0, 10.0, 15, 20);
    Bwd_Count++;
    }
    else
    {
    plot(HitsPerTrack,                               "Fwd_HitsPerTrack", "PatPixelTracking: Number of hits per forward track",
         0.5, 40.5, 40);
    plot(newTrack->chi2PerDoF(),                     "Fwd_Chi2PerTrack", "PatPixelTracking: Chi2/DoF of forward tracks",
         0.0, 10.0, 50);
    plot(newTrack->pseudoRapidity(),                 "Fwd_EtaOfTracks", "PatPixelTracking: pseudoRapidity of forward tracks",
         1.0, 6.0, 50);
    plot(newTrack->phi()*(180.0/M_PI),               "Fwd_PhiOfTracks", "PatPixelTracking: Phi-angle of forward tracks",
         -180.0, 180.0, 60);
    plot2D(newTrack->pseudoRapidity(), HitsPerTrack, "Fwd_HitsPerTrackVsEta", "PatPixelTracking: hits/track vs pseudoRapidity of forward tracks",
           1.0, 6.0, 0.5, 15.5, 50, 15);
    plot2D(newTrack->pseudoRapidity(), newTrack->chi2PerDoF(), "Fwd_Chi2VsEta", "PatPixelTracking: Chi2/DoF vs pseudoRapidity of forward tracks",
           1.0, 6.0, 0.0, 10.0, 50, 20);
    plot2D(HitsPerTrack, newTrack->chi2PerDoF(),     "Fwd_Chi2VsHitsPerTrack", "PatPixelTracking: Chi2/DoF vs hits/forward track",
           0.5, 15.5, 0.0, 10.0, 15, 20);
    Fwd_Count++;
    }
#endif // of DEBUG_HISTO

  }

#ifdef DEBUG_HISTO
  plot(Fwd_Count, "Fwd_TracksPerEvent", "PatPixelTracking: Number of forward tracks per event", 0.0, 400.0, 40);
  plot(Bwd_Count, "Bwd_TracksPerEvent", "PatPixelTracking: Number of backward tracks per event", 0.0, 400.0, 40);
#endif

  m_tracks.clear();
}

//=========================================================================
//  Remove the worst hit until all chi2 are good.
//=========================================================================
void PatPixelTracking::removeWorstHit ( double maxChi2 ) {
  double topChi2 = 1.e9;
  while( topChi2 > maxChi2 ) {
    topChi2 = 0.0;                              // find the hit with highest Chi2 (but stop anyway when reaching the limit)
    PatPixelHit* worst = NULL;
    for (  PatPixelHits::const_iterator itH = m_track.hits().begin(); m_track.hits().end() != itH; ++itH ) {
      double myChi2 = m_track.chi2( *itH );
      if ( myChi2 > topChi2 )
      { topChi2 = myChi2; worst   = *itH; }
#ifdef DEBUG_HISTO
  plot( myChi2 , "HitChi2", "PatPixelTracking: Hit chi^2 in tracks", 1.0, 21.0, 100);
#endif
    }
    if ( topChi2 > maxChi2 )                    // if highest hit chi2 above limit
    { m_track.removeHit( worst );               // remove it
      if ( m_debug )
      { info() << "After worst hit removal" << endmsg;
        printTrack( m_track ); }
    }  
  }
}

//=========================================================================
//  Add hits from the specified sensor to the track
//=========================================================================
bool PatPixelTracking::addHitsOnSensor( PatPixelSensor* sensor, double xTol, double maxChi2 )
{ if ( 0 == sensor->hits().size() ) return false;                  // if no hits on this sensor: return (no hits added)
  double xGuess = m_track.xAtZ( sensor->z() ) - xTol; //  - 1.0;        // upper X-limit on hit search
  if ( sensor->hits().back()->x() < xGuess ) return false;         // if the first hit is below this limit: return (no hits added)
                                                                   // hits on a sensor are already sorted in X to speed up the search

  PatPixelHits::const_iterator itStart = sensor->hits().begin();
  unsigned int step = sensor->hits().size();
  while ( 2 < step )                                               // quick skip of hits that are above the X-limit
  { step = step/2;
    if ( (*(itStart+step))->x() < xGuess ) itStart += step;
  }

  bool added = false;
  for ( PatPixelHits::const_iterator itH = itStart; sensor->hits().end() != itH; ++itH )
  { double xPred = m_track.xAtHit( *itH );                        // predict X at Z of this sensor
    if ( m_debug ) printHitOnTrack( *itH );
#ifdef DEBUG_HISTO
  plot( ((*itH)->x()-xPred)/xTol , "HitExtraErrPerTol", "PatPixelTracking: Hit X extrapolation error / tolerance", -3.0, +3.0, 90);
  plot2D( (*itH)->z()-m_track.meanZ(), (*itH)->x()-xPred,
           "HitExtraErrVsZdist", "PatPixelTracking: Hit X [mm] extrapolation error vs extr. Z [mm] distance",
          -500.0, +500.0, -2.0, +2.0, 50, 100);
#endif
    if ( (*itH)->x() + xTol < xPred ) continue;                   // if hit's X is above prediction+tolerance, keep looking
    if ( (*itH)->x() - xTol > xPred ) break;                      // if hit's X is below prediction-tolerance, stop the search
#ifdef DEBUG_HISTO
  plot( m_track.chi2( *itH ) ,
        "TrackChi2forNewHit", "PatPixelTracking: Chi2 of a new candidate hit for a track",
        0.0, 60.0, 120);
#endif
    if ( m_track.chi2( *itH ) < maxChi2 )
    { if ( m_debug ) printHitOnTrack( *itH, false );
      m_track.addHit( *itH );
      added = true; }
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
