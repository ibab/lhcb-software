#ifndef INTERNALPARTICLE_H
#define INTERNALPARTICLE_H

#include "ParticleBase.h"
#include <vector>

namespace DecayTreeFitter 
{

  class InternalParticle : public ParticleBase
  {
  public:
    InternalParticle(const LHCb::Particle& bc, const ParticleBase* mother, 
		     const Configuration& config) ;
    
    virtual int dim() const { return mother() ? 8 : 7 ; }
  
    virtual ErrCode initPar1(FitParams*) ; 
    virtual ErrCode initPar2(FitParams*) ; 
    virtual int type() const { return kInternalParticle ; }

    // parameter definition
    virtual int posIndex() const { return index()   ; }
    virtual int lenIndex() const { return mother() ? index()+3 : -1 ; }
    virtual int momIndex() const { return mother() ? index()+4 : index() + 3 ; }
    virtual bool hasEnergy() const { return true ; }
    virtual bool hasPosition() const { return true ; }
    virtual std::string parname(int index) const ;

    // constraints
    ErrCode projectKineConstraint(const FitParams&, Projection&) const ;
    ErrCode projectLifeTimeConstraint(const FitParams&, Projection&) const ;
    ErrCode projectConversionConstraint(const FitParams&,Projection& p) const ;
    ErrCode projectMassConstraintTwoBody(const FitParams& fitparams,Projection& p) const ;
    virtual ErrCode projectConstraint(Constraint::Type type,
				      const FitParams& fitparams,Projection& p) const ;

    // some of that other stuff
    virtual void addToConstraintList(constraintlist& alist, int depth) const ;

    //bool swapMotherDaughter(FitParams* fitparams, const ParticleBase* newmother) ;
    
  protected:
    ErrCode initMom( FitParams* fitparams ) const ;
  private:
    bool m_lifetimeconstraint ;
    bool m_isconversion ;
  } ;

}


#endif
