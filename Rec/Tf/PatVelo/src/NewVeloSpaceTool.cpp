// $Id: NewVeloSpaceTool.cpp,v 1.1 2009-04-01 08:11:45 ocallot Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// local
#include "NewVeloSpaceTool.h"
#include "NewSpaceTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : NewVeloSpaceTool
//
// 2009-03-20 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( NewVeloSpaceTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NewVeloSpaceTool::NewVeloSpaceTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_angleUtils(-Gaudi::Units::pi,Gaudi::Units::pi)
{
  declareInterface<ITracksFromTrack>(this);
  declareProperty( "RHitManagerName" ,   m_rHitManagerName    = "PatVeloRHitManager"   );
  declareProperty( "PhiHitManagerName" , m_phiHitManagerName  = "PatVeloPhiHitManager" );
  declareProperty( "TrackToolName",      m_trackToolName      = "PatVeloTrackTool"     );

  declareProperty( "PhiAngularTol",      m_phiAngularTol      = 0.005  );
  declareProperty( "PhiMatchZone",       m_phiMatchZone       = 0.0008 );
  declareProperty( "PhiMatchZoneSlope",  m_phiMatchZoneSlope  = 0.00014);
  declareProperty( "FractionFound",      m_fractionFound      = 0.70   );
  declareProperty( "ForwardStepError",   m_forwardStepError   = 0.009  );
  declareProperty( "MaxChi2PerHit",      m_maxChi2PerHit      = 16.    );
  declareProperty( "FractionForMerge",   m_fractionForMerge   = 0.70   );

  declareProperty( "DebugToolName" ,     m_debugToolName      = ""     );
  declareProperty( "DebugTrackToolName", m_debugTrackToolName = ""     );
  declareProperty( "WantedKey"         , m_wantedKey          = -100   );
  declareProperty( "PrintTracks"       , m_printTracks        = false  );
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

  info() << "Print Tracks = " << m_printTracks << endreq;

  return StatusCode::SUCCESS;
}

