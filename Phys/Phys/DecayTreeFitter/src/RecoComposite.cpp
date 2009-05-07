#include "Event/Particle.h"
#include "DecayTreeFitter/VtkBtaComposite.h"
#include "DecayTreeFitter/VtkFitParams.h"
using std::cout;
using std::endl;

#ifdef VTK_BOUNDSCHECKING
#define FAST() 
#else
#define FAST() .fast
#endif

namespace vtxtreefit
{

  extern int vtxverbose ;
  BtaComposite::BtaComposite(const LHCb::Particle& bc, const ParticleBase* mother)
    : ParticleBase(bc,mother),m_m(),m_matrixV(),m_hasEnergy(true) 
  { 
    //bool massconstraint = bc && bc->constraint(BtaConstraint::Mass) ;
    //m_hasEnergy = !massconstraint ;
    updCache() ;
  }

  template<class Matrix>
  HepSymMatrix convertToHepSymMatrix(Matrix M)
  {
    size_t dim = M.kRows ;
    HepSymMatrix rc(dim) ;
    for(size_t irow=0; irow<dim; ++irow)
      for(size_t icol=0; icol<=irow; ++icol)
	rc.fast(icol,irow) = M(icol,irow) ;
    return rc ;
  }

  void BtaComposite::updCache()
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
      cout << "cov matrix of external candidate: " << name().c_str() 
	   << " " << dimM() << " " << m_matrixV << endl ;
    }
  }

  BtaComposite::~BtaComposite() {}

  ErrCode
  BtaComposite::initPar1(FitParams* fitparams)
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
  BtaComposite::initPar2(FitParams* fitparams)
  {
    // call default lifetime initialization
    return initTau(fitparams) ;
  }

  ErrCode
  BtaComposite::projectBtaComposite(const FitParams& fitparams, 
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
	p.Vfast(row,col) = m_matrixV FAST() (row,col) ;
    }
 
    return ErrCode::success ;
  }
  
  ErrCode 
  BtaComposite::projectConstraint(Constraint::Type type, 
				  const FitParams& fitparams, 
				  Projection& p) const 
  {
    ErrCode status ;
    switch(type) {
    case Constraint::btacomposite:
      status |= projectBtaComposite(fitparams,p) ;
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
  BtaComposite::chiSquare(const FitParams* fitparams) const
  {
    Projection p(fitparams->dim(),dimM()) ;
    projectBtaComposite(*fitparams,p) ;
    return p.chiSquare() ;
  }

}
