// ============================================================================
#ifndef KERNEL_INSPECTORS_H
#define KERNEL_INSPECTORS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD &STL
// ============================================================================
#include <vector>
#include <string>
#include <iosfwd>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IAlgManager.h"
// ============================================================================
// forward decalrations
// ============================================================================
class IInterface      ;
class IProperty  ;
class Property   ;
// ============================================================================
namespace Gaudi
{
  namespace Utils
  {
    // ========================================================================
    /// the actual type for the vector of properties
    typedef std::vector<const Property*> Properties ;
    /// the actual type for the vector of property names
    typedef std::vector<std::string>     Names      ;
    // ========================================================================
    /** simple function to extract from the given component
     *  the list of properties, specified by name
     *
     *  @code
     *
     *  const Gaudi::Utils::Names& names = ... ;
     *
     *  const IInterface* component = ... ;
     *
     *  Gaudi::Utils::Properties props =
     *    Gaudi::Utils::properties ( component , names ) ;
     *
     *  for( Gaudi::Utils::Properties::const_iterator ip = props.begin() ;
     *       props.end () != ip ; ++ip )
     *   {
     *      const Property* p = *ip ;
     *      if ( 0 == p ) { continue ; }
     *      // print property:
     *      std::cout << (*p) << std::endl ;
     *   }
     *
     *  @endcode
     *
     *  @see Gaudi::Utils::property
     *  @param cmp the component
     *  @param name list of property names
     *  @retutn the list of properties
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2008-08-02
     */
    Properties properties
    ( const IInterface* cmp   ,
      const Names&      names ) ;
    // ========================================================================
    /** simple function to extract from the given component
     *  the list of properties, specified by name
     *
     *  @code
     *
     *  const Gaudi::Utils::Names& names = ... ;
     *
     *  const IInterface* component = ... ;
     *
     *  Gaudi::Utils::Properties props =
     *    Gaudi::Utils::properties ( component , names ) ;
     *
     *  for( Gaudi::Utils::Properties::const_iterator ip = props.begin() ;
     *       props.end () != ip ; ++ip )
     *   {
     *      const Property* p = *ip ;
     *      if ( 0 == p ) { continue ; }
     *      // print property:
     *      std::cout << (*p) << std::endl ;
     *   }
     *
     *  @endcode
     *
     *  @see Gaudi::Utils::property
     *  @param cmp the component
     *  @param name list of property names
     *  @param output (OUTPUT) the list of properties
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-08-02
     */
    void properties
    ( const IInterface* cmp    ,
      const Names&      names  ,
      Properties&       output ) ;
    // ========================================================================
  } // end of namespace Gaudi::Utils
  // ==========================================================================
} // end of namespace Gaudi
// ============================================================================
namespace LHCb
{
  // ==========================================================================
  /** @class Inspector
   *  Simple inspector of algorithm properties
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-08-02
   */
  class Inspector
  {
  public:
    // ========================================================================
    /** constructor the list of properties
     *  @param props   list of properties to be inspected
     */
    Inspector
    ( const Gaudi::Utils::Names& props   = Gaudi::Utils::Names() ) ;
    /** constructor the list of properties
     *  @param props   list of properties to be inspected
     *  @param members list of structural properties
     */
    Inspector
    ( const Gaudi::Utils::Names& props   ,
      const Gaudi::Utils::Names& members ) ;
    // ========================================================================
  public:
    // ========================================================================
    /** inspect the component
     *
     *    - 500   NULL pointer is used
     *    - 501   Gaudi::svcLocator points to NULL
     *    - 502   Unable to extract IAlgManager
     *    - 503   IAlgorithm points to NULL
     *    - 510+N Error code from IAlgManager::getAlgorithm
     *
     *  @param component the component to be inspected
     *  @param stream the output stream
     *  @param level the recursion level
     *  @return status code
     */
    StatusCode inspect
    ( const IInterface* component     ,
      std::ostream&     stream        ,
      const size_t      level     = 0 ) const ;
    // ========================================================================
    /** inspect the component
     *  @param component the component to be inspected
     *  @param level the recursion level
     *  @return the inspection result
     */
    std::string inspect
    ( const IInterface* component     ,
      const size_t      level     = 0 ) const ;
    // ========================================================================
  public:
    // ========================================================================
    /** set new algorithm manager (optiojally).
     *
     *  Status codes:
     *
     *    - 504 NULL pointer is used
     *    - 505 IAlgManage ris invalid after the assignement
     *
     *  @param mgr new algorithm manager
     *  @return status code
     */
    StatusCode setAlgManager ( const IInterface* mgr ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// structural properties
    std::vector<std::string>     m_members ;
    /// properties toe binspected
    std::vector<std::string>     m_names   ;
    // ========================================================================
    // the algorithm manager
    mutable SmartIF<IAlgManager> m_algMgr  ;
    // ========================================================================
  };
  // ==========================================================================
}
// ============================================================================
// The END
// ============================================================================
#endif // KERNEL_INSPECTORS_H
// ============================================================================