//=========================================================================
// Process one input track.
//=========================================================================
StatusCode NewVeloSpaceTool::tracksFromTrack ( const LHCb::Track& in,
                                               std::vector<LHCb::Track*>& out  ) {
  m_isVerbose = msgLevel(MSG::VERBOSE);
  m_isDebug   = msgLevel(MSG::DEBUG);

  if ( 0 != m_debugTrackTool ) {
    //== Get the MC truth of the candidate..
    std::vector<int> keys = m_debugTrackTool->keys( &in );
    for ( unsigned int kk = 0 ; keys.size() > kk ; ++kk ) {
      if ( msgLevel( MSG::DEBUG ) ) m_wantedKey = keys[kk];
      if ( keys[kk] == m_wantedKey ) {
        m_isVerbose = true;
        m_isDebug   = true;
      }
    }
  }


  // turn an LHCb::Track into a PatVeloSpaceTrack for fitting purposes
  Tf::PatVeloSpaceTrack* track = m_trackTool->makePatVeloSpaceTrack( in );
  int zone = track->zone();

  int firstPhiSensor, lastPhiSensor, step;
  getPhiSensorAndStep( track, firstPhiSensor, lastPhiSensor, step );

  Tf::PatVeloPhiHitManager::StationIterator firstPhiSensorIter = m_phiHitManager->stationIterAllNoPrep( firstPhiSensor );
  Tf::PatVeloPhiHitManager::StationIterator lastPhiSensorIter  = m_phiHitManager->stationIterAllNoPrep( lastPhiSensor );
  Tf::PatVeloPhiHitManager::StationIterator phiSensorIter;

  if ( m_printTracks || m_isDebug ) {
    info() << "--- Looking for Track"
           << " from RZ " << track->ancestor()->key()
           << " zone "  << zone
           << " first " << firstPhiSensor
           << " last "  << lastPhiSensor
           << " wanted = " << m_wantedKey
           << endreq;
    for ( std::vector<Tf::PatVeloRHit*>::iterator itC = track->rCoords()->begin();
          track->rCoords()->end() != itC; ++itC ) {
      printCoord( *itC, "R  " );
    }
    //== Now the PHI hits
    for ( phiSensorIter = firstPhiSensorIter; lastPhiSensorIter != phiSensorIter;
          std::advance(phiSensorIter,step) ) {
      Tf::PatVeloPhiHitManager::Station* station = *phiSensorIter;
      if ( !station->sensor()->isReadOut() ) continue; // jump sensors not in readout
      if ( !station->hitsPrepared() ) m_phiHitManager->prepareHits(station);

      for ( unsigned int kk=0 ; 2> kk ; ++kk ) {
        for ( std::vector<Tf::PatVeloPhiHit*>::const_iterator itH = station->hits( kk ).begin();
              station->hits( kk ).end() != itH; ++itH ) {
          if ( matchKey( *itH ) ) printCoord( *itH, "Phi" );
        }
      }
    }
  }

  //== Select the hits in the proper zones

  std::vector< Tf::PatVeloPhiHit*> allHits;
  unsigned int nbStationTried   = 0;
  unsigned int lastStationTried = 99999;

  double zMin =  1000.;
  double zMax = -1000.;

  for ( phiSensorIter = firstPhiSensorIter; lastPhiSensorIter != phiSensorIter;
        std::advance(phiSensorIter,step) ) {
    Tf::PatVeloPhiHitManager::Station* sensor = *phiSensorIter;
    if ( !sensor->sensor()->isReadOut() ) continue; // jump sensors not in readout
    if ( !sensor->hitsPrepared() ) m_phiHitManager->prepareHits(sensor);

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
                            << " z " << z << " r " << r  << endreq;

    //== Get the phi zone of this r
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
    if ( z < zMin ) zMin = z;
    if ( z > zMax ) zMax = z;

    double offset = sensor->sensor()->halfboxPhiOffset(phiZone,r);
    std::vector<Tf::PatVeloPhiHit*>::const_iterator itP;
    for ( itP = sensor->hits(phiZone).begin(); sensor->hits(phiZone).end() != itP; ++itP ) {

      // check if co-ord is compatible with the min and max for the sector.
      double phi = m_angleUtils.add( (*itP)->coordHalfBox(), offset );
      if ( !m_angleUtils.contains( range, phi) ) continue;

      // set a possible `point' (incorporating R info) corresponding to this co-ord
      //== Correct for the box offset
      phi = phi + sin(phi) * dxBox / r;

      (*itP)->setRadiusAndPhi( r, phi );
      (*itP)->setReferencePhi( phi );
      allHits.push_back( *itP );
    }
  }

  std::sort( allHits.begin(), allHits.end(), increasingByReferencePhi() );

  if ( m_isDebug ) {
    for ( std::vector<Tf::PatVeloPhiHit*>::iterator itH = allHits.begin();
          allHits.end() != itH; ++itH ) {
      printCoord( *itH, "Selected" );
    }
  }

  unsigned int minExpected = int( m_fractionFound * nbStationTried + .5 );
  if ( 3 > minExpected ) minExpected = 3;
  if ( minExpected > allHits.size() ) return StatusCode::SUCCESS;
  if ( m_isDebug ) info() << "== Search list with minimum size " << minExpected << endreq;

  std::vector<NewSpaceTrack> newTracks;
  std::vector<Tf::PatVeloPhiHit*>::const_iterator itH1, itH2, itH, prevItH2;
  itH1 = allHits.begin();
  itH2 = itH1 + minExpected - 1;
  prevItH2 = itH1;
  double tolPhi = m_phiMatchZone + m_phiMatchZoneSlope * (zMax - zMin );  // add 0.01 for 100 mm

  if ( allHits.size() < nbStationTried +3 ) tolPhi = 2* tolPhi;

  //== Find a good range
  while ( itH1 <= allHits.end() - minExpected ) {
    if ( itH2 < itH1 + minExpected - 1 ) itH2 = itH1 + minExpected - 1;
    if ( itH2 >= allHits.end() ) break;
    //== 1. Find an initial good range
    while ( tolPhi < (*itH2)->referencePhi() - (*itH1)->referencePhi() ) {
      if ( itH2+1 == allHits.end() ) break;
      ++itH2;
      ++itH1;
    }
    //== 2. extend to maximum
    while ( itH2+1 < allHits.end() &&
            tolPhi > (*(itH2+1))->referencePhi() - (*itH1)->referencePhi() ) {
      ++itH2;
    }
    //== 3. Check if acceptable range: Not same end, good size, enough hits
    if ( itH2 != prevItH2 &&
         tolPhi > (*itH2)->referencePhi() - (*itH1)->referencePhi() &&
         itH2 - itH1 +1 >= minExpected ) {
      prevItH2 = itH2;
      NewSpaceTrack candidate( itH1, itH2 );
      if ( m_isDebug ) {
        info() << "Candidate zone from " << (*itH1)->referencePhi()
               << " to " << (*itH2)->referencePhi()
               << " = " << (*itH2)->referencePhi() - (*itH1)->referencePhi()
               << " n " << itH2 - itH1 + 1 << endreq;
        for ( itH = candidate.begin(); candidate.end() != itH ; ++itH ) {
          info() << format( "   chi2 %8.2f  dPhi %8.4f", candidate.dist2( *itH ), candidate.dPhi( *itH ) );
          printCoord( *itH, "" );
        }
      }

      bool ok = candidate.removeWorstMultiple( m_maxChi2PerHit, minExpected, m_isDebug );

      if ( ok ) {
        if ( m_isDebug ) {
          info() << "**** Accepted , qFactor = " << candidate.qFactor() << " ****" << endreq;
          for ( itH = candidate.begin() ; candidate.end() != itH ; ++itH ) {
            info() << format( "   chi2 %8.2f", candidate.dist2( *itH ) );
            printCoord( *itH, "" );
          }
        }
        newTracks.push_back( candidate );
        minExpected = candidate.nbHits();
        if ( 4 < minExpected ) minExpected = minExpected - 1;
        //== Don't re-find the same track...
        std::sort( candidate.beginHits(), candidate.endHits(), increasingByReferencePhi() );
        Tf::PatVeloPhiHit* first = *candidate.begin();
        while ( *itH1 != first ) itH1++;
      }
    }
    ++itH1;
  }
  //== Filter and convert

  if ( m_isDebug ) info() << "Found " << newTracks.size() << " candidates." << endreq;

  //== If no candidate: Take only the unused hits, and try...
  if ( 0 ==  newTracks.size() ) {
    std::vector< Tf::PatVeloPhiHit*> unusedHits;
    for ( itH = allHits.begin(); allHits.end() != itH; ++itH ) {
      if ( !(*itH)->hit()->isUsed() ) unusedHits.push_back( *itH );
    }
    
    bool retry = true;
    while ( retry && minExpected <= unusedHits.size() ) {
      retry = false;
      NewSpaceTrack candidate( unusedHits.begin(), unusedHits.end()-1 );

      if ( m_isDebug ) {
        info() << "Try all unused hits " << endreq;
        for ( itH = candidate.begin(); candidate.end() != itH ; ++itH ) {
          info() << format( "   chi2 %8.2f  dPhi %8.4f", candidate.dist2( *itH ), candidate.dPhi( *itH ) );
          printCoord( *itH, "" );
        }
      }

      bool ok = candidate.removeWorstMultiple( m_maxChi2PerHit, minExpected, m_isDebug );

      if ( ok ) {
        if ( m_isDebug ) {
          info() << "**** Accepted , qFactor = " << candidate.qFactor() << " ****" << endreq;
          for ( itH = candidate.begin() ; candidate.end() != itH ; ++itH ) {
            info() << format( "   chi2 %8.2f", candidate.dist2( *itH ) );
            printCoord( *itH, "" );
          }
        }
        newTracks.push_back( candidate );
        retry = true;
        for ( itH = candidate.begin(); candidate.end() != itH; ++itH ) {
          unusedHits.erase( std::find( unusedHits.begin(), unusedHits.end(), *itH ) );
        }
      }
    }
  }

  mergeClones( newTracks );

  std::vector<NewSpaceTrack>::iterator itTr;
  double minQual = 1.e10;
  //== Get the properties of the best candidate
  for ( itTr = newTracks.begin(); newTracks.end() != itTr; ++itTr ) {
    if ( !(*itTr).isValid() ) continue;
    if ( (*itTr).nbHits() < 5 && 0 ==  (*itTr).nbUnused()  ) continue;
    if ( minExpected == (*itTr).nbHits() ) {
      if ( minQual > (*itTr).qFactor() ) minQual = (*itTr).qFactor();
    } else if ( (*itTr).nbHits() >= minExpected ) {
      minExpected = (*itTr).nbHits();
      minQual = (*itTr).qFactor();
    }
  }

  //== Tolerance for other lists
  if ( 4 < minExpected ) minExpected = minExpected - 1;
  if ( 3 < minExpected ) minQual += 1.0;

  //== Store the good candidates
  for ( itTr = newTracks.begin(); newTracks.end() != itTr; ++itTr ) {
    if ( !(*itTr).isValid() ) continue;
    if ( m_isDebug ) {
      info() << "Test track " << itTr - newTracks.begin() << " nbHits " << (*itTr).nbHits() << " (min " << minExpected
             << ") qFact " << (*itTr).qFactor() << " (max " << minQual << ")" << endreq;
    }
    if ( (*itTr).nbHits() < 5 && 0 ==  (*itTr).nbUnused()  ) continue;
    if ( minExpected > (*itTr).nbHits()       ) continue;
    if ( minQual < (*itTr).qFactor() ) continue;

    Tf::PatVeloSpaceTrack tempTrack( *track );
    for ( itH = (*itTr).begin() ; (*itTr).end() != itH ; ++itH ) {
      tempTrack.addPhi( *itH );
    }

    if ( m_isDebug ) {
      info() << "Track with nPhi = " << tempTrack.phiCoords()->size()
             << " nR " << tempTrack.nbCoords() << endreq;
      for ( itH = tempTrack.phiCoords()->begin();
            tempTrack.phiCoords()->end() != itH; ++itH ) {
        printCoord( *itH, "  " );
      }
    }

    // fit the track trajectory
    tempTrack.fitSpaceTrack( m_forwardStepError );
    if ( m_isDebug ) info() << "Found track with chi^2/ndf " << tempTrack.chi2Dof( ) << endmsg;

    tempTrack.tagClustersAsUsed( Tf::HitBase::UsedByVeloSpace );

    // make the found PatVeloSpaceTrack back into an LHCb::Track
    LHCb::Track *newTrack = new LHCb::Track();
    newTrack->setHistory(LHCb::Track::PatVelo);
    StatusCode sc = m_trackTool->makeTrackFromPatVeloSpace( &tempTrack, newTrack, m_forwardStepError);
    if (!sc) {
      Warning("Failed to convert to LHCb::Track");
      delete newTrack;
      continue;
    }
    out.push_back(newTrack);
  }

  if ( m_isDebug ) info() << "Returned " << out.size() << " candidates" << endreq;
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Get the first and last Phi sensor number, and the step to move inbetween
//=========================================================================
void NewVeloSpaceTool::getPhiSensorAndStep ( Tf::PatVeloSpaceTrack* track,
                                             int& firstPhiSensor, int& lastPhiSensor, int& step ) {
  Tf::PatVeloRHitManager::StationIterator lastRSensorIter;
  Tf::PatVeloRHitManager::StationIterator firstRSensorIter;
  Tf::PatVeloRHitManager::StationIterator startSensor = m_rHitManager->stationsAllBeginNoPrep();
  Tf::PatVeloRHitManager::StationIterator stopSensor  = m_rHitManager->stationsAllEndNoPrep();
  --stopSensor;
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
void NewVeloSpaceTool::mergeClones ( std::vector<NewSpaceTrack>& tracks ) {
  if ( tracks.size() <= 1 ) return;
  std::vector<NewSpaceTrack>::iterator it1, it2;
  for ( it1 = tracks.begin(); tracks.end() != it1 ; ++it1 ) {
    int n1 = (*it1).nbHits();
    for ( it2 = it1+1; tracks.end() != it2 ; ++it2 ) {
      if ( !(*it2).isValid() ) continue;
      int n2 = (*it2).nbHits();
      int minN = n1 > n2 ? n2 : n1;
      int nCommon = 0;
      std::vector<Tf::PatVeloPhiHit*>::const_iterator itH;
      for ( itH = (*it1).begin(); (*it1).end() != itH ; ++itH ) {
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
//=============================================================================
