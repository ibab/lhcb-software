// Include files

// local
#include "PatVeloTrackTool.h"
#include "PatVeloSpaceTrack.h"
#include "PatVeloRHitManager.h"
#include "PatVeloPhiHitManager.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloTrackTool
//
// 2006-07-27 : David Hutchcroft
//-----------------------------------------------------------------------------

namespace Tf {
  // Declaration of the Tool Factory
  DECLARE_TOOL_FACTORY( PatVeloTrackTool )

  //=============================================================================
  // Standard constructor, initializes variables
  //=============================================================================
  PatVeloTrackTool::PatVeloTrackTool( const std::string& type,
      const std::string& name,
      const IInterface* parent )
    : GaudiTool ( type, name , parent )
    , m_angleUtils(-Gaudi::Units::pi,Gaudi::Units::pi)
    {
      declareInterface<PatVeloTrackTool>(this);

      declareProperty( "PhiAngularTol"   , m_phiAngularTol    = 0.005     );
      declareProperty( "ChargeThreshold" , m_chargeThreshold  = 15        );
      declareProperty( "highChargeFract" , m_highChargeFract  = 0.7        );
      declareProperty( "RHitManagerName",
		       m_rHitManagerName = "PatVeloRHitManager");
      declareProperty( "PhiHitManagerName",
		       m_phiHitManagerName = "PatVeloPhiHitManager");
      declareProperty( "TracksInHalfBoxFrame",
		       m_tracksInHalfBoxFrame = false );
      declareProperty( "OverlapCorrection" , m_OverlapCorrection = true );
    }
  //=============================================================================
  // Destructor
  //=============================================================================
  PatVeloTrackTool::~PatVeloTrackTool() {}
  //=============================================================================

  StatusCode PatVeloTrackTool::initialize(){
    StatusCode sc = GaudiTool::initialize();
    if(!sc) return sc;

    m_rHitManager   = tool<PatVeloRHitManager>  ( "Tf::PatVeloRHitManager", m_rHitManagerName );
    m_phiHitManager = tool<PatVeloPhiHitManager>( "Tf::PatVeloPhiHitManager", m_phiHitManagerName );

    if( msgLevel( MSG::DEBUG ) ){
      debug() << "=== Tool " << name() << " initialised ==="<< endmsg;
    }

    m_velo = getDet<DeVelo>( DeVeloLocation::Default );
        
    return registerConditionCallBacks();
  }

  // if the track has R hits on both sides but phi hits out of the overlap
  // region delete the R hits
  bool PatVeloTrackTool::cleanNonOverlapTracks(PatVeloSpaceTrack *tr,
			double stepErr,
			unsigned int fullErrorPoints ) const {
    bool isVerbose = msgLevel( MSG::VERBOSE );

    if(isVerbose) verbose() << "Cleaning R overlap hits from track" <<endmsg;

    bool refit = false;
    std::vector<PatVeloRHit*>* rCoords = tr->rCoords();

    std::vector<PatVeloRHit*>::iterator iR =rCoords->begin();
    double xPoint = tr->point().X();
    double yPoint = tr->point().Y();
    double zPoint = tr->point().Z();
    double slopeX = tr->slopeX();
    double slopeY = tr->slopeY();
    while( iR != rCoords->end() ){

      double xTrack = xPoint + slopeX*( (*iR)->z() - zPoint );
      double yTrack = yPoint + slopeY*( (*iR)->z() - zPoint );
      double phiTrack = atan2(yTrack,xTrack);
      if( tr->side() != (*iR)->side() ){
        xTrack += xOffsetOtherHB((*iR)->sensorNumber(),phiTrack);
        yTrack += yOffsetOtherHB((*iR)->sensorNumber(),phiTrack);
        phiTrack = atan2(yTrack,xTrack);
      }
      Gaudi::XYZPoint halfBoxPoint(xTrack,yTrack,(*iR)->z());
      Gaudi::XYZPoint localPoint =
        (*iR)->sensor()->veloHalfBoxToLocal(halfBoxPoint);
      if ( !(*iR)->sensor()->isInActiveArea(localPoint) ) {
        if(isVerbose) {
          verbose() << "deleting cluster " << iR - rCoords->begin() << endmsg;
        }
        iR = rCoords->erase(iR); //delete R coord, get pointer to next cluster
        refit = true;
      }else{
        ++iR; // that cluster was OK, check next
      }
    }
    if(refit){
      if(rCoords->size() < 2) {
        if(isVerbose){
          verbose()
            << "Removed too many R clusters from track: set to invalid"
            << endmsg;
        }
        tr->setValid(false);
        return false;
      }
      // reset phi coords
      this->setPhiCoords(tr);
      tr->fitSpaceTrack( stepErr, true, true, fullErrorPoints );
      return true;
    }else{
      return false;
    }
  }

