// Include files

// from Gaudi
#include "GaudiKernel/IUpdateManagerSvc.h"

// local
#include "PatVeloGeneralTracking.h"
#include "PatVeloSpaceTrack.h"

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

  std::vector< DeVeloRType* >::const_iterator iRSens;
  for ( iRSens = m_velo->rSensorsBegin() ; iRSens != m_velo->rSensorsEnd() ;
        ++iRSens ){
    m_isLeftRSens[(*iRSens)->sensorNumber()] = (*iRSens)->isLeft();
  }

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

  if ( m_doTiming ) m_timerTool->start( m_veloGeneralTime );

  if( m_isDebug ) debug() << "==> Execute" << endmsg;

  // turn all back to back VeloCoords into 3D points
  PointsContainer createdPoints;
  buildAll3DClusters( createdPoints );

  // sort in accending x for speed later
  PointsContainer::iterator iP;
  for( iP = createdPoints.begin() ; iP != createdPoints.end() ; ++iP ){
    std::sort(iP->second.begin(), iP->second.end(),
              PatVeloLocalPoint::lessX());
  }

  // make tracks
  std::vector<PatVeloSpaceTrack*> tracks;
  findTracks(createdPoints,tracks,false);

  // if explicit overlap search recheck in overlap mode
  if( m_overlapSearch ){
    findTracks(createdPoints,tracks,true);
  }

  // store tracks
  storeTracks(tracks);

  m_NumEvt += 1;

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
  return;
}

void Tf::PatVeloGeneralTracking::
findTracks(PointsContainer &points,
           std::vector<PatVeloSpaceTrack*> &tracks, bool overlap){

  // starting from back of detector find triplets of clusters in consecutive
  // stations
  PointsContainer::reverse_iterator iS0,iS1,iS2;
  for ( iS0 = points.rbegin() ; iS0 != points.rend() ; ++iS0 ){
    if( iS0->second.empty() ) continue; // skip empty stations
    iS1 = iS0;
    ++iS1; // get next sensor
    if( !overlap ) {
      // need same side middle sensor
      while( iS1 != points.rend() &&
             m_isLeftRSens[iS1->first] != m_isLeftRSens[iS0->first] ) ++iS1;
    }else{
      // need other side middle sensor
      while( iS1 != points.rend() &&
             m_isLeftRSens[iS1->first] == m_isLeftRSens[iS0->first] ) ++iS1;
    }
    if ( iS1 == points.rend() ) continue; // out of sensors to match
    if( iS1->second.empty() ) continue; // skip empty stations
    iS2 = iS1;
    ++iS2; // next sensor, same side as first
    while( iS2 != points.rend() &&
           m_isLeftRSens[iS2->first] != m_isLeftRSens[iS0->first] ) ++iS2;
    if ( iS2 == points.rend() ) continue; // out of sensors to match
    if( iS2->second.empty() ) continue; // skip empty stations

    if(m_isVerbose) verbose() << "Search for triplets in "
                              << iS0->first << ", " << iS1->first
                              << ", " << iS2->first << endmsg;

    std::vector<PointsList> triplets;
    // using these three stations make tripets of comptaible clusters
    makeAllGoodTriplets(iS0->second,iS1->second,iS2->second,triplets,overlap);
    // remove coords used twice based on chi^2 cuts
    sortAndKeepGoodTriplets(triplets);
    if(m_isVerbose) verbose() << "Found " << triplets.size()
                              << " viable triplets"
                              << (overlap ? " for overlap" : "")
                              << endmsg;
    for( std::vector<PointsList>::iterator iT = triplets.begin();
         iT != triplets.end(); ++iT ){
      if(!iT->vaild()) continue; // skip killed triplets
      PatVeloSpaceTrack * newTrack = new PatVeloSpaceTrack(m_PatVeloTrackTool);
      newTrack->setSide( m_isLeftRSens[iS1->first] ? PatVeloHitSide::Left :
                             PatVeloHitSide::Right );
      bool goodTrack = extendTrack(*iT,points,newTrack);
      if( goodTrack ) {
        tracks.push_back(newTrack);
      }else{
        delete newTrack;
      }
    }
  }
  return;
}

