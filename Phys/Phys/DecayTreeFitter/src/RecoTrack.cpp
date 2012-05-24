#include <stdio.h>

#include "Event/Particle.h"
#include "RecoTrack.h"
#include "FitParams.h"
#include "TrackKernel/TrackTraj.h"

namespace {
  void applyBremCorrection( LHCb::State& state,
			    double bremEnergy,
			    double bremEnergyCov )
  {
    int charge = state.qOverP() > 0 ? 1 : -1 ;
    double momentum       = charge/state.qOverP();
    double momentumCov = state.covariance()(4,4) * std::pow(momentum,4) ;
    momentum += bremEnergy ;
    momentumCov += bremEnergyCov ;
    state.setQOverP( charge / momentum ) ;
    state.covariance()(4,4) = momentumCov / std::pow(momentum,4) ;
  }
}

namespace DecayTreeFitter
{

  extern int vtxverbose ;

  RecoTrack::RecoTrack(const LHCb::Particle& cand, 
		       const ParticleBase* mother)
    : RecoParticle(cand,mother),
      m_track(cand.proto()->track()),
      m_cached(false),
      m_flt(0),
      m_bremEnergy(0),
      m_bremEnergyCov(0)
  {
    // set it to something such that we have a momentum estimate
    m_state = m_track->firstState() ;

    // get the bremenergy correction
    if( particle().particleID().abspid() == 11 ) {
      // FIXME: this is a hack. it will go wrong if you fit the
      // updated decay tree twice. to do this properly we need to use
      // the bremadding tools, but there are other problems with that.
      double momentumFromTrack    = 1/std::abs(m_state.qOverP() ) ;
      double momentumError2FromTrack = m_state.covariance()(4,4) * std::pow(momentumFromTrack,4) ;
      double momentumFromParticle = particle().momentum().P() ;
      double momentumError2FromParticle = 
	particle().momCovMatrix()(3,3) * std::pow(particle().momentum().E()/momentumFromParticle,2) ;
      m_bremEnergyCov = momentumError2FromParticle - momentumError2FromTrack ;
      m_bremEnergy = momentumFromParticle - momentumFromTrack ;
      // if the correction is too small or unphysical, ignore it. the units are MeV.
      if( !(m_bremEnergyCov >1*Gaudi::Units::MeV*Gaudi::Units::MeV && 
	    m_bremEnergy>1*Gaudi::Units::MeV ) ) 
	m_bremEnergy = m_bremEnergyCov = 0 ;
      if(vtxverbose>=1) {
	std::cout << "Fitting an electron. ("
		  << momentumFromTrack << " +/- " << std::sqrt( momentumError2FromTrack ) << ") --> ("
		  << momentumFromParticle << " +/- " << std::sqrt( momentumError2FromParticle ) << ")" << std::endl ;
	std::cout << "brem correction: " << m_bremEnergy << " +/- " << std::sqrt( m_bremEnergyCov ) << std::endl ;
      }
      applyBremCorrection(m_state,m_bremEnergy,m_bremEnergyCov) ;
    }
  }
  
  RecoTrack::~RecoTrack() {}

  ErrCode
  RecoTrack::initPar2(FitParams* fitparams)
  {
    ErrCode rc = updCache(*fitparams) ;
    const Gaudi::XYZVector& recoP = m_state.momentum() ;
    int momindex = momIndex() ;
    fitparams->par(momindex+1) = recoP.x() ;
    fitparams->par(momindex+2) = recoP.y() ;
    fitparams->par(momindex+3) = recoP.z() ;
    return rc ;
  }
  
  ErrCode
  RecoTrack::initCov(FitParams* fitparams) const 
  {
    // we only need a rough estimate of the covariance 
    assert(m_cached) ;
    Gaudi::SymMatrix3x3 covP = m_state.errMomentum() ;
    int momindex = momIndex() ;
    for(int row=1; row<=3; ++row) 
      fitparams->cov()(momindex+row,momindex+row) = 1000*covP(row-1,row-1) ;
    return ErrCode() ;
  }
  
