// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/Counters.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functor.h"
#include "LoKi/ICoreFactory.h"
// ============================================================================
/** @file 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class CounterAlg
   *  Simple algorithm to dealwith counters.
   *  It has two modes: 
   *
   *    - "simple" mode, when certain counters are evaluated and are 
   *       kept locally
   *
   *    - "store-in-TES", when the values are also stored in TES in form of 
   *                      Gaudi::Numbers object at specified location
   *                      the default location is "Counters/<ALGNAME>"
   *
   *  @code 
   * 
   *    myCnt.Variables = {
   *       'nSpd'           : "CONTAINS('Raw/Spd/Digits')" , 
   *       'nVelo'          : "TrSOURCE('Rec/Track/Best', TrVELO) >> TrSIZE " , 
   *       'hasTrueMCDecay' : "MCSOURCE('MC/Particles','[B_s0 -> J/psi(1S) phi(1020)]CC') >> MCSIZE" , 
   *       'GEC-loose'      : "ACCEPT('Hlt::GEC/AcceptGESLoose')"     
   *     }
   *
   *    ## use non-standard location: 
   *    myCnt.Location = "/Event/MyCounters"
   *
   *  @endcode 
   *
   *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
   *  @date 2011-02-12
   */
  class CounterAlg : public GaudiAlgorithm
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class AlgFactory<LoKi::CounterAlg> ;
    // ========================================================================
  public:
    // ========================================================================
    /// the main method: execute 
    virtual StatusCode execute    () ;
    /// proper initialization 
    virtual StatusCode initialize () ;
    /// proper finalization 
    virtual StatusCode finalize   () ;
    // ========================================================================    
  protected:
    // ========================================================================
    /** helper class to keep the N-tuple items
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-02-16
     */
    class Item
    {
    public:
      // ====================================================================
      // the default constructor 
      Item () 
        : m_name (  ) 
        , m_fun  ( LoKi::BasicFunctors<void>::Constant ( -1.e+10 ) )
        , m_cnt  ( 0 ) 
      {}
      // ====================================================================
    public:
      // ====================================================================
      /// the variable name 
      std::string                                                 m_name ; 
      /// the functor itself 
      LoKi::Assignable<LoKi::BasicFunctors<void>::Function>::Type m_fun  ;
      /// helper counter
      StatEntity*                                                 m_cnt  ;
      // ====================================================================
    } ;
    // ======================================================================      
    /// the actual type of {"name":"functor"} map 
    typedef std::map<std::string,std::string>  Map   ;
    /// the actual type of containter of items 
    typedef std::vector<Item>                  Items ;
    // ========================================================================    
  protected:
    // ========================================================================
    /** standard constructor 
     *  @see GaudiAlgorithm 
     *  @see      Algorithm 
     *  @see      AlgFactory
     *  @see     IAlgFactory
     *  @param name the algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    CounterAlg
    ( const std::string& name , // the algorithm instance name 
      ISvcLocator*       pSvc ) // pointer to the service locator
      : GaudiAlgorithm ( name , pSvc ) 
    //
      , m_map       () 
      , m_preambulo () 
      , m_factory   ( "LoKi::Hybrid::CoreFactory/CoreFactory:PUBLIC" ) 
    //
      , m_location  ( "Counters/" + name  ) 
      , m_items     () 
    {    
      declareProperty 
        ( "Preambulo"  , 
          m_preambulo  , 
          "The preambulo lines to be used for the temporary python script" ) 
        -> declareUpdateHandler  ( &LoKi::CounterAlg::handlePreambulo  , this ) ;
      //
      declareProperty 
        ( "Factory"   , 
          m_factory   , 
          "The type/name of LoKiBender \"hybrid\" factory: void->double" ) 
        -> declareUpdateHandler  ( &LoKi::CounterAlg::handleFactory    , this ) ;
      //
      declareProperty 
        ( "Variables" , 
          m_map       , 
          "The map { 'name' : 'functor'} of variables: void -> double " ) 
        -> declareUpdateHandler  ( &LoKi::CounterAlg::handleVariables  , this ) ;
      // 
      declareProperty 
        ( "Location"  , 
          m_location  , 
          "TES-location of counters" ) ;
    } 
    /// virtual and protected destructor 
    virtual ~CounterAlg() {} ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    CounterAlg () ;                      // the default constructor is disabled 
    /// the copy constructor is disabled 
    CounterAlg ( const CounterAlg& ) ;      // the copy constructor is disabled 
    /// the assignement operator is disabled 
    CounterAlg& operator=( const CounterAlg& ) ; // the assignement is disabled
    // ========================================================================
  protected:
    // ========================================================================
    void       handlePreambulo ( Property& /* p */ )  ;
    void       handleFactory   ( Property& /* p */ )  ;
    void       handleVariables ( Property& /* p */ )  ;
    // ========================================================================
    /// update variables 
    StatusCode updateItems () ;                // update variables
    // ========================================================================
    /// the preambulo 
    std::string preambulo() const 
    {
      const std::vector<std::string>& lines  = m_preambulo ;
      //
      std::string result ;
      for ( std::vector<std::string>::const_iterator iline = 
              lines.begin() ; lines.end() != iline ; ++iline ) 
      {
        if ( lines.begin() != iline ) { result += "\n" ; }
        result += (*iline) ;
      }
      return result ;
    }
    // ========================================================================
  private:
    // ========================================================================
    /// map pf variables 
    Map                      m_map       ; // map pf variables 
    /// the preambulo 
    std::vector<std::string> m_preambulo ; // the preambulo 
    ///  factory for functor decoding 
    std::string              m_factory   ; //  factory for functor decoding 
    ///  TES-location of counters 
    std::string              m_location  ; //  TES-location of counters 
    /// decoded vector of functors/items 
    Items                    m_items     ;
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// the update handler for Preambulo
// ============================================================================
void LoKi::CounterAlg::handlePreambulo ( Property& /* p */ )  
{
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  Warning ( "Reintialization of Preambulo" ).ignore() ;
  //
  StatusCode sc = updateItems () ;
  Assert ( sc.isSuccess () , "Unable to set 'Variables'"   , sc ) ;
}
// ============================================================================
// the update handler for HltFactory
// ============================================================================
void LoKi::CounterAlg::handleFactory ( Property& /* p */ )  
{
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  Warning ( "Reintialization of HltFactory" ).ignore() ;
  //
  StatusCode sc = updateItems () ;
  Assert ( sc.isSuccess() , "Unable to set 'Variables'"   , sc ) ;
}    
// ============================================================================
// The update handler for Variables
// ============================================================================
void LoKi::CounterAlg::handleVariables ( Property& /* p */ )  
{
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  Warning ( "Reintialization of Variables" ).ignore() ;
  //
  StatusCode sc = updateItems () ;
  Assert ( sc.isSuccess() , "Unable to set 'Variables'"   , sc ) ;
}
// ============================================================================
// update  variables 
// ============================================================================
StatusCode LoKi::CounterAlg::updateItems ()                 // update variables 
{
  // get the factory
  LoKi::Hybrid::ICoreFactory* factory = tool<LoKi::Hybrid::ICoreFactory> ( m_factory , this ) ;
  //
  m_items.clear() ;
  m_items.reserve ( m_map.size() ) ;
  for ( Map::const_iterator ivar = m_map.begin() ; m_map.end() != ivar ; ++ivar ) 
  {
    Item item ;
    StatusCode sc = factory->get ( ivar->second , item.m_fun , preambulo() ) ;
    if ( sc.isFailure() )
    { return Error
        ("Unable to decode " + ivar->first + " : " + ivar->second , sc ) ; }
    //
    item.m_name =           ivar->first   ;
    item.m_cnt  = &counter( ivar->first ) ;
    //
    m_items.push_back  ( item ) ; 
    //
    debug() << "The decoded variable name is '"
            << m_items.back().m_name << "'\t, the functor : '"
            << m_items.back().m_fun  << "'" << endmsg ;
  }  
  // 
  release ( factory ) ; // we do not need the factory anymore 
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// standard initialization
// ============================================================================
StatusCode LoKi::CounterAlg::initialize () 
{
  StatusCode sc = GaudiAlgorithm::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  svc<ILoKiSvc> ( "LoKiSvc" , true ) ;
  //
  // decode variables:
  //
  if  ( ! m_map.empty() ) 
  {
    sc = updateItems() ;
    if ( sc.isFailure () ) { return Error ( "Unable to decode Variables", sc ) ; }  
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================ 
// standard finalization 
// ============================================================================
StatusCode LoKi::CounterAlg::finalize   () 
{
  m_items . clear() ;
  //
  return GaudiAlgorithm::finalize () ;
}
// ============================================================================
// the main method: execute 
// ============================================================================ 
StatusCode LoKi::CounterAlg::execute () 
{

  setFilterPassed(true);

  //
  Gaudi::Numbers* numbers = 0 ;
  //
  // get(create) counters in TES (if needed) 
  if ( !m_location.empty() ) 
  { numbers = getOrCreate<Gaudi::Numbers,Gaudi::Numbers> 
      ( evtSvc() , m_location ) ; }
  //
  for ( Items::const_iterator item = m_items.begin() ; 
        m_items.end() != item ; ++item )
  {
    // evaluate the variable 
    const double value = (item->m_fun)() ;
    // update TES object
    if ( 0 != numbers ) { numbers->counters().update ( item->m_name , value ) ; }
    // update local counters 
    item->m_cnt->add ( value ) ;
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================

// ============================================================================
/// the factory (needed for instantiation)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,CounterAlg)
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
  
