// $Id$
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IIncidentSvc.h"
// ============================================================================
// LoKi
// ============================================================================
#include "HltBase/IHltUnit.h"
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
namespace
{
  // ==========================================================================
  const Gaudi::StringKey s_NOPVS{ std::string{"NoPVs"} } ;
  // ==========================================================================
}
// ============================================================================
/*  standard constructor
 *  @param name the algorithm instance name
 *  @param pSvc pointer to Service Locator
 */
// ============================================================================
LoKi::HltUnit::HltUnit
( const std::string& name ,                       //    algorithm instance name
  ISvcLocator*       pSvc )                       // pointer to Service Locator
  : LoKi::FilterAlg ( name , pSvc )
  // services:
  , m_annSvc     { nullptr }                // "Assigned Numbers & Names " service
  , m_regSvc     { nullptr }                //                Hlt Register service
  , m_hltSvc     { nullptr }                //                    Hlt Data Service
  , m_lokiSvc    { nullptr }                //                        LoKi Service
  //
  , m_monitor    { false }
  , m_decode     { false }
  , m_in         {}
  , m_out        {}
  //
  , m_cut { LoKi::BasicFunctors<void>::BooleanConstant{ false } }
//
  ,  m_params     ()
  ,  m_params_old ()
  //
{
  // ==========================================================================
  // declare own property
  declareProperty
    ( "Monitor" ,
      m_monitor ,
      "Flag to switch on the simple embedded monitoring" ) ;
  declareProperty
    ( "PVSelection" ,
      m_pvSelection = s_NOPVS ,
      "Hlt PV selection." ) ;
  declareProperty
    ( "Tools",
      m_toolNames = std::map<std::string, std::string> {
      { "DistanceCalculator", "LoKi::DistanceCalculator:PUBLIC" },
      { "LoKi::DistanceCalculator", "LoKi::DistanceCalculator:PUBLIC" },
      { "LifetimeFitter", "LoKi::LifetimeFitter:PUBLIC" },
      { "LoKi::LifetimeFitter/lifetime:PUBLIC", "LoKi::LifetimeFitter/lifetime:PUBLIC"},
      { "VertexFitter", "LoKi::VertexFitter:PUBLIC" },
      { "ParticleCombiner", "LoKi::VertexFitter:PUBLIC" },
      { "RelatedPVFinder",  "GenericParticle2PVRelator<_p2PVWithIPChi2, OfflineDistanceCalculatorName>/P2PVWithIPChi2:PUBLIC"}},
      "Types/Names of tools" ) ;
  declareProperty
    ( "PreloadTools",
      m_preloadTools = std::vector<std::string> {},
      "Types/Names of tools to preload" ) ;
  // set the code string
  StatusCode sc = setProperty ( "Code" , "FNONE" ) ;
  Assert ( sc.isSuccess () , "Unable (re)set property 'Code'"    , sc ) ;
  // set the factory type/name
  sc = setProperty
    ( "Factory" , "LoKi::Hybrid::CoreFactory/Hlt1Factory:PUBLIC" ) ;
  Assert ( sc.isSuccess () , "Unable (re)set property 'Factory'" , sc ) ;
  // update the preambulo :
  sc = setProperty ( "Preambulo" , std::vector<std::string> {
                                   "from LoKiTracks.decorators  import *"
                                 , "from LoKiTrigger.decorators import *"
                                 , "from LoKiCore.functions     import *" } );
  Assert ( sc.isSuccess () , "Unable (re)set property 'Preambulo'" , sc ) ;
  // ==========================================================================
  declareProperty( "Params" ,
                   m_params ,
                   "Parameters accessingle via PARAM-constructions" )
    -> declareUpdateHandler ( &LoKi::HltUnit::updateParams  , this ) ;
}
// ============================================================================
// virtual & protected destructor
// ============================================================================
LoKi::HltUnit::~HltUnit(){       // virtual & protected destructor
    for ( auto& i : m_out ) delete i.second;
}
// ============================================================================
/* register the selection
 *  @param selection the selection to be registered
 *  @param client the client
 *  @return status code
 */
