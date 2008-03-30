// $Id: AuxFunBase.h,v 1.9 2008-03-30 13:31:28 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_AUXFUNBASE_H 
#define LOKI_AUXFUNBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
#include <vector>
#include <string>
#include <ostream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Interface.h"
// ============================================================================
// forward declaration
// ============================================================================
class MsgStream ;
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================
namespace LoKi
{  
  /** @class AuxFunBase
   *  Helpful function to serve as common pseudo-base for all LoKi functions
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  class AuxFunBase
  {   
  protected:
  public:    // to please Visual C++ compiler
    // ========================================================================
    /// constructor from LoKi Service
    AuxFunBase  ( const LoKi::ILoKiSvc* svc = 0 ) ; 
    // copy consructor 
    AuxFunBase  ( const AuxFunBase&     right ) ; //</ copy consructor 
    // destructor 
    virtual ~AuxFunBase (); ///< destructor 
    // ========================================================================
  protected:
  public:    // to please Visual C++ compiler
    // ========================================================================
    /** print error message 
     *  @param msg  error message 
     *  @param sc   status code 
     */
    virtual void Error
    ( const std::string& msg                                             , 
      const StatusCode&  sc  = StatusCode ( StatusCode::FAILURE , true ) ) const ;
    /** print warning message 
     *  @param msg  warning message 
     *  @param sc   status code 
     */
    virtual void Warning
    ( const std::string& msg                                             , 
      const StatusCode&  sc  = StatusCode ( StatusCode::FAILURE , true ) ) const ;
    /** thrown an exception 
     *  @param msg  warning message 
     *  @param sc   status code 
     *  @return status code  (fictive)
     */
    virtual void Exception
    ( const std::string& msg                                             , 
      const StatusCode&  sc  = StatusCode ( StatusCode::FAILURE , true ) ) const ;
    /** assert the condition 
     *  @param condition the condition to be asserted 
     *  @param message the message to be associated with the exception
     *  @param sc status code 
     */
    inline  
    void Assert 
    ( const bool         condition                                      , 
      const std::string& message                                        ,  
      const StatusCode&  sc = StatusCode ( StatusCode::FAILURE , true ) ) const 
    { if ( !condition ) { Exception ( message , sc ) ; } ; sc.ignore() ; }
    /** assert the condition 
     *  @param condition the condition to be asserted 
     *  @param message the message to be associated with the exception
     *  @param sc status code 
     */
    inline void Assert 
    ( const bool         condition                                            , 
      const char*        message                                              ,  
      const StatusCode&  sc       = StatusCode ( StatusCode::FAILURE , true ) ) const 
    { if ( !condition ) { Exception ( message , sc ) ; } ; sc.ignore() ; }    
    // ========================================================================
  public:
    // ========================================================================
    /** (virtual) printout to std::ostream 
     *  @param s output stream 
     *  @return reference to the stream 
     */
    virtual std::ostream& fillStream ( std::ostream& s ) const ;
    /** (virtual) printout in form of std::string 
     *  @return string representation (based on fillStream)   
     */
    virtual std::string   printOut  () const ;
    /// the actual object type 
    virtual std::string   objType   () const ;
    /// unique function ID (hash); see LoKi::genericID 
    virtual std::size_t   id        () const ;
    // ========================================================================
  public:
    // ========================================================================
    /// get the event-ID 
    long event    ()           const { return m_event ; }
    /// set the event-ID 
    void setEvent ( long evt ) const { m_event = evt  ; }
    /// set the event-ID from LoKi service 
    void setEvent (          ) const ; 
    // ========================================================================
  public:
    // ========================================================================
    /// check the data for the same event 
    bool sameEvent() const ;
    // ========================================================================
  public:
    // ========================================================================
    /// get LoKi service 
    const LoKi::Interface<LoKi::ILoKiSvc>& lokiSvc () const ;
    /// get LoKi service 
    const LoKi::ILoKiSvc*               getLokiSvc () const { return lokiSvc() ; }
    /// set LoKi service 
    void setLoKiSvc 
    (                       LoKi::ILoKiSvc*  svc ) const { m_lokiSvc = svc ; }
    /// set LoKi service 
    void setLoKiSvc 
    ( const LoKi::Interface<LoKi::ILoKiSvc>& svc ) const { m_lokiSvc = svc ; }
    // ========================================================================
  private:
    // ========================================================================
    // the event ID 
    mutable long m_event ; ///< the event ID 
    // LoKi service
    mutable LoKi::Interface<LoKi::ILoKiSvc> m_lokiSvc ; ///< LoKi service
    // ========================================================================
  };
  // ==========================================================================
  /** simple fuctiin to generate the default generic 
   *  (hopefully unique?) ID for the functor 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   */
  std::size_t genericID( const AuxFunBase& ) ;  
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
/** output operator of function objects to std::ostream 
 *  @param stream refeence to the stream
 *  @param obj object to be printed 
 *  @return reference to the stream
 */
std::ostream& operator<< 
  ( std::ostream&           stream , 
    const LoKi::AuxFunBase& obj    ) ;
// ============================================================================
/** output operator of function objects to MsgStream
 *  @param stream refeence to the stream
 *  @param obj object to be printed 
 *  @return reference to the stream
 */
MsgStream&    operator<< 
  ( MsgStream&              stream , 
    const LoKi::AuxFunBase& obj    ) ;
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_AUXFUNBASE_H
// ============================================================================
