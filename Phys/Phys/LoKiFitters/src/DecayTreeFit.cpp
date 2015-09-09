// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <set> 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToStream.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
// ============================================================================
// TrackInterfaces 
// ============================================================================
#include "TrackInterfaces/ITrackStateProvider.h"
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
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/PhysAlgs.h"
#include "LoKi/Constants.h"
// ============================================================================
/** @file 
 *
 *  This file is a part of 
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *  ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
 *  @date 2010-05-27
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class DecayTreeFit 
   *  "Tool-based" implementation of IDecayTreeFit interfaces.
   *
   *  Actually it is just a thin wrapper for class DecayTreeFit::Fitter by 
   *  Wouter Hulsbergen
   *
   *  The tool implements two interfaces: IDecayTreeFit and IParticleReFitter
   *  @see IDecayTreeFit 
   *  @see IParticleReFitter 
   *  @see DecayTreeFit::Fitter 
   *
   *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
   *  @date 2010-05-27
   */
  class DecayTreeFit 
    : public extends2<GaudiTool,IDecayTreeFit,IParticleReFitter>
      // : public extends1<GaudiTool,IDecayTreeFit>
  {
    // ========================================================================
    // the friend factory, needed for instantiation
    friend class ToolFactory<LoKi::DecayTreeFit> ;
    // ========================================================================
  protected:
    // ========================================================================
    typedef ::DecayTreeFitter::Fitter Fitter ; // the actual type of the fitter 
    // ========================================================================
  public: // IDecayTreeFit
    // ========================================================================
    /** fit the decay tree 
     *
     *  @code 
     * 
     *   const IDecayTreeFit* fitter = ...;  // get the fitter 
     *
     *
     *   const LHCb::Particle*   p      = ... ; // get the particle 
     *  
     *   // fit it !
     *   StatusCode sc = fitter -> fit ( p ) ;  // fit it!!  
     *   if ( sc.isFailure() ) { ... }          
     *
     *  @endcode 
     *
     *  For the fit one can also use the primary vertex constraint:
     *
     *  @code 
     * 
     *   const IDecayTreeFit* fitter = ...;  // get the fitter 
     *
     *
     *   const LHCb::Particle*   B      = ... ; // get the particle 
     *   const LHCb::VertexBase* pv     = ... ; // get the primary vertex 
     *  
     *   // fit it !
     *   StatusCode sc = fitter -> fit ( B , pv ) ;  // fit it!!  
     *   if ( sc.isFailure() ) { ... }          
     *
     *  @endcode 
     *
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
     *
     *  @code 
     * 
     *   const IDecayTreeFit* fitter = ...;  // get the fitter 
     *   const LHCb::Particle*   p      = ... ; // get the particle 
     *  
     *   // fit it !
     *   StatusCode sc = fitter -> fit ( p ) ;  // fit it!!  
     *   if ( sc.isFailure() ) { ... }          
     *
     *   // get the daughter particle:
     *   const LHCb::Particle* daughter = ... ; 
     *  
     *   const IDecayTreeFit::Fitted* fitted = 
     *      fitter ->fitter ( daughter ) ;
     *  
     *   if ( 0 == fitted ) { ... }  ;           // error here 
     *   else 
     *     {  
     *        info () << " Fitted 4-momentum of daughter  " << fitted->momenum() 
     *                << " Fitted decay-legth of daughter " << fitted->decayLength() 
     *                << endreq 
     *     }
     *
     *  @endcode 
     *
     *   The fitted result for mother particle ("decay head") can be extracted 
     *   using the defautl argument :
     *
     *  @code 
     * 
     *   const IDecayTreeFit* fitter = ...;  // get the fitter 
     *   const LHCb::Particle*   B      = ... ; // get the particle 
     *   const LHCb::VertexBase* pv     = ... ; // get the primary vertex 
     *  
     *   // fit it !
     *   StatusCode sc = fitter -> fit ( B , pv ) ;  // fit it!!  
     *   if ( sc.isFailure() ) { ... }          
     *
     *   // extract the fitted results :
     *   const IDecayTreeFit::Fitted* fitted = fitter ->fitter () ;
     *  
     *   if ( 0 == fitted ) { ... }  ;           // error here 
     *   else 
     *     {  
     *        info () << " Fitted 4-momentum   " << fitted->momenum() 
     *                << " Fitted decay-length " << fitted->decayLength() 
     *                << endreq 
     *     }
     *
     *  @endcode 
     *
     *  @see IDecayTreeFit::Fitted 
     *  @see Gaudi::Math::ParticleParams  
     *  @see Gaudi::Math::ParticleParams::momentum
     *  @see Gaudi::Math::ParticleParams::decayLength
     *       
     *  @param particle (INPUT) the particle, NULL corresponds to the decay head  
     *  @return the fitted parameters, NULL for invalid argument/fit failure 
     *
     *  @see IDecayTreeFit
     *  @see IDecayTreeFit::fitted 
     */
    virtual const Fitted* fitted ( const LHCb::Particle* p = 0 ) const ;
    // ========================================================================
    /** get the fit results in form of self-consistent decay tree 
     * 
     *  @code 
     * 
     *   const IDecayTreeFit* fitter = ...;  // get the fitter 
     *   const LHCb::Particle*   p      = ... ; // get the particle 
     *  
     *   // fit it !
     *   StatusCode sc = fitter -> fit ( p ) ;  // fit it!!  
     *   if ( sc.isFailure() ) { ... }          
     *
     *   // get the daughter particle:
     *   const LHCb::Particle* daughter = ... ; 
     *  
     *   // get the fitted tree:
     *   LHCb::DecayTree tree = fitter->fittedTree() ;
     *
     *   // get the fitted daughter form the tree 
     *   const LHCb::Particle* fitted = tree.findClone ( *daughter ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::DecayTree
     *
     *  @return the whole fitted tree    
     *
     *  @see IDecayTreeFit
     *  @see IDecayTreeFit::fittedTree
     */
    virtual LHCb::DecayTree fittedTree () const ;
    // ======================================================================== 
  public:
    // ========================================================================
    /** the chi2 of the global fit 
     *
     *  @code 
     * 
     *   IDecayTreeFitter*       fitter = ...;  // get the fitter 
     *   const LHCb::Particle*   p      = ... ; // get the particle 
     *
     *   // fit it !
     *   StatusCode sc = fitter -> fit ( p ) ;  // fit it!!  
     *   if ( sc.isFailure() ) { ... }          
     *
     *   // get chi2 
     *   const double chi2 = fitter->chi2() ;
     *
     *  @endcode 
     *
     *  @return chi2 of global fit procedure 
     */
    virtual double chi2 ( ) const  ;
    // ========================================================================
    /** number degress of freeedom for the global fit 
     *
     *  @code 
     * 
     *   IDecayTreeFitter*       fitter = ...;  // get the fitter 
     *   const LHCb::Particle*   p      = ... ; // get the particle 
     *
     *   // fit it !
     *   StatusCode sc = fitter -> fit ( p ) ;  // fit it!!  
     *   if ( sc.isFailure() ) { ... }          
     *
     *   // get degrees-of-freedom 
     *   const unsigned int ndoF = fitter->nDoF () ;
     *
     *  @endcode 
     *
     *  @return number of degrees of freedom 
     */
    virtual unsigned int nDoF ( ) const ;
    // ======================================================================== 
  public:
    // ========================================================================
    /** add the constraint 
     *
     *  @code 
     * 
     *   IDecayTreeFit*       fitter = ...;  // get the fitter 
     *   const LHCb::Particle*   p      = ... ; // get the particle 
     *
     *   // apply mass-constrainst for charm for the next fit 
     *   fitter -> addConstraint ( LHCb::ParticleID( 240  )  ) ;  
     *   fitter -> addConstraint ( LHCb::ParticleID( 140  )  ) ;  
     *
     *   // fit it !
     *   StatusCode sc = fitter -> fit ( p ) ;  // fit it!!  
     *   if ( sc.isFailure() ) { ... }          
     *
     *  @endcode 
     *
     * @attention Mass-constraints is active only for the next call 
     *            of IDecayTreeFit::fit
     *  
     *  @param pid (INPUT) particle-ID to be constrained
     *
     *  @see IDecayTreeFit
     *  @see IDecayTreeFit::addConstraint
     */
    virtual void addConstraint ( const LHCb::ParticleID& pid ) ;
    // ========================================================================
    /** add the constraint 
     *
     *  @code 
     * 
     *   IDecayTreeFit*       fitter = ...;  // get the fitter 
     *   const LHCb::Particle*   p      = ... ; // get the particle 
     *
     *   const double mass1 = ... ;
     *   const double mass2 = ... ;
     *
     *   // apply mass-constrainst for charm for the next fit 
     *   fitter -> addConstraint ( LHCb::ParticleID( 240  )  , mass1 ) ;  
     *   fitter -> addConstraint ( LHCb::ParticleID( 140  )  , mass2 ) ;  
     *
     *   // fit it !
     *   StatusCode sc = fitter -> fit ( p ) ;  // fit it!!  
     *   if ( sc.isFailure() ) { ... }          
     *
     *  @endcode 
     *
     * @attention Mass-constraints is active only for the next call 
     *            of IDecayTreeFit::fit
     *  
     *  @param pid (INPUT) particle-ID to be constrained
     *
     *  @see IDecayTreeFit
     *  @see IDecayTreeFit::addConstraint
     */
    virtual void addConstraint ( const LHCb::ParticleID& pid  , 
                                 const double            mass ) ;
    // ========================================================================
  public:  // IParticleReFitter 
    // ========================================================================
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
    // ========================================================================
  public:   // AlgTool/GaudiTool
    // ========================================================================
    /// initialize the tool 
    virtual StatusCode initialize () ; // initialize the tool 
    /// finalize  the tool 
    virtual StatusCode finalize   () ; // finalize the tool 
    // ========================================================================
  protected:  // constuctors/desctructor 
    // ========================================================================
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
      , m_extrapolator     ( 0 ) 
      , m_fitter           (   )
    //
      , m_global_pids      (   ) 
      , m_locals_pids      (   )
      , m_global_mass      (   ) 
      , m_locals_mass      (   )
    //
      , m_extrapolatorName ( "TrackStateProvider:PUBLIC" )
      , m_constraints      (   ) 
      , m_masses           (   ) 
    {
      //
      declareProperty 
        ( "TrackStateProvider"                        , 
          m_extrapolatorName                         ,
          "Track Extrapolator to be used"            ) 
        -> declareUpdateHandler 
        ( &LoKi::DecayTreeFit::updateExtrapolator  , this ) ;
      //
      declareProperty
        ( "MassConstraints"                          , 
          m_constraints                              , 
          "List of particles to be mass-constrained" ) 
        -> declareUpdateHandler 
        ( &LoKi::DecayTreeFit::updateConstraints1  , this ) ;  
      //
      declareProperty
        ( "Masses"                                   , 
          m_masses                                   , 
          "Non-default masses of particles to be usef for mass-constraints" ) 
        -> declareUpdateHandler 
        ( &LoKi::DecayTreeFit::updateConstraints2  , this ) ;  
      //
    }
    /// virtual and protected destructor 
    virtual ~DecayTreeFit () {} ;
    // ========================================================================
  private:
    // ========================================================================
    /// default constructor is disabled 
    DecayTreeFit () ;                     // default constructor is disabled
    /// copy constructor is disables 
    DecayTreeFit           ( const DecayTreeFit& ) ;    //        no copy 
    /// assignement operator is disables 
    DecayTreeFit& operator=( const DecayTreeFit& ) ;    // no assignement 
    // ========================================================================
  protected:
    // ========================================================================
    /// update-handler for the property "Track Extrapolator"
    void updateExtrapolator ( Property& /* p */ )      // update the extrapolator 
    {
      // no action if not yet initialized 
      if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
      // 
      MsgStream& log = debug() ;
      log << "New Track extrapolator to be used '" << m_extrapolatorName <<  "'" ;
      const IAlgTool* e = extrapolator() ;
      if ( 0 != e ) { log  << " : " << e->type() << "/" << e->name() ;} 
      log << endreq ;
      //
    }
    // ========================================================================
    /// update-handler for the property "MassConstraints"
    void updateConstraints1 ( Property& /* p */ )      // update the constraints 
    {
      // no action if not yet initialized 
      if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
      // 
      StatusCode sc = decodeConstraints1 () ;
      Assert ( sc.isSuccess() , "Unable to decode MassConstraints" , sc ) ;
      //
    }
    // ========================================================================
    /// update-handler for the property "Masses"
    void updateConstraints2 ( Property& /* p */ )      // update the constraints 
    {
      // no action if not yet initialized 
      if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
      // 
      StatusCode sc = decodeConstraints2 () ;
      Assert ( sc.isSuccess() , "Unable to decode Masses" , sc ) ;
      //
    }
    // ========================================================================
    /// decode vector of mass-consstraints 
    StatusCode decodeConstraints1 () ;      // decode vector of mass-constraints
    /// decode map of mass-consstraints 
    StatusCode decodeConstraints2 () ;      // decode map of mass-constraints
    // ========================================================================
  protected:
    // ========================================================================
    /// get track extrapolator 
    inline const ITrackStateProvider* extrapolator() const 
    {
      if ( 0 != m_extrapolator        ) { return m_extrapolator ; } // RETURN 
      if ( m_extrapolatorName.empty() ) { return              0 ; } // REUTRN
      m_extrapolator = tool<ITrackStateProvider> ( m_extrapolatorName ) ;
      return m_extrapolator ;
    }
    // ========================================================================
  private:
    // ========================================================================
    /// track extrapolator 
    mutable const ITrackStateProvider* m_extrapolator ;   // the track extrapolator 
    // ========================================================================
    ///  the actual fitter 
    mutable std::auto_ptr<Fitter>  m_fitter       ;   //      the actual fitter 
    // ========================================================================
    typedef std::vector<LHCb::ParticleID>  PIDs ;
    /// list of mass-constrains (global)
    PIDs         m_global_pids ;            // list of mass-constrains (global)
    /// list of mass-constrains (local) 
    mutable PIDs m_locals_pids ;            //  list of mass-constrains (local) 
    // ========================================================================
    typedef std::map<LHCb::ParticleID,double>  MASS ;
    MASS         m_global_mass ; // map { PID : mass }
    mutable MASS m_locals_mass ; // map { PID : mass }
    // ========================================================================
    // properties 
    // ========================================================================
    /// the name of extrapolator 
    std::string               m_extrapolatorName ; //  the name of extrapolator 
    /// the list of mass-constraints
    std::vector<std::string>  m_constraints ;  // the list of mass-constraints
    /// the map of non-standard masses 
    std::map<std::string,double> m_masses   ; // the map of non-standard masses 
    // ========================================================================
  };  
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// initialize the tool 
// ============================================================================
StatusCode LoKi::DecayTreeFit::initialize ()          // initialize the tool 
{
  StatusCode sc = GaudiTool::initialize () ;             // initialize the base 
  if ( sc.isFailure() ) { return sc ; }                  // RETURN 
  //
  { //
    MsgStream& log = debug() ;
    log << "Track extrapolator to be used '" << m_extrapolatorName << "'" ;
    const IAlgTool* e = extrapolator() ;
    if ( 0 != e ) { log  << " : " << e->type() << "/" << e->name() ;} 
    log << endreq ;  
  } // 
  //
  sc = decodeConstraints1 () ;
  if ( sc.isFailure() ) { return Error ( "Unable to decode MassConstaints" , sc ) ; }
  sc = decodeConstraints2 () ;
  if ( sc.isFailure() ) { return Error ( "Unable to decode Masses"         , sc ) ; }
  //
  svc<LoKi::ILoKiSvc> ("LoKiSvc" , true ) ;
  //
  return StatusCode::SUCCESS ;                           // RETURN
}
// ============================================================================
// finalize the tool 
// ============================================================================
StatusCode LoKi::DecayTreeFit::finalize ()              // finalize the tool 
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
StatusCode LoKi::DecayTreeFit::fit                     // fit the decay tree 
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
      new Fitter ( *decay           , extrapolator() ) : 
      new Fitter ( *decay , *origin , extrapolator() ) ) ;
  //
  // apply "global" constraints (if needed)
  //
  for ( PIDs::const_iterator ipid = m_global_pids.begin() ; 
        m_global_pids.end() != ipid ; ++ipid ) 
  { m_fitter -> setMassConstraint ( *ipid ) ; }
  for ( MASS::const_iterator imass = m_global_mass.begin() ; 
        m_global_mass.end() != imass ; ++imass ) 
  { m_fitter -> setMassConstraint ( imass->first , imass->second ) ; }
  //
  // apply "local" constraints (if needed)
  //
  for ( PIDs::const_iterator ipid = m_locals_pids.begin() ; 
        m_locals_pids.end() != ipid ; ++ipid )
  { m_fitter -> setMassConstraint ( *ipid ) ; }
  for ( MASS::const_iterator imass = m_locals_mass.begin() ; 
        m_locals_mass.end() != imass ; ++imass ) 
  { m_fitter -> setMassConstraint ( imass->first , imass->second ) ; }
  // fit!
  m_fitter->fit() ;
  //
  { // clear local containers of local constraints
    m_locals_pids.clear () ;
    m_locals_mass.clear () ;
  }
  // get the status
  Fitter::FitStatus status = m_fitter->status() ;
  if ( Fitter::Success != status ) 
  { 
    m_fitter.reset () ;
    return Warning ( "Fitter failed status" , 110 + status, 1 ) ; 
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
LoKi::DecayTreeFit::fitted ( const LHCb::Particle* p ) const 
{
  // 
  if ( 0 == m_fitter.get() ) 
  {
    Error ("fitted: fit is not performed yet, return NULL").ignore() ;
    return 0 ;                                                    // RETURN 
  }
  // 
  Fitter::FitStatus status = m_fitter->status() ;
  if ( Fitter::Success != status ) 
  {
    Warning ( "fitted: fit is not successfull , return NULL" , 
              120 + status, 1 ).ignore()  ;
    m_fitter.reset() ;
    return 0 ;                                                    // RETURN 
  }
  // check if the particle comes from the decay tree 
  if ( 0 != p && p != m_fitter->particle() ) 
  {
    // find the particle by scanning of the decay tree of the decay head 
    const bool found = LoKi::PhysAlgs::found 
      (  m_fitter->particle()  , 
         std::bind2nd ( std::equal_to<const LHCb::Particle*>() , p ) ) ;
    if  ( !found ) 
    {
      Error ( "fitted: Particle is not from the tree, return NULL").ignore() ;
      return  0 ;                                                   // RETURN 
    } 
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
LHCb::DecayTree 
LoKi::DecayTreeFit::fittedTree () const 
{
  //
  if ( 0 == m_fitter.get() ) 
  {
    Warning("fitted: fit is not performed yet, return empty tree") ;
    return LHCb::DecayTree() ;                                      // RETURN 
  }
  //
  Fitter::FitStatus status = m_fitter->status() ;
  if ( Fitter::Success != status ) 
  {
    Warning ( "fitted: fit is not successful, return empty tree" , 
              120 + status ) ;
    m_fitter.reset() ; 
    return LHCb::DecayTree() ;                                      // RETURN 
  }
  //
  return m_fitter->getFittedTree() ;
}
// ============================================================================
/*  the chi2 of the global fit 
 *
 *  @code 
 * 
 *   IDecayTreeFitter*       fitter = ...;  // get the fitter 
 *   const LHCb::Particle*   p      = ... ; // get the particle 
 *
 *   // fit it !
 *   StatusCode sc = fitter -> fit ( p ) ;  // fit it!!  
 *   if ( sc.isFailure() ) { ... }          
 *
 *   // get chi2 
 *   const double chi2 = fitter->chi2() ;
 *
 *  @endcode 
 *
 *  @return chi2 of global fit procedure 
 */
// ============================================================================
double LoKi::DecayTreeFit::chi2 ( ) const  
{
  //
  if ( 0 == m_fitter.get() ) 
  {
    Warning ( "chi2: fit is not performed yet, return InvalidChi2",
              StatusCode::FAILURE, 2 ) ;
    return LoKi::Constants::InvalidChi2 ;                             // RETURN 
  }
  //
  Fitter::FitStatus status = m_fitter->status() ;
  if ( Fitter::Success != status ) 
  {
    Warning ( "chi2: fit is not successfull, return InvalidChi2" , 
              120 + status, 2 ) ;
    m_fitter.reset() ; 
    return LoKi::Constants::InvalidChi2 ;                             // RETURN 
  }
  //
  return m_fitter->chiSquare () ;
}
// ============================================================================
/*  number degress of freeedom for the global fit 
 *
 *  @code 
 * 
 *   IDecayTreeFitter*       fitter = ...;  // get the fitter 
 *   const LHCb::Particle*   p      = ... ; // get the particle 
 *
 *   // fit it !
 *   StatusCode sc = fitter -> fit ( p ) ;  // fit it!!  
 *   if ( sc.isFailure() ) { ... }          
 *
 *   // get degrees-of-freedom 
 *   const unsigned int ndoF = fitter->nDoF () ;
 *
 *  @endcode 
 *
 *  @return number of degrees of freedom 
 */
// ============================================================================
unsigned int LoKi::DecayTreeFit::nDoF ( ) const
{
  //
  if ( 0 == m_fitter.get() ) 
  {
    Warning ( "nDoF: fit is not performed yet, return 0", 
              StatusCode::FAILURE, 2 ) ;
    return 0 ;                                                       // RETURN 
  }
  //
  Fitter::FitStatus status = m_fitter->status () ;
  if ( Fitter::Success != status ) 
  {
    Warning ( "nDoF: fit is not sucessfull, return 0 ", 120 + status, 2 ) ;
    m_fitter.reset() ; 
    return 0 ;                                                        // RETURN 
  }
  //
  return m_fitter->nDof () ;
}
// ============================================================================
/*  add the constraint 
 *  @param pid (INPUT) particle-ID to be constrained
 *  @see IDecayTreeFit
 *  @see IDecayTreeFit::addConstraint
 */
// ============================================================================
void LoKi::DecayTreeFit::addConstraint ( const LHCb::ParticleID& pid ) 
{
  //
  if ( parent() == toolSvc() ) 
  {
    Error ( "Mass Constraint can't be added to PUBLIC tool! ignore!").ignore() ;
    return ;
  }
  //
  m_locals_pids.push_back ( LHCb::ParticleID ( pid.abspid() ) ) ;
}
// ============================================================================
/*  add the constraint 
 *  @param pid  (INPUT) particle-ID to be constrained
 *  @param mass (INPUT) the target mass 
 *  @see IDecayTreeFit
 *  @see IDecayTreeFit::addConstraint
 */
// ============================================================================
void LoKi::DecayTreeFit::addConstraint ( const LHCb::ParticleID& pid  ,
                                         const double            mass ) 
{
  //
  if ( parent() == toolSvc() ) 
  {
    Error ( "Mass Constraint can't be added to PUBLIC tool! ignore!").ignore() ;
    return ;
  }
  //
  m_locals_mass [ pid ] = mass ;
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
StatusCode LoKi::DecayTreeFit::reFit ( LHCb::Particle& particle ) const 
{
  StatusCode sc = fit ( &particle ) ;
  if ( sc.isFailure() )
  { return Warning ("reFit: Error from fit", sc, 2 ) ; }
  //
  if ( 0 == m_fitter.get() ) { return Error ("reFit: invalid fitter") ; }
  //
  Fitter::FitStatus status = m_fitter->status() ;
  if ( Fitter::Success != status ) 
  { return Error ( "reFit: invalid fit status" , 120 + status, 2 ) ; }
  //
  // the actual refit 
  if ( !m_fitter->updateCand ( particle ) ) 
  { return Error ( "reFit: unable to update the candidate"     ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// decode vector of mass-consstraints 
// ============================================================================
StatusCode LoKi::DecayTreeFit::decodeConstraints1 ()    // decode constraints
{
  m_global_pids.clear() ;
  //
  if ( m_constraints.empty() ) 
  {
    debug() << " No Mass-Constraints will be applied " << endreq ;
    return StatusCode::SUCCESS ;
  }
  ///
  const LHCb::IParticlePropertySvc* ppsvc = 
    svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc" ) ;
  //
  std::set<LHCb::ParticleID> pids ;
  for ( std::vector<std::string>::const_iterator ic = m_constraints.begin() ;
        m_constraints.end() != ic ; ++ic ) 
  {
    const LHCb::ParticleProperty* pp = ppsvc->find ( *ic ) ;
    if ( 0 == pp ) { return Error("Unable to find particle '" + (*ic) + "'") ; }
    pids.insert ( LHCb::ParticleID ( pp->pid().abspid() ) ) ;
  }
  //
  m_global_pids.insert ( m_global_pids.end() , pids.begin() , pids.end() ) ;
  //
  std::set<std::string> parts ;
  for ( PIDs::const_iterator ipid = m_global_pids.begin() ;
        m_global_pids.end() != ipid ; ++ipid ) 
  {
    const LHCb::ParticleProperty* pp = ppsvc->find ( *ipid ) ;
    if ( 0 != pp ) { parts.insert ( pp->particle () ) ; }
  }
  //
  info() << " Mass Constraints will be applied for : "
         << Gaudi::Utils::toString ( parts ) << endreq ;
  //
  release ( ppsvc ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// decode vector of mass-consstraints 
// ============================================================================
StatusCode LoKi::DecayTreeFit::decodeConstraints2 ()    // decode constraints
{
  m_global_mass.clear() ;
  //
  if ( m_masses.empty() ) 
  {
    debug() << " No Mass-Constraints will be applied " << endreq ;
    return StatusCode::SUCCESS ;
  }
  ///
  const LHCb::IParticlePropertySvc* ppsvc = 
    svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc" ) ;
  //
  for ( std::map<std::string,double>::const_iterator im = m_masses.begin() ;
        m_masses.end() != im ; ++im ) 
  {
    const LHCb::ParticleProperty* pp = ppsvc->find ( im->first ) ;
    if ( 0 == pp ) { return Error ( "Unable to find particle '" + (im->first ) + "'") ; }
    m_global_mass [ pp->particleID() ] = im->second ;
  }
  //
  std::set<std::string> parts ;
  //
  for ( MASS::const_iterator imas = m_global_mass.begin() ;
        m_global_mass.end() != imas ; ++imas ) 
  {
    const LHCb::ParticleProperty* pp = ppsvc->find ( imas->first ) ;
    if ( 0 != pp ) { parts.insert ( pp->particle () ) ; }
  }
  //
  info() << " Mass Constraints will be applied for : " 
         << Gaudi::Utils::toString ( parts ) << endreq ;
  //
  release ( ppsvc ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,DecayTreeFit) 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