  void PatVeloTrackTool::setPhiCoords(PatVeloSpaceTrack* track) const {
    // need to reset r and phi of phi clusters which may have been overwritten
    std::vector<PatVeloPhiHit*>* phiCoords = track->phiCoords();
    std::vector<PatVeloPhiHit*>::iterator iP;
    for( iP = phiCoords->begin() ; iP != phiCoords->end() ; ++iP ){
      double r = track->rInterpolated( (*iP)->z() );
      double phi  = (*iP)->sensor()
        ->halfboxPhi((*iP)->hit()->strip(),
                     (*iP)->hit()->interStripFraction(),r);
      if( track->side() != (*iP)->side() ){
      // if track is fitted in other HB frame
        phi += phiOffsetOtherHB((*iP)->sensorNumber(),phi,r);
      }
      (*iP)->setRadiusAndPhi(r,phi);
    }
    return;
  }

  bool PatVeloTrackTool::isSpilloverTrack(PatVeloSpaceTrack* track) const {

    double nbClus = 0.0;
    double nbOver = 0.0;

    // loop over the rs
    for ( std::vector<PatVeloRHit*>::iterator itC = track->rCoords()->begin();
        track->rCoords()->end() != itC; ++itC ) {
      if ( m_chargeThreshold < (*itC)->hit()->signal() ) nbOver += 1.0;
      nbClus += 1.0;
    }

    // and the phis in the glorious leaders scheme
    for ( std::vector<PatVeloPhiHit*>::iterator itC = track->phiCoords()->begin();
        track->phiCoords()->end() != itC; ++itC ) {
      if ( m_chargeThreshold < (*itC)->hit()->signal() ) nbOver += 1.0;
      nbClus += 1.0;
    }

    return (nbClus * m_highChargeFract  > nbOver ? true : false);

  }

  //=============================================================================
  PatVeloSpaceTrack *
    PatVeloTrackTool::makePatVeloSpaceTrack(const LHCb::Track & pTrack) const {
      PatVeloSpaceTrack * newTrack = new PatVeloSpaceTrack(this);

      newTrack->setBackward( pTrack.checkFlag( LHCb::Track::Backward ) );
      newTrack->setZone( pTrack.specific() );
      newTrack->setAncestor( &pTrack );
      // -1 is the default should the RZ track not have an expectation set
      // expect twice as many phi + r as R clusters
      newTrack->setNVeloExpected(2.*pTrack.info(LHCb::Track::nPRVeloRZExpect,-1.));
      newTrack->setSide( (pTrack.specific()<4) ? PatVeloHitSide::Left : 
                                                 PatVeloHitSide::Right ); 
      // copy co-ords from rz track input tracks
      std::vector<LHCb::LHCbID>::const_iterator itR;
      for ( itR = pTrack.lhcbIDs().begin(); pTrack.lhcbIDs().end() != itR; ++itR ){
        LHCb::VeloChannelID id = (*itR).veloID();
        unsigned int sensorNumber = id.sensor();
        PatVeloRHitManager::Station* station = m_rHitManager->stationNoPrep( sensorNumber );
        if ( !station->hitsPrepared() ) m_rHitManager->prepareHits(station);
        int zone         = station->sensor()->globalZoneOfStrip(id.strip());
        PatVeloRHit* coord = station->hitByLHCbID( zone, (*itR));

        if ( 0 == coord ) {
          err() << "PatVeloRhit not found. id " << id
            << " sensor " << sensorNumber << " zone " << zone
            << endmsg;
          delete newTrack;
          return 0;
        } else {
          newTrack->addRCoord( coord );
        }
      }
      newTrack->fitRZ();
      return newTrack;
    }

