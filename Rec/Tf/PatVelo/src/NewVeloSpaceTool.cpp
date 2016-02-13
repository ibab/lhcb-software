// Include files

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"
// local
#include "NewVeloSpaceTool.h"
#include "NewSpaceTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : NewVeloSpaceTool
//
// 2009-03-20 : Olivier Callot
//-----------------------------------------------------------------------------

namespace{
  struct  increasingByReferencePhi {
    bool operator() ( const Tf::PatVeloPhiHit* h1, const Tf::PatVeloPhiHit* h2 ) const {
      return h1->referencePhi() < h2->referencePhi();
    }
  };
}

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( NewVeloSpaceTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NewVeloSpaceTool::NewVeloSpaceTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : base_class ( type, name , parent )
{
  declareInterface<ITracksFromTrack>(this);
  declareProperty( "RHitManagerName" ,   m_rHitManagerName    = "PatVeloRHitManager"   );
  declareProperty( "PhiHitManagerName" , m_phiHitManagerName  = "PatVeloPhiHitManager" );
  declareProperty( "TrackToolName",      m_trackToolName      = "PatVeloTrackTool"     );

  declareProperty( "PhiAngularTol",      m_phiAngularTol      = 0.005  );
  declareProperty( "PhiMatchZone",       m_phiMatchZone       = 0.018  );
  declareProperty( "PhiBigZone",         m_phiBigZone         = 0.10   );
  declareProperty( "FractionFound",      m_fractionFound      = 0.70   );
  declareProperty( "MaxChi2PerHit",      m_maxChi2PerHit      = 10.    );
  declareProperty( "MaxChi2ToAdd",       m_maxChi2ToAdd       = 50.    );
  declareProperty( "FractionForMerge",   m_fractionForMerge   = 0.70   );
  declareProperty( "DeltaQuality",       m_deltaQuality       = 1.0    );
  declareProperty( "FractionForDouble",  m_fractionForDouble  = 0.60   );
  declareProperty( "MaxQFactor",         m_maxQFactor         = 10.     );

  declareProperty( "StepError"         , m_stepError          = 0.002     );
  declareProperty( "ForwardStepError",   m_forwardStepError   = 0.00035   );
  declareProperty( "FullErrorPoints",    m_fullErrorPoints    = 5 );

  declareProperty( "DebugToolName" ,     m_debugToolName      = ""     );
  declareProperty( "DebugTrackToolName", m_debugTrackToolName = ""     );
  declareProperty( "WantedKey"         , m_wantedKey          = -100   );
  declareProperty( "PrintTracks"       , m_printTracks        = false  );
  declareProperty( "MeasureTime"       , m_measureTime        = false  );
}
//=============================================================================
// Destructor
//=============================================================================
NewVeloSpaceTool::~NewVeloSpaceTool() {}


//=========================================================================
//
//=========================================================================
StatusCode NewVeloSpaceTool::initialize ( ) {
  StatusCode sc = GaudiTool::initialize();
  if(!sc) return sc;

  m_rHitManager   = tool<Tf::PatVeloRHitManager>  ( "Tf::PatVeloRHitManager", m_rHitManagerName );
  m_phiHitManager = tool<Tf::PatVeloPhiHitManager>( "Tf::PatVeloPhiHitManager", m_phiHitManagerName );
  m_trackTool     = tool<Tf::PatVeloTrackTool>    ( "Tf::PatVeloTrackTool", m_trackToolName );

  //== Get detector element
  m_velo          = getDet<DeVelo>( DeVeloLocation::Default );
  m_debugTool     = 0;
  if ( "" != m_debugToolName ) m_debugTool = tool<IPatDebugTool>( m_debugToolName );
  m_debugTrackTool = 0;
  if ( "" != m_debugTrackToolName ) m_debugTrackTool = tool<IPatDebugTrackTool>( m_debugTrackToolName );

  m_timer = 0;
  if ( m_measureTime ) {
    m_timer = tool<ISequencerTimerTool>( "SequencerTimerTool", "Timing", this );
    m_totalTime = m_timer->addTimer( "Total" );
    m_timer->increaseIndent();
    m_convertTime = m_timer->addTimer( "Prepare R tracks" );
    m_prepareTime = m_timer->addTimer( "Prepare Hits" );
    m_mainTime    = m_timer->addTimer( "Search tracks" );
    m_unusedTime  = m_timer->addTimer( "Recover unused hits" );
    m_storeTime   = m_timer->addTimer( "Store tracks" );
    m_timer->decreaseIndent();
  }

  info() << "Print Tracks = " << m_printTracks << endmsg;

  return StatusCode::SUCCESS;
}

//=========================================================================
// Process one input track.
//=========================================================================
StatusCode 
NewVeloSpaceTool::tracksFromTrack ( const LHCb::Track& in,
                                    std::vector<LHCb::Track*>& out ) const {

  m_isVerbose = msgLevel(MSG::VERBOSE);
  m_isDebug   = msgLevel(MSG::DEBUG);
  if ( m_measureTime ) {
    m_timer->start( m_totalTime );
    m_timer->start( m_convertTime );
  }
  
  if ( m_debugTrackTool ) {
    //== Get the MC truth of the candidate..
    for ( int key : m_debugTrackTool->keys( &in ) ) {
      if ( msgLevel( MSG::DEBUG ) ) m_wantedKey = key;
      if ( key == m_wantedKey ) {
        m_isVerbose = true;
        m_isDebug   = true;
      }
    }
  }

  // turn an LHCb::Track into a PatVeloSpaceTrack for fitting purposes
  auto track = m_trackTool->makePatVeloSpaceTrack( in );
  int zone = track->zone();
  int nbRight = std::count_if( in.lhcbIDs().begin(), in.lhcbIDs().end(),
                               [&](const LHCb::LHCbID& id) {  
                    return m_velo->rSensor( id.veloID() )->isRight();
  });
  int nbLeft = in.lhcbIDs().size()-nbRight;

  int firstPhiSensor, lastPhiSensor, step;
  getPhiSensorAndStep( track.get(), firstPhiSensor, lastPhiSensor, step );

  auto firstPhiSensorIter = m_phiHitManager->stationIterAllNoPrep( firstPhiSensor );
  auto lastPhiSensorIter  = m_phiHitManager->stationIterAllNoPrep( lastPhiSensor );

  for (auto  phiSensorIter = firstPhiSensorIter; lastPhiSensorIter != phiSensorIter;
        std::advance(phiSensorIter,step) ) {
    Tf::PatVeloPhiHitManager::Station* sensor = *phiSensorIter;
    if ( !sensor->sensor()->isReadOut() ) continue; // jump sensors not in readout
    if ( !sensor->hitsPrepared() ) m_phiHitManager->prepareHits(sensor);
  }

  const auto& rCoords = track->rCoords();
  int nbOver = std::count_if( rCoords.begin(), rCoords.end(), 
                              [](const Tf::PatVeloRHit* c) { return c->hit()->cluster().highThreshold(); });
  bool isDouble = ( nbOver > rCoords.size() * m_fractionForDouble );
  
  int minNbPhi = int( .5 * m_fractionFound * abs( lastPhiSensor - firstPhiSensor ) );  // two sensors per station.

  if ( m_measureTime ) {
    m_timer->stop ( m_convertTime );
    m_timer->start( m_prepareTime );
  }

  if ( m_printTracks || m_isDebug ) {
    info() << "--- Looking for Track"
           << " from RZ " << track->ancestor()->key()
           << " zone "  << zone
           << " first " << firstPhiSensor
           << " last "  << lastPhiSensor
           << " double " << isDouble
           << " minNbPhi " << minNbPhi
           << " wanted = " << m_wantedKey
           << endmsg;
    for ( const auto& i : track->rCoords()) printCoord( i, "R  " );
    //== Now the PHI hits
    for ( auto phiSensorIter = firstPhiSensorIter; lastPhiSensorIter != phiSensorIter;
          std::advance(phiSensorIter,step) ) {
      Tf::PatVeloPhiHitManager::Station* station = *phiSensorIter;
      if ( !station->sensor()->isReadOut() ) continue; // jump sensors not in readout

      for ( unsigned int kk=0 ; kk<2 ; ++kk ) {
        for ( auto& hit : station->hits( kk ) ) {
          if ( matchKey( hit ) ) {
            hit->setReferencePhi( 0. );
            printCoord( hit, "Phi" );
          }
        }
      }
    }
  }

  //== Select the hits in the proper zones

  std::vector< Tf::PatVeloPhiHit*> allHits;
  std::vector< Tf::PatVeloPhiHit*> extraHits;
  allHits.reserve(1000);
  extraHits.reserve(1000);
  unsigned int nbStationTried   = 0;
  unsigned int lastStationTried = 99999;

  for (auto phiSensorIter = firstPhiSensorIter; lastPhiSensorIter != phiSensorIter;
        std::advance(phiSensorIter,step) ) {
    auto* sensor = *phiSensorIter;
    if ( !sensor->sensor()->isReadOut() ) continue; // jump sensors not in readout

    int side = 0;
    double dxBox = m_velo->halfBoxOffset(side).x();
    if ( sensor->sensor()->isRight() ) {
      side  = 1;
      dxBox = -m_velo->halfBoxOffset(side).x();
    }

    // estimated radius of track in this z position of the sensor.
    double z = sensor->z();
    double r = track->rInterpolated( z );

    if ( m_isDebug ) info() << ".. sens " << sensor->sensor()->sensorNumber()
                            << " z " << z << " r " << r  << endmsg;

    //== Get the phi zone of this r. Some tolerance at boundary...
    const DeVeloPhiType* veloSensor = sensor->sensor();
    if ( veloSensor->outerRadius() < r ) {
      if ( veloSensor->outerRadius() < r-0.05 ) continue; // point outside detector, with tolerance
      r = veloSensor->outerRadius() - 0.001;
    }
    unsigned int phiZone = 0;
    if ( veloSensor->halfboxRRange(phiZone).first > r ) {
      if ( veloSensor->halfboxRRange(phiZone).first > r + 0.05 ) break; // point outside Phi sector
      r = veloSensor->halfboxRRange(phiZone).first + 0.001;
    }
    if ( veloSensor->halfboxRRange(phiZone).second < r ) { //point outside phi sector
      phiZone = 1;
    }

    //== check if extrapolated rz track is compatible with this phi sector, and in which range
    std::pair<double,double> range;
    bool hasRange = m_trackTool->phiRange( r, zone, m_phiAngularTol, sensor, phiZone, range);
    if ( !hasRange ) continue;

    if ( lastStationTried != sensor->stationNumber() ) {
      nbStationTried++;
      lastStationTried = sensor->stationNumber();
    }

    //== The track is using right and left R sensors -> its Phi is in a narrow range...
    if ( nbLeft > 2 && nbRight > 2 ) {
      if ( range.first < 0 ) {
        range = std::make_pair( -Gaudi::Units::halfpi -0.1, -Gaudi::Units::halfpi +0.1 );
      } else {
        range = std::make_pair(  Gaudi::Units::halfpi -0.1,  Gaudi::Units::halfpi +0.1 );
      }
    }
    if ( m_isDebug ) info() << "Phi range " << range.first << " " << range.second << endmsg;
  
    double offset = sensor->sensor()->halfboxPhiOffset(phiZone,r);
    for ( auto& h : sensor->hits(phiZone) ) {

      // check if co-ord is compatible with the min and max for the sector.
      double phi = m_angleUtils.add( h->coordHalfBox(), offset );
      if ( !m_angleUtils.contains( range, phi) ) continue;

      // set a possible `point' (incorporating R info) corresponding to this co-ord
      //== Correct for the box offset
      phi = phi + sin(phi) * dxBox / r;

      h->setRadiusAndPhi( r, phi );
      h->setReferencePhi( phi ); 

      //== Store the first 4 stations in a container, the rest in another.
      if ( 4 < nbStationTried ) {
        extraHits.push_back( h );
      } else {
        allHits.push_back( h );
      }
    }
  }

  std::sort( allHits.begin(), allHits.end(), increasingByReferencePhi() );

  if ( m_isDebug ) {
    for ( const auto& h : allHits ) printCoord( h, "Selected" );
  }

  if ( m_measureTime ) {
    m_timer->stop ( m_prepareTime );
    m_timer->start( m_mainTime );
  }

  //== Fast return if no possible candidate...
  if ( 3 > allHits.size() ) {
    if ( m_measureTime ) m_timer->stop( m_totalTime );
    return {};
  }
  
  std::vector<NewSpaceTrack> newTracks;
  auto itH1 = allHits.begin();
  auto itH2 = itH1 + 2;
  auto prevItH2 = itH1;

  if ( m_isDebug ) info() << "== Search list with minimum size 3" << endmsg;

  //== Find a good range
  while ( itH1 <= allHits.end() - 3 ) {
    if ( itH2 < itH1 + 2 ) itH2 = itH1 + 2;
    if ( itH2 >= allHits.end() ) break;
    //== 1. Find an initial good range
    while ( m_phiMatchZone < (*itH2)->referencePhi() - (*itH1)->referencePhi() ) {
      if ( itH2+1 == allHits.end() ) break;
      ++itH2;
      ++itH1;
    }
    //== 2. extend to maximum
    while ( itH2+1 < allHits.end() &&
            m_phiMatchZone > (*(itH2+1))->referencePhi() - (*itH1)->referencePhi() ) {
      ++itH2;
    }
    //== 3. Check if acceptable range: Not same end, good size, enough hits
    if ( itH2 != prevItH2 &&
         m_phiMatchZone > (*itH2)->referencePhi() - (*itH1)->referencePhi() &&
         itH2 >= itH1 + 2 ) {
      prevItH2 = itH2;
      NewSpaceTrack candidate( itH1, itH2 );
      if ( m_isDebug ) {
        info() << "Candidate zone from " << (*itH1)->referencePhi()
               << " to " << (*itH2)->referencePhi()
               << " = " << (*itH2)->referencePhi() - (*itH1)->referencePhi()
               << " n " << itH2 - itH1 + 1 << endmsg;
        for (const auto& h : candidate) {
          info() << format( "   chi2 %8.2f  dPhi %8.4f", candidate.dist2( h ), candidate.dPhi( h ) );
          printCoord( h, "" );
        }
      }

      bool ok = candidate.removeWorstMultiple( m_maxChi2PerHit, 3, m_isDebug );

      if ( ok ) {
        bool added = addClustersToTrack( extraHits, candidate );
        //== Added clusters: Refit in case some clusters are now outside tolerance
        if ( added ) ok = candidate.removeWorstMultiple( m_maxChi2PerHit, 3, m_isDebug );
      }

      //== Ask for normal length if phi is not close to right-left boundary
      if ( fabs( candidate.averagePhi() - Gaudi::Units::halfpi ) > 0.2 &&
           fabs( candidate.averagePhi() + Gaudi::Units::halfpi ) > 0.2 ) {
        if ( candidate.end() - candidate.begin() < minNbPhi ) {
          if ( m_isDebug ) info() << "Rejected , not enough phi = " 
                                  << candidate.end() - candidate.begin() << " for " << minNbPhi << endmsg;
          ok = false;
        }
      }      

      //== Overall quality should be good enough...
      if ( m_maxQFactor < candidate.qFactor() ) {
        if ( m_isDebug ) info() << "Rejected , qFactor = " << candidate.qFactor() << endmsg;
        ok = false;
      }
        
      if ( ok ) {  //== Store it.
        if ( m_isDebug ) {
          info() << "**** Accepted , qFactor = " << candidate.qFactor() << " ****" << endmsg;
          for (const auto& h : candidate) {
            info() << format( "   chi2 %8.2f", candidate.dist2( h ) );
            printCoord( h, "" );
          }
        }
        newTracks.push_back( std::move(candidate) );
      }
    }
    ++itH1;
  }

  if ( m_isDebug ) info() << "Found " << newTracks.size() << " candidates." << endmsg;

  if ( m_measureTime ) {
    m_timer->stop ( m_mainTime );
    m_timer->start( m_unusedTime );
  }

  //== If no candidate: Take only the unused hits on both lists and try...
  if ( newTracks.empty() ) {
    if ( m_isDebug ) info() << "Try with only unused hits " << endmsg;
    std::vector< Tf::PatVeloPhiHit*> unusedHits;
    std::copy_if( allHits.begin(), allHits.end(),
                  std::back_inserter(unusedHits),
                  [](const Tf::PatVeloPhiHit* h) { return !h->hit()->isUsed(); } );

    bool retry = true;
    while ( retry && 3 <= unusedHits.size() ) {
      retry = false;

      //== Find a good range
      itH1 = unusedHits.begin();
      while ( itH1 <= unusedHits.end() - 3 ) {
        itH2 = itH1 + 2;
        //== 1. Find an initial good range
        while ( m_phiBigZone < (*itH2)->referencePhi() - (*itH1)->referencePhi() ) {
          if ( itH2+1 == unusedHits.end() ) break;
          ++itH2;
          ++itH1;
        }
        //== 2. extend to maximum
        while ( itH2+1 < unusedHits.end() &&
                m_phiBigZone > (*(itH2+1))->referencePhi() - (*itH1)->referencePhi() ) {
          ++itH2;
        }
        if ( m_phiBigZone > (*itH2)->referencePhi() - (*itH1)->referencePhi() &&
             itH2 >= itH1 + 2 ) {
          NewSpaceTrack candidate( itH1, itH2 );
          
          if ( m_isDebug ) {
            info() << "Candidate zone from " << (*itH1)->referencePhi()
                   << " to " << (*itH2)->referencePhi()
                   << " = " << (*itH2)->referencePhi() - (*itH1)->referencePhi()
                   << " n " << itH2 - itH1 + 1 << endmsg;
            for (const auto& h : candidate) {
              info() << format( "   chi2 %8.2f  dPhi %8.4f", candidate.dist2( h ), candidate.dPhi( h ) );
              printCoord( h, "" );
            }
          }
          
          bool ok = candidate.removeWorstMultiple( m_maxChi2PerHit, 3, m_isDebug );
          
          if ( ok ) {
            bool added = addClustersToTrack( allHits, candidate );
            added = added || addClustersToTrack( extraHits, candidate );
            //== Added clusters: Refit in case some clusters are now outside tolerance
            if ( added ) ok = candidate.removeWorstMultiple( m_maxChi2PerHit, 3, m_isDebug );
          }

          //== Ask for normal length if phi is not close to right-left boundary
          if ( fabs( candidate.averagePhi() - Gaudi::Units::halfpi ) > 0.2 &&
               fabs( candidate.averagePhi() + Gaudi::Units::halfpi ) > 0.2 ) {
            if ( candidate.end() - candidate.begin() < minNbPhi ) {
              if ( m_isDebug ) info() << "Rejected , not enough phi = " 
                                      << candidate.end() - candidate.begin() << " for " << minNbPhi << endmsg;
              ok = false;
            }
          }
        
          //== Overall quality should be good enough...
          if ( m_maxQFactor < candidate.qFactor() ) {
            if ( m_isDebug ) info() << "Rejected , qFactor = " << candidate.qFactor() << endmsg;
            ok = false;
          }

          if ( ok ) {  
            if ( m_isDebug ) {
            info() << "**** Accepted , qFactor = " << candidate.qFactor() << " ****" << endmsg;
              for ( const auto& h : candidate) {
                info() << format( "   chi2 %8.2f", candidate.dist2( h ) );
                printCoord( h, "" );
              }
            }
            newTracks.push_back( candidate );
            bool  wasRemoved = false;
            Tf::PatVeloPhiHit* first = *itH1;   // restart in unused at the same hit
            for ( const auto& h : candidate ) {
              auto itErase = std::find( unusedHits.begin(), unusedHits.end(), h );
              if ( itErase != unusedHits.end() ) {
                if ( h == first ) {
                  wasRemoved = true;
                  if ( itErase != unusedHits.begin() ) {
                    first = *(itErase-1);
                  } else {
                    first = *(itErase+1);
                  }
                }
                retry = true;
                unusedHits.erase( itErase );
              }
            }
            itH1 = std::find( unusedHits.begin(), unusedHits.end(), first );
            if ( wasRemoved ) --itH1;
          }
        }
        ++itH1;
      }
    }
  }
  if ( m_isDebug ) info() << "before MergeClone " << endmsg;
  mergeClones( newTracks );
  if ( m_isDebug ) info() << "after MergeClone " << endmsg;

  if ( m_measureTime ) {
    m_timer->stop ( m_unusedTime );
    m_timer->start( m_storeTime );
  }

  double maxQual = 1.e10;
  double secondMinQual = 1.e10;

  unsigned int maxLen = 0;
  //== Get the properties of the best candidate. Invalidate tracks with 3 or 4 clusters, all already used.
  for (auto itTr = newTracks.begin(); newTracks.end() != itTr; ++itTr ) {
    if ( (*itTr).nbHits() < 5 && 0 ==  (*itTr).nbUnused()  ) (*itTr).setValid( false );
    if ( !(*itTr).isValid() ) continue;
    if ( (*itTr).nbHits() >= maxLen ) maxLen = (*itTr).nbHits();
  }
  unsigned int minExpected = maxLen;
  if ( 4 < maxLen ) minExpected = minExpected-1;
  for ( const auto& itr : newTracks ) {
    if ( !itr.isValid() ) continue;
    if ( minExpected <= itr.nbHits() ) {
      if ( maxQual > itr.qFactor() ) {
        secondMinQual = maxQual;
        maxQual = itr.qFactor();
      } else if ( secondMinQual > itr.qFactor() ) {
        secondMinQual = itr.qFactor();
      }
    }
  }
  if ( 3 < maxLen ) maxQual += m_deltaQuality;
  if ( isDouble ) maxQual = secondMinQual + m_deltaQuality;

  //== Store the good candidates
  out.reserve(out.size()+newTracks.size());
  int ioff = 0;
  for (const auto& itr : newTracks) {
    if ( !itr.isValid() ) continue;
    if ( m_isDebug ) {
      info() << "Test track " << ioff++ << " nbHits " << itr.nbHits() << " (min " << minExpected
             << ") qFact " << itr.qFactor() << " (max " << maxQual << ")" << endmsg;
    }
    if ( minExpected > itr.nbHits() ) continue;
    if ( maxQual     < itr.qFactor() ) continue;

    Tf::PatVeloSpaceTrack tempTrack( *track );
    for (const auto& h : itr ) tempTrack.addPhi( h );

    if ( m_isDebug ) {
      info() << "Track with nPhi = " << tempTrack.phiCoords().size()
             << " nR " << tempTrack.nbCoords() << endmsg;
      for (const auto&  h : tempTrack.phiCoords()) printCoord( h, "  " );
    }

    // fit the track trajectory
    tempTrack.fitSpaceTrack( m_stepError, true, true, m_fullErrorPoints );
    if ( m_isDebug ) info() << "Found track with chi^2/ndf " << tempTrack.chi2Dof( ) << endmsg;

    tempTrack.tagClustersAsUsed( Tf::HitBase::UsedByVeloSpace );

    // make the found PatVeloSpaceTrack back into an LHCb::Track
    std::unique_ptr<LHCb::Track> newTrack{ new LHCb::Track{} };
    auto sc = m_trackTool->makeTrackFromPatVeloSpace( tempTrack, *newTrack, m_forwardStepError);
    if (!sc) {
      Warning("Failed to convert to LHCb::Track", StatusCode::SUCCESS).ignore();
      continue;
    }
    newTrack->setHistory(LHCb::Track::PatVelo);
    out.push_back(newTrack.release());
  }

  if ( m_isDebug ) info() << "Returned " << out.size() << " candidates" << endmsg;

  if ( m_measureTime ) {
     m_timer->stop ( m_storeTime );
     m_timer->stop ( m_totalTime );
  }
  
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Get the first and last Phi sensor number, and the step to move inbetween
//=========================================================================
void NewVeloSpaceTool::getPhiSensorAndStep ( Tf::PatVeloSpaceTrack* track,
                                             int& firstPhiSensor, int& lastPhiSensor, int& step ) const {
  Tf::PatVeloRHitManager::StationIterator lastRSensorIter;
  Tf::PatVeloRHitManager::StationIterator firstRSensorIter;
  auto startSensor = m_rHitManager->stationsAllBeginNoPrep();
  auto stopSensor  = std::prev(m_rHitManager->stationsAllEndNoPrep());
  if ( track->backward() ) {
    // backward track
    lastRSensorIter  = m_rHitManager->stationIterAllNoPrep(track->maxRSensor());
    firstRSensorIter = m_rHitManager->stationIterAllNoPrep(track->minRSensor());
    step = +1;
    // get previous and next R sensor numbers on same side
    if (firstRSensorIter != startSensor) --firstRSensorIter;
    if (firstRSensorIter != startSensor) --firstRSensorIter;
    if (lastRSensorIter  != stopSensor)  ++lastRSensorIter;
    if (lastRSensorIter  != stopSensor)  ++lastRSensorIter;
  } else {
    // Forward track, i.e. last sensor is smaller than first one.
    lastRSensorIter  = m_rHitManager->stationIterAllNoPrep(track->minRSensor());
    firstRSensorIter = m_rHitManager->stationIterAllNoPrep(track->maxRSensor());
    step = -1;
    // get previous and next R sensor numbers on same side
    if (lastRSensorIter  != startSensor) --lastRSensorIter;
    if (lastRSensorIter  != startSensor) --lastRSensorIter;
    if (firstRSensorIter != stopSensor)  ++firstRSensorIter;
    if (firstRSensorIter != stopSensor)  ++firstRSensorIter;
  }
  firstPhiSensor = (*firstRSensorIter)->sensor()->associatedPhiSensor()->sensorNumber();
  lastPhiSensor  = (*lastRSensorIter)->sensor()->associatedPhiSensor()->sensorNumber();
}

//=========================================================================
//  Merge tracks sharing hits, tag clones.
//=========================================================================
void NewVeloSpaceTool::mergeClones ( std::vector<NewSpaceTrack>& tracks ) const {
  if ( tracks.size() <= 1 ) return;
  for ( auto it1 = tracks.begin(); tracks.end() != it1 ; ++it1 ) {
    int n1 = (*it1).nbHits();
    for ( auto it2 = it1+1; tracks.end() != it2 ; ++it2 ) {
      if ( !(*it2).isValid() ) continue;
      int n2 = (*it2).nbHits();
      int minN = n1 > n2 ? n2 : n1;
      int nCommon = 0;
      for (auto itH = (*it1).begin(); (*it1).end() != itH ; ++itH ) {
        if ( std::find( (*it2).begin(), (*it2).end(), *itH ) != (*it2).end() ) ++nCommon;
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
//  Add clusters form the list to the track, and update the candidate
//=========================================================================
bool NewVeloSpaceTool::addClustersToTrack ( std::vector<Tf::PatVeloPhiHit*>& hitList,
                                            NewSpaceTrack& candidate ) const {
  bool added = false;
  std::vector<Tf::PatVeloPhiHit*>::const_iterator itH;
  for ( itH = hitList.begin() ; hitList.end() != itH ; ++itH ) {
    if ( fabs( candidate.dPhi( *itH ) ) > 0.1 ) continue;  // reject fast clusters too far
    if ( m_maxChi2ToAdd > candidate.dist2( *itH ) ) {
      if ( std::find( candidate.begin(), candidate.end(), *itH ) != candidate.end() ) continue;
      if ( m_isDebug ) {
        info() << format( "Add cluster with chi2 %8.2f dPhi %8.4f", candidate.dist2( *itH ), candidate.dPhi( *itH ) );
        printCoord( *itH, "" );
      }
      candidate.addCluster( *itH );
      candidate.fitTrack();
      added = true;
    }
  }
  return added;
}
//=============================================================================
