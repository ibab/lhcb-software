#include <iostream>
#include <iomanip>
#include "FitParams.h"

namespace DecayTreeFitter {

  FitParams::FitParams(int dim) 
    : m_dim(dim),m_par(dim,0),m_cov(dim,0),m_scale(dim,1),
      m_chiSquare(0),m_nConstraints(0),m_nConstraintsVec(dim,0) {}

  FitParams::~FitParams() {}

  void FitParams::resetPar() {
    for(int row=1; row<=m_dim; ++row) 
      m_par(row) = 0 ;
  }
  
  void FitParams::resetCov(double scale) {
    for(int row=1; row<=m_dim; ++row) {
      for(int col=1; col<row; ++col) 
	m_cov.fast(row,col) = 0 ;
      m_cov.fast(row,row) *= scale ;
      if(m_cov.fast(row,row) < 0 ) m_cov.fast(row,row)*=-1 ;
    }
    m_chiSquare=0 ;
    m_nConstraints=0 ;
    for(int row=1; row<=m_dim; ++row)
      nConstraintsVec(row) = 0 ;
    m_chiSquareMap.clear() ;
  }
  
  bool FitParams::testCov() const {
    bool okay=true ;
    for(int row=1; row<=m_dim && okay; ++row) 
      okay = m_cov.fast(row,row)>0 ;
    return okay ;
  }

  void FitParams::print() const {
    std::cout << std::setw(3) << "index" << std::setw(15) << "val" << std::setw(15) << "err" << std::endl ;
    std::cout << std::setprecision(5) ;
    for(int row=1; row<=m_dim; ++row) 
      std::cout << std::setw(3) << row-1
	   << std::setw(15) << m_par(row) 
	   << std::setw(15) << sqrt(m_cov(row,row)) << std::endl ;
  } ;
  
  HepSymMatrix FitParams::cov(const std::vector<int>& indexVec) const {
    int nrow = indexVec.size() ;
    HepSymMatrix thecov(nrow,0) ;
    for(int row=1; row<=nrow; ++row)
      for(int col=1; col<=row ; ++col)
	thecov(row,col) = m_cov(indexVec[row-1]+1,indexVec[col-1]+1) ;
    return thecov ;
  }

  HepVector FitParams::par(const std::vector<int>& indexVec) const {
    int nrow = indexVec.size() ;
    HepVector thepar(nrow,0) ;
    for(int row=1; row<=nrow; ++row)
      thepar(row) = m_par(indexVec[row-1]+1) ;
    return thepar ;
  }

  void FitParams::resize(int newdim)
  {
    if( newdim > m_dim ) {
      m_dim = newdim ;
      // very expensive, but okay ...
      HepVector newpar(newdim,0) ;
      newpar.sub(1,m_par);

      HepSymMatrix newcov(newdim,0) ;
      newcov.sub(1,m_cov) ;
      
      //      HepVector newpar(newdim,0) ;
      //       HepSymMatrix newcov(newdim,0) ;
      //       std::cout << newpar << std::endl ;
      //       for(int row=1; row<=m_dim ; ++row) {
      // 	newpar(row) = m_par(row) ;
      // 	for(int col=1; col<=row; ++col)
      // 	// 	  newcov(row,col) = m_cov(row,col) ;
      //       }
      //      std::cout << m_par << " " << newpar << std::endl ;

      m_par = newpar ;
      m_cov = newcov ;
      m_dim = newdim ;
      m_nConstraintsVec.resize(newdim,0) ;
    }
  }

  void FitParams::addChiSquare( double chisq, int nconstraints, const ParticleBase* p)
  {
    m_chiSquare += chisq;
    m_nConstraints += nconstraints ;
    if( p ) m_chiSquareMap[ p ] += ChiSquare(chisq,nconstraints) ;
  }

  ChiSquare FitParams::chiSquare( const ParticleBase& p) const 
  { 
    std::map<const ParticleBase*, ChiSquare>::const_iterator it = m_chiSquareMap.find( &p ) ;
    return it != m_chiSquareMap.end() ? it->second : ChiSquare() ;
  }

//   void FitParams::copy(const FitParams& rhs, 
// 		       const indexmap& anindexmap) 
//   {
//     for(indexmap::const_iterator it = anindexmap.begin() ; 
// 	it != anindexmap.end(); ++it) {
//       int idim =     it->first->dim() ;
//       int indexrhs = it->second ;
//       int indexlhs = it->first->index() ;
//       for(int i=1; i<=idim; ++i)
// 	m_par(indexlhs+i) = rhs.m_par(indexrhs+i) ;
//       for(indexmap::const_iterator it2 = it ; 
// 	  it2 != anindexmap.end(); ++it2) {
// 	int jdim     = it2->first->dim() ;
// 	int jndexrhs = it2->second ;
// 	int jndexlhs = it2->first->index() ;
// 	for(int i=1; i<=idim; ++i)
// 	  for(int j=1; j<=jdim ; ++j)
// 	    m_cov( indexlhs+i, jndexlhs+j) = rhs.m_cov( indexrhs+i, jndexrhs+j) ;
//       }
//     }
//   }
  
}
