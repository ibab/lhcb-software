// $Id: GenHybridEngine.cpp,v 1.2 2008-02-19 13:16:25 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// local
// ============================================================================
#include "LoKi/GenHybridEngine.h"
#include "LoKi/GenHybridEngineActor.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hybrid::GenEngine
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
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2004-06-30 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
// constructor
// ============================================================================
LoKi::Hybrid::GenEngine::GenEngine() {}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::GenEngine::~GenEngine() {}
// ============================================================================
namespace 
{
  template <class TYPE> 
  inline StatusCode _process 
  ( const std::string& name , const TYPE& cut )
  {
    LoKi::Hybrid::GenEngineActor& actor = LoKi::Hybrid::GenEngineActor::instance() ;
    return actor.process ( name , cut ) ;
  }
}
// ============================================================================
// predicates 
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::GenEngine::process
( const std::string&          name , 
  const LoKi::Types::GCuts&   cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::GenEngine::process
( const std::string&          name , 
  const LoKi::Types::GVCuts&  cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// functions 
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::GenEngine::process
( const std::string&          name , 
  const LoKi::Types::GFunc&   cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::GenEngine::process
( const std::string&          name , 
  const LoKi::Types::GVFunc&  cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// maps 
// ============================================================================
// add the map
// ============================================================================
StatusCode LoKi::Hybrid::GenEngine::process
( const std::string&          name , 
  const LoKi::Types::GMaps&   cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the map 
// ============================================================================
StatusCode LoKi::Hybrid::GenEngine::process
( const std::string&          name , 
  const LoKi::Types::GVMaps&  cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// pipe 
// ============================================================================
// add the pipe
// ============================================================================
StatusCode LoKi::Hybrid::GenEngine::process
( const std::string&          name , 
  const LoKi::Types::GPipes&  cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the pipe
// ============================================================================
StatusCode LoKi::Hybrid::GenEngine::process
( const std::string&          name , 
  const LoKi::Types::GVPipes& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// fun-vals
// ============================================================================
// add the fun-val
// ============================================================================
StatusCode LoKi::Hybrid::GenEngine::process
( const std::string&           name , 
  const LoKi::Types::GFunVals& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the fun-val
// ============================================================================
StatusCode LoKi::Hybrid::GenEngine::process
( const std::string&            name , 
  const LoKi::Types::GVFunVals& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// sources 
// ============================================================================
// add the source 
// ============================================================================
StatusCode LoKi::Hybrid::GenEngine::process
( const std::string&          name , 
  const LoKi::Types::GSources& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the source
// ============================================================================
StatusCode LoKi::Hybrid::GenEngine::process
( const std::string&            name , 
  const LoKi::Types::GVSources& cut  ) const { return _process ( name , cut ) ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
