// Include files

// non-local classes used
#include "Event/Track.h"
#include "VeloDet/DeVelo.h"

// local
#include "PatVeloTrackTool.h"
#include "PatVeloSpaceTrack.h"
#include "PatVeloSpaceTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloSpaceTool
// the code was in PatVeloSpaceTracking.cpp
//
// 2006-07-24 : David Hutchcroft
//-----------------------------------------------------------------------------

namespace Tf {
  // Declaration of the Tool Factory
  DECLARE_TOOL_FACTORY( PatVeloSpaceTool )

  //=============================================================================
  // Standard constructor, initializes variables
  //=============================================================================
  PatVeloSpaceTool::PatVeloSpaceTool( const std::string& type,
      const std::string& name,
      const IInterface* parent )
    : base_class ( type, name , parent )
    {
      declareInterface<ITracksFromTrack>(this);
      // normally want to only use clusters once
      declareProperty("MarkClustersUsed", m_markClustersUsed = false);

      declareProperty( "FractionFound"   , m_fractionFound    = 0.35      );
      declareProperty( "PhiAngularTol"   , m_phiAngularTol    = 0.005     );
      declareProperty( "PhiMatchTol"     , m_phiMatchTol      = 0.11      );
      declareProperty( "PhiFirstTol"     , m_phiFirstTol      = 0.09      );
      declareProperty( "AdjacentSectors" , m_adjacentSectors = false      );
      declareProperty( "FractionPhiMerge", m_fractionPhiMerge = 0.5       );
      declareProperty( "stepError"       , m_stepError        = 0.002     );
      declareProperty( "forwardStepError", m_forwardStepError = 0.00035   );
      declareProperty( "MaxChiSqDof"     , m_chiSqDofMax      = 22.0      );
      declareProperty( "CleanOverlaps"   , m_cleanOverlaps    = true      );
      declareProperty( "NMissedFirst"    , m_NMissedFirst     = 4         );
      declareProperty( "RHitManagerName" , m_rHitManagerName  = "PatVeloRHitManager" );
      declareProperty( "PhiHitManagerName" , m_phiHitManagerName  = "PatVeloPhiHitManager" );
      declareProperty( "TrackToolName", m_trackToolName = "PatVeloTrackTool" );
      declareProperty( "FullErrorPoints" , m_fullErrorPoints = 5         );


    }
  //=============================================================================
  // Destructor
  //=============================================================================
  PatVeloSpaceTool::~PatVeloSpaceTool() = default;

  StatusCode PatVeloSpaceTool::initialize(){
    StatusCode sc = GaudiTool::initialize();
    if(!sc) return sc;

    m_rHitManager   = tool<PatVeloRHitManager>  ( "Tf::PatVeloRHitManager", m_rHitManagerName );
    m_phiHitManager = tool<PatVeloPhiHitManager>( "Tf::PatVeloPhiHitManager", m_phiHitManagerName );

    //== Get detector element
    m_velo = getDet<DeVelo>( DeVeloLocation::Default );
    // get some constants for later use
    m_numberRSensors = m_velo->numberRSensors();

    m_trackTool = tool<PatVeloTrackTool>("Tf::PatVeloTrackTool",
                                         m_trackToolName );

    if(msgLevel(MSG::DEBUG))
      debug() << "========== Tool " << name() << "====== "<< endmsg;
    return StatusCode::SUCCESS;
  }

