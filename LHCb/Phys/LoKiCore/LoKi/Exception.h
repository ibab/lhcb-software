// $Id: Exception.h,v 1.6 2007-08-11 20:17:00 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_EXCEPTION_H 
#define LOKI_EXCEPTION_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel 
// ============================================================================
#include "GaudiKernel/GaudiException.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Assert.h"
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
  /** @class Exception Exception.h LoKi/Exception.h
   *  Standard exception for LoKi objects 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-12
   */
  class Exception : public GaudiException
  {    
  public:    
    /** Constructor
     *  @param Message error message
     *  @param Code status code 
     */
    Exception
    ( const std::string&   Message                       , 
      const StatusCode &   Code    = StatusCode::FAILURE , 
      const std::string&   file    = ""                  , 
      const unsigned long& line    = 0                   ) ;
    /** Constructor 
     *  @param Message error message
     *  @param Code status code
     *  @param Exception "previous"  exception
     */
    Exception 
    ( const std::string&     Message         , 
      const StatusCode &     Code            , 
      const GaudiException&  Exception       ,
      const std::string&     file       = "" , 
      const unsigned long&   line       = 0  ) ;
    /// virtual destructor 
    virtual ~Exception() throw() {} ;
    /** clone operation
     *  @see GaudiException
     *  @return the clone
     */
    virtual Exception* clone() const;
  private:
    /// the default constructor is disabled 
    Exception();
  }; // end of class Exception
}// end of namespace  LooK
// ============================================================================
/** @def LOKI_EXCEPTION
 *  
 *  Small and simple macro to throw LoKi::Exception with 
 *  the fiel name and line number for easy location of the problems 
 * 
 *  Usage:
 *
 *  <c> 
 *
 *  if( a < 0 ) { LOKI_EXCEPTION("'a' is negative",StatusCode::FAILURE ) ; } 
 *
 *  </c> 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2002-10-29
 */
// ============================================================================
#define LOKI_EXCEPTION( message , statuscode )   \
      throw LoKi::Exception (  message , statuscode , __FILE__ , __LINE__ )
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_EXCEPTION_H
// ============================================================================
