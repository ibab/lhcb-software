// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL  
// ============================================================================
#include <tuple>
// ============================================================================
//GaudiKernel
// ============================================================================
#include "GaudiKernel/ParsersFactory.h"
// ============================================================================
// GaudiAlgs 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Chrono.h"
// ============================================================================
// TrackInterfaces
// ============================================================================
#include "TrackInterfaces/ITrackStateProvider.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IParticleTransporter.h"
#include "Kernel/IParticle2State.h"
// ============================================================================
// DaVinciUtils 
// ============================================================================
#include "Kernel/TransporterFunctions.h"
// ============================================================================
namespace DaVinci
{
  // ==========================================================================
  /** @class ParticleTransporter 
   *  implementation of IParticleTransporter interface that
   *  uses ITrackStateProvider for charged tracks
   *  Note that this approch is really coherent with DecayTreeFitter
   *  @see IParticleTransporter
   *  @see ITrackStateProvider 
   *  @attention for single usage it could be a bit slower,  
   *             but being used widely it shodul be rather efficinect
   *             due to smart caching embedded into TrackStateProvider 
   *  @todo Better treatment of electrons is needed 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @daet 2013-06-03
   */
  class ParticleTransporter : public extends1<GaudiTool,IParticleTransporter> 
  {
    /// friend factory for instantiation 
    friend class ToolFactory<DaVinci::ParticleTransporter> ;
    // ======================================================================== 
  public:
    // ========================================================================
    /** Transport a Particle to specified z position
     *  @param particle    (INPUT) the particle to be transported 
     *  @param znew        (INPUT) new z-position 
     *  @param transported (UPDATE) the transported particle 
     *  @see IParticleTransporter::transport
     */
    virtual StatusCode transport
    ( const LHCb::Particle* particle    , 
      const double          znew        ,
      LHCb::Particle&       transported ) ;
    // ========================================================================    
    /** Transport and project a Particle to specified z position.
     *  @param particle    (INPUT) the particle to be transported 
     *  @param znew        (INPUT) new z-position 
     *  @param transported (UPDATE) the transported particle 
     *  @see IParticleTransporter::transportAndProject
     */
    virtual StatusCode transportAndProject
    ( const LHCb::Particle* particle    , 
      const double          znew        ,
      LHCb::Particle&       transported ) ;
    // ========================================================================
  protected:
    // ========================================================================
    virtual StatusCode initialize () ;
    // ========================================================================
  protected:
    // ========================================================================
    /// standard constructor 
    ParticleTransporter 
    ( const std::string& type   , // the type ??
      const std::string& name   , 
      const IInterface*  parent ) ;
    /// virtual and protected destructor 
    virtual ~ParticleTransporter () ;
    // ========================================================================
  protected:
    // ========================================================================
    StatusCode transportChargedBasic 
    ( const LHCb::Particle* particle    , 
      const double          znew        ,
      LHCb::Particle&       transported ) ;
    // transport electrons (to be improved in future) 
    StatusCode transportElectron
    ( const LHCb::Particle* particle    , 
      const double          znew        ,
      LHCb::Particle&       transported ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    ParticleTransporter () ; // the default constructor is disabled 
    /// copy constructor is disabled 
    ParticleTransporter ( const ParticleTransporter& ) ; // no copy 
    /// assignement operator is disabled 
    ParticleTransporter& operator=( const ParticleTransporter& ) ; // no assignment
    // ========================================================================
  private: // properties: 
    // ========================================================================
    /// the name of state provider 
    std::string m_stateprovidername  ; // the name of state provider 
    /// the name of particle <-->state tool 
    std::string m_particle2statename ; // the name of particle <-->state tool 
    /// the name of extrapolator name (1) 
    std::string m_extrapolator1name  ; // the name of extrapolator name (1) 
    /// the name of extrapolator name (2) 
    std::string m_extrapolator2name  ; // the name of extrapolator name (2) 
    /// area in Z, where "Trajectory" approximation to be used for long track
    std::pair<double,double> m_region ;
    /// measure CPU performance? 
    bool                     m_timing ; // measure CPU performance? 
    // ========================================================================    
  private:
    // ========================================================================
    ITrackStateProvider* m_stateprovider  ;
    IParticle2State*     m_particle2state ;
    ITrackExtrapolator*  m_extrapolator1  ;
    ITrackExtrapolator*  m_extrapolator2  ;
    // ========================================================================
  };
  // ==========================================================================
} //                                                   end of namesapce DaVinci
// ============================================================================
StatusCode DaVinci::ParticleTransporter::initialize () 
{
  // initialize the base 
  StatusCode sc = GaudiTool::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  //
  m_stateprovider  = tool<ITrackStateProvider>  ( m_stateprovidername  , this ) ;
  m_particle2state = tool<IParticle2State>      ( m_particle2statename , this ) ;
  m_extrapolator1  = tool<ITrackExtrapolator>   ( m_extrapolator1name  , this ) ;
  if ( !m_extrapolator2name.empty() ) 
  { m_extrapolator2  = tool<ITrackExtrapolator> ( m_extrapolator2name  , this ) ; }
  //
  // is message level is low enough, active timing 
  if ( msgLevel ( MSG::DEBUG ) ) { m_timing = true ; }
  //
  return sc ;
}
// ============================================================================
// standard constructor 
// ============================================================================
DaVinci::ParticleTransporter::ParticleTransporter 
( const std::string& type   , // the type ??
  const std::string& name   , 
  const IInterface*  parent ) 
  : base_class ( type , name , parent )
//
  , m_stateprovidername  ( "TrackStateProvider:PUBLIC"          ) 
  , m_particle2statename ( "Particle2State:PUBLIC"              )
  , m_extrapolator1name  ( "TrackParabolicExtrapolator:PUBLIC"  )
  , m_extrapolator2name  ( "TrackRungeKuttaExtrapolator:PUBLIC" )
  , m_region         ( -30 * Gaudi::Units::cm , 100 * Gaudi::Units::cm )
    //
#ifndef NDEBUG 
  , m_timing         ( true  )
#else 
  , m_timing         ( false )
#endif 
    //
  , m_stateprovider  ( 0 ) 
  , m_particle2state ( 0 ) 
  , m_extrapolator1  ( 0 ) 
  , m_extrapolator2  ( 0 )
//
{
  //
  if      ( std::string::npos != name.find ( "Master" ) ) 
  {
    m_extrapolator1name  = "TrackMasterExtrapolator:PUBLIC"     ;
    m_extrapolator2name  = "" ;
  }
  else if ( std::string::npos != name.find ( "Runge"  ) ) 
  {
    m_extrapolator1name  = "TrackRungeKuttaExtrapolator:PUBLIC" ;
    m_extrapolator2name  = "" ;
  }
  else if ( std::string::npos != name.find ( "Linear" ) ) 
  {
    m_extrapolator1name  = "TrackLinearExtrapolator:PUBLIC"     ;
    m_extrapolator2name  = "TrackParabolicExtrapolator:PUBLIC"  ;
  }
  //
  declareProperty  
    ( "TrackStateProvider"     , 
      m_stateprovidername      , 
      "Tool to be used as Track State Provider"          ) ;  
  declareProperty  
    ( "Particle2State"         , 
      m_particle2statename     , 
      "Tool to be used as Particle <--> State converter" ) ;
  declareProperty  
    ( "Extrapolator1"          , 
      m_extrapolator1name      , 
      "Track extrapolator to be used for tracks/electrons" ) ;
  declareProperty  
    ( "Extrapolator2"          , 
      m_extrapolator2name      , 
      "Track extrapolator to be used for tracks/electrons" ) ;
  declareProperty 
    ( "TrajectoryRegion"        , 
      m_region                  , 
      "Region in Z where Trajectory approximation should be used for long tracks" ) ;
  declareProperty 
    ( "MeasureCPUPerformance"   , 
      m_timing                  , 
      "Measure CPU perormance"  ) ;  
}
// ============================================================================
// destructor 
// ============================================================================
DaVinci::ParticleTransporter::~ParticleTransporter(){}
// ============================================================================
/*  Transport a Particle to specified z position
 *  @param particle    (INPUT) the particle to be transported 
 *  @param znew        (INPUT) new z-position 
 *  @param transported (UPDATE) the transported particle 
 *  @see IParticleTransporter::transport
 */
// ============================================================================
StatusCode DaVinci::ParticleTransporter::transport
( const LHCb::Particle* particle    , 
  const double          znew        ,
  LHCb::Particle&       transported ) 
{
  //
  // measure CPU if required 
  Chrono chrono ( m_timing ? chronoSvc() : nullptr , name() );
  //
  if ( 0 == particle ) 
  { return Error("Invalid particle, impossible to transport") ; }
  //
  if ( particle != &transported ) { transported = *particle ; }
  //
  if      ( !particle->isBasicParticle() ) 
  { return DaVinci::Transporter::transportComposite    
      ( particle , znew , transported ) ; }                      // RETURN 
  else if ( 0 == particle->charge() ) 
  { return DaVinci::Transporter::transportNeutralBasic
      ( particle , znew , transported ) ; }                      // RETURN 
  //
  return transportChargedBasic ( particle , znew , transported ) ;
}
// ============================================================================
/*  Transport and project a Particle to specified z position.
 *  @param particle    (INPUT) the particle to be transported 
 *  @param znew        (INPUT) new z-position 
 *  @param transported (UPDATE) the transported particle 
 *  @see IParticleTransporter::transportAndProject
 */
// ============================================================================
StatusCode DaVinci::ParticleTransporter::transportAndProject
( const LHCb::Particle* particle    , 
  const double          znew        ,
  LHCb::Particle&       transported ) 
{
  //
  // measure CPU if required 
  Chrono chrono ( m_timing ? chronoSvc() : nullptr , name() );
  //
  if ( 0 == particle ) 
  { return Error("Invalid particle, impossible to transport") ; }
  //
  if ( particle != &transported ) { transported = *particle ; }
  //
  if      ( !particle->isBasicParticle() ) 
  { return DaVinci::Transporter::transportAndProjectComposite    
      ( particle , znew , transported ) ; }                       // RETURN
  else if ( 0 == particle->charge() ) 
  { return DaVinci::Transporter::transportAndProjectNeutralBasic 
      ( particle , znew , transported ) ; }                       // RETURN  
  //
  // special treatment of electrons (to be improved)
  //
  if ( 11 == particle->particleID().abspid() ) 
  { return transportElectron ( particle , znew , transported ) ; }
  //
  return transportChargedBasic ( particle , znew , transported ) ;
}
// ============================================================================
StatusCode DaVinci::ParticleTransporter::transportChargedBasic 
( const LHCb::Particle* particle    , 
  const double          znew        ,
  LHCb::Particle&       transported ) 
{
  //
  if ( 0 == particle ) { return StatusCode::FAILURE ; }      // RETURN 
  //
  const LHCb::ProtoParticle* pp = particle->proto() ;
  if ( 0 == pp       ) { return StatusCode::FAILURE ; }      // RETURN 
  //
  const LHCb::Track*        track = pp->track() ;
  if ( 0 == track    ) { return StatusCode::FAILURE ; }      // RETURN 
  //
  // finally use two tools to get an answer 
  //
  LHCb::State state ;
  if ( track->checkType ( LHCb::Track::Long ) && 
       m_region.first <= znew && znew <= m_region.second )
  {
    StatusCode sc = m_stateprovider -> stateFromTrajectory ( state , *track , znew );
    if ( sc.isFailure() ) 
    { Warning ("Error from StateProvider::stateFromTrajectory", sc , 3 ) ; }
    sc = m_stateprovider -> state ( state , *track , znew );
    if ( sc.isFailure() ) 
    { return Warning ("Error from StateProvider::state", sc, 3 ) ; } 
  }
  else 
  {
    StatusCode sc = m_stateprovider -> state ( state , *track , znew );
    if ( sc.isFailure() ) 
    { return Warning ("Error from StateProvider::state", sc, 3 ) ; }
  }
  //
  StatusCode sc = m_particle2state -> state2Particle ( state , transported ) ;
  if ( sc.isFailure() ) 
  { return Warning ( "Error from Particle2State", sc, 3 ) ; }
  //
  return sc ;
}
// ============================================================================
StatusCode DaVinci::ParticleTransporter::transportElectron
( const LHCb::Particle* particle    ,
  const double          znew        ,
  LHCb::Particle&       transported ) 
{
  //
  if ( 0 == particle ) { return StatusCode::FAILURE ; }      // RETURN 
  //
  const LHCb::ProtoParticle* pp = particle->proto() ;
  if ( 0 == pp       ) { return StatusCode::FAILURE ; }      // RETURN 
  //
  const LHCb::Track*        track = pp->track() ;
  if ( 0 == track    ) { return StatusCode::FAILURE ; }      // RETURN 
  //
  // finally use two tools to get an answer 
  //
  LHCb::State state ;
  StatusCode sc = m_particle2state -> particle2State ( *particle , state ) ;
  if ( sc.isFailure() ) 
  { return Warning ("Error from Particle2State", sc, 3 ) ; }
  //
  if ( track->checkType ( LHCb::Track::Long  ) ||  
       track->checkType ( LHCb::Track::Velo  ) || 
       track->checkType ( LHCb::Track::VeloR ) || 0 == m_extrapolator2 ) 
  {
    // first try "simple" extrapolator 
    sc = m_extrapolator1 -> propagate ( state , znew , particle->particleID() ) ;
    if ( sc.isFailure() && 0 != m_extrapolator2 ) 
    { 
      Warning( "Failure from extrapolator-1, try the extrapolator-2", sc, 0 ) ; 
      m_particle2state -> particle2State ( *particle , state ) ;
      // the second try: use more complicated extrapolator 
      sc = m_extrapolator2 -> propagate ( state , znew , particle->particleID() ) ;
    }
  }
  else // extrapolator2 
  { sc = m_extrapolator2 -> propagate ( state , znew , particle->particleID() ) ; } 
  //
  if ( sc.isFailure() ) { return Warning ("Error TrackExtrapolator", sc, 3 ) ; }
  //
  sc = m_particle2state -> state2Particle ( state , transported ) ;
  if ( sc.isFailure() ) 
  { return Warning ( "Error from Particle2State", sc, 3 ) ; }
  //
  return sc ;
}
// ============================================================================
// finally : the factory:
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY( DaVinci , ParticleTransporter )

// ============================================================================
// The END 
// ============================================================================
