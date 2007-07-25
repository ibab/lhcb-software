// $Id: IMCHybridFactory.h,v 1.1 2007-07-25 15:16:06 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IMCHYBRIDFACTORY_H 
#define LOKI_IMCHYBRIDFACTORY_H 1
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
#include "LoKi/MCTypes.h"
// ============================================================================
namespace LoKi
{
  /** @class IMCHybridFactory IMCHybridFactory.h LoKi/IMCHybridFactory.h
   *  The abstract interface to "hybrid factory"  
   *  @author Vanya BELYAEV ibelayev@physics.syr.edu
   *  @date   2007-06-10
   */
  class IMCHybridFactory : public virtual IAlgTool
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
    ( const std::string& pycode , LoKi::Types::MCCut&  cuts ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param cuts the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::MCVCut&  cuts ) = 0 ;
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::MCFun& func ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::MCVFun&  func ) = 0 ;
    // ========================================================================
  protected:
    virtual ~IMCHybridFactory( ); ///< Destructor
  } ;
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IHYBRIDFACTORY_H
// ============================================================================
