// Include files

// from Gaudi
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/reverse.h"

// local
#include "PatVeloGeneralTracking.h"
#include "PatVeloSpaceTrack.h"

#include "Event/ODIN.h"
#include "FindKilledSensors.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloGeneric
//
// 2007-06-15 : David Hutchcroft
//-----------------------------------------------------------------------------

namespace Tf {
  DECLARE_ALGORITHM_FACTORY( PatVeloGeneralTracking )
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

Tf::PatVeloGeneralTracking::PatVeloGeneralTracking( const std::string& name,
                                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_rHitManager(NULL)
  , m_phiHitManager(NULL)
  , m_velo(NULL)
  , m_PatVeloTrackTool(NULL)
  , m_Num3DCreated(0)
  , m_NumEvt(0)
  , m_angleUtils(-Gaudi::Units::pi,Gaudi::Units::pi)
  , m_XOffsetTop(0)
  , m_XOffsetBottom(0)
  , m_YOffsetTop(0)
  , m_YOffsetBottom(0)
  , m_timerTool(NULL)
  , m_killSensorList()
{

  declareProperty( "OutputTracksLocation" ,
                   m_outputTracksLocation = LHCb::TrackLocation::Velo );
  declareProperty( "RHitManagerName", m_rHitManagerName="PatVeloRHitManager");
  declareProperty( "PhiHitManagerName", m_phiHitManagerName="PatVeloPhiHitManager");
  declareProperty( "TrackToolName",          m_trackToolName = "PatVeloTrackTool" );
  declareProperty( "PointErrorScale"  ,   m_ErrScale         = 1.1       );
  declareProperty( "PointErrorMin"    ,   m_ErrMin           =
                   0.250*Gaudi::Units::mm);
  declareProperty( "ErrorExtrapScale" ,   m_ErrExtrapScale   = 1.5       );
  declareProperty( "PhiAngularTol"    ,   m_phiAngularTol    = 0.005     );
  declareProperty( "MaxMissedSensors" ,   m_MaxMissedSensor  = 4         );
  declareProperty( "MaxChiSqDof"      ,   m_chiSqDofMax      = 22        );
  declareProperty( "UseAllCoords"     ,   m_allCoords        = false     );
  declareProperty( "stepError"        ,   m_stepError        = 0.002     );
  declareProperty( "FullErrorPoints"  ,   m_fullErrorPoints  = 5         );
  declareProperty( "forwardStepError" ,   m_forwardStepError = 0.00035   );
  declareProperty( "MaxExtrapStations" ,   m_maxExtrapStat    = 3        );

  declareProperty( "AddSingleClusters",   m_useSingleClusters= false     );
  declareProperty( "SingleStationWin" ,   m_NStationSingle   = 2         );
  declareProperty( "SingleClusTol"    ,   m_singleClusTol    =
                   0.1*Gaudi::Units::mm   );
  declareProperty( "OverlapCorrection" , m_OverlapCorrection = true );

  declareProperty( "OverlapSearch"    , m_overlapSearch = true );
  declareProperty( "OverlapXWindow"    ,
                   m_overlapXWindow = 1.*Gaudi::Units::mm );

  declareProperty( "MaxPointsInZone", m_ZoneMaxPoints = 500 );
  declareProperty( "TimingMeasurement", m_doTiming = false);

  declareProperty( "CCEscan", m_CCEscan = false);
  declareProperty( "KillSensorList", m_killSensorList);
}

//=============================================================================
// Destructor
//=============================================================================

Tf::PatVeloGeneralTracking::~PatVeloGeneralTracking() {}

//=============================================================================
// Initialization
//=============================================================================

StatusCode Tf::PatVeloGeneralTracking::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();

  if ( sc.isFailure() ) return sc;
  m_isDebug   = msgLevel( MSG::DEBUG   );
  m_isVerbose = msgLevel( MSG::VERBOSE );

  if( m_isDebug ) debug() << "==> Initialize" << endmsg;

  m_PatVeloTrackTool = tool<PatVeloTrackTool>("Tf::PatVeloTrackTool",m_trackToolName);

  m_velo = getDet<DeVelo>( DeVeloLocation::Default );

  m_rHitManager   = tool<PatVeloRHitManager>  ( "Tf::PatVeloRHitManager", m_rHitManagerName );
  m_phiHitManager = tool<PatVeloPhiHitManager>( "Tf::PatVeloPhiHitManager", m_phiHitManagerName );

  std::for_each( m_velo->rSensorsBegin(), m_velo->rSensorsEnd(),
                 [&](const DeVeloRType* s) { 
                     m_isLeftRSens[s->sensorNumber()] = s->isLeft(); 
  } );

  m_RSensorsMin = (*m_velo->rSensorsBegin())->sensorNumber();
  m_RSensorsMax = (*m_velo->rSensorsReverseBegin())->sensorNumber();
  m_PhiSensorsMin = (*m_velo->phiSensorsBegin())->sensorNumber();
  m_PhiSensorsMax = (*m_velo->phiSensorsReverseBegin())->sensorNumber();

  m_NSensorSingle = 2*m_NStationSingle;


  if ( m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timerTool->increaseIndent();
    m_veloGeneralTime = m_timerTool->addTimer( "Internal PatVeloGeneral" );
    m_timerTool->decreaseIndent();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode Tf::PatVeloGeneralTracking::execute() {

  if ( m_CCEscan ){
     // Check for ODIN banks:
     LHCb::ODIN *odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);
     if ( odin ){
        m_cceStep = odin->calibrationStep();
        if ( m_cceStep>=0 ){
           FindKilledSensors(m_cceStep,m_killSensorList);
        }
     }
     else {
        fatal() << "There is no ODIN bank" << endmsg;	
 	return StatusCode::FAILURE;
     }
  }

  if ( m_doTiming ) m_timerTool->start( m_veloGeneralTime );

  if( m_isDebug ) debug() << "==> Execute" << endmsg;

  // turn all back to back VeloCoords into 3D points
  PointsContainer createdPoints;
  buildAll3DClusters( createdPoints );

  // sort in accending x for speed later
  for(auto iP = createdPoints.begin() ; iP != createdPoints.end() ; ++iP ){
    std::sort(iP->second.begin(), iP->second.end(), PatVeloLocalPoint::lessX());
  }

  // make tracks
  auto tracks = findTracks(createdPoints,false);

  // if explicit overlap search recheck in overlap mode
  if( m_overlapSearch ) { 
      auto ot = findTracks(createdPoints,true);
      tracks.reserve( tracks.size() + ot.size() );
      std::move( ot.begin(), ot.end(), std::back_inserter(tracks));
  }

  // store tracks
  storeTracks(tracks);

  ++m_NumEvt;

  if ( m_doTiming ) m_timerTool->stop( m_veloGeneralTime );

  return StatusCode::SUCCESS;
}


void Tf::PatVeloGeneralTracking::
buildAll3DClusters( PointsContainer & createdPoints ) {
  // make 3D points from all unused clusters

  //== Loop on R stations in reverse z order
  PatVeloRHitManager::StationReverseIterator rStationReverseIter =
    m_rHitManager->stationsAllReverseBegin();
  PatVeloRHitManager::StationReverseIterator rStationsReverseEnd =
    m_rHitManager->stationsAllReverseEnd();
  for ( ; rStationReverseIter != rStationsReverseEnd; ++rStationReverseIter ) {
    const DeVeloRType* rSensor = (*rStationReverseIter)->sensor();
    if( ! rSensor->isReadOut() ) continue; // skip unreadout sensors
    const DeVeloPhiType* phiSensor = rSensor->associatedPhiSensor();

    if ( m_CCEscan ){
       if( std::binary_search(m_killSensorList.begin(),m_killSensorList.end(),rSensor->sensorNumber()) ){
       if(m_isDebug) debug() << "Killed sensor " << rSensor->sensorNumber()
                          << endmsg;
      continue; // skip as killed for readout purposes
      }
    }

    if ( phiSensor == 0 || ! phiSensor->isReadOut() ) continue;
    PatVeloPhiHitManager::StationIterator phiStationIter
      = m_phiHitManager->stationIterAll(phiSensor->sensorNumber());

    if( (*phiStationIter)->empty(0) &&
        (*phiStationIter)->empty(1) ) continue; // no Phi clusters here!

    for ( unsigned int zone = 0 ;
          (*rStationReverseIter)->nZones() > zone ; ++zone ) {
      if( (*rStationReverseIter)->empty(zone) ) continue; // no R clusters
      build3DClusters(zone,(*rStationReverseIter),(*phiStationIter),
                      createdPoints);
    }
  }
  return;
}

void Tf::PatVeloGeneralTracking::
build3DClusters(int zone,
                PatVeloRHitManager::Station * rStation,
                PatVeloPhiHitManager::Station * phiStation,
                PointsContainer &createdPoints){
  unsigned int RZone = zone;
  if( rStation->sensor()->isRight() ) RZone += 4; // other side of detector

  PatVeloRHitRange rHits = rStation->hits(zone);
  // loop over all clusters in R sector
  for ( PatVeloRHitRange::iterator iRHit = rHits.begin();
        iRHit != rHits.end() ; ++iRHit ) {

    // if cluster already used by something optionally skip
    if ( !m_allCoords &&
         (*iRHit)->hit()->isUsedByAnyOtherThan(Tf::HitBase::UsedByVeloRZ) )
      continue;

    double r = (*iRHit)->coordHalfBox();

    unsigned int phiZone = 0; // inner is 0, outer is 1
    if ( phiStation->sensor()->halfboxRRange(phiZone).second < r ) {
      //point is in outer phi zone (second is the outer r limit)
      phiZone = 1;
    }

    PatVeloPhiHitRange phiHits = phiStation->hits(phiZone);
    double offset = phiStation->sensor()->halfboxPhiOffset(phiZone,r);
    // loop over all clusters in Phi sector
    for ( PatVeloPhiHitRange::iterator iPhiHit = phiHits.begin();
          iPhiHit != phiHits.end() ; ++iPhiHit ){
      // if cluster already used by something optionally skip
      if ( !m_allCoords && (*iPhiHit)->hit()->isUsed() ) continue;

      // check Phi cluster is compatible with R sector phi range
      std::pair<double,double> range;
      bool hasRange =
        m_PatVeloTrackTool->phiRange( r, RZone, m_phiAngularTol,
                                      phiStation, phiZone, range);
      if (!hasRange) continue; // R and phi zones do not overlap
      double phi = (*iPhiHit)->coordHalfBox() + offset;
      // see if phi is in the range
      if(!m_angleUtils.contains(range,phi)) continue;

      double offsetX,offsetY;
      if( m_OverlapCorrection ) {
        offsetX = m_PatVeloTrackTool->xOffsetGlobal((*iRHit)->sensorNumber(),phi);
        offsetY = m_PatVeloTrackTool->yOffsetGlobal((*iRHit)->sensorNumber(),phi);
      }else{
        offsetX = 0.;
        offsetY = 0.;
      }
      createdPoints[(*iRHit)->sensorNumber()].
        push_back(PatVeloLocalPoint(*iRHit,*iPhiHit,phi,
                                    offsetX,offsetY));
    }
  }
  if(m_isVerbose)
    verbose() << "R Sensor " << rStation->sensor()->sensorNumber()
              << " zone " << zone
              << " # R coords " << rStation->hits(zone).size()
              << " Phi sensor " << phiStation->sensor()->sensorNumber()
              << " # phi coords inner " << phiStation->hits(0).size()
              << " outer " << phiStation->hits(1).size()
              << " # points "
              << createdPoints[rStation->sensor()->sensorNumber()].size()
              << endmsg;
  if( createdPoints[rStation->sensor()->sensorNumber()].size() > m_ZoneMaxPoints ){
    createdPoints[rStation->sensor()->sensorNumber()].clear();
    Warning( "Very hot VELO sector, removing coordinates",
	     StatusCode::SUCCESS, 0 ).ignore();
  }
  m_Num3DCreated += createdPoints[rStation->sensor()->sensorNumber()].size();
}

std::vector<std::unique_ptr<Tf::PatVeloSpaceTrack>>
Tf::PatVeloGeneralTracking::findTracks(PointsContainer &points, bool overlap) const 
{
  // starting from back of detector find triplets of clusters in consecutive
  // stations
  std::vector<std::unique_ptr<PatVeloSpaceTrack>> tracks; tracks.reserve(points.size());
  for (auto iS0 = points.rbegin() ; iS0 != points.rend() ; ++iS0 ) {
    if( iS0->second.empty() ) continue; // skip empty stations
    auto iS1 = std::next(iS0);  // get next sensor
    if( !overlap ) {
      // need same side middle sensor
      while( iS1 != points.rend() &&
             m_isLeftRSens.at(iS1->first) != m_isLeftRSens.at(iS0->first) ) ++iS1;
    }else{
      // need other side middle sensor
      while( iS1 != points.rend() &&
             m_isLeftRSens.at(iS1->first) == m_isLeftRSens.at(iS0->first) ) ++iS1;
    }
    if ( iS1 == points.rend() ) continue; // out of sensors to match
    if( iS1->second.empty() ) continue; // skip empty stations
    auto iS2 = std::next(iS1);  // next sensor, same side as first
    while( iS2 != points.rend() &&
           m_isLeftRSens.at(iS2->first) != m_isLeftRSens.at(iS0->first) ) ++iS2;
    if ( iS2 == points.rend() ) continue; // out of sensors to match
    if( iS2->second.empty() ) continue; // skip empty stations

    if(m_isVerbose) verbose() << "Search for triplets in "
                              << iS0->first << ", " << iS1->first
                              << ", " << iS2->first << endmsg;

    // using these three stations make tripets of comptaible clusters
    auto triplets = makeAllGoodTriplets(iS0->second,iS1->second,iS2->second,overlap);
    // remove coords used twice based on chi^2 cuts
    sortAndKeepGoodTriplets(triplets);
    if(m_isVerbose) verbose() << "Found " << triplets.size()
                              << " viable triplets"
                              << (overlap ? " for overlap" : "")
                              << endmsg;
    for( auto& triplet : triplets ) {
      if(!triplet.valid()) continue; // skip killed triplets
      std::unique_ptr<PatVeloSpaceTrack> newTrack{ new PatVeloSpaceTrack(m_PatVeloTrackTool) };
      newTrack->setSide( m_isLeftRSens.at(iS1->first) ? PatVeloHitSide::Left :
                                                        PatVeloHitSide::Right );
      bool goodTrack = extendTrack(triplet,points,*newTrack);
      if( goodTrack ) tracks.push_back(std::move(newTrack));
    }
  }
  return tracks;
}

auto
Tf::PatVeloGeneralTracking::makeAllGoodTriplets(std::vector<PatVeloLocalPoint> &one,
                    std::vector<PatVeloLocalPoint> &two,
                    std::vector<PatVeloLocalPoint> &three,
                    bool overlap) const -> std::vector<PointsList>
{
  std::vector<PointsList> triplets;
  // one, two and three are sorted in X so first look at x projection
  for (auto  iOne = one.begin() ; iOne != one.end() ; ++iOne ){
    if( iOne->used() ) continue;
    if( overlap && fabs(iOne->x()) > m_overlapXWindow ) continue;
    for (auto iThree = three.begin() ; iThree != three.end() ; ++iThree ){
      if( iThree->used() ) continue;
      if( overlap && fabs(iThree->x()) > m_overlapXWindow ) continue;
      auto iTwo = two.begin();
      double ratio = (iTwo->z() - iOne->z())/(iThree->z() - iOne->z());
      double predX = iOne->x() + (iThree->x() - iOne->x())*ratio;
      double predY = iOne->y() + (iThree->y() - iOne->y())*ratio;
      double dPredX = m_ErrScale*std::max(iOne->deltaX(),iThree->deltaX())
        + m_ErrMin;
      double dPredY = m_ErrScale*std::max(iOne->deltaY(),iThree->deltaY())
        + m_ErrMin;

      // get the start and end of the possible middle points in X
      auto iTwoBegin = std::lower_bound(two.begin(),two.end(), predX-dPredX,
                                        PatVeloLocalPoint::lessX() );
      auto iTwoEnd = std::upper_bound(iTwoBegin,two.end(), predX+dPredX,
                                       PatVeloLocalPoint::lessX() );
      while( iTwoEnd - iTwoBegin > 20 ) {
        Warning( "Very hot VELO triplet, reducing search windows",
                 StatusCode::SUCCESS, 0 ).ignore();
        if (m_isVerbose) {
          verbose() << "Very hot VELO triplet, reducing search windows" <<endmsg;
          verbose() << "Number of middle sensor compatible hits was "
                    << iTwoEnd - iTwoBegin
                    << " dPredX " << dPredX << " dPredY " << dPredY << endmsg;
        }

        dPredX /= 2.;
        dPredY /= 2.;
        iTwoBegin = std::lower_bound(two.begin(),two.end(), predX-dPredX,
                      PatVeloLocalPoint::lessX() );
        iTwoEnd = std::upper_bound(iTwoBegin,two.end(), predX+dPredX,
                      PatVeloLocalPoint::lessX() );
      }

      for(auto iTwo = iTwoBegin; iTwo != iTwoEnd ; ++iTwo ){
        if( iTwo->used() ) continue;
        if( fabs(iTwo->y()-predY) < (dPredY+iTwo->deltaY()) ){
          triplets.emplace_back(
                        &(*iOne),&(*iTwo),&(*iThree),
                        std::pow((iTwo->y()-predY)/(dPredY+iTwo->deltaY()),2)+
                        std::pow((iTwo->x()-predX)/(dPredX+iTwo->deltaX()),2),
                        m_isLeftRSens.at(iOne->rSensorNumber()));
        }
      }
    }
  }
  return triplets;
}

void Tf::PatVeloGeneralTracking::
sortAndKeepGoodTriplets(std::vector<PointsList> &triplets) const {
  std::sort(triplets.begin(), triplets.end(), PointsList::lessChi2());
  for( auto& t : triplets ) {
    auto& p = t.points();
    // check all clusters still OK
    if( !p[0]->used() &&
        !p[1]->used() &&
        !p[2]->used() ){
      // kill clusters for other tracks
      for ( unsigned int i = 0 ; i < 3 ; ++i ) {
        p[i]->setUsedByGeneral();
      }
    }else{
      t.setValid(false);
    }
  }
}

bool Tf::PatVeloGeneralTracking::extendTrack(PointsList &trackPoints,
                                             PointsContainer &points,
                                             PatVeloSpaceTrack& newTrack) const {
  // maintain an seperate linear X/Y fit
  PatVeloSpaceTrackLocal::FrameParam xFit,yFit;

  if( m_isVerbose) verbose() << "Extending track " << endmsg;

  bool leftSideSeed = m_isLeftRSens.at(trackPoints.points()[0]->rSensorNumber());

  // load existing points into frames
  // and make a list of used sensors
  std::vector<unsigned int> usedRSensor;
  double lastZ = 0.;
  unsigned int lastSensor = 999;
  usedRSensor.reserve(3);
  for( auto&  point :  trackPoints.points() ) {
    xFit.increment(1./std::pow(point->deltaX(),2),
                   point->x(),point->z());
    yFit.increment(1./std::pow(point->deltaY(),2),
                   point->y(),point->z());
    usedRSensor.push_back(point->rSensorNumber());
    lastZ = point->z();
    lastSensor = std::min(point->rSensorNumber(),lastSensor);
    if(m_isVerbose) verbose()
      << " R sensor " << point->rSensorNumber()
      << " x: " << point->x() << "+-" <<point->deltaX()
      << " y: " << point->y() << "+-" <<point->deltaY()
      << " from [" << point->rSensorNumber() << ","
      << point->rHit()->stripNumber() << "] and ["
      << point->phiHit()->sensorNumber() << ","
      << point->phiHit()->stripNumber() << "]"
      << endmsg;
  }

  for( auto& pcont : reverse(points) ) {
    if( pcont.first >= usedRSensor[0] ) continue;
    if( pcont.second.empty() ) continue; // skip empty containers
    // skip sensors used already
    if ( std::binary_search(usedRSensor.begin(),
                   usedRSensor.end(), pcont.first ) ) continue;
    // predict the impact point
    double xPred = xFit.pred((pcont.second.begin())->z());
    double xPredErr =
      sqrt(xFit.predErr2((pcont.second.begin())->z()));

    double yPred = yFit.pred((pcont.second.begin())->z());
    double yPredErr =
      sqrt(yFit.predErr2((pcont.second.begin())->z()));

    // scale predicted errors by deltaZ * scaleFactor
    // distance between dense stations is 32.3mm,
    // so m_ErrExtrapScale is normalised between adjacent stations
    double predScale;
    if( lastZ > (pcont.second.begin()->z())){
      predScale = ((lastZ- (pcont.second.begin()->z()))/
                   32.3*Gaudi::Units::mm) * m_ErrExtrapScale;
    }else{
      predScale = m_ErrExtrapScale;
    }

    // get the start and end of the possible middle points in X
    auto iPBegin = lower_bound(pcont.second.begin(),pcont.second.end(),
                  xPred-(predScale * xPredErr),
                  PatVeloLocalPoint::lessX());
    auto iPEnd = upper_bound(iPBegin,pcont.second.end(),
                  xPred+(predScale * xPredErr),
                  PatVeloLocalPoint::lessX());

    if(m_isVerbose) verbose()
      << "X range  ( " << xPred << " +- " << (predScale * xPredErr)
      << " ) has " << iPEnd - iPBegin << " points in R sensor "
      << pcont.first << endmsg;

    // put the best match here, if there is one
    PatVeloLocalPoint* bestPoint = nullptr;
    double bestChi2 = 999.;

    for(auto iP = iPBegin; iP != iPEnd ; ++iP ){

      if ( iP->used() ) continue; // skip used hits

      if ( fabs( iP->y() - yPred ) < (predScale*yPredErr) + iP->deltaY() ){
        double chi2 =
          std::pow(( iP->y() - yPred ) /
                    ((predScale * yPredErr)+iP->deltaY() ),2) +
          std::pow(( iP->x() - xPred ) /
                    ((predScale*xPredErr)+iP->deltaX() ),2);
        if(  chi2 < bestChi2  ){
          bestChi2 = chi2;
          bestPoint = &(*iP);
        }
      }
    }
    if( bestPoint ){

      if(m_isVerbose) verbose()
        << " added x: " << bestPoint->x()
        << " y: " << bestPoint->y()
        << " from [" << bestPoint->rSensorNumber() << ","
        << bestPoint->rHit()->stripNumber() << "] and ["
        << bestPoint->phiHit()->sensorNumber() << ","
        << bestPoint->phiHit()->stripNumber() << "]"
        << endmsg;

      xFit.increment(1./std::pow(bestPoint->deltaX(),2),
                     bestPoint->x(),bestPoint->z());
      yFit.increment(1./std::pow(bestPoint->deltaY(),2),
                     bestPoint->y(),bestPoint->z());
      // set hits as used
      bestPoint->rHit()->hit()->setUsed(true);
      bestPoint->phiHit()->hit()->setUsed(true);
      trackPoints.points().push_back(&(*bestPoint));
      lastZ = bestPoint->z();
      lastSensor = std::min(bestPoint->rSensorNumber(),lastSensor);
      usedRSensor.push_back(bestPoint->rSensorNumber());

      if(trackPoints.points().size() > m_maxExtrapStat){
        // subtract off the points we now want to ignore
        // i.e. current - m_maxExtrapStat point in list of points
	PatVeloLocalPoint* removePoint =
          *(trackPoints.points().rbegin()+m_maxExtrapStat);

        if(m_isVerbose) verbose()
          << " removed x: " << removePoint->x()
          << " y: " << removePoint->y()
          << " from [" << removePoint->rSensorNumber() << ","
          << removePoint->rHit()->stripNumber() << "] and ["
          << removePoint->phiHit()->sensorNumber() << ","
          << removePoint->phiHit()->stripNumber() << "]"
          << endmsg;

        xFit.increment(-1./std::pow(removePoint->deltaX(),2),
                       removePoint->x(),removePoint->z());
        yFit.increment(-1./std::pow(removePoint->deltaY(),2),
                       removePoint->y(),removePoint->z());

      }
    }else{
      // protect against negative answer from unsigned int subtraction
      if( (static_cast<int>(lastSensor) - static_cast<int>(pcont.first))
          > m_MaxMissedSensor ) break;
    }
  }


  // turn points into track
  for (const auto& point : trackPoints.points() ) {
    if( leftSideSeed != m_isLeftRSens.at(point->rSensorNumber())){
      newTrack.addRCoordNoFit(point->rHit());
      newTrack.addPhiCoordNoFit(point->phiHit());
    }else{
      newTrack.addRCoord(point->rHit());
      newTrack.addPhi(point->phiHit());
    }
  }

  // optionally add in unassociated clusters
  if(m_useSingleClusters) extendTrackSingleClusters(xFit,yFit,newTrack);

  // sst number of expected clusters
  int nExpected = 2 + (trackPoints.points().front()->rSensorNumber() -
                       trackPoints.points().back()->rSensorNumber());
  // Assume one side only (i.e. either just A or just C)
  newTrack.setNVeloExpected( std::abs(nExpected) );

  return newTrack.nFitCoords() > 4; // chi^2/ndf is NaN if less than 5...
}

void Tf::PatVeloGeneralTracking::
extendTrackSingleClusters(PatVeloSpaceTrackLocal::FrameParam &xFit,
                          PatVeloSpaceTrackLocal::FrameParam &yFit,
                          PatVeloSpaceTrack& newTrack) const {

  // make a list of the sensors to skip as already have clusters
  // (as int to allow subtractions later)
  std::vector<int> usedRSensor;
  std::vector<int> usedPhiSensor;
  usedRSensor.resize(newTrack.rCoords().size());
  usedPhiSensor.resize(newTrack.phiCoords().size());
  std::transform( newTrack.rCoords().begin(), newTrack.rCoords().end(),
                  std::back_inserter(usedRSensor),
                  [](const PatVeloRHit *h) { return h->sensorNumber(); } );
  std::transform( newTrack.phiCoords().begin(), newTrack.phiCoords().end(),
                  std::back_inserter(usedPhiSensor),
                  [](const PatVeloPhiHit *h) { return h->sensorNumber(); } );
  // ensure lists are in accending sensor number order
  sort(usedRSensor.begin(), usedRSensor.end());
  sort(usedPhiSensor.begin(), usedPhiSensor.end());

  // set up limits for looping over sensors
  // and not running off the ends of arrays
  int firstRSensor = std::max((*usedRSensor.begin())-m_NSensorSingle,m_RSensorsMin);
  int lastRSensor = std::min((*usedRSensor.rbegin())+m_NSensorSingle,m_RSensorsMax);
  int firstPhiSensor = std::max((*usedPhiSensor.begin())-m_NSensorSingle,m_PhiSensorsMin);
  int lastPhiSensor = std::min((*usedPhiSensor.rbegin())+m_NSensorSingle,m_PhiSensorsMax);

  // search for R clusters
  for (int currS = firstRSensor; currS != lastRSensor; ++currS ){
    if ( std::binary_search(usedRSensor.begin(),
                            usedRSensor.end(), currS ) ) continue;
    // add R clusters from sensor currS
    PatVeloRHitManager::Station * rStation =
      *(m_rHitManager->stationIterAll(currS));
    int numberOfRSectors=4;
    for ( int sector = 0 ; numberOfRSectors > sector ; ++sector ) {
      if( !rStation->sensor()->isReadOut() ) break; // skip not in readout
      if( rStation->empty(sector) ) continue;
      double trackX = xFit.pred(rStation->z());
      double trackY = yFit.pred(rStation->z());
      double trackPhi = atan2(trackY,trackX);
      double trackR = sqrt(trackX*trackX + trackY*trackY);
      auto phiRange = rStation->sensor()->halfboxPhiRange(sector);
      // check phi range of sector
      if( ! m_angleUtils.contains(phiRange,trackPhi) ) continue;
      PatVeloRHit* rHit =
        rStation->closestHitHalfBox(sector, trackR, m_singleClusTol);
      if(rHit &&
         !rHit->hit()->isUsedByAnyOtherThan(Tf::HitBase::UsedByVeloRZ)) {
        newTrack.addRCoord(rHit);
        rHit->hit()->setUsed(true);
      }
    }
  }
  // search for Phi clusters
  for (int currS = firstPhiSensor; currS != lastPhiSensor; ++currS ){
    if ( std::binary_search(usedPhiSensor.begin(),
                            usedPhiSensor.end(), currS ) ) continue;
    // add R clusters from sensor currS
    int numberOfPhiSectors=2;
    for ( int sector = 0 ; numberOfPhiSectors > sector ; ++sector ) {
      PatVeloPhiHitManager::Station * phiStation =
        *(m_phiHitManager->stationIterAll(currS));
      if( !phiStation->sensor()->isReadOut() ) break; // skip not in readout
      if( phiStation->empty(sector) ) continue;
      double trackX = xFit.pred(phiStation->z());
      double trackY = yFit.pred(phiStation->z());
      double trackR = sqrt(trackX*trackX + trackY*trackY);
      if( trackR > phiStation->sensor()->rMax(sector) ||
          trackR < phiStation->sensor()->rMin(sector) ) continue;
      double trackPhi = atan2(trackY,trackX);
      double offset = phiStation->sensor()->halfboxPhiOffset(sector,trackR);
      // work with coordinate (phi = coord+offset)
      PatVeloPhiHit* phiHit =
        phiStation->closestHitHalfBox(sector, trackPhi-offset,
                                      m_singleClusTol/trackR);
      if(phiHit && !phiHit->hit()->isUsed()) {
        newTrack.addPhi(phiHit);
        phiHit->hit()->setUsed(true);
      }
    }
  }
}

void Tf::PatVeloGeneralTracking::
storeTracks(std::vector<std::unique_ptr<PatVeloSpaceTrack>> & tracks) const {

  LHCb::Tracks * outputTracks = getIfExists<LHCb::Tracks>( m_outputTracksLocation );
  // create output track container on TES, if it does not exist
  if ( !outputTracks ) {
    outputTracks = new LHCb::Tracks();
    put(outputTracks, m_outputTracksLocation );
  }
  // make space for the new tracks
  outputTracks->reserve(outputTracks->size() + tracks.size());

  // fit returned space tracks
  for (auto& track : tracks) {

    if(!track->valid()) continue; // if fails skip to next track

    // fit the RZ trajectory (for meanZ)
    track->fitRZ();
    // set R on phi hits
    m_PatVeloTrackTool->setPhiCoords(*track);
    // fit the track trajectory
    track->fitSpaceTrack( m_stepError, true, true, m_fullErrorPoints );

    // check if probably a backward particle
    // if closest approach to beam line is at +z from average of measurements
    if( track->point().z() > track->meanZ() ) track->setBackward(true);

    // check if spillover
    if (m_PatVeloTrackTool->isSpilloverTrack(*track)) {
      if(m_isVerbose) verbose() << "Spillover track removed" << endmsg;
      continue;
    }

    //  clean the worst fitted examples
    if ( track->chi2Dof( ) > m_chiSqDofMax ) {
      if (m_isVerbose) verbose() << "Skip track with chi^2/ndf "
                                 << track->chi2Dof( )
                                 << endmsg;
      continue;
    }

    if (m_isDebug) debug() << "Found track with chi^2/ndf "
                           << track->chi2Dof( ) << endmsg;

    // make the found PatVeloSpaceTrack back into an LHCb::Track
    std::unique_ptr<LHCb::Track> newTrack{ new LHCb::Track{} };
    auto ok = m_PatVeloTrackTool->makeTrackFromPatVeloSpace(*track, *newTrack, m_forwardStepError);
    if (!ok) {
      Warning("Failed to convert to LHCb::Track",StatusCode::FAILURE,0).ignore();
      continue;
    }
    newTrack->setHistory(LHCb::Track::PatVeloGeneral);
    outputTracks->insert(newTrack.release());
  }
}

StatusCode Tf::PatVeloGeneralTracking::finalize() {
  if( m_isDebug ) debug() << "==> Finalize" << endmsg;
  if( 0 < m_NumEvt ){
    info() << "Created an average of " << (double)m_Num3DCreated / (double)m_NumEvt
           << " 3D Velo Points per event" << endmsg;
  }else{
    info() << "No events processed" << endmsg;
  }
  return GaudiAlgorithm::finalize();
}
