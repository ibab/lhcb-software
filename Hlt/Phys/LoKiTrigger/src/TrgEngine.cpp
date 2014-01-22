// $Id$
// ============================================================================
// Include files
// ============================================================================
// local
// ============================================================================
#include "LoKi/TrgEngine.h"
#include "LoKi/TrgEngineActor.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hybrid::TrgEngine
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
 *
 *  $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
// ============================================================================
// constructor
// ============================================================================
LoKi::Hybrid::TrgEngine::TrgEngine() {}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::TrgEngine::~TrgEngine() {}
// ============================================================================
namespace 
{
  // ==========================================================================
  template <class TYPE> 
  inline StatusCode _process 
  ( const std::string& name , const TYPE& cut )
  {
    LoKi::Hybrid::TrgEngineActor& actor = 
      LoKi::Hybrid::TrgEngineActor::instance() ;
    return actor.process ( name , cut ) ;
  } 
  // ==========================================================================
} //                                                 end of anonymoys namespace 
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::TrgEngine::process
( const std::string&           name , 
  const LoKi::Types::TC_Cuts&  cut  ) const
{ return _process ( name , cut ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrgEngine::process
( const std::string&           name , 
  const LoKi::Types::TC_Func&  func ) const 
{ return _process ( name , func ) ; }
// ============================================================================
// the functional part 
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrgEngine::process
( const std::string&              name , 
  const LoKi::Types::TC_Maps&     func ) const 
{ return _process ( name , func ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrgEngine::process
( const std::string&              name , 
  const LoKi::Types::TC_Pipes&    func ) const 
{ return _process ( name , func ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrgEngine::process
( const std::string&              name , 
  const LoKi::Types::TC_FunVals&  func ) const 
{ return _process ( name , func ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrgEngine::process
( const std::string&              name , 
  const LoKi::Types::TC_Sources&  func ) const
{ return _process ( name , func ) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
