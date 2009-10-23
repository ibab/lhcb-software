// $Id: HistoParam.cpp,v 1.1 2009-10-23 09:43:15 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STD
// ============================================================================
#include <sstream>
// ============================================================================
// ROOT 
// ============================================================================
#include "TH1D.h"
#include "TH2D.h"
// ============================================================================
// HistoStrings 
// ============================================================================
#include "Kernel/Histo2String.h"
// ============================================================================
// DetDesc 
// ============================================================================
#include "DetDesc/HistoParam.h"
// ============================================================================
/** @file 
 *  Implementation file for the template specialization of Param 
 *  methods for the histograms 
 *  @see class Param 
 *  @see class TH1D 
 *  @see class TH2D 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-10-22
 */
// ============================================================================
/*   The declaration of the template 
 *   specialization of Param methods for the histograms 
 *   @see class Param 
 *   @see class TH1D  
 *   @param  name      (INPUT) XML-tag name 
 *   @param  comment   (INPUT) the comment 
 *   @param  precision (INPUT) not used 
 *   @return XML-string
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date 2009-10-22
 */
// ============================================================================
template<> 
std::string Param<TH1D>::toXMLStr 
( const std::string& name    , 
  const std::string& comment , 
  int  /* precision */ ) const 
{
  std::ostringstream out ;
  //
  out << "<histo1D name=\"" << name << '"' ;                       // open TAG 
  if ( !comment.empty() ) { out << " comment=\"" << comment << '"' ; }  
  out << '>' ;
  // stream the histogram 
  Gaudi::Utils::toStream ( m_val , out ) ;
  out << "</histo1D>" ;                                           // close TAG 
  return out.str() ;
}
// ============================================================================
/*   The declaration of the template 
 *   specialization of Param methods for the histograms 
 *   @see class Param 
 *   @see class TH1D  
 *   @return string representation of the histogram 
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date 2009-10-22
 */
// ============================================================================
template<> std::string Param<TH1D>::toStr() const 
{ return Gaudi::Utils::toString ( m_val ) ; }    
// ============================================================================
/*   The declaration of the template 
 *   specialization of Param methods for the histograms 
 *   @see class Param 
 *   @see class TH1D  
 *   @param  name      (INPUT) XML-tag name 
 *   @param  comment   (INPUT) the comment 
 *   @param  precision (INPUT) not used 
 *   @return XML-string
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date 2009-10-22
 */
// ============================================================================
template<> 
std::string Param<TH2D>::toXMLStr 
( const std::string& name    , 
  const std::string& comment , 
  int /* precision */ ) const 
{
  std::ostringstream out ;
  //
  out << "<histo2D name=\"" << name << '"' ;                       // open TAG 
  if ( !comment.empty() ) { out << " comment=\"" << comment << '"' ; }  
  out << '>' ;                                                   
  // stream the histogram 
  Gaudi::Utils::toStream ( m_val , out ) ;
  out << "</histo2D>" ;                                           // close TAG 
  return out.str() ;
}
// ============================================================================
/*   The declaration of the template 
 *   specialization of Param methods for the histograms 
 *   @see class Param 
 *   @see class TH2D  
 *   @return string representation of the histogram 
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date 2009-10-22
 */
// ============================================================================
template<> std::string Param<TH2D>::toStr() const 
{ return Gaudi::Utils::toString ( m_val ) ; }    
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