  void  PatVeloTrackTool::addStateToTrack(PatVeloSpaceTrack * patTrack,
					  LHCb::Track *newTrack,
					  LHCb::State::Location location,
					  const Gaudi::TrackSymMatrix& covariance) const{
    LHCb::State state;
    state.setLocation(location);
    if( !m_tracksInHalfBoxFrame ) { // default to tracks in global frame
      // set box offset here
      const DeVeloSensor *lastSens = (*(patTrack->rCoords()->begin()))->sensor();
      Gaudi::XYZPoint localPoint1(patTrack->point().x(),patTrack->point().y(),
                                  patTrack->point().z());
      Gaudi::XYZPoint global1 = lastSens->veloHalfBoxToGlobal(localPoint1);
      // to do slopes make a point 1 unit in dx and dy offset from default
      Gaudi::XYZPoint localPoint2(patTrack->point().x()+patTrack->slopeX(),
                                 patTrack->point().y()+patTrack->slopeY(),
                                 patTrack->point().z()+1);
      Gaudi::XYZPoint global2 = lastSens->veloHalfBoxToGlobal(localPoint2);

      double globalTx = (global2.x()-global1.x())/(global2.z()-global1.z());
      double globalTy = (global2.y()-global1.y())/(global2.z()-global1.z());

      state.setState( global1.x(),global1.y(),global1.z(), globalTx, globalTy,
                      0.); // q/p unknown from the VELO
      if ( msgLevel( MSG::VERBOSE ) ){
	verbose() << "Offset track from "
                  <<format("xyz (%6.3f,%6.3f,%8.3f) (tx,ty)*10^3 (%6.1f,%6.1f)",
                           patTrack->point().x(),patTrack->point().y(),
                           patTrack->point().z(),
                           1.e3*patTrack->slopeX(),1.e3*patTrack->slopeY())
                  << " to "
                  <<format("xyz (%6.3f,%6.3f,%8.3f) (tx,ty)*10^3 (%6.1f,%6.1f)",
                           state.x(),state.y(),state.z(),1e3*state.tx(),1e3*state.ty())
                  << endmsg;
      }
    }else{
      state.setState(patTrack->point().x(),patTrack->point().y(),patTrack->point().z(),
		     patTrack->slopeX(),patTrack->slopeY(),0.); // q/p unknown from the VELO
      if (msgLevel( MSG::VERBOSE )) verbose() << "Ignore box offset" << endmsg;
    }
    state.setCovariance( covariance );
    newTrack->addToStates( state );
    return;
  }

  StatusCode
  PatVeloTrackTool::makeTrackFromPatVeloSpace(PatVeloSpaceTrack * patTrack,
					      LHCb::Track *newTrack,
					      double forwardStepError,
					      double beamState) const{

    if ( !patTrack->valid() ) return StatusCode::FAILURE;
    LHCb::Track::History history = newTrack->history();
    newTrack->reset(); // in case reusing track
    newTrack->setHistory(history);

    const LHCb::Track* ances = patTrack->ancestor();

    if( ances ) {
      newTrack->addToAncestors( ances );
    }
    std::vector<PatVeloRHit*>::iterator itR;
    for ( itR = patTrack->rCoords()->begin();
          patTrack->rCoords()->end() != itR; ++itR ) {
      newTrack->addToLhcbIDs( (*itR)->hit()->lhcbID() );
    }

    newTrack->setFlag( LHCb::Track::Backward, patTrack->backward() );
    newTrack->setType( LHCb::Track::Velo );
    newTrack->setPatRecStatus( LHCb::Track::PatRecIDs );
    // the number of "expected" r+phi clusters
    if( patTrack->nVeloExpected() > -0.5 ){ // default if unset is -1
      newTrack->addInfo(LHCb::Track::nPRVelo3DExpect,patTrack->nVeloExpected());
    }

    if(beamState){
      addStateToTrack(patTrack,newTrack,LHCb::State::ClosestToBeam,patTrack->covariance());
    }else{
      addStateToTrack(patTrack,newTrack,LHCb::State::FirstMeasurement,patTrack->covariance());
    }

    newTrack->setChi2PerDoF( patTrack->chi2Dof( ) );
    newTrack->setNDoF( patTrack->rCoords()->size() +
        patTrack->phiCoords()->size() - 4 );

    // fit away from z=0 to improve extrapolation to TT and beyond
    // reset phi coords
    this->setPhiCoords(patTrack);
    patTrack->fitSpaceTrack( forwardStepError, false );

    addStateToTrack(patTrack,newTrack,LHCb::State::EndVelo,patTrack->covariance());

    std::vector<PatVeloPhiHit*>::iterator itC;
    for ( itC = patTrack->phiCoords()->begin();
        patTrack->phiCoords()->end() != itC; ++itC ) {
      newTrack->addToLhcbIDs( (*itC)->hit()->lhcbID() );
    }
    // add "no fit" coords from other side of detector
    for ( itR = patTrack->rCoordsNoFit()->begin();
	  patTrack->rCoordsNoFit()->end() != itR; ++itR ) {
      newTrack->addToLhcbIDs( (*itR)->hit()->lhcbID() );
    }
    for ( itC = patTrack->phiCoordsNoFit()->begin();
        patTrack->phiCoordsNoFit()->end() != itC; ++itC ) {
      newTrack->addToLhcbIDs( (*itC)->hit()->lhcbID() );
    }

    newTrack -> setPatRecStatus( LHCb::Track::PatRecIDs );
    return StatusCode::SUCCESS;
  }

