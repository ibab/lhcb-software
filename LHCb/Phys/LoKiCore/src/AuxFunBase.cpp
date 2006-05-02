// $Id: AuxFunBase.cpp,v 1.3 2006-05-02 14:29:10 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
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
// ============================================================================

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

// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @var s_AuxFunBaseCounter 
   *  statsic instace counter for all functions 
   */
  // ==========================================================================
#ifdef LOKI_DEBUG
  static InstanceCounter<AuxFunBase>  s_AuxFunBaseCounter  ;
#endif 
  // ==========================================================================
};
// ============================================================================

// ============================================================================
/// default constructor 
// ============================================================================
LoKi::AuxFunBase::AuxFunBase() 
{
#ifdef LOKI_DEBUG
  // increment the instance counter
  LoKi::s_AuxFunBaseCounter.increment();
#endif 
};
// ============================================================================

// ============================================================================
/// copy constructor 
// ============================================================================
LoKi::AuxFunBase::AuxFunBase( const AuxFunBase& /* func */ ) 
{
#ifdef LOKI_DEBUG
  // increment the instance counter
  LoKi::s_AuxFunBaseCounter.increment();
#endif 
  LoKi::Welcome::instance() ;
};
// ============================================================================

// ============================================================================
/// destructor  
// ============================================================================
LoKi::AuxFunBase::~AuxFunBase() 
{
#ifdef LOKI_DEBUG
  // decrement the instance counter
  LoKi::s_AuxFunBaseCounter.decrement();
#endif 
};
// ============================================================================

// ============================================================================
/** print error message 
 *  @param msg  error message 
 *  @param sc   status code 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::AuxFunBase::Error
( const std::string& msg , 
  const StatusCode&  sc  ) const 
{
  return LoKi::Report::Error( objType() + ": \t" + msg  , sc );
};
// ============================================================================

// ============================================================================
/** print Warning message 
 *  @param msg  error message 
 *  @param sc   status code 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::AuxFunBase::Warning
( const std::string& msg , 
  const StatusCode&  sc  ) const 
{
  return LoKi::Report::Warning( objType() + ": \t " + msg , sc );
};
// ============================================================================

// ============================================================================
/** thrown an exception  
 *  @param msg  error message 
 *  @param sc   status code 
 *  @return status code (fictive)
 */
// ============================================================================
StatusCode LoKi::AuxFunBase::Exception
( const std::string& msg , 
  const StatusCode&  sc  ) const 
{
  LoKi::Report::Error( objType() + " *EXCEPTION* : \t" + msg  , sc ) ;
  //
  throw LoKi::Exception( objType() + ": \t" + msg , sc ) ;
  //
  return sc ;
};
// ============================================================================

// ============================================================================
/** (virtual) printout to std::ostream 
 *  @param s output stream 
 *  @restur reference to teh stream 
 */
// ============================================================================
std::ostream& LoKi::AuxFunBase::fillStream( std::ostream& s ) const 
{
  s << objType() ;
  return s ;
};
// ============================================================================

// ============================================================================
/** (virtual) printout in form of std::string 
 *  @restur outptu string 
 */
// ============================================================================
std::string LoKi::AuxFunBase::printOut() const 
{
  std::ostringstream s ;
  fillStream( s ) ;
  return s.str() ;
};
// ============================================================================

// ============================================================================
std::string LoKi::AuxFunBase::objType () const 
{ return System::typeinfoName( typeid( *this ) ) ; } 
// ============================================================================

// ============================================================================
/** output operator of function objects to std::ostream 
 *  @param stream refeence to the stream
 *  @param obj    object to be printed 
 *  @return reference to the stream
 */
// ============================================================================
std::ostream& operator<<( std::ostream&           stream , 
                          const LoKi::AuxFunBase& obj    ) 
{ return obj.fillStream( stream ) ; } ;
// ============================================================================

// ============================================================================
/** output operator of function objects to MsgStream
 *  @param stream refeence to the stream
 *  @param obj    object to be printed 
 *  @return reference to the stream
 */
// ============================================================================
MsgStream&    operator<<( MsgStream&              stream , 
                          const LoKi::AuxFunBase& obj    ) 
{ 
  if ( stream.isActive() ) { obj.fillStream( stream.stream() ) ; }
  return stream ;
} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
