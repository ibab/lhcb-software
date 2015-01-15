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
#include <tuple>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/ILoKiSvc.h"
#include "LoKi/ToCpp.h"
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
  // ==========================================================================
  /** @class AuxFunBase
   *  Helpful function to serve as common pseudo-base for all LoKi functions
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  class GAUDI_API AuxFunBase
  {
  protected:
    // public:    // to please Visual C++ compiler
    // ========================================================================
    /// default constructor 
    AuxFunBase  () ; 
    /// constructor with arguments : 
    template <typename ... ARGS>
      AuxFunBase ( const std::tuple<ARGS...>& tup ) : AuxFunBase ()
    { m_cargs = Gaudi::Utils::toCpp ( tup ) ; }
    // ,  m_cargs ( Gaudi::Utils::toCpp ( tup ) ) 
    // {}
    // ========================================================================
    /// copy consructor
    AuxFunBase  ( const AuxFunBase&  right      ) ;          // copy consructor
    /// destructor
    virtual ~AuxFunBase ();                                   // destructor
    // ========================================================================
  public:  
    // ========================================================================
    /** print error message
     *  @param msg  error message
     *  @param sc   status code
     */
    StatusCode Error
    ( const std::string& msg                                             ,
      const StatusCode&  sc  = StatusCode ( StatusCode::FAILURE , true ) ,
      int mxprint = 10                                                   ) const ;
    /** print warning message
     *  @param msg  warning message
     *  @param sc   status code
     */
    StatusCode Warning
    ( const std::string& msg                                             ,
      const StatusCode&  sc  = StatusCode ( StatusCode::FAILURE , true ) ,
      int mxprint = 10                                                   ) const ;
    /** thrown an exception
     *  @param msg  warning message
     *  @param sc   status code
     */
    void Exception
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
    /** (virtual) printout in form of std::string
     *  @return string representation (must be valid C++)
     */ 
    virtual std::string   toCpp     () const ;
    // ========================================================================
  public:
    // ========================================================================
    /// get the event-ID
    unsigned long long event    ()                 const { return m_event ; }
    /// set the event-ID
    void setEvent ( const unsigned long long evt ) const { m_event = evt  ; }
    /// set the event-ID from LoKi service
    void setEvent (          ) const ;
    // ========================================================================
  public:
    // ========================================================================
    /// check the data for the same event
    bool sameEvent() const ;
    /// get constructor arguments 
    const std::string& cargs() const { return m_cargs ; }
    // ========================================================================
  public:
    // ========================================================================
    /// get LoKi service
    static const LoKi::Interface<LoKi::ILoKiSvc>& lokiSvc () ;
    /// set LoKi service
    static bool  setLoKiSvc ( const LoKi::ILoKiSvc*  svc ) ;
    /// set LoKi service
    static bool  setLoKiSvc ( const LoKi::Interface<LoKi::ILoKiSvc>& svc ) ;
    // ========================================================================
  public:
    // ========================================================================
    // does Gaudi/LoKi run?
    bool gaudi() const { return !(!lokiSvc()) ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the event ID
    mutable unsigned long long m_event ;                        // the event ID
    /// constructor arguments 
    std::string                m_cargs ;   // constructor arguments 
    /// =======================================================================
  };
  // ==========================================================================
  /** simple function to generate the default generic
   *  (hopefully unique?) ID for the functor
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   */
  GAUDI_API
  std::size_t genericID ( const AuxFunBase& ) ;
  // ==========================================================================
  /** output operator of function objects to std::ostream
   *  @param stream reference to the stream
   *  @param obj object to be printed
   *  @return reference to the stream
   */
  GAUDI_API
  std::ostream& operator<<
    ( std::ostream&      stream ,
      const AuxFunBase&  obj    ) ;
  // ==========================================================================
  /** output operator of function objects to std::ostream
   *  @param stream reference to the stream
   *  @param obj object to be printed
   *  @return reference to the stream
   */
  GAUDI_API
  MsgStream& operator<<
    ( MsgStream&         stream ,
      const AuxFunBase&  obj    ) ;
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Utils
  {
    // ========================================================================
    /** string representation of the object
     */
    GAUDI_API
    std::string toString ( const LoKi::AuxFunBase& o ) ;
    // ========================================================================
    /** dump the object to the stream
     */
    GAUDI_API
    std::ostream&
    toStream ( const LoKi::AuxFunBase& o , std::ostream& s ) ;
    // ========================================================================    
    /** string representation of the object (valid C++ code) 
     */
    GAUDI_API
    std::string toCpp    ( const LoKi::AuxFunBase& o ) ;
    // ========================================================================
  } //                                            end of namespace Gaudi::Utils
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_AUXFUNBASE_H
// ============================================================================
