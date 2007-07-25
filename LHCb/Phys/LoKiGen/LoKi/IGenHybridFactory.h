// $Id: IGenHybridFactory.h,v 1.1 2007-07-25 15:07:57 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IGENHYBRIDFACTORY_H 
#define LOKI_IGENHYBRIDFACTORY_H 1
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
#include "LoKi/GenTypes.h"
// ============================================================================
namespace LoKi
{
  /** @class IGenHybridFactory IMCHybridFactory.h LoKi/IMCHybridFactory.h
   *  The abstract interface to "hybrid factory"  
   *  @author Vanya BELYAEV ibelayev@physics.syr.edu
   *  @date   2007-06-10
   */
  class IGenHybridFactory : public virtual IAlgTool
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
    ( const std::string& pycode , LoKi::Types::GCut&  cuts ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param cuts the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::GVCut&  cuts ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::GFun& func ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::GVFun&  func ) = 0 ;
    // ========================================================================
  protected:
    virtual ~IGenHybridFactory( ); ///< Destructor
  } ;
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IGENHYBRIDFACTORY_H
// ============================================================================
