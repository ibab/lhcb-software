#include "DecayTreeFitter/VtkFitParams.h"
#include "DecayTreeFitter/VtkInteractionPoint.h"
#include "Event/Particle.h"

using std::cout;
using std::endl;

#ifdef VTK_BOUNDSCHECKING
#define FAST() 
#else
#define FAST() .fast
#endif

namespace vtxtreefit
{
//   extern int vtxverbose ;

//   InteractionPoint::InteractionPoint(const LHCb::Particle& bc, bool forceFitAll)
//     : InternalParticle(bc,0,forceFitAll),
//       m_constrainXY(false), m_constrainXYZ(false), m_ipPos(3,0), m_ipCov(3,1), m_ipCovInv(3,1) 
//   {
//     //assert(bc->pdtEntry()->lundId()%1000 == PdtLund::Upsilon ) ;
//     initBeamSpot(bc) ;
//   }

//   InteractionPoint::InteractionPoint(const LHCb::Particle& bc, bool forceFitAll, bool /*addupsilon*/) 
//     : InternalParticle(0,0,forceFitAll),
//       m_constrainXY(false), m_constrainXYZ(false), m_ipPos(3,0), m_ipCov(3,1), m_ipCovInv(3,1) 
//   {
//     addDaughter(bc,forceFitAll) ;
//     initBeamSpot(bc) ;
//   }

//   ErrCode
//   InteractionPoint::initBeamSpot(const LHCb::Particle& /*bc*/)
//   {
//     std::cout << "VtkInteractionPoint: still using dummy beamspot"<< std::endl ;
//     ErrCode status ;
//     m_constrainXY = m_constrainXYZ = true ;
//     m_ipCov(1,1) = 0.1 ;
//     m_ipCov(2,2) = 0.1 ;
//     m_ipCov(3,3) = 50 ;
//     int ierr ;
//     m_ipCovInv = m_ipCov.inverse(ierr) ;
    
//     if(vtxverbose>=2)
//       cout << "VtkInteractionPoint: initial beam spot = (" 
// 	   <<m_ipPos(1) << "," << m_ipPos(2) << "," << m_ipPos(3) << ")" << endl ;
    
//     return status ;
//   }

//   InteractionPoint::~InteractionPoint() {}

//   ErrCode 
//   InteractionPoint::initPar1(FitParams* fitparams)
//   {
//     ErrCode status ;
//     int posindex = posIndex() ;
//     for(int row=1; row<=3; ++row)
//       fitparams->par()(posindex+row) = m_ipPos(row) ;

//     for(daucontainer::const_iterator it = daughters().begin() ;
// 	it != daughters().end() ; ++it) {
//       status |= (*it)->initPar1(fitparams) ;
//       status |= (*it)->initPar2(fitparams) ;
//     }

//     return status ;
//   }

//   ErrCode 
//   InteractionPoint::initPar2(FitParams* /*fitparams*/)
//   {
//     // nothing left to do: actually, should never be called
//     assert(0) ;
//     return ErrCode::success ;
//   }

//   ErrCode
//   InteractionPoint::initCov(FitParams* fitpar) const 
//   {
//     ErrCode status ;
//     int posindex = posIndex() ;
//     for(int row=1; row<=3; ++row)
//       fitpar->cov() FAST() (posindex+row,posindex+row) 
// 	= 1000*m_ipCov FAST() (row,row) ;
    
//     for(daucontainer::const_iterator it = daughters().begin() ;
// 	it != daughters().end() ; ++it)
//       status |= (*it)->initCov(fitpar) ;

//     return status ;
//   }

//   ErrCode
//   InteractionPoint::projectIPConstraint(const FitParams& fitparams, 
// 					Projection& p) const
//   { 
//     int posindex = posIndex() ;
//     int maxrow = m_constrainXYZ ? 3 : (m_constrainXY ? 2 : 0 ) ;
//     for(int row=1; row<=maxrow; ++row) {
//       p.r(row) = fitparams.par()(posindex+row) - m_ipPos(row) ;
//       p.H(row,posindex+row) = 1 ;
//       for(int col=1; col<=row; ++col)
// 	p.Vfast(row,col) = m_ipCov FAST() (row,col) ;
//     }
//     return ErrCode::success ;
//   }
  
//   ErrCode 
//   InteractionPoint::projectConstraint(Constraint::Type type, 
// 				      const FitParams& fitparams, 
// 				      Projection& p) const 
//   {
//     ErrCode status ;
//     switch(type) {
//     case Constraint::beamspot:
//       status |= projectIPConstraint(fitparams,p) ;
//       break ;
//     default:
//       status |= InternalParticle::projectConstraint(type,fitparams,p) ;
//     }
//     return status ;
//   }
  
//   double 
//   InteractionPoint::chiSquare(const FitParams* fitparams) const
//   {
//     // calculate the chi2
//     int posindex = posIndex() ;
//     HepVector residual = m_ipPos - fitparams->par().sub(posindex+1,posindex+3) ;
//     double chisq = m_ipCovInv.similarity(residual) ;

//     // add the daughters
//     chisq += InternalParticle::chiSquare(fitparams) ;

//     return chisq ;
//   }

//   void InteractionPoint::addToConstraintList(constraintlist& alist, int depth) const
//   {
//     // first the beamspot
//     int dim = m_constrainXYZ ? 3 : (m_constrainXY ? 2 : 0 ) ;
//     alist.push_back(Constraint(this,Constraint::beamspot,depth,dim)) ;

//     // then the base class
//     InternalParticle::addToConstraintList(alist,depth) ;
//   }

}


