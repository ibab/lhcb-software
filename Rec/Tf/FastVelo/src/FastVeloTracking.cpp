// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/Track.h"
#include "Event/ProcStatus.h" 

#include "Event/VeloLiteCluster.h"

// local
#include "FastVeloTracking.h"
#include "FastVeloKalmanTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FastVeloTracking
//
// 2010-09-09 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FastVeloTracking )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FastVeloTracking::FastVeloTracking( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_debugTool(NULL)
  , m_timerTool(NULL)
  , m_hitManager(NULL)
{
  declareProperty( "OutputTracksName" , m_outputLocation = LHCb::TrackLocation::Velo );
  declareProperty( "HitManagerName"   , m_hitManagerName = "FastVeloHitManager" );

  declareProperty( "OnlyForward"     , m_onlyForward    = false );
  declareProperty( "OnlyBackward"    , m_onlyBackward   = false );
  declareProperty( "HLT1Only"        , m_HLT1Only       = false );
  declareProperty( "HLT2Complement"  , m_HLT2Complement = false );
  declareProperty( "MaxRZForExtra"   , m_maxRZForExtra  = 200  );
  declareProperty( "StateAtBeam"     , m_stateAtBeam    = true );
  declareProperty( "ResetUsedFlags"  , m_resetUsedFlags = false );

  declareProperty( "ZVertexMin"      , m_zVertexMin     = -170. *Gaudi::Units::mm );
  declareProperty( "ZVertexMax"      , m_zVertexMax     = +120. *Gaudi::Units::mm );
  declareProperty( "ZVertexMaxBack"  , m_zVertexMaxBack = +1200. *Gaudi::Units::mm );
  declareProperty( "MaxRSlope"       , m_maxRSlope      = 0.450     );
  declareProperty( "rMatchTol4"      , m_rMatchTol4     = 1.00      );
  declareProperty( "rMatchTol3"      , m_rMatchTol3     = 1.10      );
  declareProperty( "rExtraTol"       , m_rExtraTol      = 4.0       );
  declareProperty( "rOverlapTol"     , m_rOverlapTol    = 1.0       );
  declareProperty( "MaxMissed"       , m_maxMissed      = 1         );
  declareProperty( "MinToTag"        , m_minToTag       = 4         );

  declareProperty( "PhiMatchZone"    , m_phiMatchZone   = 0.410  );   // sin(22.5 degrees) = 0.38, some tolerance
  declareProperty( "PhiCentralZone"  , m_phiCentralZone = 0.040  );   // in overlap...
  declareProperty( "MaxDelta2"       , m_maxDelta2      = 0.05   );
  declareProperty( "FractionFound"   , m_fractionFound  = 0.70   );
  declareProperty( "MaxChi2PerHit"   , m_maxChi2PerHit  = 12.    );
  declareProperty( "MaxChi2ToAdd"    , m_maxChi2ToAdd   = 40.    );
  declareProperty( "MaxQFactor"      , m_maxQFactor     = 6.     );
  declareProperty( "MaxQFactor3"     , m_maxQFactor3    = 3.     );
  declareProperty( "DeltaQuality"    , m_deltaQuality   = 0.5    );
  declareProperty( "FractionForMerge", m_fractionForMerge= 0.70   );

  declareProperty( "MaxDistToMerge"  , m_maxDistToMerge  = 0.1 * Gaudi::Units::mm );
  declareProperty( "MaxDeltaSlopeToMerge", m_maxDeltaSlopeToMerge = 0.002 );

  declareProperty( "PhiUnusedFirstTol",  m_phiUnusedFirstTol  = 5. );
  declareProperty( "PhiUnusedSecondTol", m_phiUnusedSecondTol = 10. );

  declareProperty( "MaxTrackClusterFrac",m_maxTrackClusterFrac = 0.25 );
  declareProperty( "MaxMergeManyTracks",m_maxMergeManyTracks=0.1);
  declareProperty( "MaxPhiReuseLimit",m_PhiReuseLimit=4.0);

  // Steering the simple kalman fit that we could do at the end
  declareProperty( "UseKalmanFit", m_useKalmanFit = false ) ;
  declareProperty( "KalmanStateLastMeasurement", m_kalmanStateLastMeasurement = false ) ;
  declareProperty( "KalmanStateEndVelo", m_kalmanStateEndVelo = false ) ;
  declareProperty( "KalmanTransverseMomentumForScattering", m_kalmanPtForScattering = 400 * Gaudi::Units::MeV ) ;
  declareProperty( "KalmanScatteringNoiseParameters", m_kalmanScatteringNoiseParameters = {} ) ;

  // Parameters for debugging
  declareProperty( "DebugToolName"     , m_debugToolName  = ""        );
  declareProperty( "WantedKey"         , m_wantedKey      = -100      );
  declareProperty( "TimingMeasurement" , m_doTiming       = false     );
}
//=============================================================================
// Destructor
//=============================================================================
FastVeloTracking::~FastVeloTracking() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode FastVeloTracking::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hitManager  = tool<FastVeloHitManager>( "FastVeloHitManager", m_hitManagerName );
  m_debugTool   = 0;
  if ( "" != m_debugToolName ) m_debugTool = tool<IPatDebugTool>( m_debugToolName );

  if ( m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool/Timer", this );
    m_timeTotal   = m_timerTool->addTimer( "Fast Velo total" );
    m_timerTool->increaseIndent();
    m_timePrepare = m_timerTool->addTimer( "Fast Velo prepare" );
    m_timeFwd4    = m_timerTool->addTimer( "Fast Velo forward quadruplets" );
    m_timeBkwd4   = m_timerTool->addTimer( "Fast Velo backward quadruplets" );
    m_timeFwd3    = m_timerTool->addTimer( "Fast Velo forward triplets" );
    m_timeBkwd3   = m_timerTool->addTimer( "Fast Velo backward triplets" );
    m_timeSpace   = m_timerTool->addTimer( "Fast Velo space tracks" );
    m_timeUnused  = m_timerTool->addTimer( "Fast velo unused Phi" );
    m_timeFinal   = m_timerTool->addTimer( "Fast velo store tracks" );
    m_timerTool->decreaseIndent();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FastVeloTracking::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  m_isDebug   = msgLevel( MSG::DEBUG   );

  if ( m_doTiming ){
    m_timerTool->start( m_timeTotal );
    m_timerTool->start( m_timePrepare );
  }

  LHCb::Tracks* outputTracks;
  if ( exist<LHCb::Tracks>( m_outputLocation ) ) {
    outputTracks = get<LHCb::Tracks>( m_outputLocation );
  } else {
    outputTracks = new LHCb::Tracks();
    put(outputTracks, m_outputLocation );
  }
  outputTracks->reserve(500);
  m_hitManager->buildFastHits();
  if ( m_resetUsedFlags ) m_hitManager->resetUsedFlags();
  m_tracks.clear();
  m_spaceTracks.clear();
  unsigned int sensorNb;

  //== If needed, debug the cluster associated to the requested MC particle.
  if ( 0 <= m_wantedKey ) {
    info() << "--- Looking for Track " << m_wantedKey << endmsg;
    for ( sensorNb = m_hitManager->firstRSensor(); m_hitManager->lastRSensor() >= sensorNb; ++sensorNb ) {
      for ( unsigned int zone = 0; 4 > zone ; ++zone ) {
        for ( FastVeloHits::const_iterator itH = m_hitManager->hits( sensorNb, zone ).begin();
              m_hitManager->hits( sensorNb, zone ).end() != itH; ++itH ) {
          if ( matchKey( *itH ) ) printCoord( *itH, " " );
        }
      }
    }
  }

  //== First, look for quadruplets
  double minZ = m_zVertexMin + 200.;  // 200 mrad, 40 mm radius -> 200 mm in z
  double maxZ = m_zVertexMax - 200.;
  if ( m_doTiming ) m_timerTool->stop( m_timePrepare );

  //== Find quadruplet, if not HLT2 complement only. Do forward and backward if needed
  if ( !m_HLT2Complement ) {
    if ( !m_onlyBackward ) {
      if ( m_doTiming ) m_timerTool->start( m_timeFwd4 );
      for ( sensorNb = m_hitManager->lastRSensor(); m_hitManager->firstRSensor()+8 < sensorNb; --sensorNb ) {
        if ( m_hitManager->sensor(sensorNb)->z() < minZ ) break;
        findQuadruplets( sensorNb, true );  // forward
      }
      if ( m_doTiming ) m_timerTool->stop( m_timeFwd4 );
    }

    if ( !m_onlyForward ) {
      if ( m_doTiming ) m_timerTool->start( m_timeBkwd4 );
      for ( sensorNb = m_hitManager->firstRSensor(); m_hitManager->lastRSensor() > sensorNb+8; ++sensorNb ) {
        if ( m_hitManager->sensor(sensorNb)->z() > maxZ ) break;
        findQuadruplets( sensorNb, false );  // backward
      }
      if ( m_doTiming ) m_timerTool->stop( m_timeBkwd4 );
    }
  }

  //== Find triplets if not HLT1, backward and/or forward
  if ( !m_HLT1Only ) {
    if ( !m_onlyBackward ) {
      if ( m_doTiming ) m_timerTool->start( m_timeFwd3 );
      for ( sensorNb = m_hitManager->lastRSensor(); m_hitManager->firstRSensor()+6 < sensorNb; --sensorNb ) {
        if ( m_hitManager->sensor(sensorNb)->z() < minZ ) break;
        findUnusedTriplets( sensorNb, true );  // forward
      }
      if ( m_doTiming ) m_timerTool->stop( m_timeFwd3 );
    }
    if ( !m_onlyForward ) {
      if ( m_doTiming ) m_timerTool->start( m_timeBkwd3 );
      for ( sensorNb = m_hitManager->firstRSensor(); m_hitManager->lastRSensor() > sensorNb+6; ++sensorNb ) {
        if ( m_hitManager->sensor(sensorNb)->z() > maxZ ) break;
        findUnusedTriplets( sensorNb, false );  // backward
      }
      if ( m_doTiming ) m_timerTool->stop( m_timeBkwd3 );
    }
  }

  if ( m_doTiming ) m_timerTool->start( m_timeSpace );
  std::stable_sort( m_tracks.begin(), m_tracks.end(), FastVeloTrack::DecreasingByRLength() );

  for ( FastVeloTracks::iterator itT = m_tracks.begin(); m_tracks.end() != itT; ++itT ) {
    makeSpaceTracks( *itT );
  }
  if ( m_doTiming ) m_timerTool->stop( m_timeSpace );

  //== Perform the recovery, starting from Phi hits, with some constraints:
  //== Not HLT1, not too many tracks, space has foudn some OR there were not that many.

  if ( !m_HLT1Only &&
       m_maxRZForExtra > m_tracks.size() &&
       ( m_tracks.size() * 0.5 <=  m_spaceTracks.size() ||
         m_tracks.size() < 20 ) ) {
    if ( m_doTiming ) m_timerTool->start( m_timeUnused );
    findUnusedPhi();
    if ( m_doTiming ) m_timerTool->stop( m_timeUnused );
  }

  if ( m_doTiming ) m_timerTool->start( m_timeFinal );
  mergeSpaceClones();  // Cleanup tracks with different R, same phis...

  // Add a test to phi segments of tracks to remove when reuse is too high
  phiReuseKill();

  // Add a test on ratio of tracks to clusters to instigate second level of merging
  beamSplashSpaceMerge();

  makeLHCbTracks( outputTracks );

  if ( m_doTiming ) m_timerTool->stop( m_timeFinal );

  //== Debugging information: Status of all hits from the wanted track.

  if ( 0 <= m_wantedKey ) {
    info() << "*** Final status of hits for Track " << m_wantedKey << endmsg;
    for ( sensorNb = m_hitManager->firstRSensor(); m_hitManager->lastRSensor() >= sensorNb; ++sensorNb ) {
      for ( unsigned int zone = 0; 4 > zone ; ++zone ) {
        for ( FastVeloHits::const_iterator itH = m_hitManager->hits( sensorNb, zone ).begin();
              m_hitManager->hits( sensorNb, zone ).end() != itH; ++itH ) {
          if ( matchKey( *itH ) ) printCoord( *itH, "R  " );
        }
      }
    }
    for ( sensorNb = m_hitManager->firstPhiSensor(); m_hitManager->lastPhiSensor() >= sensorNb; ++sensorNb ) {
      for ( unsigned int zone = 0; 2 > zone ; ++zone ) {
        for ( FastVeloHits::const_iterator itH = m_hitManager->hits( sensorNb, zone ).begin();
              m_hitManager->hits( sensorNb, zone ).end() != itH; ++itH ) {
          if ( matchKey( *itH ) ) printCoord( *itH, "Phi" );
        }
      }
    }
  }

  if ( m_doTiming ) m_timerTool->stop( m_timeTotal );

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FastVeloTracking::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  Find quadruplets in RZ, i.e. 4 hits, with optionally a hole.
//  Extend the track if possible.
//=========================================================================
void FastVeloTracking::findQuadruplets( unsigned int sens0, bool forward ) {

  FastVeloSensor* sensor0 = m_hitManager->sensor( sens0 );
  for ( unsigned int zone = 0; 4 > zone; ++zone ) {
    if ( sensor0->hits( zone ).empty() ) continue;
    //== 4 cases: no holes, s3 missing, s2 missing, s1 missing
    FastVeloSensor* sensor1 = m_hitManager->sensor( sensor0->next( forward ) );
    FastVeloSensor* sensor2 = m_hitManager->sensor( sensor1->next( forward ) );
    FastVeloSensor* sensor3 = m_hitManager->sensor( sensor2->next( forward ) );
    for ( int iCase = 0; 4 > iCase; ++iCase) {
      if ( iCase == 1 ) sensor3 = m_hitManager->sensor( sensor3->next( forward ) );
      if ( iCase == 2 ) sensor2 = m_hitManager->sensor( sensor2->next( forward ) );
      if ( iCase == 3 ) sensor1 = m_hitManager->sensor( sensor1->next( forward ) );
      if ( sensor1->hits( zone ).empty() ) continue;
      if ( sensor2->hits( zone ).empty() ) continue;
      if ( sensor3->hits( zone ).empty() ) continue;

      double z0 = sensor0->z();
      double z1 = sensor1->z();
      double z2 = sensor2->z();
      double z3 = sensor3->z();

      double zFrac1 = ( z1 - z0 ) / ( z3 - z0 );
      double zFrac2 = ( z2 - z0 ) / ( z3 - z0 );

      if ( m_isDebug ) {
        info()<< "Checking  sensors [" << sensor0->number() << "] [" << sensor1->number()
              << "] [" << sensor2->number() << "] [" << sensor3->number()
              << "]  zone " << zone
              << " nbCoord " << sensor0->hits(zone).size() << " " << sensor1->hits(zone).size()
              << " " << sensor2->hits(zone).size() << " " << sensor3->hits(zone).size()
              << endmsg;
      }

      FastVeloHits::const_iterator c0, c3, first3;
      first3 = sensor3->hits(zone).begin();

      for ( c0 = sensor0->hits(zone).begin(); sensor0->hits(zone).end() != c0 ; ++c0 ) {
        if ( 0 < (*c0)->nbUsed() ) continue;
        double r0 = (*c0)->global();
        double rMin = r0 + m_maxRSlope * (z3 - z0);
        double rMax = r0 + 1.; //== Allow parallel tracks
        double rMaxNormal = r0 * ( z3 - m_zVertexMin ) / ( z0 - m_zVertexMin );
        if ( !forward)  { // tracks should have -ve gradient
          rMin = r0 - m_maxRSlope * (z3 - z0);
          rMax = r0 * ( z3 - m_zVertexMaxBack ) / ( z0 - m_zVertexMaxBack );
          rMaxNormal = rMax;
        }

        if ( 0 != m_debugTool && matchKey( *c0 ) ) {
          info() << format( "4.%d: rMin %8.3f rMax %8.3f ", iCase, rMin, rMax );
          printCoord ( *c0,   "St0 " );
        }
        // loop over clusters in fourth station finding a possible match
        for ( c3 = first3; sensor3->hits(zone).end() != c3 ; ++c3 ) {
          if ( 0 != iCase && 0 < (*c3)->nbUsed() ) continue; // cluster already used
          double r3 = (*c3)->global();
          if ( rMin > r3 ) {
            first3 = c3;
            continue;
          }
          if ( rMax <= r3 ) break;

          //== Sensor 1
          double rPred  = r0 + zFrac1 * (r3-r0);
          double rPitch = sensor1->rPitch( rPred );
          double tol    = m_rMatchTol4 * rPitch;
          if ( 0 != m_debugTool && matchKey( *c0 ) && matchKey( *c3) ) {
            info() << format( "4.%d:  rPred %8.3f (tol %6.3f)", iCase, rPred, tol );
            printCoord ( *c3,   "St3 " );
          }

          FastVeloHit* ok1 = closestHit( sensor1->hits(zone) , rPred, tol );
          if ( NULL == ok1 ) continue;


          //== Sensor 2
          rPred  = r0 + zFrac2 * (r3-r0);
          rPitch = sensor2->rPitch( rPred );
          tol    = m_rMatchTol4 * rPitch;
          if (  0 != m_debugTool && matchKey( *c0 ) && matchKey( *c3) ) {
            info() << format( "4.%d:  rPred %8.3f (tol %6.3f)", iCase, rPred, tol );
            printCoord ( ok1,   "St1 " );
          }
          FastVeloHit* ok2 = closestHit( sensor2->hits(zone), rPred, tol );
          if ( NULL == ok2 ) continue;

          m_debug = m_isDebug;
          if ( 0 != m_debugTool ) {
            if ( matchKey( *c0 ) || matchKey( ok1 ) || matchKey( ok2 ) || matchKey( *c3 )) m_debug = true;
            if ( m_debug ) {
              info() << "=== valid quadruplet ===" << endmsg;
              printCoord ( *c0, "St0 " );
              printCoord ( ok1, "St1 " );
              printCoord ( ok2, "St2 " );
              printCoord ( *c3, "St3 " );
            }
          }

          // create track
          FastVeloTrack newTrack;
          newTrack.addRHit( *c0 );
          newTrack.addRHit( ok1 );
          newTrack.addRHit( ok2 );
          newTrack.addRHit( *c3 );
          newTrack.setBackward( !forward );

          int nMiss = extendTrack( newTrack, sensor3, zone, forward );
          if ( 1 >= newTrack.nbRHits() - newTrack.nbUsedRHits() ) {
            if( m_debug ) info() << "Killed too few new hits : " << newTrack.nbUsedRHits()
                                 << " used from " <<  newTrack.nbRHits() << endmsg;
            continue;
          }

          newTrack.setMissedSensors( nMiss );  // store the number of missed stations to put on LHCb::Track later

          int setSector = zone;
          if ( sensor0->isRight() ) setSector += 4;
          newTrack.setZone( setSector );

          //== Try to add hits on the other half station, if sector 0/3 and forward
          if ( 0 == zone || 3 == zone ) {
            addOppositeSideRHits( newTrack, zone, sensor0, forward );
          }

          if ( forward && ( sens0 <= 33 && sens0 >= 30 ) ) {  // large Z gap -> may need to recover...
            extendTrack( newTrack, sensor0, zone, !forward );
          }

          if ( forward ) {
            std::sort( newTrack.rHits().begin(), newTrack.rHits().end(), FastVeloHit::IncreasingByZ() );
          } else {
            std::sort( newTrack.rHits().begin(), newTrack.rHits().end(), FastVeloHit::DecreasingByZ() );
          }
          
          if ( m_debug ) {
            info() << "      -- stored n=" << m_tracks.size()-1 << endmsg;
            printRTrack( newTrack );
          }
          if ( m_minToTag <= newTrack.nbRHits() ) {
            newTrack.tagUsedRHits();
            rMax = rMaxNormal;
          }
          
          m_tracks.push_back( std::move(newTrack) );
          
        }
      }
    }
  }
}

//=========================================================================
//  Merge space track clones: Test PHI ( m_fractionForMerge) and R >1
//=========================================================================
void FastVeloTracking::mergeSpaceClones( ) {
  if ( m_spaceTracks.size() < 2 ) return;
  if ( 0 <= m_wantedKey ) info() << endmsg << "===== Merge Space Clones ====" << endmsg;
  for ( FastVeloTracks::iterator itT1 = m_spaceTracks.begin(); m_spaceTracks.end()-1 != itT1; ++itT1 ) {
    if ( (*itT1).backward() ) continue;
    if ( !(*itT1).isValid() ) continue;

    std::vector<int> strip1(128,-1);
    FastVeloHits::const_iterator itH;
    for ( itH = (*itT1).rHits().begin(); (*itT1).rHits().end() != itH; ++itH ) {
      int sensor = (*itH)->sensor();
      int strip  = (*itH)->cluster().channelID().strip();
      if ( sensor < 128 ) strip1[sensor] = strip;
    }
    for ( itH = (*itT1).phiHits().begin(); (*itT1).phiHits().end() != itH; ++itH ) {
      int sensor = (*itH)->sensor();
      int strip  = (*itH)->cluster().channelID().strip();
      if ( sensor < 128 ) strip1[sensor] = strip;
    }

    double zMid = .5 * ( (*itT1).rHits().front()->z() + (*itT1).rHits().back()->z() );
    double slx  = 0.01 * ((*itT1).xAtZ( zMid+100.) - (*itT1).xAtZ( zMid )) ;
    double sly  = 0.01 * ((*itT1).yAtZ( zMid+100.) - (*itT1).yAtZ( zMid )) ;
    
    
    for ( FastVeloTracks::iterator itT2 = itT1+1; m_spaceTracks.end() != itT2; ++itT2 ) {
      m_debug = m_isDebug;
      if ( (*itT2).backward() ) continue;
      if ( !(*itT2).isValid() ) continue;

      double slx2  = 0.01 * ((*itT2).xAtZ( zMid+100.) - (*itT2).xAtZ( zMid )) ;
      double sly2  = 0.01 * ((*itT2).yAtZ( zMid+100.) - (*itT2).yAtZ( zMid )) ;

      double dx = (*itT1).xAtZ( 0. ) - (*itT2).xAtZ( 0. );
      double dy = (*itT1).yAtZ( 0. ) - (*itT2).yAtZ( 0. );
      double dslx = slx - slx2;
      double dsly = sly - sly2;
      if ( fabs( dslx ) > m_maxDeltaSlopeToMerge || 
           fabs( dsly ) > m_maxDeltaSlopeToMerge    ) continue;
      
      double zClosest = -( dx * dslx + dy * dsly ) / ( dslx*dslx + dsly * dsly );      

      if ( fabs( (*itT1).xAtZ( zClosest ) - (*itT2).xAtZ( zClosest ) ) > m_maxDistToMerge ) continue;
      if ( fabs( (*itT1).yAtZ( zClosest ) - (*itT2).yAtZ( zClosest ) ) > m_maxDistToMerge ) continue;
      
      std::vector<int> strip2(128,-1);
      FastVeloHits::const_iterator itH;
      for ( itH = (*itT2).rHits().begin(); (*itT2).rHits().end() != itH; ++itH ) {
        if ( matchKey( *itH ) ) m_debug = true;
        int sensor = (*itH)->sensor();
        int strip  = (*itH)->cluster().channelID().strip();
        if ( sensor < 128 ) strip2[sensor] = strip;
      }
      for ( itH = (*itT2).phiHits().begin(); (*itT2).phiHits().end() != itH; ++itH ) {
        if ( matchKey( *itH ) ) m_debug = true;
        int sensor = (*itH)->sensor();
        int strip  = (*itH)->cluster().channelID().strip();
        if ( sensor < 128 ) strip2[sensor] = strip;
      }

      //== Count how many sensors have hits on both, and how many are in commmon
      int nRInBoth = 0;
      int nRCommon = 0;
      for ( int kR = 0 ; 64 > kR ; ++kR ) {
        if ( strip1[kR] >= 0 && strip2[kR] >=0 ) {
          nRInBoth++;
          if ( strip1[kR] == strip2[kR] ) nRCommon++;
        }
      }
      int nPhiInBoth = 0;
      int nPhiCommon = 0;
      for ( int kP = 64 ; 128 > kP ; ++kP ) {
        if ( strip1[kP] >= 0 && strip2[kP] >=0 ) {
          nPhiInBoth++;
          if ( strip1[kP] == strip2[kP] ) nPhiCommon++;
        }
      }

      if ( m_debug ) {
        info() << "#sensors with hits on both " << nRInBoth << "/" << nPhiInBoth 
               << " with same cluster " << nRCommon << "/" << nPhiCommon << endmsg;
        printTrack( *itT1, "itT1" );
        printTrack( *itT2, "itT2" );
      }

      if ( ( nRInBoth   == 0 || nRCommon   > .4 * nRInBoth   ) &&
           ( nPhiInBoth == 0 || nPhiCommon > .4 * nPhiInBoth )    ){
        if ( m_debug ) info() << " === merge ===" << endmsg;
        for ( itH = (*itT2).rHits().begin(); (*itT2).rHits().end() != itH; ++itH ) {
          if ( strip1[(*itH)->sensor()] < 0 ) (*itT1).addRHit( *itH );
        }
        for ( itH = (*itT2).phiHits().begin(); (*itT2).phiHits().end() != itH; ++itH ) {
          if ( strip1[(*itH)->sensor()] < 0 ) (*itT1).addPhiCluster( *itH, m_maxChi2ToAdd );
        }
        std::sort( (*itT1).rHits().begin(), (*itT1).rHits().end(), FastVeloHit::DecreasingByZ() );
        std::sort( (*itT1).phiHits().begin(), (*itT1).phiHits().end(), FastVeloHit::DecreasingByZ() );
        
        (*itT1).updateRParameters();
        (*itT1).updatePhiWeights();
        (*itT1).fitTrack();
        (*itT1).removeWorstRAndPhi( m_maxChi2PerHit, 6 );
        (*itT2).setValid( false );
        if ( m_debug ) printTrack( *itT1, "After merge" );
      }     
    }
  }
}

//=========================================================================
// Remove tracks with phi clusters used too many times
//=========================================================================
void FastVeloTracking::phiReuseKill( ){
  if ( m_debug ) {
    debug() << "phiReuseKill " << m_spaceTracks .size() 
	    << " tracks " << endmsg;
  }
  if ( m_spaceTracks.size() < 2 ) return;
  for ( auto &track : m_spaceTracks ) {
    if ( track.backward() ) continue;
    if ( !track.isValid() ) continue;
    double nPhiUsed = 0;
    for ( auto &hit : track.phiHits() ) {
      nPhiUsed += hit->nbUsed();
    }
    if ( m_debug ) {
      debug() << "Track with (" <<nPhiUsed<<"/"<<track.phiHits().size()
		<< ") = " << (nPhiUsed/track.phiHits().size())
		<< " reuse of phi clusters" << endmsg;
    }
    if( (nPhiUsed/track.phiHits().size()) > m_PhiReuseLimit ){      
      track.setValid(false);
      for ( auto &hit : track.phiHits() ) {
	hit->clearUsed();
      }
      if ( m_debug ) {
	debug() << "Killed track with (" <<nPhiUsed<<"/"<<track.phiHits().size()
		<< ") = " << (nPhiUsed/track.phiHits().size())
		<< " reuse of phi clusters" << endmsg;
      }
    }  
  }  
}


//=========================================================================
//  In the case of beam splashes where nTrack > nClus*maxFrac do second level of merging
//=========================================================================
void FastVeloTracking::beamSplashSpaceMerge( ) {
  if( m_spaceTracks.size() < 2 ) return; // skip empty or near empty events
  // grab the number of VeloLiteClusters
  unsigned int nVlite = 
    get<LHCb::VeloLiteCluster::FastContainer>(LHCb::VeloLiteClusterLocation::Default)->size();

  unsigned int nGoodBefore(0);
  unsigned int nGoodAfter(0);
  if ( m_isDebug ) {
    debug() << "Comparing " << m_spaceTracks.size() << " tracks to " << nVlite
            << "*" << m_maxTrackClusterFrac 
            << " = "<< (nVlite*m_maxTrackClusterFrac) << " max tracks before splash killer" << endmsg;
    for ( FastVeloTracks::iterator itT1 = m_spaceTracks.begin(); m_spaceTracks.end() != itT1; ++itT1 ) {
      if ( (*itT1).isValid() ) ++nGoodBefore;
    }
  }
  // happly nothing to do if we can return here
  if( m_spaceTracks.size() < (nVlite*m_maxTrackClusterFrac) ) return; 
  // OK so this is a splash event
  Warning("Fired high track to cluster ratio: assume beam splash and do aggressive clone killing",
	  StatusCode::SUCCESS,0).ignore();

  // set ProcStat for this event to flag the splash in ProcStatus 
  // not aborting the event and using 1 as flag for FastVeloTracking status 
  LHCb::ProcStatus *pStat =  
    getOrCreate<LHCb::ProcStatus,LHCb::ProcStatus>(LHCb::ProcStatusLocation::Default); 
  pStat->addAlgorithmStatus("FastVeloTracking","VELO","BeamSplashFastVelo",1,false);  
 

  for ( FastVeloTracks::iterator itT1 = m_spaceTracks.begin(); m_spaceTracks.end() != itT1; ++itT1 ) {
    if ( !(*itT1).isValid() ) continue;
    for ( FastVeloTracks::iterator itT2 = itT1+1; m_spaceTracks.end() != itT2; ++itT2 ) {
      if ( !(*itT2).isValid() ) continue;
      unsigned int nShort = ((*itT2).nbRHits() + (*itT2).phiHits().size());
      unsigned int nLong  = ((*itT1).nbRHits() + (*itT1).phiHits().size());
      FastVeloTracks::iterator iShort = itT2;
      if( nLong < nShort ){
        nShort = nLong;
        iShort = itT1;
      }
      unsigned int nCommon = 0;
      FastVeloHits::iterator iV1,iV2;
      // compare R
      for( iV1 = (*itT1).rHits().begin(); iV1 != (*itT1).rHits().end(); ++iV1 ){        
        for( iV2 = (*itT2).rHits().begin(); iV2 != (*itT2).rHits().end(); ++iV2 ){
          if( (*iV1)->lhcbID() == (*iV2)->lhcbID() ) ++nCommon;
        }
      }
      // then compare phi
      for( iV1 = (*itT1).phiHits().begin(); iV1 != (*itT1).phiHits().end(); ++iV1 ){        
        for( iV2 = (*itT2).phiHits().begin(); iV2 != (*itT2).phiHits().end(); ++iV2 ){
          if( (*iV1)->lhcbID() == (*iV2)->lhcbID() ) ++nCommon;
        }
      }
      if( ((double)nCommon/(double)nShort) > m_maxMergeManyTracks ) {
        iShort->setValid(false);
        if ( m_isDebug ) {
          debug() << "removed track " << iShort - m_spaceTracks.begin() << endmsg;
        }
      }
    }
    if((*itT1).isValid()) ++nGoodAfter;
  }        
  if(m_isDebug ) 
    debug() <<"Reduced " << nGoodBefore << " tracks to " << nGoodAfter
            << " in clone killing for splash events" << endmsg;
}

//=========================================================================
//  Find triplets in RZ with unused hits, i.e. 3 hits, with optionally a hole.
//  Extend the track if possible.
//=========================================================================
void FastVeloTracking::findUnusedTriplets( unsigned int sens0, bool forward ) {

  FastVeloSensor* sensor0 = m_hitManager->sensor( sens0 );
  for ( unsigned int zone = 0; 4 > zone; ++zone ) {
    if ( sensor0->hits( zone ).empty() ) continue;
    //== 4 cases: no holes, s3 missing, s2 missing, s1 missing
    FastVeloSensor* sensor1 = m_hitManager->sensor( sensor0->next( forward ) );
    FastVeloSensor* sensor2 = m_hitManager->sensor( sensor1->next( forward ) );
    for ( int iCase = 0; 3 > iCase; ++iCase) {
      if ( iCase == 1 ) sensor2 = m_hitManager->sensor( sensor2->next( forward ) );
      if ( iCase == 2 ) sensor1 = m_hitManager->sensor( sensor1->next( forward ) );
      if ( sensor1->hits( zone ).empty() ) continue;
      if ( sensor2->hits( zone ).empty() ) continue;

      double z0 = sensor0->z();
      double z1 = sensor1->z();
      double z2 = sensor2->z();

      double zFrac = ( z1 - z0 ) / ( z2 - z0 );

      if ( m_isDebug ) {
        info()<< "Checking  sensors [" << sensor0->number() << "] [" << sensor1->number()
              << "] [" << sensor2->number() << "]  zone " << zone
              << " nbCoord " << sensor0->hits(zone).size() << " " << sensor1->hits(zone).size()
              << " " << sensor2->hits(zone).size()
              << endmsg;
      }

      FastVeloHits::const_iterator c0, c2, first2;
      first2 = sensor2->hits(zone).begin();

      for ( c0 = sensor0->hits(zone).begin(); sensor0->hits(zone).end() != c0 ; ++c0 ) {
        if ( 0 < (*c0)->nbUsed() ) continue;
        double r0 = (*c0)->global();
        double rMin = r0 + m_maxRSlope * (z2 - z0);
        double rMax = r0;
        if ( !forward)  { // tracks should have -ve gradient and come from the vertex area
          rMin = r0 - m_maxRSlope * (z2 - z0);
          rMax = r0 * ( z2 - m_zVertexMaxBack ) / ( z0 - m_zVertexMaxBack );
        }

        if (  0 != m_debugTool && matchKey( *c0 ) ) {
          info() << format( "3.%d: rMin %8.3f rMax %8.3f ", iCase, rMin, rMax );
          printCoord ( *c0,   "St0 " );
        }
        // loop over clusters in third station finding a possible match
        for ( c2 = first2; sensor2->hits(zone).end() != c2 ; ++c2 ) {
          if ( 0 < (*c2)->nbUsed() ) continue; // cluster already used
          double r2 = (*c2)->global();
          if ( rMin > r2 ) {
            first2 = c2;
            continue;
          }
          if ( rMax <= r2 ) break;

          //== Sensor 1
          double rPred  = r0 + zFrac * (r2-r0);
          double rPitch = sensor1->rPitch( rPred );
          double tol    = m_rMatchTol3 * rPitch;
          if (  0 != m_debugTool && matchKey( *c0 ) && matchKey( *c2) ) {
            info() << format( "3.%d: rPred %8.3f (tol %6.3f) ", iCase, rPred, tol );
            printCoord ( *c2,   "St2 " );
          }
          FastVeloHit* ok1 = closestUnusedHit( sensor1->hits(zone), rPred, tol );
          if ( NULL == ok1 ) continue;

          m_debug = m_isDebug;
          if ( 0 != m_debugTool ) {
            if ( matchKey( *c0 ) || matchKey( ok1 ) || matchKey( *c2 )) m_debug = true;
            if ( m_debug ) {
              info() << "=== valid triplet ===" << endmsg;
              printCoord ( *c0, "St0 " );
              printCoord ( ok1, "St1 " );
              printCoord ( *c2, "St2 " );
            }
          }

          // create track
          FastVeloTrack newTrack;
          newTrack.addRHit( *c0 );
          newTrack.addRHit( ok1 );
          newTrack.addRHit( *c2 );
          newTrack.setBackward( !forward );

          int nMiss = extendTrack( newTrack, sensor2, zone, forward );
          newTrack.setMissedSensors( nMiss );  // store the number of missed stations to put on LHCb::Track later
          if ( 1 >= newTrack.nbRHits() - newTrack.nbUsedRHits() ) {
            if( m_debug ) info() << "Killed too few new hits" << endmsg;
            continue;
          }
          //== Tracks with last 3 sensors, a gap, a single other. remove the last one.
          if ( sens0 > 39 && newTrack.rHits().size() == 4 ) {
            if ( newTrack.rHits().back()->sensor() < 34 ) {
              newTrack.rHits().pop_back();
            }
          }

          //== More demanding for minimal tracks of 3 clusters. Allow change of zone...
          if ( 3 == newTrack.nbRHits() ) {
            int nMissBack = extendTrack( newTrack, sensor0, zone, !forward );
            if ( m_maxMissed <  nMissBack ) {
              FastVeloSensor *lastSensor = sensor0;
              if( 0 != zone ){
                if(3 < newTrack.nbRHits() ){
                  // Added clusters in the above. Start adding from the end of the track
                  // Last sensor added is the end of the list
                  lastSensor = m_hitManager->sensor( newTrack.rHits().back()->sensor() );
                }
                nMissBack = extendTrack( newTrack, lastSensor, zone-1, !forward );
              }
            }
            if ( m_maxMissed <  nMissBack ) {
              FastVeloSensor *lastSensor = sensor0;
              if ( 3 != zone ) {
                if(3 < newTrack.nbRHits() ){
                  // Added clusters in the above. Start adding from the end of the track
                  // Last sensor added is the end of the list
                  lastSensor = m_hitManager->sensor( newTrack.rHits().back()->sensor() );
                }
                nMissBack = extendTrack( newTrack, lastSensor, zone+1, !forward );
              }
            }
            if ( m_maxMissed <  nMissBack ) {
              if ( m_debug ) info() << "Short track with missed sensors before." << endmsg;
              continue;
            }
          }
          
          int setSector = zone;
          if ( sensor0->isRight() ) setSector += 4;
          newTrack.setZone( setSector );

          //== Try to add hits on the other half station, if sector 0/3 and forward

          if ( forward && ( 0 == zone || 3 == zone ) ) {
            addOppositeSideRHits( newTrack, zone, sensor0, forward );
          }
          //== 3 hit tracks: If sharing hits, take the 'best' one
          if ( newTrack.rHits().size() == 3 ) {
            bool reject = false;
            for ( FastVeloTracks::iterator itT = m_tracks.begin();
                  m_tracks.end() != itT; ++itT ) {
              if ( (*itT).rHits().size() != 3 ) continue;
              int nCommon = 0;
              for ( FastVeloHits::const_iterator itH = (*itT).rHits().begin();
                    (*itT).rHits().end() != itH; ++itH ) {
                if ( std::find( newTrack.rHits().begin(), newTrack.rHits().end(), *itH ) !=  newTrack.rHits().end() ) nCommon++;
              }
              if ( 0 != nCommon ) {
                if ( m_debug ) info() << "Share " << nCommon << " hits with "
                                      << itT - m_tracks.begin() << ", chi2 : "
                                      <<  newTrack.rChi2() << " / " <<  (*itT).rChi2();
                if ( newTrack.rChi2() > (*itT).rChi2() ) {
                  if ( m_debug ) info() << " -> rejected" << endmsg;
                } else {
                  if ( m_debug ) info() << " -> replace it" << endmsg;
                  (*itT) = std::move(newTrack);
                }
                reject = true;
                break;
              }

            }
            if ( reject ) continue;
          }

          if ( forward ) {
            std::sort( newTrack.rHits().begin(), newTrack.rHits().end(), FastVeloHit::IncreasingByZ() );
          } else {
            std::sort( newTrack.rHits().begin(), newTrack.rHits().end(), FastVeloHit::DecreasingByZ() );
          }

          if ( m_debug ) {
            info() << "      -- stored n=" << m_tracks.size()-1
                   << " nbRHits " << newTrack.nbRHits()
                   << " chi2 " << newTrack.rChi2() << endmsg;
            printRTrack( newTrack );
          }
          if ( m_minToTag <= newTrack.nbRHits() ) newTrack.tagUsedRHits();

          m_tracks.push_back( std::move(newTrack) );
        }
      }
    }
  }
}
//=========================================================================
//  Get the closest hit in the window.
//=========================================================================
FastVeloHit* FastVeloTracking::closestHit( FastVeloHits& hits, double coord, double tol ) {
  double minDist = tol;
  FastVeloHit* best = NULL;
  for ( FastVeloHits::const_iterator itH = hits.begin(); hits.end() != itH ; ++itH ) {
    double dist = (*itH)->global() - coord;
    if ( dist < -minDist ) continue;
    if ( dist >  minDist ) break;
    minDist = fabs( dist );
    best = *itH;
  }
  return best;
}
//=========================================================================
//  Get the closest hit in the window.
//=========================================================================
FastVeloHit* FastVeloTracking::closestUnusedHit( FastVeloHits& hits, double coord, double tol ) {
  double minDist = tol;
  FastVeloHit* best = NULL;
  for ( FastVeloHits::const_iterator itH = hits.begin(); hits.end() != itH ; ++itH ) {
    double dist = (*itH)->global() - coord;
    if ( dist < -minDist ) continue;
    if ( dist >  minDist ) break;
    if ( 0 != (*itH)->nbUsed() ) continue;
    minDist = fabs( dist );
    best = *itH;
  }
  return best;
}
//=========================================================================
//  Convert the local track to LHCb tracks
//=========================================================================
void FastVeloTracking::makeLHCbTracks( LHCb::Tracks* outputTracks ) {

  LHCb::Track::LHCbIDContainer ids;
  LHCb::Track::StateContainer states; states.reserve(2);
  for ( auto& track : m_spaceTracks ) {
    if ( track.rHits().size() < 3 || track.phiHits().size() < 3 ) track.setValid( false );
    if ( !track.isValid() ) continue;
    LHCb::Track *newTrack = new LHCb::Track();
    newTrack->setType( LHCb::Track::Velo );
    newTrack->setHistory( LHCb::Track::PatFastVelo );
    newTrack->setPatRecStatus( LHCb::Track::PatRecIDs );
    if ( UNLIKELY(m_debug) ) {
      info() << "=== Store track Nb " << outputTracks->size() << endmsg;
      printCoords( track.rHits(),   "Stored R   " );
      printCoords( track.phiHits(), "Stored Phi " );
    }
    //== Add the number of found + missed R hits. Note that 'missed' is incomplete as we stop
    //== searching missed sensors after the maximum number of missed sensors has been reached.
    //== Multiply by 2 to mimic the previous implementation in Pat.
    newTrack->addInfo( LHCb::Track::nPRVelo3DExpect, 2 * ( track.nbRHits() + track.nbMissedSensors() ) );

    ids.clear(); ids.reserve( track.rHits().size() + track.phiHits().size() );

    auto hit2lhcbid = [](const FastVeloHit* hit) { return hit->lhcbID(); };
    std::transform( std::begin(track.rHits()), std::end(track.rHits()), 
                    std::back_inserter(ids),
                    std::cref(hit2lhcbid)
    );
    std::transform( std::begin(track.phiHits()), std::end(track.phiHits()), 
                    std::back_inserter(ids),
                    std::cref(hit2lhcbid)
    );
    newTrack->addToLhcbIDs( ids );

    auto update_z_minmax = [](std::pair<double,double> z, const FastVeloHit* hit) {
            auto hz = hit->z();
            if (hz > z.second) z.second=hz;
            if (hz < z.first)  z.first = hz;
            return z;
    };
    auto zMinMax = std::accumulate( std::begin(track.phiHits()), std::end(track.phiHits()), 
                                    std::accumulate( std::begin(track.rHits()), std::end(track.rHits()), 
                                                     std::make_pair(1.e9,-1.e9), 
                                                     std::cref(update_z_minmax) ),
                                    std::cref(update_z_minmax) );
    double zMin = zMinMax.first;
    double zMax = zMinMax.second;


    //== Define backward as z closest to beam downstream of hits
    auto zBeam = track.zBeam();
    bool backward = zBeam > zMax;
    newTrack->setFlag( LHCb::Track::Backward, backward );

    states.clear();
    if ( m_stateAtBeam ) {
      states.push_back( new LHCb::State() );
      auto& state = states.back();
      state->setLocation( LHCb::State::ClosestToBeam );
      state->setState( track.state( zBeam ) );
      state->setCovariance( track.covariance( zBeam ) );
    } else {
      states.push_back( new LHCb::State() );
      auto& state = states.back();
      state->setLocation( LHCb::State::FirstMeasurement );
      state->setState( track.state( zMin ) );
      state->setCovariance( track.covariance( zMin ) );
    }

    if ( !backward ) {
      states.push_back( new LHCb::State() );
      auto& state = states.back();
      state->setLocation( LHCb::State::EndVelo );
      state->setState( track.state( zMax ) );
      state->setCovariance( track.covariance( zMax ) );
    }
    newTrack->addToStates( states );

    if( m_useKalmanFit ) {
      // as tracks share hits, we need to update everything
      track.updateRPhi() ;
      FastVeloKalmanTrack kalmantrack( track ) ;
      kalmantrack.addStates( *newTrack,newTrack->firstState(),m_kalmanPtForScattering,m_kalmanScatteringNoiseParameters,
			     m_stateAtBeam,m_kalmanStateLastMeasurement,m_kalmanStateEndVelo ) ;
    }

    outputTracks->insert( newTrack );
  }
}
//=========================================================================
//  Extend a track towards vertex
//=========================================================================
int FastVeloTracking::extendTrack( FastVeloTrack &newTrack, FastVeloSensor* sensor,
                                   unsigned int zone, bool forward ) {

  //== Extrapolate until too many consecutive hits missed.
  int nMiss = 0;
  while ( m_maxMissed >= nMiss ) {
    int nextSensor = sensor->next( forward );
    if ( 0 > nextSensor ) break; // No other sensor...
    sensor        = m_hitManager->sensor( nextSensor );
    double z      = sensor->z();
    double rPred  = newTrack.rPred( z ); // new predicted co-ord
    double trTol  = sqrt( newTrack.rErr2( z ) );
    const DeVeloRType* rSensor = sensor->rSensor();
    double rPitch = rSensor->rPitch( rPred );
    double tol    = m_rExtraTol * trTol + rPitch;

    if ( m_debug ) {
      info() << format( "       sensor %3d z%7.1f tol  %5.3f        pred %8.3f",
                        sensor->number(), z,  tol, rPred ) << endmsg;
    }

    if ( rSensor->rMin(zone) > rPred ) break;  // no longer inside.
    if ( rSensor->rMax(zone) < rPred ) break;  // no longer inside.

    //== Select the best hit when extrapolating
    FastVeloHit* best = closestHit( sensor->hits( zone ), rPred, tol );
    if ( 0 != best ) {
      nMiss = 0;
      newTrack.addRHit( best );
      if ( m_debug ) printCoord( best, "add " );
    } else {
      if ( sensor->rSensor()->tell1EventInfo().wasDecoded() ) ++nMiss;      // no hit found
    }
  }
  return nMiss;
}

//=========================================================================
//  Add hits from the other side of the detector (overlap tracks)
//=========================================================================
void FastVeloTracking::addOppositeSideRHits( FastVeloTrack &newTrack, const int &zone,
                                             FastVeloSensor* firstSensor, bool forward ) {

  int sensorNb = firstSensor->number() + 1;
  if ( sensorNb > (int)m_hitManager->lastRSensor() ) sensorNb = sensorNb - 2;

  // since zones are ordered in global phi, we always have to flip
  int otherZone = 3-zone;

  if ( m_debug ) info() << " +++ extend to the other side, start at sensor " << sensorNb << endmsg;

  //== Extrapolate until too many consecutive hits missed,
  FastVeloHits extraHits;

  while ( 0 <= sensorNb ) {
    FastVeloSensor* sensor = m_hitManager->sensor( sensorNb );
    double z      = sensor->z();
    double rPred  = newTrack.rPred( z );
    double rPitch = sensor->rPitch( rPred );
    double tol    = m_rOverlapTol * rPitch;   // for overlap
    if ( m_debug ) {
      info() << format( "Opp:   sensor %3d z%7.1f tol  %5.3f        pred %8.3f",
                        sensor->number(), z,  tol, rPred ) << endmsg;
    }

    if (  forward && sensor->rMin() > rPred ) break;  // no longer inside.
    if ( !forward && sensor->rMax() < rPred ) break;  // no longer inside.

    //== Select the best hit when extrapolating
    FastVeloHit* best = closestHit( sensor->hits( otherZone ), rPred, tol );
    if ( 0 != best ) {
      extraHits.push_back( best );
      if ( m_debug ) printCoord( best, "add " );
    }
    sensorNb = sensor->next( forward );
  }
  if ( 2 < extraHits.size() ) {
    for ( FastVeloHits::const_iterator itH = extraHits.begin(); extraHits.end() != itH; ++itH ) {
      newTrack.addRHit( *itH );
    }
  }
}

//=========================================================================
//  Make space tracks...
//=========================================================================
void FastVeloTracking::makeSpaceTracks( FastVeloTrack& input ) {
  m_debug = m_isDebug;
  FastVeloHits::iterator itH;
  if ( NULL != m_debugTool ) {
    for ( itH = input.rHits().begin(); input.rHits().end() != itH; ++itH ) {
      if ( matchKey( *itH ) ) m_debug = true;
    }
  }
  int nbLeft  = 0;
  int nbRight = 0;
  unsigned int lowSensor = 1000;
  unsigned int highSensor = 0;
  int sumSize = 0;
  for ( itH = input.rHits().begin(); input.rHits().end() != itH; ++itH ) {
    if ( (*itH)->sensor() > highSensor ) highSensor =  (*itH)->sensor();
    if ( (*itH)->sensor() < lowSensor )  lowSensor  =  (*itH)->sensor();
    sumSize += (*itH)->cluster().pseudoSize();
    if ( m_hitManager->sensor( (*itH)->sensor() )->isRight() ) {
      ++nbRight;
    } else {
      ++nbLeft;
    }
  }
  highSensor = highSensor + 64 + 2;
  lowSensor  = lowSensor  + 64 - 2;

  bool forward   = !input.backward();
  bool inOverlap = nbLeft > 2 && nbRight > 2;

  m_cosPhi = m_hitManager->cosPhi( input.zone() );
  m_sinPhi = m_hitManager->sinPhi( input.zone() );
  if ( inOverlap ) {
    m_cosPhi = 0;
    m_sinPhi = m_sinPhi/fabs(m_sinPhi);
  }

  bool isVertical = ((input.zone()+1)/2)%2 == 0 ; // zones 0, 3, 4, 7
  double signOfSolution = 1.;
  if ( input.zone() < 1 || input.zone() > 4 ) signOfSolution = -1.;

  int firstSensor = m_hitManager->lastPhiSensor();
  //  int lastSensor  = m_hitManager->firstPhiSensor(); // not used 
  int step = -2;
  if ( input.zone() < 4 )  firstSensor -= 1;  // other side
  if( !forward ) {
    firstSensor = m_hitManager->firstPhiSensor();
    //    lastSensor  = m_hitManager->lastPhiSensor();
    step = +2;
    if ( input.zone() > 3 )  firstSensor += 1;  // other side
  }
  if ( isVertical ) step /= 2;                    // look on both sides

  FastVeloSensor* first =  m_hitManager->sensor( firstSensor );
  double rOffset = first->rOffset( input.zone()%4 );

  while( input.rPred( first->z() ) - rOffset > first->rMax() ||
         input.rPred( first->z() ) - rOffset < first->rMin()    ) {
    FastVeloSensor* temp = m_hitManager->sensor( first->number() + step );
    if ( 0 == temp ) break;
    first = temp;
    rOffset = first->rOffset( input.zone()%4 );
  }

  FastVeloSensor* last = first;
  rOffset = last->rOffset( input.zone()%4 );
  while( input.rPred( last->z() ) - rOffset < last->rMax() &&
         input.rPred( last->z() ) - rOffset > last->rMin()  ) {
    FastVeloSensor* temp = m_hitManager->sensor( last->number() + step );
    if ( 0 == temp ) break;
    last = temp;
    rOffset = last->rOffset( input.zone()%4 );
    if ( last->number() + step > m_hitManager->lastPhiSensor() ) break;
  }

  if ( m_debug ) info() << "Space tracking. Zone " << input.zone() << " from " << first->number()
                        << " to " << last->number() << " restrict to " << lowSensor << " - " << highSensor
                        << " isVertical " << isVertical << " inOverlap " << inOverlap
                        << " forward " << forward << " nbRight " << nbRight << " Left " << nbLeft << endreq;


  //== Define the sine of the distance as measure of the track. Store those in the correct range
  FastVeloSensor* sensor = first;
  int firstSensorWithHit = -1;

  std::vector< FastVeloHits > goodPhiHits(21, FastVeloHits() );

  while ( sensor != last ) {
    if ( sensor->number() > highSensor || sensor->number() < lowSensor  ) {
      sensor = m_hitManager->sensor( sensor->number() + step );
      continue;
    }
    unsigned int module = sensor->number()/2 - 32;
    goodPhiHits[module].reserve(200); // avoid relocation...

    double rPred  = input.rInterpolated( sensor->z() ) - sensor->rOffset( input.zone()%4 );
    double x0     = sensor->xCentre();
    double y0     = sensor->yCentre();
    double dR2    = rPred * rPred - x0 * x0 - y0 * y0;
    double xPred  = rPred * m_cosPhi + x0;
    double yPred  = rPred * m_sinPhi + y0;

    double maxDSin = m_phiMatchZone;
    if ( inOverlap )  maxDSin = m_phiCentralZone;
    double minDist = -maxDSin * rPred;
    double maxDist =  maxDSin * rPred;

    for ( unsigned int zone = 0; 2 > zone ; ++zone ) {
      if ( rPred < sensor->rMin( zone ) || rPred > sensor->rMax( zone ) ) continue;
      double minR = 0.4 * (sensor->rMin( zone ) + sensor->rMax( zone ) );  // .8 of centre

      for ( itH = sensor->hits(zone).begin(); sensor->hits(zone).end() != itH; ++itH ) {
        double dx = (*itH)->xStripCentre()-x0;
        double dy = (*itH)->yStripCentre()-y0;
        if ( m_cosPhi * dx + m_sinPhi * dy < minR )  continue;  //scalar product OK

        double dist = (*itH)->distance( xPred, yPred );
        if ( dist < minDist || dist > maxDist ) continue;

        double a = (*itH)->a();
        double b = (*itH)->b();
        double c = (*itH)->c();

        double x = 0.;
        double y = 0.;

        if ( isVertical ) {
          double bEq = b * c + a * b * x0 - a * a * y0;
          double cEq = c * c + 2 * a * c * x0 - a * a * dR2;
          y   = - bEq + signOfSolution * sqrt( bEq * bEq - cEq );
          x   = ( - c - b * y ) / a;
        } else {
          double bEq = a * c + a * b * y0 - b * b * x0;
          double cEq = c * c + 2 * b * c * y0 - b * b * dR2;
          x   = - bEq + signOfSolution * sqrt( bEq * bEq - cEq );
          y   = ( - c - a * x ) / b;
        }
        (*itH)->setGlobalPosition( x, y );
        double dSin = ( y * m_cosPhi - x * m_sinPhi)/rPred;
        (*itH)->setGlobal( dSin );

        if ( m_debug && matchKey( *itH ) ) {
          double xMc = m_debugTool->xTrue( m_wantedKey, sensor->z() );
          double yMc = m_debugTool->yTrue( m_wantedKey, sensor->z() );
          double rMc = sqrt( xMc * xMc + yMc * yMc );
          info() << format( "x%7.3f y%7.3f R%7.3f dSin%7.3f, MC: %7.3f %7.3f R%7.3f dist%7.3f",
                            x, y, sqrt( x*x + y*y ), dSin,
                            xMc, yMc, rMc, (*itH)->distance( xMc, yMc ) );
          printCoord( *itH, ":" );
        }
        if ( fabs( dSin ) > maxDSin ) continue;

        (*itH)->setZ( sensor->z( x, y ) );
        (*itH)->setPhiWeight( rPred );
        if ( 0 > firstSensorWithHit ) firstSensorWithHit = sensor->number();
        goodPhiHits[module].push_back( *itH );
      }
    }
    sensor = m_hitManager->sensor( sensor->number() + step );
  }
  //== Sort and find the station range;
  unsigned int firstStation = 0;
  unsigned int lastStation  = 20;
  unsigned int station;
  for ( station = 0; 21 > station; ++station ) {
    if ( goodPhiHits[station].size() == 0 ) {
      if ( station == firstStation ) ++firstStation;
    } else {
      lastStation = station;
      if ( goodPhiHits[station].size() > 1 ) std::sort( goodPhiHits[station].begin(), goodPhiHits[station].end(),
                                                        FastVeloHit::LowerByGlobal() );
    }
  }

  int stationStep = 1;
  if ( forward ) {
    unsigned int tmp = firstStation;
    firstStation = lastStation;
    lastStation  = tmp;
    stationStep = -1;
  }

  unsigned int nbStations =  abs( int(lastStation) - int(firstStation) ) + 1;
  if ( nbStations > input.rHits().size() ) nbStations = input.rHits().size();
  if ( inOverlap ) {
    nbStations = nbRight;
    if ( nbLeft > nbRight ) nbStations = nbLeft;
  }
  unsigned int minNbPhi = int( m_fractionFound * nbStations );
  if ( 3 > minNbPhi ) minNbPhi = 3;

  if ( m_debug ) {
    info() << "Space search from station " <<  firstStation << " to " << lastStation
           << " minNbPhi " << minNbPhi << endmsg;
    printCoords( input.rHits(), "R  " );
    sensor = first;
    while ( sensor != last ) {
      for ( unsigned int zone = 0; 2 > zone ; ++zone ) {
        for ( itH = sensor->hits(zone).begin(); sensor->hits(zone).end() != itH; ++itH ) {
          if ( matchKey( *itH ) ) printCoord( *itH, "Phi" );
        }
      }
      sensor = m_hitManager->sensor( sensor->number() + step );
    }

    //== Display the list of selected hits
    info() <<"--- goodPhiHits ---" << endmsg;
    for ( station = firstStation; lastStation+stationStep != station; station += stationStep ) {
      std::sort( goodPhiHits[station].begin(), goodPhiHits[station].end(), FastVeloHit::LowerByGlobal() );
      printCoords( goodPhiHits[station], "Selected " );
      if ( stationStep == -1 && station == 0 ) break;
    }
  }

  FastVeloTracks newTracks;
  //== Search sensors for 'lists' with low angle, i.e. d(global)/dz
  FastVeloHits::iterator itH1, itH2, itH3;
  unsigned int s1, s2, s3;
  double z1;

  s1 = firstStation;
  s2 = s1 + stationStep;
  s3 = s2 + stationStep;

  bool hasFullLength = false;

  for ( int iCase = 0; 5 > iCase; ++iCase ) {
    if ( 2 == iCase ) s3 += stationStep;
    if ( 3 == iCase ) s2 += stationStep;
    if ( 4 == iCase ) s1 += stationStep;
    if ( s1 > 20 || s2 > 20 || s3 > 20 ) continue;  // protect agains access to no existent modules
    for ( itH1 = goodPhiHits[s1].begin(); goodPhiHits[s1].end() != itH1; ++itH1 ) {
      if ( 0 == iCase && 0 != (*itH1)->nbUsed() ) continue;
      z1 = (*itH1)->z();
      double rPred = input.rInterpolated( z1 );
      double minDelta2 = m_maxDelta2;
      if ( input.backward() ) minDelta2 = 0.01;
      FastVeloHit* best2 = NULL;
      for ( itH2 = goodPhiHits[s2].begin(); goodPhiHits[s2].end() != itH2; ++itH2 ) {
        if ( 0 == iCase && 0 != (*itH2)->nbUsed() ) continue;
        double dist = fabs( ( (*itH2)->global() - (*itH1)->global() ) * rPred / ( (*itH2)->z() - z1 ) );
        if ( minDelta2 > dist ) {
          best2 = *itH2;
          if ( m_debug  ) {
            info() << "+++ iCase " << iCase << " Try pair with dist = " << dist << " s3 = " << s3 << endmsg;
            printCoord( *itH1, "s1" );
            printCoord( best2, "s2" );
          }

          //== Check that this pair is not already on an existing candidate
          bool reject = false;
          for ( FastVeloTracks::iterator itT = newTracks.begin(); newTracks.end() != itT ; ++itT ) {
            if ( std::find( (*itT).phiHits().begin(), (*itT).phiHits().end(), *itH1 ) != (*itT).phiHits().end() ) {
              if ( std::find( (*itT).phiHits().begin(), (*itT).phiHits().end(), best2 ) != (*itT).phiHits().end() ) {
                reject = true;
                break;
              }
            }
          }
          if ( reject ) {
            if ( m_debug ) info() << "Already used hits..." << endmsg;
            continue;
          }

          double dz = z1 - best2->z();
          double tx = ( (*itH1)->xGlobal() - best2->xGlobal() ) / dz;
          double ty = ( (*itH1)->yGlobal() - best2->yGlobal() ) / dz;
          double x0 = (*itH1)->xGlobal() - tx * z1;
          double y0 = (*itH1)->yGlobal() - ty * z1;

          double minDelta3 = 1.e9;
          FastVeloHit* best3 = NULL;
          for ( itH3 = goodPhiHits[s3].begin(); goodPhiHits[s3].end() != itH3; ++itH3 ) {
            if ( 0 == iCase && 0 != (*itH3)->nbUsed() ) continue;
            if ( 1 == iCase &&
                 0 == (*itH1)->nbUsed() &&
                 0 == (best2)->nbUsed() &&
                 0 == (*itH3)->nbUsed()) continue;  // already tried in iCase = 0.
            double dist = (*itH3)->chi2( x0 + (*itH3)->z() * tx, y0 + (*itH3)->z() * ty );
            if ( minDelta3 > dist ) {
              minDelta3 = dist;
              best3 = *itH3;
            }
          }
          if ( minDelta3 > 4 * m_maxChi2ToAdd ) continue;
          if ( m_debug ) {
            info() << "+++ Found triplet with minDelta3 = " << minDelta3 << endmsg;
            printCoord( *itH1, "s1" );
            printCoord( best2, "s2" );
            printCoord( best3, "s3" );
          }

          //== Update Z of the R sensors
          for ( itH = input.rHits().begin(); input.rHits().end() != itH ; ++itH ) {
            FastVeloSensor* _sensor = m_hitManager->sensor( (*itH)->sensor() );
            double x = x0 + tx * _sensor->z();
            double y = y0 + ty * _sensor->z();
            (*itH)->setZ( _sensor->z( x, y ) );
          }
          FastVeloTrack temp;
          temp.setPhiClusters( input, x0, tx, y0, ty, *itH1, best2, best3 );
          if ( m_debug ) printTrack( temp, "Initial track" );
          bool ok = temp.removeWorstMultiple( m_maxChi2PerHit, 3 );
          if ( !ok ) {
            if ( m_debug ) info() << "Triplet not acceptable: not enough good phi " << endmsg;
            continue;
          }
          if ( inOverlap ) {
            temp.addBestClusterOtherSensor( goodPhiHits[s1], m_maxChi2ToAdd );
            temp.addBestClusterOtherSensor( goodPhiHits[s2], m_maxChi2ToAdd );
            temp.addBestClusterOtherSensor( goodPhiHits[s3], m_maxChi2ToAdd );
            ok = temp.removeWorstMultiple( m_maxChi2PerHit, 3 );
            if ( !ok ) {
              if ( m_debug ) info() << "After adding other side: Not enough phi " << endmsg;
              continue;
            }
          }
          temp.updateRParameters();

          int nbMissed = 0;
          double lastZ = best3->z();
          for ( unsigned int s = s3 + stationStep; lastStation+stationStep != s; s += stationStep ){
            if ( goodPhiHits[s].size() != 0 ) {
              if ( m_debug ) {
                info() << "Try to add more on station " << s << endmsg;
                for ( itH = goodPhiHits[s].begin(); goodPhiHits[s].end() != itH; ++itH ) {
                  info() << format( "Dist%8.3f Chi2%8.2f", temp.distance( *itH ), temp.chi2( *itH ) );
                  printCoord( *itH, "Selected " );
                }
              }
              double addChi2 = m_maxChi2ToAdd;
              if ( fabs( goodPhiHits[s].front()->z() - lastZ ) > 140. ) addChi2 = 4 * addChi2;
              bool ok1 = temp.addBestPhiCluster( goodPhiHits[s], addChi2 );
              if ( !ok1 ) {
                nbMissed++;
              } else {
                if ( inOverlap ) temp.addBestClusterOtherSensor( goodPhiHits[s], m_maxChi2ToAdd );
                if ( m_debug ) printTrack( temp, "After adding" );
              }
            } else {
              nbMissed++;
            }
            if ( nbMissed  > 2 ) break;
          }
          temp.updateRParameters();
          ok = temp.removeWorstMultiple( m_maxChi2PerHit, minNbPhi );
          if ( !ok ) {
            if ( m_debug ) info() << "Rejected , not enough phi = "
                                  << temp.phiHits().size() << " for " << minNbPhi << endreq;
            continue;
          }

          //== Overall quality should be good enough...
          if ( m_maxQFactor < temp.qFactor() ) {
            if ( m_debug ) info() << "Rejected , qFactor = " << temp.qFactor() << endreq;
            continue;
          }
          //== Check that there are phi hits on the same side as the R hits...
          int nbPhiLeft = 0;
          int nbPhiRight = 0;
          for ( itH = temp.phiHits().begin(); temp.phiHits().end() != itH ; ++itH ) {
            if (  m_hitManager->sensor( (*itH)->sensor() )->isRight() ) {
              nbPhiRight++;
            } else {
              nbPhiLeft++;
            }
          }
          if ( nbLeft * nbPhiLeft + nbRight * nbPhiRight == 0 ) {
            if ( m_debug ) {
              info() << "Phi and R are all on opposite sides: Left R " << nbLeft << " Phi " << nbPhiLeft
                     << "  Right R " << nbRight << " phi " << nbPhiRight << ". Reject" << endmsg;
            }
            continue;
          }

          if ( ok ) {  //== Store it.
            if ( m_debug ) {
              info() << "**** Accepted qFactor : " << temp.qFactor() << " ***" << endmsg;
              printTrack( temp );
            }
            if ( temp.phiHits().size() >= temp.rHits().size() ) hasFullLength = true;
            newTracks.push_back( std::move(temp) );
          }
        }
      }
    }
    if ( iCase > 0 && hasFullLength ) break;
  }

  //== If no track found AND in overlap: Try all hits first
  if ( inOverlap && 0 == newTracks.size() ) {
    s1 = firstStation;
    s2 = s1 + stationStep;
    s3 = s2 + stationStep;
    if ( s1 < 21 && s2 < 21 && s3 < 21 ) {  // protect agains access to no existent modules
      FastVeloHits all;
      all.reserve(200);
      station = s1;
      while ( lastStation+stationStep != station ) {
        all.insert( all.end(), goodPhiHits[station].begin(), goodPhiHits[station].end() );
        station += stationStep;
        if ( station > 20 ) break;
      }
      if( !(all.begin() == all.end()) ){ // skip cases with zero clusters
        FastVeloTrack temp;
        temp.setPhiClusters( input, m_cosPhi, m_sinPhi, all.begin(), all.end() );
        if ( m_debug ) {
          info() << "+++ Try with all hits on stations " << s1 << " to " << station
                 << ", minNbPhi " << minNbPhi << endmsg;
          printTrack( temp );
        }
        bool ok = temp.removeWorstMultiple( m_maxChi2PerHit, minNbPhi );
        if ( ok ) {
          //== Overall quality should be good enough...
          if ( m_maxQFactor < temp.qFactor() ) {
            if ( m_debug ) info() << "Rejected , qFactor = " << temp.qFactor() << endreq;
            ok = false;
          }
          
          if ( ok ) {  //== Store it.
            if ( m_debug ) {
              info() << "**** Accepted qFactor : " << temp.qFactor() << " ***" << endmsg;
              printTrack( temp );
            }
            newTracks.push_back( std::move(temp) );
          }
        }
      }
    }
  }

  //== Last chance: This is a fake overlap. Split....
  if ( inOverlap && 0 == newTracks.size() ) {
    FastVeloHits other;
    for ( itH = input.rHits().begin(); input.rHits().end() != itH; ++itH ) {
      if ( (*itH)->zone() != (input.zone()%4) ) {
        other.push_back( *itH );
        itH = input.rHits().erase( itH );
        itH--;
      }
    }
    bool localDebug = m_debug;
    if ( localDebug ) {
      info() <<"=== Try with hits from the Main zone itself ===" << endmsg;
      printCoords( input.rHits(), "Main " );
    }
    makeSpaceTracks( input );
    input.rHits() = other;
    input.setZone( 7 - input.zone() );
    if ( localDebug ) {
      info() <<"=== Try with hits from the other zone ===" << endmsg;
      printCoords( input.rHits(), "Other" );
    }
    makeSpaceTracks( input );
    return;
  }

  //== Any need for merging tracks ?
  if ( m_debug ) info() <<"Before merge clones, size " << newTracks.size() << endmsg;
  mergeClones( newTracks );

  FastVeloTracks::iterator itTr;
  double maxQual = 1.e10;

  unsigned int maxLen = 0;
  //== Get the properties of the best candidate. Invalidate tracks with 3 or 4 clusters and all already used.
  for ( itTr = newTracks.begin(); newTracks.end() != itTr; ++itTr ) {
    if ( !(*itTr).isValid() ) continue;
    if ( 4 >   (*itTr).phiHits().size() &&
         0 ==  (*itTr).nbUnusedPhiHits()  ) {
      (*itTr).setValid( false );
      if ( m_debug )  printTrack( *itTr, "Reject track: less than 4 phi, all already used..." );
      continue;
    }
    if ( (*itTr).phiHits().size() >= maxLen ) maxLen = (*itTr).phiHits().size();
  }
  unsigned int minExpected = maxLen;
  if ( 4 < maxLen ) minExpected = minExpected-1;
  for ( itTr = newTracks.begin(); newTracks.end() != itTr; ++itTr ) {
    if ( !(*itTr).isValid() ) continue;
    if ( minExpected <= (*itTr).phiHits().size() ) {
      if ( maxQual > (*itTr).qFactor() )  maxQual = (*itTr).qFactor();
    }
  }

  if ( 3 < maxLen ) {
    maxQual += m_deltaQuality;
    if ( sumSize > 1.9 * input.rHits().size() ) maxQual += 1. ;
  }
  
  //== Store the good candidates
  bool foundSpaceTrack = false;
  for ( itTr = newTracks.begin(); newTracks.end() != itTr; ++itTr ) {
    if ( !(*itTr).isValid() ) continue;
    if ( m_debug ) {
      info() << "Test track " << itTr - newTracks.begin() << " nbPhiHits " << (*itTr).phiHits().size()
             << " (min " << minExpected << ") qFact " << (*itTr).qFactor()
             << " (max " << maxQual << ")";
    }
    if ( minExpected > (*itTr).phiHits().size() ||
         maxQual     < (*itTr).qFactor() ) {
      if ( m_debug ) info() << endmsg;
      continue;
    }

    //== Stronger test on 3+3 hit tracks
    if ( (*itTr).rHits().size() == 3 &&
         (*itTr).phiHits().size() == 3 ) {
      if ( (*itTr).qFactor() > m_maxQFactor3 ) {
        if ( m_debug ) {
          info() << "Short track. qFactor too low : " << (*itTr).qFactor() << endmsg;
          printTrack( *itTr );
        }
        continue;
      }
    }
    if ( !(*itTr).removeWorstRAndPhi( m_maxChi2PerHit, 6 ) ) {
      if ( m_debug ) {
        info() << "Track with less than 3 R or Phi after cleanup" << endmsg;
        printTrack( *itTr );
      }
      continue;
    }

    if ( m_debug ) {
      info() << " ... stored as " << m_spaceTracks.size() << endmsg;
      printTrack( *itTr );
    }
    (*itTr).tagUsedPhiHits();
    (*itTr).setBackward( input.backward() );
    m_spaceTracks.push_back( std::move(*itTr) );
    foundSpaceTrack = true;
  }
  if ( !foundSpaceTrack ) {
    if ( m_minToTag <= input.nbRHits() ) input.untagUsedRHits();
  }
}

//=========================================================================
//  Merge tracks sharing hits, tag clones.
//=========================================================================
void FastVeloTracking::mergeClones ( FastVeloTracks& tracks ) {
  if ( tracks.size() <= 1 ) return;
  FastVeloTracks::iterator it1, it2;
  for ( it1 = tracks.begin(); tracks.end()-1 > it1 ; ++it1 ) {
    int n1 = (*it1).phiHits().size();
    for ( it2 = it1+1; tracks.end() != it2 ; ++it2 ) {
      if ( !(*it2).isValid() ) continue;
      int n2 = (*it2).phiHits().size();
      int minN = n1 > n2 ? n2 : n1;
      int nCommon = 0;
      FastVeloHits::const_iterator itH;
      for ( itH = (*it1).phiHits().begin(); (*it1).phiHits().end() != itH ; ++itH ) {
        if ( std::find( (*it2).phiHits().begin(), (*it2).phiHits().end(), *itH ) != (*it2).phiHits().end() ) ++nCommon;
      }
      if ( nCommon > m_fractionForMerge * minN ) {
        if ( n2 > n1 ) {
          (*it1).setValid( false );
        } else if ( n1 > n2 ) {
          (*it2).setValid( false );
        } else if ( (*it1).qFactor() < (*it2).qFactor() ) {
          (*it2).setValid( false );
        } else {
          (*it1).setValid( false );
        }
      }
    }
  }
}
//=========================================================================
//  Try to find tracks with unused phi clusters, only forward
//=========================================================================
void FastVeloTracking::findUnusedPhi( ) {
  m_debug = m_isDebug;
  if ( 0 <= m_wantedKey ) info() << endmsg << "===== Unused Phi procesing ====" << endmsg;

  double phiUnusedFirstTol  = m_phiUnusedFirstTol;
  double phiUnusedSecondTol = m_phiUnusedSecondTol;
  double maxQFactor         = m_maxQFactor;
  int firstSensor = m_hitManager->lastPhiSensor();
  int lastSensor  = firstSensor - 15;
  if ( m_tracks.size() < 10 ) {    // clean event (e.g. Velo open)
    phiUnusedFirstTol  = 20.;
    phiUnusedSecondTol = 20.;
    maxQFactor         =  2.;
    lastSensor         = firstSensor - 30;
  }

  for ( int phi0 = firstSensor; lastSensor <= phi0; --phi0 ) {
    int phi1 = phi0 - 2;
    int phi2 = phi1 - 2;
    FastVeloSensor* s0 = m_hitManager->sensor( phi0 );
    FastVeloSensor* s1 = m_hitManager->sensor( phi1 );
    FastVeloSensor* s2 = m_hitManager->sensor( phi2 );
    FastVeloSensor* r0 = m_hitManager->sensor( phi0-64 );
    FastVeloSensor* r1 = m_hitManager->sensor( phi1-64 );
    FastVeloSensor* r2 = m_hitManager->sensor( phi2-64 );
    for ( unsigned int zone = 0; 2 > zone ; ++zone ) {
      for ( FastVeloHits::iterator itH0 = s0->hits(zone).begin(); s0->hits(zone).end() != itH0; ++itH0 ) {
        if ( 0 != (*itH0)->nbUsed() ) continue;
        for ( unsigned int zone1 = 0; zone+1 > zone1 ; ++zone1 ) {
          for ( FastVeloHits::iterator itH1 = s1->hits(zone1).begin(); s1->hits(zone1).end() != itH1; ++itH1 ) {
            if ( 0 != (*itH1)->nbUsed() ) continue;
            double d1 = (*itH1)->distance( (*itH0)->xStripCentre(), (*itH0)->yStripCentre() );
            if (  matchKey( *itH0 ) && matchKey( *itH1 ) ) {
              info() << "S0 " << phi0 << " z0 " << zone << " S1 " << phi1 << " z1 " << zone1 << " d1 " << d1 << endmsg;
            }
            if ( fabs( d1 ) > phiUnusedFirstTol ) continue;
            double dx = (*itH0)->xStripCentre() -  (*itH1)->xStripCentre();
            double dy = (*itH0)->yStripCentre() -  (*itH1)->yStripCentre();
            if ( fabs(dx) > phiUnusedFirstTol ) continue;
            if ( fabs(dy) > phiUnusedFirstTol ) continue;
            double xSeed = .5 * ( (*itH0)->xStripCentre() +  (*itH1)->xStripCentre() );
            double ySeed = .5 * ( (*itH0)->yStripCentre() +  (*itH1)->yStripCentre() );

            if ( 0 != m_debugTool ) {
              m_debug = m_isDebug;
              if (  matchKey( *itH0 ) || matchKey( *itH1 ) ) {
                m_debug = true;
                info() << "Extrapolate, d1 " << d1 << endmsg;
                printCoord( *itH0, "phi0" );
                printCoord( *itH1, "phi1" );
              }
            }

            for ( unsigned int zone2 = 0; zone1+1 > zone2 ; ++zone2 ) {
              for ( FastVeloHits::iterator itH2 = s2->hits(zone2).begin(); s2->hits(zone2).end() != itH2; ++itH2 ) {
                if ( 0 != (*itH2)->nbUsed() ) continue;
                double d2 = (*itH2)->distance( xSeed, ySeed );
                if ( m_debug ) {
                  info() << format( " ++ D2 %8.3f", d2 );
                  printCoord( *itH2, " : " );
                }

                if ( fabs( d2 ) > phiUnusedSecondTol ) continue;

                if ( m_debug ) {
                  info() << "*** FindUnusedPhi : phi triplet, d2 = " << d2 << endmsg;
                  printCoord( *itH0, "phi0" );
                  printCoord( *itH1, "phi1" );
                  printCoord( *itH2, "phi2" );
                }

                int rZone = 0;
                if ( fabs( xSeed - r0->xCentre() ) > fabs( ySeed - r0->yCentre() ) ) rZone = 1;
                if ( xSeed * ySeed > 0 ) rZone = 3 - rZone;
                if ( m_debug ) {
                  info() << "Looking in R zone " << rZone << " sensor " << r0->number()
                         << " xSeed " << xSeed << " ySeed " << ySeed
                         << " dx " << dx << " dy " << dy
                         << " xCentre " << r0->xCentre() << " yCentre " << r0 -> yCentre() << endmsg;
                }
                for ( FastVeloHits::const_iterator itR0 = r0->hits(rZone).begin();
                      r0->hits(rZone).end() != itR0; ++itR0 ) {
                  if ( (*itR0)->rLocal() < s0->rMin( zone ) ||
                       (*itR0)->rLocal() > s0->rMax( zone ) ) continue;
                  (*itR0)->setStartingPoint( xSeed, ySeed );
                  int nbUnused = 0;
                  if ( 0 == (*itR0)->nbUsed() ) nbUnused++;

                  FastVeloTrack temp;
                  int spaceZone = rZone;
                  if ( xSeed < 0 ) spaceZone = 4 + rZone;
                  temp.setPhiClusters( *itR0, spaceZone, *itH0, *itH1, *itH2 );
                  temp.updatePhiWeights();
                  if ( temp.rAtZ( r1->z() ) > 1000. ) continue;

                  if ( m_debug ) {
                    double xLocal = temp.xAtZ( r1->z() ) - r1->xCentre();
                    double yLocal = temp.yAtZ( r1->z() ) - r1->yCentre();
                    double rLocal = sqrt( xLocal * xLocal + yLocal * yLocal );
                    info() << format( "rPred %8.3f zone%2d", rLocal, rZone );
                    printCoord( *itR0, "R0  " );
                  }

                  if ( !temp.addBestRCluster( r1, 400.) ) continue;
                  temp.updatePhiWeights();

                  if ( 0 == temp.rHits().back()->nbUsed() ) nbUnused++;
                  if ( m_debug ) {
                    double xLocal = temp.xAtZ( r2->z() ) - r2->xCentre();
                    double yLocal = temp.yAtZ( r2->z() ) - r2->yCentre();
                    double rLocal = sqrt( xLocal * xLocal + yLocal * yLocal );
                    info() << format( "rPred %8.3f zone%2d", rLocal, rZone );
                    printCoord(  temp.rHits().back(), "R1  " );
                  }

                  if ( !temp.addBestRCluster( r2, m_maxChi2ToAdd ) ) continue;
                  if ( 0 == temp.rHits().back()->nbUsed() ) nbUnused++;

                  if ( m_debug ) {
                    info() << format( "qFactor %8.3f NbUnused%2d  ", temp.qFactor(), nbUnused );
                    printCoord(  temp.rHits().back(), "R2  " );
                    printTrack( temp );
                  }

                  if ( 2 > nbUnused ) continue;

                  if ( temp.qFactor() > maxQFactor ) continue;
                  FastVeloSensor* s = r2;
                  int nMiss = 0;
                  while ( 0 <= s->next( true ) && nMiss <= m_maxMissed ) {
                    s = m_hitManager->sensor( s->next( true ) );
                    if ( temp.rAtZ( s->z() ) < s->rMin() ) break;
                    if ( !temp.addBestRCluster( s, m_maxChi2ToAdd ) ) nMiss++;
                    FastVeloSensor* phiSensor = m_hitManager->sensor( s->number() + 64 );
                    int phiZone = 0;
                    double rInPhi = temp.rAtZ( phiSensor->z() );
                    if ( temp.rAtZ( phiSensor->z() ) > phiSensor->rMax( 0 ) ) phiZone = 1;
                    for ( FastVeloHits::iterator itH = phiSensor->hits(phiZone).begin();
                          phiSensor->hits(phiZone).end() != itH ; ++itH ) {
                      (*itH)->setPhiWeight(rInPhi );
                    }
                    if ( !temp.addBestPhiCluster( phiSensor->hits(phiZone), m_maxChi2ToAdd ) ) nMiss++;
                  }

                  if ( !temp.removeWorstRAndPhi( m_maxChi2PerHit, 6 ) ) {
                    if ( m_debug ) {
                      info() << "Track with less than 3 R or Phi after cleanup" << endmsg;
                      printTrack( temp );
                    }
                    continue;
                  }

                  if ( m_debug ) {
                    info() << "*** Good track, qFactor " << temp.qFactor() << endmsg;
                    printTrack( temp );
                  }
                  temp.tagUsedRHits();
                  temp.tagUsedPhiHits();
                  m_spaceTracks.push_back( std::move(temp) );
                }
              }
            }
          }
        }
      }
    }
  }
  m_debug = m_isDebug;
}

//=========================================================================
//  Debug tool: Print the coordinate
//=========================================================================
void FastVeloTracking::printCoord( const FastVeloHit* hit, std::string title ) {

  info() << "  " << title
         << format( " sensor %3d z%7.1f zone%2d strip%5d coord%8.3f local%8.3f size%2d frac %5.2f used%2d ",
                    hit->sensor(), hit->z(), hit->zone(), hit->cluster().channelID().strip(), hit->global(), hit->rLocal(),
                    hit->cluster().pseudoSize(), hit->cluster().interStripFraction() , hit->nbUsed() );
  LHCb::LHCbID myId =  hit->lhcbID();
  if ( 0 != m_debugTool ) m_debugTool->printKey( info(), myId );
  if ( matchKey( hit ) ) info() << " ***";
  info() << endreq;
}


//=========================================================================
//  Debug a track, with all its hits
//=========================================================================
void FastVeloTracking::printTrack( FastVeloTrack& track, std::string header ) {
  if ( "" != header ) info() << "*** " << header << " ***" << endmsg;
  for ( FastVeloHits::iterator itH = track.rHits().begin(); track.rHits().end() != itH ; ++itH ) {
    info() << format( "Dist%8.3f Chi2%8.2f", track.distance( *itH ), track.chi2( *itH ) );
    printCoord( *itH, " R   " );
  }
  for ( FastVeloHits::iterator itH = track.phiHits().begin(); track.phiHits().end() != itH ; ++itH ) {
    info() << format( "Dist%8.3f Chi2%8.2f", track.distance( *itH ), track.chi2( *itH ) );
    printCoord( *itH, " Phi " );
  }
  info() << endmsg;
}

//=========================================================================
//  Debug a R track, with all its hits
//=========================================================================
void FastVeloTracking::printRTrack( FastVeloTrack& track, std::string header ) {
  if ( "" != header ) info() << "*** " << header << " ***" << endmsg;
  for ( FastVeloHits::iterator itH = track.rHits().begin(); track.rHits().end() != itH ; ++itH ) {
    double d = track.rPred( (*itH)->z() )-(*itH)->global();
    info() << format( "Dist%8.3f Chi2%8.2f", d, d * d * (*itH)->weight() );
    printCoord( *itH, " R " );
  }
  info() << endmsg;
}
//=========================================================================
//  Print a vector of hits
//=========================================================================
void FastVeloTracking::printCoords( FastVeloHits& hits, std::string header ) {
  for ( FastVeloHits::iterator itH = hits.begin(); hits.end() != itH ; ++itH ) {
    printCoord( *itH, header );
  }
}
//=============================================================================
