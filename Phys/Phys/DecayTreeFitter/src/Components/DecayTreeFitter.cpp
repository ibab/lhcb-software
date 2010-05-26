// $Id: DecayTreeFitter.cpp,v 1.1 2010-05-26 15:46:32 ibelyaev Exp $
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
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IDecayTreeFit.h"
#include "Kernel/IParticleReFitter.h"
#include "Kernel/IParticleTransporter.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/ParticleParams.h"
// ============================================================================
// DecayTreeFitter 
// ============================================================================
#include "DecayTreeFitter/Fitter.h"
#include "DecayTreeFitter/VtxFitStatus.h"
#include "DecayTreeFitter/VtxFitParams.h"
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
  {}
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
  /// get particle transporter 
  inline IParticleTransporter* transporter() const 
  {
    if ( 0 != m_transporter ) { return m_transporter ; }
    m_transporter = tool<IParticleTransporter> ( "ParticleTransporter" , this ) ;
    return m_transporter ;
  }
  // ==========================================================================
private:
  // ==========================================================================
  /// the particle transporter 
  mutable IParticleTransporter* m_transporter ;    // the particle transporter 
  // ==========================================================================
  // the fitter 
  // ==========================================================================
  mutable std::auto_ptr<DecayTreeFitter::Fitter>  m_fitter ; // the fitter 
  // ==========================================================================
  /// the actual type for map : particle -> fit parameters 
  typedef std::map<const LHCb::Particle*,IDecayTreeFit::Fitted>  Map ;
  /// the map : particle -> fit parameters 
  mutable Map m_map;  // the map : particle -> fit parameters 
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
  transporter() ;
  //
  return StatusCode::SUCCESS ;                           // RETURN
}
// ============================================================================
// finalize the tool 
// ============================================================================
StatusCode DecayTreeFit::finalize ()                   // finalize the tool 
{
  // reset the status 
  m_transporter = 0 ;
  m_fitter.reset()  ;
  m_map.clear()     ;
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
  m_map.clear()     ;
  // initialize fitter 
  m_fitter.reset 
    ( 0 == origin ? 
      new DecayTreeFitter::Fitter ( *decay           ) : 
      new DecayTreeFitter::Fitter ( *decay , *origin ) ) ;
  //
  // fit!
  m_fitter->fit() ;
  // get the status 
  const int status =  m_fitter->status() ;
  if ( DecayTreeFitter::FitStatus::Success != status ) 
  { 
    m_fitter.reset () ;
    m_map.clear()     ;
    return Error ("Error fomr fitter, status" , 110 +  status ) ; 
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
  const int status = m_fitter->status() ;
  if ( DecayTreeFitter::FitStatus::Success != status ) 
  {
    Warning("fitted: fit is not perforemd yet, return NULL", 120 + status ) ;
    m_map.clear()     ;
    m_fitter.reset() ;
    return 0 ;                                                    // RETURN 
  }
  //
  // already in the map ?
  Map::const_iterator ifind = m_map.find ( p ) ;
  if ( m_map.end() != ifind ) { return &(ifind->second) ; }       // REUTRN
  //
  // if ( not_from_decay ) 
  // {
  //  Warning ("Particle is not from the decay tree, return NULL") ;
  //  return 0 ;
  // }
  //
  LHCb::VtxFitParams pars = 
    0 == p ?
    m_fitter->fitParams(    ) : 
    m_fitter->fitParams( *p ) ;
  
  m_map[p] = Fitted 
    ( pars.position      () ,
      pars.p4            () , 
      pars.decayLength   () ,
      pars.cov8          () ) ;
  
  if ( 0 == p ) 
  { m_map[ m_fitter->particle() ] = m_map[ p ] ; }
  //
  return &m_map[p] ; 
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
    Warning("fitted: fit if not perforemd yet, return empty tree ") ;
    return LHCb::DecayTree() ;                                      // RETURN 
  }
  const int status = m_fitter->status() ;
  if ( DecayTreeFitter::FitStatus::Success != status ) 
  {
    Warning("fitted: fit is not performed yet, return empty tree" , 120 + status ) ;
    m_map.clear()     ;
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
void DecayTreeFit::addConstraint ( const LHCb::ParticleID& /* pid */ ) 
{ Warning ( "addConstrainst:not implemented yet" ) ; }
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
StatusCode DecayTreeFit::reFit ( LHCb::Particle& /* particle */ ) const 
{ return Error ("reFit:not implemented yet") ; }
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