void Tf::PatVeloGeneralTracking::
makeAllGoodTriplets(std::vector<PatVeloLocalPoint> &one,
                    std::vector<PatVeloLocalPoint> &two,
                    std::vector<PatVeloLocalPoint> &three,
                    std::vector<PointsList> &triplets,
                    bool overlap){
  std::vector<PatVeloLocalPoint>::iterator iOne,iTwo,iThree;
  // one, two and three are sorted in X so first look at x projection
  for ( iOne = one.begin() ; iOne != one.end() ; ++iOne ){
    if( iOne->used() ) continue;
    if( overlap && fabs(iOne->x()) > m_overlapXWindow ) continue;
    for ( iThree = three.begin() ; iThree != three.end() ; ++iThree ){
      if( iThree->used() ) continue;
      if( overlap && fabs(iThree->x()) > m_overlapXWindow ) continue;
      iTwo = two.begin();
      double ratio = (iTwo->z() - iOne->z())/(iThree->z() - iOne->z());
      double predX = iOne->x() + (iThree->x() - iOne->x())*ratio;
      double predY = iOne->y() + (iThree->y() - iOne->y())*ratio;
      double dPredX = m_ErrScale*std::max(iOne->deltaX(),iThree->deltaX())
        + m_ErrMin;
      double dPredY = m_ErrScale*std::max(iOne->deltaY(),iThree->deltaY())
        + m_ErrMin;

      // get the start and end of the possible middle points in X
      std::vector<PatVeloLocalPoint>::iterator iTwoBegin =
        lower_bound(two.begin(),two.end(), predX-dPredX,
                    PatVeloLocalPoint::lessX() );
      std::vector<PatVeloLocalPoint>::iterator iTwoEnd =
        upper_bound(iTwoBegin,two.end(), predX+dPredX,
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
        iTwoBegin =
          lower_bound(two.begin(),two.end(), predX-dPredX,
                      PatVeloLocalPoint::lessX() );
        iTwoEnd =
          upper_bound(iTwoBegin,two.end(), predX+dPredX,
                      PatVeloLocalPoint::lessX() );
      }

      for( iTwo = iTwoBegin; iTwo != iTwoEnd ; ++iTwo ){
        if( iTwo->used() ) continue;
        if( fabs(iTwo->y()-predY) < (dPredY+iTwo->deltaY()) ){
          triplets.push_back
            (PointsList(&(*iOne),&(*iTwo),&(*iThree),
                        std::pow((iTwo->y()-predY)/(dPredY+iTwo->deltaY()),2)+
                        std::pow((iTwo->x()-predX)/(dPredX+iTwo->deltaX()),2),
                        m_isLeftRSens[iOne->rSensorNumber()]));
        }
      }
    }
  }
  return;
}

void Tf::PatVeloGeneralTracking::
sortAndKeepGoodTriplets(std::vector<PointsList> &triplets){
  std::sort(triplets.begin(), triplets.end(), PointsList::lessChi2());
  for( std::vector<PointsList>::iterator iT = triplets.begin();
       iT != triplets.end() ; ++iT ){

    // check all clusters still OK
    if( !iT->points()[0]->used() &&
        !iT->points()[1]->used() &&
        !iT->points()[2]->used() ){
      // kill clusters for other tracks
      for ( unsigned int i = 0 ; i < 3 ; ++i ) {
        iT->points()[i]->setUsedByGeneral();
      }
    }else{
      iT->setValid(false);
    }
  }
  return;
}

