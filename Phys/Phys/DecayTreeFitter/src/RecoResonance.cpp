#include "RecoResonance.h"
#include "Event/Particle.h"
#include "FitParams.h"

namespace DecayTreeFitter
{

  extern int vtxverbose ;

  RecoResonance::RecoResonance(const LHCb::Particle& bc, const ParticleBase* mother)
    : RecoComposite(bc,mother) {}
  

  RecoResonance::~RecoResonance() {}

  ErrCode
  RecoResonance::initPar1(FitParams* fitparams)
  {
    int posindex = posIndex() ;
    int momindex = momIndex() ;
    
    //quick map for parameters
    int indexmap[7]  ;
    for(int i=0; i<3; ++i) indexmap[i]   = posindex+i ; 
    for(int i=0; i<4; ++i) indexmap[i+3] = momindex+i ;

    // copy the 'measurement' -> this overwrites mother position !
    for(int row=1; row<=dimM(); ++row) 
      fitparams->par()(indexmap[row-1]+1) = m_m(row) ;
    return ErrCode::success ;
  }

  ErrCode
  RecoResonance::initPar2(FitParams* /*fitparams*/)
  {
    // nothing to do!
    return ErrCode::success ;
  }

  ErrCode 
  RecoResonance::projectConstraint(Constraint::Type type, 
				  const FitParams& fitparams, 
				  Projection& p) const 
  {
    ErrCode status ;
    switch(type) {
    case Constraint::btaresonance:
      status |= projectRecoComposite(fitparams,p) ;
      break ;
    default:
      status |= ParticleBase::projectConstraint(type,fitparams,p) ;
    }
    return status ;
  }

  std::string RecoResonance::parname(int index) const
  {
    return ParticleBase::parname(index+4) ;
  }


}
