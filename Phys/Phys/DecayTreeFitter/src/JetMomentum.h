#ifndef _VTK_JETMOMENTUM_HH_
#define _VTK_JETMOMENTUM_HH_

#include "ParticleBase.h"

namespace DecayTreeFitter
{
  class JetMomentum : public ParticleBase
  {
  public:
    JetMomentum(const LHCb::Particle& bc, const ParticleBase* mother) ;
    virtual ~JetMomentum() ;

    // the number of parameters
    virtual int dim() const { return 4 ; } // px,py,pz,E)

    // the number of 'measurements'
    int dimM() const        { return 4 ; }
    ErrCode projectJetMomentum(const FitParams&, Projection&) const ;
    virtual ErrCode projectConstraint(Constraint::Type, const FitParams&, Projection&) const ;
 
    virtual ErrCode initPar1(FitParams*) ;
    virtual ErrCode initPar2(FitParams*) { return ErrCode::success ; }
    virtual int type() const { return kJetMomentum ; }  
    
    virtual int momIndex() const { return index() ; }
    virtual bool hasEnergy() const { return true ; }

    virtual void updCache() ;
    virtual double chiSquare(const FitParams* fitparams) const ; 
    
    std::string parname(int index) const { return ParticleBase::parname(index+4) ; }
    
    virtual void addToConstraintList(constraintlist& alist, int depth) const {
      alist.push_back( Constraint(this,Constraint::externalmomentum,depth,dimM()) ) ;
    }
    
  protected: // I hate this, so we need to change the design ...
    // cache
    Gaudi::Vector4 m_m ;    // 'measurement' (px,py,pz,E)
    Gaudi::SymMatrix4x4 m_V ; // covariance in measurement
  } ;

}

#endif
