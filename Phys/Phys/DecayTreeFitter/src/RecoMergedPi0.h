#ifndef _VTK_RECOMERGEDPI0_HH_
#define _VTK_RECOMERGEDPI0_HH_

#include "ParticleBase.h"

namespace LHCb {
  class CaloMomentum ;
}

namespace DecayTreeFitter
{

  class RecoMergedPi0 : public ParticleBase
  {
  public:
    RecoMergedPi0(const LHCb::Particle& bc, const ParticleBase* mother) ;
    virtual ~RecoMergedPi0() ;
    
    // the number of parameters
    int dim() const { return hasMassConstraint() ? 3 : 4 ; }
    
    // the number of 'measurements'
    int dimM() const        { return dim(); }

    // does it have an energy component?
    bool hasEnergy() const { return !hasMassConstraint() ; }
    
    // project the constraint
    ErrCode projectPi0Constraint(const FitParams&, Projection&) const ;
    virtual ErrCode projectConstraint(Constraint::Type type, const FitParams& fitparams, Projection& p) const {
      ErrCode status ;
      switch(type) {
      case Constraint::btacomposite:
	status |= projectPi0Constraint(fitparams,p) ;
	break ;
      default:
	status |= ParticleBase::projectConstraint(type,fitparams,p) ;
      }
      return status ;
    }

    std::string parname(int index) const {
      return ParticleBase::parname(index+4) ;
    }
    
    ErrCode initParPi0(FitParams*) ;
    virtual ErrCode initPar1(FitParams*) ;
    virtual ErrCode initPar2(FitParams*) ;
    virtual ErrCode initCov(FitParams*) const ;
    virtual int type() const { return kRecoMergedPi0 ; }  
    double chiSquare(const FitParams* fitparams) const ;

    virtual int momIndex() const { return index() ; }
    
    virtual void addToConstraintList(constraintlist& alist, int depth) const {
      alist.push_back( Constraint(this,Constraint::btacomposite,depth,dimM()) ) ;
    }
    
  protected:
    LHCb::CaloMomentum* m_calomom ;
  } ;

}

#endif
