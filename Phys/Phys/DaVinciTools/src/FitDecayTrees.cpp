// $Id: FitDecayTrees.cpp,v 1.1 2010-07-10 14:22:38 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToStream.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IDecay.h"
#include "LoKi/Decays.h"
// ============================================================================
// DecatTreeFitter 
// ============================================================================
#include "DecayTreeFitter/Fitter.h"
// ============================================================================
// local
// ============================================================================
#include "FilterDesktop.h"
// ============================================================================
/** @class FitDecayTrees 
 *  Simple algorithm to perform the decay tree fit for 'good' particles
 *
 *  The algorothm selectd certains decay trees form the input and 
 *  perform the refit of <b>cloned</b> decay trees.
 * 
 *  @code 
 * 
 *  from Configurables import FitDecayTrees 
 *
 *  myAlg = FitDecayTrees ( 
 *     ...                                                , 
 *     Code          = " [B_s0 -> J/psi(1S) phi(1020)]CC" , 
 *     MaxChi2PerDoF = 10                                 ,
 *     ...                                                ,
 *     ) 
 *    
 *  @endcode  
 *
 *  Optionally one can apply the Primary Vertex pointing constraint:
 * 
 *  @code 
 * 
 *  myAlg = FitDecayTrees ( 
 *     ...                                                  , 
 *     Code            = " [B_s0 -> J/psi(1S) phi(1020)]CC" , 
 *     MaxChi2PerDoF   = 10                                 ,
 *     UsePVConstraint = True                               , 
 *     ...                                                  ,
 *     ) 
 *    
 *  @endcode  
 *
 *  Also optionally one can apply certainmass-constraints:
 * 
 *  @code 
 * 
 *  myAlg = FitDecayTrees ( 
 *     ...                                                  , 
 *     Code            = " [B_s0 -> J/psi(1S) phi(1020)]CC" , 
 *     MaxChi2PerDoF   = 10                                 ,
 *     MassConstraints = [ 'J/psi(1S)' ]                    , 
 *     ...                                                  ,
 *     ) 
 *    
 *  @endcode  
 *
 *  Clearly all constraints can be combined together:
 * 
 *  @code 
 * 
 *  myAlg = FitDecayTrees ( 
 *     ...                                                  , 
 *     Code            = " [B_s0 -> J/psi(1S) phi(1020)]CC" , 
 *     MaxChi2PerDoF   = 10                                 ,
 *     UsePVConstraint = True                               , 
 *     MassConstraints = [ 'J/psi(1S)' ]                    , 
 *     ...                                                  ,
 *     ) 
 *    
 *  @endcode  
 *
 *  See also 
 *  <a href="https://twiki.cern.ch/twiki/bin/view/LHCb/FAQ/DaVinciFAQ#How_to_perform_the_refit_of_the"/>DaVinci TWiki FAQ </a>
 * 
 *  @author Vanya BELYAEV   Ivan.Belyaev@nikhef.nl
 *  @date 2010-07-09
 */
