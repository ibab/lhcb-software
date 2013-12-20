// $Id$
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
#include "LHCbMath/LHCbMath.h"
#include "LHCbMath/Kinematics.h"
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/MatrixTransforms.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Trees.h"
// ============================================================================
// ROOT/Mathlib
// ============================================================================
#include "Math/Functions.h"
// ============================================================================
// KalmanFilter 
// ============================================================================
#include "KalmanFilter/VertexFit.h"
// ============================================================================
// Local 
// ============================================================================
#include "VertexFitter.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-05-24
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
 */
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
  /** @var s_equal 
   *  equality comparison 
   *  @see LHCb::Math::Equal_To
   */
  const LHCb::Math::Equal_To<double> s_equal ; // equailty comparison 
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
  { return _Warning ( "_load(): the error from _load:" , sc          ) ; } // RETURN 
  if ( m_entries.empty() ) 
  { return _Warning ( "_load(): no valid data found"   , InvalidData ) ; } // RETURN 
  //
  if ( !LoKi::KalmanFilter::okForVertex ( m_entries ) &&
       ( !m_use_rho_like_branch || 
         !LoKi::KalmanFilter::rhoPlusLike ( m_entries ) ) )
  { return _Error( "Input set could not be verticized"  , InvalidData ) ; }
  //
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// load the data into internal representation 
// ============================================================================
StatusCode LoKi::VertexFitter::_load      
( const LHCb::Particle*     particle , 
  LoKi::VertexFitter::Entry& entry    ) const 
{
  //
  if ( 0 == particle ) 
  { return _Warning ( "_load(): invalid particle" , InvalidParticle ) ; } // RETURN 
  //
  return LoKi::KalmanFilter::load ( *particle , particleType ( particle ) , entry ) ;
}
// ============================================================================
// add one particle at the end of the queue
// ============================================================================
StatusCode LoKi::VertexFitter::_add 
( const LHCb::Particle*  child , 
  const Gaudi::XYZPoint& point ) const
{
  m_entries.push_back( Entry() ) ;
  StatusCode sc = _load      ( child , m_entries.back() ) ;
  if ( sc.isFailure() ) 
  { _Warning ("_add(): the error from _add()      , ignore", sc ) ; }
  sc = _transport ( m_entries.back() , point ) ;
  if ( sc.isFailure() ) 
  { _Warning ("_add(): the error from _transport(), ignore", sc ) ; }
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
  const bool two_prongs   = 
    m_use_twobody_branch    && LoKi::KalmanFilter::twoProngs   ( m_entries ) ;
  const bool three_prongs = 
    m_use_threebody_branch  && LoKi::KalmanFilter::threeProngs ( m_entries ) ;
  const bool four_prongs  = 
    m_use_fourbody_branch   && LoKi::KalmanFilter::fourProngs  ( m_entries ) ;
  const bool rho_like     = 
    m_use_rho_like_branch   && LoKi::KalmanFilter::rhoPlusLike ( m_entries ) ;
  //
  for ( size_t iIter = 1 ; iIter <= nIterMax ; ++iIter ) 
  {    
    // make a proper transportation 
    Gaudi::XYZPoint point ;
    Gaudi::Math::la2geo ( *x , point ) ;
    StatusCode sc = _transport ( point ) ;
    if ( sc.isFailure() ) 
    { _Warning ( "_iterate(): problem with transport ", sc ) ; }    
    // initialize the covariance matrix 
    if ( 0 != iIter ) { m_seedci = (*ci) * s_scale2 ; }
    ci   = &m_seedci ;
    // initialize the chi2 
    chi2 = &_chi2 ;
    const Gaudi::Vector3 x0 ( *x ) ;    
    // start the kalman filter 
    bool special = false ;
    // A) the simplest case: 2 body fit 
    if      ( two_prongs ) 
    {
      sc = LoKi::KalmanFilter::step ( m_entries[0] , 
                                      m_entries[1] , *chi2 ) ;
      if ( sc.isSuccess() ) { special = true ; }
      else { _Warning ( "Error from three-body Kalman step" , sc ) ; }      
    }
    // B) use three-body branch 
    else if ( three_prongs ) 
    {
      sc = LoKi::KalmanFilter::step ( m_entries[0] , 
                                      m_entries[1] ,
                                      m_entries[2] , *chi2 ) ;
      if ( sc.isSuccess() ) { special = true ; }
      else { _Warning ( "Error from three-body Kalman step" , sc ) ; }      
    }
    // C) use four-body branch 
    else if ( four_prongs ) 
    {
      sc = LoKi::KalmanFilter::step ( m_entries[0] , 
                                      m_entries[1] ,
                                      m_entries[2] ,
                                      m_entries[3] , *chi2 ) ;
      if ( sc.isSuccess() ) { special = true ; }
      else { _Warning ( "Error from  four-body Kalman step" , sc ) ; }      
    }
    // D) use ``rho+-Like'' branch 
    else if ( rho_like ) 
    {
      sc = LoKi::KalmanFilter::stepRho ( m_entries , *chi2 ) ;
      if ( sc.isSuccess() ) { special = true ; }
      else { _Warning ( "Error from  rho+-like Kalman step" , sc ) ; }      
    }
    //
    if ( special && sc.isSuccess () ) 
    {
      // update the parameters 
      const LoKi::KalmanFilter::Entry& last = m_entries.back() ;
      ci   = &last.m_ci   ;
      x    = &last.m_x    ;
      chi2 = &last.m_chi2 ;
    }
    //
    // D) general case (or failure of any N-body specialiation) 
    //
    if  ( !special || sc.isFailure()  ) 
    {
      for ( EIT entry = m_entries.begin() ; m_entries.end() != entry ; ++entry ) 
      {
        // make one Kalman step 
        sc = LoKi::KalmanFilter::step ( *entry , *x , *ci , *chi2 ) ;
        // skip on Failure
        if ( sc.isFailure() ) 
        {  
          _Warning ( "Error from Kalman-step, skip" , sc ) ;
          continue ;                                                // CONTINUE 
        }
        // update the parameters 
        ci   = &entry->m_ci   ;
        x    = &entry->m_x    ;
        chi2 = &entry->m_chi2 ;
      }
    }
    //
    // distance in the absolute position 
    const double d1 = ROOT::Math::Mag        ( (*x) - x0 ) ;
    // distance in the chi2 units 
    double d2 = ROOT::Math::Similarity ( (*x) - x0 , *ci ) ;
    if ( d2 < 0 ) 
    { 
      if ( abs ( d2 ) < std::min ( m_DistanceChi2  , 0.01 ) || s_equal ( d2 , 0 ) ) 
      {
        d2 = 0 ; 
        _Warning ( "_iterate: small negative chi2 detected, adjusted" , sc ) ; 
      }
      else 
      { 
        _Warning ( "_iterate: large negative chi2 detected, ignore"   , sc ) ; 
      }
    }
    //
    // termination conditions:
    //
    //  (1) STOP if the distance is sufficiently small 
    //      - either the absolute distance 
    //      - or chi2 distance (if at least one iteration is performed) 
    //
    if ( d1 < m_DistanceMax || ( 1 < iIter && 0 <= d2 && d2 < m_DistanceChi2 ) )
    {
      //    
      // kalman smooth : could it be moved just before evalCov ? 
      // @todo study the possibility to move smoothig out of iteration loop
      sc = LoKi::KalmanFilter::smooth ( m_entries ) ;
      if ( sc.isFailure() ) 
      { 
        _Warning ( "_iterate(): problem with smoother", sc ) ; 
        continue ; // CONTINUE: continue iterations in case smoother fails 
      }
      //
      sc = LoKi::KalmanFilter::evalCov ( m_entries ) ;
      if ( sc.isFailure() ) 
      { _Warning ( "_iterate(): problems with covariances" , sc ) ; }
      // 
      counter ( "#iterations" ) += iIter ;
      //
      return StatusCode::SUCCESS ;                             // RETURN 
    } 
  } // end of iterations
  //
  return _Warning ( "No convergency has been reached" , NoConvergency ) ; // RETURN 
} 
// ============================================================================
// make a seed 
// ============================================================================
StatusCode LoKi::VertexFitter::_seed ( const LHCb::Vertex* vertex ) const
{
  // check if vertex could be used as a seed 
  const Gaudi::XYZPoint&     p = vertex->position  () ;
  const Gaudi::SymMatrix3x3& c = vertex->covMatrix () ;
  
  //
  if ( seedOK ( p ) 
       && 0 < c ( 0 , 0 ) && c ( 0 , 0 ) <   1 * Gaudi::Units::cm2 
       && 0 < c ( 1 , 1 ) && c ( 1 , 1 ) <   1 * Gaudi::Units::cm2 
       && 0 < c ( 2 , 2 ) && c ( 2 , 2 ) < 100 * Gaudi::Units::cm2 
       && s_small2 < Gaudi::Math::min_diagonal ( c ) 
       && s_large2 > Gaudi::Math::max_diagonal ( c )  ) 
  {
    // use the vertex parameters as the proper seed 
    Gaudi::Math::geo2LA  ( p , m_seed ) ;
    int ifail = 0 ;
    m_seedci = c.Inverse ( ifail ) ;
    if ( !ifail ) 
    {
      // properly scale the seed matrix 
      Gaudi::Math::scale ( m_seedci , s_scale2 ) ;
      ++counter("Seed:case(0)") ;
      return StatusCode::SUCCESS ;                            // RETURN 
    } 
  }
  //
  const bool massage = forMassage ( m_entries ) ;
  //
  const Entries* entries = 0 ;
  //
  Entries e_good ;
  if ( massage ) 
  {
    // pick up good entries:
    for ( EIT entry = m_entries.begin() ; m_entries.end() != entry ; ++entry ) 
    { if ( !forMassage ( entry->m_p0 ) ) { e_good.push_back ( *entry ) ; } }
    //
    entries = &e_good ;
  }
  else { entries = &m_entries ; }
  //
  // relatively obvious case:
  //
  if ( LoKi::KalmanFilter::okForVertex ( *entries ) ) 
  {
    // estimate "seed" using only good components 
    StatusCode sc = LoKi::KalmanFilter::seed ( *entries , m_seed , m_seedci , s_scale2 ) ;  
    if ( sc.isSuccess() && seedOK ( m_seed ) ) 
    { 
      sc = _transport ( m_seed ) ; 
      if ( sc.isSuccess() ) 
      {
        // adjust seed using all entries 
        sc = LoKi::KalmanFilter::seed ( m_entries , m_seed , m_seedci , s_scale2 ) ;  
        if ( sc.isSuccess () && seedOK ( m_seed ) ) 
        { 
          ++counter("Seed:case(1)") ;
          return StatusCode::SUCCESS ; 
        }  
      }
    } 
  }
  //
  // less trivial case: perform soem "massage" action 
  //
  if ( massage && !m_massage.empty() ) 
  {
    //
    Gaudi::Vector3      p_seed      ;
    Gaudi::SymMatrix3x3 ci_seed     ;
    unsigned short      n_seed  = 0 ;
    //
    for ( std::vector<double>::const_iterator iz = m_massage.begin() ; 
          m_massage.end() != iz ; ++iz ) 
    {
      //
      if ( *iz > m_seedZmax || *iz < m_seedZmin ) { continue ; }
      // transport all? - think a bit more... 
      StatusCode sc = _transport ( Gaudi::XYZPoint ( 0 , 0 , *iz ) ) ;
      if ( sc.isFailure()      ) { continue ; } 
      sc = LoKi::KalmanFilter::seed ( m_entries , m_seed , m_seedci , s_scale2 ) ;
      if ( sc.isFailure()      ) { continue ; }
      if ( !seedOK ( m_seed )  ) { continue ; }
      //
      ci_seed += m_seedci          ;
      p_seed  += m_seedci * m_seed ;
      ++n_seed ;
      //
    }
    //
    counter("Seed:massage") += n_seed ;
    //
    int ifail =  0  ;
    Gaudi::SymMatrix3x3  c = ci_seed.Inverse ( ifail ) ;
    if ( 0 == ifail ) 
    {
      m_seed = c * p_seed ; 
      if ( seedOK ( m_seed ) ) 
      {
        // transport all ?
        StatusCode sc = _transport ( m_seed ) ;
        if ( sc.isSuccess() ) 
        {
          // make proper seed from all components 
          sc = LoKi::KalmanFilter::seed ( m_entries , m_seed , m_seedci , s_scale2 ) ;
          if ( sc.isSuccess() && seedOK ( m_seed ) ) 
          { 
            ++counter("Seed:case(2)") ;
            return StatusCode::SUCCESS ;
          }
        }   
      }
    } 
  } 
  //
  // the basic case 
  //
  StatusCode sc = LoKi::KalmanFilter::seed ( m_entries , m_seed , m_seedci , s_scale2 ) ;  
  if ( sc.isSuccess() && seedOK ( m_seed ) ) 
  {
    ++counter("Seed:case(3)") ;
    return StatusCode::SUCCESS ; 
  }
  //
  m_seed[0] =   0                    ;
  m_seed[1] =   0                    ;
  m_seed[2] = 100 * Gaudi::Units::mm ;
  //
  Gaudi::Math::setToUnit ( m_seedci , 1.0/s_middle2 ) ;
  m_seedci(2,2) = 1.0/s_large2 ;
  //
  ++counter("Seed:case(4)") ;
  //
  return _Warning("No proper seed is found" , StatusCode::SUCCESS  ) ;
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
  { return _Error ( "fit(): failure from _load() ", sc ) ; }      // RETURN 
  sc = _seed ( &vertex ) ; 
  if ( sc.isFailure() ) 
  { _Warning  ( "fit(): failure from _seed()"     , sc ) ; }
  // make "m_nIterMax" iterations 
  sc = _iterate ( m_nIterMaxI , m_seed ) ;
  if ( sc.isFailure() ) 
  { return _Error ( "fit(): failure from _iterate()"  , sc ) ; } // RETURN 
  
  // get the data from filter 
  const Entry&               entry = m_entries.back() ;
  const Gaudi::Vector3&      x     = entry.m_x         ;
  const Gaudi::SymMatrix3x3& c     = entry.m_c         ;
  // const double               chi2  = entry.m_chi2      ;
  // update the vertex parameters:
  vertex.setPosition  ( Gaudi::XYZPoint ( x(0) , x(1) , x(2) ) ) ;
  vertex.setCovMatrix ( c    ) ;
  //
  double chi2 =  LoKi::KalmanFilter::chi2 ( m_entries ) ;
  //
  if ( chi2 < 0 ) 
  { 
    if ( abs ( chi2 ) < std::min ( m_DistanceChi2 , 0.01 ) || s_equal ( chi2 , 0 ) ) 
    {
      chi2 = 0 ; 
      _Warning ( "fit(): small negative chi2 detected, adjusted" , sc ) ; 
    }
    else 
    { 
      _Warning ( "fit(): large negative chi2 detected, ignore"   , sc ) ; 
    }
  }
  //
  vertex.setChi2      ( chi2 ) ;
  vertex.setNDoF      ( LoKi::KalmanFilter::nDoF ( m_entries ) ) ;
  // fill the vertex 
  vertex.clearOutgoingParticles() ;
  for ( Entries::const_iterator ientry = m_entries.begin() ; 
        m_entries.end() != ientry ; ++ientry ) 
  {
    if ( ientry->regular() )  
    { vertex.addToOutgoingParticles( ientry -> m_p0 ) ; }  
  }
  // keep for future tracing
  m_vertex = &vertex ;
  if ( !seedOK ( vertex.position() ) ) 
  { _Warning ( "fit(): Vertex is outside of fiducial volume " ) ; }
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
  if ( sc.isFailure() ) { return _Error ( "fit(): failure from fit", sc ) ; }
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
  Gaudi::Math::setToScalar ( m_mm_c  , 0.0 ) ;
  //
  // 1. for true kinematics: 
  Gaudi::LorentzVector       vct   ;
  const Gaudi::SymMatrix3x3& pos = particle.posCovMatrix() ;
  // 2. for "measured-mass":
  Gaudi::LorentzVector       mm_v  ;
  
  for ( EIT i = m_entries.begin() ; m_entries.end() != i ; ++i ) 
  {
    //
    Gaudi::Math::add ( vct , i->m_q ) ;
    m_cmom  += i -> m_d ;
    m_mpcov += i -> m_e ;
    //
    // for measured mass: blind sum of 4-momenta for extrapolated daughters 
    //
    mm_v    += i -> m_p.momentum     () ;
    m_mm_c  += i -> m_p.momCovMatrix () ;
    //
    if ( i->special() ) { continue ; } // gamma & digamma
    //
    for ( EIT j = i + 1 ; m_entries.end() != j ; ++j ) 
    {	
      m_cmom1 += i -> m_f * pos * Transpose ( j -> m_f ) ;
      m_cmom1 += j -> m_f * pos * Transpose ( i -> m_f ) ; 
    } 
  }
  Gaudi::Math::add ( m_cmom , m_cmom1 ) ;
  particle.setMomentum        ( vct     ) ;
  particle.setMomCovMatrix    ( m_cmom  ) ;
  particle.setPosMomCovMatrix ( m_mpcov ) ;
  //
  // measured mass & error in measured mass 
  //
  double mmerr = Gaudi::Math::sigmamass ( mm_v , m_mm_c ) ;
  if ( 0 >= mmerr ) 
  {
    if      ( mmerr  > -0.1 * Gaudi::Units::MeV ||  s_equal ( mmerr , 0 ) ) 
    {
      mmerr = Gaudi::Units::MeV ;
      _Warning ( "fit(): measured mass error is a bit    non-positive, adjust to 1 MeV" ) ; 
    }
    else if ( mmerr  > -1.0 * Gaudi::Units::MeV ) 
    {
      mmerr = Gaudi::Units::MeV ;
      _Warning ( "fit(): measured mass error is slightly non-positive, adjust to 1 MeV" ) ; 
    }
    else
    {
      counter ("bad mass error") += mmerr ;
      mmerr = Gaudi::Units::MeV ;
      _Warning ( "fit(): measured mass error is non-positive, adjust to 1 MeV  " ) ; 
    }    
  }  
  //
  double mmass =  mm_v.M() ;
  if ( 0 > mmass ) 
  {
    if      ( mmass  > -0.1 * Gaudi::Units::MeV ||  s_equal ( mmerr , 0 ) ) 
    {
      mmass = Gaudi::Units::MeV ;
      _Warning ( "fit(): measured mass is a bit    non-positive , adjust to 1 MeV" ) ; 
    }
    else if ( mmass  > -0.5 * mmerr ) 
    {
      mmass = Gaudi::Units::MeV ;
      _Warning ( "fit(): measured mass is slightly non-positive , adjust to 1 MeV" ) ; 
    }
    else 
    {
      counter ("bad mass ") += mmass ;
      mmass = Gaudi::Units::MeV ;
      _Warning ( "fit(): measured mass is non-positive , adjust to 1 MeV" ) ; 
    }    
  }
  //
  particle.setMeasuredMass    ( mmass ) ;
  particle.setMeasuredMassErr ( mmerr ) ;
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
  { return _Error ( "add: Particle* point to NULL!" , InvalidParticle ) ; }
  //
  switch ( particleType ( particle ) ) 
  {
  case LoKi::KalmanFilter::GammaLikeParticle   :
    return _Error ( "add: No way to add   Gamma-like" , InvalidParticle ) ; 
  case LoKi::KalmanFilter::DiGammaLikeParticle : 
    return _Error ( "add: No way to add DiGamma-like" , InvalidParticle ) ;
  default: 
    break ;
  }
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
    { return _Error ( "add: error from 'fit'", sc ) ; }
  }
  StatusCode sc = _add ( particle , vertex.position() ) ;
  if ( sc.isFailure() ) { _Warning ("add(): failure from _add" , sc ) ; }
  //
  Entry& entry    =   m_entries.back()   ;
  const Entry& e0 = *(m_entries.end()-2) ;
  // make one Kalman step using the previos parameters as estimate
  sc = LoKi::KalmanFilter::step  ( entry , e0.m_x , e0.m_ci , e0.m_chi2) ;
  if ( sc.isFailure() ) { _Warning ("add(): failure from _step" , sc ) ; }
  // smooth it!
  sc = LoKi::KalmanFilter::smooth ( m_entries ) ;
  if ( sc.isFailure() ) { _Warning ("add(): failure from _smooth" , sc ) ; }
  // make few more full iterations 
  m_seedci = entry.m_ci ;
  Gaudi::Math::scale ( m_seedci , s_scale2 ) ;
  sc = _iterate ( m_nIterMaxII , entry.m_x ) ;
  if ( sc.isFailure() ) { _Warning ("add(): failure from _iterate" , sc ) ; }
  //
  const Gaudi::Vector3&      x     = entry.m_x         ;
  const Gaudi::SymMatrix3x3& c     = entry.m_c         ;
  // const double               chi2  = entry.m_chi2      ;
  // update the vertex parameters:
  vertex.setPosition  ( Gaudi::XYZPoint ( x(0) , x(1) , x(2) ) ) ; 
  vertex.setCovMatrix ( c    ) ;
  vertex.setChi2      ( LoKi::KalmanFilter::chi2 ( m_entries ) ) ;
  vertex.setNDoF      ( LoKi::KalmanFilter::nDoF ( m_entries ) ) ;
  // fill the vertex 
  vertex.addToOutgoingParticles( particle ) ;
  // keep for future tracing
  m_vertex = &vertex ;
  // check for positions
  if ( !seedOK ( vertex.position() ) ) 
  { _Warning ( "fit():Vertex is outside of fiducial volume " , sc ) ; }
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
  return Error ( "remove(): not implemented (yet)!" , NotImplemented ) ; // ATTENTION!!!
  //
  const StatusCode OK = StatusCode::SUCCESS ;
  if ( 0 == particle ) 
  { return _Warning ( "remove: nothing to remove" , OK ) ; }        // RETURN 
  if ( &vertex != m_vertex ) 
  {
    // first need to fit it !
    const IVertexFit* vFit = this ;
    StatusCode sc = 
      vFit->fit ( vertex                             , 
                  vertex.outgoingParticles().begin() , 
                  vertex.outgoingParticles().end  () ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "remove: error from 'fit'", sc ) ;  }           // RETURN 
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
  , m_nIterMaxI            ( 10 ) // maximal number of iteration for vertex fit  
