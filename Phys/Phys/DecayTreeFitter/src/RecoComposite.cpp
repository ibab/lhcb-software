
#include "Event/Particle.h"
#include "RecoComposite.h"
#include "FitParams.h"

namespace DecayTreeFitter
{

  extern int vtxverbose ;

  RecoComposite::RecoComposite(const LHCb::Particle& bc, const ParticleBase* mother)
    : ParticleBase(bc,mother),m_m(),m_matrixV(),m_hasEnergy(true) 
  { 
    //bool massconstraint = bc && bc->constraint(RecoConstraint::Mass) ;
    //m_hasEnergy = !massconstraint ;
    updCache() ;
  }

  template<class Matrix>
  HepSymMatrix convertToHepSymMatrix(const Matrix& M)
  {
    size_t dim = M.kRows ;
    HepSymMatrix rc(dim) ;
    for(size_t irow=0; irow<dim; ++irow)
      for(size_t icol=0; icol<=irow; ++icol)
	rc.fast(irow+1,icol+1) = M(irow,icol) ;
    return rc ;
  }

  void RecoComposite::updCache()
  {
    // cache par7 (x,y,z,px,py,pz,E) cov7
    Gaudi::XYZPoint pos = particle().referencePoint() ;
    Gaudi::LorentzVector mom = particle().momentum() ;
    m_m = HepVector(dimM()) ;
    m_m(1) = pos.x() ;
    m_m(2) = pos.y() ;
    m_m(3) = pos.z() ;
    m_m(4) = mom.x() ;
    m_m(5) = mom.y() ;
    m_m(6) = mom.z() ;
    if(hasEnergy()) m_m(7) = mom.t() ;
    HepSymMatrix cov7 = convertToHepSymMatrix( particle().covMatrix()) ;
    m_matrixV = cov7.sub(1,dimM()) ; // so either 7 or 6, depending on mass constraint
    if(vtxverbose>=4) {
      std::cout << "cov matrix of external candidate: " << name().c_str() 
		<< " " << dimM() << " " << m_matrixV << std::endl ;
    }
  }

  RecoComposite::~RecoComposite() {}

  ErrCode
  RecoComposite::initPar1(FitParams* fitparams)
  {
    int posindex = posIndex() ;
    int momindex = momIndex() ;

    //quick map for parameters
    int indexmap[7]  ;
    for(int i=0; i<3; ++i) indexmap[i]   = posindex+i ; 
    for(int i=0; i<4; ++i) indexmap[i+3] = momindex+i ;
    // copy the 'measurement'
    for(int row=1; row<=dimM(); ++row) 
      fitparams->par()(indexmap[row-1]+1) = m_m(row) ;
    
    return ErrCode::success ;
  }

  ErrCode
  RecoComposite::initPar2(FitParams* fitparams)
  {
    // call default lifetime initialization
    return initTau(fitparams) ;
  }

  ErrCode
  RecoComposite::projectRecoComposite(const FitParams& fitparams, 
				    Projection& p) const
  {
    int posindex = posIndex() ;
    int momindex = momIndex() ;
    
    // quick map for parameters
    int indexmap[7]  ;
    for(int i=0; i<3; ++i) indexmap[i]   = posindex+i ; 
    for(int i=0; i<4; ++i) indexmap[i+3] = momindex+i ;
    for(int row=1; row<=dimM(); ++row) {
      p.r(row)                   = fitparams.par()(indexmap[row-1]+1) - m_m(row) ;
      p.H(row,indexmap[row-1]+1) = 1 ;
      for(int col=1; col<=row; ++col)
	p.Vfast(row,col) = m_matrixV.fast(row,col) ;
    }
 
    return ErrCode::success ;
  }
  
  ErrCode 
  RecoComposite::projectConstraint(Constraint::Type type, 
				  const FitParams& fitparams, 
				  Projection& p) const 
  {
    ErrCode status ;
    switch(type) {
    case Constraint::btacomposite:
      status |= projectRecoComposite(fitparams,p) ;
      break ;
    case Constraint::geometric:
      status |= projectGeoConstraint(fitparams,p) ;
      break ;
    default:
      status |= ParticleBase::projectConstraint(type,fitparams,p) ;
    }
    return status ;
  }

  double 
  RecoComposite::chiSquare(const FitParams* fitparams) const
  {
    Projection p(fitparams->dim(),dimM()) ;
    projectRecoComposite(*fitparams,p) ;
    return p.chiSquare() ;
  }

}
