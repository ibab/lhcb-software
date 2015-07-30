// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <map>
#include <set>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/IDecodeSimpleDecayString.h"
#include "Kernel/GetDecay.h"
#include "Kernel/GetParticlesForDecay.h"
#include "Kernel/IPlotTool.h"
#include "Kernel/ISetInputParticles.h"
#include "Kernel/PVSentry.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/ATypes.h"
#include "LoKi/IHybridFactory.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/CompareParticles.h"
// ============================================================================
#include "LoKi/Selected.h"
#include "LoKi/Combiner.h"
// ============================================================================
#include "LoKi/Particles0.h"
#include "LoKi/PIDOperators.h"
// ============================================================================
// local
// ============================================================================
#include "CombineParticles.h"
#include "Counters.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/algorithm/string.hpp"
// ============================================================================
/** @file CombineParticles.cpp
 *
 *  Implementtation fiel for class CombineParticles
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 * 
 *  @see CombineParticles
 *
 *  Version           $Revision$
 *  Last modification $Date$
 *                 by $Author$
 *  
 */
// ============================================================================
namespace
{
  // ==========================================================================
  /// make the plots ?
  inline bool validPlots ( const std::string& name )
  {
    if ( name.empty() ) { return false ; }
    return "none" != boost::to_lower_copy ( name ) ;
  }
  /// local "none"
  typedef LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant       _PBOOL ;
  /// local "none"
  typedef LoKi::BasicFunctors<LHCb::Particle::ConstVector>::BooleanConstant _CBOOL ;
  /// local "none"
  typedef LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant   _ABOOL ;
  // ==========================================================================
} // end of anonymous namespace
// ============================================================================
// constructor
// ============================================================================
CombineParticles::MyCut::MyCut() 
  : m_pid  ()                         // the particle PID
  , m_name ()                         // the particle NAME
  , m_cut     ( _PBOOL ( true  ) )    // the selection predicate
  , m_moni    ( false            )    // monitoring flag
  , m_monitor ( _CBOOL ( false ) )    // the monitoring functor
{}
// ============================================================================
// construct the preambulo string
// ============================================================================
std::string CombineParticles::preambulo() const
{
  std::string result ;
  for ( std::vector<std::string>::const_iterator iline =
          m_preambulo.begin() ; m_preambulo.end() != iline ; ++iline )
  {
    if ( m_preambulo.begin() != iline ) { result += "\n" ; }
    result += (*iline) ;
  }
  return result ;
}
// ============================================================================
using namespace LoKi ;
// ============================================================================
/*  the standard constructor
 *  @param name algorithm instance name
 *  @param pSvc the service locator
 */
// ============================================================================
CombineParticles::CombineParticles
( const std::string& name ,   // the algorithm instance name
  ISvcLocator*       pSvc )   // the service locator
  : base_class (  name , pSvc )
//
// properties
//
  , m_decayDescriptors ()  // the decay descriptors
  , m_daughterCuts     ()  // the cuts for daughter particles
  , m_combinationCut   ( "ATRUE" ) // the cut for the combination of daughters
  , m_motherCut        ( "Configure me!" ) // the final cut for the mother particle. Must be configured
  , m_factory  ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC" )
  , m_preambulo () // the preambulo
//
// plots:
//
  , m_daughtersPlotsName   ( "LoKi::Hybrid::PlotTool/DaughtersPlots"   )
  , m_combinationPlotsName ( "LoKi::Hybrid::PlotTool/CombinationPlots" )
  , m_motherPlotsName      ( "LoKi::Hybrid::PlotTool/MotherPlots"      )
//
// plot paths:
//
  , m_daughtersPlotsPath   ( "" )
  , m_combinationPlotsPath ( "" )
  , m_motherPlotsPath      ( "" )
//
// monitoring
//
  , m_monitor                ( false )
  , m_daughterMonitors       ()
  , m_combinationMonitorCode ()
  , m_motherMonitorCode      ()
  , m_combinationMonitor     ( _CBOOL ( false ) )
  , m_motherMonitor          ( _CBOOL ( false ) )
//
// for "heavy" events
//
  , m_maxComb      ( 0     )
  , m_maxCombStop  ( false )
  , m_maxCand      ( 0     )
  , m_maxCandStop  ( false )
  , m_stopIncidentName ()
//
// locals
//
// the actual list of the decays
  , m_decays ()  // the actual list of the decays
