// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/FilterTool.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::FilterTool
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
 *  @see LoKi::FilterTool
 *  @date 2008-09-23 
 *  @author Vanya  BELYAEV Ivan.Belyaev@cern.ch
 */
// ============================================================================
// standard constructor 
// ============================================================================
LoKi::FilterTool::FilterTool 
( const std::string& type   ,   // tool type (?)
  const std::string& name   ,   // toolinstance name
  const IInterface*  parent )   // tool's parent 
  : GaudiTool ( type , name , parent ) 
// the type/name for LoKi/Bender "hybrid" factory 
  , m_factory ( "<UNSPECIFIED>" )
// the filter/code criteria itself 
  , m_code   ( "<unspecified>" ) 
// the preambulo 
  , m_preambulo_ () 
// the preambulo 
  , m_preambulo  ()
//
  , m_factory_updated   ( false )
  , m_code_updated      ( false )
  , m_preambulo_updated ( false )
{
  // the factory
  declareProperty 
    ( "Factory" , 
      m_factory , 
      "The type/name of LoKiBender \"hybrid\" factory" ) 
    -> declareUpdateHandler ( &LoKi::FilterTool::updateFactory  , this ) ;
  // the code 
  declareProperty 
    ( "Code"    , 
      m_code    , 
      "The Bender/Python code to be used" ) 
    -> declareUpdateHandler ( &LoKi::FilterTool::updateCode     , this ) ;
  // the code 
  declareProperty 
    ( "Preambulo"  , 
      m_preambulo_ , 
      "The preambulo to be used for the temporary python script" ) 
    -> declareUpdateHandler ( &LoKi::FilterTool::updatePreambulo , this ) ;
  // 
}
// ============================================================================
// virtual and protected destructor 
// ============================================================================
LoKi::FilterTool::~FilterTool () {}
// ============================================================================
// add to preambulo 
// ============================================================================
void LoKi::FilterTool::addToPreambulo 
( const std::string&              item ) 
{
  m_preambulo_.push_back ( item ) ;
  m_preambulo_updated = true ;
}
// ============================================================================
// sety preambulo 
// ============================================================================
void LoKi::FilterTool::setPreambulo  
( const std::vector<std::string>& items )
{
  m_preambulo_        = items ;
  m_preambulo_updated = true  ;
}
// ============================================================================
// update the factory 
// ============================================================================
void LoKi::FilterTool::updateFactory ( Property& /* p */ ) // update the factory 
{
  // no action if not yet initialized 
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  // mark as "to-be-updated" 
  m_factory_updated = true ;
  //
  // postpone the action 
  if ( !m_code_updated || !m_preambulo_updated ) { return ; } 
  //
  // perform the actual immediate decoding  
  //
  StatusCode sc = decode () ;
  Assert ( sc.isSuccess () , "Error from LoKi::FilterTool::decode()" , sc ) ;
  //
  m_code_updated      = false ;
  m_factory_updated   = false ;
  m_preambulo_updated = false ;
}
// ============================================================================
// update the code 
// ============================================================================
void LoKi::FilterTool::updateCode ( Property& /* p */ )    // update the factory 
{
  // no action if not yet initialized 
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  // mark as "to-be-updated" 
  m_code_updated = true ;
  //
  // postpone the action 
  if ( !m_factory_updated || !m_preambulo_updated ) { return ; } 
  //
  // perform the actual immediate decoding  
  //
  StatusCode sc = decode  () ;
  Assert ( sc.isSuccess () , "Error from LoKi::FilterTool::decode()" , sc ) ;
  //
  m_code_updated      = false ;
  m_factory_updated   = false ;
  m_preambulo_updated = false ;
}
// ============================================================================
// update the preambulo
// ============================================================================
void LoKi::FilterTool::updatePreambulo ( Property& /* p */ )  // update preambulo
{
  //
  // decode the preambulo:
  m_preambulo.clear() ;
  for ( std::vector<std::string>::const_iterator iline = 
          m_preambulo_.begin() ; m_preambulo_.end() != iline ; ++iline ) 
  { 
    if ( m_preambulo_.begin() != iline ) { m_preambulo += "\n" ; }
    m_preambulo += (*iline) ;
  }
  // no further action if not yet initialized 
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  // mark as "to-be-updated"
  m_preambulo_updated = true ;
  //
  // postpone the action 
  if ( !m_factory_updated || !m_code_updated ) { return ; } 
  //
  // perform the actual immediate decoding  
  //
  StatusCode sc = decode  () ;
  Assert ( sc.isSuccess () , "Error from LoKi::FilterTool::decode()" , sc ) ;
  //
  m_code_updated      = false ;
  m_factory_updated   = false ;
  m_preambulo_updated = false ;
}
// ============================================================================
// the initialization of the algorithm 
// ============================================================================
StatusCode LoKi::FilterTool::initialize () 
{
  /// initialize the base 
  StatusCode sc = GaudiTool::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  // force LoKi service 
  svc<IService>( "LoKiSvc" , true ) ;
  // decode the functor 
  return decode () ;
}
// ============================================================================
// the finalization of the algorithm 
// ============================================================================
StatusCode LoKi::FilterTool::finalize () 
{ return GaudiTool::finalize () ; }
// ============================================================================
// The END 
// ============================================================================