class FitDecayTrees : public FilterDesktop 
{
  // ==========================================================================
  /// friend factory for instantiation 
  friend class AlgFactory<FitDecayTrees> ;
  // ==========================================================================
public:
  // ==========================================================================
  /// intialize the algorithm 
  virtual StatusCode initialize () 
  {
    StatusCode sc = FilterDesktop::initialize () ;
    if ( sc.isFailure() ) { return sc ; }
    //
    if ( cloneFilteredParticles() ) 
    {
      warning() 
        << "The property 'CloneFilteredParticles' reset to be True"
        << endmsg ;
      setCloneFilteredParticles ( true ) ;
    }
    /// decode mass-constraints 
    return decodeConstraints () ;
  }
  // ==========================================================================
protected:
  // ==========================================================================
  /** standard constructor 
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
  FitDecayTrees                                // standard contructor 
  ( const std::string& name ,                  // the algorithm instance name 
    ISvcLocator*       pSvc )                  // pointer to Service Locator
    : FilterDesktop ( name , pSvc ) 
  // invalid decay finder 
    , m_finder   ( Decays::Trees::Types_<const LHCb::Particle*>::Invalid() ) 
  // chi2 cut for decay tree fit  
    , m_chi2cut  ( 10 )
  // list of mass-constraints to be applied 
    , m_mc_1 () 
  // list of mass-constraints to be applied 
    , m_mc_2 ()
  // use PV-constraint ? 
    , m_use_PV_Constraint ( false ) 
  {
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
      Property* p = Gaudi::Utils::getProperty ( this , "CloneFilteredParticles" ) ;
      if ( 0 != p && 0 == p->updateCallBack() ) 
      { p -> declareUpdateHandler ( &FitDecayTrees::updateCloneProp , this ) ; }
    }
  }
  // ==========================================================================
  /// virtual & protected destructor 
  virtual ~FitDecayTrees() {} ;               // virtual & protected destructor  
  // ==========================================================================
public:
  // ==========================================================================
  /** the major method for filter input particles 
   *  @param input    (INPUT) the input  container of particles
   *  @param filtered (OUPUT) the output container of particles
   *  @return Status code 
   */
  virtual StatusCode filter 
  ( const LHCb::Particle::ConstVector& input    , 
    LHCb::Particle::ConstVector&       filtered ) const ;
  // ==========================================================================
  /// decode the code
  virtual StatusCode decodeCode () ;
  // ==========================================================================
  void updateConstraints ( Property& /* p */ )      // update the constraints 
  {
    // no action if not yet initialized 
    if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
    // 
    StatusCode sc = decodeConstraints () ;
    Assert ( sc.isSuccess() , "Unable to decode Mass-Constraints" , sc ) ;
    //
  }
  // ==========================================================================
  /// update handler for 'CloneFilteredParticles' property 
  void updateCloneProp ( Property& /* p */ )
  {
    setCloneFilteredParticles ( true ) ;
    // no action if not yet initialized 
    if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
    //
    warning() 
      << "The property 'CloneFilteredParticles' reset to be True"
      << endmsg ;
    setCloneFilteredParticles ( true ) ;
  }
  /// decode mass-consraints 
  StatusCode decodeConstraints () ;
  // ==========================================================================
private:
  // ==========================================================================
  /// the default constructor is disabled 
  FitDecayTrees () ;                      // the default consructor is disabled
  /// copy constructor is disabled 
  FitDecayTrees ( const FitDecayTrees& ) ;      // copy constructor is disabled 
  /// assignement operator is disabled 
  FitDecayTrees& operator=( const FitDecayTrees& ) ;          // no assignement
  // ==========================================================================
private:
  // ==========================================================================
  /// the decay finder 
  Decays::IDecay::Finder m_finder  ;     //                    the decay finder 
  /// the chi2-cut for the decay tree fit 
  double         m_chi2cut ;             // the chi2-cut for the decay tree fit 
  /// the list of mass-constraints to be applied 
  std::vector<std::string>      m_mc_1 ; //        the list of mass-constraints
  /// the list of mass-constraints to be applied 
  std::vector<LHCb::ParticleID> m_mc_2 ; //        the list of mass-constraints
  /// use PV-constraint for re-Fit ? 
  bool   m_use_PV_Constraint ;           //      use PV-constraint for re-Fit ? 
  // ==========================================================================
};
// ============================================================================  
/*  the major method for filter input particles 
 *  @param input    (INPUT) the input  container of particles
 *  @param filtered (OUPUT) the output container of particles
 *  @return Status code 
 */
