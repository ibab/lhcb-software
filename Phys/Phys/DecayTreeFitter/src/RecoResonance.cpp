#include "Event/Particle.h"
#include "DecayTreeFitter/VtkBtaResonance.h"
#include "DecayTreeFitter/VtkFitParams.h"

namespace vtxtreefit
{

  extern int vtxverbose ;

  BtaResonance::BtaResonance(const LHCb::Particle& bc, const ParticleBase* mother)
    : BtaComposite(bc,mother) {}
  

  BtaResonance::~BtaResonance() {}

  ErrCode
  BtaResonance::initPar1(FitParams* fitparams)
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
  BtaResonance::initPar2(FitParams* /*fitparams*/)
  {
    // nothing to do!
    return ErrCode::success ;
  }

  ErrCode 
  BtaResonance::projectConstraint(Constraint::Type type, 
				  const FitParams& fitparams, 
				  Projection& p) const 
  {
    ErrCode status ;
    switch(type) {
    case Constraint::btaresonance:
      status |= projectBtaComposite(fitparams,p) ;
      break ;
    default:
      status |= ParticleBase::projectConstraint(type,fitparams,p) ;
    }
    return status ;
  }

  std::string BtaResonance::parname(int index) const
  {
    return ParticleBase::parname(index+4) ;
  }


}
