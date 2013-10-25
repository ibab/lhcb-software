#include "Event/Particle.h"
#include "JetMomentum.h"
#include "FitParams.h"

namespace DecayTreeFitter
{

  extern int vtxverbose ;

  JetMomentum::JetMomentum(const LHCb::Particle& bc, const ParticleBase* mother)
    : ParticleBase(bc,mother)
  { 
    //bool massconstraint = bc && bc->constraint(RecoConstraint::Mass) ;
    //m_hasEnergy = !massconstraint ;
    updCache() ;
  }

  void JetMomentum::updCache()
  {
    // cache par7 (x,y,z,px,py,pz,E) cov7
    Gaudi::LorentzVector mom = particle().momentum() ;
    m_m(0) = mom.Px() ;
    m_m(1) = mom.Py() ;
    m_m(2) = mom.Pz() ;
    m_m(3) = mom.E() ;
    m_V    = particle().momCovMatrix() ;
    if(vtxverbose>=4) {
      std::cout << "cov matrix of external candidate: " << name().c_str() 
		<< " " << dimM() << " " << m_V << std::endl ;
    }
  }
  
  JetMomentum::~JetMomentum() {}

  ErrCode
  JetMomentum::initPar1(FitParams* fitparams)
  {
    int momindex = momIndex() ;
    // copy the 'measurement'
    for(int row=1; row<=4; ++row) 
      fitparams->par()(momindex + row) = m_m(row-1) ;
    
    return ErrCode::success ;
  }

  ErrCode
  JetMomentum::projectJetMomentum(const FitParams& fitparams, 
				  Projection& p) const
  {
    int momindex = momIndex() ;
    
    // quick map for parameters
    for(int row=1; row<=4; ++row) {
      p.r(row)  = fitparams.par()(momindex + row) - m_m(row-1) ;
      p.H(row,momindex+row) = 1 ;
      for(int col=1; col<=row; ++col)
	p.Vfast(row,col) = m_V(row-1,col-1) ;
    }
    
    return ErrCode::success ;
  }
  
  ErrCode 
  JetMomentum::projectConstraint(Constraint::Type type, 
				  const FitParams& fitparams, 
				  Projection& p) const 
  {
    ErrCode status ;
    switch(type) {
    case Constraint::externalmomentum:
      status |= projectJetMomentum(fitparams,p) ;
      break ;
    default:
      status |= ParticleBase::projectConstraint(type,fitparams,p) ;
    }
    return status ;
  }

  double 
  JetMomentum::chiSquare(const FitParams* fitparams) const
  {
    Projection p(fitparams->dim(),dimM()) ;
    projectJetMomentum(*fitparams,p) ;
    return p.chiSquare() ;
  }

}
