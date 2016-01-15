// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ParsersFactory.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleTransporter.h"
#include "Kernel/IDistanceCalculator.h"
// ============================================================================
// KalmanFilter 
// ============================================================================
#include "KalmanFilter/VertexFit.h"
#include "KalmanFilter/VertexFitWithTracks.h"
// ============================================================================
// local
// ============================================================================
#include "DistanceCalculator.h"
#include "FitterUtils.h"
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
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-03-05
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
 */
// ============================================================================
/*  Standard constructor
 *  @param type tool type(?)
 *  @param name tool instance name 
 *  @param parent the pointer to the parent
 */
// ============================================================================
LoKi::DistanceCalculator::DistanceCalculator
( const std::string& type   , // tool type (?)
  const std::string& name   , // tool instance name 
  const IInterface*  parent ) // the parent 
  : base_class ( type , name , parent )
    // the maximal number of iterations 
  , m_nIter_max  ( 10 ) // the maximal number of iterations 
    // the convergency criterion for ip-evaluation
  , m_deltaZ     ( 2 * Gaudi::Units::micrometer ) // the criteria for ip-evaluation
    // the convergency criterion for delta(chi2) 
  , m_delta_chi2 ( 0.05 ) 
    // the convergency criterion for delta(path) 
  , m_delta_path ( 2 * Gaudi::Units::micrometer )
    // the range where 'Trajectory' approximation is good for long tracks 
  , m_region     ( -30 * Gaudi::Units::cm , 
                   100 * Gaudi::Units::cm ) 
    // use fast (==no transportation, no iteration) algorithms?
  , m_fastalgo  ( false ) 
    // the local storages of particles
  , m_particle1 ( LHCb::ParticleID ( 511 ) ) 
  , m_particle2 ( LHCb::ParticleID ( 511 ) )
{
  //
  declareProperty 
    ( "MaxIterations"    , m_nIter_max           , 
      "The maximal number of iterations "        ) ;
  declareProperty 
    (  "ToleranceInZ"    , m_deltaZ              , 
       "The allowed tolerance in Z (for transporters)" ) ;
  declareProperty 
    ( "DeltaChi2"        , m_delta_chi2          , 
      "Delta chi2  (convergency criterion)"      ) ;
  declareProperty 
    ( "DeltaPath"        , m_delta_path          , 
      "Delta c*tau (convergency criterion)"      ) ; 
  declareProperty 
    ( "TrajectoryRegion"        , 
      m_region                  , 
      "Region in Z where Trajectory approximation should be used for long tracks" ) ;
  declareProperty 
    ( "UseFastAlgorithms" , m_fastalgo           , 
      "Use fast(==no transportation, no iterations) algorithms?" );
  //
  StatusCode sc = setProperty ( "MaxPrints" , 2 ) ;
  Assert ( sc.isSuccess () , "Unable (re)set property 'MaxPrints'"    , sc ) ;
}
// ============================================================================
// virtual and protected desctructor 
// ============================================================================
LoKi::DistanceCalculator::~DistanceCalculator(){}
// ============================================================================
/* The method for the evaluation of the impact parameter ("distance") 
 *  vector of the particle with respect to some vertex. 
 *  @param particle (input) pointer to the particle 
 *  @param vertex   (input) pointer to the vertex 
 *  @param imppar   (output) the impact parameter ("distance") vector 
 *  @param chi2     (output,optional) the chi2 of the inmpact parameter
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::DistanceCalculator::_distance
( const LHCb::Particle&   particle ,
  const LHCb::VertexBase& vertex   , 
  Gaudi::XYZVector&       impact   , 
  const bool              allow    , 
  double*                 chi2     ) const 
{
  // 
  // check the particle category , and redirect is needed 
  // 
  LoKi::KalmanFilter::ParticleType type1 = particleType_ ( particle ) ;
  switch ( type1 ) 
  {
  case LoKi::KalmanFilter::GammaLikeParticle     : ;
  case LoKi::KalmanFilter::DiGammaLikeParticle   : ;
  case LoKi::KalmanFilter::MergedPi0LikeParticle :
    {
      impact =  Gaudi::XYZVector() ;
      if ( 0 != chi2 ) { *chi2 = 0 ; }
      if ( printStat() || msgLevel( MSG::INFO ) ) { ++counter ( "IP->(DI)GAMMA" ) ; }
      return StatusCode::SUCCESS ;                                    // RETURN 
    }
  case LoKi::KalmanFilter::ShortLivedParticle : 
    {
      if ( allow ) 
      { 
        const LHCb::VertexBase* v1 = particle.endVertex() ;
        if ( 0 == v1 ) { break ; }  
        if ( printStat() || msgLevel( MSG::INFO ) ) { ++counter ( "IP->VD" ) ; }
        // make the real calculations 
        double dist = 0 ;
        StatusCode sc = i_distance ( *v1 , vertex , dist , chi2 ) ; // RETURN 
        impact = v1->position() - vertex.position () ;
        return sc ;
      }
    } 
  default: ;
  }
  
  //
  // start normal processing 
  //
  using namespace Gaudi::Math::Operators ;
  
  // make the fast evaluation:
  i_distance ( particle , vertex , impact ) ;
  
  const LHCb::Particle* good  = &particle ;
  
  // iterate if needed 
  if ( !fastalgo() ) 
  {
    //
    const double Vz = vertex.position().Z() ;
    double deltaZ = std::abs ( Vz + impact.Z() - good->referencePoint().Z() ) ;
    //
    // make at leats one iteration 
    for ( unsigned int iIter = 0 ; 
          deltaZ >= m_deltaZ && iIter < std::max ( 1u , m_nIter_max ) ; ++iIter ) 
    { 
      // transport the particle into the new impact parameter point.
      StatusCode sc = transportAndProject 
        ( good                         ,   // what to be transported 
          vertex.position() + impact   ,   // new Z 
          m_particle1                  ) ; // the result
      if ( sc.isFailure() ) 
      { _Warning ( "Error from ParticleTransporter(ip_1), ignore", sc, 0 ) ; }
      else 
      { good = &m_particle1 ; } // the properly transported particle 
      
      // make the new evaluation of impact parameter
      i_distance ( *good , vertex , impact ) ;
      
      // check the delta_z 
      deltaZ = std::abs ( Vz + impact.Z() - good->referencePoint().Z() ) ;
      
    } /// the end of iteration loop 
    
    // check for  the convergency
    if ( m_deltaZ <= deltaZ  )
    { _Warning ( "There is no convergency-I", NoConvergency, 0 ) ; }
    
  }
  //
  // evaluate chi2 (if needed) 
  if ( 0 != chi2 ) 
  {
    // ========================================================================
    *chi2 = -1.e+10 ;
    // transport the particle into the new impact parameter point.
    if ( !fastalgo() && good != &m_particle1 ) 
    {
      // not yet transported? 
      StatusCode sc = transportAndProject 
        ( good                         ,   // what to be transported 
          vertex.position() + impact   ,   // new Z 
          m_particle1                  ) ; // the result 
      if ( sc.isFailure() ) 
      { _Warning ( "Error from ParticleTransporter(chi2_1), ignore", sc, 0 ) ; }
      else 
      { good = &m_particle1 ; } // the properly transported particle    
    }
    // prepare the Kalman Filter machinery, fixed by VB 2016-01-015 
    StatusCode sc = LoKi::KalmanFilter::loadAsFlying ( *good , m_entry ) ; 
    if ( sc.isFailure() ) 
    { return _Warning("distance(I): KalmanFilter::load failed", sc, 0 ) ; }
    // get the "the previus" Kalman Filter estimate == vertex
    Gaudi::SymMatrix3x3 ci = vertex.covMatrix() ; // the gain matrix 
    if ( !ci.Invert() ) 
    { return _Warning ( "distance(I): unable to calculate the gain matrix", 
                        StatusCode::FAILURE, 0 ) ; }
    // make one step of Kalman filter 
    sc = LoKi::KalmanFilter::step ( m_entry , vertex.position() , ci , 0 ) ;
    if ( sc.isFailure() ) 
    { return _Warning ( "distance(I): error from Kalman Filter step", sc, 0 ) ; }
    // get the chi2 
    *chi2 = m_entry.m_chi2 ;
    // ========================================================================
  }
  //
  return StatusCode::SUCCESS ;                                 // RETURN 
}
// ============================================================================
/* The method for the evaluation of the impact parameter ("distance") 
 *  vector of the particle with respect to the fixed vertex  
 *  @param particle (input) the particle 
 *  @param point    (input) the fixed point
 *  @param imppar   (output) the impact parameter ("distance") vector 
 *  @param chi2     (output,optional) the chi2 of the inmpact parameter
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::DistanceCalculator::_distance
( const LHCb::Particle&   particle ,
  const Gaudi::XYZPoint&  point    , 
  Gaudi::XYZVector&       impact   , 
  const bool              allow    , 
  double*                 chi2     ) const 
{
  //
  // the particle type
  //
  LoKi::KalmanFilter::ParticleType type1 = particleType_ ( particle ) ;
  switch ( type1 ) 
  {
  case LoKi::KalmanFilter::GammaLikeParticle     : ;
  case LoKi::KalmanFilter::DiGammaLikeParticle   : ;
  case LoKi::KalmanFilter::MergedPi0LikeParticle :
    {
      impact =  Gaudi::XYZVector() ;
      if ( 0 != chi2 ) { *chi2 = 0 ; }
      if ( printStat() && msgLevel( MSG::INFO ) ) { ++counter ( "IP->(DI)GAMMA" ) ; }
      return StatusCode::SUCCESS ;                                    // RETURN 
    }
  case LoKi::KalmanFilter::ShortLivedParticle : 
    {
      if ( allow )
      {
        const LHCb::VertexBase* v1 = particle.endVertex() ;
        if ( 0 == v1 ) { break ; }  
        if ( printStat() && msgLevel( MSG::INFO ) ) { ++counter ( "IP->VD" ) ; }
        // make the real calculations 
        double dist   = 0 ;
        StatusCode sc = i_distance ( *v1 , point , dist , chi2 ) ;
        impact        = v1->position() - point ;
        return sc ;
      } 
    }
  default: ;
  }
  //
  // regular processing  
  //
  using namespace Gaudi::Math::Operators ;
  
  // make the fast evaluation:
  i_distance ( particle , point , impact ) ;
  
  const LHCb::Particle* good  = &particle ;
  
  if ( !fastalgo() ) 
  {
    const double Vz = point.Z() ;
    double deltaZ   = std::abs ( Vz + impact.Z() - good->referencePoint().Z() ) ;
    
    // make the iterations here (if needed) 
    for ( unsigned int iIter = 0 ; 
          deltaZ >= m_deltaZ && iIter < std::max ( 1u , m_nIter_max ) ; ++iIter ) 
    {
      // transport the particle into the new impact parameter point.
      StatusCode sc = transportAndProject 
        ( good             ,   // what to be transported 
          point + impact   ,   // new Z 
          m_particle1      ) ; // the result 
      if ( sc.isFailure() ) 
      { _Warning ( "Error from ParticleTransporter(ip_2), ignore", sc, 0 ) ; }
      else 
      { good = &m_particle1 ; } // the properly transported particle 
      
      // the transported particle 
      good = &m_particle1 ;
      
      // make the new evaluation of impact parameter
      i_distance ( *good , point , impact ) ;
      
      // check the delta_z 
      deltaZ = std::abs ( Vz + impact.Z() - good->referencePoint().Z() ) ;
      
    } // the end of iteration loop 
    
    // check for  the convergency
    if ( deltaZ >= m_deltaZ )
    { _Warning ( "There is no convergency-II", NoConvergency, 0 ) ; }
    
  }
  
  // evaluate the chi2 (if needed) 
  if ( 0 != chi2 ) 
  {
    *chi2 = -1.e+10 ;
    // transport the particle into the new impact parameter point.
    if ( !fastalgo() && good != &m_particle1 ) 
    {
      // not yet transported? 
      StatusCode sc = transportAndProject 
        ( good                         ,   // what to be transported 
          point + impact               ,   // new Z 
          m_particle1                  ) ; // the result 
      if ( sc.isFailure() ) 
      { _Warning ( "Error from ParticleTransporter(chi2_2), ignore", sc, 0 ) ; }
      else 
      { good = &m_particle1 ; } // the properly transported particle 
    }
    // prepare the Kalman Filter machinery 
    StatusCode sc = LoKi::KalmanFilter::loadAsFlying ( *good , m_entry ) ;
    if ( sc.isFailure() ) 
    { return _Error("distance(II): error from KalmanFilter::load" , sc ) ; }
    // here the evaluations of chi2 is just trivial:
    *chi2 = Gaudi::Math::Similarity ( m_entry.m_vxi , impact ) ;
  }
  //
  return StatusCode::SUCCESS ;                                 // RETURN 
}
// ============================================================================
/*  The method for evaluation of the scalar distance between two particles, 
 *  aka "distance of the closest approach" and also its 
 *   \f$\chi^2\f$ for separation significance 
 *  @param p1 (input) the first particle 
 *  @param p2 (input) the second particle 
 *  @param dist (output) the shortest distance between trajectories   
 *  @param chi2 (output,optional) chi2-estimate for the separation significance
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::DistanceCalculator::_distance
( const LHCb::Particle& p1    , 
  const LHCb::Particle& p2    ,
  double&               dist  , 
  const bool            allow , 
  double*               chi2  ) const 
{
  //
  if ( &p1 == &p2 ) 
  {
    dist = 0 ;
    if ( 0 != chi2 ) { *chi2 = 0 ; }
    return _Warning("distance(p,p): the same particle",StatusCode::SUCCESS,0);  
  }
  //
  // the first particle category 
  LoKi::KalmanFilter::ParticleType type1 = particleType_ ( p1 ) ;
  switch ( type1 ) 
  {
  case LoKi::KalmanFilter::GammaLikeParticle     : ;
  case LoKi::KalmanFilter::DiGammaLikeParticle   : ;
  case LoKi::KalmanFilter::MergedPi0LikeParticle :
    {
      dist = 0 ;
      if ( 0 != chi2 ) { *chi2 = 0 ; }
      if ( printStat() && msgLevel( MSG::INFO ) ) { ++counter ( "DOCA->(DI)GAMMA" ) ; }
      return StatusCode::SUCCESS ;                                    // RETURN 
    }
  case LoKi::KalmanFilter::ShortLivedParticle : 
    {
      if ( allow )
      {
        const LHCb::VertexBase* v1 = p1.endVertex() ;
        if ( 0 == v1 ) { break ; }  
        if ( printStat() && msgLevel ( MSG::INFO ) ) { ++counter ( "DOCA->IP" ) ; }
        // make the proper evaluations 
        Gaudi::XYZVector impact ;
        StatusCode sc  = _distance ( p2 , *v1 , impact , allow , chi2 ) ;
        dist           = impact.R() ;
        return sc  ;                                                    // RETURN 
      } 
    }
  default: ;
  }   
  //
  // the second particle category 
  LoKi::KalmanFilter::ParticleType type2 = particleType_ ( p2 ) ;
  switch ( type2 ) 
  {
  case LoKi::KalmanFilter::GammaLikeParticle     : ;
  case LoKi::KalmanFilter::DiGammaLikeParticle   : ;
  case LoKi::KalmanFilter::MergedPi0LikeParticle : 
    {
      dist = 0 ;
      if ( 0 != chi2 ) { *chi2 = 0 ; }
      if ( printStat() && msgLevel( MSG::INFO ) ) { ++counter ( "DOCA->(DI)GAMMA" ) ; }
      return StatusCode::SUCCESS ;                                    // RETURN 
    }
  case LoKi::KalmanFilter::ShortLivedParticle :
    {
      if ( allow ) 
      {
        const LHCb::VertexBase* v2 = p2.endVertex() ;
        if ( 0 == v2 ) { break ; }  
        if ( printStat() && msgLevel ( MSG::INFO ) ) { ++counter ( "DOCA->IP" ) ; }
        // make the proper evaluations 
        Gaudi::XYZVector impact ;
        StatusCode sc  = _distance ( p1 , *v2 , impact , allow , chi2 ) ;
        dist           = impact.R() ;
        return sc ;                                                    // RETURN 
      }
    }
  default: ;
  }
  //
  using namespace Gaudi::Math::Operators ;
  //
  Gaudi::XYZPoint point1 ;
  Gaudi::XYZPoint point2 ;
  //
  // make the evaluation of the distance:
  //
  i_distance ( p1 , p2 , point1 , point2 ) ;
  //
  const LHCb::Particle* good1 = &p1 ;
  const LHCb::Particle* good2 = &p2 ;
  //
  //
  if ( !fastalgo() ) 
  {
    //
    double dz1 = std::abs ( point1.Z() - good1 -> referencePoint().Z() ) ;
    double dz2 = std::abs ( point2.Z() - good2 -> referencePoint().Z() ) ;
    double dz  = std::max ( dz1 , dz2 ) ;
    //
    // make iterations here (if needed)
    //
    for ( unsigned int iIter = 0 ; 
          dz >= m_deltaZ && iIter < std::max ( 1u , m_nIter_max ) ; ++iIter ) 
    {
      //
      // transport the first particle into new positions:
      //
      StatusCode sc = transport 
        ( good1       ,    // what to transport 
          point1      ,    // where to transport 
          m_particle1 )  ; // destination 
      if ( sc.isFailure() ) 
      { _Warning ( "distance(III):Error from ParticleTransporter, ignore", sc, 0 ) ; }
      else { good1 = &m_particle1 ; } // the properly transported particles:
      //
      // transport the second particle into new positions:
      //
      sc = transport 
        ( good2       ,    // what to transport 
          point2      ,    // where to transport 
          m_particle2 )  ; // destination 
      if ( sc.isFailure() ) 
      { _Warning ( "distance(III):Error from ParticleTransporter, ignore", sc, 0 ) ; }
      else { good2 = &m_particle2 ; } // the properly transported particles:
      //
      // make new (improved) evaluation of the distance:
      //
      i_distance ( *good1 , *good2 , point1 , point2 ) ;
      //
      // check delta_Z 
      //
      dz1 = std::abs ( point1.Z () - good1 -> referencePoint().Z() ) ;
      dz2 = std::abs ( point2.Z () - good2 -> referencePoint().Z() ) ;
      dz  = std::max ( dz1 , dz2 ) ;
      
    } /// end of iterations
    //
    // check for  the convergency
    //
    if ( dz >= m_deltaZ ) { _Warning ( "There is no convergency-III", NoConvergency, 0 ) ; }  
  }
  
  //
  // evaluate the distance 
  //
  dist = ( point1 - point2 ) . R () ;
  //
  // evaluate chi2 (if needed) 
  //
  if ( 0 != chi2 ) 
  {
    // ========================================================================
    *chi2 = 1.e+10 ;    
    //
    // prepare the Kalman Filter machinery
    //
    m_entries.resize(2) ;
    LoKi::KalmanFilter::Entries::iterator first  = m_entries.begin() ;
    LoKi::KalmanFilter::Entries::iterator second = first + 1         ;   
    //
    // load as long-lived particles:   
    //
    StatusCode sc = LoKi::KalmanFilter::load 
      (  *good1 , LoKi::KalmanFilter::LongLivedParticle , *first  ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "distance(III): error from KalmanFilter::load(1)" , sc ) ; }
    //
    sc            = LoKi::KalmanFilter::load 
      (  *good2 , LoKi::KalmanFilter::LongLivedParticle , *second ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "distance(III): error from KalmanFilter::load(2)" , sc ) ; }
    //    
    // make the special step of Kalman filter 
    //
    sc = LoKi::KalmanFilter::step ( *first  , *second , 0 ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "distance(III): error from KalmanFilter::step(2)" , sc ) ; }
    //
    // get the final chi2 
    //
    *chi2 = second->m_chi2 ;
    // ========================================================================
  }
  //
  return StatusCode::SUCCESS ;                                 // RETURN
}
// ============================================================================
/*  the method for the evaluation of "path"-distance
 *  @param primary  (input) the production(primary) vertex 
 *  @param particle (input) the particle 
 *  @param decay    (input) the decay vertex of the particle 
 *  @param path     (output) the path-distance  
 *  @param error    (output) the error inpath distance 
 *  @param chi2     (output) the chi2 of the procedure 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::DistanceCalculator::_distance 
( const LHCb::VertexBase& primary  , // the production/primary vertex  
  const LHCb::Particle&   particle , // the particle 
  const LHCb::VertexBase& decay    , // the decay vertex   
  double&                 path     , // the estimate of path-distance
  double&                 error    , // the error in  path-distance 
  double&                 chi2     ) const 
{
  //
  // reset the initial values 
  error = -1.e+10 * Gaudi::Units::mm ;
  chi2  = -1.e+10                    ;
  
  const LHCb::Particle* good = &particle ;
  if ( !fastalgo() ) 
  {
    // propagate particle into its own decay vertex:
    StatusCode sc = transport ( &particle , decay.position() , m_particle1 ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "distance(IV):Error from IParticleTransporter" , sc ) ; }
    
    // the properly transported particle
    good = &m_particle1 ;  
  }
  
  // get the first estimate of the path parameter
  StatusCode sc = LoKi::Fitters::path0 ( primary , *good , decay , path ) ;
  if ( sc.isFailure() ) 
  {
    _Warning ( "distance(IV):Error code from LoKi::Fitters::path0", sc, 0 ) ;
    path = 0 ;
  }
  
  // get the expansion point:
  Gaudi::LorentzVector momentum   = good     -> momentum       () ;
  Gaudi::XYZPoint      decvertex  = good     -> referencePoint () ;
  Gaudi::XYZPoint      primvertex = primary  .  position       () ;
  
  //
  // make one step of the fit:
  //
  sc = LoKi::Fitters::path_step 
    ( primary , *good , momentum , decvertex , primvertex , m_fitter ) ;
  if ( sc.isFailure() ) 
  { return _Error ( "_pathDistance(): error from path_step" , sc ) ; }
  
  // get the parameters from the fitter
  path  = m_fitter.m_var  ;
  chi2  = m_fitter.m_chi2 ;
  // get the error in "path" 
  error = std::sqrt ( m_fitter.m_Vvar ) ;
  //
  if ( fastalgo() ) { return StatusCode::SUCCESS ; }   // RETURN
  
  //
  // reset parameters: 
  //

  momentum   = good     -> momentum       () ;
  decvertex  = good     -> referencePoint () ;
  primvertex = primary  .  position       () ;
  
  /// make iterations here (if needed)
  for ( unsigned int iIter = 0 ; iIter < std::max ( 1u , m_nIter_max ) ; ++iIter ) 
  {
    //
    // make one step of the fit:
    //
    m_fitter.m_var = path ; 
    StatusCode sc = LoKi::Fitters::path_step 
      ( primary , *good , momentum , decvertex , primvertex , m_fitter ) ;
    if ( sc.isFailure() ) 
    {
      // reset  to the initial expansion point and reiterate 
      _Warning ( "distance(IV): error from path_step", sc, 0 ) ;
      momentum   = good     -> momentum       () ;
      decvertex  = good     -> referencePoint () ;
      primvertex = primary  .  position       () ;
      path       = 0 ; 
      continue  ;                                                 // CONTINUE
    }
    
    const double o_chi2 = chi2 ;
    const double o_path = path ;
    
    // get the parameters from the fitter
    path  = m_fitter.m_var  ;
    chi2  = m_fitter.m_chi2 ;
    
    // check for the convergency: 
    //    - by the change in either chi2 *OR* in the proper lifetime  
    if ( std::abs ( chi2  - o_chi2 ) < m_delta_chi2 || 
         std::abs ( path  - o_path ) < m_delta_path ) 
    {
      // get the error in "path" 
      error = std::sqrt ( m_fitter.m_Vvar ) ;
      //
      return StatusCode::SUCCESS ;                                   // RETURN 
    }  
  }
  
  _Warning ( "There is no convergency-IV" , NoConvergency, 0 ) ;
  // get the error in "path" 
  error = std::sqrt ( m_fitter.m_Vvar ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================ 
/* Calculate the projected distance 
 * @param primary  (input)  the production vertex 
 * @param particle (input)  the particle 
 * @param decay    (input)  the decay particle 
 * @param dist     (output) the projected distance
 * @param error    (output) the estimate of the error in the distance 
 * @return status code 
 */
