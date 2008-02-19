// $Id: IGenHybridFactory.h,v 1.2 2008-02-19 13:16:25 ibelyaev Exp $
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
    // predicates 
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param cuts the placeholder for the result 
     *  @param context context code fragment to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode       , 
      LoKi::Types::GCut& cuts         , 
      const std::string& context = "" )  = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param cuts the placeholder for the result 
     *  @param context context code fragment to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&  pycode       , 
      LoKi::Types::GVCut& cuts         ,
      const std::string&  context = "" )  = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // functions 
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context context code fragment to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode       ,
      LoKi::Types::GFun& func         , 
      const std::string& context = "" )  = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context context code fragment to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&  pycode       ,
      LoKi::Types::GVFun& func         ,
      const std::string&  context = "" )  = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // maps 
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context context code fragment to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode       ,
      LoKi::Types::GMap& func         , 
      const std::string& context = "" )  = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context context code fragment to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&  pycode       ,
      LoKi::Types::GVMap& func         ,
      const std::string&  context = "" )  = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // pipes 
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context context code fragment to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&  pycode       ,
      LoKi::Types::GPipe& func         , 
      const std::string&  context = "" )  = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context context code fragment to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&   pycode       ,
      LoKi::Types::GVPipe& func         ,
      const std::string&   context = "" )  = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // fun-vals 
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context context code fragment to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&    pycode       ,
      LoKi::Types::GFunVal& func         , 
      const std::string&    context = "" )  = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context context code fragment to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&     pycode       ,
      LoKi::Types::GVFunVal& func         ,
      const std::string&     context = "" )  = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // elements
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context context code fragment to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&     pycode       ,
      LoKi::Types::GElement& func         , 
      const std::string&     context = "" )  = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context context code fragment to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&      pycode       ,
      LoKi::Types::GVElement& func         ,
      const std::string&      context = "" )  = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // sources 
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context context code fragment to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&     pycode       ,
      LoKi::Types::GSource&  func         , 
      const std::string&     context = "" )  = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @param context context code fragment to be executed 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string&      pycode       ,
      LoKi::Types::GVSource&  func         ,
      const std::string&      context = "" )  = 0 ;
    // ========================================================================
  protected:
    // ========================================================================
    // virtual & protecte destructor 
    virtual ~IGenHybridFactory( ); ///< Destructor
    // ========================================================================
  } ;
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IGENHYBRIDFACTORY_H
// ============================================================================
