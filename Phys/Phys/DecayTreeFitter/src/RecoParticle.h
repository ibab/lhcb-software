#ifndef RECOPARTICLE_HH
#define RECOPARTICLE_HH

#include "ParticleBase.h"

namespace DecayTreeFitter
{

  class RecoParticle : public ParticleBase
  {
  public:
    RecoParticle(const LHCb::Particle& bc, const ParticleBase* mother) ;
    virtual ~RecoParticle() ;

    virtual int dimM() const = 0; // dimension of the measurement    
    virtual ErrCode initPar1(FitParams*) { return ErrCode::success ; } 
    //virtual ErrCode initCov(FitParams*) const ;
    virtual std::string parname(int index) const ;
    virtual int dim() const { return 3; }   //(px,py,pz)
 
    virtual int momIndex() const { return index() ; }
    virtual bool hasEnergy() const { return false ; }

    virtual ErrCode projectRecoConstraint(const FitParams& fitparams, Projection& p) const = 0 ;
    virtual ErrCode projectConstraint(Constraint::Type, const FitParams&, Projection&) const ;
    virtual double chiSquare(const FitParams* fitparams) const ;

  } ;

}
#endif
