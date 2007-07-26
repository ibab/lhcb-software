// $Id: IHybridFactory.h,v 1.1 2007-07-26 13:25:09 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IHYBRIDFACTORY_H 
#define LOKI_IHYBRIDFACTORY_H 1
// ============================================================================
// Include files 
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/ATypes.h"
// ============================================================================
namespace LoKi
{
  /** @class IHybridFactory IHybridFactory.h LoKi/IHybridFactory.h
   *  The abstract interface to "hybrid factory"  
   *  @author Vanya BELYAEV ibelayev@physics.syr.edu
   *  @date   2007-06-10
   */
  class IHybridFactory : public virtual IAlgTool
  {
  public: 
    /// Return the unique interface ID
    static const InterfaceID& interfaceID() ;
  public:
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param cuts the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::Cut&  cuts ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param cuts the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::VCut&  cuts ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param cuts the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::ACut& cuts ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::Fun& func ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::VFun&  func ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::AFun& func ) = 0 ;
    // ========================================================================
  protected:
    virtual ~IHybridFactory( ); ///< Destructor
  } ;
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IHYBRIDFACTORY_H
// ============================================================================
