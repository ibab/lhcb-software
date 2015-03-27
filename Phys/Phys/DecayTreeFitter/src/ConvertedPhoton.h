#ifndef __VTK_CONVERTEDPHOTON_HH__
#define __VTK_CONVERTEDPHOTON_HH__

#include "InternalParticle.h"

namespace DecayTreeFitter 
{
  class ConvertedPhoton : public InternalParticle
  {
  public:
    ConvertedPhoton(const LHCb::Particle& bc, const ParticleBase* mother, 
		    const Configuration& config) ;
    virtual ~ConvertedPhoton() {} 
    
    virtual int type() const { return kConvertedPhoton ; }
    
    ErrCode projectConversionPositionConstraint(const FitParams&,
						Projection&) const ;
    ErrCode projectConversionMassConstraint(const FitParams& fitparams,
					    Projection& p) const ;
    ErrCode projectConstraint(Constraint::Type type,
			      const FitParams& fitparams,
			      Projection& p) const ;
    void addToConstraintList(constraintlist& alist,
			     int depth) const ;

    ErrCode initCov( FitParams* fitpars) const ;
  private:
    void updateCache(const FitParams& pars) ;
  private:
    double m_conversionZ ;
    double m_conversionZCov ;
  } ;

}


#endif