// ============================================================================ 
StatusCode LoKi::DistanceCalculator::_distance   
( const LHCb::VertexBase& primary  ,         // the production vertex 
  const LHCb::Particle&   particle ,         // the particle  
  const LHCb::VertexBase& decay    ,         // the production vertex 
  double&                 dist     ,         // the distance 
  double*                 error    ) const   // the error in distance 
{

  using namespace Gaudi::Math::Operators ;
  
  //
  //  properly propagated particle:
  //
  const LHCb::Particle* good = &particle ;
  if ( !fastalgo() ) 
  {
    // propagate particle into its own decay vertex:
    StatusCode sc = transport ( &particle , decay.position() , m_particle1 ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "distance(IV):Error from IParticleTransporter" , sc ) ; }
    
    // the properly transported particle
    good = &m_particle1 ;  
  }
  
  //
  // evaluate the projected distance 
  //
  dist = i_distance ( primary  , *good , decay ) ;
  
  // evaluate the error? 
  if ( 0 == error ) { return StatusCode::SUCCESS ; }         // RETURN 
  //
  
  const Gaudi::XYZPoint& vd   = decay    . position() ;
  const Gaudi::XYZPoint& vpv  = primary  . position() ;
  const Gaudi::XYZVector  p   = good     -> momentum() . Vect () ;
  //
  //  H = (vd-vp)*p - s* |p|
  //
  // get dH/d(p4)
  Gaudi::Vector4 dHdp4 ;
  const Gaudi::XYZVector delta = vd - vpv - dist * p.Unit() ;
  dHdp4 ( 0 ) = delta.X () ; // dH/d(px) 
  dHdp4 ( 1 ) = delta.Y () ; // dH/d(py)
  dHdp4 ( 2 ) = delta.Z () ; // dH/d(pz) 
  dHdp4 ( 3 ) = 0          ; // dH/d(E) 
  //
  // get dH/d(vd) 
  const Gaudi::XYZVector& dHdvd =  p ; // dH/d(vd) 
  // get dH/d(vp) 
  // const Gaudi::XYZVector  dHdvp = -p ; // dH/d(vp) 
  //
  // V_D = ( D^T * V * D ) ^{-1}
  const double V_D = 1.0 /
    ( ROOT ::Math::Similarity ( dHdp4 , good ->    momCovMatrix ()     ) + 
      2 * ROOT::Math::Dot     ( dHdp4 , good -> posMomCovMatrix () * p ) + 
      Gaudi::Math::Similarity ( dHdvd , decay    .    covMatrix ()     ) + 
      Gaudi::Math::Similarity ( dHdvd , primary  .    covMatrix ()     ) );
  
  // v_s = ( E^T * V_D * E ) ^ {-1}
  //  where E = d(H)/d(s) = -|p|  
  const double V_s = 1.0 / ( V_D * p.Mag2() ) ;
  //
  if ( 0 <= V_s ) { *error = std::sqrt ( V_s ) ; }
  else 
  {
    *error = -1 ;
    _Warning ( "distance(V):The negative covarinace, return error=-1" ) ;    
  }
  //
  return StatusCode::SUCCESS ;                                       // RETURN
}
// ============================================================================
/*  The method for the evaluation of the impact parameter ("distance") 
 *  vector of the particle with respect to some vertex. 
 *  @param tarck    (input)  the tarck 
 *  @param vertex   (input)  the vertex 
 *  @param imppar   (output) the impact parameter ("distance") vector 
 *  @param chi2     (output,optional) the chi2 of the impact parameter
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::DistanceCalculator::_distance   
( const LHCb::Track&      track  ,
  const LHCb::VertexBase& vertex , 
  Gaudi::XYZVector&       impact , 
  double*                 chi2   ) const 
{
  //
  const double vZ = vertex.position().Z() ;
  //
  const LHCb::State& s  = state ( track , vZ ) ;
  const LHCb::State* ss = &s ;
  //
  // get the distance 
  // 
  i_distance ( *ss , vertex , impact ) ;
  //
  if ( !fastalgo() ) 
  {  
    double dz = std::abs ( vZ + impact.Z() - ss->z() ) ;
    //  
    // make iterations (if needed) 
    //
    for ( unsigned int iIter = 0 ; 
          dz >= m_deltaZ && iIter < std::max ( 1u , m_nIter_max ) ; ++iIter ) 
    { 
      // transport the state 
      StatusCode sc = stateAtZ ( m_state1 , track , vZ + impact.Z() ) ;    
      if ( sc.isFailure() ) 
      { _Warning ("distance(VI): can't get the proper state(1)", sc ) ; }
      else 
      { ss = &m_state1 ; }
      //
      // get the distance 
      i_distance ( *ss , vertex , impact ) ;
      //
      dz = std::abs ( vZ + impact.Z() - ss->z() ) ;
      // 
    }
    // check for  the convergency
    if ( dz >= m_deltaZ )
    { _Warning ( "There is no convergency-VI", NoConvergency, 0 ) ; }
  }
  //
  // evaluate chi2 (if needed) 
  //
  if ( 0 != chi2 ) 
  {
    // ========================================================================
    *chi2 = -1.e+10 ;
    // prepare the Kalman Filter machinery 
    StatusCode sc = LoKi::KalmanFilter::load ( ss , m_entry4 ) ;
    if ( sc.isFailure() ) 
    { return _Error("_distance(VI): error from KalmanFilter::load", sc ) ; }
    // get the "the previus" Kalman Filter estimate == vertex
    Gaudi::SymMatrix3x3 ci = vertex.covMatrix() ; // the gain matrix 
    if ( !ci.Invert() ) 
    { return _Error ( "_distance(VI): unable to calculate the gain matrix" ) ; }
    // make one step of Kalman filter 
    sc = LoKi::KalmanFilter::step ( m_entry4 , vertex.position() , ci , 0 ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "_distance(VI): error from Kalman Filter step" , sc ) ; }
    // get the chi2 
    *chi2 = m_entry4.m_chi2 ;
    // ========================================================================
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  The method for the evaluation of the distance between two tracks 
 *  @param track1  (input)  the first tarck 
 *  @param track2  (input)  the first tarck 
 *  @param doca    (output) the distance 
 *  @param chi2    (output,optional) the chi2 of distance 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::DistanceCalculator::_distance   
( const LHCb::Track&      track1 ,
  const LHCb::Track&      track2 ,
  double&                 doca   ,
  double*                 chi2   ) const 
{
  //
  if ( &track1 == &track2 ) 
  {
    doca = 0 ;
    if ( 0 != chi2 ) { *chi2 = 0 ; }
    return _Warning("distance(t,t): the same track",StatusCode::SUCCESS,0 ) ;  
  }
  //
  Gaudi::XYZPoint point1 ;
  Gaudi::XYZPoint point2 ;
  //
  const LHCb::State& s1 =  state ( track1 ) ;
  const LHCb::State& s2 =  state ( track2 ) ;
  //
  // get the distance 
  // 
  const LHCb::State* ss1 = &s1 ;
  const LHCb::State* ss2 = &s2 ;
  //
  // get the estimate:
  i_distance ( *ss1 , *ss2 , point1 , point2 ) ;
  //
  if ( !fastalgo() ) 
  {
    //
    double dz1 = std::abs ( point1.Z() - ss1->z() ) ;
    double dz2 = std::abs ( point2.Z() - ss2->z() ) ;
    double dz  = std::max ( dz1 , dz2 ) ;
    //
    if ( dz1 <= m_deltaZ ) { m_state1 = *ss1 ; }
    if ( dz2 <= m_deltaZ ) { m_state2 = *ss2 ; }
    //
    // make the iterations (if needed) 
    for ( unsigned int iIter = 0 ; 
          dz >= m_deltaZ && iIter < std::max ( 1u , m_nIter_max ) ; ++iIter ) 
    {
      //
      if ( m_deltaZ < dz1 ) 
      {
        StatusCode sc = stateAtZ ( m_state1 , track1 , point1.Z () ) ;
        if ( sc.isFailure() ) { _Warning ("distance(V): can't get the proper state(1)" , sc ) ; } 
        ss1 = sc.isFailure() ? &s1 : &m_state1 ;
      }
      //
      if ( m_deltaZ < dz2 ) 
      {
        StatusCode sc = stateAtZ ( m_state2 , track2 , point2.Z () ) ;
        if ( sc.isFailure() ) { _Warning ("distance(V): can't get the proper state(2)" , sc ) ; } 
        ss2 = sc.isFailure() ? &s2 : &m_state2 ;
      }
      //
      i_distance ( *ss1 , *ss2 , point1 , point2 ) ;
      //    
      dz1 = std::abs ( point1.Z() - ss1->z() ) ;
      dz2 = std::abs ( point2.Z() - ss2->z() ) ;
      dz  = std::max ( dz1 , dz2 ) ;
      //
    }
    // check for  the convergency
    if ( dz >= m_deltaZ )
    { _Warning ( "There is no convergency-V", NoConvergency, 0 ) ; }
    //  
  }
  
  //
  // evaluate the distance 
  //
  doca = ( point1 - point2 ) . R () ;
  //
  // get chi2 if needed 
  //
  if ( 0 != chi2 ) 
  {
    // =======================================================================
    *chi2 = 1.e+10 ;
    // prepare the Kalman Filter machinery
    m_entries4.resize ( 2 ) ;
    LoKi::KalmanFilter::TrEntries4::iterator first  = m_entries4.begin() ;
    LoKi::KalmanFilter::TrEntries4::iterator second = first + 1          ;   
    //
    StatusCode sc = LoKi::KalmanFilter::load ( ss1 , *first  ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "_distance(V): error from KalmanFilter::load(1)" , sc ) ; }
    //
    sc            = LoKi::KalmanFilter::load ( ss2 , *second ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "_distance(V): error from KalmanFilter::load(2)" , sc ) ; }
    //
    // make the special step of Kalman filter 
    sc            = LoKi::KalmanFilter::step ( *first  , *second , 0 ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "distance(V): error from KalmanFilter::step(2)" , sc ) ; }
    //
    // get the final chi2 
    *chi2 = second->m_chi2 ;
    // ========================================================================
  }  
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// the factory (needed for instantiation)
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,DistanceCalculator)
// ============================================================================
// The END 
// ============================================================================
