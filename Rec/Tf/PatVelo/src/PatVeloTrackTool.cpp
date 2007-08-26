// $Id: PatVeloTrackTool.cpp,v 1.1.1.1 2007-08-26 21:03:29 krinnert Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "PatVeloTrackTool.h"
#include "PatVeloSpaceTrack.h"
#include "PatVeloRHitManager.h"
#include "PatVeloPhiHitManager.h"
#include "Event/Track.h"

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

    }
  //=============================================================================
  // Destructor
  //=============================================================================
  PatVeloTrackTool::~PatVeloTrackTool() {} 
  //=============================================================================

  StatusCode PatVeloTrackTool::initialize(){
    StatusCode sc = GaudiTool::initialize();
    if(!sc) return sc;

    m_rHitManager   = tool<PatVeloRHitManager>  ( "Tf::PatVeloRHitManager" );
    m_phiHitManager = tool<PatVeloPhiHitManager>( "Tf::PatVeloPhiHitManager" );

    info() << "=== Tool " << name() << " param. ==="<< endreq
      << "PhiAngularTol        = " << m_phiAngularTol       << endreq
      << "highChargeFract      = " << m_highChargeFract     << endreq
      << "ChargeThreshold      = " << m_chargeThreshold     << endreq
      << "================================================="<< endreq;

    return StatusCode::SUCCESS;
  }

  // if the track has R hits on both sides but phi hits out of the overlap region
  // delete the R hits
  bool PatVeloTrackTool::cleanNonOverlapTracks(PatVeloSpaceTrack *tr, 
      double stepErr ) const {
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
      if ( !m_angleUtils.contains((*iR)->sensor()->halfboxPhiRange((*iR)->hit()->zone()),phiTrack,m_phiAngularTol)) {
        if(isVerbose) {
          verbose() << "deleting cluster " << iR - rCoords->begin() << endreq;
        }
        iR = rCoords->erase(iR); // delete R coord, get pointer to next cluster
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
      tr->fitSpaceTrack( stepErr );
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
      double z = (*iP)->z();
      double r = track->rInterpolated( z );      
      (*iP)->setRadiusAndPhi(r,(*iP)->coordHalfBox());
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

      // copy co-ords from rz track input tracks
      std::vector<LHCb::LHCbID>::const_iterator itR;
      for ( itR = pTrack.lhcbIDs().begin(); pTrack.lhcbIDs().end() != itR; ++itR ){
        LHCb::VeloChannelID id = (*itR).veloID();
        int sensorNumber = id.sensor();
        PatVeloRHitManager::Station* station = m_rHitManager->station( sensorNumber );
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

  StatusCode PatVeloTrackTool::makeTrackFromPatVeloSpace(PatVeloSpaceTrack * patTrack,
        LHCb::Track *newTrack, 
        double forwardStepError) const{

      if ( !patTrack->valid() ) return StatusCode::FAILURE;
      LHCb::Track::History history = newTrack->history();
      newTrack->reset(); // in case reusing track
      newTrack->setHistory(history);

      const LHCb::Track* ances = patTrack->ancestor();

      if( ances ) {
        newTrack->setLhcbIDs( ances->lhcbIDs() );
        newTrack->addToAncestors( ances );
        newTrack->addToStates( ances->firstState() );
      }else{
        std::vector<PatVeloRHit*>::iterator itC;
        for ( itC = patTrack->rCoords()->begin(); 
            patTrack->rCoords()->end() != itC; ++itC ) {
          newTrack->addToLhcbIDs( (*itC)->hit()->lhcbID() );
        }
        newTrack->addToStates(LHCb::State());
      }

      newTrack->setFlag( LHCb::Track::Backward, patTrack->backward() );
      newTrack->setType( LHCb::Track::Velo );
      newTrack->setPatRecStatus( LHCb::Track::PatRecIDs );

      // set box offset here
      Gaudi::XYZPoint localZero(0.,0.,0.);
      Gaudi::XYZPoint global = (*(patTrack->rCoords()->begin()))->sensor()->veloHalfBoxToGlobal(localZero);
      Gaudi::XYZVector boxOffset = global-localZero;
      if ( msgLevel( MSG::VERBOSE ) ){
        verbose() << "Offset track by " << boxOffset << endreq;
      }

      newTrack->firstState().setState( patTrack->point().x() + boxOffset.x(),
          patTrack->point().y() + boxOffset.y(),
          patTrack->point().z() + boxOffset.z(),
          patTrack->slopeX(),
          patTrack->slopeY(),
          0.); // q/p unknown from the VELO

      newTrack->firstState().setLocation( LHCb::State::ClosestToBeam );
      newTrack->firstState().setCovariance( patTrack->covariance() );
      newTrack->setChi2PerDoF( patTrack->chi2Dof( ) );
      newTrack->setNDoF( patTrack->rCoords()->size() + 
          patTrack->phiCoords()->size() - 4 );

      // fit away from z=0 to improve extrapolation to TT and beyond
      // reset phi coords
      this->setPhiCoords(patTrack);
      patTrack->fitSpaceTrack( forwardStepError, false );
      LHCb::State lastVeloState;
      lastVeloState.setState( patTrack->point().x() + boxOffset.x(),
          patTrack->point().y() + boxOffset.y(),
          patTrack->point().z() + boxOffset.z(),
          patTrack->slopeX(),
          patTrack->slopeY(),
          0.); // q/p unknown from the VELO
      lastVeloState.setLocation( LHCb::State::EndVelo );
      lastVeloState.setCovariance( patTrack->covariance() );
      newTrack->addToStates( lastVeloState );

      std::vector<PatVeloPhiHit*>::iterator itC;
      for ( itC = patTrack->phiCoords()->begin(); 
          patTrack->phiCoords()->end() != itC; ++itC ) {
        newTrack->addToLhcbIDs( (*itC)->hit()->lhcbID() );
      }
      newTrack -> setPatRecStatus( LHCb::Track::PatRecIDs );
      return StatusCode::SUCCESS;
    }
}
