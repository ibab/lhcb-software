// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <sstream>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/DecayChain.h"
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
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2011-06-03
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
/*  Standard constructor
 *  @param maxDepth  maximal decay depth 
 *  @param vertex    vertex info to be printed 
 *  @param mode      mode for printout of 4-vectors 
 *  @param fg        color for foreground for 'marked' lines 
 *  @param bg        color for background for 'marked' lines 
 */
// ============================================================================
LoKi::DecayChain::DecayChain
( const size_t          maxDepth , 
  const bool            vertex   , 
  const Mode            mode     ,
  const MSG::Color&     fg       ,
  const MSG::Color&     bg       ) 
  : LoKi::DecayChainBase ( maxDepth , 
                           vertex   , 
                           mode     , 
                           fg       , 
                           bg       ) 
{}
// ==========================================================================
/*  Standard constructor
 *  @param vertexe   end-vertex info for MC particles to be printed 
 */
// ==========================================================================
LoKi::DecayChain::DecayChain ( const LoKi::DecayChainBase& base ) 
  : LoKi::DecayChainBase ( base    ) 
{}  
// ===========================================================================
// virtual destructor 
// ===========================================================================
LoKi::DecayChain::~DecayChain () {}
// ===========================================================================
// predefined printout    
// ===========================================================================
std::string LoKi::DecayChain::print_
( const LHCb::Particle*       p      , 
  const LoKi::Types::Cuts&    accept , 
  const LoKi::Types::Cuts&    mark   ) const 
{
  std::ostringstream s ;
  //
  print ( p      , 
          s      , 
          '\n'   , 
          accept ,
          mark   ) ;
  //
  return s.str() ;
}
// ===========================================================================
// predefined printout    
// ===========================================================================
std::string LoKi::DecayChain::print_
( const LHCb::Particle::Container* p      , 
  const LoKi::Types::Cuts&         accept , 
  const LoKi::Types::Cuts&         mark   ) const 
{
  //
  if ( 0 == p )
  {
    Error ( " LHCb::Particle::Container* points to NULL" ) ;
    return "" ; 
  }
  //
  std::ostringstream s ;
  //
  print ( p -> begin () ,
          p -> end   () , 
          s             , 
          '\n'          , 
          accept        ,
          mark          , 
          " "           , 
          0             ) ;
  //
  return s.str() ;
}
// ===========================================================================
// predefined printout    
// ===========================================================================
std::string LoKi::DecayChain::print_
( const LHCb::Particle::Selection* p      , 
  const LoKi::Types::Cuts&         accept , 
  const LoKi::Types::Cuts&         mark   ) const 
{
  //
  if ( 0 == p )
  {
    Error ( " LHCb::Particle::Selection* points to NULL" ) ;
    return "" ; 
  }
  //
  std::ostringstream s ;
  //
  print ( p -> begin () ,
          p -> end   () , 
          s             , 
          '\n'          , 
          accept        ,
          mark          , 
          " "           , 
          0             ) ;
  //
  return s.str() ;
}
// ===========================================================================
// predefined printout    
// ===========================================================================
std::string LoKi::DecayChain::print_
( const LHCb::Particle::ConstVector& p      , 
  const LoKi::Types::Cuts&           accept , 
  const LoKi::Types::Cuts&           mark   ) const 
{
  //
  std::ostringstream s ;
  //
  print ( p.begin () ,
          p.end   () ,
          s          , 
          '\n'       , 
          accept     ,
          mark       , 
          " "        , 
          0          ) ;
  //
  return s.str() ;
}
// ===========================================================================
// predefined printout    
// ===========================================================================
std::string LoKi::DecayChain::print_
( const LHCb::Particle::Range&       p      , 
  const LoKi::Types::Cuts&           accept , 
  const LoKi::Types::Cuts&           mark   ) const 
{
  //
  std::ostringstream s ;
  //
  print ( p.begin () ,
          p.end   () ,
          s          , 
          '\n'       , 
          accept     ,
          mark       , 
          " "        , 
          0          ) ;
  //
  return s.str() ;
}
// ============================================================================
// The END 
// ============================================================================