  /// compare the phi of a sector with the RZ track to see if compatible
  bool PatVeloTrackTool::
    phiRange( double radius, int RZone, double tol,
        PatVeloPhiHitManager::Station* phiStation, unsigned int phiZone,
        std::pair<double,double>& phiOverlap) {
      // check if extrapolated rz track is compatible with this phi sector
      // RZone is the R sensor zone of the RZ track

      // tol is a tolerance parameter to allow for uncertainties
      // the resulting overlap range, if any, is returned

      // sector radius check
      if ( phiStation->sensor()->halfboxRRange(phiZone).first  > radius )
        return false;
      if ( phiStation->sensor()->halfboxRRange(phiZone).second < radius )
        return false;

      // hits in this phi sector
      if ( phiStation->empty(phiZone) ) return false;

      // Determine which r sensor to use based on the r zone.
      // R zones from 4-7 are on the right hand side of the VELO.
      const DeVeloPhiType* phiSensor = phiStation->sensor();
      const DeVeloRType*   rSensor   = phiSensor->associatedRSensor();
      double offsetPhi(0.);
      // do we need to look at the other side?
      if ((phiSensor->isLeft() && RZone > 3) ||
          (phiSensor->isRight() && RZone < 4)) {
        rSensor = phiSensor->otherSideRSensor();
	offsetPhi = phiOffsetOtherHB(rSensor->sensorNumber(),
				     phiGlobalRZone(RZone),radius);
	tol *= 2.; // double tolerance for overlap sensors
      }
      // does this sensor exist?
      if (!rSensor) return false;

      // map to local zone
      unsigned int localRZone = rSensor->isRight() ? RZone-4 : RZone;

      // get the overlap, if any
      std::pair<double,double> pRange = phiSensor->halfboxPhiRange(phiZone);
      pRange.first += offsetPhi;
      pRange.second += offsetPhi;
      return m_angleUtils.overlap(rSensor->halfboxPhiRange(localRZone),
				  pRange,tol,phiOverlap);
  }

  StatusCode PatVeloTrackTool::registerConditionCallBacks() {
    if(msgLevel(MSG::DEBUG)) debug() << "Register call backs" << endmsg;
    updMgrSvc()->
      registerCondition(this,(*(m_velo->leftSensorsBegin()))->geometry(),
                        &Tf::PatVeloTrackTool::updateBoxOffset);
    updMgrSvc()->
      registerCondition(this,(*(m_velo->rightSensorsBegin()))->geometry(),
                        &Tf::PatVeloTrackTool::updateBoxOffset);
    StatusCode sc = updMgrSvc()->update(this);
    if(!sc.isSuccess())
      return Error("Failed to update conditions!",StatusCode::FAILURE);

    return StatusCode::SUCCESS;
  }

