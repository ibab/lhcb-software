#ifndef VTK_MISSINGPARTICLE_HH
#define VTK_MISSINGPARTICLE_HH

#include "ParticleBase.h"

namespace DecayTreeFitter
{

  class MissingParticle : public ParticleBase
  {
  public:
    MissingParticle(const LHCb::Particle& bc, const ParticleBase* mother) ;
    virtual ~MissingParticle() ;

    virtual ErrCode initPar1(FitParams*) ;
    virtual ErrCode initPar2(FitParams*) { return ErrCode::success ; }

    virtual std::string parname(int index) const ;
    virtual int dim() const  { return hasMassConstraint() ? 3 : 4 ; }
    virtual int momIndex() const { return index() ; }
    virtual bool hasEnergy() const { return hasMassConstraint() ? false : true ; }
    virtual int type() const { return kMissingParticle ; }
    virtual void addToConstraintList(constraintlist& /*alist*/, int /*depth*/) const {}
  } ;

}
#endif
