#ifndef RECOPHOTON_H
#define RECOPHOTON_H

#include "RecoParticle.h"

namespace DecayTreeFitter
{

  class RecoPhoton : public RecoParticle
  {
  public:
    
    RecoPhoton(const LHCb::Particle& bc, const ParticleBase* mother) ;
    virtual ~RecoPhoton() ;
    
    virtual int dimM() const { return 3 ; }
    virtual ErrCode initPar1(FitParams*) ;
    virtual ErrCode initPar2(FitParams*) ;

    virtual ErrCode initCov(FitParams*) const ;
    virtual int type()     const { return kRecoPhoton ; }
    virtual ErrCode projectRecoConstraint(const FitParams&,Projection&) const ;
    ErrCode updCache() ;
    
    virtual void addToConstraintList(constraintlist& alist, int depth) const {
      alist.push_back( Constraint(this,Constraint::photon,depth,dimM()) ) ; }
    
  private:
    virtual ErrCode initParPhoton(FitParams*, const Gaudi::XYZPoint& motherpos) const ;
    double m_z ;
    Gaudi::Vector3 m_m ;
    Gaudi::SymMatrix3x3 m_V ;
  } ;

}
#endif
