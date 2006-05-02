// $Id: AuxFunBase.h,v 1.3 2006-05-02 14:29:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
// forward declaration
// ============================================================================
class MsgStream ;
// ============================================================================

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


#define _LOKI_FUNCTION_TYPES_( FUNC , TYPE )             \
  typedef          FUNC<TYPE>            Self         ;  \
  typedef typename LoKi::Function<TYPE>  FunB         ;  \
  typedef typename FunB::result_type     result_type  ;  \
  typedef typename FunB::argument        argument     ;


#define _LOKI_PREDICATE_TYPES_( PRED , TYPE )            \
  typedef          PRED<TYPE>            Self         ;  \
  typedef typename LoKi::Predicate<TYPE> FunB         ;  \
  typedef typename FunB::result_type     result_type  ;  \
  typedef typename FunB::argument        argument     ;

namespace LoKi
{
  
  /** @class AuxFunBase
   *
   *  Helpful function to serve as common pseudo-base for all LoKi functions
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  class AuxFunBase
  {
    
  protected:
  public: // to please Visual C++ compiler 
    
    // default constructor 
    AuxFunBase  ();
    // copy consructor 
    AuxFunBase  ( const AuxFunBase& );
    // destructor 
    virtual ~AuxFunBase ();
    
  protected:
  public: // to please Visual C++ compiler 
    
    /** print error message 
     *  @param msg  error message 
     *  @param sc   status code 
     *  @return status code 
     */
    virtual StatusCode Error
    ( const std::string& msg                       , 
      const StatusCode&  sc  = StatusCode::FAILURE ) const ;
    /** print warning message 
     *  @param msg  warning message 
     *  @param sc   status code 
     *  @return status code 
     */
    virtual StatusCode Warning
    ( const std::string& msg                       , 
      const StatusCode&  sc  = StatusCode::FAILURE ) const ;
    /** thrown an exception 
     *  @param msg  warning message 
     *  @param sc   status code 
     *  @return status code  (fictive)
     */
    virtual StatusCode Exception
    ( const std::string& msg                       , 
      const StatusCode&  sc  = StatusCode::FAILURE ) const ;

  public:
    
    /** (virtual) printout to std::ostream 
     *  @param s output stream 
     *  @return reference to the stream 
     */
    virtual std::ostream& fillStream( std::ostream& s ) const ;
    /** (virtual) printout in form of std::string 
     *  @return string representation (based on fillStream)   
     */
    virtual std::string   printOut  () const ;
    /// the actual object type 
    virtual std::string   objType   () const ;
  };
  
}; // end of namespac eLoKi 

// ============================================================================
/** output operator of function objects to std::ostream 
 *  @param stream refeence to the stream
 *  @param obj object to be printed 
 *  @return reference to the stream
 */
// ============================================================================
std::ostream& operator<< ( std::ostream&           stream , 
                           const LoKi::AuxFunBase& obj    ) ;
// ============================================================================

// ============================================================================
/** output operator of function objects to MsgStream
 *  @param stream refeence to the stream
 *  @param obj object to be printed 
 *  @return reference to the stream
 */
// ============================================================================
MsgStream&    operator<< ( MsgStream&              stream , 
                           const LoKi::AuxFunBase& obj    ) ;
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_AUXFUNBASE_H
// ============================================================================
