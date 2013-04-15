// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
#include <sstream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/System.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Hash.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/InstanceCounter.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/AuxFunBase.h"
#include "LoKi/Report.h"
#include "LoKi/Welcome.h"
#include "LoKi/Exception.h"
#include "LoKi/shifts.h"
#include "LoKi/Services.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/functional/hash.hpp"
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::AuxFunBase
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
namespace
{
  // ==========================================================================
  /// static instance counter for all functions
  // ==========================================================================
#ifdef LOKI_DEBUG
  static InstanceCounter<LoKi::AuxFunBase>  s_AuxFunBaseCounter  ;
#endif
  // ==========================================================================
  /// LoKi service
  // ==========================================================================
  LoKi::Interface<LoKi::ILoKiSvc> s_lokiSvc = LoKi::Interface<LoKi::ILoKiSvc>() ;
  // ========================================================================
}
// ============================================================================
// STATIC: get LoKi service
// ============================================================================
const LoKi::Interface<LoKi::ILoKiSvc>& LoKi::AuxFunBase::lokiSvc ()
{ return s_lokiSvc ; }
// ============================================================================
// STATIC: set LoKi service
// ============================================================================
bool  LoKi::AuxFunBase::setLoKiSvc
( const LoKi::ILoKiSvc*  svc )
{
  s_lokiSvc = svc ;
  return s_lokiSvc.validPointer() ;
}
// ============================================================================
// STATIC: set LoKi service
// ============================================================================
bool LoKi::AuxFunBase::setLoKiSvc
( const LoKi::Interface<LoKi::ILoKiSvc>& svc )
{
  s_lokiSvc = svc ;
  return s_lokiSvc.validPointer() ;
}
// ============================================================================
// constructor from LoKi Service
// ============================================================================
LoKi::AuxFunBase::AuxFunBase ()
  : m_event   ( 0 )
{
#ifdef LOKI_DEBUG
  // increment the instance counter
  LoKi::s_AuxFunBaseCounter.increment();
#endif
}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::AuxFunBase::AuxFunBase( const AuxFunBase& func  )
  : m_event   ( func.m_event   )
{
#ifdef LOKI_DEBUG
  // increment the instance counter
  LoKi::s_AuxFunBaseCounter.increment();
#endif
  LoKi::Welcome::instance() ;
}
// ============================================================================
// destructor
// ============================================================================
LoKi::AuxFunBase::~AuxFunBase()
{
#ifdef LOKI_DEBUG
  // decrement the instance counter
  LoKi::s_AuxFunBaseCounter.decrement();
#endif
}
// ============================================================================
/*  print error message
 *  @param msg  error message
 *  @param sc   status code
 *  @return status code
 */
// ============================================================================
StatusCode LoKi::AuxFunBase::Error
( const std::string& msg ,
  const StatusCode&  sc  ,
  int mxprint            ) const
{
  sc.ignore() ;
  return LoKi::Report::Error ( objType() + ": \t" + msg  , sc, mxprint ) ;
}
// ============================================================================
/*  print Warning message
 *  @param msg  error message
 *  @param sc   status code
 *  @return status code
 */
// ============================================================================
StatusCode LoKi::AuxFunBase::Warning
( const std::string& msg ,
  const StatusCode&  sc  ,
  int mxprint            ) const
{
  sc.ignore() ;
  return LoKi::Report::Warning ( objType() + ": \t " + msg , sc, mxprint ) ;
}
// ============================================================================
/*  thrown an exception
 *  @param msg  error message
 *  @param sc   status code
 */
// ============================================================================
void LoKi::AuxFunBase::Exception
( const std::string& msg ,
  const StatusCode&  sc  ) const
{
  sc.ignore() ;
  //
  LoKi::Report::Error ( objType() + " *EXCEPTION* : \t" + msg  , sc ) ;
  //
  throw LoKi::Exception ( objType() + ": \t" + msg , sc ) ;
  //
}
// ============================================================================
/*  (virtual) printout to std::ostream
 *  @param s output stream
 *  @restur reference to teh stream
 */
// ============================================================================
std::ostream& LoKi::AuxFunBase::fillStream( std::ostream& s ) const
{
  s << objType() ;
  return s ;
}
// ============================================================================
/*  (virtual) printout in form of std::string
 *  @return outptut string
 */
// ============================================================================
std::string LoKi::AuxFunBase::printOut() const
{
  std::ostringstream s ;
  fillStream( s ) ;
  return s.str() ;
}
// ============================================================================
// the actual object type
// ============================================================================
std::string LoKi::AuxFunBase::objType () const
{ return System::typeinfoName ( typeid( *this ) ) ; }
// ============================================================================
/// unique function ID (hash); see LoKi::genericID
// ============================================================================
std::size_t LoKi::AuxFunBase::id () const
{ return LoKi::genericID ( *this ) ; }
// ============================================================================
/*  simple fuction to generate the default generic
 *  (hopefully unique?) ID for the functor
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
std::size_t LoKi::genericID ( const LoKi::AuxFunBase& o )
{
  // Use Boost::hash
  std::size_t _id = boost::hash_value ( o.printOut() ) ;
  // make a cyclic shift to 12 bits left
  return LoKi::Shifts::cyclicShiftLeft ( _id , 12 ) ;
}
// ============================================================================
// set the event-ID from LoKi service
// ============================================================================
void LoKi::AuxFunBase::setEvent (          ) const
{
  LoKi::ILoKiSvc* svc = lokiSvc() ;
  setEvent ( 0 ) ;
  //
  if ( 0 != svc ) { m_event = svc->event() ; }
  else { Error ( "setEvent(): invalid pointer to LoKi::ILoKiSvc, set 0") ; }
}
// ============================================================================
// check the data for the same event
// ============================================================================
bool LoKi::AuxFunBase::sameEvent () const
{
  if ( 0 == m_event ) { return  false ; }                    // RETURN
  // get the service
  LoKi::ILoKiSvc* svc = lokiSvc() ;
  if ( 0 == svc )
  {
    Warning ( "sameEvent(): could not check the event, return false ") ;
    return false ;                                          // RETURN
  }
  return svc->event() == m_event ;
}
// ============================================================================
/*  output operator of function objects to std::ostream
 *  @param stream reference to the stream
 *  @param obj object to be printed
 *  @return reference to the stream
 */
// ============================================================================
std::ostream& LoKi::operator<<
  ( std::ostream&      stream ,
    const AuxFunBase&  obj    ) { return obj.fillStream ( stream ) ; }
// ============================================================================
/*  output operator of function objects to std::ostream
 *  @param stream reference to the stream
 *  @param obj object to be printed
 *  @return reference to the stream
 */
// ============================================================================
MsgStream& LoKi::operator<<
  ( MsgStream&         stream ,
    const AuxFunBase&  obj    )
{
  if ( stream.isActive() ) { obj.fillStream( stream.stream() ) ; }
  return stream ;
}
// ============================================================================
// print it!
// ============================================================================
std::string Gaudi::Utils::toString ( const LoKi::AuxFunBase& o )
{ return o.printOut () ; }
// ============================================================================
// print it!
// ============================================================================
std::ostream& Gaudi::Utils::toStream
( const LoKi::AuxFunBase& o ,
  std::ostream&           s ) { return o.fillStream( s ) ; }
// ============================================================================
// The END
// ============================================================================
