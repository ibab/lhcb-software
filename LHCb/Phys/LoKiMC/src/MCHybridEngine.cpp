// $Id: MCHybridEngine.cpp,v 1.1 2007-07-25 15:16:07 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// local
// ============================================================================
#include "LoKi/MCHybridEngine.h"
#include "LoKi/MCHybridEngineActor.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::MCHybridEngine
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
LoKi::Hybrid::MCEngine::MCEngine() {}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::MCEngine::~MCEngine() {}
// ============================================================================
namespace 
{
  template <class TYPE> 
  inline StatusCode _process 
  ( const std::string& name , const TYPE& cut )
  {
    LoKi::Hybrid::MCEngineActor& actor = LoKi::Hybrid::MCEngineActor::instance() ;
    return actor.process ( name , cut ) ;
  } ;
}
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::MCEngine::process 
( const std::string&          name , 
  const LoKi::Types::MCCuts&  cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::MCEngine::process
( const std::string&          name , 
  const LoKi::Types::MCVCuts& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::MCEngine::process
( const std::string&          name , 
  const LoKi::Types::MCFunc&  cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::MCEngine::process
( const std::string&          name , 
  const LoKi::Types::MCVFunc& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
