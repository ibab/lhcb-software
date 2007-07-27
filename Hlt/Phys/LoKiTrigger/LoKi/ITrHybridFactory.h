// $Id: ITrHybridFactory.h,v 1.1.1.1 2007-07-27 15:56:42 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_ITRHYBRIDFACTORY_H 
#define LOKI_ITRHYBRIDFACTORY_H 1
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
#include "LoKi/TrackTypes.h"
// ============================================================================
namespace LoKi
{
  /** @class ITrHybridFactory ITrHybridFactory.h LoKi/ITrHybridFactory.h
   *  The abstract interface to "hybrid factory"  
   *  @author Vanya BELYAEV ibelayev@physics.syr.edu
   *  @date   2007-06-10
   */
  class ITrHybridFactory : public virtual IAlgTool
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
    ( const std::string& pycode , LoKi::Types::TrCut&  cuts ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param cuts the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::RVCut&  cuts ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param cuts the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::TTrCut& cuts ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param cuts the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::TrVCut& cuts ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param cuts the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::RVVCut& cuts ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::TrFun& func ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::RVFun&  func ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::TTrFun& func ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::TrVFun& func ) = 0 ;
    // ========================================================================
    /** "Factory": get the the object form python code 
     *  @param pycode the python pseudo-code of the function
     *  @param func the placeholder for the result 
     *  @return StatusCode 
     */
    virtual StatusCode get
    ( const std::string& pycode , LoKi::Types::RVVFun& func ) = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    /** set the pointer to some reference container of tracks 
     *  @param tracks ponter to the reference container of tracks 
     *  @param who    the client 
     *  @return Status Code 
     */
    virtual StatusCode set 
    ( std::vector<LHCb::Track*>*     tracks     , 
      INamedInterface*               who    = 0 ) = 0 ;
    // ========================================================================
    /** set the pointer to some reference container of reconstructed vertices 
     *  @param tracks ponter to the reference container of reocntructed vertices 
     *  @param who    the client 
     *  @return Status Code 
     */
    virtual StatusCode set 
    ( std::vector<LHCb::RecVertex*>* tracks     , 
      INamedInterface*               who    = 0 ) = 0 ;
    // ========================================================================
  protected:
    // virtual and protected destructor
    virtual ~ITrHybridFactory( ); ///< Destructor
  } ;
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ITRHYBRIDFACTORY_H
// ============================================================================
