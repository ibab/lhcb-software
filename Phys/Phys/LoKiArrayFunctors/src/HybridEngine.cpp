// $Id: HybridEngine.cpp,v 1.1 2007-07-26 13:25:09 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// local
// ============================================================================
#include "LoKi/HybridEngine.h"
#include "LoKi/HybridEngineActor.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hybrid::Engine
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
 *  @date 2004-06-30 
 */
// ============================================================================
// constructor
// ============================================================================
LoKi::Hybrid::Engine::Engine() {}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::Engine::~Engine() {}
// ============================================================================
namespace 
{
  template <class TYPE> 
  inline StatusCode _process 
  ( const std::string& name , const TYPE& cut )
  {
    LoKi::Hybrid::EngineActor& actor = LoKi::Hybrid::EngineActor::instance() ;
    return actor.process ( name , cut ) ;
  } ;
}
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process 
( const std::string&          name , 
  const LoKi::Types::Cuts&    cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::VCuts&   cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::ACuts&   cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::Func&    cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::VFunc&   cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::Engine::process
( const std::string&          name , 
  const LoKi::Types::AFunc&   cut  ) const { return _process ( name , cut ) ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
