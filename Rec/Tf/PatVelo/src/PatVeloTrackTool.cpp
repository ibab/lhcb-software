// $Id: PatVeloTrackTool.cpp,v 1.13 2010/02/18 14:12:07 dhcroft Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

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
  DECLARE_TOOL_FACTORY( PatVeloTrackTool );


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
      debug() << "=== Tool " << name() << " initialised ==="<< endreq;
    }

    return StatusCode::SUCCESS;
  }

  // if the track has R hits on both sides but phi hits out of the overlap 
  // region delete the R hits
  bool PatVeloTrackTool::cleanNonOverlapTracks(PatVeloSpaceTrack *tr, 
			double stepErr, 
			unsigned int fullErrorPoints ) const {
    bool isVerbose = msgLevel( MSG::VERBOSE );

    if(isVerbose) verbose() << "Cleaning R overlap hits from track" <<endreq;

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
      Gaudi::XYZPoint halfBoxPoint(xTrack,yTrack,(*iR)->z());
      Gaudi::XYZPoint localPoint = 
        (*iR)->sensor()->veloHalfBoxToLocal(halfBoxPoint);
      if ( !m_angleUtils.
          contains((*iR)->sensor()->halfboxPhiRange((*iR)->hit()->zone()),
            phiTrack,m_phiAngularTol) ||
          !(*iR)->sensor()->isInActiveArea(localPoint) ) {
        if(isVerbose) {
          verbose() << "deleting cluster " << iR - rCoords->begin() << endreq;
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
            << endreq;
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
      double phi = (*iP)->sensor()->halfboxPhi((*iP)->hit()->strip(),
					       (*iP)->hit()->interStripFraction(),r);
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
      PatVeloSpaceTrack * newTrack = new PatVeloSpaceTrack();

      newTrack->setBackward( pTrack.checkFlag( LHCb::Track::Backward ) );
      newTrack->setZone( pTrack.specific() );
      newTrack->setAncestor( &pTrack );
      // -1 is the default should the RZ track not have an expectation set
      // expect twice as many phi + r as R clusters
      newTrack->setNVeloExpected(2.*pTrack.info(LHCb::Track::nPRVeloRZExpect,-1.));

      // copy co-ords from rz track input tracks
      std::vector<LHCb::LHCbID>::const_iterator itR;
      for ( itR = pTrack.lhcbIDs().begin(); pTrack.lhcbIDs().end() != itR; ++itR ){
        LHCb::VeloChannelID id = (*itR).veloID();
        int sensorNumber = id.sensor();
        PatVeloRHitManager::Station* station = m_rHitManager->stationNoPrep( sensorNumber );
        if ( !station->hitsPrepared() ) m_rHitManager->prepareHits(station);        
        int zone         = station->sensor()->globalZoneOfStrip(id.strip());
        PatVeloRHit* coord = station->hitByLHCbID( zone, (*itR));

        if ( 0 == coord ) {
          err() << "PatVeloRhit not found. id " << id
            << " sensor " << sensorNumber << " zone " << zone
            << endreq;
          delete newTrack;
          return 0;
        } else {
          newTrack->addRCoord( coord );
        }
      }
      newTrack->fitRZ();
      return newTrack;
    }

  void  PatVeloTrackTool::addStateToTrack(PatVeloSpaceTrack * patTrack, LHCb::Track *newTrack,
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
                  << endreq;    
      }
    }else{
      state.setState(patTrack->point().x(),patTrack->point().y(),patTrack->point().z(),
		     patTrack->slopeX(),patTrack->slopeY(),0.); // q/p unknown from the VELO
      if (msgLevel( MSG::VERBOSE )) verbose() << "Ignore box offset" << endreq;
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
      newTrack->setLhcbIDs( ances->lhcbIDs() );
      newTrack->addToAncestors( ances );
    }else{
      std::vector<PatVeloRHit*>::iterator itC;
      for ( itC = patTrack->rCoords()->begin(); 
          patTrack->rCoords()->end() != itC; ++itC ) {
        newTrack->addToLhcbIDs( (*itC)->hit()->lhcbID() );
      }
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
    std::vector<PatVeloRHit*>::iterator itR;
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
      // do we need to look at the other side?
      if ((phiSensor->isLeft() && RZone > 3) || 
          (phiSensor->isRight() && RZone < 4)) {
        rSensor = phiSensor->otherSideRSensor();
      }
      // does this sensor exist?
      if (!rSensor) return false;

      // map to local zone
      unsigned int localRZone = rSensor->isRight() ? RZone-4 : RZone;

      // get the overlap, if any
      return m_angleUtils.overlap(rSensor->halfboxPhiRange(localRZone),
          phiSensor->halfboxPhiRange(phiZone),
          tol,phiOverlap);
    }

}
