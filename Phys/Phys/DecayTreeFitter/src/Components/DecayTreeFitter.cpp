// $Id: DecayTreeFitter.cpp,v 1.2 2010-05-28 17:04:43 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// TrackInterfaces 
// ============================================================================
#include "TrackInterfaces/ITrackExtrapolator.h"
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IDecayTreeFit.h"
#include "Kernel/IParticleReFitter.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/ParticleParams.h"
// ============================================================================
// DecayTreeFitter 
// ============================================================================
#include "DecayTreeFitter/Fitter.h"
#include "DecayTreeFitter/VtxFitStatus.h"
// ============================================================================
class DecayTreeFit
  : public extends2<GaudiTool,IDecayTreeFit,IParticleReFitter>
{
  // ==========================================================================
  // the friend factory, needed for instantiation
  friend class ToolFactory<DecayTreeFit> ;
  // ==========================================================================
public: // IDecayTreeFit
  // ==========================================================================
  /** fit the decay tree 
   *  @param decay  (INPUT) the decay tree to fit 
   *  @param origin (INPUT) the origin vertex 
   *  @return status code 
   *  @see IDecayTreeFit
   *  @see IDecayTreeFit::fit 
   */                                                           
  virtual StatusCode fit 
  ( const LHCb::Particle*   decay      , 
    const LHCb::VertexBase* origin = 0 ) const ;
  /** get the fitted parameters for the particle 
   *  @param particle (INPUT) the particle, NULL corresponds to the decay head  
   *  @return the fitted parameters, NULL for invalid argument/fit failure 
   *  @see IDecayTreeFit
   *  @see IDecayTreeFit::fitted 
   */
  virtual const Fitted* fitted ( const LHCb::Particle* p = 0 ) const ;
  // ==========================================================================  
  /** get the fit results in form of self-consistent decay tree 
   *  @see IDecayTreeFit
   *  @see IDecayTreeFit::fittedTree
   */
  virtual LHCb::DecayTree fittedTree () const ;
  // ==========================================================================  
  /** add the constraint 
   *  @param pid (INPUT) particle-ID to be constrained
   *  @see IDecayTreeFit
   *  @see IDecayTreeFit::addConstraint
   */
  virtual void addConstraint ( const LHCb::ParticleID& pid ) ;
  // ==========================================================================
public:  // IParticleReFitter 
  // ==========================================================================
  /** The basic method for "refit" of the particle
   *
   *  @code 
   *
   *  // locate the tool 
   *  const IParticleReFitter* refitter = tool<IParticleRefitter>( ... ) ;
   * 
   *  // particle to be refit 
   *  LHCb::Particle* p = ... ;
   *
   *  StatusCode sc = refitter->reFit ( *p ) ;
   *  if ( sc.isFailure() ) { Warning("Error in reFit" , sc ) ; }
   *
   *  @endcode 
   *
   *  @see IParticleReFitter
   *  @see IParticleReFitter::reFit 
   *
   *  @param particle reference to the particle 
   *  @return status code 
   */  
  virtual StatusCode reFit ( LHCb::Particle& particle ) const ;
  // ==========================================================================
public:   // AlgTool/GaudiTool
  // ==========================================================================
  /// initialize the tool 
  virtual StatusCode initialize () ; // initialize the tool 
  /// finalize  the tool 
  virtual StatusCode finalize   () ; // finalize the tool 
  // ==========================================================================
protected:  // constuctors/desctructor 
  // ==========================================================================
  /** constructor 
   *  @param type tool type (?)
   *  @paran name tool instance name 
   *  @param parent tool parent 
   */
  DecayTreeFit 
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) 
    : base_class ( type , name , parent )
  //
    , m_extrapolator ( 0 ) 
    , m_fitter           ()
  //
    , m_global_pids      () 
    , m_locals_pids      ()
  //
    , m_extrapolatorName ()
    , m_constraints      () 
  {
    //
    declareProperty 
      ( "TrackExtrapolator"                        , 
        m_extrapolatorName                         ,
        "Track Extrapolator to be used"            ) ;
    //
    declareProperty
      ( "MassConstraints"                          , 
        m_constraints                              , 
        "List of particles to be mass-constrained" ) ;
  }
  /// virtual and protected destructor 
  virtual ~DecayTreeFit () {} ;
  // ==========================================================================