// ============================================================================
StatusCode LoKi::HltUnit::registerOutput
( std::unique_ptr<Hlt::Selection>    selection  ,
  const Client&   /* client */  ) const
{
  if ( !selection )
  { return Error ("registerSelection: Hlt::Selection points to NULL") ; }
  //
  StatusCode sc = regSvc()->registerOutput ( selection.get() , this ) ;
  if ( sc.isFailure() )
  { return Error ( "Unable to register OUTPUT selection '" +
                   selection->id().str()+ "'" , sc ) ; }
  //
  // register as "output" selection
  //FIXME:TODO: VectorMap does not support 'move' insertion...
  // so we cannot have a VectorMap<..., std::unique_ptr<...>>
  auto id = selection->id();
  m_out.insert ( id , selection.release() ) ;
  //
  return StatusCode::SUCCESS ;
}
// ========================================================================
/** declare the input selection
 *  @param key the selection key
 *  @param client the client
 */
// ========================================================================
const Hlt::Selection* LoKi::HltUnit::declareInputSelection
( const Key&       key       ,
  const Client& /* client */ ) const
{
  // use the registration service
  StatusCode sc = regSvc()->registerInput ( key , this ) ;
  Assert ( sc.isSuccess () ,
           "Unable to register  INPUT selection '" + key.str() + "'" ) ;
  // get the selection for the service
  const Hlt::Selection* sel = hltSvc()->selection ( key , this ) ;
  Assert ( sel , "Unable to locate the selection '" + key.str() + "'" ) ;
  //
  m_in.insert ( key , sel ) ;
  return sel ;
}
// ============================================================================
/*  get the (const) selection by key
 *  @param key the key
 *  @param client the client
 *  @return pointer to the selection
 */
// ============================================================================
const Hlt::Selection* LoKi::HltUnit::selection
( const Key&       key       ,
  const Client& /* client */ ) const
{
  auto ifind = m_in.find ( key ) ;
  Assert ( m_in.end() != ifind ,
           "Unable to get LOCAL INPUT selection '" + key.str() + "'" ) ;
  return ifind->second ;
}
// ============================================================================
/*  get the (const) selection by key  (anonymous)
 *  @param key the key
 *  @return pointer to the selection
 */
// ============================================================================
const Hlt::Selection* LoKi::HltUnit::selection ( const Key& key ) const
{
  Warning ("Anonymous access to the selection!") ;
  auto ifind = m_in.find ( key ) ;
  Assert ( m_in.end() != ifind ,
           "Unable to get LOCAL INPUT selection '" + key.str() + "'" ) ;
  return ifind->second ;
}
// ============================================================================
/*  decode the functor
 *  @see LoKi::ICoreFactory
 */
// ============================================================================
StatusCode LoKi::HltUnit::decode()
{
  if (!m_decode) return StatusCode::SUCCESS;
  StatusCode sc = defineCode () ;
  Assert ( sc.isSuccess() , "Unable to defineCode for functor!" ) ;
  // =========================================================================
  return sc;
}
// ============================================================================
/*  define the code
 *  @see LoKi::FilterAlg
 *  @see LoKi::ICoreFactory
 */
// ============================================================================
StatusCode LoKi::HltUnit::defineCode ()
{
  // ==========================================================================
  /// decode & initialize the functors
  StatusCode sc = i_decode<LoKi::Hybrid::ICoreFactory> ( m_cut ) ;
  Assert ( sc.isSuccess() , "Unable to decode the functor!" ) ;
  // =========================================================================
  return sc;
}
// ============================================================================
/*  define the stream
 *  @see LoKi::FilterAlg
 *  @see LoKi::ICoreFactory
 */
