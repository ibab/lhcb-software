#ifndef RECORESONANCE_H
#define RECORESONANCE_H

#include "RecoComposite.h"

namespace DecayTreeFitter
{

  class RecoResonance : public RecoComposite
  {
  public:
    RecoResonance(const LHCb::Particle& bc, const ParticleBase* mother) ;
    virtual ~RecoResonance() ;

    virtual int dim() const { return hasEnergy() ? 4 : 3 ; } // (px,py,pz,(E))
   
    virtual ErrCode projectConstraint(Constraint::Type, const FitParams&, Projection&) const ;
    virtual ErrCode initPar1(FitParams*) ; 
    virtual ErrCode initPar2(FitParams*) ;
    virtual int type() const { return kRecoResonance ; }  
    
    virtual int posIndex() const { return mother()->posIndex()  ; }
    virtual int momIndex() const { return index() ; }
    virtual int lenIndex() const { return -1 ; }

    virtual std::string parname(int index) const ;

    virtual void addToConstraintList(constraintlist& alist, int depth) const {
      alist.push_back( Constraint(this,Constraint::btaresonance,depth,dimM()) ) ; }
    
  private:
  } ;

}

#endif
