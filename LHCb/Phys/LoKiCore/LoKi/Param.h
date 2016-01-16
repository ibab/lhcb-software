// $Id$ 
// ============================================================================
#ifndef LOKI_PARAM_H 
#define LOKI_PARAM_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// forward declarations 
// ============================================================================
/* @file LoKi/Param.h
 *
 * This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 * The package has been designed with the kind help from
 * Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 * contributions and advices from G.Raven, J.van Tilburg,
 * A.Golutvin, P.Koppenburg have been used in the design.
 *
 * By usage of this code one clearly states the disagreement 
 * with the smear campaign of Dr.O.Callot et al.: 
 * ``No Vanya's lines are allowed in LHCb/Gaudi software''
 *
 * @date 2014-02-02 
 * @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$ 
 */
namespace LoKi 
{
  // ==========================================================================
  /** @class Param LoKi/Param.h
   *  simple representation of "parameter" 
   *  @author Vanya Belyaev
   *  @date   2014-02-02
   *  @thanks Alexander MAZUROV for regex parsing code 
   */
  class GAUDI_API Param 
  {
  public:
    // ========================================================================
    /** Constructor from full specialization 
     *  format: 'property[key]/algorithm'
     *  - property name 
     *  - key in property, if property is map-property 
     *  - the (regex)-pattern for the algorithm name 
     */
    Param ( const std::string& property  = "" ) ;
    //
    virtual ~Param() ;
    // ========================================================================
  public:
    // ========================================================================
    const std::string& property  () const { return m_property  ; }
    const std::string& key       () const { return m_key       ; }
    const std::string& algorithm () const { return m_algorithm ; }
    // ========================================================================
  public:
    // ========================================================================
    /// printout 
    std::ostream& fillStream ( std::ostream& s ) const ;
    // ========================================================================
  private:
    // ========================================================================
    /// the property name 
    std::string m_property  ;
    /// the key name if property is a map 
    std::string m_key       ;
    /// the algorithm pattern (if relevant) 
    std::string m_algorithm ;
    // ========================================================================
  };
  // ==========================================================================
  /** output operator of function objects to std::ostream
   *  @param stream reference to the stream
   *  @param obj object to be printed
   *  @return reference to the stream
   */
  inline 
  std::ostream& operator<<( std::ostream& stream , const Param&  obj ) 
  { return obj.fillStream ( stream ) ; }
  // ==========================================================================
} //                                                  the end of namespace LoKi
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Utils 
  {
    // ========================================================================
    GAUDI_API std::string toCpp ( const LoKi::Param& o ) ;
    // ========================================================================
  } 
  // ==========================================================================
} // 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARAM_H
// ============================================================================
