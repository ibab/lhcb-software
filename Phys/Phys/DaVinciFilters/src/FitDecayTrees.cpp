// $Id$
// ============================================================================
// Include files
// ============================================================================
#include <algorithm>
#include <functional>
// ============================================================================
// DecayTreeFitter
// ============================================================================
#include "DecayTreeFitter/Fitter.h"
// ============================================================================
// local
// ============================================================================
#include "FitDecayTrees.h"
// ============================================================================
/** @file
 *  Implementation file for algorithm FitDecayTrees
 *  @author Vanya BELAYEV Ivan.Belyaev@cern.ch
 *  @author Vanya BELYAEV   Ivan.Belyaev@nikhef.nl
 *  @date 2010-07-09
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
/* standard constructor
 *  @see DVAlgorithm
 *  @see GaudiTupleAlg
 *  @see GaudiHistoAlg
 *  @see GaudiAlgorithm
 *  @see Algorithm
 *  @see AlgFactory
 *  @see IAlgFactory
 *  @param name the algorithm instance name
 *  @param pSvc pointer to Service Locator
 */
// ============================================================================
FitDecayTrees::FitDecayTrees                       // standard contructor
( const std::string& name ,                        // the algorithm instance name
  ISvcLocator*       pSvc )                        // pointer to Service Locator
  : FilterDesktop ( name , pSvc )
// Track extrapolator
  , m_extrapolator     ( 0 )
  , m_extrapolatorName ( "TrackStateProvider" )
// chi2 cut for decay tree fit
  , m_chi2cut  ( 10 )
// list of mass-constraints to be applied
  , m_mc_1 ()
// list of mass-constraints to be applied
  , m_mc_2 ()
// use PV-constraint ?
  , m_use_PV_Constraint ( false )
{
  declareProperty
    ( "TrackStateProvider"                        ,
      m_extrapolatorName                         ,
      "Track Extrapolator to be used"            )
    -> declareUpdateHandler
    ( &FitDecayTrees::updateExtrapolator  , this ) ;

  //
  declareProperty
    ( "MaxChi2PerDoF" ,
      m_chi2cut       ,
      "The maximal value for chi2/nDoF" ) ;
  //
  declareProperty
    ( "UsePVConstraint"   ,
      m_use_PV_Constraint ,
      "Use Primary Vertex pointing constraint for Decay Tree Fitter " ) ;
  //
  declareProperty
    ( "MassConstraints"                          ,
      m_mc_1                                     ,
      "List of particles to be mass-constrained" )
    -> declareUpdateHandler
    ( &FitDecayTrees::updateConstraints   , this ) ;
  //
  StatusCode sc = setProperty ( "CloneFilteredParticles" , true ) ;
  Assert ( sc.isSuccess() , "Unable to set 'CloneFilteresParticles'" ) ;
  {
    auto p = Gaudi::Utils::getProperty ( this , "CloneFilteredParticles" ) ;
    if ( p && ! p->updateCallBack() )
    { p -> declareUpdateHandler ( &FitDecayTrees::updateCloneProp , this ) ; }
  }
}
// ============================================================================
// virtual & protected destructor
// ============================================================================
FitDecayTrees::~FitDecayTrees () {}                      // standard contructor
// ============================================================================
// intialize the algorithm
// ============================================================================
StatusCode FitDecayTrees::initialize ()
{
  StatusCode sc = FilterDesktop::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  //
  if ( !cloneFilteredParticles() )
  {
    warning()
      << "The property 'CloneFilteredParticles' reset to be True"
      << endmsg ;
    setCloneFilteredParticles ( true ) ;
  }
  /// decode mass-constraints
  return decodeConstraints () ;
}
// ============================================================================
// finalize the algorithm
// ============================================================================
StatusCode FitDecayTrees::finalize ()
{ return FilterDesktop::initialize () ; }
    /// update-handler for the property "Track Extrapolator"
void FitDecayTrees::updateExtrapolator ( Property& /* p */ )      // update the extrapolator
{
  // no action if not yet initialized
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  MsgStream& log = info() ;
  log << "New Track extrapolator to be used '" << m_extrapolatorName <<  "'" ;
  const IAlgTool* e = extrapolator() ;
  if ( 0 != e ) { log  << " : " << e->type() << "/" << e->name() ;}
  log << endmsg ;
  //
}