// ============================================================================
StatusCode LoKi::HltUnit::setCode ( const LoKi::Types::FCuts& cut )
{
  m_cut = cut ;
  //
  if ( msgLevel ( MSG::DEBUG ) )
  { debug() << "Set new Code: " << m_cut << endmsg ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  query interface trick (please do not abuse it!)
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
  if ( Hlt::IUnit::interfaceID() == iid )
  {
    *ppvi = static_cast<Hlt::IUnit*>( this ) ;
    addRef() ;
    return StatusCode::SUCCESS ; // RETURN
  }
  else if ( IIncidentListener::interfaceID() == iid )
  {
    *ppvi = static_cast<IIncidentListener*>( this ) ;
    addRef() ;
    return StatusCode::SUCCESS ; // RETURN
  }
  else if ( IDVAlgorithm::interfaceID() == iid )
  {
    *ppvi = static_cast<IDVAlgorithm*>( this ) ;
    addRef() ;
    return StatusCode::SUCCESS ; // RETURN
  }
  else if ( Hlt::IRegister ::interfaceID() == iid )
  { return regSvc() -> queryInterface ( iid , ppvi ) ; }              // RETURN
  else if ( Hlt::IData     ::interfaceID() == iid )
  { return hltSvc() -> queryInterface ( iid , ppvi ) ; }              // RETURN
  else if ( IANNSvc        ::interfaceID() == iid )
  { return annSvc() -> queryInterface ( iid , ppvi ) ; }              // RETURN
  else if ( IANSvc         ::interfaceID() == iid )
  { return annSvc() -> queryInterface ( iid , ppvi ) ; }              // RETURN
  else if ( LoKi::ILoKiSvc ::interfaceID() == iid )
  { return lokiSvc() -> queryInterface( iid , ppvi ) ; }              // RETURN
  // check the basic interfaces
  StatusCode sc = Algorithm::queryInterface ( iid , ppvi ) ;
  if ( sc.isSuccess () ) { return sc ; }                              // RETURN
  // check the Hlt Service
  sc = hltSvc  () -> queryInterface ( iid , ppvi ) ;
  if ( sc.isSuccess () ) { return sc ; }                              // RETURN
  // check the LoKi Service
  sc = lokiSvc () -> queryInterface ( iid , ppvi ) ;
  if ( sc.isSuccess () ) { return sc ; }                              // RETURN
  // come back to the base
  return Algorithm::queryInterface ( iid , ppvi ) ;
}
// ============================================================================
// initialize the algorithm
// ============================================================================
StatusCode LoKi::HltUnit::initialize ()
{
  /// lock the context
  Gaudi::Utils::AlgContext lock { this , contextSvc() } ;
  // ==========================================================================
  // ensure the validity of Hlt registration service
  Assert ( regSvc() , "Hlt::IRegister is not available" ) ;
  // ==========================================================================
  /// lock Hlt Register Service
  Hlt::IRegister::Lock     lock2 { regSvc() , this         } ;
  /// decode & initialize the functors
  StatusCode sc = LoKi::FilterAlg::initialize();
  if ( !sc.isSuccess() ) return sc;
  /// preload tools
  for (const auto& preload : m_preloadTools) {
     auto loaded = tool<IAlgTool>(preload, this);
     Info(std::string("Preloaded ") + loaded->type() + "/" + loaded->name());
  }
  m_decode = true;
  decode();
  // We have a dummy that is equal to the default in case no PVs are needed
  if ( m_pvSelection != s_NOPVS ) {
    declareInputSelection( m_pvSelection, LoKi::Constant<void,bool>( true ) ) ;
  }
  // register with the incident service
  SmartIF<IIncidentSvc> incSvc;
  incSvc = serviceLocator()->service( "IncidentSvc" );
  incSvc->addListener( this, IncidentType::BeginEvent );
  return sc;
}
// ============================================================================
// execute the algorithm
// ============================================================================
StatusCode LoKi::HltUnit::execute ()
{
  /// lock the context
  Gaudi::Utils::AlgContext lock { this , contextSvc() } ;
  ///
  Assert ( !updateRequired() , "Update is not possible for Hlt!" ) ;

  /// MANDATORY: clear all "out" selections
  for ( auto& out : m_out ) { out.second->clean() ; }

  /// OPTIONAL: Some decorative monitoring
  std::map<Gaudi::StringKey,size_t>  map ;

  // get the status of all selections
  if ( monitor() )
  {
    /** do somehting here */
  }
  // ==========================================================================
  // use the functor                                                   // NB !!
  const bool result = m_cut () ;                                       // NB !!
  // ==========================================================================
  //
  // some statistics
  counter ("#passed" ) += result ;
  //
  // set the filter:
  setFilterPassed ( result ) ;

  for ( auto& out : m_out )
  { out.second->setDecision( !out.second->empty() ) ; }

  /// Monitor output selections  (*ALWAYS*)
  for ( const auto& out : m_out )
  { counter ( "# " + out.first.str() ) += out.second->size() ; }

  // DECORATION? monitoring
  if ( monitor() ) // the output selections are *ALWAYS* monitored
  {
    /** do somehting here */
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  get the object form TES
 *  @param client the client
 *  @param location the TES location
 *  @return the object
 */
// ============================================================================
const DataObject*
LoKi::HltUnit::tes
( const Hlt::IUnit::Client& /* client  */ ,
  const Hlt::IUnit::Key&       location   ) const
{
  // check the location
  auto ifind = std::find ( m_tes.begin() , m_tes.end  () , location ) ;
  //
  Assert ( m_tes.end() != ifind ,
           "tes: unauthorized access to TES -data" ) ;
  return hltSvc()->tes ( this , location ) ;
  //
}
// ============================================================================
/* register the query  to TES-selection
 *  @param location TES location to be registered
 *  @param consumer algorithm/consumer
 *  @return Status Code
 */
// ============================================================================
StatusCode LoKi::HltUnit::registerTESInput
( const Hlt::IUnit::Key&       location    ,        //         TES location
  const Hlt::IUnit::Client& /* client   */ ) const  //               client
{
  //
  StatusCode sc = regSvc()->registerTESInput ( location , this ) ;
  if ( sc.isFailure() )
  { return Error ( "Unable to register INPUT TES location '" +
                   location.str() + "'" , sc ) ; }
  //
  auto ifind = std::find ( m_tes.begin() , m_tes.end() , location ) ;
  if ( m_tes.end () != ifind ) { return StatusCode::SUCCESS ;}        // RETURN
  //
  m_tes.push_back ( location ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
void LoKi::HltUnit::handle ( const Incident& )
{
  m_p2PVMap.clear();
}
// ============================================================================
const LHCb::RecVertex::Range LoKi::HltUnit::primaryVertices () const
{
  if (m_pvSelection == s_NOPVS) {
    return LHCb::RecVertex::Range();
  }
  auto sel = selection( m_pvSelection, LoKi::Constant<void,bool>( true )  );
  auto vertices = sel->down_cast<LHCb::RecVertex>();
  return LHCb::RecVertex::Range(vertices->begin(), vertices->end());
}
// ============================================================================
const LHCb::VertexBase* LoKi::HltUnit::bestVertex
( const LHCb::Particle* particle ) const
{
  auto it = m_p2PVMap.find( particle );
  if ( it != m_p2PVMap.end() ) return it->second;
  //
  auto relatedFinder = relatedPVFinder();
  LHCb::RecVertex::Range pvs = primaryVertices();
  auto bestPV = relatedFinder->relatedPV( particle, pvs );
  m_p2PVMap[particle] = bestPV;
  return bestPV;
}

// ============================================================================
void LoKi::HltUnit::updateParams ( Property& /* p */ ) // update the factory
{
  // no action if not yet initialized
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() )
  {
    /// silently redefine "old" parameters
    m_params_old = m_params ; // silently redefine "old" parameters
    return ;                                                         // RETURN
  }
  //
  // check if different from "old" value
  //
  MsgStream& log = warning ();
  if ( m_params_old != m_params && log.isActive () )
  {
    log << "Parameters are redefined:" ;
    log << "\n NEW:" ;  Gaudi::Utils::toStream ( m_params     , log.stream() ) ;
    log << "\n OLD:" ;  Gaudi::Utils::toStream ( m_params_old , log.stream() ) ;
    log << endmsg ;
  }
  //
  m_params_old = m_params ;
  //
}
// ============================================================================
// the factory (needed for instantiations):
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,HltUnit)
// ============================================================================
//  The END
// ============================================================================