  StatusCode
  PatVeloSpaceTool::tracksFromTrack( const LHCb::Track& seed,std::vector<LHCb::Track*>& tracks ) const {

    bool isVerbose = msgLevel(MSG::VERBOSE);
    bool isDebug   = msgLevel(MSG::DEBUG);

    if( seed.checkFlag(LHCb::Track::Invalid) ) {
      Warning("Invalid RZ Velo track as input", StatusCode::FAILURE ).ignore();
      return { };
    }

    // turn an LHCb::Track into a PatVeloSpaceTrack for fitting purposes
    auto space = m_trackTool->makePatVeloSpaceTrack(seed);
    auto spaceTracks = find3DSpaceTrack(*space);
    tracks.reserve(tracks.size()+spaceTracks.size());
    // fit returned space tracks
    for (const auto& strack : spaceTracks ) {

      if(!strack->valid()) continue; // if fails skip to next track

      // set R on phi hits
      m_trackTool->setPhiCoords(*strack);
      // fit the track trajectory
      strack->fitSpaceTrack( m_stepError, true, true, m_fullErrorPoints );

      // check if spillover
      if (m_trackTool->isSpilloverTrack(*strack)) {
        if(isVerbose) verbose() << "Spillover track removed" << endmsg;
        continue;
      }

      if( m_cleanOverlaps ){
        // if the track has R hits on both sides but phi hits out of the
        // overlap region delete the R hits
        bool cleaned =
	  m_trackTool->cleanNonOverlapTracks(*strack, m_stepError,
					     m_fullErrorPoints);
        if(!strack->valid()) {
          if (isVerbose) verbose()
            << "Non-overlap track has overlap R clusters removed"
              << endmsg;
          continue; // if fails skip to next track
        }
        if(cleaned && isVerbose) verbose()
          << "Cleaned R clusters from non-overlap track" << endmsg;
      }
      //  clean the worst fitted examples
      if ( strack->chi2Dof( ) > m_chiSqDofMax ) {
        if (isVerbose) verbose() << "Skip track with chi^2/ndf "
          << strack->chi2Dof( )
            << endmsg;
        continue;
      }

      if (isDebug) debug() << "Found track with chi^2/ndf "
        << strack->chi2Dof( ) << endmsg;

      // make the found PatVeloSpaceTrack back into an LHCb::Track
      std::unique_ptr<LHCb::Track> newTrack{ new LHCb::Track() };
      auto sc = m_trackTool->makeTrackFromPatVeloSpace(*strack, *newTrack, m_forwardStepError);
      if (!sc) {
        Warning("Failed to convert to LHCb::Track",StatusCode::SUCCESS,0).ignore();
        continue;
      }
      newTrack->setHistory(LHCb::Track::PatVelo);
      tracks.push_back(newTrack.release());
    }
    return StatusCode::SUCCESS;
  }

