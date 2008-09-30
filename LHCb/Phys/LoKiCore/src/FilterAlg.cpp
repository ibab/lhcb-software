// $Id: FilterAlg.cpp,v 1.2 2008-09-30 15:58:41 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/FilterAlg.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::FilterAlg
 *  @see LOKi::FilterAlg
 *  @date 2008-09-23 
 *  @author Vanya  BELYAEV Ivan.Belyaev@inkhef.nl
 */
// ============================================================================
/*  standard constructor 
 *  @see GaudiAlgorithm 
 *  @see      Algorithm 
 *  @see      AlgFactory
 *  @see     IAlgFactory
 *  @param name the algorithm instance name 
 *  @param pSvc pointer to Service Locator 
 */
// ============================================================================
LoKi::FilterAlg::FilterAlg 
( const std::string& name ,                     // the algorithm instance name 
  ISvcLocator*       pSvc )                  // pointer to the service locator
  : GaudiAlgorithm ( name , pSvc ) 
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
    -> declareUpdateHandler ( &LoKi::FilterAlg::updateFactory  , this ) ;
  // the code 
  declareProperty 
    ( "Code"    , 
      m_code    , 
      "The Bender/Python code to be used" ) 
    -> declareUpdateHandler ( &LoKi::FilterAlg::updateCode     , this ) ;
  // the code 
  declareProperty 
    ( "Preambulo"  , 
      m_preambulo_ , 
      "The preambulo to be used for the temporary python script" ) 
    -> declareUpdateHandler ( &LoKi::FilterAlg::updatePreambulo , this ) ;
}
// ============================================================================
// virtual and protected destructor 
// ============================================================================
LoKi::FilterAlg::~FilterAlg () {}
// ============================================================================
// update the factory 
// ============================================================================
void LoKi::FilterAlg::updateFactory ( Property& /* p */ ) // update the factory 
{
  // no action if not yet initialized 
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  /// mark as "to-be-updated" 
  m_factory_updated = true ;
  //
  // postpone the action 
  if ( !m_code_updated || !m_preambulo_updated ) { return ; } 
  
  // perform the actual immediate decoding  
  
  StatusCode sc = decode () ;
  Assert ( sc.isSuccess () , "Error from LoKi::FilterAlg::decode()" , sc ) ;
  
  m_code_updated      = false ;
  m_factory_updated   = false ;
  m_preambulo_updated = false ;
}
// ============================================================================
// update the code 
// ============================================================================
void LoKi::FilterAlg::updateCode ( Property& /* p */ )    // update the factory 
{
  // no action if not yet initialized 
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  /// mark as "to-be-updated" 
  m_code_updated = true ;
  //
  // postpone the action 
  if ( !m_factory_updated || !m_preambulo_updated ) { return ; } 
  
  // perform the actual immediate decoding  
  
  StatusCode sc = decode  () ;
  Assert ( sc.isFailure() , "Error from LoKi::FilterAlg::decode()" , sc ) ;
  
  m_code_updated      = false ;
  m_factory_updated   = false ;
  m_preambulo_updated = false ;
}
// ============================================================================
// update the preambulo
// ============================================================================
void LoKi::FilterAlg::updatePreambulo ( Property& /* p */ )  // update preambulo
{
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
  /// mark as "to-be-updated"
  m_preambulo_updated = true ;
  // postpone the action 
  if ( !m_factory_updated || !m_code_updated ) { return ; } 
  
  // perform the actual immediate decoding  
  
  StatusCode sc = decode  () ;
  Assert ( sc.isFailure() , "Error from LoKi::FilterAlg::decode()" , sc ) ;
  
  m_code_updated      = false ;
  m_factory_updated   = false ;
  m_preambulo_updated = false ;
}
// ============================================================================
// the initialization of the algorithm 
// ============================================================================
StatusCode LoKi::FilterAlg::initialize () 
{
  /// initialize the base 
  StatusCode sc = GaudiAlgorithm::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  // force LoKi service 
  svc<IService>( "LoKiSvc" ) ;
  // decode the functor 
  return decode () ;
}
// ============================================================================
// the finalization of the algorithm 
// ============================================================================
StatusCode LoKi::FilterAlg::finalize () 
{ return GaudiAlgorithm::finalize () ; }



// ============================================================================
// The END 
// ============================================================================
