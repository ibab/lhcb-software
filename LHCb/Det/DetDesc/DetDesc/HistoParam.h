// $Id: HistoParam.h,v 1.2 2009-12-05 14:25:46 dgolubko Exp $
// ============================================================================
#ifndef DETDESC_HISTOPARAM_H 
#define DETDESC_HISTOPARAM_H 1
// ============================================================================
// Include files
// ============================================================================
#include "DetDesc/Param.h"
// ============================================================================
// ROOT 
// ============================================================================
#include "TH1D.h"
#include "TH2D.h"

namespace DetDesc {
namespace Params {
  typedef TH1D Histo1D;
  typedef TH2D Histo2D;
}
}
// ============================================================================
/**  The declaration of the template 
 *   specialization of Param methods for the histograms 
 *   @see class Param 
 *   @see class TH1D  
 *   @param  name    (INPUT) XML-tag name 
 *   @param  comment (INPUT the comment 
 *   @param  precision (INPUT) not used 
 *   @return XML-string
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date 2009-10-22
 */
template<> 
std::string Param<DetDesc::Params::Histo1D>::toXMLStr 
( const std::string& name    , 
  const std::string& comment , 
  int precision              ) const ;
// ============================================================================
/**  The declaration of the template 
 *   specialization of Param methods for the histograms 
 *   @see class Param 
 *   @see class TH1D  
 *   @return string representation of the histogram 
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date 2009-10-22
 */
template<> std::string Param<DetDesc::Params::Histo1D>::toStr() const ;
// ============================================================================
/**  The declaration of the template 
 *   specialization of Param methods for the histograms 
 *   @see class Param 
 *   @see class TH1D  
 *   @param  name     (INPUT) XML-tag name 
 *   @param  comment  (INPUT the comment 
 *   @param  precision (INPUT) not used 
 *   @return XML-string
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date 2009-10-22
 */
template<> 
std::string Param<DetDesc::Params::Histo2D>::toXMLStr 
( const std::string& name    , 
  const std::string& comment , 
  int precision              ) const ;
// ============================================================================
/**  The declaration of the template 
 *   specialization of Param methods for the histograms 
 *   @see class Param 
 *   @see class TH2D  
 *   @return string representation of the histogram 
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date 2009-10-22
 */
template<> std::string Param<DetDesc::Params::Histo2D>::toStr() const ;
// ============================================================================
// The END 
// ============================================================================
#endif // DETDESC_HISTOPARAM_H
// ============================================================================
