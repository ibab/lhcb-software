// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
#include <memory>
#include <sstream>
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
#include "LoKi/Child.h"
// ============================================================================
// ROOT/Mathlib
// ============================================================================
#include "Math/Functions.h"
// ============================================================================
// KalmanFilter 
// ============================================================================
#include "KalmanFilter/VertexFit.h"
// ============================================================================
// Photon transportation
// ============================================================================
#include "CaloUtils/CaloMomentum.h"
#include "CaloUtils/CaloParticle.h"
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
  const LHCb::Math::Equal_To<double> s_equal ; // equality comparison 
  // ==========================================================================
}
// ============================================================================
// load the data from the daughter particles into the internal structures 
// ============================================================================
StatusCode LoKi::VertexFitter::_load 
( const LHCb::Particle::ConstVector& ds ) const 
{
  StatusCode sc = StatusCode::SUCCESS ;
  m_entries.clear   () ;
  m_entries.reserve ( ds.size() ) ;
  //
  for ( LHCb::Particle::ConstVector::const_iterator c = ds.begin() ;
        ds.end() != c && sc.isSuccess() ; ++c ) { sc = _load ( *c ) ; } 
  //
  if ( sc.isFailure () ) 
  { return _Warning ( "_load(): the error from _load:" , sc          ) ; } // RETURN 
  if ( m_entries.empty() ) 
  { return _Warning ( "_load(): no valid data found"   , InvalidData ) ; } // RETURN 
  //
  if ( !LoKi::KalmanFilter::okForVertex ( m_entries ) &&
       ( !m_use_rho_like_branch || !LoKi::KalmanFilter::rhoPlusLike ( m_entries ) ) )
  { return _Error( "Input set could not be vertexed"  , InvalidData ) ; }
  //
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// load the data into internal representation 
// ============================================================================
StatusCode LoKi::VertexFitter::_load ( const LHCb::Particle* particle  ) const 
{
  //
  if ( 0 == particle ) 
  { return _Warning ( "_load(): invalid particle" , InvalidParticle ) ; } // RETURN 
  //
  const LoKi::KalmanFilter::ParticleType t = particleType_( particle ) ;
  // regular particles 
  if ( LoKi::KalmanFilter::JetLikeParticle != t )
  {
    m_entries.push_back( Entry() ) ;
    StatusCode sc = LoKi::KalmanFilter::load ( *particle , t , m_entries.back() ) ;
    if ( sc.isFailure() )
    {
      std::ostringstream ostr ; 
      ostr << " Failure to load particle ID " << particle->particleID().abspid() 
           << " as particle type "            << t ;
      return _Error( ostr.str() , sc ) ;                   // RETURN
    }
    return sc ;                                            // RETURN 
  }
  // for jets, load all components...
  typedef SmartRefVector<LHCb::Particle> DAUGHTERS ;
  const DAUGHTERS& daughters = particle->daughters() ;
  for ( DAUGHTERS::const_iterator idau = daughters.begin() ; 
        daughters.end() != idau ; ++idau ) 
  {
    StatusCode sc = _load ( *idau ) ;
    if ( sc.isFailure() ) { return sc ; }  // RETURN 
  }
  // 
  return StatusCode::SUCCESS ;
}
// ============================================================================
// add one particle at the end of the queue
// ============================================================================
StatusCode LoKi::VertexFitter::_add 
( const LHCb::Particle*  child , 
  const Gaudi::XYZPoint& point ) const
{
  StatusCode sc = _load ( child ) ;
  if ( sc.isFailure() ) 
  { _Warning ("_add(): the error from _add()      , ignore", sc ) ; }
  sc = _transport ( m_entries.back() , point ) ;
  if ( sc.isFailure() ) 
  { _Warning ("_add(): the error from _transport(), ignore", sc ) ; }
  return StatusCode::SUCCESS ;
}
// ============================================================================
// stopping criteria for iterations 
// ============================================================================
bool LoKi::VertexFitter::stop_iter  
( const Gaudi::Vector3&      dx   , 
  const Gaudi::SymMatrix3x3& ci   , 
  const size_t               iter ) const 
{
  // distance in the absolute position 
  const double d1 = ROOT::Math::Mag        ( dx ) ;
  // distance in the chi2 units 
  double       d2 = ROOT::Math::Similarity ( dx , ci ) ;
  //
  static const StatusCode sc0 = StatusCode ( StatusCode::FAILURE , true ) ;
  //
  if ( d2 < 0 ) 
  { 
    if ( abs ( d2 ) < std::min ( m_DistanceChi2  , 0.01 ) || s_equal ( d2 , 0 ) ) 
    {
      d2 = 0 ; 
      _Warning ( "stop_iter: small negative chi2 detected, adjusted" , sc0 , 0 ) ; 
    }
    else 
    { 
      _Warning ( "stop_iter: large negative chi2 detected, ignore"   , sc0 , 0 ) ;
    }
  }
  //
  // termination conditions:
  //
  //  (1) STOP if the distance is sufficiently small 
  //       - either the absolute distance 
  //       - or chi2 distance (if at least one iteration is performed) 
  //
  const bool ok1 = d1 < m_DistanceMax ;
  const bool ok2 = 1  < iter && 0 <= d2 && d2 < m_DistanceChi2 ;
  if ( ok1 || ok2 ) 
  {
    //    
    // kalman smooth : could it be moved just before evalCov ? 
    // @todo study the possibility to move smoothig out of iteration loop
    StatusCode sc = LoKi::KalmanFilter::smooth ( m_entries ) ;
    if ( sc.isFailure() ) 
    { 
      _Warning ( "stop_iter(): problem with smoother", sc, 0 ) ;
      return false ;   // make more iterations ..
    }
    //
    sc = LoKi::KalmanFilter::evalCov ( m_entries ) ;
    if ( sc.isFailure() ) 
    { 
      _Warning ( "stop_iter(): problems with covariances", sc, 0 ) ; 
      return false ;   // make more iterations ..
    }
    //
    if ( ok1 ) { counter ( "#iterations/1" ) += iter ; }
    if ( ok2 ) { counter ( "#iterations/2" ) += iter ; }
    //
    return true ;
  }
  //
  return false ;
}
// ============================================================================

 
// ============================================================================
// make optimised Kalman filter iterations 
// ============================================================================
StatusCode LoKi::VertexFitter::_iterate_opt ( const size_t nMax ) const 
{
  // timing measurements 
  Chrono chrono ( timing() ? chronoSvc()           : nullptr       , 
                  timing() ? name() + ":optimized" : std::string() ) ;
  //
  Gaudi::Vector3  x0    ;
  for ( size_t iIter = 0 ; iIter <= nMax + 1 ; ++iIter ) 
  {
    //
    StatusCode sc = LoKi::KalmanFilter::steps ( m_entries , 0 ) ;
    if ( sc.isFailure() ) { _Warning ( "_iterate_opt(): problem with steps"  , sc ) ; }    
    // 
    const Gaudi::Vector3&      x  = m_entries[0].m_x ;
    const Gaudi::SymMatrix3x3& ci = m_entries[0].m_ci ;
    //
    if ( sc.isSuccess() && 1 <= iIter && stop_iter ( x - x0 , ci , iIter ) ) 
    {
      counter ( "#iterations/Opt") += ( iIter - 1 ) ; // the first one acts as seeding
      return StatusCode::SUCCESS ; 
    }
    //
    Gaudi::XYZPoint point ; 
    Gaudi::Math::la2geo ( x , point ) ;
    sc = _transport ( point ) ;
    if ( sc.isFailure() ) { _Warning ( "_iterate_opt(): problem with transport", sc ) ; }
    //
    x0 = x ;
  }
  //
  return _Warning( "_iterate_opt: No convergency has been reached", NoConvergency, 0 ) ; // RETURN
}
// ============================================================================
// make few kalman iterations 
// ============================================================================
StatusCode LoKi::VertexFitter::_iterate 
( const size_t               nIterMax , 
  const Gaudi::Vector3&      _x       ) const 
{  
  // timing measurements 
  Chrono chrono ( timing() ? chronoSvc()         : nullptr       , 
                  timing() ? name() + ":iterate" : std::string() ) ;
  //
  // initial position
  const Gaudi::Vector3* x = &_x ;
  // chi2 
  const double  _chi2 = 0 ;
  const double*  chi2 = &_chi2 ;
  // inverse covariance matrix for the position  
  const Gaudi::SymMatrix3x3* ci = &m_seedci ;
  //
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
    // D) use ``rho+-Like'' branch 
    if ( rho_like ) 
    {
      sc = LoKi::KalmanFilter::stepRho ( m_entries , *chi2 ) ;
      if ( sc.isFailure() ) { _Warning ( "Error from  rho+-like Kalman step" , sc ) ; } 
      else 
      {
        // update the parameters 
        const LoKi::KalmanFilter::Entry& last = m_entries.back() ;
        ci   = &last.m_ci   ;
        x    = &last.m_x    ;
        chi2 = &last.m_chi2 ;
      }
    }
    //
    // D) general case (or failure specialization ) 
    //
    if  ( !rho_like || sc.isFailure()  ) 
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
    if ( sc.isSuccess() && stop_iter ( *x - x0 , *ci , iIter ) ) 
    {
      counter("#iterations/Gen") += iIter ;
      return StatusCode::SUCCESS ; 
    }
    //
  } // end of iterations
  //
  return _Warning( "No convergency has been reached", NoConvergency, 0 ) ; // RETURN 
} 
// ============================================================================
// make few kalman iterations 
// ============================================================================
StatusCode LoKi::VertexFitter::_iterate_rho 
( const size_t               nIterMax ) const 
{  
  // timing measurements 
  Chrono chrono ( timing() ? chronoSvc()       : nullptr       , 
                  timing() ? name() + ":rho"   : std::string() ) ;
  //
  // initial position
  const Gaudi::Vector3*      x    = 0 ; 
  const Gaudi::SymMatrix3x3* ci   = 0 ;
  //
  Entries::const_iterator igood = m_entries.begin() ;
  for ( Entries::const_iterator ientry = m_entries.begin() ; m_entries.end() != ientry ; ++ientry ) 
  {
    if ( LoKi::KalmanFilter::LongLivedParticle  == ientry->m_type || 
         LoKi::KalmanFilter::ShortLivedParticle == ientry->m_type ) { igood = ientry ; break ; }
  }
  //
  x = &igood->m_x ;
  // iterate 
  for ( size_t iIter = 1 ; iIter <= nIterMax ; ++iIter ) 
  {
    // make a proper transportation 
    Gaudi::XYZPoint point ;
    Gaudi::Math::la2geo ( *x , point ) ;
    StatusCode sc = _transport ( point ) ;
    if ( sc.isFailure() ) { _Warning ( "_iterate(): problem with transport ", sc ) ; }    
    //
    const Gaudi::Vector3 x0 ( *x ) ;    
    //
    // use ``rho+-Like'' branch 
    sc = LoKi::KalmanFilter::stepRho ( m_entries ,  0  ) ;
    if ( sc.isFailure() ) { _Warning ( "Error from  rho+-like Kalman step" , sc ) ; } 
    else 
    {
      // update the parameters 
      const LoKi::KalmanFilter::Entry& last = m_entries.back() ;
      ci   = &last.m_ci   ;
      x    = &last.m_x    ;
    }
    //
    if ( sc.isSuccess() && 0 != ci && stop_iter ( *x - x0 , *ci , iIter ) ) 
    {
      counter("#iterations/rho") += iIter ;
      return StatusCode::SUCCESS ; 
    }
    //
  } // end of iterations
  //
  return _Warning( "No convergency has been reached", NoConvergency, 0 ) ; // RETURN 
} 
// ============================================================================
// make a seed 
// ============================================================================
StatusCode LoKi::VertexFitter::_seed ( const LHCb::Vertex* vertex ) const
{
  // timing measurements 
  Chrono chrono ( timing() ? chronoSvc()       : nullptr       , 
                  timing() ? name() + ":seed"  : std::string() ) ;
  //
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
  if ( m_use_shortlived_as_seed ) 
  {
    for ( EIT entry = m_entries.begin() ; m_entries.end() != entry ; ++entry )
    {
      if ( LoKi::KalmanFilter::ShortLivedParticle == entry->m_type ) 
      {
        m_seed   = entry->m_parx ;
        m_seedci = entry->m_vxi  ;
        Gaudi::Math::scale ( m_seedci , s_scale2 ) ;
        ++counter("Seed:case(2)") ;
        return StatusCode::SUCCESS ;                            // RETURN        
      }
    }
  }
  //
  StatusCode sc = StatusCode::FAILURE ;
  const bool massage = forMassage ( m_entries ) ;
  //
  // the simple case with no-massage 
  if ( !massage && okForVertex ( m_entries ) )
  {
    sc = LoKi::KalmanFilter::steps ( m_entries , 0 ) ;
    if ( sc.isSuccess() ) 
    {
      m_seed   = m_entries[0].m_x  ;
      m_seedci = m_entries[0].m_ci ;
      Gaudi::Math::scale ( m_seedci , s_scale2 ) ;
      ++counter("Seed:case(3)") ;
      return StatusCode::SUCCESS ;                            // RETURN 
    }
  }
  //
  // specific cases: seeds for 2,3,4-body decays 
  //
  if      ( 2 == m_entries.size() && LoKi::KalmanFilter::twoProngs   ( m_entries ) )  
  { sc = LoKi::KalmanFilter::step ( m_entries[0] , m_entries[1] , 0 ) ; }
  else if ( 3 == m_entries.size() && LoKi::KalmanFilter::threeProngs ( m_entries ) )
  { sc = LoKi::KalmanFilter::step ( m_entries[0] , m_entries[1] , m_entries[2] , 0 ) ; }
  else if ( 4 == m_entries.size() && LoKi::KalmanFilter::fourProngs   ( m_entries ) )
  { sc = LoKi::KalmanFilter::step ( m_entries[0] , m_entries[1] , 
                                    m_entries[2] , m_entries[3] , 0 ) ; }
  //
  if ( sc.isSuccess() ) 
  {
    m_seed   = m_entries[0].m_x  ;
    m_seedci = m_entries[0].m_ci ;
    Gaudi::Math::scale ( m_seedci , s_scale2 ) ;
    ++counter("Seed:case(1)") ;
    return StatusCode::SUCCESS ;                            // RETURN 
  }
  //
  // simple case: make seed ignoring massaged stuff
  const Entries* entries = 0 ;
  //
  Entries e_good ;
  if ( massage ) 
  {
    e_good.reserve ( m_entries.size() ) ;
    // pick up good entries:
    for ( EIT entry = m_entries.begin() ; m_entries.end() != entry ; ++entry ) 
    { if ( entry->regular() && !forMassage ( entry->m_p0 ) ) 
      { e_good.push_back ( *entry ) ; } }
    //
    entries = &e_good ;
  }
  else { entries = &m_entries ; }
  //
  // relatively obvious case:
  //
  if ( massage && LoKi::KalmanFilter::okForVertex ( *entries ) ) 
  {
    // estimate "seed" using only good components 
    sc = LoKi::KalmanFilter::seed ( *entries , m_seed , m_seedci , s_scale2 ) ;  
    if ( sc.isSuccess() && seedOK ( m_seed ) ) 
    { 
      sc = _transport ( m_seed ) ; 
      if ( sc.isSuccess() ) 
      {
        // adjust seed using all entries 
        sc = LoKi::KalmanFilter::seed ( m_entries , m_seed , m_seedci , s_scale2 ) ;  
        if ( sc.isSuccess () && seedOK ( m_seed ) ) 
        { 
          ++counter("Seed:case(4)") ;
          return StatusCode::SUCCESS ; 
        }  
      }
    } 
  }
  //
  // less trivial case: perform some "massage" action 
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
            ++counter("Seed:case(5)") ;
            return StatusCode::SUCCESS ;
          }
        }   
      }
    } 
  } 
  //
  // the basic case 
  //
  sc = LoKi::KalmanFilter::seed ( m_entries , m_seed , m_seedci , s_scale2 ) ;  
  if ( sc.isSuccess() && seedOK ( m_seed ) ) 
  {
    ++counter("Seed:case(6)") ;
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
  ++counter("Seed:case(7)") ;
  //
  return _Warning( "No proper seed is found", StatusCode::SUCCESS, 0  ) ;
}
// ============================================================================
// The vertex fitting method without creation of a Particle
// ============================================================================
StatusCode LoKi::VertexFitter::fit 
( LHCb::Vertex&                      vertex    ,
  const LHCb::Particle::ConstVector& daughters ) const 
{
  // timing measurements 
  Chrono chrono ( timing() ? chronoSvc()     : nullptr       , 
                  timing() ? name() + ":fit" : std::string() ) ;
  //
  // load the data 
  StatusCode sc = _load ( daughters ) ;
  if ( sc.isFailure() ) 
  { return _Error ( "fit(): failure from _load() ", sc, 0 ) ; }      // RETURN 
  //
  bool special = false ;
  // special case:  use optimized branch when possible 
  if      ( m_use_optimized && okForVertex ( m_entries ) && !forMassage ( m_entries ) ) 
  { sc = _iterate_opt  ( m_nIterMaxI ) ; special = true ; }
  // special case: rho+-like branch 
  else if ( m_use_rho_like_branch && LoKi::KalmanFilter::rhoPlusLike ( m_entries ) ) 
  {  sc = _iterate_rho ( m_nIterMaxI ) ; special = true ; }
  //
  // regular case 
  if ( !special || sc.isFailure() )
  {
    /// make a seed 
    sc = _seed ( &vertex ) ; 
    if ( sc.isFailure() ) 
    { _Warning  ( "fit(): failure from _seed()"     , sc, 0 ) ; }
    // make "m_nIterMax" iterations 
    sc = _iterate ( m_nIterMaxI , m_seed ) ;
    if ( sc.isFailure() ) 
    { return _Warning ( "fit(): failure from _iterate()", sc, 0 ) ; } // RETURN 
  }
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
      _Warning ( "fit(): small negative chi2 detected, adjusted" , sc, 0 ) ; 
    }
    else 
    { 
      _Warning ( "fit(): large negative chi2 detected, ignore"   , sc, 0 ) ; 
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
    if ( ientry->regular() ) { vertex.addToOutgoingParticles( ientry -> m_p0 ) ; }  
  }
  // keep for future tracing
  m_vertex = &vertex ;
  if ( !seedOK ( vertex.position() ) ) 
  { _Warning( "fit(): Vertex is outside of fiducial volume", 
              StatusCode::SUCCESS, 0 ) ; }
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
  //
  using namespace ROOT::Math ;
  //
  // play a bit with extra-info
  if ( particle.hasInfo ( LHCb::Particle::Chi2OfVertexConstrainedFit ) ) 
  {  particle.eraseInfo ( LHCb::Particle::Chi2OfVertexConstrainedFit ) ; }
  
  // make a vertex fit 
  StatusCode sc = fit ( vertex , daughters ) ;
  if ( sc.isFailure() ) { return _Warning ( "fit(): failure from fit", sc, 0 ) ; }
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
      _Warning ( "fit(): measured mass error is a bit    non-positive, adjust to 1 MeV", 
                 StatusCode::SUCCESS, 0 ) ; 
    }
    else if ( mmerr  > -1.0 * Gaudi::Units::MeV ) 
    {
      mmerr = Gaudi::Units::MeV ;
      _Warning ( "fit(): measured mass error is slightly non-positive, adjust to 1 MeV",
                 StatusCode::SUCCESS, 0 ) ; 
    }
    else
    {
      counter ("bad mass error") += mmerr ;
      mmerr = Gaudi::Units::MeV ;
      _Warning ( "fit(): measured mass error is non-positive, adjust to 1 MeV",
                 StatusCode::SUCCESS, 0 ) ; 
    }    
  }  
  //
  double mmass =  mm_v.M() ;
  if ( 0 > mmass ) 
  {
    if      ( mmass  > -0.1 * Gaudi::Units::MeV ||  s_equal ( mmerr , 0 ) ) 
    {
      mmass = Gaudi::Units::MeV ;
      _Warning ( "fit(): measured mass is a bit    non-positive , adjust to 1 MeV",
                 StatusCode::SUCCESS, 0 ) ; 
    }
    else if ( mmass  > -0.5 * mmerr ) 
    {
      mmass = Gaudi::Units::MeV ;
      _Warning ( "fit(): measured mass is slightly non-positive , adjust to 1 MeV",
                 StatusCode::SUCCESS, 0 ) ; 
    }
    else 
    {
      counter ("bad mass ") += mmass ;
      mmass = Gaudi::Units::MeV ;
      _Warning ( "fit(): measured mass is non-positive , adjust to 1 MeV",
                 StatusCode::SUCCESS, 0 ) ; 
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
  switch ( particleType_ ( particle ) ) 
  {
  case LoKi::KalmanFilter::GammaLikeParticle   :
    return _Error ( "add: No way to add      Gamma-like" , InvalidParticle ) ; 
  case LoKi::KalmanFilter::DiGammaLikeParticle : 
    return _Error ( "add: No way to add    DiGamma-like" , InvalidParticle ) ;
  case LoKi::KalmanFilter::MergedPi0LikeParticle : 
    return _Error ( "add: No way to add Merged-Pi0-like" , InvalidParticle ) ;
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
    { return _Warning ( "add: error from 'fit'", sc ) ; }
  }
  StatusCode sc = _add ( particle , vertex.position() ) ;
  if ( sc.isFailure() ) { _Warning ("add(): failure from _add" , sc, 0 ) ; }
  //
  Entry& entry    =   m_entries.back()   ;
  const Entry& e0 = *(m_entries.end()-2) ;
  // make one Kalman step using the previos parameters as estimate
  sc = LoKi::KalmanFilter::step  ( entry , e0.m_x , e0.m_ci , e0.m_chi2) ;
  if ( sc.isFailure() ) { _Warning ("add(): failure from _step" , sc, 0 ) ; }
  // smooth it!
  sc = LoKi::KalmanFilter::smooth ( m_entries ) ;
  if ( sc.isFailure() ) { _Warning ("add(): failure from _smooth", sc, 0 ) ; }
  // make few more full iterations 
  m_seedci = entry.m_ci ;
  Gaudi::Math::scale ( m_seedci , s_scale2 ) ;
  sc = _iterate ( m_nIterMaxII , entry.m_x ) ;
  if ( sc.isFailure() ) { _Warning ("add(): failure from _iterate" , sc, 0 ) ; }
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
  { _Warning ( "fit():Vertex is outside of fiducial volume", sc, 0 ) ; }
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
    { return _Warning ( "remove: error from 'fit'", sc ) ;  }           // RETURN 
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
    /// Allow "rho+"-like particles ?
  , m_use_rho_like_branch     ( true   ) // allow "rho+"-like particles ?
    // Use short-lived particles as seeds 
  , m_use_shortlived_as_seed  ( true   ) 
    /// The transport tolerance  
  , m_transport_tolerance     ( 10 * Gaudi::Units::micrometer )
    /// Use optimized algorithm ?
  , m_use_optimized           ( true   )  // ATTENTION !!! 
    ///
  , m_massage                 (        ) 
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
       std::string::npos != name.find ( "Gam"        ) || 
       std::string::npos != name.find ( "State"      ) || 
       std::string::npos != name.find ( "Master"     )  ) 
  {
    m_massage.push_back ( 180 * Gaudi::Units::cm ) ;
    m_massage.push_back ( 120 * Gaudi::Units::cm ) ;    
    m_massage.push_back (  60 * Gaudi::Units::cm ) ;    
    m_massage.push_back (  10 * Gaudi::Units::cm ) ;    
    m_transporterName   = "DaVinci::ParticleTransporter:PUBLIC" ;
  }
  //
  if ( std::string::npos != name.find ( "Opt" ) || 
       std::string::npos != name.find ( "OPT" ) ) { m_use_optimized = true ; }
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
    ( "AllowRhoPlusLikeParticle"  , 
      m_use_rho_like_branch       , 
      "Allow ``rho+''-like particles?" ) ;
  declareProperty 
    ( "USeShortLivedParticlesAsSeed"     , 
      m_use_shortlived_as_seed           , 
      "Use shortlived particles as seed" ) ;
  declareProperty 
    ( "TransportTolerance"  , 
      m_transport_tolerance , 
      "The tolerance for particle transport" ) ;
  declareProperty 
    ( "UseOptimizedAlgorithm"   , 
      m_use_optimized           , 
      "Use optimised algorithm" ) ;
  // ========================================================================== 
  declareProperty 
    ( "Massage"          , 
      m_massage          , 
      "Z-coordinates for downstream track massage" ) ;
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
    info () << "Redefine 'TransportTolerance' property to " 
            << m_transport_tolerance << endmsg ;
  }
  //
  if ( m_use_optimized ) 
  { info() << "Option for Optimised Kalman Filter fit is activated " << endmsg ; }
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
  /// special "transport" for rho+-like particles 
  else if ( LoKi::KalmanFilter::RhoPlusLikeParticle == entry.m_type )
  { return _transportRhoPlusLike ( entry , point ) ; }
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
// transport the data to a certain position 
// ============================================================================
StatusCode LoKi::VertexFitter::_transportRhoPlusLike 
( LoKi::VertexFitter::Entry& entry , 
  const Gaudi::XYZPoint&     point ) const 
{
  const double refPointZ = entry.m_p.referencePoint().Z() ;
  const double parZ      = entry.m_parx [2]               ;
  //
  // no need for transport?
  //
  if ( std::fabs ( refPointZ - point.Z () ) < m_transport_tolerance && 
       std::fabs ( parZ      - point.Z () ) < m_transport_tolerance  ) 
  { return StatusCode::SUCCESS ; }                                    // RETURN 
  //
  const LHCb::Particle* lparticle = longFromRhoPlusLike_ ( entry.m_p0 ) ;
  if ( 0 == lparticle ) { return _Error ("Rho+-transport: wrong structure[1]!" ) ; }
  // temporary particle 
  LHCb::Particle temp (*lparticle) ;
  StatusCode sc = transporter() -> transportAndProject
    ( lparticle , point.Z() , temp ) ;
  if ( sc.isFailure() ) { return _Warning ("Error in rho+-transport", sc , 0 ) ; }
  //
  const Gaudi::SymMatrix3x3& _pmcov = temp.posCovMatrix() ;
  Gaudi::SymMatrix2x2 cixy ;
  // basic particle? : use some tricks to 
  cixy ( 0 , 0 ) = _pmcov ( 0, 0 ) ;
  cixy ( 0 , 1 ) = _pmcov ( 0, 1 ) ;
  cixy ( 1 , 1 ) = _pmcov ( 1, 1 ) ;
  // Invert matrix: use Cholesky's decomposition; if it fails, use regular inversion 
  if ( !cixy.InvertChol() && !cixy.Invert () )
  { return Warning ("Rho+-transport: can't inverse matrix!")  ; }  // RETURN
  //
  // The most tricky part I
  entry.m_vxi ( 0 , 0 ) = cixy ( 0 , 0 ) ;
  entry.m_vxi ( 0 , 1 ) = cixy ( 0 , 1 ) ;
  entry.m_vxi ( 1 , 1 ) = cixy ( 1 , 1 ) ;
  //
  // The most tricky part II 
  const Gaudi::LorentzVector& mom = temp.momentum() ;
  const Gaudi::Vector2 slopes ( mom.Px() / mom.Pz() , mom.Py() / mom.Pz() ) ;
  const Gaudi::Vector2 cslope ( cixy * slopes )  ;
  entry.m_vxi ( 0 , 2 ) = - cslope ( 0 ) ;
  entry.m_vxi ( 1 , 2 ) = - cslope ( 1 ) ;
  entry.m_vxi ( 2 , 2 ) = ROOT::Math::Similarity ( slopes , cixy ) ;
  //
  // transported particle:
  //
  LHCb::Particle& tp = entry.m_p ;
  //
  tp.setMomentum        ( temp.momentum         () ) ;
  tp.setReferencePoint  ( temp.referencePoint   () ) ;
  //
  tp.setMomCovMatrix    ( temp.momCovMatrix     () ) ;
  tp.setPosCovMatrix    ( temp.posCovMatrix     () ) ;
  tp.setPosMomCovMatrix ( temp.posMomCovMatrix  () ) ;
  //
  bool found = false ;
  typedef SmartRefVector<LHCb::Particle> DAUGHTERS ;
  const DAUGHTERS& daughters = entry.m_p0->daughters() ;
  //
  for ( DAUGHTERS::const_iterator idau = daughters.begin() ; 
        daughters.end() != idau ; ++idau ) 
  {
    const LHCb::Particle* dau = *idau ;
    if ( dau == lparticle ) { found = true ; continue  ; } // CONTINUE
    //
    StatusCode sc = _transportCalo ( dau , temp , entry.m_p.referencePoint()  ) ;
    if ( sc.isFailure() ) { _Warning("Rho+-transport: can't transport `Calo'!") ; }
    //
    const_cast<Gaudi::LorentzVector&> ( tp.momentum()        ) += temp.momentum        () ;
    const_cast<Gaudi::SymMatrix4x4&>  ( tp.momCovMatrix()    ) += temp.momCovMatrix    () ;
    const_cast<Gaudi::SymMatrix3x3&>  ( tp.posCovMatrix()    ) += temp.posCovMatrix    () ;
    const_cast<Gaudi::Matrix4x3&>     ( tp.posMomCovMatrix() ) += temp.posMomCovMatrix () ;
  } 
  if  (!found) { return Error("Rho+-transport: wrong structure[1]!"); }
  //
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// transport gamma & digmma & merged pi0 particles 
// ============================================================================
StatusCode LoKi::VertexFitter::_transportCalo
( const LHCb::Particle*      gamma       , 
  LHCb::Particle&            transported ,
  const Gaudi::XYZPoint&     point       , 
  const Gaudi::SymMatrix3x3* pointCov2   ) const 
{
  if ( 0 == gamma ) { return StatusCode::FAILURE ; }
  // assign 
  if ( gamma != &transported ) { transported = *gamma ; }  
  // the actual type: 
  const LoKi::KalmanFilter::ParticleType t = particleType_( gamma ) ;
  if ( LoKi::KalmanFilter::MergedPi0LikeParticle == t ) 
  {
    int status = 0 ;
    if ( 0 == pointCov2 ) 
    {
      LHCb::CaloParticle calo ( &transported , point ) ;
      calo.updateParticle  () ;
      status = calo.status () ;
    }
    else 
    {
      LHCb::CaloParticle calo ( &transported , point , *pointCov2 ) ;
      calo.updateParticle  () ;
      status = calo.status () ;
    }
    //
    if( 0 != status ) { _Warning("Calo-transport: can't update merged pi0!" , status ) ; }
    //
    // mass and mass uncertainties
    transported. setMeasuredMass    ( gamma-> measuredMass    () ) ; // corrected mass
    transported. setMeasuredMassErr ( gamma-> measuredMassErr () ) ; 
    //
    return StatusCode::SUCCESS ;  
  }
  //
  // Gamma-like & Digamma-like entry 
  LHCb::CaloMomentum calo ;
  //
  calo.setReferencePoint ( point ) ;
  if ( 0 != pointCov2 ) { calo.setPosCovMatrix ( *pointCov2 ) ; }
  //
  if      ( LoKi::KalmanFilter::GammaLikeParticle == t) 
  {
    const LHCb::ProtoParticle* proto = gamma->proto() ;
    if ( 0 == proto ) { return Error("Calo-transport: wrong protoparticle[1]!") ; }
    //
    calo.addCaloPosition ( proto ) ;
  }
  else if ( LoKi::KalmanFilter::DiGammaLikeParticle == t ) 
  {
    // the first gamma  : 
    const LHCb::Particle*      gamma1 = LoKi::Child::child ( gamma , 1 ) ;
    if ( 0 == gamma1 ) { return _Error("Calo-transport: invalid first gamma"    ) ; }
    const LHCb::ProtoParticle* proto1 = gamma1 -> proto() ;
    if ( 0 == proto1 ) { return _Error("Calo-transport: invalid protoparticle-1"); }
    // the second gamma :
    const LHCb::Particle*      gamma2 = LoKi::Child::child ( gamma , 2 ) ;
    if ( 0 == gamma2 ) { return _Error("Calo-transport: invalid second gamma"   ); }
    const LHCb::ProtoParticle* proto2 = gamma2 -> proto() ;
    if ( 0 == proto1 ) { return _Error("Calo-transport: invalid protoparticle-2"); }
    //
    calo.addCaloPosition ( proto1 ) ;
    calo.addCaloPosition ( proto2 ) ;
    //
  }
  else { return _Error("Calo-transport: illegal case"); }  // RETURN
  //
  const bool ok = calo.evaluate() ;
  if ( !ok ) { return Error("Calo-transport: invalid calo momentum") ; }
  //
  // extract the values:
  transported.setReferencePoint    ( point                     ) ;
  transported.setMomentum          ( calo.momentum          () ) ;
  transported.setMomCovMatrix      ( calo.momCovMatrix      () ) ;
  //
  if ( 0 != pointCov2 ) 
  {
    transported.setPosCovMatrix    ( calo.pointCovMatrix    () ) ;
    transported.setPosMomCovMatrix ( calo.momPointCovMatrix () ) ;
  }
  else 
  {
    Gaudi::Math::setToScalar
      ( const_cast<Gaudi::SymMatrix3x3&>( transported.posCovMatrix    () ) , 0.0 ) ;
    Gaudi::Math::setToScalar
      ( const_cast<Gaudi::Matrix4x3&>   ( transported.posMomCovMatrix () ) , 0.0 ) ;
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// the factory needed for instantiation
DECLARE_NAMESPACE_TOOL_FACTORY ( LoKi , VertexFitter )
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