// the list of cuts for daughter particles
  , m_cuts   ()  // the list of cuts for daughter particles
// the actual cut for combination of good daughters
  , m_acut ( _ABOOL ( true  ) )
// the final cut the constructed mother particle
  , m_cut  ( _PBOOL ( false ) )
// plots:
  , m_daughtersPlots   ( NULL )
  , m_combinationPlots ( NULL )
  , m_motherPlots      ( NULL )
// special
  , m_inputParticles    (       )
  , m_useInputParticles ( false )
// update:
  , m_to_be_updated1    ( true )
  , m_to_be_updated2    ( true )
//
  , m_incSvc ( NULL )
{
  //
  if      ( 0 == name.find ( "Hlt1" ) ) 
  { m_factory = "LoKi::Hybrid::Tool/Hlt1HybridFactory:PUBLIC" ; }
  else if ( 0 == name.find ( "Hlt2" ) )
  { m_factory = "LoKi::Hybrid::Tool/Hlt2HybridFactory:PUBLIC" ; }
  //
  declareProperty
    ( "DecayDescriptors" , m_decayDescriptors ,
      "The list of (simple) decays to be recontructed/selected"   )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  {
    auto p = Gaudi::Utils::getProperty ( this , "DecayDescriptor" ) ;
    if ( p && ! p->updateCallBack() )
    { p -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ; }
  }
  //
  declareProperty
    ( "Factory"          , m_factory          ,
      "Type/Name for C++/Python Hybrid Factory"                   )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  declareProperty
    ( "Preambulo"        , m_preambulo        ,
      "The preambulo to be used for Bender/Python script"         )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  declareProperty
    ( "DaughtersCuts"    , m_daughterCuts     ,
      "The map of cuts, to be applied for daughter particles"     )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  declareProperty
    ( "CombinationCut"   , m_combinationCut   ,
      "The cut to be applied for combination of good daughters"   )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  declareProperty
    ( "MotherCut"        , m_motherCut        ,
      "The cut to be applied for the constructed mother particle. Must be configured." )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  declareProperty
    ( "DaughtersPlots"   , m_daughtersPlotsName   ,
      "The type/name for 'Daughters   Plots' tool " )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty
    ( "CombinationPlots" , m_combinationPlotsName ,
      "The type/name for 'Combination Plots' tool " )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty
    ( "MotherPlots"      , m_motherPlotsName      ,
      "The type/name for 'Mother      Plots' tool " )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty
    ( "DaughtersPlotsPath"   , m_daughtersPlotsPath  ,
      "The path for 'Daughters   Plots'" )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty
    ( "CombinationPlotsPath" , m_combinationPlotsPath ,
      "The path for 'Combination Plots'" )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty
    ( "MotherPlotsPath"      , m_motherPlotsPath      ,
      "The path for 'Mother      Plots'" )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  // The general flag to switch on/off the monitoring
  declareProperty
    ( "Monitor" ,
      m_monitor ,
      "The general flag to switch on/off the monitoring" )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  // The map of monitoring functors for daughter particles
  declareProperty
    ( "DaughtersMonitors" ,
      m_daughterMonitors  ,
      "The map of monitoring functors for daughter particles")
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  // The monitoring functor for combinations
  declareProperty
    ( "CombinationMonitor"     ,
      m_combinationMonitorCode ,
      "The monitoring functor for combinations before the cuts" )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  //
  // The monitoring functor for mother particles
  declareProperty
    ( "MotherMonitor"     ,
      m_motherMonitorCode ,
      "The monitoring functor for mother particles after all cuts" )
    -> declareUpdateHandler ( &CombineParticles::propertyHandler1 , this ) ;
  // Histogramming
  declareProperty
    ( "HistoProduce"          ,
      m_produceHistos = false ,
      "Switch on/off the production of histograms" ) 
    -> declareUpdateHandler ( &CombineParticles::propertyHandler2 , this ) ;
  //
  declareProperty
    ( "MaxCombinations" ,
      m_maxComb         ,
      "The maximal number of combinations (per channel) to be processed" ) ;
  declareProperty
    ( "StopAtMaxCombinations" ,
      m_maxCombStop,
      "Stop at the maximal number of combinations (per channel) " ) ;
  declareProperty
    ( "MaxCandidates"   ,
      m_maxCand         ,
      "The maximal number of candidates   (per channel) to be processed" ) ;
  declareProperty
    ( "StopAtMaxCandidates" ,
      m_maxCandStop,
      "Stop at the maximal number of candidates (per channel) " ) ;
  declareProperty
    ( "StopIncidentType" ,
      m_stopIncidentName ,
      "The type of incident to be fired for problematic processing" ) ;
  //
}
// ============================================================================
// virtual and protected destrcutor
// ============================================================================
CombineParticles::~CombineParticles() {} // virtual and protected destrcutor
// ============================================================================
// decode all cuts
// ============================================================================
StatusCode CombineParticles::decodeAllCuts()
{
  m_cuts.clear() ;
  
  // locate the factory
  LoKi::IHybridFactory* factory = tool<LoKi::IHybridFactory> ( m_factory , this ) ;

  // 1) decode "daughter" cuts
  for ( Map::const_iterator ic = m_daughterCuts.begin () ;
        m_daughterCuts.end() != ic ; ++ic )
  {
    if ( ic->first.empty() ) { continue ; }
    //
    { // check the correctness of particle name
      Assert ( 0 !=  pid ( ic->first ) ,
               " Invalid Particle Name is used as the key : '"  + (ic->first) + "'" ) ;
    }
    //
    MyCut item ;
    StatusCode sc = factory->get ( ic->second , item.m_cut , preambulo () ) ;
    if ( sc.isFailure() )
    { return Error ( "Unable to  decode the cut for '" + ic->first + "':" + ic->second , sc ) ; }
    item.m_name = ic->first ;
    item.m_pid  = LoKi::Particles::pidFromName ( ic->first ) ;
    item.m_moni = false     ;
    //
    debug () << "The decoded cuts for '"+ ( ic->first ) +"' are: " << item.m_cut << endreq ;
    //
    if ( monitor() && !m_daughterMonitors.empty() )
    {
      Map::const_iterator imoni = m_daughterMonitors.find ( ic->first ) ;
      if ( m_daughterMonitors.end() != imoni && !imoni->second.empty() )
      {
        sc = factory->get ( imoni->second , item.m_monitor , preambulo() ) ;
        if ( sc.isFailure() )
        { return Error ( "Unable to  decode the monitor for '" + 
                         ic->first + "':" + imoni->second , sc ) ; }
        item.m_moni = true ;
        debug () << "The decoded monitor for '"+ ( ic->first )
          + "' is: " << item.m_moni << endreq ;
      }
    }
    m_cuts.push_back ( item ) ;
  }
  // 2) decode the cut for the combination of good daughters
  {
    StatusCode sc = factory -> get ( m_combinationCut , m_acut , preambulo () ) ;
    if ( sc.isFailure () )
    { return Error ( "Unable to  decode the 'A'-cut: " + m_combinationCut  , sc ) ; }
    debug () << "The decoded cut for 'combination' is: " << m_acut << endreq ;
  }
  // 3) decode the final cut for the constructed mother particle
  {
    StatusCode sc = factory -> get ( m_motherCut , m_cut , preambulo () ) ;
    if ( sc.isFailure () )
    { return Error ( "Unable to  decode 'mother'-cut: " + m_motherCut  , sc ) ; }
    debug () << "The decoded cut for 'mother' is: " << m_motherCut << endreq ;
  }
  // monitor combinations ?
  if ( monitor() && !m_combinationMonitorCode.empty() )
  {
    StatusCode sc = factory -> get ( m_combinationMonitorCode ,
                                     m_combinationMonitor     , preambulo () ) ;
    if ( sc.isFailure () )
    { return Error ( "Unable to  decode 'combination-monitor': "
                     + m_combinationMonitorCode  , sc ) ; }
    debug () << "The decoded 'combination-monitor' is: "
             << m_combinationMonitor << endreq ;
  }
  // monitor mothers ?
  if ( monitor() && !m_motherMonitorCode.empty() )
  {
    StatusCode sc = factory -> get ( m_motherMonitorCode ,
                                     m_motherMonitor     , preambulo () ) ;
    if ( sc.isFailure () )
    { return Error ( "Unable to  decode 'mother-monitor': "
                     + m_motherMonitorCode  , sc ) ; }
    debug () << "The decoded 'mother-monitor' is: "
             << m_motherMonitor << endreq ;
  }
  //
  StatusCode sc = decodeRest ( factory ) ;
  if ( sc.isFailure() ) 
  { return Error ("Unable to decode the rest" ,sc ) ; }
  //
  release ( factory ) ;
  //
  return StatusCode::SUCCESS ;                                        // RETURN
}
// ============================================================================
/*  helper function for further decoding 
 *  @param factory the hybrid factory for decoding 
 */
