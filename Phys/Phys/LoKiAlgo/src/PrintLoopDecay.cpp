// $Id: PrintLoopDecay.cpp,v 1.3 2007-11-28 14:54:41 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
#include <sstream>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoopObj.h"
#include "LoKi/Loop.h"
#include "LoKi/Report.h"
#include "LoKi/LoopChild.h"
#include "LoKi/PrintDecay.h"
#include "LoKi/PrintLoopDecay.h"
#include "LoKi/MoreFunctions.h"
// ============================================================================
/** @file 
 *  Implementation file for functions LoKi::Print::printLoopDecay
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-08-16
 */
// ============================================================================
// Simple function to print decay in more or less "readable" format 
// ============================================================================
MsgStream& LoKi::PrintLoop::printDecay 
( const LoKi::Loop&            particle , 
  MsgStream&                   stream   , 
  const LoKi::Types::Cuts&     cut      ,
  const int                    level    ,
  const std::string&           blank    ) 
{
  if ( stream.isActive() ) 
  { LoKi::PrintLoop::printDecay 
      ( particle , stream.stream() , cut , level , blank ) ; }
  return stream ;
} 
// ============================================================================
// Simple function to print decay in more or less "readable" format 
// ============================================================================
std::string LoKi::PrintLoop::printDecay 
( const LoKi::Loop&            particle , 
  const LoKi::Types::Cuts&     cut      ,
  const int                    level    ,
  const std::string&           blank    ) 
{
  std::ostringstream stream ;
  LoKi::PrintLoop::printDecay ( particle , stream, cut , level , blank ) ;
  return stream.str() ;
} 
// ============================================================================
// Simple function to print decay in more or less "readable" format 
// ============================================================================
std::ostream& LoKi::PrintLoop::printDecay 
( const LoKi::Loop&            particle , 
  std::ostream&                stream   , 
  const LoKi::Types::Cuts&     cut      ,
  const int                    level    ,
  const std::string&           blank    ) 
{
  if ( !particle ) 
  { 
    LoKi::Report::Warning ( "LoKi::printDecay, invalid particle" ) ; 
    return stream << " <NULL> " ;                                 // RETURN 
  }
  //
  if ( !cut( particle ) ) { return stream << blank ; } ;          // RETURN 
  //
  // own name 
  const std::string& name = particle->pidName() ;
  const LHCb::Particle::ConstVector& daugs = 
    LoKi::LoopChild::children ( particle ) ;
  if ( daugs.empty() ) { return stream << " " << name << " " ; } // RETURN 
  // too many levels ?
  if ( 0 >= level    ) { return stream << " " << name << " " ; } // RETURN 
  // print the decay 
  stream << " ( " << name << " -> " ;
  for ( LHCb::Particle::ConstVector::const_iterator id = daugs.begin() ; 
        daugs.end() != id ; ++id ) 
  { LoKi::PrintPhys::printDecay 
      ( *id , stream , cut , level - 1 , blank ) ; }            // RECURSION
  //  
  return stream << " ) " ;                                      // RETURN 
} 
// ============================================================================
// Simple function to print decay in more or less "readable" format 
// ========================================================================    
std::ostream& LoKi::PrintLoop::printDecay 
( const LoKi::Loop&            particle , 
  std::ostream&                stream   )
{
  return LoKi::PrintLoop::printDecay 
    ( particle , stream , 
      LoKi::Constant<const  LHCb::Particle*,bool> ( true ) ) ;
}
// ========================================================================    
//  Simple function to print decay in more or less "readable" format 
// ========================================================================
MsgStream& LoKi::PrintLoop::printDecay 
( const LoKi::Loop&            particle , 
  MsgStream&                   stream   ) 
{
  return LoKi::PrintLoop::printDecay 
    ( particle , stream , 
      LoKi::Constant<const  LHCb::Particle*,bool> ( true ) ) ;
}
// ========================================================================
//  Simple function to print decay in more or less "readable" format 
// ========================================================================
std::string LoKi::PrintLoop::printDecay 
( const LoKi::Loop&            particle )
{
  return LoKi::PrintLoop::printDecay 
    ( particle , LoKi::Constant<const  LHCb::Particle*,bool> ( true ) ) ;
}
// ============================================================================
// The END 
// ============================================================================
