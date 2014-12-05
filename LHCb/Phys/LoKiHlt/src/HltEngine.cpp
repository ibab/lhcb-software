// $Id: HltEngine.cpp,v 1.2 2008-09-23 13:13:35 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// local
// ============================================================================
#include "LoKi/HltEngine.h"
#include "LoKi/HltEngineActor.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hybrid::HltEngine
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-09-18
 */
// ============================================================================
// constructor
// ============================================================================
LoKi::Hybrid::HltEngine::HltEngine() {}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::HltEngine::~HltEngine() {}
// ============================================================================
namespace 
{
  template <class TYPE> 
  inline StatusCode _process 
  ( const std::string& name , const TYPE& cut )
  {
    LoKi::Hybrid::HltEngineActor& actor = LoKi::Hybrid::HltEngineActor::instance() ;
    return actor.process ( name , cut ) ;
  }
}
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::HltEngine::process
( const std::string&          name , 
  const LoKi::Types::L0_Cuts& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::HltEngine::process
( const std::string&          name , 
  const LoKi::Types::L0_Func& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::HltEngine::process
( const std::string&          name , 
  const LoKi::Types::ODIN_Cuts& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::HltEngine::process
( const std::string&          name , 
  const LoKi::Types::ODIN_Func& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::HltEngine::process
( const std::string&           name , 
  const LoKi::Types::HLT_Cuts& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::HltEngine::process
( const std::string&           name , 
  const LoKi::Types::HLT_Func& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// The END 
// ============================================================================
