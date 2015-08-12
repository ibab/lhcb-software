// Include files

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// output tracks and states
#include "Event/Track.h"
#include "Event/State.h"

#include "TfKernel/VeloRHit.h"

// local
#include "PatVeloRTracking.h"

namespace Tf {

  //-----------------------------------------------------------------------------
  // Implementation file for class : PatVeloRTracking
  //
  // 2005-05-31 : Olivier Callot
  //-----------------------------------------------------------------------------

  DECLARE_ALGORITHM_FACTORY( PatVeloRTracking )

  //=============================================================================
  // Standard constructor, initializes variables
  //=============================================================================
  PatVeloRTracking::PatVeloRTracking( const std::string& name,
      ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
    , m_hitManager(NULL)
    , m_trackTool(NULL)
    , m_velo(NULL)
    , m_timerTool(NULL)
    {
      declareProperty( "ZVertexMin"      , m_zVertexMin = -170. *Gaudi::Units::mm );
      declareProperty( "ZVertexMax"      , m_zVertexMax = +120. *Gaudi::Units::mm );
      declareProperty( "MaxRSlope"       , m_maxRSlope        = 0.400     );
      declareProperty( "rMatchTol"       , m_rMatchTol        = 0.90      );
      declareProperty( "rExtraTol"       , m_rExtraTol        = 3.5       );
      declareProperty( "rOverlapTol"     , m_rOverlapTol      = 0.6       );
      declareProperty( "MaxMissed"       , m_maxMissed        = 1         );
      declareProperty( "MinToTag"        , m_minToTag         = 4         );
      declareProperty( "ChargeThreshold" , m_chargeThreshold  = 15        );
      declareProperty( "highChargeFract" , m_highChargeFract  = .5        );
      declareProperty( "OutputTracksName", m_outputLocation   =
          LHCb::TrackLocation::RZVelo     );
      declareProperty( "HitManagerName", m_hitManagerName     =
          "PatVeloRHitManager"     );
      declareProperty( "TrackToolName",          m_trackToolName = "PatVeloTrackTool" );
      declareProperty( "MergeTracks"     , m_mergeTracks      = false     );
      declareProperty( "NCommonToMerge"  , m_nCommonToMerge   = 2         );
      declareProperty( "AdjacentSectors" , m_adjacentSectors  = false     );
      declareProperty( "OnlyForward"     , m_onlyForward      = false     );
      declareProperty( "OnlyBackward"    , m_onlyBackward     = false     );
      declareProperty( "OverlapCorrection" , m_OverlapCorrection = true   );
      declareProperty( "BackwardOverlapSearch" , m_backWardOverlap = true );
      declareProperty( "TimingMeasurement", m_doTiming = false);

    }
  //=============================================================================
  // Destructor
  //=============================================================================
  PatVeloRTracking::~PatVeloRTracking() {}


  //=============================================================================
  // Initialization
  //=============================================================================
  StatusCode PatVeloRTracking::initialize() {
    StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

    m_isDebug   = msgLevel( MSG::DEBUG   );
    m_isVerbose = msgLevel( MSG::VERBOSE );

    if(m_isDebug){debug() << "==> Initialize" << endmsg;}

    m_hitManager = tool<PatVeloRHitManager>( "Tf::PatVeloRHitManager", m_hitManagerName );
      m_trackTool = tool<PatVeloTrackTool>("Tf::PatVeloTrackTool",m_trackToolName);

    //== Get detector element
    m_velo = getDet<DeVelo>( DeVeloLocation::Default );

    if( msgLevel( MSG::DEBUG ) ){
      debug() << "========" << name() << " initialised ========" << endmsg;
    }
    if ( m_zVertexMin >= m_zVertexMax ){
      return Error("No overlap between forward and backward track z ranges",
		   StatusCode::FAILURE);
    }

    sc = registerConditionCallBacks();
    if (sc.isFailure()) {
      return Error("Failure to register condition update call backs.",
		   StatusCode::FAILURE);
    }

    if ( m_doTiming) {
      m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
      m_timerTool->increaseIndent();
      m_veloRZTime = m_timerTool->addTimer( "Internal VeloRZ" );
      m_timerTool->decreaseIndent();
    }

    return StatusCode::SUCCESS;
  }

  //=============================================================================
  // Main execution
  //=============================================================================
  StatusCode PatVeloRTracking::execute() {

    if(m_isDebug) {debug() << "==> Execute" << endmsg;}

    if ( m_doTiming ) m_timerTool->start( m_veloRZTime );
    
    LHCb::Tracks* outputTracks;
    if ( exist<LHCb::Tracks>( m_outputLocation ) ) {
      outputTracks = get<LHCb::Tracks>( m_outputLocation );
    } else {
      outputTracks = new LHCb::Tracks();
      put(outputTracks, m_outputLocation );
    }
    outputTracks->reserve(500);


    std::vector<PatRZTrack> rzTracks; ///< working space to store tracks
    rzTracks.reserve( 400 );

    if(!m_onlyBackward){
      //== Loop on velo halfs
      for (unsigned int half=0; half<2; ++half) {

        //== Loop on R stations in reverse z order
        //== stop when second station is at z < 0
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
                stationsReverseEnd, zone, rzTracks);

            // geometrically only see tracks crossing from outer to inner zones 
            // when VELO is open, not at all when VELO is closed
            if(m_adjacentSectors && (zone == 1 || zone == 2)){
              int outerZone = 0;
              if( zone == 2 ){
                outerZone = 3;
              }

              StationReverseIterator station0Outer = stationReverseIter;

              // are we at the end already?
              StationReverseIterator station1Outer = station0Outer; ++station1Outer;
              if ( station1Outer == stationsReverseEnd ) continue;

              // are we at the end already?
              StationReverseIterator station2Outer = station1Outer; ++station2Outer;
              if ( station2Outer == stationsReverseEnd ) continue;

              // 1st of two combinations In-In-out and In-Out-Out
              makeForwardSectorTriplets( station0, zone, station1, zone, station2Outer, outerZone,
                  stationsReverseEnd, zone, rzTracks);
              // 2nd of two combinations In-In-out and In-Out-Out
              makeForwardSectorTriplets( station0, zone, station1Outer, outerZone, station2Outer, outerZone,
                  stationsReverseEnd, zone, rzTracks);
            }
          } // zone loop
        } // station loop
      }
    }
    //==========================================================================
    // Backwards tracks
    //==========================================================================
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

            if( m_backWardOverlap ){
              // not the default to keep the code faster
              makeBackwardSectorTriplets(station0, zone,
                                         station1, zone,
                                         station2, zone,
                                         stationsEnd,
                                         zone,
                                         rzTracks);
            }else{
              // default is check only continous tracks (do not look for skips)
              seedInSectorTriplet(false,rzTracks, 0, zone,
                                  station0, zone,
                                  station1, zone,
                                  station2, zone);
            }
          } // zone loop
        } // station loop
      } // velo half loop
    } // onlyForward 
    //==========================================================================

    //==========================================================================
    //  Filter tracks with not enough high charge clusters
    //==========================================================================
    for ( std::vector<PatRZTrack>::iterator itRzt = rzTracks.begin();
        rzTracks.end() != itRzt; ++itRzt ) {
      PatRZTrack* tr = &(*itRzt);
      double nbClus = tr->coords()->size();
      double nbOver = 0;
      for ( PatVeloRHits::iterator itC = tr->coords()->begin();
          tr->coords()->end() != itC; ++itC ) {
        if ( m_chargeThreshold < (*itC)->hit()->signal() ) ++nbOver;
      }

      if ( nbClus * m_highChargeFract > nbOver ) {
        if(m_isVerbose){
          verbose() << "Removed track " << itRzt-rzTracks.begin()
            << " as only " <<nbOver << " of "
            <<  nbClus << " clusters were above the threshold of "
            << m_chargeThreshold << " min=" << m_highChargeFract*nbClus
            << endmsg;
        }
        tr->setValid( false );
      }
    }

    //== Merge tracks with many common hits
    if ( m_mergeTracks ) mergeTracks( rzTracks );  

    //==========================================================================
    //== Store the tracks in the Track tool here...
    //==========================================================================

    for ( std::vector<PatRZTrack>::iterator itRzt = rzTracks.begin();
        rzTracks.end() != itRzt; ++itRzt ) {
      PatRZTrack* tr = &(*itRzt);
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
        // JAH: set the phi of the 2nd coordenate
        state[1] = m_trackTool->phiGlobalRZone(tr->zone());

        LHCb::State temp;
        temp.setZ( z );
        temp.setState( state );
        Gaudi::TrackSymMatrix & cov = temp.covariance();
        cov(0,0) =  tr->errR2();// set 0,0 to errR2
        cov(2,2) = tr->errSl2();// set 2,2 to errSl2
        track->addToStates( temp );
        // set chi2 per dof from RZ fit
        track->setChi2AndDoF(tr->chi2(),tr->nbCoords()-2);
	// also add the number of found + missed stations 
	// to the track information for ghost rejection later if required
	track->addInfo(LHCb::Track::nPRVeloRZExpect,
		       static_cast<double>(tr->nbCoords()+
					   tr->missedStations()) );

        // store details of hits on track
        for ( PatVeloRHits::iterator itC = tr->coords()->begin();
            tr->coords()->end() != itC; ++itC ) {
          track->addToLhcbIDs( (*itC)->lhcbID() );
        }
        track -> setPatRecStatus( LHCb::Track::PatRecIDs );
        outputTracks->insert(track);
      }
    }


    if(m_isDebug){
      //debug() << "Stored " << m_outputTracks->size() << " tracks in "
      debug() << "Stored " << outputTracks->size() << " tracks in "
        << m_outputLocation << " from " << rzTracks.size()
        << " rz tracks found" << endmsg;
    }


    if ( m_doTiming ) m_timerTool->stop( m_veloRZTime );

    

    return StatusCode::SUCCESS;
  }

  //=============================================================================
  //  Finalize
  //=============================================================================
  StatusCode PatVeloRTracking::finalize() {

    debug() << "==> Finalize" << endmsg;

    return GaudiAlgorithm::finalize();  // must be called after all other actions
  }

  //=============================================================================

  // Starting from sector make downstream combinations of sectors to look for 
  // triplets of hits, if failed then run out of sensors to use

  void PatVeloRTracking::
    makeForwardSectorTriplets( StationReverseIterator station0, unsigned int zone0,
        StationReverseIterator station1, unsigned int zone1,
        StationReverseIterator station2, unsigned int zone2,
        StationReverseIterator stationsEnd, 
        unsigned int sector,
        std::vector<PatRZTrack> &rzTracks){

      // going to search for three matching clusters
      // do this with three cases
      for ( int iCase = 0; 3 > iCase; ++iCase ){

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

        seedInSectorTriplet(true,rzTracks,
            iCase, sector,
            sIter0, zone0,
            sIter1, zone1,
            sIter2, zone2); 
      }
      return;
    }

  void PatVeloRTracking::
    makeBackwardSectorTriplets( StationIterator station0, unsigned int zone0,
        StationIterator station1, unsigned int zone1,
        StationIterator station2, unsigned int zone2,
        StationIterator stationsEnd, 
        unsigned int sector,
        std::vector<PatRZTrack> &rzTracks){

      // going to search for three matching clusters
      // do this with three cases
      for ( int iCase = 0; 3 > iCase; ++iCase ){

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

        seedInSectorTriplet(false,rzTracks,
            iCase, sector,
            station0, zone0,
            station1, zone1,
            station2, zone2); 
      }
      return;
    }

  // find using 3 sector pointers seeds tracks into rzTracks
  // also extends tracks from initial triplet, into opposite side
  // for forward tracks

  void PatVeloRTracking::
    seedInSectorTriplet(bool forward, std::vector<PatRZTrack> &rzTracks,
        const int &iCase, unsigned int sector,
        StationIterator station0, unsigned int zone0,
        StationIterator station1, unsigned int zone1,
        StationIterator station2, unsigned int zone2) {

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

      // to get the pitch later.
      // LHCb::VeloChannelID chId((*station1)->sensor()->sensorNumber(),0);
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
        }else{ // tracks should have -ve gradient
          rMin = r0 - m_maxRSlope * (z2 - z0);
          rMax = r0 * ( z2 - m_zVertexMax ) / ( z0 - m_zVertexMax );
        }

        // loop over clusters in third station finding a possible match
        for ( c2 = first2; hits2.end() != c2 ; ++c2 ) {

          if ( 0 != iCase && (*c2)->hit()->isUsed() ) continue;

          double r2 = (*c2)->coordHalfBox();
          if ( rMax <= r2 ) break;
          if ( rMin > r2 ) {
            first2 = c2;
            continue;
          }
          // from first and third clusters predict position of second cluster
          double rPred = r0 + zFrac * (r2-r0);

          //const DeVeloRType* rSensor=m_velo->rSensor(chId.sensor());
          double rPitch = rSensor1->rPitch( rPred );
          double rTol = m_rMatchTol * rPitch;

          // try to find a matching co-ord
          PatVeloRHit* bestc1 = (*station1)->closestHitHalfBox(zone1, rPred, rTol );
          if ( 0 == bestc1 ) continue;

          //=== We have 3 aligned points
          if ( m_isVerbose ) {
            double r1 = bestc1->coordHalfBox();
            verbose() << format( "Triplet  z0 %6.1f  r %6.3f  ", z0, r0 );
            printCluster( (*c0)->hit(), verbose() );
            verbose() << format( "         z1 %6.1f  r %6.3f  ", z1, r1 );
            printCluster( bestc1->hit(), verbose() );
            verbose() << format( "         z2 %6.1f  r %6.3f  ", z2, r2 );
            printCluster( (*c2)->hit(), verbose() );
          }
          // create track
          PatRZTrack newTrack;
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
            if( msgLevel( MSG::VERBOSE) ) {
              verbose() << "Extend into sens "<< (*station2Outer)->sensor()->sensorNumber()
                << " sector " << outerZone << endmsg;
            }
            nMiss += extendTrack(newTrack, station2Outer, outerZone, nbUsed, 
                m_rExtraTol, forward , 0);
          }

          //== Ask for enough coordinates that have not previously
          // been used by other tracks
          // require at least 2 previously unused

          if ( 1 >= newTrack.nbCoords() - nbUsed ) {
            if( msgLevel( MSG::VERBOSE) ) verbose() << "Killed too few new hits" << endmsg;
            continue;
          }
          // if only 3 hits found, all 3 must be unused
          if ( 3 == newTrack.nbCoords() && 0 != nbUsed ) {
            if( msgLevel( MSG::VERBOSE) ) {
              verbose() << "Killed all three hits should be new" << endmsg;
            }
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
          if(!forward){
            newTrack.setBackward(true);
          }

          //== Try to add hits on the other half station, if sector 0/3
          // and (optionally) forward 
          if ( ( forward || m_backWardOverlap )
              //&& ( 4 < newTrack.nbCoords()  )
              && (0 == zone0 || 3 == zone0 ) ) {
            addOppositeSideHits(newTrack,zone0,forward,(*station0)->sensor());
          }
          if ( forward ) {
            std::sort( newTrack.coords()->begin(), newTrack.coords()->end(),
                VeloRHit::DecreasingByZ() );
          } else {
            std::sort( newTrack.coords()->rbegin(), newTrack.coords()->rend(),
                VeloRHit::DecreasingByZ() );
          }

          //== Store. Tag hits if long enough
          rzTracks.push_back( newTrack );
          if ( m_isVerbose ) {
            verbose() << "      -- stored n=" << rzTracks.size()-1 << endmsg;
          }
          if ( m_minToTag <= newTrack.nbCoords() ) {
            newTrack.tagUsedCoords();
            break;
          }
        }
      }
    }


  // extend a tracks into more sectors, starting with sectPt
  // if extraCoord==0 add hits into track, otherwise return new hits
  // in extraCoord
  // returns number of missed stations before track exits acceptance at low r
  int PatVeloRTracking::extendTrack(PatRZTrack &newTrack,
      StationIterator station, unsigned int zone,
      unsigned int &nbUsed,
      double myTol,
      bool forward,
      PatVeloRHits *extraCoord){

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
    // double lastZ = (*station)->z();
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

      if( extraCoord ){
	// switching between box halves looking for overlaps
	// need to correct for an offset in y of the VELO sides (if any)
	if( zone == 0 || zone == 7 ){
	  rPred += m_ROffsetTop[(*station)->sensor()->sensorNumber()];
	}else{
	  rPred += m_ROffsetBottom[(*station)->sensor()->sensorNumber()];
	}
	if(m_isVerbose) {
	  verbose() << "Offset overlap sensor " 
		    << (*station)->sensor()->sensorNumber()
		    << " in zone " << zone << " by " 
		    << m_ROffsetTop[(*station)->sensor()->sensorNumber()]
		    << endmsg;
	}      
      }

      const DeVeloRType* rSensor=(*station)->sensor();
      double rPitch = rSensor->rPitch( rPred );
      double tol   = myTol * rPitch;

      if ( m_isVerbose ) {
        verbose() << "  in sensor " << (*station)->sensor()->sensorNumber()
          << " pred " << rPred << " tol " << tol
          << " z " << z
          << endmsg;
      }

      if ( (*station)->sensor()->rMin(zone) > rPred ) break;  // no longer inside.

      //== Select the best hit when extrapolating
      PatVeloRHit* bestc = (*station)->closestHitHalfBox( zone, rPred, tol );
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
        if ( m_isVerbose ) {
          verbose()   
            << format( "    add  z  %6.1f  r %6.3f  ", z, bestc->coordHalfBox() );
          printCluster( bestc->hit(), verbose() );
        }
      } else {
        // no hit found
        ++nMiss;
      }
    }
    return nMiss;
  }

  /// add the hits from the other side of the detector to a track
  void PatVeloRTracking::
        addOppositeSideHits(PatRZTrack &newTrack,const int &zone,bool forward,
        const DeVeloRType* sensor ){

      PatVeloRHits extraCoords;

      const DeVeloRType* otherSideSensor = sensor->otherSideRSensor();
      if (!otherSideSensor) return;

      int iSens =  otherSideSensor->sensorNumber();

      // since zones are ordered in global phi, we always have to flip
      int otherZone = 3-zone;

      StationIterator station = m_hitManager->stationIterHalf( iSens);

      unsigned int nbUsed =0;
      extendTrack(newTrack,station, otherZone,nbUsed, m_rOverlapTol, forward, &extraCoords);

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
  //  Print a Velo cluster, and its associated truth
  //=========================================================================
  void PatVeloRTracking::printCluster(  const VeloRHit* clus, MsgStream& msg ) {
    msg << format( " Strip%4d.%4d used%2d size%2d signal%6.0f", 
        clus->sensor()->sensorNumber(), clus->strip(), clus->isUsed(), 
        clus->size(), clus->signal() );
    msg << endmsg;
  }

  //=========================================================================
  //  
  //=========================================================================
  void PatVeloRTracking::mergeTracks ( std::vector<PatRZTrack> &rzTracks ) {
    std::vector<PatRZTrack>::iterator it1, it2;
    for ( it1 = rzTracks.begin(); rzTracks.end()-1 != it1; ++it1 ) {
      if ( !(*it1).valid() ) continue;
      if ( (*it1).backward() ) continue;
      for ( it2 = it1+1; rzTracks.end() != it2; ++it2 ) {
        if ( !(*it2).valid() ) continue;
        if ( (*it1).minSensor() > (*it2).maxSensor() - m_nCommonToMerge ) continue;
        if ( (*it2).minSensor() > (*it1).maxSensor() - m_nCommonToMerge ) continue;
        int nCommon = 0;
        int nCommonSensor = 0;
        PatVeloRHits::iterator itC1, itC2;
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
              printCluster( (*itC1)->hit(), debug() );
            }
            debug() << " Track 2" << endmsg;
            for ( itC2 = (*it2).coords()->begin(); (*it2).coords()->end() != itC2; ++itC2 ) {
              printCluster( (*itC2)->hit(), debug() );
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
                VeloRHit::DecreasingByZ() );
            (*it2).setValid(false);
            if ( msgLevel( MSG::DEBUG ) ) {
              debug() << "---> Tracks are merged <--- Resulting track: " << endmsg;
              for ( itC1 = (*it1).coords()->begin(); (*it1).coords()->end() != itC1; ++itC1 ) {
                printCluster( (*itC1)->hit(), debug() );
              }
            }
          }
        } 
      }
    }
  }

  StatusCode PatVeloRTracking::registerConditionCallBacks() {
    if(m_isDebug) debug() <<"Register callbacks" << endmsg;
    updMgrSvc()->
      registerCondition(this,(*(m_velo->leftSensorsBegin()))->geometry(),
		      &Tf::PatVeloRTracking::updateBoxOffset);
    updMgrSvc()->
      registerCondition(this,(*(m_velo->rightSensorsBegin()))->geometry(),
			&Tf::PatVeloRTracking::updateBoxOffset);
    StatusCode sc = updMgrSvc()->update(this);
    if(!sc.isSuccess()) 
      return Error("Failed to update conditions!",StatusCode::FAILURE);
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode PatVeloRTracking::updateBoxOffset(){
    if(m_isDebug) debug() << "updateBoxOffset" << endmsg;
    // use the parameters ZVertexMin and MaxRSlope to calculate the z position of 
    // the last sensor to search in z for forward tracks
    // first get the minimum radius of a sensor in the local frame
    double maxR = (*(m_velo->rSensorsBegin()))->rMax(0);
    // Correct for the opening of the VELO, if any
    // Assume the half box is not too skew to the beam
    double dX = fabs(m_velo->halfBoxOffset(DeVelo::LeftHalf).x());
    m_zSensorSearchMin = m_zVertexMin + ((maxR+dX) / m_maxRSlope);
    // same for backward tracks
    m_zSensorSearchMax = m_zVertexMax - ((maxR+dX) / m_maxRSlope);

    if(m_isDebug){
      debug() << "zSearchMin= " << m_zSensorSearchMin << endmsg;
      debug() << "zSearchMax= " << m_zSensorSearchMax << endmsg;
    }

    // need to calculate the expected shift of the left -> right and
    // vice versaboxes in r which is the overlap region is in the y direction
    // Lets be unsubtle and make a table of each possible offset
    unsigned int nRSens = m_velo->numberRSensors();
    m_ROffsetTop.resize(nRSens,0.);
    m_ROffsetBottom.resize(nRSens,0.);
    if( !m_OverlapCorrection) {
      return Warning("Overlap correction is not applied",StatusCode::SUCCESS);
    }
    for ( std::vector<DeVeloRType*>::const_iterator iR = 
	    m_velo->rSensorsBegin() ; 
	  iR !=  m_velo->rSensorsEnd() ; ++iR ){

      const DeVeloSensor *thisSensor = *iR;
      const DeVeloSensor *otherSensor = (*iR)->otherSideRSensor();
      if( ! otherSensor ){ // in case someone kills the sensors in the XML
	continue;
      }
      unsigned int sNum = thisSensor->sensorNumber();
      // point half way up sensitive overlap region, top side of detector
      // in other side half box frame
      Gaudi::XYZPoint otherTopPoint(0.,25.,otherSensor->z());
      // and at bottom of detector
      Gaudi::XYZPoint otherBottomPoint(0.,-25.,otherSensor->z());
      // other HB -> global -> this HB to get offsets
      Gaudi::XYZPoint globalTop = otherSensor->veloHalfBoxToGlobal(otherTopPoint);
      Gaudi::XYZPoint globalBottom = otherSensor->veloHalfBoxToGlobal(otherBottomPoint);
      Gaudi::XYZPoint localTop = thisSensor->globalToVeloHalfBox(globalTop);
      Gaudi::XYZPoint localBottom = thisSensor->globalToVeloHalfBox(globalBottom);
      // convert the two local frame points to delta r between the frames
      m_ROffsetTop[sNum] =  localTop.rho() - otherTopPoint.rho(); 
      m_ROffsetBottom[sNum] =  localBottom.rho() - otherBottomPoint.rho(); 

      if(m_isVerbose) {
	verbose() << "Sensor " << sNum << " paired with "
		  <<  otherSensor->sensorNumber()
		  << " deltaR top " << m_ROffsetTop[sNum]
		  << " deltaR bottom " << m_ROffsetBottom[sNum]
		  << endmsg;
      }
    }
    return StatusCode::SUCCESS;
  }

}
