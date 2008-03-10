// $Id: DistanceCalculator.cpp,v 1.1 2008-03-10 18:24:43 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKenrel
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
// local
// ============================================================================
#include "DistanceCalculator.h"
#include "FitterUtils.h"
// ============================================================================
/** @file 
 *  the implementation of the class LoKi::DistanceCalculator 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-03-05
 */
// ============================================================================
/** Standard constructor
 *  @param type tool type(?)
 *  @param name tool instance name 
 *  @param parent the pointer to the parent
 */
// ============================================================================
LoKi::DistanceCalculator::DistanceCalculator
( const std::string& type   , // tool type (?)
  const std::string& name   , // tool instance name 
  const IInterface*  parent ) // the parent 
  : LoKi::DistanceCalculatorBase ( type , name , parent ) 
  /// the maximal number of iterations 
  , m_nIter_max ( 10 ) // the maximal number of iterations 
  /// the convergency criterion for ip-evaluation
  , m_deltaZ     ( 1 * Gaudi::Units::micrometer ) // the criteria for ip-evaluation
  /// the convergency criterion for delta(chi2) 
  , m_delta_chi2 ( 0.01 ) 
  /// the convergency criterion for delta(path) 
  , m_delta_path ( 1 * Gaudi::Units::micrometer )
  /// The name of particle transpoter tool 
  , m_transporterName ( "ParticleTransporter:PUBLIC" ) // The name of particle transpoter tool
  /// The tarnsported tool itself 
  , m_transporter ( 0 ) // The transporter tool itself 
  /// the local storages of particles
  , m_particle1 ( ) 
  , m_particle2 ( )
{
  //
  declareInterface<IDistanceCalculator> ( this ) ;
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
    ( "Transporter"      , m_transporterName     , 
      "The Particle Transporter tool to be used" );
}
// ============================================================================
// virtual and protected desctrustor 
// ============================================================================
LoKi::DistanceCalculator::~DistanceCalculator(){}
// ============================================================================
//  The method for the evaluation of the impact parameter ("distance")
//  vector of the particle with respect to some vertex. 
// ============================================================================
StatusCode LoKi::DistanceCalculator::_distance
( const LHCb::Particle&   particle ,
  const LHCb::VertexBase& vertex   , 
  Gaudi::XYZVector&       impact   , 
  double*                 chi2     ) const 
{
  
  using namespace Gaudi::Math::Operators ;
  
  // make the fast evaluation:
  i_distance ( particle , vertex , impact ) ;
  
  const double Vz = vertex.position().Z() ;
  
  const LHCb::Particle* good  = &particle ;
  
  double deltaZ = ::fabs ( Vz + impact.Z() - good->referencePoint().Z() ) ;
  
  // make the iterations (if needed) 
  unsigned int nIter = 0 ;
  for ( unsigned int iIter = 0 ; 
        deltaZ >= m_deltaZ && iIter < m_nIter_max ; ++iIter ) 
  { 
    // transport the particle into the new impact parameter point.
    StatusCode sc = transport 
      ( good             ,   // what to be transported 
        vertex.position() + impact   ,   // new Z 
        m_particle1      ) ; // the result 
    if ( sc.isFailure() ) 
    { Warning ( "Error from ParticleTransporter, ignore" , sc ) ; }
    else 
    { good = &m_particle1 ; } // the properly transported particle 
    
    // make the new evaluation of impact parameter
    i_distance ( *good , vertex , impact ) ;
    
    // check the delta_z 
    deltaZ = ::fabs ( Vz + impact.Z() - good->referencePoint().Z() ) ;
    
    ++nIter ;
  } /// the end of iteration loop 
  
  // check for  the convergency
  if ( deltaZ >= m_deltaZ )
  { 
    counter ("delta(Z)-I") += deltaZ ;
    return Error ( "Ehere is no convergency", NoConvergency ) ;
  }
  
  // evaluate chi2 (if needed) 
  if ( 0 != chi2 ) 
  {
    *chi2 = -1.e+10 ;
    // prepare the Kalman Filter machinery 
    StatusCode sc = LoKi::KalmanFilter::load ( *good , m_entry ) ;
    if ( sc.isFailure() ) 
    { return Error("_distance(I): error from KalmanFilter::load", sc ) ; }
    // get the "the previus" Kalman Filter estimate == vertex
    Gaudi::SymMatrix3x3 ci = vertex.covMatrix() ; // the gain matrix 
    if ( !ci.Invert() ) 
    { return Error ( "_distance(I): unable to calculate the gain matrix" ) ; }
    // make one step of Kalman filter 
    sc = LoKi::KalmanFilter::step ( m_entry , vertex.position() , ci , 0 ) ;
    if ( sc.isFailure() ) 
    { return Error ( "_distance(I): error from Kalman Filter step" , sc ) ; }
    // get the chi2 
    *chi2 = m_entry.m_chi2 ;
  }
  
  counter ( "#iterations-I" ) += nIter ;
  //
  return StatusCode::SUCCESS ;                                 // RETURN 
}
// ============================================================================
//  The method for the evaluation of the impact parameter ("distance")
//  vector of the particle with respect to  the fixed point 
// ============================================================================
StatusCode LoKi::DistanceCalculator::_distance
( const LHCb::Particle&   particle ,
  const Gaudi::XYZPoint&  point    , 
  Gaudi::XYZVector&       impact   , 
  double*                 chi2     ) const 
{
  
  using namespace Gaudi::Math::Operators ;
  
  // make the fast evaluation:
  i_distance ( particle , point , impact ) ;
  
  const double Vz = point.Z() ;
  
  const LHCb::Particle* good  = &particle ;
  
  double deltaZ = ::fabs ( Vz + impact.Z() - good->referencePoint().Z() ) ;
  
  // make the iterations here (if needed) 
  unsigned int nIter = 0 ;
  for ( unsigned int iIter = 0 ; 
        deltaZ >= m_deltaZ && iIter < m_nIter_max ; ++iIter ) 
  { 
    // transport the particle into the new impact parameter point.
    StatusCode sc = transport 
      ( good             ,   // what to be transported 
        point + impact   ,   // new Z 
        m_particle1      ) ; // the result 
    if ( sc.isFailure() ) 
    { return Error ( "_distance(II): error from transport()" , sc ) ; }
    
    // the transported particle 
    good = &m_particle1 ;
    
    // make the new evaluation of impact parameter
    i_distance ( *good , point , impact ) ;
    
    // check the delta_z 
    deltaZ = ::fabs ( Vz + impact.Z() - good->referencePoint().Z() ) ;
    
    ++nIter ;
  } /// the end of iteration loop 
  
  // check for  the convergency
  if ( deltaZ >= m_deltaZ )
  { 
    counter ("delta(Z)-II") += deltaZ ;
    return Error ( "_distance(II): there is no convergency", NoConvergency ) ;
  }
  
  // evaluate the chi2 (if needed) 
  if ( 0 != chi2 ) 
  {
    *chi2 = -1.e+10 ;
    // prepare the Kalman Filter machinery 
    StatusCode sc = LoKi::KalmanFilter::load ( *good , m_entry ) ;
    if ( sc.isFailure() ) 
    { return Error("_distance(II): eroro from KalmanFilter::load" , sc ) ; }
    // here the evaluations of chi2 is just trivial:
    *chi2 = Gaudi::Math::Similarity ( m_entry.m_vxi , impact ) ;
  }
  
  counter ( "#iterations-II" ) += nIter ;
  //
  return StatusCode::SUCCESS ;                                 // RETURN 
}
// ============================================================================
// The method for evaluation of the scalar distance between two particles, 
//  aka "distance of the closest approach" and also its 
//  chi^2-for separation significance 
// ============================================================================
StatusCode LoKi::DistanceCalculator::_distance
( const LHCb::Particle& p1   , 
  const LHCb::Particle& p2   ,
  double&               dist , 
  double*               chi2 ) const 
{

  using namespace Gaudi::Math::Operators ;
  
  Gaudi::XYZPoint point1 ;
  Gaudi::XYZPoint point2 ;
  // make the evaluation of the distance:
  i_distance ( p1 , p2 , point1 , point2 ) ;
  
  const LHCb::Particle* good1 = &p1 ;
  const LHCb::Particle* good2 = &p2 ;
  
  double dz1 = ::fabs ( point1.Z() - good1 -> referencePoint().Z() ) ;
  double dz2 = ::fabs ( point2.Z() - good2 -> referencePoint().Z() ) ;
  double dz  = std::max ( dz1 , dz2 ) ;
  
  // make iterations here (if needed)
  unsigned int nIter = 0 ;
  for ( unsigned int iIter = 0 ; 
        dz >= m_deltaZ && iIter <= m_nIter_max ; ++iIter ) 
  {
    // transport the first particle into new positions:
    StatusCode sc = transport 
      ( good1       ,  // what to transport 
        point1      ,  // where to transport 
        m_particle1 )  ; // destination 
    if ( sc.isFailure() ) 
    { Warning ( "Error from ParticleTransporter, ignore" , sc ) ; }
    else { good1 = &m_particle1 ; } // the properly transported particles:
    
    // transport the second particle into new positions:
    sc = transport 
      ( good2       ,  // what to transport 
        point2      ,  // where to transport 
        m_particle2 )  ; // destination 
    if ( sc.isFailure() ) 
    { Warning ( "Error from ParticleTransporter, ignore" , sc ) ; }
    else { good2 = &m_particle2 ; } // the properly transported particles:
    
    // make new (improved) evaluation of the distance:
    i_distance ( *good1 , *good2 , point1 , point2 ) ;
    
    // check delta_Z 
    dz1 = ::fabs ( point1.Z () - good1 -> referencePoint().Z() ) ;
    dz2 = ::fabs ( point2.Z () - good2 -> referencePoint().Z() ) ;
    dz  = std::max ( dz1 , dz2 ) ;
    
    ++nIter ;
  } /// end of iterations 
  
  // check for  the convergency
  if ( dz >= m_deltaZ )
  { 
    counter ("delta(Z)-III") += dz ;
    return Error ( "There is no convergency", NoConvergency ) ;
  }
  
  // evaluate the distance 
  dist = ( point1 - point2 ) . R () ;
  
  // evaluate chi2 (if needed) 
  if ( 0 != chi2 ) 
  {
    *chi2 = 1.e+10 ;
    // prepare the Kalman Filter machinery
    m_entries.resize(2) ;
    LoKi::KalmanFilter::Entries::iterator first  = m_entries.begin() ;
    LoKi::KalmanFilter::Entries::iterator second = first + 1         ;   
    
    StatusCode sc = LoKi::KalmanFilter::load ( *good1 , *first  ) ;
    if ( sc.isFailure() ) 
    { return Error ( "_distance(III): error from KalmanFilter::load(1)" , sc ) ; }
    
    sc =            LoKi::KalmanFilter::load ( *good2 , *second ) ;
    if ( sc.isFailure() ) 
    { return Error ( "_distance(III): error from KalmanFilter::load(2)" , sc ) ; }
    
    // construct the proper seed:
    Gaudi::Vector3       x  ;
    Gaudi::SymMatrix3x3 ci ;
    sc = LoKi::KalmanFilter::seed ( m_entries , x , ci , 1.e-4 ) ;
    if ( sc.isFailure() ) 
    { 
      Warning ( "_distance(III): error from KalmanFilter::seed" , sc ) ; 
      // set manually the seed to be equal to th emiddle position 
      Gaudi::Math::geo2LA ( point1 + 0.5 * ( point2 - point1 ) , x ) ;
      // set the gain matrix to be "small enought" 
      const double sc = std::max ( 5 * ( point1 - point2 ).R() , Gaudi::Units::cm ) ;
      Gaudi::Math::setToUnit ( ci , 1./(sc*sc) ) ;
    }
    
    const Gaudi::Vector3*      _x  = &x  ;
    const Gaudi::SymMatrix3x3* _ci = &ci ;
    
    // make the first step of Kalman filter 
    sc = LoKi::KalmanFilter::step ( *first , *_x , *_ci , 0 ) ;
    if ( sc.isFailure() ) 
    { return Error ( "_distance(III): error from KalmanFilter::step(1)" , sc ) ; }
    
    // use the step result as seed for the second step:
    _x  = &first->m_x  ;
    _ci = &first->m_ci ;
    
    // make the second step of Kalman filter 
    sc = LoKi::KalmanFilter::step ( *second , *_x , *_ci , first->m_chi2 ) ;
    if ( sc.isFailure() ) 
    { return Error ( "_distance(III): error from KalmanFilter::step(2)" , sc ) ; }
    
    // get the final chi2 
    *chi2 = second->m_chi2 ;
  }
  //
  counter ( "#iterations-III" ) += nIter ;
  //
  return StatusCode::SUCCESS ;                                 // RETURN
}
// ========================================================================
// the method for the evaluation of "path"-distance
// ========================================================================
StatusCode LoKi::DistanceCalculator::_distance 
( const LHCb::VertexBase& primary  , // the production/primary vertex  
  const LHCb::Particle&   particle , // the particle 
  const LHCb::VertexBase& decay    , // the decay vertex   
  double&                 path     , // the estimate of path-distance
  double&                 error    , // the errr in  path-distance 
  double&                 chi2     ) const 
{
  
  // reset the initial values 
  error = -1.e+10 * Gaudi::Units::mm ;
  chi2  = -1.e+10                    ;
  
  // propagate particle into its own decay vertex:
  StatusCode sc = transport ( &particle , decay.position() , m_particle1 ) ;
  if ( sc.isFailure() ) 
  { return Error ( "The error from IParticleTransporter, ignore" , sc ) ; }

  
  const LHCb::Particle* good = &m_particle1 ;// the properly transported particle
  
  // get the first estimate of the path parameter
  sc = LoKi::Fitters::path0 ( primary , *good , decay , path ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error code from LoKi::Fitters::path0" , sc ) ;
    path = 0 ;
  }
  
  // get the expansion point:
  Gaudi::LorentzVector momentum   = good     -> momentum       () ;
  Gaudi::XYZPoint      decvertex  = good     -> referencePoint () ;
  Gaudi::XYZPoint      primvertex = primary  .  position       () ;
  
  // make iterations here (if needed)
  for ( unsigned int iIter = 1 ; iIter <= m_nIter_max ; ++iIter ) 
  {
    const double o_chi2 = chi2 ;
    const double o_path = path ;
    
    // make one step of the fit:
    sc = LoKi::Fitters::path_step 
      ( primary , *good , momentum , decvertex , primvertex , m_fitter ) ;
    if ( sc.isFailure() ) 
    {
      counter ( "#reset" ) += 1 ;
      // reset  to the initial expansion point and reiterate 
      momentum   = good     -> momentum       () ;
      decvertex  = good     -> referencePoint () ;
      primvertex = primary  .  position       () ;
      m_fitter.m_var = 0  ; 
      continue  ;                                                 // CONTINUE
    }
    
    // get the parameters from the fitter
    path  = m_fitter.m_var  ;
    chi2  = m_fitter.m_chi2 ;
    
    // check for the convergency: 
    //    - by the change in either chi2 *OR* in the proper lifetime  
    if ( ::fabs ( chi2  - o_chi2 ) < m_delta_chi2 
         || 
         ::fabs ( path  - o_path ) < m_delta_path ) 
    {
      // get the error c*tau 
      error = ::sqrt ( m_fitter.m_Vvar ) ;
      //
      counter ( "#iterations-IV" ) += iIter ;
      return StatusCode::SUCCESS ;                                   // RETURN 
    }  
  }
  //
  return Error ( "There is no convergency" , NoConvergency ) ;
}
// ============================================================================ 
// The functions from the interface 
// ============================================================================
// Particle - Vertex 
// ============================================================================
//  The method for the evaluation of the impact parameter ("distance") 
//  of the particle with respect to some vertex. 
// ============================================================================
StatusCode 
LoKi::DistanceCalculator::distance 
( const LHCb::Particle*   particle ,
  const LHCb::VertexBase* vertex   , 
  double&                 imppar   ) const 
{
  StatusCode sc = check ( particle , vertex ) ;
  if ( sc.isFailure() ) { return sc ; }                        // RETURN 
  // make the proper evaluations 
  Gaudi::XYZVector impact ;
  sc = _distance ( *particle , *vertex , impact ) ;
  imppar = impact.R() ;
  return sc ;                                               // RETURN 
}
// ============================================================================
//  The method for the evaluation of the impact parameter ("distance") 
// of the particle with respect to some vertex. 
// ============================================================================
StatusCode 
LoKi::DistanceCalculator::distance 
( const LHCb::Particle*   particle ,
  const LHCb::VertexBase* vertex   , 
  double&                 imppar   , 
  double&                 chi2     ) const 
{
  StatusCode sc = check ( particle , vertex ) ;
  if ( sc.isFailure() ) { return sc ; }                        // RETURN 
  // make the proper evaluations 
  Gaudi::XYZVector impact ;
  sc = _distance ( *particle , *vertex , impact , &chi2 ) ;
  imppar = impact.R() ;
  return sc ;                                               // RETURN 
}
// ============================================================================
// Particle - Point
// ============================================================================
// The method for the evaluation of the impact parameter ("distance")
//  of the particle with respect to fixed point 
// ============================================================================
StatusCode 
LoKi::DistanceCalculator::distance 
( const LHCb::Particle*   particle ,
  const Gaudi::XYZPoint&  point    , 
  double&                 imppar   ) const 
{ 
  StatusCode sc = check ( particle ) ;
  if ( sc.isFailure() ) { return sc ; }                        // RETURN 
  // make the proper evaluations 
  Gaudi::XYZVector impact ;
  sc = _distance ( *particle , point , impact ) ;
  imppar = impact.R() ;
  return sc ;                                               // RETURN 
}
// ============================================================================
// The method for the evaluation of the impact parameter ("distance")
//  of the particle with respect to fixed point 
// ============================================================================
StatusCode 
LoKi::DistanceCalculator::distance 
( const LHCb::Particle*   particle ,
  const Gaudi::XYZPoint&  point    , 
  double&                 imppar   , 
  double&                 chi2     ) const 
{ 
  StatusCode sc = check ( particle ) ;
  if ( sc.isFailure() ) { return sc ; }                        // RETURN 
  // make the proper evaluations 
  Gaudi::XYZVector impact ;
  sc = _distance ( *particle , point , impact , &chi2 ) ;
  imppar = impact.R() ;
  return sc ;                                               // RETURN 
}
// ============================================================================
// Vertex - Vertex 
// ============================================================================
// The trivial method for evaluation of the distance between two vertices 
// ============================================================================
StatusCode 
LoKi::DistanceCalculator::distance 
( const LHCb::VertexBase*  v1   , 
  const LHCb::VertexBase*  v2   , 
  double&                  dist ) const 
{
  StatusCode sc = check ( v1 , v2 ) ;
  if ( sc.isFailure() ) { return sc ; }               // RETURN 
  // make the real calculations 
  return i_distance ( *v1 , *v2 , dist ) ;
}
// ============================================================================
// The method for evaluation of the distance between two vertices and the 
//  corresponding \f$\chi^2\f$ for the separation significance.
// ============================================================================
StatusCode 
LoKi::DistanceCalculator::distance 
( const LHCb::VertexBase*  v1   , 
  const LHCb::VertexBase*  v2   , 
  double&                  dist , 
  double&                  chi2 ) const 
{  
  StatusCode sc = check ( v1 , v2 ) ;
  if ( sc.isFailure() ) { return sc ; }           // RETURN 
  // make the real calculations 
  return i_distance ( *v1 , *v2 , dist , &chi2 ) ;   // RETURN 
}
// ============================================================================
// Vertex - Point
// ============================================================================
// The trivial method for evaluation of the distance between the vertex 
//  and some "fixed" point
// ============================================================================
StatusCode 
LoKi::DistanceCalculator::distance
( const LHCb::VertexBase*  vertex , 
  const Gaudi::XYZPoint&   point  , 
  double&                  dist   ) const 
{
  StatusCode sc = check ( vertex ) ;
  if ( sc.isFailure() ) { return sc ; }                // RETURN 
  // make the real calculations 
  return i_distance ( *vertex , point , dist ) ;       // RETURN 
}
// ============================================================================
// The method for evaluation of the distance between the vertices 
//  and some fixed point and the corresponding chi^2 for 
//  the separation significance.
// ============================================================================
StatusCode 
LoKi::DistanceCalculator::distance
( const LHCb::VertexBase*  vertex , 
  const Gaudi::XYZPoint&   point  , 
  double&                  dist   , 
  double&                  chi2   ) const 
{
  StatusCode sc = check ( vertex ) ;
  if ( sc.isFailure() ) { return sc ; }                    // RETURN 
  // make the real calculations 
  return i_distance ( *vertex , point , dist , &chi2 ) ;   // RETURN 
}
// ============================================================================
// Particle-Particle 
// ============================================================================
// The method for evaluation of the scalar distance between two particles, 
//  aka "distance of the closest approach". 
// ============================================================================
StatusCode 
LoKi::DistanceCalculator::distance
( const LHCb::Particle* p1   , 
  const LHCb::Particle* p2   , 
  double&               dist ) const 
{
  StatusCode sc = check ( p1 , p2 ) ;
  if ( sc.isFailure() ) { return sc ; }                    // RETURN 
  // make the real calculations 
  return _distance ( *p1 , *p2 , dist ) ;                  // RETURN  
}
// ============================================================================
// The method for evaluation of the scalar distance between two particles, 
// aka "distance of the closest approach" and also its 
//  chi2 for separation significance 
// ============================================================================
StatusCode 
LoKi::DistanceCalculator::distance
( const LHCb::Particle* p1   , 
  const LHCb::Particle* p2   ,
  double&               dist , 
  double&               chi2 ) const 
{
  StatusCode sc = check ( p1 , p2 ) ;
  if ( sc.isFailure() ) { return sc ; }                      // RETURN 
  // make the real calculations 
  return _distance ( *p1 , *p2 , dist , &chi2 ) ;            // RETURN  
} 
// ============================================================================
// The method for evaluation of the "path"-distance 
//  between the decay vertex of the particle and the vertex.
// ============================================================================
StatusCode 
LoKi::DistanceCalculator::pathDistance 
( const LHCb::Particle*   particle , 
  const LHCb::VertexBase* primary  ,  
  double&                 path     ,
  double&                 error    ,
  double&                 chi2     ) const 
{
  // check the input data 
  StatusCode sc = check ( particle , primary ) ;
  if ( sc.isFailure() ) { return sc ; }                     // RETURN 
  // check the end-vertex
  const LHCb::VertexBase* decay = particle->endVertex() ;
  sc = check ( decay ) ;
  if ( sc.isFailure() ) { return sc ; }                     // RETURN 
  // make the real evaluation:
  //return _distance ( *primary  , *particle , *decay , path , error , chi2 ) ;
  return Warning("pathDistance: not yet imeplmented", NotImplemented ) ;
}
// ============================================================================
// Calculate the projected distance 
// ============================================================================
StatusCode 
LoKi::DistanceCalculator::projectedDistance   
( const LHCb::Particle*   particle , 
  const LHCb::VertexBase* primary  , 
  double&                 dist     , 
  double&                 error    ) const 
{
  // check the input data 
  StatusCode sc = check ( particle , primary ) ;
  if ( sc.isFailure() ) { return sc ; }                     // RETURN 
  // check the end-vertex
  const LHCb::VertexBase* decay = particle->endVertex() ;
  sc = check ( decay ) ;
  if ( sc.isFailure() ) { return sc ; }                     // RETURN 
  // make the real evaluation:
  //return _distance ( *primary  , *particle , *decay , path , error ) ;
  return Warning("pathDistance: not yet imeplmented", NotImplemented ) ;
}





// ============================================================================
/// the factory (needed for instantiation)
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,DistanceCalculator) ;
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
