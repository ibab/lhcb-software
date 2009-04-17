// $Id: SimpleValidity.cpp,v 1.9 2009-04-17 08:54:24 cattanem Exp $
// ============================================================================
// Include files
// GaudiKernel
#include "GaudiKernel/Time.h"
#include "GaudiKernel/StatusCode.h"

#include "DetDesc/SimpleValidity.h"

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
  : m_since ( Gaudi::Time::epoch() ) 
  , m_till  ( Gaudi::Time::max() ) 
{}

// ============================================================================
/** standard (default) constructor 
 *  @param since "since" time for validity range 
 *  @param till  "till"  time for validity range 
 */
// ============================================================================
SimpleValidity::SimpleValidity( const Gaudi::Time& since ,
                                const Gaudi::Time& till  ) 
  : m_since ( since ) 
  , m_till  ( till ) 
{}

// ============================================================================
/** (explicit) constructor from other 
 *   IValidity object
 *   @param copy another IValidity object 
 */
// ============================================================================
SimpleValidity::SimpleValidity( const IValidity& copy )
  : m_since ( copy.validSince() ) 
  , m_till  ( copy.validTill() ) 
{}

// ============================================================================
/** copy constructor  (deep copy)
 *  @param copy another IValidity object 
 */
// ============================================================================
SimpleValidity::SimpleValidity( const SimpleValidity& copy )
  : IValidity()
  , m_since ( copy.m_since )
  , m_till  ( copy.m_till  )
{}

// ============================================================================
/** assignement from any IValidity object 
 *  @param copy another IValidity object 
 */
// ============================================================================
SimpleValidity& SimpleValidity::operator=( const IValidity&      copy )
{
  m_since = copy.validSince();
  m_till  = copy.validTill();
  return *this;
}

// ============================================================================
/** assignement (deep copy)
 *  @param copy another IValidity object 
 */
// ============================================================================
SimpleValidity& SimpleValidity::operator=( const SimpleValidity& copy )
{
  if( &copy == this ) { return *this ; }                       ///< RETURN
  m_since = copy.m_since;
  m_till  = copy.m_till;
  return *this;
}

// ============================================================================
/** destructor 
 */
// ============================================================================
SimpleValidity::~SimpleValidity() {}

// ============================================================================
/** is the Object valid? (it can be always invalid)
 *  @return true if object valid 
 */
// ============================================================================
bool SimpleValidity::isValid    () const
{ return (m_since) <= (m_till) ;}

// ============================================================================
/** is the Object valid for a given time?
 *  @param  time  time to be checked 
 *  @return true if objetc is valid for given time 
 */
// ============================================================================
bool SimpleValidity::isValid    
( const Gaudi::Time&  time ) const
{ return ( (m_since) <= time ) && ( time <= (m_till) ) ; }

// ============================================================================
/// set the validity range of the Object
// ============================================================================
void SimpleValidity::setValidity 
( const Gaudi::Time& since , 
  const Gaudi::Time& till  ) 
{
  m_since = since;
  m_till = till;
}

// ============================================================================
/// set the validity time of the Object 
// ============================================================================
void SimpleValidity::setValiditySince  
( const Gaudi::Time& since ) 
{
  m_since = since;
}

// ============================================================================
/// set the validity time of the Object 
// ============================================================================
void SimpleValidity::setValidityTill   
( const Gaudi::Time& till  ) 
{
  m_till = till;  
}

// ============================================================================
// The End 
// ============================================================================

