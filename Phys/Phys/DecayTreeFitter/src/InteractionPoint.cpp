#include "FitParams.h"
#include "InteractionPoint.h"
#include "Event/Particle.h"
#include "Event/VertexBase.h"

namespace DecayTreeFitter
{
  extern int vtxverbose ;

  InteractionPoint::InteractionPoint(const LHCb::VertexBase& ipvertex,
				     const LHCb::Particle& daughter, 
				     const Configuration& config)
    : ParticleBase("IP")
  {
    addDaughter( daughter, config ) ;
    m_ipPos(0) = ipvertex.position().x() ;
    m_ipPos(1) = ipvertex.position().y() ;
    m_ipPos(2) = ipvertex.position().z() ;
    m_ipCov    = ipvertex.covMatrix() ;
    int ierr ;
    m_ipCovInv = m_ipCov.Inverse(ierr) ;
    if(vtxverbose>=2) {
      std::cout << "InteractionPoint: initial beam spot = (" 
		<<m_ipPos(0) << "," << m_ipPos(1) << "," << m_ipPos(2) << ")" <<std::endl ;
      std::cout << "daughter: " << daughters().front()->name() << std::endl ;
    }
  }

  ErrCode 
  InteractionPoint::initPar1(FitParams* fitparams)
  {
    ErrCode status ;
    int posindex = posIndex() ;
    for(int row=1; row<=3; ++row)
      fitparams->par()(posindex+row) = m_ipPos(row-1) ;
    for(daucontainer::iterator idau = daughters().begin() ;
	idau != daughters().end(); ++idau ) {
      status |= (*idau)->initPar1(fitparams) ;
      status |= (*idau)->initPar2(fitparams) ;
    }
    return status ;
  }

  ErrCode 
  InteractionPoint::initPar2(FitParams* /*fitparams*/)
  {
    // nothing left to do: actually, should never be called
    assert(0) ;
    return ErrCode::success ;
  }

  ErrCode
  InteractionPoint::initCov(FitParams* fitpar) const 
  {
    ErrCode status ;
    int posindex = posIndex() ;
    for(int row=1; row<=3; ++row)
      fitpar->cov().fast(posindex+row,posindex+row) = 1000*m_ipCov(row-1,row-1) ;

    for(daucontainer::const_iterator it = daughters().begin() ;
	it != daughters().end() ; ++it)
      status |= (*it)->initCov(fitpar) ;
    return status ;
  }
  
  ErrCode
  InteractionPoint::projectIPConstraint(const FitParams& fitparams, 
					Projection& p) const
  { 
    int posindex = posIndex() ;
    int maxrow = 3 ;
    for(int row=1; row<=maxrow; ++row) {
      p.r(row) = fitparams.par()(posindex+row) - m_ipPos(row-1) ;
      p.H(row,posindex+row) = 1 ;
      for(int col=1; col<=row; ++col)
	p.Vfast(row,col) = m_ipCov(row-1,col-1) ;
    }
    return ErrCode::success ;
  }

  ErrCode 
  InteractionPoint::projectConstraint(Constraint::Type type, 
				      const FitParams& fitparams, 
				      Projection& p) const 
  {
    ErrCode status ;
    switch(type) {
    case Constraint::beamspot:
      status |= projectIPConstraint(fitparams,p) ;
      break ;
    default:
      status |= ParticleBase::projectConstraint(type,fitparams,p) ;
    }
    return status ;
  }

  double 
  InteractionPoint::chiSquare(const FitParams* fitparams) const
  {
    // calculate the chi2
    int posindex = posIndex() ;
    Gaudi::Vector3 residual = m_ipPos ;
    for(int row=0; row<3; ++row)
      residual(row) -= fitparams->par()(posindex+row+1) ;
    double chisq = ROOT::Math::Similarity(residual,m_ipCovInv) ;
    // add the daughters
    chisq += ParticleBase::chiSquare(fitparams) ;

    return chisq ;
  }

  void InteractionPoint::addToConstraintList(constraintlist& alist, int depth) const
  {
    // first the daughters
    for(daucontainer::const_iterator it = daughters().begin() ;
	it != daughters().end() ; ++it)
      (*it)->addToConstraintList(alist,depth-1) ;

    // then the beamspot
    int dim = 3 ;
    alist.push_back(Constraint(this,Constraint::beamspot,depth,dim)) ;
  }

}