// ============================================================================
// update constraints
// ============================================================================
void FitDecayTrees::updateConstraints ( Property& /* p */ )
{
  // no action if not yet initialized
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  StatusCode sc = decodeConstraints () ;
  Assert ( sc.isSuccess() , "Unable to decode Mass-Constraints" , sc ) ;
  //
}
// ==========================================================================
// update handler for 'CloneFilteredParticles' property
// ==========================================================================
void FitDecayTrees::updateCloneProp ( Property& /* p */ )
{
  setCloneFilteredParticles ( true ) ;
  // no action if not yet initialized
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  warning()
    << "The property 'CloneFilteredParticles' reset to be True" << endmsg ;
  //
  setCloneFilteredParticles ( true ) ;
}
// ============================================================================
// get the refitted tree
// ============================================================================
LHCb::DecayTree FitDecayTrees::reFitted ( const LHCb::Particle* p ) const
{
  //
  if ( 0 == p ) { return LHCb::DecayTree () ; }
  //
  typedef DecayTreeFitter::Fitter Fitter ;
  //
  const LHCb::VertexBase* pv = 0 ;
  if ( use_PV_Constraint ()  )
  {
    pv = bestVertex ( p ) ;
    if ( 0 == pv )
    { Warning ("No bestVertex is found!", StatusCode::FAILURE , 1 ).ignore() ; }
  }
  //
  // initialize fitter
  std::auto_ptr<Fitter>  fitter ;
  //
  // instantiate the fitter
  fitter.reset ( pv ? new Fitter( *p , *pv, extrapolator() ) :
                      new Fitter( *p, extrapolator() ) ) ;
  //
  // apply mass-constraints (if needed)
  for ( const auto& ipid : m_mc_2 )
  { fitter->setMassConstraint ( ipid )  ; }
  //
  // fit!
  fitter->fit() ;
  //
  // get the status
  Fitter::FitStatus status = fitter->status() ;
  if ( Fitter::Success != status )
  {
    Warning ( "Error from fitter, status" , 110 + status , 1 ) ;
    return LHCb::DecayTree () ;
  }
  //
  // apply chi2/nDoF cut:
  if ( fitter->chiSquare() > m_chi2cut * fitter->nDof() )
  { return LHCb::DecayTree () ; }
  //
  // finally get the refitted tree from the fitter
  return fitter->getFittedTree () ;
}
// ============================================================================
/*  the major method for filter input particles
 *  @param input    (INPUT) the input  container of particles
 *  @param filtered (OUPUT) the output container of particles
 *  @return Status code
 */
// ============================================================================
StatusCode FitDecayTrees::filter
( const LHCb::Particle::ConstVector& input    ,
  LHCb::Particle::ConstVector&       filtered )
{
  //
  // Filter particles!!  - the first important most important line :-)
  //
  LHCb::Particle::ConstVector good ;
  good.reserve ( input.size  () ) ;
  std::copy_if ( input.begin () ,
                 input.end   () ,
                 std::back_inserter ( good ) , std::cref(predicate()) ) ;
  if ( good.empty() ) { return StatusCode::SUCCESS ; }
  //
  for ( const auto& p : good )
  {
    if ( !p ) { continue ; }
    //
    LHCb::DecayTree tree = reFitted ( p ) ;
    //
    if ( !tree ) { continue ; }
    //
    filtered.push_back ( tree.release() ) ;
  }
  //
  // mark & store filtered particles in DVAlgorithm local container
  markNewTrees ( filtered ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// decode mass-constraints
// ============================================================================
StatusCode FitDecayTrees::decodeConstraints ()    // decode constraints
{
  m_mc_2.clear() ;
  //
  if ( m_mc_1.empty() )
  {
    debug() << "No Mass-Constraints will be applied" << endmsg ;
    return StatusCode::SUCCESS ;
  }
  ///
  const LHCb::IParticlePropertySvc* ppsvc =
    svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc" ) ;
  //
  std::set<LHCb::ParticleID> pids ;
  for ( const auto& ic : m_mc_1 )
  {
    const LHCb::ParticleProperty* pp = ppsvc->find ( ic ) ;
    if ( !pp ) { return Error ( "Unable to find particle '" + ic + "'") ; }
    pids.emplace( pp->pid().abspid() ) ;
  }
  //
  m_mc_2.insert( m_mc_2.end() , pids.begin() , pids.end() ) ;
  //
  MsgStream& log = info() ;
  std::set<std::string> parts ;
  for ( const auto& ipid : m_mc_2 )
  {
    auto pp = ppsvc->find ( ipid ) ;
    if ( pp ) { parts.insert ( pp->particle () ) ; }
  }
  log << "Mass Constraints will be applied for : " <<
    Gaudi::Utils::toString ( parts ) << endmsg ;
  //
  release ( ppsvc ) ;
  //
  return StatusCode::SUCCESS ;
}
// ===========================================================================
// attention:restore the original action by DVAlgorithm
// ===========================================================================
StatusCode FitDecayTrees::_saveInTES ()
{
  return FilterDesktop::BaseClass::_saveInTES() ;
}
// ===========================================================================
// attention:restore the original action by DVAlgorithm
// ===========================================================================
void FitDecayTrees::writeEmptyTESContainers()
{
  FilterDesktop::BaseClass::writeEmptyTESContainers() ;
}
// ===========================================================================
/// the factory
DECLARE_ALGORITHM_FACTORY(FitDecayTrees)
// ============================================================================
// The END
// ============================================================================
