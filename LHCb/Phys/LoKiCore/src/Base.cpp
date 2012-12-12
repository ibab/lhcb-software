// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/System.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/InstanceCounter.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/IReporter.h"
#include "LoKi/Base.h"
#include "LoKi/Report.h"
// ============================================================================
/** @file
 *
 * Implementation file for class LoKi::Base
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
 *  @date 2006-03-10 
 */
// ============================================================================
namespace 
{ 
  // ==========================================================================
  /** @var s_BaseCounter 
   *  static instance ounter of object of the type LoKi::Base 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-03-10
   */
  // ==========================================================================
  InstanceCounter<LoKi::Base> s_BaseCounter ;
  // ==========================================================================
} 
// ============================================================================
// Standard constructor
// ============================================================================
LoKi::Base::Base 
( const std::string& name     , 
  const IReporter*   reporter ) 
  : m_name       ( name     ) 
  , m_typeName   ()
  , m_reporter   ( reporter ) 
  , m_refCount   ( 0        ) 
{
  s_BaseCounter.increment();
}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Base::Base
( const LoKi::Base& right ) 
  : m_name       ( right.m_name     ) 
  , m_typeName   ()
  , m_reporter   ( right.m_reporter ) 
  , m_refCount   ( 0 )
{
  s_BaseCounter.increment();
}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Base::~Base() { s_BaseCounter.decrement() ; } 
// ============================================================================
// ASSIGNEMENT operator : refCount is not affected!
// ============================================================================
LoKi::Base& LoKi::Base::operator=
( const LoKi::Base& right ) 
{
  if ( &right == this ) { return *this ; }
  // assigne the name 
  m_name     = right.m_name ;
  // reset the type/name pair 
  m_typeName = "";
  // assign the reporter
  m_reporter = right.m_reporter ;
  // m_refCount is not affected!!!              // ATTENTION! 
  return *this ;
}
// ============================================================================
/*  Print the error message and return with the given StatusCode.
 *
 *  Also performs statistical analysis of the error messages and
 *  suppression after the defined number of error instances.
 *
 *  @code
 *
 *   if( a < 0 ) { return Error("a is negative!") ;}
 *   if( b < 0 ) { return Error("b is illegal!" , StatusCode(25) );
 *   if( c < 0 )
 *      { return Error("c is negative" , StatusCode(35) , 50 );
 *
 *  @endcode
 *
 *  @see MsgStream
 *  @see IMessageSvc
 *  @see StatusCode
 *
 *  @param msg    Error message
 *  @param st     StatusCode to return
 *  @param mx     Maximum number of printouts for this message
 */
// ============================================================================
StatusCode LoKi::Base::Error
( const std::string& msg ,
  const StatusCode   st  ,
  const size_t       mx  ) const 
{
  return 
    m_reporter.validPointer() ?
    m_reporter->   Error (                               msg , st , mx ) :
    LoKi::Report:: Error ( type() + "/" + name() + ":" + msg , st , mx ) ;
} 
// ============================================================================
/*  Print the warning message and return with the given StatusCode.
 *
 *  Also performs statistical analysis of the error messages and
 *  suppression after the defined number of error instances.
 *
 *  @code
 *
 *   if( a < 0 ) { return Warning("a is negative!") ;}
 *   if( b < 0 ) { return Warning("b is illegal!" , StatusCode(25) );
 *   if( c < 0 )
 *      { return Warning("c is negative" , StatusCode(35) , 50 );
 *
 *  @endcode
 *
 *  @see MsgStream
 *  @see IMessageSvc
 *  @see StatusCode
 *
 *  @param msg    Warning message
 *  @param st     StatusCode to return
 *  @param mx     Maximum number of printouts for this message
 *  @return       The given StatusCode
 */
// ============================================================================
StatusCode LoKi::Base::Warning
( const std::string& msg ,
  const StatusCode   st  ,
  const size_t       mx  ) const 
{
  return 
    m_reporter.validPointer() ?
    m_reporter->   Warning (                                 msg , st , mx ) :
    LoKi::Report:: Warning ( type() + "/" + name() + ": " +  msg , st , mx ) ;
} 
// ============================================================================
/*  Print the message and return status code 
 *  @see MsgStream
 *  @see IMessageSvc 
 *  @see StatusCode 
 *  @param msg    warning message 
 *  @param st     status code 
 *  @param lev    print level 
 *  @return       status code 
 */
// ============================================================================
void LoKi::Base::Print     
( const std::string& msg , 
  const StatusCode   st  ,
  const MSG::Level   lev ) const 
{
  st.ignore() ;
  if ( m_reporter.validPointer() ) 
  { m_reporter->   Print  ( msg , st , lev ).ignore() ; }
  else 
  { LoKi::Report:: Print  ( type() + "/" + name() + ": " + msg , st , lev ) ; }
} 
// ============================================================================
/*  Create and (re)-throw the exception  
 *  @see GaudiException
 *  @exception CaudiException always!
 *  @param msg    exception message 
 *  @param exc    (previous) exception of type GaudiException
 *  @param sc     status code  
 */
// ============================================================================
void LoKi::Base::Exception 
( const std::string    & msg ,  
  const GaudiException & exc , 
  const StatusCode       sc  ) const 
{
  sc.ignore() ;
  if ( m_reporter.validPointer() ) 
  { m_reporter->   Exception ( msg , exc , sc ); }
  else 
  { LoKi::Report:: Exception ( type() + "/" + name() + ": " + msg , exc , sc ) ; }
} 
// ============================================================================
/*  Create and (re)-throw the exception  
 *  @see GaudiException
 *  @exception GaudiException always!
 *  @param msg    exception message 
 *  @param exc    (previous) exception of type std::exception
 *  @param sc     status code  
 */
// ============================================================================
void LoKi::Base::Exception 
( const std::string    & msg ,  
  const std::exception & exc , 
  const StatusCode       sc  ) const 
{
  sc.ignore() ;
  if ( m_reporter.validPointer() ) 
  { m_reporter->   Exception ( msg , exc , sc ); }
  else 
  { LoKi::Report:: Exception ( type() + "/" + name() + ": " + msg , exc , sc ) ; }
} 
// ============================================================================
/*  Create and throw the exception  
 *  @see GaudiException
 *  @exception GaudiException always!
 *  @param msg    exception message 
 *  @param sc     status code  
 */
// ============================================================================
void LoKi::Base::Exception
( const std::string& msg ,  
  const StatusCode   sc  ) const 
{
  sc.ignore() ;
  if ( m_reporter.validPointer() ) 
  { m_reporter->   Exception ( msg , sc ); }
  else 
  { LoKi::Report:: Exception ( type() + "/" + name() + ": " + msg , sc ) ; }
} 
// ============================================================================
// get the type of the base object 
// ============================================================================
const std::string& LoKi::Base::type() const
{
  static std::string s_type = "" ;
  if ( s_type.empty() ) { s_type = System::typeinfoName( typeid( *this ) ); }
  return s_type ;
} 
// ============================================================================
// decreemnt the reference counter 
// ============================================================================
long LoKi::Base::release    ()
{
  if ( 0 == --m_refCount ) { delete this ; }   ///< ATTENTION!
  return m_refCount ;
} 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