  StatusCode PatVeloTrackTool::updateBoxOffset(){
    if(msgLevel(MSG::DEBUG)) debug() << "updateBoxOffset" << endmsg;
    // set average global zones
    m_phiOfRZone.clear();
    m_phiOfRZone.resize(8,0.);
    std::vector<int> nVal(8,0);
    for ( std::vector<DeVeloRType *>::const_iterator iR =
            m_velo->rSensorsBegin(); iR != m_velo->rSensorsEnd(); ++iR ){
      for( unsigned int zone = 0 ; zone < 4 ; ++zone ){
        double val = ((*iR)->halfboxPhiRange(zone).first+
                      (*iR)->halfboxPhiRange(zone).second)/2.;
	if( fabs((*iR)->halfboxPhiRange(zone).first - 
		 (*iR)->halfboxPhiRange(zone).second) > Gaudi::Units::pi ){
	  // OK tripped over 2pi boundary : add pi to the average 
	  val += Gaudi::Units::pi;
	}	    
        // range to match how RZ tracks have used the zone previously
        if( val < -1.*Gaudi::Units::halfpi ) val += Gaudi::Units::twopi;
        if( val > 3.*Gaudi::Units::halfpi ) val -= Gaudi::Units::twopi;
        if( (*iR)->isLeft() ) {
          m_phiOfRZone[zone] += val;	  
          nVal[zone] += 1;
        }
        if( (*iR)->isRight() ) {
          m_phiOfRZone[zone+4] += val;
          nVal[zone+4] += 1;
        }
      }
    }
    for( unsigned int i = 0 ; i < m_phiOfRZone.size() ; ++i ){
      m_phiOfRZone[i] /= static_cast<double>(nVal[i]);
      if(msgLevel( MSG::VERBOSE )) {
        verbose() << "Global R zone " << i << " is at average phi " 
                  << m_phiOfRZone[i] << endmsg;
      }
    }
    // need to calculate the expected shift of the box to the global frame
    // Lets be unsubtle and make a table of each possible offset
    unsigned int nSens = 132; // include the pileup even if not used here
    std::vector<double> null2;
    null2.resize(2,0.);
    m_XOffsetGlobal.resize(nSens,null2);
    m_YOffsetGlobal.resize(nSens,null2);
    m_XOffsetOtherHB.resize(nSens,null2);
    m_YOffsetOtherHB.resize(nSens,null2);
    m_ROffsetOtherHB.resize(nSens,null2); 
    // do not cache
    std::vector< std::vector<double> > ROffsetGlobal;
    ROffsetGlobal.resize(nSens,null2);
    if( ! m_OverlapCorrection ) {
      return Warning("Overlap correction is off",StatusCode::SUCCESS);
    }
    for ( std::vector<DeVeloSensor*>::const_iterator i =
            m_velo->sensorsBegin() ;
          i !=  m_velo->sensorsEnd() ; ++i ){

      const DeVeloSensor *thisSensor = *i;
      unsigned int sNum = thisSensor->sensorNumber();
      // loop top/bottom
      for(unsigned int tb = 0 ; tb < 2 ; ++tb ){ //tb = 0 top, tb = 1 bottom
        // point at nominal middle of the R zone
        Gaudi::XYZPoint local(0.,25.*(-2.*tb+1),thisSensor->z());
        // other HB -> global -> this HB to get offsets
        Gaudi::XYZPoint global = thisSensor->veloHalfBoxToGlobal(local);
        // convert the local frame point to delta x,y between the frames
        m_XOffsetGlobal[sNum][tb] = global.x() - local.x() ;
        m_YOffsetGlobal[sNum][tb] = global.y() - local.y() ;
        ROffsetGlobal[sNum][tb] = global.rho() - local.rho() ;
	if ( msgLevel( MSG::VERBOSE ) ){
          verbose() << format("Sens %3i %6s to global DX,DY (%5.3f,%5.3f)",
                              sNum,(tb==0 ? "top" : "bottom"),
                              m_XOffsetGlobal[sNum][tb],
                              m_YOffsetGlobal[sNum][tb])
                    << endmsg;
        }
      }
    }
    for ( std::vector<DeVeloSensor*>::const_iterator i =
            m_velo->sensorsBegin() ;
          i !=  m_velo->sensorsEnd() ; ++i ){
      bool isLeft = (*i)->isLeft();
      unsigned int sNum = (*i)->sensorNumber();
      // walk down detector to find first other side sensor
      std::vector<DeVeloSensor*>::const_iterator j = i;
      ++j;
      while( j != m_velo->sensorsEnd() && ( isLeft == (*j)->isLeft() ) ) ++j;
      if ( j ==  m_velo->sensorsEnd() ){ // no more other side at +z try -z
        j = i;
        --j;
        while( isLeft == (*j)->isLeft() ) --j;
      }
      for(unsigned int tb = 0 ; tb < 2 ; ++tb ){ //tb = 0 top, tb = 1 bottom
        m_XOffsetOtherHB[sNum][tb] = m_XOffsetGlobal[sNum][tb] -
          m_XOffsetGlobal[(*j)->sensorNumber()][tb];
        m_YOffsetOtherHB[sNum][tb] = m_YOffsetGlobal[sNum][tb] -
          m_YOffsetGlobal[(*j)->sensorNumber()][tb];
        m_ROffsetOtherHB[sNum][tb] = ROffsetGlobal[sNum][tb] -
          ROffsetGlobal[(*j)->sensorNumber()][tb];
	if ( msgLevel( MSG::VERBOSE ) ){
          verbose() << format("Sens %3i %6s to other HB DX,DY (%5.3f,%5.3f) DR %5.3f",
                              sNum,(tb==0 ? "top" : "bottom"),
                              m_XOffsetOtherHB[sNum][tb],
                              m_YOffsetOtherHB[sNum][tb],
                              m_ROffsetOtherHB[sNum][tb])
                    << endmsg;
        }
      }
    }
    return StatusCode::SUCCESS;
  }

}
