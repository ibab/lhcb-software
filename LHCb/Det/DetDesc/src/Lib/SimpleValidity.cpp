// $Id: SimpleValidity.cpp,v 1.4 2005-04-22 13:10:41 marcocle Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2003/06/16 13:42:36  sponce
// fixes for gcc 3.2 and 3.3
//
// Revision 1.2  2002/01/21 14:46:46  sponce
// Remove all warnings + some bug fixes
//
// Revision 1.1  2001/11/18 15:32:45  ibelyaev
//  update for Logical Assemblies
// 
// ============================================================================
// Include files
// GaudiKernel
#include "GaudiKernel/TimePoint.h"
#include "GaudiKernel/StatusCode.h"
// local
#include "SimpleValidity.h"

// ============================================================================
/** @file SimpleValidity.cpp
 *
 * Implementation file for class : SimpleValidity
 * 
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @date 17/11/2001 
 */
// ============================================================================

// ============================================================================
/** standard (default) constructor 
 */
// ============================================================================
SimpleValidity::SimpleValidity() 
  : m_since ( 0 ) 
  , m_till  ( 0 ) 
{
  m_since = new TimePoint( time_absolutepast   ) ;
  m_till  = new TimePoint( time_absolutefuture ) ;  
};

// ============================================================================
/** standard (default) constructor 
 *  @param since "since" time for validity range 
 *  @param till  "till"  time for validity range 
 */
// ============================================================================
SimpleValidity::SimpleValidity( const ITime& since ,
                                const ITime& till  ) 
  : m_since ( 0 ) 
  , m_till  ( 0 ) 
{
  m_since = new TimePoint (since) ;
  m_till  = new TimePoint (till) ;  
};

// ============================================================================
/** (explicit) constructor from other 
 *   IValidity object
 *   @param copy another IValidity object 
 */
// ============================================================================
SimpleValidity::SimpleValidity( const IValidity& copy )
  : m_since ( 0 ) 
  , m_till  ( 0 ) 
{
  IValidity* ival = const_cast<IValidity*> ( &copy );
  m_since = new TimePoint( ival->validSince () );
  m_till  = new TimePoint( ival->validTill  () );
};

// ============================================================================
/** copy constructor  (deep copy)
 *  @param copy another IValidity object 
 */
// ============================================================================
SimpleValidity::SimpleValidity( const SimpleValidity& copy )
  : IValidity(),
    m_since ( new TimePoint( *(copy.m_since) ) )
  , m_till  ( new TimePoint( *(copy.m_till)  ) )
{};

// ============================================================================
/** assignement from any IValidity object 
 *  @param copy another IValidity object 
 */
// ============================================================================
SimpleValidity& SimpleValidity::operator=( const IValidity&      copy )
{
  IValidity* im = static_cast<IValidity*> (this);
  if( &copy == im  ) { return *this; }                         ///< RETURN 
  /// reset contents  
  if( 0 != m_since ) { delete m_since ; m_since = 0 ; }
  if( 0 != m_till  ) { delete m_till  ; m_till  = 0 ; }
  /// deep copy the contents 
  IValidity* ival = const_cast<IValidity*> ( &copy );
  m_since = new TimePoint( ival->validSince () );
  m_till  = new TimePoint( ival->validTill  () );  
  ///
  return *this;
};

// ============================================================================
/** assignement (deep copy)
 *  @param copy another IValidity object 
 */
// ============================================================================
SimpleValidity& SimpleValidity::operator=( const SimpleValidity& copy )
{
  if( &copy == this ) { return *this ; }                       ///< RETURN
  /// reset contents  
  if( 0 != m_since ) { delete m_since ; m_since = 0 ; }
  if( 0 != m_till  ) { delete m_till  ; m_till  = 0 ; }
  /// deep copy the contents 
  m_since = new TimePoint( *(copy.m_since) );
  m_till  = new TimePoint( *(copy.m_till)  );
  ///
  return *this;
};

// ============================================================================
/** destructor 
 */
// ============================================================================
SimpleValidity::~SimpleValidity()
{
  /// reset contents  
  if( 0 != m_since ) { delete m_since ; m_since = 0 ; }
  if( 0 != m_till  ) { delete m_till  ; m_till  = 0 ; }  
};

// ============================================================================
/** is the Object valid? (it can be always invalid)
 *  @return true if object valid 
 */
// ============================================================================
bool SimpleValidity::isValid    () const
{ return (*m_since) <= (*m_till) ;}

// ============================================================================
/** is the Object valid for a given time?
 *  @param  time  time to be checked 
 *  @return true if objetc is valid for given time 
 */
// ============================================================================
bool SimpleValidity::isValid    
( const ITime&  time ) const
{ return ( (*m_since) <= time ) && ( time <= (*m_till) ) ; }

// ============================================================================
/// set the validity range of the Object
// ============================================================================
void SimpleValidity::setValidity 
( const ITime& since , 
  const ITime& till  ) 
{
  /// reset contents  
  if( 0 != m_since ) { delete m_since ; m_since = 0 ; }
  if( 0 != m_till  ) { delete m_till  ; m_till  = 0 ; }
  /// deep copy the contents 
  m_since = new TimePoint( since  );
  m_till  = new TimePoint( till   );  
};

// ============================================================================
/// set the validity time of the Object 
// ============================================================================
void SimpleValidity::setValiditySince  
( const ITime& since ) 
{
  /// reset contents  
  if( 0 != m_since ) { delete m_since ; m_since = 0 ; }
  /// deep copy the contents 
  m_since = new TimePoint( since  );
};

// ============================================================================
/// set the validity time of the Object 
// ============================================================================
void SimpleValidity::setValidityTill   
( const ITime& till  ) 
{
  /// reset contents  
  if( 0 != m_till  ) { delete m_till  ; m_till  = 0 ; }
  /// deep copy the contents 
  m_till  = new TimePoint( till   );  
};

// ============================================================================
// The End 
// ============================================================================

