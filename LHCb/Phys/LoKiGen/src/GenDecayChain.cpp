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
#include "LoKi/GenDecayChain.h"
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
 *  @param vertexe   end-vertex info for MC particles to be printed 
 *  @param mode      mode for printout of 4-vectors 
 *  @param fg        color for foreground for 'marked' lines 
 *  @param bg        color for background for 'marked' lines 
 */
// ============================================================================
LoKi::GenDecayChain::GenDecayChain
( const size_t          maxDepth , 
  const bool            vertex   , 
  const Mode            mode     ,
  const MSG::Color&     fg       ,
  const MSG::Color&     bg       ,
  const bool            vertexe  ) 
  : LoKi::DecayChainBase ( maxDepth , 
                           vertex   , 
                           mode     , 
                           fg       , 
                           bg       )
  , m_vertexe            ( vertexe  )
{}
// ==========================================================================
/*  Standard constructor
 *  @param vertexe   end-vertex info for MC particles to be printed 
 */
// ==========================================================================
LoKi::GenDecayChain::GenDecayChain
( const LoKi::DecayChainBase& base    , 
  const bool                  vertexe ) 
  : LoKi::DecayChainBase ( base    ) 
  , m_vertexe            ( vertexe )
{}  
// ===========================================================================
// virtual destructor 
// ===========================================================================
LoKi::GenDecayChain::~GenDecayChain () {}
// ===========================================================================
// predefined printout    
// ===========================================================================
std::string LoKi::GenDecayChain::print_
( const HepMC::GenParticle*   p      , 
  const LoKi::Types::GCuts&   accept , 
  const LoKi::Types::GCuts&   mark   ) const 
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
// ============================================================================
// predefined printout    
// ============================================================================
std::string LoKi::GenDecayChain::print_
( const LoKi::Types::GRange&  e      , 
  const LoKi::Types::GCuts&   accept , 
  const LoKi::Types::GCuts&   mark   ) const 
{
  std::ostringstream s ;
  //
  print ( e.begin () ,
          e.end   () , 
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
// predefined printout    
// ============================================================================
std::string LoKi::GenDecayChain::print_
( const LoKi::GenTypes::GenContainer& e      , 
  const LoKi::Types::GCuts&           accept , 
  const LoKi::Types::GCuts&           mark   ) const 
{
  std::ostringstream s ;
  //
  print ( e.begin () ,
          e.end   () , 
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
std::string LoKi::GenDecayChain::print_
( const HepMC::GenEvent*      p      , 
  const LoKi::Types::GCuts&   accept , 
  const LoKi::Types::GCuts&   mark   ) const 
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
std::string LoKi::GenDecayChain::print_
( const LHCb::HepMCEvent*     p      , 
  const LoKi::Types::GCuts&   accept , 
  const LoKi::Types::GCuts&   mark   ) const 
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
std::string LoKi::GenDecayChain::print_
( const LHCb::HepMCEvent::Container* p      , 
  const LoKi::Types::GCuts&          accept , 
  const LoKi::Types::GCuts&          mark   ) const 
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
// The END 
// ===========================================================================