/// maximal number of iteration for "add" 
  , m_nIterMaxII           (  5 ) // maximal number of iteration for "add" 
/// maximal number of iteration for "remove"    
  , m_nIterMaxIII          (  5 ) // maximal number of iteration for "remove"    
  , m_DistanceMax          ( 1.0 * Gaudi::Units::micrometer ) 
  , m_DistanceChi2         ( 1.0 * Gaudi::Units::perCent    ) 
  , m_transporterName      ( "ParticleTransporter:PUBLIC")  
  , m_transporter          ( 0 )
  , m_seedZmin             ( -1.0 * Gaudi::Units::meter      ) 
  , m_seedZmax             (  3.0 * Gaudi::Units::meter      ) 
  , m_seedRhoZmax          ( 80.0 * Gaudi::Units::centimeter )
  , m_seedRhoZmin          ( 20.0 * Gaudi::Units::centimeter )
/// Use the special branch for   two-body decays ?
  , m_use_twobody_branch   ( true   ) // Use the special branch for   two-body decays?
/// Use the special branch for three-body decays ?
  , m_use_threebody_branch ( true   ) // Use the special branch for three-body decays?
/// Use the special branch for  four-body decays ?
  , m_use_fourbody_branch  ( true   ) // Use the special branch for  four-body decays?
