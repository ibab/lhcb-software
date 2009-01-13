#include <sstream>
#include <set>
#include "AlResiduals.h"
#include "AlignmentElement.h"
#include "Event/Node.h"

namespace Al
{
  
  Residual1D::Residual1D(const LHCb::Node& n, const AlignmentElement& e) :
    m_node(&n), m_element(&e),
    m_r(n.residual()), m_V(n.errMeasure2()), m_HCH(n.errMeasure2()-n.errResidual2()) {}

  Residuals::Residuals(double chi2, size_t ndof, size_t numexthits) 
    : m_chi2(chi2), m_nDoF(ndof), m_nExternalHits(numexthits) {}

  size_t Residuals::nAlignables() const {
    std::set<size_t> alignables ;
    for( ResidualContainer::const_iterator inode = m_residuals.begin() ;
	 inode != m_residuals.end(); ++inode )
      alignables.insert( inode->element().index() ) ;
    return alignables.size() ;
  }

  bool Residuals::testHCH(std::string& message) const
  {

   // Let's check the result. Keep track of the roots.
    std::stringstream warningmsg ;
    const double tol = 1e-3 ;
    std::vector<double> HCHdiagroots ;
    std::vector<double> Rdiagroots ;
    bool error(false) ;
    for(size_t irow=0; irow<size() && !error; ++irow) {
      double HCH = m_residuals[irow].HCH() ;
      double V   = m_residuals[irow].V() ;
      error = ! (0<=HCH) ;
      if(error) warningmsg << "found negative element on diagonal: " << HCH << std::endl ;
      error = ! ( HCH<V ) ;
      if(error) {
	warningmsg << "found too large element on diagonal: " << HCH/V << std::endl ;
      }
      HCHdiagroots.push_back( std::sqrt(HCH) ) ;
      Rdiagroots.push_back( std::sqrt(V-HCH)) ;
    }
    bool offdiagerror(false) ;
    for( Al::Residuals::CovarianceContainer::const_iterator it = m_HCHElements.begin() ; 
	 it != m_HCHElements.end() && !error; ++it) {
      size_t irow = it->row ;
      size_t icol = it->col ;
      double c = it->val / (HCHdiagroots[irow]*HCHdiagroots[icol]) ;
      bool thiserror =  ! (-1-tol < c && c <1+tol) ;
      double d = it->val / (Rdiagroots[irow]*Rdiagroots[icol]) ;
      bool thisderror =  ! (-1-tol < d && d <1+tol) ;
      if(thiserror||thisderror) {
	warningmsg << "found bad element on offdiagonal: " 
		  << irow << " " << icol << " " 
		  << c << " " << d << std::endl ;
      }
      offdiagerror |= thiserror || thisderror ;
    }
    error = error || offdiagerror ;
    if(error) message = warningmsg.str() ;
    return error ;
  }


}
