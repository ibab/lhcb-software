// $Id: HltUnit.cpp,v 1.2 2008-11-13 13:14:42 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IHltUnit.h"
#include "LoKi/ICoreFactory.h"
// ============================================================================
// local 
// ============================================================================
#include "HltUnit.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::HltUnit
 *  @date 2008-11-10 
 *  @author Vanya BELYAEV Ivan.BELYAEV@nikhef.nl
 */
// ============================================================================
/*  standard constructor 
 *  @param name the algorithm instance name 
 *  @param pSvc pointer to Service Locator 
 */
// ============================================================================
LoKi::HltUnit::HltUnit
( const std::string& name ,                      //    algorithm instance name 
  ISvcLocator*       pSvc )                      // pointer to Service Locator
  : LoKi::FilterAlg ( name , pSvc ) 
  //
  , m_hltSvc  ( 0     )
  , m_monitor ( false )
  , m_all     () 
  , m_out     ()
  //
  , m_cut ( LoKi::BasicFunctors<void>::BooleanConstant( false ) )
{
  // ==========================================================================
  // declare own property  
  declareProperty 
    ( "Monitor" , 
      m_monitor , 
      "Flag to switch on the simple embedded monitoring" ) ;
  // set the code string 
  StatusCode sc = setProperty ( "Code" , "FNONE" ) ;
  Assert ( sc.isSuccess () , "Unable (re)set property 'Code'"    , sc ) ;
  // set the factory type/name 
  sc = setProperty
    ( "Factory" , "LoKi::Hybrid::CoreFactory/CoreFactory::PUBLIC" ) ;
  Assert ( sc.isSuccess () , "Unable (re)set property 'Factory'" , sc ) ;
  // ==========================================================================
}
// ============================================================================
// virtual & protected destructor 
// ============================================================================
LoKi::HltUnit::~HltUnit(){}                   // virtual & protected destructor
// ============================================================================
/* register the selection 
 *  (internal method, should not be invoked directly) 
 *  @see LoKi::IHltUnit::registerSelection
 *  @param selection the seelction to be registered 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::HltUnit::registerSelection ( Hlt::Selection* selection ) 
{
  if ( 0 == selection ) 
  { return Error ("registerSelection: Hlt::Selection points to NULL") ; }
  // get the service 
  IHltDataSvc* hlt = hltSvc() ;
  // register selection 
  StatusCode sc = hlt->addSelection ( selection , this ) ;
  if ( sc.isFailure() ) 
  { return Error( "Unable to register selection " + selection->id().str() ) ; }
  //
  // register as "output" selection 
  m_out.insert ( selection->id () , selection ) ; 
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  get the selection by key 
 *  @param key the key 
 *  @return pointer to the selection 
 */
// ============================================================================
Hlt::Selection* LoKi::HltUnit::selection ( const stringKey& key ) const 
{
  // get the selection 
  Hlt::Selection* s = i_selection ( key ) ;
  // register "all" selection 
  if ( monitor() ) { m_all.insert ( key , s ) ; }
  //
  return s ;
}
// ============================================================================
/*  decode the functor 
 *  @see LoKi::ICoreFactory 
 */
// ============================================================================
StatusCode LoKi::HltUnit::decode() 
{
  StatusCode sc = i_decode<LoKi::Hybrid::ICoreFactory> ( m_cut ) ;
  Assert ( sc.isSuccess() , "Unable to decode the functor!" ) ;
  return StatusCode::SUCCESS ;  
}
// ============================================================================
/* query interface trick (please do not abuse it!)
 *  @see IInterface::queryInterface 
 *  @param iid  the unique interface ID 
 *  @param ppvi pointer to the interface 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::HltUnit::queryInterface 
(const InterfaceID& iid, void** ppvi ) 
{
  /// valid placeholder?
  if ( 0 == ppvi ) { return StatusCode::FAILURE ; }
  /// 
  if ( LoKi::IHltUnit::interfaceID() == iid ) 
  {
    *ppvi = static_cast<LoKi::IHltUnit*>( this ) ;
    addRef() ; //                               NB: Increment the ref-count!
    return StatusCode::SUCCESS ;                                   // RETURN 
  }
  else if ( IHltDataSvc::interfaceID() == iid ) 
  {
    IHltDataSvc* hlt = hltSvc() ;
    return hlt -> queryInterface ( iid , ppvi ) ;                  // RETUTRN
  }
  else if ( IANNSvc::interfaceID() == iid ) 
  {
    IANNSvc* ann = annSvc() ;
    return ann -> queryInterface ( iid , ppvi ) ;                  // RETURN
  }
  else if ( LoKi::ILoKiSvc::interfaceID() == iid ) 
  {
    LoKi::ILoKiSvc* loki = svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;
    return loki -> queryInterface( iid , ppvi ) ;
  }
  // 
  return Algorithm::queryInterface ( iid , ppvi ) ;
}
// ===========================================================================
// execute the algorithm
// ===========================================================================
StatusCode LoKi::HltUnit::execute () 
{
  if ( updateRequired() )
  {
    Warning ( "Update of the algorithm is required!!!") ;
    StatusCode sc = decode() ;
    Assert ( sc.isSuccess() , "Unable to decode the functor!" ) ;
  } 
  
  // MANDATORY: clear all "out" seelctions
  for ( Map::iterator iout = m_out.begin() ; m_out.end() != iout ; ++iout ) 
  { iout->second->clean() ; }
  
  
  // OPTIONAL: Some decorative monitoring 
  typedef std::map<stringKey,size_t> Sizes  ;
  Sizes map ;
  // get the status of all selections 
  if ( monitor() ) 
  {
    for ( CMap::const_iterator ikey = m_all.begin() ; m_all.end() != ikey ; ++ikey ) 
    { map[ ikey->first ] = ikey->second->size() ; }  
  }
  // use the functor 
  const bool result = m_cut () ;
  //
  // some statistics
  counter ("#passed" ) += result ;
  //
  // set the filter:
  setFilterPassed ( result ) ;
  //
  
  /// Monitor output selections  (*ALWAYS*)
  for ( Map::const_iterator iout = m_out.begin() ; m_out.end() != iout ; ++iout ) 
  { counter ( "#" + iout->first ) += iout->second->size() ; }
  
  // DECORATION? monitoring
  if ( monitor() ) // the output selections are *ALWAYS* monitored   
  {
    for ( Sizes::const_iterator ikey = map.begin() ; map.end() != ikey ; ++ikey )
    {
      // get the selection 
      const Hlt::Selection* s = m_all [ ikey->first ] ;
      // count the changes in the number of candidates  
      if ( m_out.end() == m_out.find ( ikey->first ) ) 
      { counter ( "delta " + ikey->first ) += ( s->size() - ikey->second ) ; }
      // ======================================================================
    } 
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// the factory (needed for instantiations):
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,HltUnit)
// ============================================================================
//  The END 
// ============================================================================
