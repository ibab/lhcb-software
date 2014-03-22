#include <stdio.h>
#include "Event/Particle.h"
#include "Event/CaloPosition.h"

#include "CaloUtils/CaloMomentum.h"

#include "RecoMergedPi0.h"
#include "FitParams.h"

namespace DecayTreeFitter
{
  extern int vtxverbose ;

  RecoMergedPi0::RecoMergedPi0(const LHCb::Particle& bc, const ParticleBase* mother) 
    : ParticleBase(bc,mother), m_calomom(0)
  {
    if( bc.proto() ) m_calomom = new LHCb::CaloMomentum( bc.proto() ) ;
  }
  
  RecoMergedPi0::~RecoMergedPi0()
  {
    delete m_calomom ;
  }

  ErrCode
  RecoMergedPi0::initParPi0( FitParams* fitparams )
  {
    if(vtxverbose>=3)
      std::cout << "In RecoMergedPi0::initParPi0 " 
		<< dim() << " " << dimM() << " " << hasEnergy() << " posindex: "
		<< posIndex() << " momindex: " << momIndex() << std::endl ;
    ErrCode rc ;
    if( m_calomom ) {
      // assign the momentum
      Gaudi::LorentzVector p4 = m_calomom->momentum() ;
      int momindex = momIndex() ;
      fitparams->par(momindex+1) = p4.Px() ;
      fitparams->par(momindex+2) = p4.Py() ;
      fitparams->par(momindex+3) = p4.Pz() ;
      if( hasEnergy() ) fitparams->par(momindex+4) = p4.E() ;
    } else {
      rc = ErrCode::badsetup ;
    }
    return rc ;
  }
   
  ErrCode
  RecoMergedPi0::initPar1(FitParams* fitparams)
  {
    return initParPi0( fitparams ) ;
  }

  ErrCode
  RecoMergedPi0::initPar2(FitParams* fitparams)
  {
    // get the position of the mother
    int posindexmother = mother()->posIndex() ;
    Gaudi::XYZPoint motherpos(fitparams->par(posindexmother+1),
			      fitparams->par(posindexmother+2),
			      fitparams->par(posindexmother+3)) ;
    // update the calomom and initialize
    ErrCode rc ;
    if( m_calomom ) {
      m_calomom->setReferencePoint( motherpos ) ;
      rc = initParPi0( fitparams ) ;
    } else {
      rc = ErrCode::badsetup ;
    }
    return rc ;
  }

  ErrCode
  RecoMergedPi0::initCov(FitParams* fitparams) const 
  {  
    int momindex = momIndex() ;
    int thisdim = dim() ;
    const double factor = 1000;
    const LHCb::CaloMomentum::MomCovariance momcov = m_calomom->momCovMatrix() ;
    for(int row=0; row<thisdim; ++row) 
      fitparams->cov().fast(momindex+1+row,momindex+row+1) = factor * momcov(row,row) ;
    return ErrCode() ;
  }