  std::vector<std::unique_ptr<PatVeloSpaceTrack>> PatVeloSpaceTool::find3DSpaceTrack(const PatVeloSpaceTrack & track) const
  {

    bool isVerbose = msgLevel(MSG::VERBOSE);
    bool isDebug   = msgLevel(MSG::DEBUG);

    int zone = track.zone();

    auto startStation = m_rHitManager->stationsAllBeginNoPrep();
    auto stopStation  = std::prev(m_rHitManager->stationsAllEndNoPrep());
    PatVeloRHitManager::StationIterator lastRStationIter;
    PatVeloRHitManager::StationIterator firstRStationIter;
    int step;

    if ( track.backward() ) {
      // backward track
      lastRStationIter  = m_rHitManager->stationIterAllNoPrep(track.maxRSensor());
      firstRStationIter = m_rHitManager->stationIterAllNoPrep(track.minRSensor());
      step = +1;
      // get previous and next R sensor numbers on same side
      if (firstRStationIter != startStation) --firstRStationIter;
      if (firstRStationIter != startStation) --firstRStationIter;
      if (lastRStationIter  != stopStation)  ++lastRStationIter;
      if (lastRStationIter  != stopStation)  ++lastRStationIter;
    } else {
      // Forward track, i.e. last sensor is smaller than first one.
      lastRStationIter  = m_rHitManager->stationIterAllNoPrep(track.minRSensor());
      firstRStationIter = m_rHitManager->stationIterAllNoPrep(track.maxRSensor());
      step = -1;
      // get previous and next R sensor numbers on same side
      if (lastRStationIter  != startStation) --lastRStationIter;
      if (lastRStationIter  != startStation) --lastRStationIter;
      if (firstRStationIter != stopStation)  ++firstRStationIter;
      if (firstRStationIter != stopStation)  ++firstRStationIter;
    }

    if (isDebug) debug() << "--- Looking for Track"
      << " from RZ " << track.ancestor()->key()
        << " zone "  << zone
        << " first " << (*firstRStationIter)->sensor()->sensorNumber()
        << " last "  << (*lastRStationIter)->sensor()->sensorNumber()
        << endmsg;

    // dump any old phi lists
    m_phiPt.clear();

    int nStationsTried =0; // number of stations tried to match with this track
    unsigned int lastStationTried =0;

    // loop over Phi sensors in station range of R hits on track
    auto firstPhiStationIter
      = m_phiHitManager->stationIterAllNoPrep((*firstRStationIter)->sensor()->associatedPhiSensor()->sensorNumber());
    auto lastPhiStationIter
      = m_phiHitManager->stationIterAllNoPrep((*lastRStationIter)->sensor()->associatedPhiSensor()->sensorNumber());

    
    for ( auto phiStationIter = firstPhiStationIter; lastPhiStationIter != phiStationIter; std::advance(phiStationIter,step) ) {
      // try inner sector of phi sensor
      PatVeloPhiHitManager::Station* station = *phiStationIter;
      if ( !station->sensor()->isReadOut() ) continue; // jump sensors not in readout
      if ( !station->hitsPrepared() ) m_phiHitManager->prepareHits(station);

      double z = station->z();
      // estimated radius of track in this sensor
      double r = track.rInterpolated( z );

      if (isDebug) debug() << ".. sens " << station->sensor()->sensorNumber()
        << " z " << z << " r " << r  << endmsg;

      const DeVeloPhiType* sensor = station->sensor();
      if ( sensor->outerRadius() < r ) {
        if ( sensor->outerRadius() < r-0.05 ) continue; // point outside detector, with tolerance
        r = sensor->outerRadius() - 0.001;
      }
      //if ( sensor->outerRadius() < r ) continue; // point outside detector

      unsigned int phiZone = 0;
      if ( sensor->halfboxRRange(phiZone).first > r ) {
        if ( sensor->halfboxRRange(phiZone).first > r + 0.05 ) break; // point outside Phi sector
        r = sensor->halfboxRRange(phiZone).first + 0.001;
      }
      //if ( sensor->halfboxRRange(phiZone).first  > r ) break; // point outside Phi sector
      if ( sensor->halfboxRRange(phiZone).second < r ) { //point outside phi sector
        phiZone = 1;
      }

      // see how many stations we have looked for sensors in...
      if ( nStationsTried == 0 ) {
        ++nStationsTried;
        lastStationTried = station->stationNumber();
      }else if ( lastStationTried != station->stationNumber() ){
        ++nStationsTried;
        lastStationTried = station->stationNumber();
      }

      std::pair<double,double> range;
      // check if extrapolated rz track is compatible with this phi sector
      bool hasRange = m_trackTool->phiRange( r, zone, m_phiAngularTol, station, phiZone, range);

      // Adjacent sectors applies only on 2nd and later sensors
      if ( m_adjacentSectors && nStationsTried > 1 ){
        // if zone = 1,2,5,6 switch to zone 0,3,4,7 respectivly
        int newZone = -9;
        if( zone == 1 ) newZone = 0;
        if( zone == 2 ) newZone = 3;
        if( zone == 5 ) newZone = 4;
        if( zone == 6 ) newZone = 7;
        if( newZone != -9 ){
          std::pair<double,double> newRange;
          bool oldHasRange = hasRange;
          hasRange = m_trackTool->phiRange( r, newZone, m_phiAngularTol, station, phiZone, newRange);
          // if new zone check is OK
          if(hasRange){
            // if either bigger range or old zone check failed use new zone check
            if (!oldHasRange) {
              std::pair<double,double> unionRange;
              if (m_angleUtils.rangeUnion(range,newRange,unionRange)) range = unionRange;
            }
          }
        }
      }
      if ( !hasRange ) continue;

      if(isDebug) debug()<<format("sens %2d R%8.3f phiMin%8.4f max%8.4f ",
          station->sensor()->sensorNumber(), r, range.first, range.second )
          << " PhiPt size " << m_phiPt.size()
          << " nStations " << nStationsTried
          << endmsg;

      //== Match to the list of existing PhiList candidates.
      //== One stores only the best per list, and create a new list
      //== at beginning of the track (first two pairs) only.

      for (auto iPhiList = m_phiPt.begin(); m_phiPt.end() != iPhiList; ++iPhiList ) {
        // once candidate phi hits have been found
        iPhiList->setExtrapolation( z, r, m_phiMatchTol, m_phiFirstTol);

      }
      //loop over all phi clusters in the phi sensor sector
      // note phi list can grow in this operation
      findBestPhiClusInSect(station, phiZone, zone, nStationsTried, r, range);

      for (auto iPhiList = m_phiPt.begin(); m_phiPt.end() != iPhiList; ++iPhiList ) {
        // save the candidate phi clusters if it survived
        iPhiList->saveCandidate( );
        if(isVerbose)verbose()<<format(" sensor %3d list #%2d phi%7.4f size %2d",
            station->sensor()->sensorNumber(), iPhiList-m_phiPt.begin(),
            iPhiList->phi(), iPhiList->size() )
            << endmsg;
      }
    } // end of loop over phi sensors for this RZ track

    auto accepted = getBestPhiList(track,nStationsTried);
    if(!accepted.empty()) {
      if ( isDebug ) debug() << "Good track, nAccepted ="
        << accepted.size() << endmsg;
    }
    return accepted;
  }

