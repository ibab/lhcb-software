// Include files

// local
#include "NewVeloRTracking.h"

//-----------------------------------------------------------------------------
// Implementation file for class : NewVeloRTracking
//
// 2009-08-28 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( NewVeloRTracking )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NewVeloRTracking::NewVeloRTracking( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_velo(NULL)
  , m_hitManager(NULL)
  , m_debugTool(NULL)
  , m_timer(NULL)
{
  declareProperty( "ZVertexMin"      , m_zVertexMin = -170. *Gaudi::Units::mm );
  declareProperty( "ZVertexMax"      , m_zVertexMax = +120. *Gaudi::Units::mm );
  declareProperty( "MaxRSlope"       , m_maxRSlope        = 0.450     );
  declareProperty( "rMatchTol"       , m_rMatchTol        = 0.90      );
  declareProperty( "rExtraTol"       , m_rExtraTol        = 3.5       );
  declareProperty( "rOverlapTol"     , m_rOverlapTol      = 0.6       );
  declareProperty( "MaxMissed"       , m_maxMissed        = 1         );
  declareProperty( "MinToTag"        , m_minToTag         = 4         );
  declareProperty( "MergeTracks"     , m_mergeTracks      = false     );
  declareProperty( "NCommonToMerge"  , m_nCommonToMerge   = 2         );
  declareProperty( "AdjacentSectors" , m_adjacentSectors  = false     );
  declareProperty( "OnlyForward"     , m_onlyForward      = false     );
  declareProperty( "OnlyBackward"    , m_onlyBackward     = false     );

  declareProperty( "OutputTracksName", m_outputLocation   = LHCb::TrackLocation::RZVelo     );
  declareProperty( "RHitManagerName" , m_rHitManagerName  = "PatVeloRHitManager"   );
  declareProperty( "DebugToolName" ,   m_debugToolName    = ""     );
  declareProperty( "WantedKey"       , m_wantedKey        = -100   );
  declareProperty( "MeasureTime"     , m_measureTime      = false  );

}
//=============================================================================
// Destructor
//=============================================================================
NewVeloRTracking::~NewVeloRTracking() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode NewVeloRTracking::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  m_hitManager  = tool<Tf::PatVeloRHitManager>  ( "Tf::PatVeloRHitManager", m_rHitManagerName );
  m_velo        = getDet<DeVelo>( DeVeloLocation::Default );
  m_debugTool   = 0;
  if ( "" != m_debugToolName ) m_debugTool = tool<IPatDebugTool>( m_debugToolName );
  m_timer = 0;
  if ( m_measureTime ) {
    m_timer = tool<ISequencerTimerTool>( "SequencerTimerTool", "Timing", this );
    m_totalTime = m_timer->addTimer( "Total" );
    m_timer->increaseIndent();
    m_timer->decreaseIndent();
  }
  m_zSensorSearchMin = 0.;
  m_zSensorSearchMax = 0.;

  info() << "zMin " << m_zSensorSearchMin << " zMax " << m_zSensorSearchMax << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode NewVeloRTracking::execute() {

  m_isDebug   = msgLevel( MSG::DEBUG   );
  if ( m_isDebug ) debug() << "==> Execute" << endmsg;

  LHCb::Tracks* outputTracks;
  if ( exist<LHCb::Tracks>( m_outputLocation ) ) {
    outputTracks = get<LHCb::Tracks>( m_outputLocation );
  } else {
    outputTracks = new LHCb::Tracks();
    put(outputTracks, m_outputLocation );
  }
  outputTracks->reserve(500);

  std::vector<Tf::PatRZTrack> rzTracks; ///< working space to store tracks
  rzTracks.reserve( 400 );

  //== If needed, debug the cluster associated to the requested MC particle.
  if ( 0 <= m_wantedKey ) {
    info() << "--- Looking for Track " << m_wantedKey << endmsg;
    for ( int half = 0; 2 > half; ++half ) {
      StationIterator stationIter = m_hitManager->stationsHalfBegin(half);
      StationIterator stationsEnd = m_hitManager->stationsHalfEnd(half);
      for ( ; stationIter != stationsEnd; ++stationIter ) {
        Tf::PatVeloRHitManager::Station* station = *stationIter;
        if ( !station->sensor()->isReadOut() ) continue; // jump sensors not in readout

        for ( unsigned int zone = 0; 4 > zone ; ++zone ) {
          for ( std::vector<Tf::PatVeloRHit*>::const_iterator itH = station->hits( zone ).begin();
                station->hits( zone ).end() != itH; ++itH ) {
            if ( matchKey( *itH ) ) printCoord( *itH, " " );
          }
        }
      }
    }
  }

  if(!m_onlyBackward){
    //== Two iterations: First the 4++ cluster tracks, then the rest, only unused.
    for ( unsigned int iter = 0 ; 2 > iter ; ++iter ) {
      //== Loop on velo halfs
      for (unsigned int half=0; half<2; ++half) {

        //== Loop on R stations in reverse z order
        //== stop when second station is at z < m_zSensorSearchMin
        StationReverseIterator stationReverseIter = m_hitManager->stationsHalfReverseBegin(half);
        StationReverseIterator stationsReverseEnd = m_hitManager->stationsHalfReverseEnd(half);
        for ( ; stationReverseIter != stationsReverseEnd; ++stationReverseIter ) {

          //== Loop on zones
          for ( unsigned int zone = 0 ; zone<4; ++zone ) {

            StationReverseIterator station0 = stationReverseIter;

            // nothing to see here, please move along
            if ((*station0)->empty(zone)) continue;

            // code to make "walk" down Velo stop
            // at last R sensor is before z=0 +ve -> 0
            if ( (*station0)->z() < m_zSensorSearchMin ) break;

            // are we at the end already?
            StationReverseIterator station1 = station0; ++station1;
            if ( station1 == stationsReverseEnd ) continue;

            // are we at the end already?
            StationReverseIterator station2 = station1; ++station2;
            if ( station2 == stationsReverseEnd ) continue;
            makeForwardSectorTriplets(station0, zone, station1, zone, station2, zone,
                                      stationsReverseEnd, zone, rzTracks, iter);
          }
        }
      }
    }
  }
  if(!m_onlyForward){

    if(m_isDebug){debug() << "========= Backwards tracks ==========" << endmsg;}

    //== Loop on velo halfs
    for (unsigned int half=0; half<2; ++half) {

      //== Loop on R stations in reverse z order
      //== stop when second station is at z < 0
      StationIterator stationIter = m_hitManager->stationsHalfBegin(half);
      StationIterator stationsEnd = m_hitManager->stationsHalfEnd(half);
      for ( ; stationIter != stationsEnd; ++stationIter ) {

        //== Loop on zones
        for ( unsigned int zone = 0 ; zone<4; ++zone ) {

          //== stop when second sensor is at z > 0

          StationIterator station0 = stationIter;

          //== nothing to see here move along
          if ( (*station0)->empty(zone) ) continue;

          StationIterator station1 = station0; ++station1;
          if ( station1 == stationsEnd ) continue;

          // code to make "walk"  Velo stop at z=0 -ve -> 0
          if ( (*station1)->z() > m_zSensorSearchMax ) break;
          if ( (*station1)->empty(zone) ) continue;

          StationIterator station2 = station1; ++station2;
          if ( station2 == stationsEnd ) continue;
          if ( (*station2)->empty(zone) ) continue;

          seedInSectorTriplet( false, rzTracks, 0, zone, station0, zone, station1, zone, station2, zone);

        } // zone loop
      } // station loop
      //      StationReverseIterator stationReverseIter = m_hitManager->stationsHalfReverseBegin(half);
    } // velo half loop
  }

  //== Merge tracks with many common hits
  if ( m_mergeTracks ) mergeTracks( rzTracks );

  //==========================================================================
  //== Store the tracks in the Track tool here...
  //==========================================================================

  for ( std::vector<Tf::PatRZTrack>::iterator itRzt = rzTracks.begin();
        rzTracks.end() != itRzt; ++itRzt ) {
    Tf::PatRZTrack* tr = &(*itRzt);
    if ( tr->valid() ) {

      // tag all hits including those not yet tagged
      tr->tagUsedCoords();

      LHCb::Track* track = new LHCb::Track(outputTracks->size());

      // set a type for the track
      track->setType( LHCb::Track::VeloR );
      track->setHistory( LHCb::Track::PatVeloR );
      track->setSpecific( tr->zone() );

      track->setFlag( LHCb::Track::Backward, tr->backward() );

      double z = tr->meanZ();
      Gaudi::TrackVector state;
      state[0] = tr->rPred(z);
      state[2] = tr->rSlope();
      state[1] = (tr->zone()*Gaudi::Units::pi/4.)-(3.*Gaudi::Units::pi/8.);

      LHCb::State temp;
      temp.setZ( z );
      temp.setState( state );
      Gaudi::TrackSymMatrix & cov = temp.covariance();
      cov(0,0) =  tr->errR2();// set 0,0 to errR2
      cov(2,2) = tr->errSl2();// set 2,2 to errSl2
      track->addToStates( temp );
      track->setChi2AndDoF(tr->chi2(),tr->nbCoords()-2);
      // also add the number of found + missed stations
      // to the track information for ghost rejection later if required
      track->addInfo(LHCb::Track::nPRVeloRZExpect,
                     static_cast<double>(tr->nbCoords()+
                                         tr->missedStations()) );

      // store details of hits on track
      for ( Tf::PatVeloRHits::iterator itC = tr->coords()->begin();
            tr->coords()->end() != itC; ++itC ) {
        track->addToLhcbIDs( (*itC)->lhcbID() );
      }
      track -> setPatRecStatus( LHCb::Track::PatRecIDs );
      outputTracks->insert(track);
    }
  }

  if(m_isDebug){
    debug() << "Stored " << outputTracks->size() << " tracks in "
            << m_outputLocation << " from " << rzTracks.size()
            << " rz tracks found" << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode NewVeloRTracking::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


//=========================================================================
//
//=========================================================================
void NewVeloRTracking::makeForwardSectorTriplets( StationReverseIterator station0, unsigned int zone0,
                                                  StationReverseIterator station1, unsigned int zone1,
                                                  StationReverseIterator station2, unsigned int zone2,
                                                  StationReverseIterator stationsEnd,
                                                  unsigned int sector,
                                                  std::vector<Tf::PatRZTrack> &rzTracks,
                                                  unsigned int iter ){

  // going to search for three matching clusters
  // do this with three cases
  for ( int iCase = 0; 3 > iCase; ++iCase ){
    if ( 0 != iter && 0 != iCase ) break; // only oCse = 0 for second step

    //case 0, clusters in consevutive stations
    if ( 1 == iCase ) {
      // case 1, cluster missing in third station
      ++station2;
      if (station2 == stationsEnd) return;
    } else if ( 2 == iCase ) {
      // case 2, cluster missing in second station
      ++station1;
    }

    // if any of sectors has no clusters continue
    if ( (*station1)->empty(zone1) ) continue;
    if ( (*station2)->empty(zone2) ) continue;

    // convert reverse iterators to iterators
    StationReverseIterator tmpRI;
    tmpRI = station0;
    StationIterator sIter0 = (++tmpRI).base();
    tmpRI = station1;
    StationIterator sIter1 = (++tmpRI).base();
    tmpRI = station2;
    StationIterator sIter2 = (++tmpRI).base();

    seedInSectorTriplet( true, rzTracks,
                         iCase, sector,
                         sIter0, zone0,
                         sIter1, zone1,
                         sIter2, zone2,
                         iter );
  }
  return;
}

//=========================================================================
//
//=========================================================================
void NewVeloRTracking::seedInSectorTriplet( bool forward, std::vector<Tf::PatRZTrack> &rzTracks,
                                            const int &iCase, unsigned int sector,
                                            StationIterator station0, unsigned int zone0,
                                            StationIterator station1, unsigned int zone1,
                                            StationIterator station2, unsigned int zone2,
                                            unsigned int iter ) {

  // skip if any of the sensors are not in the DAQ
  if( !(*station0)->sensor()->isReadOut()
      || !(*station1)->sensor()->isReadOut()
      || !(*station2)->sensor()->isReadOut() ) return;

  double z0 = (*station0)->z();
  double z1 = (*station1)->z();
  double z2 = (*station2)->z();

  double zFrac = (z1-z0)/ (z2-z0);

  if ( m_isDebug ) {
    debug()<< "Checking sensors [" << (*station0)->sensor()->sensorNumber()
           << "," << zone0
           << "] [" << (*station1)->sensor()->sensorNumber()
           << "," << zone1
           << "] [" << (*station2)->sensor()->sensorNumber()
           << "," << zone2
           << "]  zone " << zone0
           << " nbCoord " << (*station0)->size(zone0)
           << " " << (*station1)->size(zone1)
           << " " << (*station2)->size(zone2)
           << endmsg;
  }

  const DeVeloRType* rSensor1 = (*station1)->sensor();

  // get first cluster in third sensor
  RHitRange hits2 = (*station2)->hits(zone2);
  RHitRange::const_iterator  first2 = hits2.begin();
  // iterators for the othe sensors
  RHitRange::const_iterator c0, c2;
  // loop over all possible clusters in first station
  RHitRange hits0 = (*station0)->hits(zone0);
  for ( c0 = hits0.begin(); hits0.end() != c0 ; ++c0 ) {
    if ( (*c0)->hit()->isUsed() ) continue; // cluster already used
    double r0 = (*c0)->coordHalfBox();

    // calculate min and max r of cluster in first sensor
    double rMin,rMax;
    if(forward){ // tracks should have a +ve gradient in R/Z
      rMin = r0 + m_maxRSlope * (z2 - z0);
      rMax = r0 * ( z2 - m_zVertexMin ) / ( z0 - m_zVertexMin );
      if ( 0 != iter ) rMax =  r0;  // more tolerant, get K/Lambda decay...
    }else{ // tracks should have -ve gradient
      rMin = r0 - m_maxRSlope * (z2 - z0);
      rMax = r0 * ( z2 - m_zVertexMax ) / ( z0 - m_zVertexMax );
    }

    if ( 0 != m_debugTool ) {
      if ( matchKey( *c0    ) ) {
        info() << format( "rMin  %8.3f rMax %8.3f ", rMin, rMax );
        printCoord ( *c0,   "St0 " );
      }
    }

    // loop over clusters in third station finding a possible match
    for ( c2 = first2; hits2.end() != c2 ; ++c2 ) {

      if ( 0 != iCase && (*c2)->hit()->isUsed() ) continue;
      if ( 0 != iter  && (*c2)->hit()->isUsed() ) continue; // cluster already used

      double r2 = (*c2)->coordHalfBox();
      if ( rMin > r2 ) {
        first2 = c2;
        continue;
      }
      if ( rMax <= r2 ) break;

      // from first and third clusters predict position of second cluster
      double rPred = r0 + zFrac * (r2-r0);

      //const DeVeloRType* rSensor=m_velo->rSensor(chId.sensor());
      double rPitch = rSensor1->rPitch( rPred );
      double rTol = m_rMatchTol * rPitch;

      if ( 0 != m_debugTool ) {
        if ( matchKey( *c0 ) && matchKey( *c2) ) {
          info() << format( "rPred %8.3f rTol %8.3f ", rPred, rTol );
          printCoord ( *c0,   "St0 " );
        }
      }

      // try to find a matching co-ord
      Tf::PatVeloRHit* bestc1 = (*station1)->closestHitHalfBox(zone1, rPred, rTol );
      if ( 0 == bestc1 ) continue;
      if ( 0 != iter  && bestc1->hit()->isUsed() ) continue; // cluster already used

      //=== We have 3 aligned points
      m_debug = m_isDebug;
      if ( 0 != m_debugTool ) {
        if ( matchKey( *c0    ) ||
             matchKey( bestc1 ) ||
             matchKey( *c2    ) ) m_debug = true;
        if ( m_debug ) {
          printCoord ( *c0,   "St0 " );
          printCoord ( bestc1,"St1 " );
          printCoord ( *c2,   "St2 " );
        }
      }

      // create track
      Tf::PatRZTrack newTrack;
      newTrack.addRCoord( *c0 );
      newTrack.addRCoord( bestc1 );
      newTrack.addRCoord( *c2 );

      //count "used" hits
      unsigned int nbUsed = 0; // first is unused by construction.
      if ( bestc1->hit()->isUsed() ) ++nbUsed;
      if ( (*c2)->hit()->isUsed()  ) ++nbUsed;

      int nMiss = extendTrack(newTrack,station2,zone2,nbUsed, m_rExtraTol, forward , 0);

      // geometrically only see tracks crossing from outer to outer sectors
      // when VELO is open, not at all when VELO is closed
      if(m_adjacentSectors && (zone0 == 1 || zone0 == 2)){
        int outerZone = 0;
        if( zone0 == 2 ){
          outerZone = 3;
        }
        // get the outer sector pointing to the last hit on the track
        StationIterator station2Outer =
          m_hitManager->stationIterHalf( newTrack.minSensor() );
        if( m_isDebug ) {
          debug() << "Extend into sens "<< (*station2Outer)->sensor()->sensorNumber()
                  << " sector " << outerZone << endmsg;
        }
        nMiss += extendTrack(newTrack, station2Outer, outerZone, nbUsed,
                             m_rExtraTol, forward , 0);
      }

      if ( 0 == iter &&  3 == newTrack.nbCoords() ) {
        if( m_debug ) info() << "On first pass, track should have more than 3 hits" << endmsg;
        continue;
      }

      //== More demanding for minimal tracks of 3 clusters
      if ( 3 == newTrack.nbCoords() ) {
        int nMissBack = extendTrack( newTrack, station0, zone0, nbUsed, m_rExtraTol, !forward , 0);
        if ( m_maxMissed <  nMissBack ) {
          if ( m_debug ) info() << "Short track with missed sensors before." << endmsg;
          continue;
        }
      }

      //== Ask for enough coordinates that have not previously
      // been used by other tracks
      // require at least 2 previously unused

      if ( 1 >= newTrack.nbCoords() - nbUsed ) {
        if( m_debug ) info() << "Killed too few new hits" << endmsg;
        continue;
      }

      // store the number of missed stations to put on LHCb::Track later
      newTrack.setMissedStations( nMiss );

      // define the global frame 45 degree zone in which the track was
      // found. Use the sectors of an upstream lefthand sensor 0->3
      // and continue counting 4->7 for sectors 0->3 of an upstream
      // right hand sensor
      int setSector = sector;
      if ( (*station0)->sensor()->isRight() ) setSector += 4;
      newTrack.setZone( setSector );
      if (!forward)  newTrack.setBackward(true);

      //== Try to add hits on the other half station, if sector 0/3
      // and forward and at least 5 hits long

      if ( forward
           //&& ( 4 < newTrack.nbCoords()  )
           && (0 == zone0 || 3 == zone0 ) ) {
        addOppositeSideHits(newTrack,zone0,(*station0)->sensor());
      }
      if ( forward ) {
        std::sort( newTrack.coords()->begin(), newTrack.coords()->end(),
                   Tf::VeloRHit::DecreasingByZ() );
      } else {
        std::sort( newTrack.coords()->rbegin(), newTrack.coords()->rend(),
                   Tf::VeloRHit::DecreasingByZ() );
      }

      //== Store. Tag hits if long enough
      rzTracks.push_back( newTrack );
      if ( m_debug ) info() << "      -- stored n=" << rzTracks.size()-1 << endmsg;
      if ( m_minToTag <= newTrack.nbCoords() ) {
        newTrack.tagUsedCoords();
      }
      break;
    }
  }
}

//=========================================================================
//
//=========================================================================
int NewVeloRTracking::extendTrack(Tf::PatRZTrack &newTrack,
                                  StationIterator station, unsigned int zone,
                                  unsigned int &nbUsed,
                                  double myTol,
                                  bool forward,
                                  Tf::PatVeloRHits *extraCoord){

  // determine where to stop
  unsigned int half = static_cast<unsigned int>((*station)->sensor()->isRight());
  StationIterator        stationsEnd        = m_hitManager->stationsHalfEnd(half);
  StationReverseIterator stationsReverseEnd = m_hitManager->stationsHalfReverseEnd(half);
  StationIterator tmpI(station);
  StationReverseIterator reverseStation(++tmpI);

  // Try to add other hits
  //== Extrapolate until too many consecutive hits missed,
  // or outside sensor
  int nMiss = 0;
  //  double lastZ = (*station)->z();
  bool notFirst = false;
  while ( m_maxMissed >= nMiss ) {

    // if not doing opposite side must update sectPt first, then for both every time
    if(!extraCoord  || notFirst){
      if(forward){
        ++reverseStation; // look in previous sensor (-z dir.)
        if ( reverseStation == stationsReverseEnd ) break;
        StationReverseIterator tmpRI = reverseStation;
        station = (++tmpRI).base();
      }else{
        ++station; // look in next sensor (+z dir.)
        if ( station == stationsEnd ) break;
      }
    }
    notFirst = true;

    if ( !((*station)->sensor()->isReadOut()) ) continue; // jump missing sensors
    double z      = (*station)->z();
    double rPred  = newTrack.rPred( z ); // new predicted co-ord
    double trTol  = sqrt( newTrack.rErr2( z ) );
    const DeVeloRType* rSensor=(*station)->sensor();
    double rPitch = rSensor->rPitch( rPred );
    double tol   = 4 * trTol + rPitch;
    if ( myTol < 1. ) tol = myTol * rPitch;   // for overlap

    if ( m_debug ) {
      info() << format( "       sensor %3d z%7.1f tol  %5.3f pred %8.3f",
                        (*station)->sensor()->sensorNumber(), z,  tol, rPred )
             << endmsg;
    }

    if ( (*station)->sensor()->rMin(zone) > rPred ) break;  // no longer inside.
    if ( (*station)->sensor()->rMax(zone) < rPred ) break;  // no longer inside.

    //== Select the best hit when extrapolating
    Tf::PatVeloRHit* bestc = (*station)->closestHitHalfBox( zone, rPred, tol );
    if ( 0 != bestc ) {
      // lastZ = z;
      // add hit to track or to temporary storage
      nMiss = 0;
      if(!extraCoord){ // i.e. adding directly to the track
        newTrack.addRCoord( bestc );
      }else{
        extraCoord->push_back( bestc );
      }
      if ( bestc->hit()->isUsed() ) ++nbUsed;
      if ( m_debug ) printCoord( bestc, "add " );
    } else {
      // no hit found
      ++nMiss;
    }
  }
  return nMiss;
}

//=========================================================================
//
//=========================================================================
void NewVeloRTracking::addOppositeSideHits(Tf::PatRZTrack &newTrack,const int &zone,
                                           const DeVeloRType* sensor ){

  Tf::PatVeloRHits extraCoords;

  const DeVeloRType* otherSideSensor = sensor->otherSideRSensor();
  if (!otherSideSensor) return;

  int iSens =  otherSideSensor->sensorNumber();

  // since zones are ordered in global phi, we always have to flip
  int otherZone = 3-zone;

  StationIterator station = m_hitManager->stationIterHalf( iSens);

  unsigned int nbUsed =0;
  extendTrack(newTrack,station, otherZone,nbUsed, m_rOverlapTol, true, &extraCoords);

  //== Store if relevant size...
  // track assumed to be fully passing through overlap region
  // hence min. hits is one less than on initial side
  unsigned int minExtra =  newTrack.nbCoords() - 1;
  if ( 3 > minExtra ) minExtra = 3;
  if ( minExtra <= extraCoords.size() ) {
    for ( unsigned int kk = 0; extraCoords.size() > kk; ++kk ){
      newTrack.addRCoord( extraCoords[kk] );
    }
    if ( m_isDebug ) {
      debug() << " ... Added " << extraCoords.size()
              << " extra R cooridnates" << endmsg;
    }
  }
}

//=========================================================================
//
//=========================================================================
void NewVeloRTracking:: mergeTracks ( std::vector<Tf::PatRZTrack> &rzTracks ) {
  std::vector<Tf::PatRZTrack>::iterator it1, it2;
  for ( it1 = rzTracks.begin(); rzTracks.end()-1 != it1; ++it1 ) {
    if ( !(*it1).valid() ) continue;
    if ( (*it1).backward() ) continue;
    for ( it2 = it1+1; rzTracks.end() != it2; ++it2 ) {
      if ( !(*it2).valid() ) continue;
      if ( (*it1).minSensor() > (*it2).maxSensor() - m_nCommonToMerge ) continue;
      if ( (*it2).minSensor() > (*it1).maxSensor() - m_nCommonToMerge ) continue;
      int nCommon = 0;
      int nCommonSensor = 0;
      Tf::PatVeloRHits::iterator itC1, itC2;
      for ( itC1 = (*it1).coords()->begin(); (*it1).coords()->end() != itC1; ++itC1 ) {
        if ( (*itC1)->sensorNumber() < (*it2).minSensor() ) continue;
        if ( (*itC1)->sensorNumber() > (*it2).maxSensor() ) continue;
        for ( itC2 = (*it2).coords()->begin(); (*it2).coords()->end() != itC2; ++itC2 ) {
          if ( (*itC1) == (*itC2) ) nCommon++;
          if ( (*itC1)->sensorNumber() == (*itC2)->sensorNumber() ) nCommonSensor++;
        }
        if ( nCommonSensor > nCommon ) break;
      }
      if ( m_nCommonToMerge <= nCommon ) {
        if ( msgLevel( MSG::DEBUG ) ) {
          debug() << "***** Found " << nCommon << " cluster in " << nCommonSensor
                  << " common sensors" << endmsg;
          debug() << " Track 1" << endmsg;
          for ( itC1 = (*it1).coords()->begin(); (*it1).coords()->end() != itC1; ++itC1 ) {
            printCoord( *itC1, " " );
          }
          debug() << " Track 2" << endmsg;
          for ( itC2 = (*it2).coords()->begin(); (*it2).coords()->end() != itC2; ++itC2 ) {
            printCoord( *itC2, " " );
          }
        }
        if ( nCommon == nCommonSensor ) {
          for ( itC2 = (*it2).coords()->begin(); (*it2).coords()->end() != itC2; ++itC2 ) {
            unsigned int mySens = (*itC2)->sensorNumber();
            bool foundSensor = false;
            for ( itC1 = (*it1).coords()->begin(); (*it1).coords()->end() != itC1; ++itC1 ) {
              if ( (*itC1)->sensorNumber() == mySens ) foundSensor = true;
            }
            if ( !foundSensor ) (*it1).addRCoord( *itC2 );
          }
          std::sort( (*it1).coords()->begin(), (*it1).coords()->end(),
                     Tf::VeloRHit::DecreasingByZ() );
          (*it2).setValid(false);
          if ( msgLevel( MSG::DEBUG ) ) {
            debug() << "---> Tracks are merged <--- Resulting track: " << endmsg;
            for ( itC1 = (*it1).coords()->begin(); (*it1).coords()->end() != itC1; ++itC1 ) {
              printCoord( *itC1, " " );
            }
          }
        }
      }
    }
  }
}

//=============================================================================
