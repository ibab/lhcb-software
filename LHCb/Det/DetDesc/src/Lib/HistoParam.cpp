// $Id: HistoParam.cpp,v 1.2 2009-12-05 14:25:47 dgolubko Exp $
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
std::string Param<DetDesc::Params::Histo1D>::toXMLStr 
( const std::string& name    , 
  const std::string& comment , 
  int  /* precision */ ) const 
{
  std::ostringstream out ;
  //
  out << "<param name=\"" << name << "\" type=\"Histo1D\"" ;       // open TAG 
  if ( !comment.empty() ) { out << " comment=\"" << comment << '"' ; }  
  out << '>' ;
  // stream the histogram 
  Gaudi::Utils::toStream ( m_val , out ) ;
  out << "</param>" ;                                           // close TAG 
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
template<> std::string Param<DetDesc::Params::Histo1D>::toStr() const 
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
std::string Param<DetDesc::Params::Histo2D>::toXMLStr 
( const std::string& name    , 
  const std::string& comment , 
  int /* precision */ ) const 
{
  std::ostringstream out ;
  //
  out << "<param name=\"" << name << "\" type=\"Histo2D\"" ;       // open TAG 
  if ( !comment.empty() ) { out << " comment=\"" << comment << '"' ; }  
  out << '>' ;                                                   
  // stream the histogram 
  Gaudi::Utils::toStream ( m_val , out ) ;
  out << "</param>" ;                                           // close TAG 
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
template<> std::string Param<DetDesc::Params::Histo2D>::toStr() const 
{ return Gaudi::Utils::toString ( m_val ) ; }    
// ============================================================================
// The END 
// ============================================================================