  void PatVeloSpaceTool::
  findBestPhiClusInSect(PatVeloPhiHitManager::Station* station,
			unsigned int zone,
			unsigned int RZone,
			int nStationsTried,
			double r,
			const std::pair<double,double>& phiRange) const {
    // set the same side offset in phi for this radius
    double offset = station->sensor()->halfboxPhiOffset(zone,r);
    // if other side from RZ track add a correction to phi
    if ((station->sensor()->isLeft() && RZone > 3) ||
	(station->sensor()->isRight() && RZone < 4)) {
      offset +=m_trackTool-> phiOffsetOtherHB(station->sensor()->sensorNumber(),
					     m_trackTool->phiGlobalRZone(RZone),r);
      r += m_trackTool->rOffsetOtherHB(station->sensor()->sensorNumber(),
					  m_trackTool->phiGlobalRZone(RZone));
    }
    for (auto itP = station->hits(zone).begin();
	  station->hits(zone).end() != itP; ++itP ) {

      // check if co-ord is compatible with the min and max for the sector.
      double phi = m_angleUtils.add((*itP)->coordHalfBox(),offset);
      if (!m_angleUtils.contains(phiRange,phi)) continue;

      // set a possible `point' (incorporating R info) corresponding to
      // this co-ord
      (*itP)->setRadiusAndPhi( r, phi );

      //=== First sensors. Create a list only.
      if ( nStationsTried == 1 ) {
        m_phiPt.addEntry(phi, *itP);
        if ( msgLevel(MSG::VERBOSE) ) {
          verbose() << " nStationsTried " << nStationsTried << " "
            << format( "new candidate phi = %8.4f ", phi )
            << endmsg;
        }
        continue;
      }

      // calculate corresponding X/Y postion
      double xPred = r * (*itP)->cosPhi();
      double yPred = r * (*itP)->sinPhi();

      // loop over each phi list to see if cluster is the new closest
      bool found = false;
      for ( auto iPhiList = m_phiPt.begin(); m_phiPt.end() != iPhiList; ++iPhiList ) {
        bool better = iPhiList->isCloser( xPred, yPred, *itP );
        if ( better ) {
          found = true;
          if(msgLevel(MSG::VERBOSE)) {
            dumpClusterComparison(iPhiList, xPred, yPred, **itP);
          }
        }
      }

      //=== Second or third pair of sensors and a matching phi cluster
      //    Create a new phiList, if not used in existing phiList
      if ( !found && nStationsTried <= m_NMissedFirst ) {
        if ( (*itP)->hit()->isUsed() ) continue;
        m_phiPt.addEntry(phi, *itP);
        if ( msgLevel(MSG::VERBOSE) ) {
          verbose() << "  nStationsTried " << nStationsTried << " "
            << format( "new candidate phi = %8.4f", phi )
            << endmsg;
        }
      }
    }
  }

  std::vector<std::unique_ptr<PatVeloSpaceTrack>> 
  PatVeloSpaceTool::getBestPhiList(const PatVeloSpaceTrack & track, double nStationsTried ) const
  {

    // if no phi clusters found this is not a good track
    if(m_phiPt.empty()) return {};

    int minExpected = int( m_fractionFound * nStationsTried ) -1;
    if ( 3 > minExpected ) minExpected = 3;

    // if too few phi hits from number of R clusters reject track
    if ( msgLevel(MSG::DEBUG) ) debug() << "nStationsTried " << nStationsTried
      << " request " << minExpected
        << " phi clusters " << endmsg;

    // All acceptable combinations of phi clusters: start with 1st
    double minChi2 = 1.e10;

    mergePhiLists( track.backward() );

    //== Get the properties of the best list
    for (auto iPhiList = m_phiPt.begin(); m_phiPt.end() != iPhiList; ++iPhiList ) {
      if ( iPhiList->size() < 5 && 0 ==  iPhiList->nbUnused()  ) continue;
      if ( minExpected == iPhiList->size() ) {
        if ( minChi2 > iPhiList->qFactor() ) minChi2 = iPhiList->qFactor();
      } else if ( iPhiList->size() >= minExpected ) {
        minExpected = iPhiList->size();
        minChi2 = iPhiList->qFactor();
      }
    }
    //== Tolerance for other lists
    if ( 4 < minExpected ) minExpected = minExpected - 1;
    if ( 3 < minExpected ) minChi2 += 0.5;

    std::vector<std::unique_ptr<PatVeloSpaceTrack>> accepted;
    for (auto iPhiList = m_phiPt.begin(); m_phiPt.end() != iPhiList; ++iPhiList ) {
      int nbFound  = iPhiList->size();
      if ( 3 > nbFound ) continue;
      if ( msgLevel(MSG::DEBUG) ) {
        debug() << format( "Philist #%2d phi= %7.4f n%3d minExp%3d unused%3d chisq%7.3f",
            iPhiList -m_phiPt.begin(), iPhiList->phi(), nbFound, minExpected,
            iPhiList->nbUnused(), iPhiList->chiSq() ) << endmsg;
      }
      if ( iPhiList->size() < 5 && 0 ==  iPhiList->nbUnused()  ) continue;
      if ( minExpected > nbFound       ) continue;
      if ( minChi2 < iPhiList->qFactor() ) continue;
      accepted.emplace_back( new PatVeloSpaceTrack(track) );
      auto* phiCan = accepted.back().get();
      for (const auto& c : iPhiList->coords() ) { phiCan->addPhi(c); }
      if ( m_markClustersUsed ) phiCan->tagClustersAsUsed( HitBase::UsedByVeloSpace );
      phiCan->setChiSqDof( iPhiList->chiSq() );
    }

    if ( msgLevel( MSG::DEBUG ) && accepted.size() > 1 ) {
      debug() << "Multiple solutions for same R" << endmsg;
      int indx = 0;
      for ( const auto& t : accepted ) {
        debug() << "... track " << indx++ << " Chisq/dof " << t->chi2Dof() << endmsg;
      }
    }
    return accepted;
  }