  void constrainMass( Gaudi::LorentzVector& p4,
		      LHCb::CaloMomentum::MomCovariance& p4cov,
		      const double mass ) 
  {
    double px = p4.Px () ;
    double py = p4.Py () ;
    double pz = p4.Pz () ;
    double E  = p4.E() ;
    double m2 = (E*E - pz*pz) - px*px - py*py ;
    //double m  = std::sqrt(m2) ;
  
    // the trick is to rotate first everything from (p3,E) to
    // (p3,m2). we then filter the mass constrained. after that we
    // rotate back.
    
    // first transform to the (px,py,pz,m2) basis. since the jacobian is
    // rather empty, this could be optimized, another time.
    ROOT::Math::SMatrix<double,4,4> p4jacobian ; // = dQnew/dQold
    for ( size_t col=0; col<3; ++col ) { p4jacobian(col,col) = 1 ; }
    p4jacobian(3,0) = -2*px ;
    p4jacobian(3,1) = -2*py ;
    p4jacobian(3,2) = -2*pz ;
    p4jacobian(3,3) =  2*E  ;

    // we need only one row of this matrix, so this can be done a lot faster
    LHCb::CaloMomentum::MomCovariance p4primecov = ROOT::Math::Similarity(p4jacobian , p4cov ) ;
    // apply the constraint, which is trivial now
    const double residual    = mass*mass - m2;
    const double residualV   = p4primecov(3,3) ;
    px += residual * p4primecov(3,0)/residualV ;
    py += residual * p4primecov(3,1)/residualV ;
    pz += residual * p4primecov(3,2)/residualV ;
    // update the cov matrix, but only the part we'll actually need: we'll skip the hole energy part
    for(int row=0; row<=3; ++row)
      for(int col=0; col<=row; ++col)
	p4cov(row,col) -=  p4primecov(3,row)*p4primecov(3,col)/residualV ;
  }

  
  ErrCode
  RecoMergedPi0::projectPi0Constraint(const FitParams& fitparams, Projection& p) const
  {
    // residual of the merged pi0 is just its four-vector. the mass
    // constraint we omit because the cov matrix is anyway not very
    // reliable for merged pi0: if mass constraint is set, we'll just 
    // use the pdg mass for its mass.

    // We use calomom to evaluate the p4 and its covariance. This p4 is computed as
    
    // px = (x1 - x)/L1 * E1 + (x2-x)/L2 * E2
    
    // where L  = sqrt( (x1-x)**2 + (y1-y)**2 + (z1-z)**2)

    // etc.

    // So, to compute the derivatives to (x,y,z) of the mother, we
    // need "( E1/L1 + E2/L2)" for the clusters. To a very good
    // approximation L1==L2. We still need an estimate for that, but
    // there is certainly no reason to change it in between iterations.

    ErrCode status ;

    // calculate dX = Xc - Xmother
    const int posindex = mother()->posIndex() ;
    Gaudi::XYZPoint motherpos(fitparams.par(posindex+1),
			      fitparams.par(posindex+2),
			      fitparams.par(posindex+3)) ;    

    // update the CaloMomentum cache, if we have moved 'significantly'
    if( (motherpos - m_calomom->referencePoint() ).R() > 1*Gaudi::Units::cm )
      m_calomom->setReferencePoint(motherpos) ;
    
    // get the LorentzVector and cov matrix from calomom
    Gaudi::LorentzVector p4 = m_calomom->momentum() ;
    LHCb::CaloMomentum::MomCovariance p4cov = m_calomom->momCovMatrix() ;

    //if( hasMassConstraint() ) {
    //  constrainMass(p4,p4cov,pdtMass()) ;
    //}

    // fill the residual
    const int momindex = momIndex() ;
    p.r(1) = p4.Px() - fitparams.par(momindex+1) ;
    p.r(2) = p4.Py() - fitparams.par(momindex+2) ;
    p.r(3) = p4.Pz() - fitparams.par(momindex+3) ;
    if(hasEnergy()) 
      p.r(4) = p4.E() - fitparams.par(momindex+4) ;
    
    // derivative to origin. more complicated than I had hoped:-)
    for( LHCb::CaloMomentum::CaloPositions::const_iterator
	   ipos = m_calomom->caloPositions().begin() ;
	 ipos != m_calomom->caloPositions().end() ; ++ipos) {
      const LHCb::CaloPosition& calopos = *ipos ;
      double dx = calopos.x() - motherpos.x() ;
      double dy = calopos.y() - motherpos.y() ;
      double dz = calopos.z() - motherpos.z() ;
      double dx2 = dx*dx ;
      double dy2 = dy*dy ;
      double dz2 = dz*dz ;
      double L2 = dx2+dy2+dz2 ;
      double EOverL3 = calopos.e()/(L2*std::sqrt(L2)) ;
      p.H(1,posindex+1) += -(dy2+dz2) * EOverL3 ;
      p.H(2,posindex+1) +=  dy*dx     * EOverL3 ;
      p.H(3,posindex+1) +=  dz*dx     * EOverL3 ;
      p.H(1,posindex+2) +=  dx*dy     * EOverL3 ;
      p.H(2,posindex+2) += -(dx2+dz2) * EOverL3 ;
      p.H(3,posindex+2) +=  dz*dy     * EOverL3 ;
      p.H(1,posindex+3) +=  dx*dz     * EOverL3 ;
      p.H(2,posindex+3) +=  dy*dz     * EOverL3 ;
      p.H(3,posindex+3) += -(dx2+dy2) * EOverL3 ;
    }
    
    // derivative to momentum
    int maxrow = dimM() ;
    for(int irow=1; irow<=maxrow; ++irow)
      p.H(irow,momindex+irow) = -1 ; 

    // error
    for(int irow=1; irow<=maxrow; ++irow)
      for(int icol=1; icol<=irow; ++icol)
	p.Vfast(irow,icol) = p4cov(irow-1,icol-1) ;
    
    return status ;
  }

  double RecoMergedPi0::chiSquare(const FitParams* fitparams) const
  {
    // project
    Projection p(fitparams->dim(),dimM()) ;
    projectPi0Constraint(*fitparams,p) ;
    return p.chiSquare() ;
  }
  
}
