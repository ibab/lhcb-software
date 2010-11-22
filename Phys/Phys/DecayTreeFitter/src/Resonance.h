#ifndef __VTK_INTERNALRESONANCE_HH__
#define __VTK_INTERNALRESONANCE_HH__

#include "InternalParticle.h"

namespace DecayTreeFitter 
{
  class FitParams ;

  class Resonance : public InternalParticle
  {
  public:
    Resonance(const LHCb::Particle& bc, const ParticleBase* mother, 
	      const Configuration& config) ;
    virtual ~Resonance() ;

    virtual int dim() const { return 4 ; }
    virtual int type() const { return kResonance ; }
    virtual std::string parname(int index) const ;

    virtual ErrCode initPar1(FitParams*) ;
    virtual ErrCode initPar2(FitParams*) ;

    virtual int posIndex() const { return mother()->posIndex()   ; }
    virtual int momIndex() const { return index() ; }
    virtual int lenIndex() const { return -1 ; }
    virtual bool hasPosition() const { return false ; }

  private:
  } ;

}


#endif