  ErrCode
  RecoTrack::updCache(const FitParams& fitparams)
  {
    // we'll do this properly another time
    double z = fitparams.par()(mother()->posIndex() + 3 ) ;
    m_state = m_track->closestState(z) ;

    // temporary hack: inside RecoTrack we only use linear
    // extrapolation. for V0 this is not good enough. so, if the
    // closest state is not 'ClosestToBeamLine' or if we are
    // extrapolating far or are far away from the beam line, we'll use
    // tracktraj.  why not use TrackTraj always? actually there isn't
    // a really good reason, but you would slightly overestimate the
    // errors for prompt tracks in between the first-measurement state
    // and the closest-to-beam state.
    const double maxR = 1 * Gaudi::Units::cm ;
    const double maxDeltaZ = 10 * Gaudi::Units::cm ;
    double dz = z - m_state.z() ;
    double x = m_state.x() + dz * m_state.tx() ;
    double y = m_state.y() + dz * m_state.ty() ;
    double r = std::sqrt( x*x + y*y ) ;
    if( m_state.location() != LHCb::State::ClosestToBeam || r > maxR || std::abs(dz) > maxDeltaZ ) {
      LHCb::TrackTraj traj( *m_track ) ;
      m_state = traj.state( z ) ;
    }

    //deal with bremstrahlung
    if( m_bremEnergyCov > 0 ) 
      applyBremCorrection(m_state,m_bremEnergy,m_bremEnergyCov) ;
    
    m_cached = true ;
    return ErrCode() ;
  }

  HepVector symdiag(const HepSymMatrix& m) {
    HepVector rc(m.num_row()) ;
    for(int i=1; i<=m.num_row(); ++i)
      rc(i) = std::sqrt(m.fast(i,i)) ;
    return rc ;
  }
  
  ErrCode
  RecoTrack::projectRecoConstraint(const FitParams& fitparams, Projection& p) const
  {
    ErrCode status ;
    assert(m_cached) ;

    int posindex = mother()->posIndex() ;
    Gaudi::XYZPoint motherpos(fitparams.par()(posindex+1),
			      fitparams.par()(posindex+2),
			      fitparams.par()(posindex+3)) ;

    int momindex = momIndex() ;
    double px = fitparams.par()(momindex+1) ;
    double py = fitparams.par()(momindex+2) ;
    double pz = fitparams.par()(momindex+3) ;
    double mom2 = px*px+py*py+pz*pz ;
    double mom = std::sqrt(mom2) ;
    double tx  = px/pz ;
    double ty  = py/pz ;
    double qop = charge() / mom ;

    double dtxdpx = 1/pz ;
    double dtxdpz = -tx/pz ;
    double dtydpy = 1/pz ;
    double dtydpz = -ty/pz ;

    double dqopdpx = - qop * px/mom2 ;
    double dqopdpy = - qop * py/mom2 ;
    double dqopdpz = - qop * pz/mom2 ;

    // fill the residual
    double dz  = m_state.z() - motherpos.z() ;
    const Gaudi::Vector5& m_m = m_state.stateVector() ;
    const Gaudi::SymMatrix5x5& m_V = m_state.covariance() ;
    p.r(1) = m_m(0) - motherpos.x() - dz*tx ;
    p.r(2) = m_m(1) - motherpos.y() - dz*ty ;
    p.r(3) = m_m(2) - tx ;
    p.r(4) = m_m(3) - ty ;
    p.r(5) = m_m(4) - qop ;

    // compute the projection matrix
    // derivative of position to position of mother
    p.H(1,posindex+1) = -1 ;
    p.H(2,posindex+2) = -1 ;
    p.H(1,posindex+3) = tx ;
    p.H(2,posindex+3) = ty ;

    // derivative of position to 3-momentum
    p.H(1,momindex+1) = -dz * dtxdpx ; 
    p.H(1,momindex+3) = -dz * dtxdpz ;
    p.H(2,momindex+2) = -dz * dtydpy ; 
    p.H(2,momindex+3) = -dz * dtydpz ;

    // derivative of slopes/qop to 3-momentum
    p.H(3,momindex+1) = -dtxdpx ;
    p.H(3,momindex+3) = -dtxdpz ;
    p.H(4,momindex+2) = -dtydpy ;
    p.H(4,momindex+3) = -dtydpz ;
    p.H(5,momindex+1) = -dqopdpx ; 
    p.H(5,momindex+2) = -dqopdpy ; 
    p.H(5,momindex+3) = -dqopdpz ;
    
    // copy the covariance matrix
    for(int row=1; row<=5; ++row) 
      for(int col=1; col<=row; ++col)
	p.Vfast(row,col) = m_V(row-1,col-1) ;

    return status ;
  }
}
