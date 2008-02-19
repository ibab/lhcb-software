// $Id: IMCHybridFactory.h,v 1.2 2008-02-19 15:04:01 ibelyaev Exp $
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
    // predicates:
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param cuts the placeholder for the result 
     *  @param context the code fragment to be pre-executed
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&   pycode       ,
      LoKi::Types::MCCut&  cuts         , 
      const std::string&   context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param cuts the placeholder for the result 
     *  @param context the code fragment to be pre-executed
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&   pycode       , 
      LoKi::Types::MCVCut& cuts         , 
      const std::string&   context = "" ) = 0 ;
  public:
    // ========================================================================
    // functions:
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the code fragment to be pre-executed
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&   pycode       , 
      LoKi::Types::MCFun&  func         ,
      const std::string&   context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the code fragment to be pre-executed
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&   pycode       , 
      LoKi::Types::MCVFun& func         , 
      const std::string&   context = "" ) = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // maps:
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the code fragment to be pre-executed
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&   pycode       , 
      LoKi::Types::MCMap&  func         ,
      const std::string&   context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the code fragment to be pre-executed
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&   pycode       , 
      LoKi::Types::MCVMap& func         , 
      const std::string&   context = "" ) = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // pipes:
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the code fragment to be pre-executed
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&   pycode       , 
      LoKi::Types::MCPipe& func         ,
      const std::string&   context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the code fragment to be pre-executed
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&    pycode       , 
      LoKi::Types::MCVPipe& func         , 
      const std::string&    context = "" ) = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // fun-vals:
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the code fragment to be pre-executed
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&     pycode       , 
      LoKi::Types::MCFunVal& func         ,
      const std::string&     context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the code fragment to be pre-executed
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&      pycode       , 
      LoKi::Types::MCVFunVal& func         , 
      const std::string&      context = "" ) = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // elements:
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the code fragment to be pre-executed
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&      pycode       , 
      LoKi::Types::MCElement& func         ,
      const std::string&      context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the code fragment to be pre-executed
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&       pycode       , 
      LoKi::Types::MCVElement& func         , 
      const std::string&       context = "" ) = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // sources:
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the code fragment to be pre-executed
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&      pycode       , 
      LoKi::Types::MCSource&  func         ,
      const std::string&      context = "" ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context the code fragment to be pre-executed
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&      pycode       , 
      LoKi::Types::MCVSource& func         , 
      const std::string&      context = "" ) = 0 ;
    // ========================================================================
  protected:
    // ========================================================================
    // virtual & protected destructor 
    virtual ~IMCHybridFactory( ); ///< Destructor
    // ========================================================================
  } ;
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IHYBRIDFACTORY_H
// ============================================================================
