// $Id: VertexFitter.cpp,v 1.8 2010-03-08 11:35:35 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/IVertexFit.h"
#include "Kernel/IParticleTransporter.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/MatrixTransforms.h"
// ============================================================================
// ROOT/Mathlib
// ============================================================================
#include "Math/Functions.h"
// ============================================================================
// Local 
// ============================================================================
#include "KalmanFilter.h"
#include "VertexFitter.h"
// ============================================================================
/// anonymous namespace to hide few technical constants
namespace 
{
  // ==========================================================================
  const double s_scale   = Gaudi::Units::perCent           ;
  const double s_scale2  = s_scale  * s_scale              ;
  const double s_small   = 0.1 * Gaudi::Units::micrometer  ;
  const double s_small2  = s_small  * s_small              ;
  const double s_middle  =  1  * Gaudi::Units::centimeter  ;
  const double s_middle2 = s_middle * s_middle             ;
  const double s_large   = 10  * Gaudi::Units::centimeter  ;
  const double s_large2  = s_large  * s_large              ; 
  // ==========================================================================
}
// ============================================================================
// load the data from the daughter particles into the internal structures 
// ============================================================================
StatusCode LoKi::VertexFitter::_load 
( const LHCb::Particle::ConstVector& ds ) const 
{
  StatusCode sc = StatusCode::SUCCESS ;
  m_entries.clear() ;
  m_entries.resize ( ds.size() ) ;
  LHCb::Particle::ConstVector::const_iterator c = ds.begin()        ;
  Entries::iterator                           e = m_entries.begin() ;
  //
  for ( ; ds.end() != c && sc.isSuccess() ; ++c , ++e ) 
  { sc = _load ( *c , *e ) ; } ;
  if ( sc.isFailure () ) 
  { return Error ( "_load(): the error from _load:" , sc          , 1 ) ; } // RETURN 
  if ( m_entries.empty() ) 
  { return Error ( "_load(): no valid data found"   , InvalidData , 1 ) ; } // RETURN 
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// load the data into internal representation 
// ============================================================================
StatusCode LoKi::VertexFitter::_load      
( const LHCb::Particle*     particle , 
  LoKi::VertexFitter::Entry& entry    ) const 
{
  if ( 0 == particle ) 
  { return Error ( "_load(): invalid particle" , InvalidParticle , 1 ) ; } // RETURN 
  //
  switch ( particleType ( particle ) ) 
  {
    //
  case LoKi::KalmanFilter::LongLivedParticle  : 
    return LoKi::KalmanFilter::loadAsFlying     ( *particle  , entry ) ; // RETURN 
    //
  case LoKi::KalmanFilter::GammaLikeParticle  : 
    return LoKi::KalmanFilter::loadAsGamma      ( *particle  , entry ) ; // RETURN 
    // 
  case LoKi::KalmanFilter::ShortLivedParticle : 
    return LoKi::KalmanFilter::loadAsShortLived ( *particle  , entry ) ; // RETURN 
    //
  default:
    return LoKi::KalmanFilter::load         ( *particle  , entry ) ; // RETURN 
  }
  //
  return LoKi::KalmanFilter::load ( *particle  , entry ) ;  
}
// ============================================================================
// add one particle at the end of the queue
// ============================================================================
StatusCode LoKi::VertexFitter::_add 
( const LHCb::Particle*              child     , 
  const double                       newZ      ) const
{
  m_entries.push_back( Entry() ) ;
  StatusCode sc = _load      ( child , m_entries.back() ) ;
  if ( sc.isFailure() ) 
  { Warning ("_add(): the error from _add()      , ignore", sc , 1 ).ignore() ; }
  sc = _transport ( m_entries.back() , newZ ) ;
  if ( sc.isFailure() ) 
  { Warning ("_add(): the error from _transport(), ignore", sc , 1 ).ignore() ; }
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// make few kalman iterations 
// ============================================================================
StatusCode LoKi::VertexFitter::_iterate 
( const size_t               nIterMax , 
  const Gaudi::Vector3&      _x       ) const 
{  
  // initial position
  const Gaudi::Vector3* x = &_x ;
  // chi2 
  const double  _chi2 = 0 ;
  const double*  chi2 = &_chi2 ;
  // inverse covariance matrix for the position  
  const Gaudi::SymMatrix3x3* ci = &m_seedci ;
  //
  for ( size_t iIter = 1 ; iIter <= nIterMax ; ++iIter ) 
  {    
    // make a proper transportation 
    const double newZ = (*x)(2) ;
    StatusCode sc = _transport ( newZ ) ;
    if ( sc.isFailure() ) 
    { Warning ( "_iterate(): problem with transport ", sc , 1 ).ignore() ;}    
    // initialize the covariance matrix 
    if ( 0 != iIter ) { m_seedci = (*ci) * s_scale2 ; }
    ci   = &m_seedci ;
    // initialize the chi2 
    chi2 = &_chi2 ;
    const Gaudi::Vector3 x0 ( *x ) ;    
    // start the kalman filter 
    bool done = false ;
    // A) the simplest case: 2 body fit 
    if  ( 2 == m_entries.size() && m_use_twobody_branch ) 
    {
      sc = LoKi::KalmanFilter::step ( m_entries[0] , 
                                      m_entries[1] , 
                                      *x , *ci , *chi2 ) ;
      if ( sc.isSuccess () ) 
      {
        // update the parameters 
        const LoKi::KalmanFilter::Entry& last = m_entries.back() ;
        ci   = &last.m_ci   ;
        x    = &last.m_x    ;
        chi2 = &last.m_chi2 ;
        //
        done = true ;
      }
      else { Warning ( "Error from twobody Kalman step" , sc , 1 ).ignore() ; }      
    }
    // B) general case (of failure of two-body specialiation) 
    if  ( !done ) 
    {
      for ( EIT entry = m_entries.begin() ; m_entries.end() != entry ; ++entry ) 
      {
        // make one Kalman step 
        sc = LoKi::KalmanFilter::step ( *entry , *x , *ci , *chi2 ) ;
        // skip on Failure
        if ( sc.isFailure() ) 
        {  
          Warning ( "Error from Kalman-step, skip" , sc , 1 ).ignore() ;
          continue ;                                                // CONTINUE 
        }
        // update the parameters 
        ci   = &entry->m_ci   ;
        x    = &entry->m_x    ;
        chi2 = &entry->m_chi2 ;
      }
    }
    // kalman smooth
    sc = LoKi::KalmanFilter::smooth ( m_entries ) ;
    if ( sc.isFailure() ) 
    { Warning ( "_iterate(): problem with smoother", sc , 1 ).ignore() ; }
    // distance in the absolute position 
    const double d1 = ROOT::Math::Mag        ( (*x) - x0 ) ;
    // distance in the chi2 units 
    const double d2 = ROOT::Math::Similarity ( (*x) - x0 , *ci ) ;
    if ( d2 < 0 ) 
    {  Warning ( "_iterate: negative chi2 detected, ignore" , sc , 1 ).ignore() ; }
    // termination conditions:
    //
    //  (1) STOP if the distance is sufficiently small 
    //      - either the absolute distance 
    //      - or chi2 distance (if at least one iteration is performed) 
    //
    if ( d1 < m_DistanceMax ) // || ( 1 < iIter && 0 <= d2 && d2 < m_DistanceChi2 ) )
    {
      sc = LoKi::KalmanFilter::evalCov ( m_entries ) ;
      if ( sc.isFailure() ) 
      { Warning ( "_iterate(): problems with covariances" , sc , 1 ).ignore() ; }
      // 
      counter ( "#iterations" ) += iIter ;
      //
      return StatusCode::SUCCESS ;                             // RETURN 
    } 
  } // end of iterations
  //
  return Warning ( "No convergency has been reached" , NoConvergency , 1  ) ; // RETURN 
} 
// ============================================================================
// make a seed 
// ============================================================================
StatusCode LoKi::VertexFitter::_seed ( const LHCb::Vertex* vertex ) const
{
  // check if vertex could be used as a seed 
  const Gaudi::XYZPoint&     p = vertex->position  () ;
  const Gaudi::SymMatrix3x3& c = vertex->covMatrix () ;
  
  if ( m_seedZmin < p.Z() && m_seedZmax > p.Z() && m_seedRho > p.Rho()   && 
       s_small2 < Gaudi::Math::min_diagonal ( c ) && 
       s_large2 > Gaudi::Math::max_diagonal ( c )   ) 
  {
    // use the vertex parameters as proper seed 
    Gaudi::Math::geo2LA ( p , m_seed ) ;
    int ifail = 0 ;
    m_seedci = c.Inverse( ifail ) ;
    if ( !ifail ) 
    {
      // properly scale the seed matrix 
      Gaudi::Math::scale ( m_seedci , s_scale2 ) ;
      return StatusCode::SUCCESS ;                            // RETURN 
    } 
  }
  
  StatusCode sc = LoKi::KalmanFilter::seed 
    ( m_entries , m_seed , m_seedci , s_scale2 ) ;
  
  if ( sc.isFailure() ) 
  { 
    m_seed[0] = 0.0 ; m_seed[1] = 0.0 ; m_seed [2] = 0.0 ;
    Gaudi::Math::setToUnit ( m_seedci , 1.0/s_middle2 ) ;
    m_seedci(2,2) = 1.0/s_large2 ;
    Warning ( "_seed(): error in matrix inversion" , sc , 1 ).ignore()  ; 
  }
  /// check the validity of the seed 
  Gaudi::XYZPoint pnt ( m_seed[0] , m_seed[1] , m_seed[2] ) ;
  if ( m_seedZmin > pnt.Z() ) 
  { 
    Warning ("_seed(): Seed is outside of 'Zmin' fiducial volume " , sc , 1 ).ignore() ; 
    m_seed[2] = 0.5 * ( 0.0 + m_seedZmin ) ; 
    m_seedci(0,2) = 0 ; m_seedci(1,2) = 0 ;
    m_seedci(2,2) = 4.0/m_seedZmin/m_seedZmin;
  }
  if ( m_seedZmax < pnt.Z() ) 
  { 
    Warning ("_seed(): Seed is outside of 'Zmax' fiducial volume " , sc , 1 ).ignore() ; 
    m_seed[2] = 0.5 * ( 0.0 + m_seedZmax ) ; 
    m_seedci(0,2) = 0 ; m_seedci(1,2) = 0 ;
    m_seedci(2,2) = 4.0/m_seedZmax/m_seedZmax;
  }
  if ( m_seedRho  < pnt.Rho() ) 
  { 
    Warning ("_seed(): Seed is outside of 'Rho'  fiducial volume " , sc , 1 ).ignore() ; 
    m_seed[0]     = 0.0 ; m_seed[1] = 0.0 ; 
    m_seedci(0,1) = 0   ; m_seedci(0,2) = 0 ; 
    m_seedci(1,2) = 0   ; m_seedci(0,2) = 0 ;
    m_seedci(0,0) = 4.0/m_seedRho/m_seedRho ;
    m_seedci(1,1) = m_seedci(0,0);
  }  
  return StatusCode::SUCCESS ;
}  
// ============================================================================
// The vertex fitting method without creation of a Particle
// ============================================================================
StatusCode LoKi::VertexFitter::fit 
( LHCb::Vertex&                      vertex    ,
  const LHCb::Particle::ConstVector& daughters ) const 
{
  // load the data 
  StatusCode sc = _load ( daughters ) ;
  if ( sc.isFailure() ) 
  { return Error ( "fit(): failure from _load() ", sc , 1 ) ; }      // RETURN 
  sc = _seed ( &vertex ) ; 
  if ( sc.isFailure() ) 
  { Warning  ( "fit(): failure from _seed()"     , sc , 1 ).ignore() ; }
  // make "m_nIterMax" iterations 
  sc = _iterate ( m_nIterMaxI , m_seed ) ;
  if ( sc.isFailure() ) 
  { return Error ( "fit(): failure from _iterate()"  , sc , 10 ) ; } // RETURN 
  
  // get the data from filter 
  const Entry&               entry = m_entries.back() ;
  const Gaudi::Vector3&      x     = entry.m_x         ;
  const Gaudi::SymMatrix3x3& c     = entry.m_c         ;
  const double               chi2  = entry.m_chi2      ;
  // update the vertex parameters:
  vertex.setPosition  ( Gaudi::XYZPoint ( x(0) , x(1) , x(2) ) ) ;
  vertex.setCovMatrix ( c    ) ;
  vertex.setChi2      ( chi2 ) ;
  vertex.setNDoF      ( 2 * m_entries.size() - 3 ) ;
  // fill the vertex 
  vertex.clearOutgoingParticles() ;
  for ( LHCb::Particle::ConstVector::const_iterator ip = daughters.begin() ; 
        daughters.end() != ip ; ++ip ) 
  { vertex.addToOutgoingParticles( *ip ) ; } ;
  // keep for future tracing
  m_vertex = &vertex ;
  if ( m_seedZmin > vertex.position().Z() ) 
  { Warning ( "fit(): Vertex is outside of 'Zmin' fiducial volume " ) ; }
  if ( m_seedZmax < vertex.position().Z() ) 
  { Warning ( "fit(): Vertex is outside of 'Zmax' fiducial volume " ) ; }
  if ( m_seedRho  < vertex.position().Rho() ) 
  { Warning ( "fit(): Vertex is outside of 'Rho'  fiducial volume " ) ; }
  //
  return sc ;
} 
// ============================================================================
// The vertex fitting method with the creation of a Particle
// ============================================================================
StatusCode LoKi::VertexFitter::fit 
( const LHCb::Particle::ConstVector& daughters ,
  LHCb::Vertex&                      vertex    ,
  LHCb::Particle&                    particle  ) const 
{
  using namespace ROOT::Math ;
  //
  // play a bit with extra-info
  if ( particle.hasInfo ( LHCb::Particle::Chi2OfVertexConstrainedFit ) ) 
  { particle.eraseInfo ( LHCb::Particle::Chi2OfVertexConstrainedFit )  ; }
  
  // make a vertex fit 
  StatusCode sc = fit ( vertex , daughters ) ;
  if ( sc.isFailure() ) { return Error ( "fit(): failure from fit", sc , 1 ) ; }
  // links:
  
  particle.clearDaughters() ;
  for ( LHCb::Particle::ConstVector::const_iterator dau = 
          daughters.begin() ; daughters.end() != dau ; ++dau ) 
  { particle.addToDaughters( *dau ) ; } ;
  // 
  particle.setEndVertex( &vertex ) ;  
  //
  // fill the particle with the proper information 
  // 1) reference point and proper covariance matrices
  particle.setReferencePoint ( vertex.position  () ) ;
  particle.setPosCovMatrix   ( vertex.covMatrix () ) ;
  // 2) momentum, its covariance matrix and covariance with position
  // reset existing matrices
  Gaudi::Math::setToScalar ( m_cmom  , 0.0 ) ;
  Gaudi::Math::setToScalar ( m_cmom1 , 0.0 ) ;
  Gaudi::Math::setToScalar ( m_mpcov , 0.0 ) ;
  //
  Gaudi::LorentzVector vct   ;
  const Gaudi::SymMatrix3x3& pos = particle.posCovMatrix() ;
  for ( EIT i = m_entries.begin() ; m_entries.end() != i ; ++i ) 
  {
    Gaudi::Math::add ( vct , i->m_parq ) ;
    m_cmom  += i->m_d ;
    m_mpcov += i->m_e ;
    for ( EIT j = i + 1 ; m_entries.end() != j ; ++j ) 
    {	
      m_cmom1 += i->m_f * pos * Transpose( j->m_f ) ;
      m_cmom1 += j->m_f * pos * Transpose( i->m_f ) ; 
    } 
  }
  Gaudi::Math::add ( m_cmom , m_cmom1 ) ;
  particle.setMomentum        ( vct     ) ;
  particle.setMomCovMatrix    ( m_cmom  ) ;
  particle.setPosMomCovMatrix ( m_mpcov ) ;
  // mass & error in mass 
  const double mass = particle.momentum().M() ;
  if ( 0 > mass ) { Warning( "fit(): mass is negative!") ; }
  Gaudi::Vector4 dmdp (  -vct.X() , -vct.Y() , -vct.Z() , vct.E() ) ;
  dmdp /= mass ;
  const double merr = Similarity ( dmdp , m_cmom ) ;
  if ( 0 > merr ) { Warning( "fit(): mass error is negative " ) ; }  
  particle.setMeasuredMass    ( mass             ) ;
  particle.setMeasuredMassErr ( ::sqrt ( ::fabs ( merr  ) ) ) ;
  //
  // play a bit with extra-info:
  particle.addInfo 
    ( LHCb::Particle::Chi2OfVertexConstrainedFit , vertex.chi2() ) ;
  //
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// add the particle to the vertex and refit 
// ============================================================================
StatusCode LoKi::VertexFitter::add
( const LHCb::Particle*  particle , 
  LHCb::Vertex&          vertex   ) const
{
  if ( 0 == particle ) 
  { return Error ( "add: Particle* point to NULL!" , InvalidParticle , 1 ) ;}
  //
  if ( &vertex != m_vertex ) 
  {
    // first need to fit it! 
    const IVertexFit* vFit = this ;
    StatusCode sc = 
      vFit->fit ( vertex                              , 
                  vertex.outgoingParticles().begin () , 
                  vertex.outgoingParticles().end   () ) ;
    if ( sc.isFailure() ) 
    { return Error ( "add: error from 'fit'", sc , 1 ) ; }
  }
  StatusCode sc = _add ( particle , vertex.position().Z() ) ;
  if ( sc.isFailure() ) { Warning ("add(): failure from _add" , sc , 1 ).ignore() ; }
  //
  Entry& entry    =   m_entries.back()   ;
  const Entry& e0 = *(m_entries.end()-2) ;
  // make one Kalman step using the previos parameters as estimate
  sc = LoKi::KalmanFilter::step  ( entry , e0.m_x , e0.m_ci , e0.m_chi2) ;
  if ( sc.isFailure() ) { Warning ("add(): failure from _step" , sc , 1 ).ignore() ; }
  // smooth it!
  sc = LoKi::KalmanFilter::smooth ( m_entries ) ;
  if ( sc.isFailure() ) { Warning ("add(): failure from _smooth" , sc , 1 ).ignore() ; }
  // make few more full iterations 
  m_seedci = entry.m_ci ;
  Gaudi::Math::scale ( m_seedci , s_scale2 ) ;
  sc = _iterate ( m_nIterMaxII , entry.m_x ) ;
  if ( sc.isFailure() ) { Warning ("add(): failure from _iterate" , sc , 1 ).ignore() ; }
  //
  const Gaudi::Vector3&      x     = entry.m_x         ;
  const Gaudi::SymMatrix3x3& c     = entry.m_c         ;
  const double               chi2  = entry.m_chi2      ;
  // update the vertex parameters:
  vertex.setPosition  ( Gaudi::XYZPoint ( x(0) , x(1) , x(2) ) ) ; 
  vertex.setCovMatrix ( c    ) ;
  vertex.setChi2      ( chi2 ) ;
  // fill the vertex 
  vertex.addToOutgoingParticles( particle ) ;
  // keep for future tracing
  m_vertex = &vertex ;
  // check for positions
  if ( m_seedZmin > vertex.position().Z() ) 
  { Warning ( "fit():Vertex is outside of 'Zmin' fiducial volume " , sc , 1 ).ignore() ; }
  if ( m_seedZmax < vertex.position().Z() ) 
  { Warning ( "fit():Vertex is outside of 'Zmax' fiducial volume " , sc , 1 ).ignore() ; }
  if ( m_seedRho  < vertex.position().Rho() ) 
  { Warning ( "fit():Vertex is outside of 'Rho'  fiducial volume " , sc , 1 ).ignore() ; }
  //
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// remove the particle from  the vertex and refit 
// ============================================================================
StatusCode LoKi::VertexFitter::remove
( const LHCb::Particle*  particle , 
  LHCb::Vertex&          vertex   ) const
{
  return Warning ( "remove(): not implemented (yet)!" , NotImplemented , 1 ) ; // ATTENTION!!!
  //
  const StatusCode OK = StatusCode::SUCCESS ;
  if ( 0 == particle ) 
  { return Warning ( "remove: nothing to remove" , OK , 1 ) ; }        // RETURN 
  if ( &vertex != m_vertex ) 
  {
    // first need to fit it !
    const IVertexFit* vFit = this ;
    StatusCode sc = 
      vFit->fit ( vertex                             , 
                  vertex.outgoingParticles().begin() , 
                  vertex.outgoingParticles().end  () ) ;
    if ( sc.isFailure() ) 
    { return Error ( "remove: error from 'fit'", sc , 1 ) ;  }           // RETURN 
  }
  // remove the particle from the vertex 
  vertex.removeFromOutgoingParticles ( particle ) ;
  // find the corresponding entry
  
  //   EIT ifind = m_entries.end() ;
  //   for ( EIT entry = m_entries.begin() ; m_entries.end() != entry ; ++entry ) 
  //   { if ( particle == entry->m_p0 ) { ifind = entry ; break ; } }
  //   if ( m_entries.end() == ifind ) 
  //   { return Warning ("remove: particle is not in the fit", OK ) ; } // RETURN
  
  //   const Entry& back = m_entries.back() ;
  //   // FIX ME!!!
  //   const Gaudi::SymMatrix3x3 nci= ( back.m_ci  /* + ... FIX ME !!!! */ ) ;
  //   int iFail = 0 ;
  //   const Gaudi::SymMatrix3x3 nc = nci.Sinverse( iFail ) ;
  //   if ( 0 != iFail ) { iFail  = 0 ; Error ("remove: invaid matrix inversion") ;}
  //   // FIX ME!!!
  //   const Gaudi::Vector3 x     = nc*back.m_ci*back.m_x /** - ... FIX ME */ ;
  //   // FIX ME !!!
  //   const double         nchi2 = back.m_chi2 /** - ... FIX ME!!! */  ;
  
  //   // remove the entry 
  //   m_entries.erase ( ifind ) ;
  
  //   // make few more full iterations 
  //   StatusCode sc  = _iterate ( m_nIterMaxIII , x ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
// standard constructor 
// ============================================================================
LoKi::VertexFitter::VertexFitter 
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : base_class ( type , name , parent )
  /// maximal number of iteration for vertex fit  
  , m_nIterMaxI          ( 10 ) // maximal number of iteration for vertex fit  
  /// maximal number of iteration for "add" 
  , m_nIterMaxII         (  5 ) // maximal number of iteration for "add" 
  /// maximal number of iteration for "remove"    
  , m_nIterMaxIII        (  5 ) // maximal number of iteration for "remove"    
  , m_DistanceMax        ( 1.0 * Gaudi::Units::micrometer ) 
  , m_DistanceChi2       ( 1.0 * Gaudi::Units::perCent    ) 
  , m_transporterName    ( "ParticleTransporter:PUBLIC")  
  , m_transporter        ( 0 )
  , m_seedZmin           ( -1.5 * Gaudi::Units::meter      ) 
  , m_seedZmax           (  3.0 * Gaudi::Units::meter      ) 
  , m_seedRho            ( 50.0 * Gaudi::Units::centimeter )
  /// Use the sepcial branch for two-body decays 
  , m_use_twobody_branch  ( false   ) // Use the sepcial branch for two-body decays?
  /// The transport tolerance  
  , m_transport_tolerance (  10 * Gaudi::Units::micrometer ) 
  //
  , m_ppSvc              ( 0       ) 
  , m_longLived          (         ) 
  , m_shortLived         (         ) 
  , m_gammaLike          ( "gamma" )
    //
{
  // ==========================================================================
  declareProperty 
    ( "MaxIterations"      , 
      m_nIterMaxI          , 
      "Maximal number of iterations"                     ) ;
  declareProperty
    ( "MaxIterForAdd"      , 
      m_nIterMaxII         , 
      "Maximal number of iterations for 'Add'-method"    ) ;
  declareProperty 
    ( "MaxIterForRemove"   , 
      m_nIterMaxIII        , 
      "Maximal number of iterations for 'Remove'-method" ) ;
  declareProperty 
    ( "DeltaDistance"      , 
      m_DistanceMax        , 
      "Delta-distance as convergency criterion"    ) ;
  declareProperty 
    ( "DeltaChi2"          ,  
      m_DistanceChi2       , 
      "Delta-chi2     as convergency criterion"    ) ;
  declareProperty 
    ( "UseTwoBodyBranch"   , 
      m_use_twobody_branch , 
      "Use the special branch for two-body decays" ) ;
  declareProperty 
    ( "TransportTolerance"  , 
      m_transport_tolerance , 
      "The tolerance for particle transport" ) ;
  // ==========================================================================
  declareProperty ( "SeedZmin"         , m_seedZmin    ) ;
  declareProperty ( "SeedZmax"         , m_seedZmax    ) ;
  declareProperty ( "SeedRho"          , m_seedRho     ) ;
  declareProperty ( "Transporter"      , m_transporterName );
} 
// ============================================================================
// desctructor 
// ============================================================================
LoKi::VertexFitter::~VertexFitter(){}
// ============================================================================
// the standard initialization of the tool 
// ============================================================================
StatusCode LoKi::VertexFitter::initialize() 
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  svc<IService>( "LoKiSvc" , true ) ;
  
  // get particle property service 
  m_ppSvc = svc<LHCb::IParticlePropertySvc> ( "LHCb::ParticlePropertySvc" , true );
  // validate  
  sc = m_longLived.validate ( m_ppSvc ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Unable to validate Long-Lived particles"  , sc , 1 ) ; }
  sc = m_shortLived.validate ( m_ppSvc ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Unable to validate Short-Lived particles" , sc , 1 ) ; }
  sc = m_gammaLike.validate ( m_ppSvc ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Unable to validate Short-Lived particles" , sc , 1 ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ========================================================================
// the standard finalization of the tool 
// ========================================================================
StatusCode LoKi::VertexFitter::finalize() 
{
  // reset particle property service & functors 
  if ( msgLevel ( MSG::DEBUG ) ) 
  {
    MsgStream& log = debug () ;
    if ( !m_shortLived.accepted().empty() ) 
    {
      log << "Short-Lived particles : " << std::endl ;
      LHCb::ParticleProperties::printAsTable ( m_shortLived.accepted () , log , m_ppSvc ) ;
      log << endmsg ;
    }
    if ( !m_longLived.accepted().empty() ) 
    {
      log << "Long-Lived  particles : " << std::endl ;
      LHCb::ParticleProperties::printAsTable ( m_longLived .accepted () , log , m_ppSvc ) ;
      log << endmsg ;
    }
    log << "Gamma-like  particles : " << std::endl ;
    log << m_gammaLike ;
    log << endmsg ;
  }
  if ( !m_unclassified.empty() ) 
  {
    MsgStream& log = warning() ;
    std::vector<LHCb::ParticleID> tmp ( m_unclassified.begin() , 
                                        m_unclassified.end  () ) ;
    log << "Unclassified particles : " << std::endl ;
    LHCb::ParticleProperties::printAsTable ( tmp , log , m_ppSvc ) ;
    log << endmsg ;      
  }
  // 
  m_ppSvc = 0 ;
  m_shortLived . setService ( m_ppSvc ) ;
  m_longLived  . setService ( m_ppSvc ) ;
  //
  return GaudiTool::finalize () ;
}
// ============================================================================
// get the particle type 
// ============================================================================
LoKi::KalmanFilter::ParticleType 
LoKi::VertexFitter::particleType ( const LHCb::Particle* p ) const 
{
  //
  if      ( 0 == p ) 
  { return LoKi::KalmanFilter::UnspecifiedParticle ; }  // RETURN 
  else if ( m_gammaLike  ( p->particleID () ) )
  { return LoKi::KalmanFilter::GammaLikeParticle   ; }  // RETURN 
  else if ( m_longLived  ( p->particleID () ) ) 
  { return LoKi::KalmanFilter::LongLivedParticle   ; }  // RETURN 
  else if ( m_shortLived ( p->particleID () ) )
  { return LoKi::KalmanFilter::ShortLivedParticle  ; }  // RETURN 
  //
  m_unclassified.insert ( p->particleID() ) ;
  //
  return LoKi::KalmanFilter::UnspecifiedParticle ;
}
// ============================================================================
// transport the data to a certain position 
// ============================================================================
StatusCode LoKi::VertexFitter::_transport 
( LoKi::VertexFitter::Entry& entry , const double newZ ) const 
{
  const double refPointZ = entry.m_p.referencePoint().Z() ;
  const double parZ      = entry.m_parx [2]               ;
  //
  // for short-lived particles one needs to transport them into their decay vertex
  if ( LoKi::KalmanFilter::ShortLivedParticle == entry.m_type )
  {
    // get the end-vertex 
    const LHCb::VertexBase* vertex = entry.m_p0->endVertex() ;
    if ( 0 != vertex ) 
    {
      const double vertexZ = vertex->position().Z() ;
      // already properly transported ? 
      if ( std::fabs ( refPointZ - vertexZ ) < m_transport_tolerance && 
           std::fabs ( parZ      - vertexZ ) < m_transport_tolerance ) 
      { return StatusCode::SUCCESS ; }                                // RETURN 
      // else : transport them into own vertex 
      return LoKi::KalmanFilter::transport 
        ( entry , vertexZ , transporter () ) ;                        // RETURN
    }
  }
  // no need for transport?
  if ( std::fabs ( refPointZ - newZ ) < m_transport_tolerance && 
       std::fabs ( parZ      - newZ ) < m_transport_tolerance  ) 
  { return StatusCode::SUCCESS ; }                                    // RETURN 
  // finally: transport it! 
  return LoKi::KalmanFilter::transport ( entry , newZ , transporter() ) ; 
}  
// ============================================================================
/// the factory needed for instantiation
DECLARE_NAMESPACE_TOOL_FACTORY ( LoKi , VertexFitter ) ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

