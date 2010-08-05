// $Id$
// ============================================================================
// $URL$
// ============================================================================
// Include files
// ============================================================================
// local
// ============================================================================
#include "LoKi/TrackEngine.h"
#include "LoKi/TrackEngineActor.h"
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
 *
 *  $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
// ============================================================================
// constructor
// ============================================================================
LoKi::Hybrid::TrackEngine::TrackEngine() {}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Hybrid::TrackEngine::~TrackEngine() {}
// ============================================================================
namespace 
{
  // ==========================================================================
  template <class TYPE> 
  inline StatusCode _process 
  ( const std::string& name , const TYPE& cut )
  {
    LoKi::Hybrid::TrackEngineActor& actor = 
      LoKi::Hybrid::TrackEngineActor::instance() ;
    return actor.process ( name , cut ) ;
  } 
  // ==========================================================================
} //                                                 end of anonymoys namespace 
// ============================================================================
// add the cut 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngine::process
( const std::string&          name , 
  const LoKi::Types::TrCuts&  cut  ) const { return _process ( name , cut ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngine::process
( const std::string&          name , 
  const LoKi::Types::TrFunc&  func ) const { return _process ( name , func ) ; }
// ============================================================================
// the functional part 
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngine::process
( const std::string&             name , 
  const LoKi::Types::TrMaps&     func ) const 
{ return _process ( name , func ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngine::process
( const std::string&             name , 
  const LoKi::Types::TrPipes&    func ) const 
{ return _process ( name , func ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngine::process
( const std::string&             name , 
  const LoKi::Types::TrFunVals&  func ) const 
{ return _process ( name , func ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngine::process
( const std::string&             name , 
  const LoKi::Types::TrElements& func ) const 
{ return _process ( name , func ) ; }
// ============================================================================
// add the function 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngine::process
( const std::string&             name , 
  const LoKi::Types::TrSources&  func ) const
{ return _process ( name , func ) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