// ============================================================================  
StatusCode FitDecayTrees::filter 
( const LHCb::Particle::ConstVector& input    , 
  LHCb::Particle::ConstVector&       filtered ) const 
{
  //
  typedef DecayTreeFitter::Fitter Fitter ;
  //
  LHCb::Particle::ConstVector good ;
  m_finder.findDecay ( input , good ) ;
  if ( good.empty() ) { return StatusCode::SUCCESS ; }
  //
  for ( LHCb::Particle::ConstVector::const_iterator ip = good.begin() ; 
        good.end() != ip ; ++ip ) 
  {
    //
    const LHCb::Particle* p = *ip ;
    if ( 0 == p ) { continue ; }
    //
    const LHCb::VertexBase* pv = 0 ;
    if ( m_use_PV_Constraint ) 
    {
      pv = bestVertex ( p ) ; 
      if ( 0 == pv ) 
      {
        Warning ("No bestVertex is found!", StatusCode::FAILURE , 1 ).ignore() ;
        continue ;
      }
    }
    /// initialize fitter 
    std::auto_ptr<Fitter>  fitter ;
    /// instantiate the fitter 
    fitter.reset ( 0 == pv ? new Fitter ( *p ) : new Fitter ( *p , *pv ) ) ;
    /// apply mass-constraints (if needed) 
    for ( std::vector<LHCb::ParticleID>::const_iterator ipid = m_mc_2.begin() ; 
          m_mc_2.end() != ipid ; ++ipid ) 
    { fitter->setMassConstraint ( *ipid )  ; }
    /// fit! 
    fitter->fit() ;
    // get the status
    Fitter::FitStatus status = fitter->status() ;
    if ( Fitter::Success != status ) 
    { 
      Warning ( "Error from fitter, status" , 110 + status , 1 ) ; 
      continue ;
    }
    /// apply chi2/nDoF cut: 
    if ( fitter->chiSquare() > m_chi2cut * fitter->nDof() ) { continue ; }  
    /// finally get the refitted tree from the fitter
    LHCb::DecayTree tree = fitter->getFittedTree() ;
    if ( !tree ) { continue ; }
    //
    filtered.push_back ( tree.release() ) ;
  }
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
    MsgStream& log = debug() ;
    log << " No Mass-Constraints will be applied " << endreq ;
    return StatusCode::SUCCESS ;
  }
  ///
  const LHCb::IParticlePropertySvc* ppsvc = 
    svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc" ) ;
  //
  std::set<LHCb::ParticleID> pids ;
  for ( std::vector<std::string>::const_iterator ic = m_mc_1.begin() ;
        m_mc_1.end() != ic ; ++ic ) 
  {
    const LHCb::ParticleProperty* pp = ppsvc->find ( *ic ) ;
    if ( 0 == pp ) { return Error ( "Unable to find particle '" + (*ic) + "'") ; }
    pids.insert ( LHCb::ParticleID ( pp->pid().abspid() ) ) ;
  }  
  //
  m_mc_2.insert ( m_mc_2.end() , pids.begin() , pids.end() ) ;
  //
  MsgStream& log = info() ;
  std::set<std::string> parts ;
  for ( std::vector<LHCb::ParticleID>::const_iterator ipid = m_mc_2.begin() ;
        m_mc_2.end() != ipid ; ++ipid ) 
  {
    const LHCb::ParticleProperty* pp = ppsvc->find ( *ipid ) ;
    if ( 0 != pp ) { parts.insert ( pp->particle () ) ; }
  }
  log << " Mass Constraints will be applied for : " <<
    Gaudi::Utils::toString ( parts ) << endreq ;
  //
  release ( ppsvc ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// decode the code
// ============================================================================
StatusCode FitDecayTrees::decodeCode () 
{
  /// get the factory
  Decays::IDecay* factory = tool<Decays::IDecay>( "LoKi::Decay" , this ) ;
  
  /// construct the tree 
  Decays::IDecay::Tree tree = factory->tree ( code () ) ;
  
  /// check the validity 
  if ( !tree ) 
  {
    StatusCode sc = tree.validate ( ppSvc() ) ;
    if ( sc.isFailure() ) 
    {
      return Error ( "Uable to validate the tree '" + 
                     tree.toString() + "' built from the descriptor '"
                     + code() + "'" , sc ) ;
    }
  }
  //
  if ( !tree ) 
  {
    return Error ( "Invalid tree '" + 
                   tree.toString()  + 
                   "' built from the descriptor '"
                   + code() + "'" ) ;
  }
  // reser the finder 
  m_finder = Decays::IDecay::Finder ( tree ) ;
  if ( !m_finder ) 
  {
    return Error ( "Invalid finder '" + 
                   m_finder.tree().toString()  + 
                   "' built from the descriptor '"
                   + code() + "'" ) ;
  }  
  /// release the factory ( not needed anymore) 
  release ( factory ) ;
  ///
  return StatusCode::SUCCESS ;
}
// ===========================================================================
/// the factory 
DECLARE_ALGORITHM_FACTORY(FitDecayTrees) 
// ============================================================================
// The END 
// ============================================================================