bool Tf::PatVeloGeneralTracking::extendTrack(PointsList &trackPoints,
                                             PointsContainer &points,
                                             PatVeloSpaceTrack *newTrack){
  // maintain an seperate linear X/Y fit
  PatVeloSpaceTrackLocal::FrameParam xFit,yFit;

  if( m_isVerbose) verbose() << "Extending track " << endmsg;

  bool leftSideSeed = m_isLeftRSens[trackPoints.points()[0]->rSensorNumber()];

  // load existing points into frames
  // and make a list of used sensors
  std::vector<unsigned int> usedRSensor;
  double lastZ = 0.;
  unsigned int lastSensor = 999;
  usedRSensor.reserve(3);
  std::vector<PatVeloLocalPoint*>::iterator iPoint;
  for( iPoint = trackPoints.points().begin();
       iPoint != trackPoints.points().end(); ++iPoint ){
    xFit.increment(1./std::pow((*iPoint)->deltaX(),2),
                   (*iPoint)->x(),(*iPoint)->z());
    yFit.increment(1./std::pow((*iPoint)->deltaY(),2),
                   (*iPoint)->y(),(*iPoint)->z());
    usedRSensor.push_back((*iPoint)->rSensorNumber());
    lastZ = (*iPoint)->z();
    lastSensor = std::min((*iPoint)->rSensorNumber(),lastSensor);
    if(m_isVerbose) verbose()
      << " R sensor " << (*iPoint)->rSensorNumber()
      << " x: " << (*iPoint)->x() << "+-" <<(*iPoint)->deltaX()
      << " y: " << (*iPoint)->y() << "+-" <<(*iPoint)->deltaY()
      << " from [" << (*iPoint)->rSensorNumber() << ","
      << (*iPoint)->rHit()->stripNumber() << "] and ["
      << (*iPoint)->phiHit()->sensorNumber() << ","
      << (*iPoint)->phiHit()->stripNumber() << "]"
      << endmsg;
  }

  PointsContainer::reverse_iterator iPCont;
  for( iPCont = points.rbegin() ; iPCont != points.rend() ; ++iPCont ){
    if ( iPCont->first >= usedRSensor[0] ) continue;
    if( iPCont->second.empty() ) continue; // skip empty containers
    // skip sensors used already
    if ( std::find(usedRSensor.begin(),
                   usedRSensor.end(), iPCont->first ) != usedRSensor.end() ) continue;
    // predict the impact point
    double xPred = xFit.pred((iPCont->second.begin())->z());
    double xPredErr =
      sqrt(xFit.predErr2((iPCont->second.begin())->z()));

    double yPred = yFit.pred((iPCont->second.begin())->z());
    double yPredErr =
      sqrt(yFit.predErr2((iPCont->second.begin())->z()));

    // scale predicted errors by deltaZ * scaleFactor
    // distance between dense stations is 32.3mm,
    // so m_ErrExtrapScale is normalised between adjacent stations
    double predScale;
    if( lastZ > (iPCont->second.begin()->z())){
      predScale = ((lastZ- (iPCont->second.begin()->z()))/
                   32.3*Gaudi::Units::mm) * m_ErrExtrapScale;
    }else{
      predScale = m_ErrExtrapScale;
    }

    // get the start and end of the possible middle points in X
    std::vector<PatVeloLocalPoint>::iterator iPBegin =
      lower_bound(iPCont->second.begin(),iPCont->second.end(),
                  xPred-(predScale * xPredErr),
                  PatVeloLocalPoint::lessX());
    std::vector<PatVeloLocalPoint>::iterator iPEnd =
      upper_bound(iPBegin,iPCont->second.end(),
                  xPred+(predScale * xPredErr),
                  PatVeloLocalPoint::lessX());

    if(m_isVerbose) verbose()
      << "X range  ( " << xPred << " +- " << (predScale * xPredErr)
      << " ) has " << iPEnd - iPBegin << " points in R sensor "
      << iPCont->first << endmsg;

    // put the best match here, if there is one
    PatVeloLocalPoint* bestPoint = 0;
    double bestChi2 = 999.;
    std::vector<PatVeloLocalPoint>::iterator iP;

    for( iP = iPBegin; iP != iPEnd ; ++iP ){

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
    if( bestPoint != 0 ){

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
      if( (static_cast<int>(lastSensor) - static_cast<int>(iPCont->first))
          > m_MaxMissedSensor ) break;
    }
  }


  // turn points into track
  std::vector<PatVeloLocalPoint*>::iterator ipP;
  for ( ipP = trackPoints.points().begin() ;
        ipP != trackPoints.points().end() ; ++ipP ){

    if( leftSideSeed != m_isLeftRSens[(*ipP)->rSensorNumber()]){
      newTrack->addRCoordNoFit((*ipP)->rHit());
      newTrack->addPhiCoordNoFit((*ipP)->phiHit());
    }else{
      newTrack->addRCoord((*ipP)->rHit());
      newTrack->addPhi((*ipP)->phiHit());
    }
  }

  // optionally add in unassociated clusters
  if(m_useSingleClusters) {
    extendTrackSingleClusters(xFit,yFit,newTrack);
  }

  // sst number of expected clusters
  int nExpected = 2 + (trackPoints.points().front()->rSensorNumber() -
                       trackPoints.points().back()->rSensorNumber());
  // cast to double as that is how value is stored in LHCb::Track
  // Assume one side only (i.e. either just A or just C)
  newTrack->setNVeloExpected( fabs(static_cast<double>(nExpected)) );

  if( newTrack->nFitCoords() < 5 ){ // chi^2/ndf is NaN if less...
    return false;
  }

  return true; // track is OK
}

void Tf::PatVeloGeneralTracking::
extendTrackSingleClusters(PatVeloSpaceTrackLocal::FrameParam &xFit,
                          PatVeloSpaceTrackLocal::FrameParam &yFit,
                          PatVeloSpaceTrack *newTrack){

  // make a list of the sensors to skip as already have clusters
  // (as int to allow subtractions later)
  std::vector<int> usedRSensor;
  std::vector<int> usedPhiSensor;
  usedRSensor.resize(newTrack->rCoords()->size());
  usedPhiSensor.resize(newTrack->phiCoords()->size());
  std::vector< PatVeloRHit * >::iterator iRHit;
  for( iRHit = newTrack->rCoords()->begin() ;
       iRHit != newTrack->rCoords()->end() ; ++iRHit ){
    usedRSensor.push_back((*iRHit)->sensorNumber());
  }
  std::vector< PatVeloPhiHit * >::iterator iPhiHit;
  for( iPhiHit = newTrack->phiCoords()->begin() ;
       iPhiHit != newTrack->phiCoords()->end() ; ++iPhiHit ){
    usedPhiSensor.push_back((*iPhiHit)->sensorNumber());
  }
  // ensure lists are in accending sensor number order
  sort(usedRSensor.begin(), usedRSensor.end());
  sort(usedPhiSensor.begin(), usedPhiSensor.end());

  // set up limits for looping over sensors
  // and not running off the ends of arrays
  int firstRSensor =
    std::max((*usedRSensor.begin())-m_NSensorSingle,m_RSensorsMin);
  int lastRSensor =
    std::min((*usedRSensor.rbegin())+m_NSensorSingle,m_RSensorsMax);
  int firstPhiSensor =
    std::max((*usedPhiSensor.begin())-m_NSensorSingle,m_PhiSensorsMin);
  int lastPhiSensor =
    std::min((*usedPhiSensor.rbegin())+m_NSensorSingle,m_PhiSensorsMax);
  int currS;

  // search for R clusters
  for ( currS = firstRSensor; currS != lastRSensor; ++currS ){
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
      std::pair<double,double> phiRange =
        rStation->sensor()->halfboxPhiRange(sector);
      // check phi range of sector
      if( ! m_angleUtils.contains(phiRange,trackPhi) ) continue;
      PatVeloRHit* rHit =
        rStation->closestHitHalfBox(sector, trackR, m_singleClusTol);
      if(rHit &&
         !rHit->hit()->isUsedByAnyOtherThan(Tf::HitBase::UsedByVeloRZ)) {
        newTrack->addRCoord(rHit);
        rHit->hit()->setUsed(true);
      }
    }
  }
  // search for Phi clusters
  for ( currS = firstPhiSensor; currS != lastPhiSensor; ++currS ){
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
        newTrack->addPhi(phiHit);
        phiHit->hit()->setUsed(true);
      }
    }
  }
  return;
}

