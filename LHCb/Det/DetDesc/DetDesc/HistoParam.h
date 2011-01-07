// $Id: HistoParam.h,v 1.3 2009-12-06 19:13:45 dgolubko Exp $
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
#ifdef __INTEL_COMPILER        // Disable ICC remark from Root headers
  #pragma warning(push)
  #pragma warning(disable:2259) // non-pointer conversion may lose significant bits
#endif
#include "TH1D.h"
#include "TH2D.h"
#ifdef __INTEL_COMPILER        // Disable ICC remark from Root headers
  #pragma warning(pop)
#endif

namespace DetDesc
{
  namespace Params 
  {
    typedef TH1D Histo1D;
    typedef TH2D Histo2D;

    // ========================================================================
    /**  Interface function converting 1d histogram to xml string
     *   @see class Param 
     *   @see class TH1D  
     *   @param  histo     (INPUT) histogram
     *   @param  name      (INPUT) XML-tag name 
     *   @param  comment   (INPUT) the comment 
     *   @param  precision (INPUT) not used 
     *   @return XML-string
     *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl, Dmitry Golubkov
     *   @date 2009-12-06
     */
    GAUDI_API std::string toXMLStr
    ( const DetDesc::Params::Histo1D &histo,
      const std::string& name,
      const std::string& comment,
      int precision                         );
    
    // ========================================================================
    /**  Interface function converting 2d histogram to xml string
     *   @see class Param 
     *   @see class TH1D  
     *   @param  histo     (INPUT) histogram
     *   @param  name      (INPUT) XML-tag name 
     *   @param  comment   (INPUT) the comment 
     *   @param  precision (INPUT) not used 
     *   @return XML-string
     *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl, Dmitry Golubkov
     *   @date 2009-12-06
     */
    GAUDI_API std::string toXMLStr
    ( const DetDesc::Params::Histo2D &histo,
      const std::string& name,
      const std::string& comment,
      int precision                         );
    
    // ========================================================================
    /**  Interface function converting 1d histogram to a custom format string
     *   @see class Param 
     *   @see class TH1D  
     *   @param  histo     (INPUT) histogram
     *   @return string
     *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl, Dmitry Golubkov
     *   @date 2009-12-06
     */
    GAUDI_API std::string toStr
    ( const DetDesc::Params::Histo1D &histo );

    // ========================================================================
    /**  Interface function converting 2d histogram to a custom format string
     *   @see class Param 
     *   @see class TH1D  
     *   @param  histo     (INPUT) histogram
     *   @return string
     *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl, Dmitry Golubkov
     *   @date 2009-12-06
     */
    GAUDI_API std::string toStr
    ( const DetDesc::Params::Histo2D &histo );
  }
}
// ============================================================================
/**  The template specialization of Param methods for the histograms 
 *   @see class Param 
 *   @see class TH1D  
 *   @param  name    (INPUT) XML-tag name 
 *   @param  comment (INPUT the comment 
 *   @param  precision (INPUT) not used 
 *   @return XML-string
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date 2009-10-22
 */
template<> inline
std::string Param<DetDesc::Params::Histo1D>::toXMLStr 
( const std::string& name    , 
  const std::string& comment , 
  int precision              ) const
{ 
  return DetDesc::Params::toXMLStr(m_val, name, comment, precision);
}
// ============================================================================
/**  The template specialization of Param methods for the histograms 
 *   @see class Param 
 *   @see class TH1D  
 *   @return string representation of the histogram 
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date 2009-10-22
 */
template<> inline std::string Param<DetDesc::Params::Histo1D>::toStr() const
{
  return DetDesc::Params::toStr( m_val );
}
// ============================================================================
/**  The template specialization of Param methods for the histograms 
 *   @see class Param 
 *   @see class TH1D  
 *   @param  name     (INPUT) XML-tag name 
 *   @param  comment  (INPUT the comment 
 *   @param  precision (INPUT) not used 
 *   @return XML-string
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date 2009-10-22
 */
template<> inline
std::string Param<DetDesc::Params::Histo2D>::toXMLStr 
( const std::string& name    , 
  const std::string& comment , 
  int precision              ) const
{
  return DetDesc::Params::toXMLStr(m_val, name, comment, precision); 
}
// ============================================================================
/**  The template specialization of Param methods for the histograms 
 *   @see class Param 
 *   @see class TH2D  
 *   @return string representation of the histogram 
 *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *   @date 2009-10-22
 */
template<> inline std::string Param<DetDesc::Params::Histo2D>::toStr() const
{
  return DetDesc::Params::toStr( m_val ); 
}
// ============================================================================
// The END 
// ============================================================================
#endif // DETDESC_HISTOPARAM_H
// ============================================================================