  void PatVeloSpaceTool::dumpClusterComparison(PatVeloPhiLists::iterator & iPhiList,
      double xPred, double yPred,
      PatVeloPhiHit & pCoord) const{
    double dist = iPhiList->distSquared( xPred, yPred );
    verbose() << " strip " << pCoord.lhcbID().veloID().strip()
      << format(" PhiList%8.4f Xpred %7.3f/%7.3f"\
          " YPred %7.3f/%7.3f dist2%7.3f",
          iPhiList->phi(), xPred, iPhiList->predX(),
          yPred, iPhiList->predY(), dist )
      << endmsg;
  }

  //=========================================================================
  //  Merge lists if they share hits
  //=========================================================================
  void PatVeloSpaceTool::mergePhiLists ( bool backward ) const {

    // make all pairs of Phi lists
    for (auto it1 = m_phiPt.begin(); m_phiPt.end() != it1; ++it1 ) {
      int n1        = (*it1).size();
      if ( 3 > n1 ) continue; // too short, so ignore
      for (auto it2 = std::next(it1) ; m_phiPt.end() != it2; ++it2 ) {
        int n2 = (*it2).size();
        if ( 3 > n2 ) continue; // too short, so ignore
        // pick the short/long combination (arbitary if same length)
        PatVeloPhiLists::iterator iShort,iLong;
        if ( n1 < n2 ){
          iShort = it1;
          iLong = it2;
        }else{
          iShort = it2;
          iLong = it1;
        }
        // count clusters in common
        int nCommon = std::count_if(iLong->coords().begin(), iLong->coords().end(),
                                    [&](const Tf::PatVeloPhiHit* h) { 
                      const auto& c = iShort->coords();
                      return std::find(c.begin(),c.end(), h )!= c.end(); 
        });
        bool added = false;

        // check found enough hits in common to start merge
        if ( nCommon > m_fractionPhiMerge * std::min(n1,n2) ) {
          if ( msgLevel(MSG::DEBUG) ) {
            debug() << "For list " << iLong-m_phiPt.begin() << " size "
              << std::max(n1,n2)
              << " and list " << iShort-m_phiPt.begin() << " size "
              << std::min(n1,n2)
              << " " << nCommon << " shared hits." << endmsg;
          }
          for (auto itHShort = (*iShort).coords().begin();
              (*iShort).coords().end() != itHShort; ++itHShort  ) {
            bool foundSensor = false;
            for (auto itHLong = (*iLong).coords().begin();
                (*iLong).coords().end() != itHLong; ++itHLong  ) {
              if ( (*itHLong)->sensor()->sensorNumber() ==
                  (*itHShort)->sensor()->sensorNumber() ) {
                foundSensor = true;
                break;
              }
            }
            if (!foundSensor ) {
              added = true;
              (*iLong).addExtraPhi( *itHShort );
            }
          }
          if ( added ) {
            if ( backward ) {
              std::sort( (*iLong).coords().rbegin(),
                  (*iLong).coords().rend(),
                  PatVeloPhiHit::DecreasingByZ() );
            } else {
              std::sort( (*iLong).coords().begin(),
                  (*iLong).coords().end(),
                  PatVeloPhiHit::DecreasingByZ() );
            }
          }
          (*iShort).coords().clear();
        }
      }
    }
  }
}