void Tf::PatVeloGeneralTracking::
storeTracks(std::vector<PatVeloSpaceTrack*> & tracks){

  LHCb::Tracks * outputTracks;
  // create output track container on TES, if it does not exist
  if ( exist<LHCb::Tracks>( m_outputTracksLocation ) ) {
    outputTracks = get<LHCb::Tracks>( m_outputTracksLocation );
  } else {
    outputTracks = new LHCb::Tracks();
    put(outputTracks, m_outputTracksLocation );
  }
  // make space for the new tracks
  outputTracks->reserve(outputTracks->size() + tracks.size());

  // fit returned space tracks
  std::vector<PatVeloSpaceTrack*>::iterator iTr;

  for ( iTr = tracks.begin(); tracks.end() != iTr; ++iTr ) {

    if(!(*iTr)->valid()) continue; // if fails skip to next track

    // fit the RZ trajectory (for meanZ)
    (*iTr)->fitRZ();
    // set R on phi hits
    m_PatVeloTrackTool->setPhiCoords(*iTr);
    // fit the track trajectory
    (*iTr)->fitSpaceTrack( m_stepError, true, true, m_fullErrorPoints );

    // check if probably a backward particle
    // if closest approach to beam line is at +z from average of measurements
    if( (*iTr)->point().z() > (*iTr)->meanZ() ) (*iTr)->setBackward(true);

    // check if spillover
    if (m_PatVeloTrackTool->isSpilloverTrack(*iTr)) {
      if(m_isVerbose) verbose() << "Spillover track removed" << endmsg;
      continue;
    }

    //  clean the worst fitted examples
    if ( (*iTr)->chi2Dof( ) > m_chiSqDofMax ) {
      if (m_isVerbose) verbose() << "Skip track with chi^2/ndf "
                                 << (*iTr)->chi2Dof( )
                                 << endmsg;
      continue;
    }

    if (m_isDebug) debug() << "Found track with chi^2/ndf "
                           << (*iTr)->chi2Dof( ) << endmsg;

    // make the found PatVeloSpaceTrack back into an LHCb::Track
    LHCb::Track* newTrack = new LHCb::Track();
    outputTracks->insert(newTrack);
    newTrack->setHistory(LHCb::Track::PatVeloGeneral);
    StatusCode sc =
      m_PatVeloTrackTool->makeTrackFromPatVeloSpace((*iTr),newTrack,
                                                    m_forwardStepError);
    if (!sc) {
      Warning("Failed to convert to LHCb::Track",sc,0).ignore();
      delete newTrack;
      continue;
    }
  }

  // delete all PatVeloSpaceTracks
  for ( iTr = tracks.begin(); tracks.end() != iTr; ++iTr ) {
    delete (*iTr);
  }

  return;
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