private:
  // ==========================================================================
  /// default constructor is disabled 
  DecayTreeFit () ;                          // default constructor is disables 
  /// copy constructor is disables 
  DecayTreeFit           ( const DecayTreeFit& ) ;            //        no copy 
  /// assignement operator is disables 
  DecayTreeFit& operator=( const DecayTreeFit& ) ;            // no assignement 
  // ==========================================================================
protected:
  // ==========================================================================
  /// get track extrapolator 
  inline ITrackExtrapolator* extrapolator() const 
  {
    if ( 0 != m_extrapolator        ) { return m_extrapolator ; } // RETURN 
    if ( m_extrapolatorName.empty() ) { return              0 ; } // REUTRN
    m_extrapolator = tool<ITrackExtrapolator> ( m_extrapolatorName , this ) ;
    return m_extrapolator ;
  }
  // ==========================================================================
private:
  // ==========================================================================
  /// track extrapolator 
  mutable ITrackExtrapolator* m_extrapolator ;        // the track extrapolator 
  // ==========================================================================
  // the fitter 
  // ==========================================================================
  mutable std::auto_ptr<DecayTreeFitter::Fitter>  m_fitter ; // the fitter 
  // ==========================================================================
  typedef std::vector<LHCb::ParticleID>  PIDs ;
  /// list of mass-constrains (global)
  PIDs         m_global_pids ;              // list of mass-constrains (global)
  /// list of mass-constrains (local) 
  mutable PIDs m_locals_pids ;              //  list of mass-constrains (local) 
  // ==========================================================================
  // properties 
  // ==========================================================================
  /// the name of extrapolator 
  std::string               m_extrapolatorName ; //    the name of extrapolator 
  /// the list of mass-constraints
  std::vector<std::string>  m_constraints ;     // the list of mass-constraints
  // ==========================================================================
};  
// ============================================================================
DECLARE_TOOL_FACTORY(DecayTreeFit) ;
// ============================================================================
// initialize the tool 
// ============================================================================
StatusCode DecayTreeFit::initialize ()                   // initialize the tool 
{
  StatusCode sc = GaudiTool::initialize () ;             // initialize the base 
  if ( sc.isFailure() ) { return sc ; }                  // RETURN 
  //
  //
  return StatusCode::SUCCESS ;                           // RETURN
}
// ============================================================================
// finalize the tool 
// ============================================================================
StatusCode DecayTreeFit::finalize ()                   // finalize the tool 
{
  // reset the status 
  m_fitter.reset()    ;
  //
  m_extrapolator  = 0 ;
  // finalize the base 
  return GaudiTool::finalize () ;                      // RETURN 
}
// ============================================================================
/* fit the decay tree 
 *  @param decay  (INPUT) the decay tree to fit 
 *  @param origin (INPUT) the origin vertex 
 *  @return status code 
 *  @see IDecayTreeFit
 *  @see IDecayTreeFit::fit 
 */                                                           