/// Allow "rho+"-like particles ?
  , m_use_rho_like_branch  ( true   ) // allow "rho+"-like particles ?
/// The transport tolerance  
  , m_transport_tolerance  ( 10 * Gaudi::Units::micrometer ) 
/// number of prints 
  , m_prints               ( 2     )
  , m_massage              (       ) 
/// pure technical stuff: 
  , m_entries (   ) 
  , m_vertex  ( 0 )
  , m_seed    (   ) 
  , m_seedci  (   )
  , m_cixy    (   ) 
  , m_cmom    (   ) 
  , m_cmom1   (   ) 
  , m_mpcov   (   ) 
  , m_mm_c    (   )
{
  //
  if ( std::string::npos != name.find ( "Massage"    ) || 
       std::string::npos != name.find ( "Downstream" ) || 
       std::string::npos != name.find ( "DD"         ) || 
       std::string::npos != name.find ( "V0"         ) || 
       std::string::npos != name.find ( "KS"         ) || 
       std::string::npos != name.find ( "Ks"         ) || 
       std::string::npos != name.find ( "K0S"        ) || 
       std::string::npos != name.find ( "Lam"        ) || 
       std::string::npos != name.find ( "L0"         ) || 
       std::string::npos != name.find ( "State"      ) || 
       std::string::npos != name.find ( "Master"     )  ) 
  {
    m_massage.push_back ( 180 * Gaudi::Units::cm ) ;
    m_massage.push_back ( 120 * Gaudi::Units::cm ) ;    
    m_massage.push_back (  60 * Gaudi::Units::cm ) ;    
    m_massage.push_back (  10 * Gaudi::Units::cm ) ;    
    m_transporterName   = "DaVinci::ParticleTransporter:PUBLIC" ;
  }
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
    ( "UseTwoBodyBranch"     , 
      m_use_twobody_branch   , 
      "Use the special branch for   two-body decays" ) ;
  declareProperty 
    ( "UseThreeBodyBranch"   , 
      m_use_threebody_branch , 
      "Use the special branch for three-body decays" ) ;
  declareProperty 
    ( "UseFourBodyBranch"    , 
      m_use_fourbody_branch  , 
      "Use the special branch for  four-body decays" ) ;
  declareProperty 
    ( "AllowRhoPlusLikeParticle"  , 
      m_use_rho_like_branch       , 
      "Allow ``rho+''-like particles?" ) ;
  declareProperty 
    ( "TransportTolerance"  , 
      m_transport_tolerance , 
      "The tolerance for particle transport" ) ;
  // ========================================================================== 
  declareProperty 
    ( "MaxPrints"        , 
      m_prints           , 
      "Maximal number of prints "        ) ;
  // ===========================================================================
  declareProperty 
    ( "Massage"          , 
      m_massage          , 
      "Z-coordiates for downstream track massage" ) ;
  // ===========================================================================
  declareProperty 
    ( "SeedZmin"         , 
      m_seedZmin         ,
      "Allowed Z-min   for seed-vertex "       ) ;
  declareProperty 
    ( "SeedZmax"         ,
      m_seedZmax         , 
      "Allowed Z-max   for seed-vertex "       ) ;
  declareProperty 
    ( "SeedRhoZmax"      , 
      m_seedRhoZmax      ,
      "Allowed Rho (at Zmax) for seed-vertex " ) ;
  declareProperty 
    ( "SeedRhoZmin"      , 
      m_seedRhoZmin      ,
      "Allowed Rho (at Zmin) for seed-vertex " ) ;
  declareProperty 
    ( "Transporter"      , 
      m_transporterName  , 
      "The typename of transporter to be used" ) ;
  // ==========================================================================
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
  /// initialize the base class 
  StatusCode sc = LoKi::ParticleClassificator::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  //
  svc<IService>( "LoKiSvc" , true ) ;
  //
  if ( msgLevel ( MSG::DEBUG ) &&  0 == m_prints ) 
  {
    m_prints = 10 ;
    warning () << "Redefine 'MaxPrints' property to " << m_prints << endmsg ;
  }
  //
  if ( m_DistanceChi2 < 0.0005 ) 
  {
    m_DistanceChi2 = 0.0005 ;
    info () << "Redefine 'DeltaChi2' property to " << m_DistanceChi2 << endmsg ;
  }
  //
  if ( m_DistanceMax < 0.001 * Gaudi::Units::micrometer ) 
  {
    m_DistanceMax = 0.001 * Gaudi::Units::micrometer ;
    info () << "Redefine 'DeltaDistance' property to " << m_DistanceMax << endmsg ;
  }
  //
  if ( m_nIterMaxI  > 100 ) 
  {
    m_nIterMaxI = 100 ;
    info () << "Redefine 'MaxIterations' property to " << m_nIterMaxI << endmsg ;
  }
  //
  if ( m_nIterMaxII > 100 ) 
  {
    m_nIterMaxII = 100 ;
    info () << "Redefine 'MaxIterForAdd' property to " << m_nIterMaxII << endmsg ;
  }
  //
  if ( m_transport_tolerance < 0.1 * Gaudi::Units::micrometer ) 
  {
    m_transport_tolerance = 0.1 * Gaudi::Units::micrometer ;
    info () << "Redefine 'TransportTolerance' property to " <<
      m_transport_tolerance << endmsg ;
  }
  //
  if ( !m_massage.empty() && std::string::npos == m_transporterName.find("DaVinci::") ) 
  {
    warning() << "The transporter '" << m_transporterName 
              << "'is not optimal for activated ``massage''-option" 
              << " ``Massage'' is OFF"
              << endmsg ;
    m_massage.clear() ;
  }
  //
  if ( !m_massage.empty() ) { info() << "``Massage''-option is ON" << endmsg ; }
  //
  return StatusCode::SUCCESS ;
}
// ========================================================================
// the standard finalization of the tool 
// ========================================================================
StatusCode LoKi::VertexFitter::finalize()
{ return LoKi::ParticleClassificator::finalize () ; }
// ============================================================================
// transport the data to a certain position 
// ============================================================================
StatusCode LoKi::VertexFitter::_transport 
( LoKi::VertexFitter::Entry& entry , 
  const Gaudi::XYZPoint&     point ) const 
{
  const double refPointZ = entry.m_p.referencePoint().Z() ;
  const double parZ      = entry.m_parx [2]               ;
  //
  // for short-lived particles one needs to transport them into their decay vertex
  //
  if ( LoKi::KalmanFilter::ShortLivedParticle == entry.m_type )
  {
    //
    // get the end-vertex 
    //
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
        ( entry , vertex->position() , transporter () ) ;             // RETURN
    }
  }
  //
  // no need for transport?
  //
  if ( std::fabs ( refPointZ - point.Z () ) < m_transport_tolerance && 
       std::fabs ( parZ      - point.Z () ) < m_transport_tolerance  ) 
  { return StatusCode::SUCCESS ; }                                    // RETURN 
  //
  // finally: transport it! 
  //
  return LoKi::KalmanFilter::transport ( entry , point , transporter () ) ; 
}  
// ============================================================================
/// the factory needed for instantiation
DECLARE_NAMESPACE_TOOL_FACTORY ( LoKi , VertexFitter )
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

