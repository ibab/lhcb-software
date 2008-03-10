// $Id: VertexFitter.cpp,v 1.5 2008-03-10 18:24:43 ibelyaev Exp $
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
  const double s_scale   = Gaudi::Units::perCent           ;
  const double s_scale2  = s_scale  * s_scale              ;
  const double s_small   = 0.1 * Gaudi::Units::micrometer  ;
  const double s_small2  = s_small  * s_small              ;
  const double s_middle  =  1  * Gaudi::Units::centimeter  ;
  const double s_middle2 = s_middle * s_middle             ;
  const double s_large   = 10  * Gaudi::Units::centimeter  ;
  const double s_large2  = s_large  * s_large              ; 
}
// ============================================================================
// load the data from the daughter particles into the internal structures 
// ============================================================================
StatusCode LoKi::VertexFitter::_load 
( const LHCb::Particle::ConstVector& ds ) const 
{
  StatusCode sc = StatusCode::SUCCESS ;
  m_entries.resize ( ds.size() ) ;
  LHCb::Particle::ConstVector::const_iterator c = ds.begin()        ;
  Entries::iterator                           e = m_entries.begin() ;
  
  for ( ; ds.end() != c && sc.isSuccess() ; ++c , ++e ) 
  { sc = _load ( *c , *e ) ; } ;
  if ( sc.isFailure () ) 
  { return Error ( "_load(): the error from _load:" , sc ) ; }        // RETURN 
  if ( m_entries.empty() ) 
  { return Error ( "_load(): no valid data found" , InvalidData ) ; } // RETURN 
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
  { return Error ( "_load(): invalid particle" , InvalidParticle ) ; } // RETURN 
  return LoKi::KalmanFilter::load ( *particle , entry ) ;
}
// ============================================================================/
// add one particle at the end of the queue
// ============================================================================
StatusCode LoKi::VertexFitter::_add 
( const LHCb::Particle*              child     , 
  const double                       newZ      ) const
{
  m_entries.push_back( Entry() ) ;
  StatusCode sc = _load      ( child , m_entries.back() ) ;
  if ( sc.isFailure() ) 
  { Warning ("_add(): the error from _add()      , ignore", sc ) ; }
  sc = _transport ( m_entries.back() , newZ ) ;
  if ( sc.isFailure() ) 
  { Warning ("_add(): the error from _transport(), ignore", sc ) ; }
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
    { Warning ( "_iterate(): problem with transport ", sc ) ;}    
    // initialize the covariance matrix 
    if ( 0 != iIter ) { m_seedci = (*ci) * s_scale2 ; }
    ci   = &m_seedci ;
    // initialize the chi2 
    chi2 = &_chi2 ;
    const Gaudi::Vector3 x0 ( *x ) ;    
    // start the kalman filter 
    for ( EIT entry = m_entries.begin() ; m_entries.end() != entry ; ++entry ) 
    {
      // make one Kalman step 
      sc = LoKi::KalmanFilter::step ( *entry , *x , *ci , *chi2 ) ;
      // skip on Failure
      if ( sc.isFailure() ) { continue ; }                    // CONTINUE 
      // update the parameters 
      ci   = &entry->m_ci   ;
      x    = &entry->m_x    ;
      chi2 = &entry->m_chi2 ;
    }
    // kalman smooth
    sc = LoKi::KalmanFilter::smooth ( m_entries ) ;
    if ( sc.isFailure() ) 
    { Warning ( "_iterate(): problem with smoother", sc ) ; }
    // distance in the absolute position 
    const double d1 = ROOT::Math::Mag        ( (*x) - x0 ) ;
    // distance in the chi2 units 
    const double d2 = ROOT::Math::Similarity ( (*x) - x0 , *ci ) ;
    if ( d2 < 0 ) 
    {  Warning ( "_iterate: negative chi2 detected, ignore" ) ; }
    // termination conditions:
    //
    //  (1) STOP if the distance is sufficiently small 
    //      - either the absolute distance 
    //      - or chi2 distance (if at least one iteration is performed) 
    //
    if ( d1 < m_DistanceMax || ( 1 < iIter && 0 <= d2 && d2 < m_DistanceChi2 ) )
    {
      sc = LoKi::KalmanFilter::evalCov ( m_entries ) ;
      if ( sc.isFailure() ) 
      { Warning ( "_iterate(): problems with covariances" , sc ) ; }
      // 
      counter ( "#iterations" ) += iIter ;
      //
      return StatusCode::SUCCESS ;                             // RETURN 
    }    
  } // end of iterations
  return Error ( "No convergency has been reached" , NoConvergency ) ; // RETURN 
} 
// ============================================================================
// make a seed 
// ============================================================================
StatusCode LoKi::VertexFitter::_seed ( const LHCb::Vertex* vertex ) const
{
  // check if vertex could be used as a seed 
  const Gaudi::XYZPoint& p     = vertex->position  () ;
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
    Warning ( "_seed(): error in matrix inversion" ) ; 
  }
  /// check the validity of the seed 
  Gaudi::XYZPoint pnt ( m_seed[0] , m_seed[1] , m_seed[2] ) ;
  if ( m_seedZmin > pnt.Z() ) 
  { 
    Warning ("_seed(): Seed is outside of 'Zmin' fiducial volume ") ; 
    m_seed[2] = 0.5 * ( 0.0 + m_seedZmin ) ; 
    m_seedci(0,2) = 0 ; m_seedci(1,2) = 0 ;
    m_seedci(2,2) = 4.0/m_seedZmin/m_seedZmin;
  }
  if ( m_seedZmax < pnt.Z() ) 
  { 
    Warning ("_seed(): Seed is outside of 'Zmax' fiducial volume ") ; 
    m_seed[2] = 0.5 * ( 0.0 + m_seedZmax ) ; 
    m_seedci(0,2) = 0 ; m_seedci(1,2) = 0 ;
    m_seedci(2,2) = 4.0/m_seedZmax/m_seedZmax;
  }
  if ( m_seedRho  < pnt.Rho() ) 
  { 
    Warning ("_seed(): Seed is outside of 'Rho'  fiducial volume ") ; 
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
( const LHCb::Particle::ConstVector& daughters ,
  LHCb::Vertex&                      vertex    ) const 
{
  // load the data 
  StatusCode sc = _load ( daughters ) ;
  if ( sc.isFailure() ) 
  { return Error ( "fit(): failure from _load() ", sc ) ; }      // RETURN 
  sc = _seed ( &vertex ) ; 
  if ( sc.isFailure() ) 
  { Warning  ( "fit(): failure from _seed()"     , sc ) ; }
  // make "m_nIterMax" iterations 
  sc = _iterate ( m_nIterMaxI , m_seed ) ;
  if ( sc.isFailure() ) 
  { return Error ( "fit(): failure from _iterate()"  , sc ) ; } // RETURN 
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
  LHCb::Particle&                    particle  ,
  LHCb::Vertex&                      vertex    ) const 
{
  using namespace ROOT::Math ;
  //
  // play a bit with extra-info
  if ( particle.hasInfo ( LHCb::Particle::Chi2OfVertexConstrainedFit ) ) 
  { particle.eraseInfo ( LHCb::Particle::Chi2OfVertexConstrainedFit )  ; }
  
  // make a vertex fit 
  StatusCode sc = fit ( daughters , vertex ) ;
  if ( sc.isFailure() ) { return Error ( "fit(): failure form fit", sc ) ; }
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
  { return Error ( "add: Particle* point to NULL!" , InvalidParticle ) ;}
  //
  if ( &vertex != m_vertex ) 
  {
    // first need to fit it! 
    const IVertexFit* vFit = this ;
    StatusCode sc = 
      vFit->fit ( vertex.outgoingParticles().begin () , 
                  vertex.outgoingParticles().end   () , vertex ) ;
    if ( sc.isFailure() ) 
    { return Error ( "add: error from 'fit'", sc ) ; }
  }
  StatusCode sc = _add ( particle , vertex.position().Z() ) ;
  if ( sc.isFailure() ) { Warning ("add(): failure from _add" , sc ) ; }
  //
  Entry& entry    =   m_entries.back()   ;
  const Entry& e0 = *(m_entries.end()-2) ;
  // make one Kalman step using the previos parameters as estimate
  sc = LoKi::KalmanFilter::step  ( entry , e0.m_x , e0.m_ci , e0.m_chi2) ;
  if ( sc.isFailure() ) { Warning ("add(): failure from _step" , sc ) ; }
  // smooth it!
  sc = LoKi::KalmanFilter::smooth ( m_entries ) ;
  if ( sc.isFailure() ) { Warning ("add(): failure from _smooth" , sc ) ; }
  // make few more full iterations 
  m_seedci = entry.m_ci ;
  Gaudi::Math::scale ( m_seedci , s_scale2 ) ;
  sc = _iterate ( m_nIterMaxII , entry.m_x ) ;
  if ( sc.isFailure() ) { Warning ("add(): failure from _iterate" , sc ) ; }
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
  { Warning ( "fit():Vertex is outside of 'Zmin' fiducial volume " ) ; }
  if ( m_seedZmax < vertex.position().Z() ) 
  { Warning ( "fit():Vertex is outside of 'Zmax' fiducial volume " ) ; }
  if ( m_seedRho  < vertex.position().Rho() ) 
  { Warning ( "fit():Vertex is outside of 'Rho'  fiducial volume " ) ; }
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
  return Warning ( "remove(): not implemented (yet)!" , NotImplemented ) ; // ATTENTION!!!
  //
  const StatusCode OK = StatusCode::SUCCESS ;
  if ( 0 == particle ) 
  { return Warning ( "remove: nothing to remove" , OK ) ; }        // RETURN 
  if ( &vertex != m_vertex ) 
  {
    // first need to fit it !
    const IVertexFit* vFit = this ;
    StatusCode sc = 
      vFit->fit ( vertex.outgoingParticles().begin() , 
                  vertex.outgoingParticles().end  () , vertex ) ;
    if ( sc.isFailure() ) 
    { return Error ( "remove: error from 'fit'", sc ) ;  }           // RETURN 
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
/// the factory needed for instantiation
DECLARE_NAMESPACE_TOOL_FACTORY ( LoKi , VertexFitter ) ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