// ============================================================================
StatusCode DecayTreeFit::fit 
( const LHCb::Particle*   decay  , 
  const LHCb::VertexBase* origin ) const 
{ 
  if ( 0 == decay ) 
  { return Error ( "fit(): invalid argument" , 101 ) ; }      // RETURN
  // reset fitter 
  m_fitter.reset () ;
  // initialize fitter 
  m_fitter.reset 
    ( 0 == origin ? 
      new DecayTreeFitter::Fitter ( *decay           ) : 
      new DecayTreeFitter::Fitter ( *decay , *origin ) ) ;
  //
  // apply "global" constraints (if needed)
  for ( PIDs::const_iterator ipid = m_global_pids.begin() ; m_global_pids.end() != ipid ; ++ipid )
  { m_fitter->setMassConstraint ( *ipid ) ; }
  // apply "global" constraints (if needed)
  for ( PIDs::const_iterator ipid = m_locals_pids.begin()  ; m_locals_pids.end() != ipid ; ++ipid )
  { m_fitter->setMassConstraint ( *ipid ) ; }
  //
  // fit!
  m_fitter->fit() ;
  //
  { // clear local container of local constraints
    m_locals_pids.clear () ;
  }
  // get the status 
  if ( DecayTreeFitter::Fitter::Success != m_fitter->status() ) 
  { 
    m_fitter.reset () ;
    return Error ( "Error from fitter, status" , 
                   110 +  m_fitter->status() ) ; 
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/** get the fitted parameters for the particle 
 *  @param particle (INPUT) the particle, NULL corresponds to the decay head  
 *  @return the fitted parameters, NULL for invalid argument/fit failure 
 *  @see IDecayTreeFit
 *  @see IDecayTreeFit::fitted 
 */
// ============================================================================
const IDecayTreeFit::Fitted* 
DecayTreeFit::fitted ( const LHCb::Particle* p ) const 
{
  if ( 0 == m_fitter.get() ) 
  {
    Warning("fitted: fit if not perforemd yet, return NULL") ;
    return 0 ;                                                    // RETURN 
  }
  if ( DecayTreeFitter::Fitter::Success != m_fitter->status() ) 
  {
    Warning ( "fitted: fit is not perforemd yet, return NULL" , 
              120 + m_fitter->status() ) ;
    m_fitter.reset() ;
    return 0 ;                                                    // RETURN 
  }
  //
  const Fitted* fitted = m_fitter->fitParams ( p ) ;
  if ( 0 == fitted ) 
  {
    Warning ("fitted:  Fitted* points to NULL").ignore() ;
    return fitted ;
  }
  //
  return fitted ; 
}
// ============================================================================
/*  get the fit results in form of self-consistent decay tree 
 *  @see IDecayTreeFit
 *  @see IDecayTreeFit::fittedTree
 */
// ============================================================================
LHCb::DecayTree DecayTreeFit::fittedTree () const 
{
  if ( 0 == m_fitter.get() ) 
  {
    Warning("fitted: fit is not performed yet, return empty tree ") ;
    return LHCb::DecayTree() ;                                      // RETURN 
  }
  if ( DecayTreeFitter::Fitter::Success != m_fitter->status() ) 
  {
    Warning ( "fitted: fit is not performed yet, return empty tree" , 
              120 + m_fitter->status() ) ;
    m_fitter.reset() ; 
    return LHCb::DecayTree() ;                                      // RETURN 
  }
  // ==========================================================================
  return m_fitter->getFittedTree() ;
}
// ============================================================================
/*  add the constraint 
 *  @param pid (INPUT) particle-ID to be constrained
 *  @see IDecayTreeFit
 *  @see IDecayTreeFit::addConstraint
 */
// ============================================================================
void DecayTreeFit::addConstraint ( const LHCb::ParticleID& pid ) 
{
  //
  if ( parent() == toolSvc() ) 
  {
    Error ("Mass Constraint can't be added to PUBLIC tool! ignore!").ignore() ;
    return ;
  }
  //
  m_locals_pids.push_back ( LHCb::ParticleID ( pid.abspid() ) ) ;
}
// ============================================================================
/*  The basic method for "refit" of the particle
 *
 *  @code 
 *
 *  // locate the tool 
 *  const IParticleReFitter* refitter = tool<IParticleRefitter>( ... ) ;
 * 
 *  // particle to be refit 
 *  LHCb::Particle* p = ... ;
 *
 *  StatusCode sc = refitter->reFit ( *p ) ;
 *  if ( sc.isFailure() ) { Warning("Error in reFit" , sc ) ; }
 *
 *  @endcode 
 *
 *  @see IParticleReFitter
 *  @see IParticleReFitter::reFit 
 *
 *  @param particle reference to the particle 
 *  @return status code 
 */  
// ============================================================================
StatusCode DecayTreeFit::reFit ( LHCb::Particle& particle ) const 
{
  StatusCode sc = fit ( &particle ) ;
  if ( sc.isFailure() )
  { return Error ("reFit: error form fit", sc ) ; }
  //
  if ( 0 == m_fitter.get() ) { return Warning("reFit: invalid fitter") ; }
  //
  if ( DecayTreeFitter::Fitter::Success != m_fitter->status() ) 
  { return Error ( "reFit: invalid fit status " , 120 + m_fitter->status() ) ; }
  //
  // the actual refit 
  if ( !m_fitter->updateCand ( particle ) ) 
  { return Error ( "reFit: unable to update the candidate"     ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
