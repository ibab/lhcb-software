// $Id: PrintMCDecay.cpp,v 1.10 2007-08-16 13:51:41 ibelyaev Exp $
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
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// MCEvent
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ParticleProperties.h"
#include "LoKi/Primitives.h"
#include "LoKi/PrintMCDecay.h"
#include "LoKi/MCExtract.h"
#include "LoKi/Report.h"
// ============================================================================
/** @file
 *
 *  Implementation file for the function printMCDecay
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
// Simple function to print MC decay in more or less "readable" format 
// ============================================================================
MsgStream& LoKi::PrintMC::printDecay 
( const LHCb::MCParticle*      particle  , 
  MsgStream&                   stream    , 
  const LoKi::MCTypes::MCCuts& cut       , 
  const bool                   decayOnly ,
  const int                    level     , 
  const std::string&           blank     ) 
{
  if ( stream.isActive() ) 
  { LoKi::PrintMC::printDecay 
      ( particle , stream.stream() , cut , decayOnly ,level , blank ) ; }
  return stream ;
}
// ============================================================================
// Simple function to print MC decay in more or less "readable" format 
// ============================================================================
std::string LoKi::PrintMC::printDecay 
( const LHCb::MCParticle*      particle  , 
  const LoKi::MCTypes::MCCuts& cut       , 
  const bool                   decayOnly ,
  const int                    level     , 
  const std::string&           blank     ) 
{
  std::ostringstream stream ;
  LoKi::PrintMC::printDecay 
    ( particle , stream , cut , decayOnly , level , blank ) ;
  return stream.str() ;
} 
// ============================================================================
//  Simple function to print MC decay in more or less "readable" format 
// ============================================================================
std::ostream& LoKi::PrintMC::printDecay 
( const LHCb::MCParticle*      particle  , 
  std::ostream&                stream    , 
  const LoKi::MCTypes::MCCuts& cut       ,
  const bool                   decayOnly , 
  const int                    level     , 
  const std::string&           blank     ) 
{
  if ( 0 == particle    ) 
  { 
    LoKi::Report::Warning ( "LoKi::printMCDecay, invalid particle" ) ; 
    return stream << " <NULL> " ;                           // RETURN 
  }
  //
  if ( !cut( particle ) ) { return stream << blank ; } ;    // RETURN 
  //
  typedef std::vector<const LHCb::MCParticle*> MCPs ;
  MCPs daugs ;
  LoKi::Extract::getMCChildren 
    ( particle , std::back_inserter( daugs ) , decayOnly ) ;
  // own name 
  const std::string name = 
    LoKi::Particles::nameFromPID ( particle->particleID() ) ;
  if ( daugs.empty() ) 
  { return stream << " " << name << " " ; } ;               // RETURN 
  // 
  if ( 0 >= level    ) { return stream << " " << name << " " ; } ; // RETURN 
  // print the decay 
  stream << " ( " << name << " ->" ;
  for ( MCPs::const_iterator id = daugs.begin() ; 
        daugs.end() != id ; ++id ) 
  { LoKi::PrintMC::printDecay 
      ( *id , stream , cut , decayOnly , level - 1 , blank ) ; }   // RECURSION
  //
  return stream << " ) " ;
} 
// ============================================================================
//  Simple function to print MC decay in more or less "readable" format 
// ========================================================================
std::ostream&
LoKi::PrintMC::printDecay 
( const LHCb::MCParticle* particle  , 
  std::ostream&           stream    ,
  const bool              decayOnly ) 
{
  return LoKi::PrintMC::printDecay 
    ( particle , 
      stream   ,
      LoKi::BooleanConstant<const  LHCb::MCParticle*> ( true ) , 
      decayOnly ) ;
}
// ========================================================================
// Simple function to print MC decay in more or less "readable" format 
// ============================================================================
MsgStream& 
LoKi::PrintMC::printDecay 
( const LHCb::MCParticle* particle  , 
  MsgStream&              stream    ,
  const bool              decayOnly ) 
{
  return LoKi::PrintMC::printDecay 
    ( particle , 
      stream   ,
      LoKi::BooleanConstant<const  LHCb::MCParticle*> ( true ) , 
      decayOnly ) ;
}
// ========================================================================
//  Simple function to print MC decay in more or less "readable" format 
// ============================================================================
std::string 
LoKi::PrintMC::printDecay 
( const LHCb::MCParticle* particle  ,
  const bool              decayOnly ) 
{
  return LoKi::PrintMC::printDecay 
    ( particle , 
      LoKi::BooleanConstant<const  LHCb::MCParticle*> ( true ) , decayOnly ) ;
}
// ============================================================================
// The END 
// ============================================================================