// ============================================================================
StatusCode CombineParticles::decodeRest ( LoKi::IHybridFactory* /* factory */ ) 
{ return StatusCode::SUCCESS ; }
// ============================================================================
// the standard initialization of the algorithm
// ============================================================================
StatusCode CombineParticles::initialize ()
{
  // CRJ : Temporary configuration check to see if VertexFitters has been incorrectly set
  if ( !m_vertexFitNames.empty() )
  {
    warning() << "===============================================================================" 
              << endmsg;
    warning() << " The property 'VertexFitters' has been set for an instance of CombineParticles"
              << endmsg;
    warning() << " CombineParticles does not use this option so it will have no affect." << endmsg;
    warning() << " You should configure 'ParticleCombiners' instead." << endmsg;
    if ( m_particleCombinerNames.empty() ) { m_particleCombinerNames = m_vertexFitNames; }
    warning() << "===============================================================================" 
              << endmsg;
  }

  StatusCode sc = DaVinciAlgorithm::initialize () ;
  if ( sc.isFailure() ) { return sc ; }

  // check for LoKi service
  svc<IService> ( "LoKiSvc" , true ) ;

  // subscribe the incident
  incSvc() -> addListener ( this , IncidentType::BeginEvent ) ;

  // (re)lock the context again:

  // register for the algorithm context service
  IAlgContextSvc* ctx = 0 ;
  if ( registerContext() ) { ctx = contextSvc() ; }
  // setup sentry/guard
  Gaudi::Utils::AlgContext sentry ( ctx , this ) ;

  // the actual tuning/decoding
  sc = updateMajor () ;
  if ( sc.isFailure () )
  { return Error ( "Error from updateMajor", sc ) ; }  // RETURN

  // deal with the histograms:
  sc = updateHistos () ;
  
  // return
  return sc ;
}
// ============================================================================
// pre-execute hook 
// ============================================================================
StatusCode CombineParticles::pre_action () 
{
  //
  setFilterPassed ( false ) ;
  //
  if ( m_to_be_updated1 )
  {
    const StatusCode sc = updateMajor() ;
    if ( sc.isFailure() )
    { return Error ( "Error from updateMajor"  , sc ) ; }  // RETURN
  }
  //
  if ( m_to_be_updated2 )
  {
    const StatusCode sc = updateHistos () ;
    if ( sc.isFailure() )
    { return Error ( "Error from updateHistos" , sc ) ; }  // RETURN
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// get all input paritcles
// ============================================================================
StatusCode CombineParticles::get_daughters 
( CombineParticles::Selected& daughters )  
{
  //
  // get the particles from the local storage
  //
  const LHCb::Particle::ConstVector& particles
    = m_useInputParticles ? m_inputParticles : i_particles () ;
  //
  if ( m_useInputParticles && msgLevel ( MSG::INFO ) )
  {
    info() << " The external set of " << particles.size()
           << " particles is used instead of InputLocations "
           << endreq ;
  }
  //
  if ( msgLevel ( MSG::VERBOSE ) )
  {
    for ( LHCb::Particle::ConstVector::const_iterator i = particles.begin();
          i != particles.end(); ++i )
    {
      verbose() << "Input :  " << (*i)->key() << " " << (*i)->particleID().pid() << " "
                << (*i)->momentum() << endmsg ;
    }
  }
  //
  // (pre)select all daughters:
  //
  for ( MyCuts::const_iterator idau = m_cuts.begin() ; m_cuts.end() != idau ; ++idau )
  {
    Selected::Range r = daughters.add
      ( idau->m_name       ,
        particles.begin () ,
        particles.end   () ,
        LoKi::Particles::Identifier() == idau->m_pid && idau->m_cut.func () ) ;
    // some statistics:
    counter ( "# " + r.name () ) += r.size() ;
    if (msgLevel(MSG::VERBOSE))
    {
      for ( Selected::Range::const_iterator i = r.begin(); i!=r.end();++i)
        verbose() << "Select: " << (*i)->key() << " " << (*i)->particleID().pid() << " "
                  << (*i)->momentum() << endmsg ;
    }
    // make plots ?
    if ( m_daughtersPlots )
    {
      StatusCode sc = m_daughtersPlots -> fillPlots ( r.begin() , r.end() , r.name() ) ;
      if ( sc.isFailure() ) { Warning ("Error from DaughterPlots" , sc ) ; }
    }
    // monitor
    if ( monitor() && idau->m_moni )
    { idau->m_monitor ( LHCb::Particle::ConstVector( r.begin() , r.end() ) ) ; }
    //
  }
  //
  return StatusCode::SUCCESS ;  
}
// =============================================================================
/*  treat combination
 *  - check overlap 
 *  - check cuts  
 *  return true if combination is good 
 */
// =============================================================================
bool CombineParticles::treat_combination 
( const LHCb::Particle::ConstVector& combination ) const 
{
  //
  if ( UNLIKELY ( msgLevel( MSG::VERBOSE ) ) ) 
  {
    for ( LHCb::Particle::ConstVector::const_iterator i = combination.begin();
          i != combination.end(); ++i )
    {
      verbose() << "New Com: " << (*i)->key() << " " << (*i)->particleID().pid() << " "
                << (*i)->momentum() << endmsg ;
    }
  }
  //
  // check overlap using the tool 
  if ( checkOverlap()->foundOverlap ( combination ) )
  {
    if ( msgLevel( MSG::VERBOSE ) ) { verbose() << "    Overlap!" << endmsg ; }
    //
    return false ;                             // RETURN
  }
  // monitor combinations?
  if ( monitorCombinations () ) { m_combinationMonitor.fun ( combination ) ; }
  //
  return m_acut.fun ( combination ) ;
}
// =============================================================================
/*  make and check mother 
 *  - create mother prticles 
 *  - check it 
 *  - save if good 
 *  - monitor 
 *  @param pid         (INPUT) the PID for mother particle 
 *  @param combination (INPUT) the list of daughters 
 *  @param combiner    (INPUT) the combiner tool 
 *  @param saved       (UPDATE) the list of saved mother particles for monitoring 
 *  @return true if good mother particle is saved 
 */
// =============================================================================
bool CombineParticles::treat_mother  
( const LHCb::ParticleID&            pid         ,   
  const LHCb::Particle::ConstVector& combination , 
  const IParticleCombiner*           combiner    , 
  LHCb::Particle::ConstVector&       saved       ) 
{
  
  //
  // here we can create the particle (and the vertex)
  //
  LHCb::Vertex   vertex   ;
  LHCb::Particle mother ( pid ) ;
  //
  // lock the relation table Particle -> Primary Vertex:
  DaVinci::PVSentry lock ( this , &mother ) ;
  //
  // use default particle combiner to create the composed particle
  const StatusCode sc = combiner -> combine ( combination , mother , vertex ) ;
  if ( sc.isFailure() )
  {
    static const std::string mess = "Error from IParticleCombiner, skip the combination";
    Print ( mess, sc, MSG::DEBUG ) ;
    ++counter(mess);
    return false ;                                             // RETURN  
  }
  //
  if ( msgLevel ( MSG::VERBOSE ) ) verbose() << "    Passed IParticleCombiner" << endmsg ;
  //
  if ( m_combinationPlots )
  {
    StatusCode scc = m_combinationPlots->fillPlots ( &mother ) ;
    if ( scc.isFailure() ) { Warning ( "Error from CombinationPlots", scc ).ignore() ; }
  }
  //
  // apply the cut on "mother" particle
  if ( !m_cut( &mother ) )  { return false ; }                 // RETURN
  //
  // keep the good candidate:
  const LHCb::Particle* particle = this->markTree( &mother ) ;
  //
  if ( monitorMother()  ) { saved.push_back ( particle ) ; }
  //
  if ( m_motherPlots )
  {
    StatusCode scc = m_motherPlots->fillPlots ( particle ) ;
    if ( scc.isFailure() ) { Warning ( "Error from MotherPlots" , scc ).ignore() ; }
  }
  //
  // good decay, saved!
  return  true ;
}
// ============================================================================
/*  post-action
 *  - monitor number of total saved decays , 
 *  - monitor mother particles (if requred)
 *  - make the final algorithm decision 
 *  - fire the problem incident    (if needed)
 *  @param nTotal   (INPUT) total number of saved decays 
 *  @param problem  (INPUT) problem-flag 
 *  @param saved    (INPUT) saved mother particles for monitorin
 *  @return status code 
 */
// ============================================================================
StatusCode CombineParticles::post_action 
( size_t                             nTotal  ,
  const bool                         problem ,
  const LHCb::Particle::ConstVector& saved   ) 
{
  
  // If a problem occurred, do not save anything
  if ( problem ) { nTotal = 0; }
  
  // the final statistics
  counter ( "# selected" ) += nTotal;
  
  // monitor mother particles
  const bool monitorMother  = monitor() && !m_motherMonitorCode.empty() ;
  if ( monitorMother && !problem ) { m_motherMonitor ( saved ) ; }
  
  // reset the "use external input" flag
  m_useInputParticles = false ;
  
  // the final decision
  setFilterPassed ( 0 != nTotal && !problem ) ;
  
  // Finally, if configured fire an incident
  if ( problem )
  {
    if ( UNLIKELY ( msgLevel( MSG::DEBUG ) ) ) 
    { debug() <<  "A problem with combinatorics has occured"  << endmsg; }
    if ( !m_stopIncidentName.empty() )
    { incSvc()->fireIncident( Incident ( name() , m_stopIncidentName ) ) ; }
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// the standard execution of the algorithm
// ============================================================================
StatusCode CombineParticles::execute()
{
  //
  // check the execution conditions 
  //
  StatusCode sc = pre_action () ;
  if ( sc.isFailure() ) 
  { return Error("Error from pre-action conditions", sc ) ; }
  
  //
  // preselect all daughter particles 
  //
  Selected daughters ;
  sc = get_daughters ( daughters ) ;
  if ( sc.isFailure() ) 
  { return Error("Error from get-daughters method", sc ) ; }
  
  //
  // start some local activity 
  //

  // the looping/combiner engine
  typedef LoKi::Combiner_<LHCb::Particle::ConstVector>  Combiner ;
  
  /** get the default particle combiner/creator
   *  @attention Particle Combiner is used for creation of Mother Particle!
   */
  const IParticleCombiner* combiner = particleCombiner() ; // get the particle combiner
  
  // the counter of recontructed/selected decays:
  size_t nTotal = 0 ;
  
  // Flag to indicate if processing is aborted
  bool problem = false ;
  
  LHCb::Particle::ConstVector saved ;
  saved.reserve ( 100 ) ; // CRJ : Was 1000. Seems a bit big ?
  
  // a bit of monitoring
  
  StatEntity& cnt_c = counter ( s_combination ) ;
  StatEntity& cnt_m = counter ( s_mother      ) ;
  
  // loop over all decays
  for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin() ;
        m_decays.end() != idecay && !problem ; ++idecay )
  {
    // the counter of "good" selected decays
    size_t nGood = 0 ;

    // comparison criteria to remove of double counts for the same pid
    const LoKi::Particles::PidCompare compare = LoKi::Particles::PidCompare () ;

    // create the actual object for looping
    Combiner loop ;

    // fill it with the input data
    const Decays::Decay::Items& items = idecay->children() ;
    for ( Decays::Decay::Items::const_iterator child = items.begin() ;
          items.end() != child ; ++child ) { loop.add ( daughters ( child->name() ) ) ; }

    // check for max
    if ( msgLevel( MSG::DEBUG ) || 0 < m_maxComb ) 
    { counter ( "# max size" + idecay->toString() ) += loop.size() ; }
    //
    problem = problem || tooMuchCombinations ( loop.size() , *idecay ) ;
    if ( problem ) { continue ; }  // CONITNUE
    
    //
    // here we can start the actual looping
    for ( ; loop.valid() ; loop.next() )  // we are in the loop!
    {
      //
      problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
      if ( problem ) { break ; }                                  // BREAK!!! 
      //
      if ( !loop.unique ( compare )  ) { continue ; }              // CONTINUE
      
      // get the actual('current') combination:
      LHCb::Particle::ConstVector combination ( loop.dim() ) ;
      
      loop.current ( combination.begin() ) ;
      
      //
      // check the combinations for overlaps and cuts 
      //
      const bool pass_combination = treat_combination ( combination ) ;
      cnt_c += pass_combination ;
      if ( !pass_combination ) { continue ; }
      
      //
      // here we can create the mother particle, the vertex, 
      // check them and save in TES 
      const bool pass_mother = treat_mother
        ( idecay->mother().pid() , 
          combination            , 
          combiner               , 
          saved                  ) ;
      //
      cnt_m += pass_mother ;
      if ( !pass_mother ) { continue ; }
      
      // increment number of good decays
      ++nGood; 
      
    } // the loop over combinations
    
    // some statistics
    if ( problem ) nGood = 0; // Nothing saved if a problem occurred
    counter ( "#" + idecay->toString() ) += nGood;
    nTotal += nGood;
    
  } // the loop over the decays
  
  //
  // Final!
  // 
  return post_action ( nTotal , problem , saved ) ;
}
// ============================================================================
/*  Inform that a new incident has occured
 *  @see IIncidentListener
 */
// ============================================================================
void CombineParticles::handle ( const Incident& )
{
  m_inputParticles.clear()    ;      // clear the input container
  m_useInputParticles = false ;      // reset the flag
}
// ============================================================================
/** set the input particles
 *  @see ISetInputParticles
 *  @param input the vector of input particles
 *  @return status code
 */
// ============================================================================
StatusCode CombineParticles::setInput
( const LHCb::Particle::ConstVector& input )
{
  m_inputParticles    = input ;
  m_useInputParticles = true  ;
  info() << "The external set of " << input.size()
         << " particles will be used as input instead of InputLocations "
         << endreq ;
  return StatusCode ( StatusCode::SUCCESS , true ) ;
}
// =============================================================================
// the handle for the property update
// =============================================================================
void CombineParticles::propertyHandler1 ( Property& p )
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  /// mark as "to-be-updated"
  m_to_be_updated1 = true ;
  Warning ( "The structural property '" + p.name() +
            "' is updated. It will take affect at the next 'execute'" ,
            StatusCode( StatusCode::SUCCESS, true ) ) ;
  info () << "The updated property is: " << p << endreq ;
}
// =============================================================================
// the handle for the property update
// =============================================================================
void CombineParticles::propertyHandler2 ( Property& p )
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  /// mark as "to-be-updated"
  m_to_be_updated2 = true ;
  debug () << "The histogramming property is updated: " << p << endreq ;
}
// ============================================================================
// perform the update of the major properties
// ============================================================================
StatusCode CombineParticles::updateMajor  ()
{
  // 1) decode all decay descriptors:
  {
    if ( m_decayDescriptors.empty() )
    { m_decayDescriptors.push_back ( getDecayDescriptor() ) ; }
    // locate the decoder tool:
    IDecodeSimpleDecayString* decoder =
      tool <IDecodeSimpleDecayString> ( "DecodeSimpleDecayString:PUBLIC" , this ) ;
    //
    m_decays = Decays::decays ( m_decayDescriptors , decoder ) ;
    if ( m_decays.empty() )
    { return Error ( "Unable to decode DecayDescriptor(s) " ) ; }    // RETURN
    // release the tool (not needed anymore)
    release ( decoder );
  }
  // 2) check the list of daughters, involved into all decays:
  {
    typedef std::set<std::string> PIDs ;
    PIDs pids ;
    for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin() ;
          m_decays.end()  != idecay ; ++idecay )
    {
      const Decays::Decay::Items& ds= idecay->daughters () ;
      for ( Decays::Decay::Items::const_iterator ic = ds.begin() ;
            ds.end ()  != ic ; ++ic ) { pids.insert ( ic->name()  ) ; }
    }
    // default cut: accept all
    if  ( m_daughterCuts.end() == m_daughterCuts.find ("") ) { m_daughterCuts[""] = "ALL" ; }
    // check that all daughters has the cuts associated:
    for ( PIDs::const_iterator ipid = pids.begin() ; pids.end () != ipid ; ++ipid )
    {
      // get the antiparticle:
      const std::string anti = LoKi::Particles::antiParticle ( *ipid ) ;

      { // play with daughter cuts
        // find the cuts for particle
        Map::const_iterator i1 = m_daughterCuts.find ( *ipid ) ;
        // find the cuts for antiparticle
        Map::const_iterator i2 = m_daughterCuts.find (  anti ) ;
        //
        // both cuts are already specified explicitly?
        if      ( m_daughterCuts.end() != i1 &&  m_daughterCuts.end() != i2 ) { } // nothing to do
        else if ( m_daughterCuts.end() != i1 ) // the cut for particle is already specified
        { m_daughterCuts[ anti ] = m_daughterCuts[ *ipid ] ; } // use the same cust for antiparticle
        else if ( m_daughterCuts.end() != i2 ) // the cut for anti-particle is already specified
        { m_daughterCuts[ *ipid ] = m_daughterCuts[ anti ] ; } // use the same cuts as particle
        else // none cuts for particle&antiparticle are specified: use the default cuts
        {
          m_daughterCuts [ *ipid ] = m_daughterCuts[""] ;  // the default cuts for particle
          m_daughterCuts [ anti  ] = m_daughterCuts[""] ;  // the default cuts for antiparticle
        }
      }

      // play the same game with daughter monitors
      if ( monitor() && !m_daughterMonitors.empty() )
      {
        // find the cuts for particle
        Map::const_iterator j1 = m_daughterMonitors.find ( *ipid ) ;
        // find the cuts for antiparticle
        Map::const_iterator j2 = m_daughterMonitors.find (  anti ) ;
        //
        // both monitors are already specified explicitly?
        if      ( m_daughterMonitors.end() != j1 &&
                  m_daughterMonitors.end() != j2 ) { } // nothing to do
        // the monitor for particle is already specified
        else if ( m_daughterMonitors.end() != j1 )
        { m_daughterMonitors[ anti  ] = m_daughterMonitors[ *ipid ] ; } // use the same monitor for antiparticle
        // the monitor for anti-particle is already specified
        else if ( m_daughterMonitors.end() != j2 )
        { m_daughterMonitors[ *ipid ] = m_daughterMonitors[ anti  ] ; } // use the same monitor as particle
      }
      
    }
    
    if ( UNLIKELY ( msgLevel ( MSG::DEBUG ) ) ) 
    {
      debug () << "The updated map of daughter cuts is : "
               << Gaudi::Utils::toString ( m_daughterCuts ) << endreq ;
    }
    
    if ( UNLIKELY ( msgLevel( MSG::DEBUG ) && monitor() && !m_daughterMonitors.empty() ) )
    {
      debug () << "The updated map of daughter monitors is : "
               << Gaudi::Utils::toString ( m_daughterMonitors ) << endreq ;
    }
    
  }
  // 3) decode the cuts for the daughters:
  {
    StatusCode sc = decodeAllCuts () ;
    if ( sc.isFailure() )
    { return Error ( "The error from decodeAlLCuts", sc ) ; }       // RETURN
  }
  //
  m_to_be_updated1 = false ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// perform the update of the histogram properties
// ============================================================================
StatusCode CombineParticles::getHistoTool
( IPlotTool*&        histoTool ,
  const std::string& name      ,
  const std::string& path      )
{
  if ( validPlots ( name ) )
  {
    histoTool = tool<IPlotTool>( name, this ) ;
    if ( !path.empty() )
    {
      StatusCode sc = m_daughtersPlots -> setPath ( path ) ;
      if ( sc.isFailure() )
      { return Error ( "Unable to set Plots Path "+path+" for tool "+name , sc ) ; }
    }
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
// perform the update of the histogram properties
// ============================================================================
StatusCode CombineParticles::updateHistos ()
{
  // ==========================================================================
  if ( m_daughtersPlots   )
  { releaseTool ( m_daughtersPlots   ) ; m_daughtersPlots   = 0 ; }
  if ( m_combinationPlots )
  { releaseTool ( m_combinationPlots ) ; m_combinationPlots = 0 ; }
  if ( m_motherPlots      )
  { releaseTool ( m_motherPlots      ) ; m_motherPlots      = 0 ; }
  // ==========================================================================
  if ( m_produceHistos )
  {
    StatusCode sc = getHistoTool( m_daughtersPlots, m_daughtersPlotsName, m_daughtersPlotsPath );
    if (sc) sc = getHistoTool( m_combinationPlots, m_combinationPlotsName, m_combinationPlotsPath );
    if (sc) sc = getHistoTool( m_motherPlots, m_motherPlotsName, m_motherPlotsPath );
    if (!sc) return sc ;
  }
  // ========================================================================
  //
  m_to_be_updated2 = false ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// the standard finalization of the algorithm
// ============================================================================
StatusCode CombineParticles::finalize   ()           //  standard  finalization
{
  m_incSvc = 0 ;
  // reset functors:
  m_acut               = _ABOOL ( true  ) ;
  m_cut                = _PBOOL ( false ) ;
  m_cuts.clear() ;
  m_combinationMonitor = _CBOOL ( false ) ;
  m_motherMonitor      = _CBOOL ( false ) ;
  m_to_be_updated1     = true ;

  // finalize the base class
  return DaVinciAlgorithm::finalize () ;              // finalize the base class
}
// ============================================================================
/// The factory (needed for the proper instantiation)
DECLARE_ALGORITHM_FACTORY(CombineParticles)
// ============================================================================
// The END
// ============================================================================
