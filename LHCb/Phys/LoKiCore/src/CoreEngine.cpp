// $Id$
// ============================================================================
// Include files
// ============================================================================
// local
// ============================================================================
#include "LoKi/CoreEngine.h"
#include "LoKi/CoreEngineActor.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hybrid::CoreEngine
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
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-09-18
 */
// ============================================================================
// constructor
// ============================================================================
LoKi::Hybrid::CoreEngine::CoreEngine() {}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::CoreEngine::~CoreEngine() {}
// ============================================================================
namespace 
{
  template <class TYPE> 
  inline StatusCode _process 
  ( const std::string& name , const TYPE& cut )
  {
    LoKi::Hybrid::CoreEngineActor& actor = 
      LoKi::Hybrid::CoreEngineActor::instance() ;
    return actor.process ( name , cut ) ;
  }
}
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngine::process
( const std::string&        name , 
  const LoKi::Types::FCuts& cut  ) const 
{ return _process ( name , cut ) ; }
// ============================================================================
//  propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngine::process
( const std::string&        name , 
  const LoKi::Types::FFunc& cut  ) const 
{ return _process ( name , cut ) ; }
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngine::process
( const std::string&        name , 
  const LoKi::Types::XCuts& cut  ) const 
{ return _process ( name , cut ) ; }
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngine::process
( const std::string&        name , 
  const LoKi::Types::XFunc& cut  ) const 
{ return _process ( name , cut ) ; }
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngine::process
( const std::string&           name , 
  const LoKi::Types::XMaps&    cut  ) const 
{ return _process ( name , cut ) ; }
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngine::process
( const std::string&           name , 
  const LoKi::Types::XFunVals& cut  ) const 
{ return _process ( name , cut ) ; }
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngine::process
( const std::string&           name , 
  const LoKi::Types::XSources& cut  ) const 
{ return _process ( name , cut ) ; }
// ============================================================================
// The END
// ============================================================================
