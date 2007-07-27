// $Id: TrHybridEngine.cpp,v 1.1.1.1 2007-07-27 15:56:42 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// local
// ============================================================================
#include "LoKi/TrHybridEngine.h"
#include "LoKi/TrHybridEngineActor.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hybrid::TrEngine
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
LoKi::Hybrid::TrEngine::TrEngine() {}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::TrEngine::~TrEngine() {}
// ============================================================================
namespace 
{
  template <class TYPE> 
  inline StatusCode _process 
  ( const std::string& name , const TYPE& cut )
  {
    LoKi::Hybrid::TrEngineActor& actor = LoKi::Hybrid::TrEngineActor::instance() ;
    return actor.process ( name , cut ) ;
  } ;
}
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngine::process
( const std::string&          name , 
  const LoKi::Types::TrCuts&  cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngine::process
( const std::string&          name , 
  const LoKi::Types::RVCuts&  cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngine::process
( const std::string&          name , 
  const LoKi::Types::TTrCuts& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngine::process
( const std::string&          name , 
  const LoKi::Types::TrVCuts& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngine::process
( const std::string&          name , 
  const LoKi::Types::RVVCuts& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngine::process
( const std::string&          name , 
  const LoKi::Types::TrFunc&  func ) const { return _process ( name , func ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngine::process
( const std::string&          name , 
  const LoKi::Types::RVFunc&  func ) const { return _process ( name , func ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngine::process
( const std::string&          name , 
  const LoKi::Types::TTrFunc& func ) const { return _process ( name , func ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngine::process
( const std::string&          name , 
  const LoKi::Types::TrVFunc& func ) const { return _process ( name , func ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngine::process
( const std::string&          name , 
  const LoKi::Types::RVVFunc& func ) const { return _process ( name , func ) ; }
// ============================================================================
// get the reference tracks    ( for Trigger/Hlt)
// ============================================================================
std::vector<LHCb::Track*>*     
LoKi::Hybrid::TrEngine::tracks   () const 
{
  const LoKi::Hybrid::TrEngineActor& actor = 
    LoKi::Hybrid::TrEngineActor::instance() ;
  return actor.tracks () ;
}
// ============================================================================
// get the reference vertices  ( for Trigger/Hlt)
// ============================================================================
std::vector<LHCb::RecVertex*>* 
LoKi::Hybrid::TrEngine::vertices () const 
{
  const LoKi::Hybrid::TrEngineActor& actor = 
    LoKi::Hybrid::TrEngineActor::instance() ;
  return actor.vertices () ;
}
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
