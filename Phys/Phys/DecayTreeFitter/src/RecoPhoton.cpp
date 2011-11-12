#include <stdio.h>
#include "Event/Particle.h"
#include "Event/CaloHypo.h"
#include "Event/CaloPosition.h"

#include "RecoPhoton.h"
#include "FitParams.h"

namespace DecayTreeFitter
{
  extern int vtxverbose ;

  RecoPhoton::RecoPhoton(const LHCb::Particle& bc, const ParticleBase* mother) 
    : RecoParticle(bc,mother)
  {
    updCache() ;
  }
  
  RecoPhoton::~RecoPhoton() {}

  ErrCode RecoPhoton::initParPhoton(FitParams* fitparams,
				    const Gaudi::XYZPoint& motherpos) const
  {
    Gaudi::XYZVector dx( m_m(0) - motherpos.x(),
			 m_m(1) - motherpos.y(),
			 m_z    - motherpos.z()) ;
    double l = dx.r() ;

    // get the energy
    double energy = m_m(2) ;
    
    // assign the momentum
    int momindex = momIndex() ;
    fitparams->par(momindex+1) = energy*dx.x()/l ;
    fitparams->par(momindex+2) = energy*dx.y()/l ;
    fitparams->par(momindex+3) = energy*dx.z()/l ;
    
    return ErrCode() ;
  }
  
  ErrCode
  RecoPhoton::initPar1(FitParams* fitparams)
  {
    return initParPhoton( fitparams, Gaudi::XYZPoint(0,0,0) ) ;
  }

  ErrCode
  RecoPhoton::initPar2(FitParams* fitparams)
  {
    // calculate the direction
    int posindexmother = mother()->posIndex() ;
    Gaudi::XYZPoint motherpos(fitparams->par(posindexmother+1),
			      fitparams->par(posindexmother+2),
			      fitparams->par(posindexmother+3)) ;
    return initParPhoton(fitparams, motherpos ) ;
  }

  ErrCode
  RecoPhoton::initCov(FitParams* fitparams) const 
  {  
    int momindex = momIndex() ;
    double varEnergy =  m_V(2,2) ;
    const double factor = 1000;
    for(int row=1; row<=3; ++row) 
      fitparams->cov().fast(momindex+row,momindex+row) = factor * varEnergy ;
    return ErrCode() ;
  }
  
  ErrCode
  RecoPhoton::updCache()
  {
    const LHCb::CaloHypo* hypo = particle().proto()->calo()[0] ;
    // this works for photons
    const LHCb::CaloPosition* pos = hypo->position() ;
    // and this seems to work for merged pi0 ...
    if(!pos && hypo->clusters().size()>0)
      pos = &(hypo->clusters()[0]->position()) ;
    ErrCode rc ;
    if(pos) {
      m_m = pos->parameters() ;
      m_V = pos->covariance() ;
      m_z = pos->z() ;
    } else {
      std::cout << "DecayTreeFitter::RecoPhoton: cannot find position info for cluster" << std::endl ;
      rc = ErrCode::badsetup ;
    }
    return ErrCode() ;
  }

  ErrCode
  RecoPhoton::projectRecoConstraint(const FitParams& fitparams, Projection& p) const
  { 
    // residual of photon:
    // r(1-3) = motherpos + mu * photon momentum - cluster position
    // r(4)   = |momentum| - cluster energy
    // mu is calculated from the 'chi2-closest approach' (see below)

    ErrCode status ;

    // calculate the total momentum and energy:
    int momindex  = momIndex() ;
    double px = fitparams.par()(momindex+1) ;
    double py = fitparams.par()(momindex+2) ;
    double pz = fitparams.par()(momindex+3) ;
    double m  = pdtMass() ; // could be non-zero for mergedpi0

    double energy = std::sqrt(px*px+py*py+pz*pz+m*m) ;
    double dedpx  = px/energy ;
    double dedpy  = py/energy ;
    double dedpz  = pz/energy ;
    
    double tx = px/pz ;
    double ty = py/pz ;
    double dtxdpx = 1/pz ;
    double dtxdpz = -tx/pz ;
    double dtydpy = 1/pz ;
    double dtydpz = -ty/pz ;
    
    // calculate dX = Xc - Xmother
    int posindex  = mother()->posIndex() ;

    double dz = m_z    - fitparams.par(posindex+3) ;
    double dx = m_m(0) - fitparams.par(posindex+1) - dz * tx ;
    double dy = m_m(1) - fitparams.par(posindex+2) - dz * ty ;
    double dE = m_m(2) - energy ;

    p.r(1) = dx ;
    p.r(2) = dy ;
    p.r(3) = dE ;

    // calculate the projection matrix
    // first the 'position' part

    // derivative of position to position of mother
    p.H(1,posindex+1) = -1 ;
    p.H(2,posindex+2) = -1 ;

    // derivative to position to 3-momentum
    p.H(1,momindex+1) = -dz * dtxdpx ; 
    p.H(1,momindex+3) = -dz * dtxdpz ;
    p.H(2,momindex+2) = -dz * dtydpy ; 
    p.H(2,momindex+3) = -dz * dtydpz ;

    // derivative of momentum to 3-momentum
    p.H(3,momindex+1) = -dedpx ; 
    p.H(3,momindex+2) = -dedpy ; 
    p.H(3,momindex+3) = -dedpz ; 
    
    // error
    for(int irow=1; irow<=3; ++irow)
      for(int icol=1; icol<=irow; ++icol)
	p.Vfast(irow,icol) = m_V(irow-1,icol-1) ;
    
    return status ;
  }
}
