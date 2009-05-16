// $Id: KalmanFilter.cpp,v 1.2 2009-05-16 12:28:12 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleTransporter.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/MatrixTransforms.h"
// ============================================================================
// Local
// ============================================================================
#include "KalmanFilter.h"
// ============================================================================
/** @file 
 *  Implementation file for functions from namespace LoKi::KalmanFilter
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-03-06
 */
// ========================================================================-===
// the purely internal program 
// ============================================================================
namespace 
{
  // ==========================================================================
  /** "update" the entry and get the valid "gain" matrix
   *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *   @date 2008-03-06
   */
  StatusCode _update ( LoKi::KalmanFilter::Entry& entry ) 
  {
    const Gaudi::SymMatrix3x3& _pmcov = entry.m_p.posCovMatrix() ;
    if ( _pmcov ( 2 , 2 ) < 0.25 * ( _pmcov ( 0 , 0 ) + _pmcov ( 1 , 1 ) ) )
    {
      Gaudi::SymMatrix2x2 cixy ;
      // basic particle? : use some tricks to 
      cixy ( 0 , 0 ) = _pmcov ( 0, 0 ) ;
      cixy ( 0 , 1 ) = _pmcov ( 0, 1 ) ;
      cixy ( 1 , 1 ) = _pmcov ( 1, 1 ) ;
      if ( !cixy.Invert() )  
      { return StatusCode 
          ( LoKi::KalmanFilter::ErrorInMatrixInversion1 , true ) ; }  // RETURN 
      // The most tricky part I
      entry.m_vxi ( 0 , 0 ) = cixy ( 0 , 0 ) ;
      entry.m_vxi ( 0 , 1 ) = cixy ( 0 , 1 ) ;
      entry.m_vxi ( 1 , 1 ) = cixy ( 1 , 1 ) ;
      // The most tricky part II 
      const Gaudi::LorentzVector& mom = entry.m_p.momentum() ;
      const Gaudi::Vector2 slopes ( mom.Px() / mom.Pz() , mom.Py() / mom.Pz() ) ;
      const Gaudi::Vector2 cslope ( cixy * slopes )  ;
      entry.m_vxi ( 0 , 2 ) = -1 * cslope ( 0 ) ;
      entry.m_vxi ( 1 , 2 ) = -1 * cslope ( 1 ) ;
      entry.m_vxi ( 2 , 2 ) = ROOT::Math::Similarity ( slopes , cixy ) ;
      //
    }
    else
    {
      // the regular particle:
      entry.m_vxi = _pmcov ;
      if ( !entry.m_vxi.Invert() )
      { return StatusCode 
          ( LoKi::KalmanFilter::ErrorInMatrixInversion2 , true ) ; }   // RETURN 
    }
    // 
    Gaudi::Math::geo2LA ( entry.m_p.referencePoint () , entry.m_parx ) ;
    Gaudi::Math::geo2LA ( entry.m_p.momentum       () , entry.m_parq ) ;
    //
    return StatusCode::SUCCESS ;
  }
} // end of anonymous namespace
// ============================================================================
// Load the particle into "entry" representation"
// ============================================================================
StatusCode LoKi::KalmanFilter::load 
( const LHCb::Particle&      particle , 
  LoKi::KalmanFilter::Entry& entry    ) 
{ 
  entry.m_p0 = &particle ;
  entry.m_p  =  particle ;
  //
  return _update ( entry ) ;
}
// ============================================================================
// transport the particle and update the entry
// ============================================================================
StatusCode LoKi::KalmanFilter::transport 
( LoKi::KalmanFilter::Entry& entry    , 
  const double               newZ     , 
  IParticleTransporter*      tool     ) 
{
  if ( 0 == tool ) 
  { return StatusCode ( ErrorFromParticleTransporter , true )  ; }
  // 
  StatusCode sc = tool -> transport ( entry.m_p0 , newZ , entry.m_p ) ;
  if ( sc.isFailure() ) { entry.m_p = *entry.m_p0 ; }   
  //
  return _update ( entry ) ;
}
// ============================================================================
// make one step of Kalman filter 
// ============================================================================
StatusCode LoKi::KalmanFilter::step 
( LoKi::KalmanFilter::Entry&  entry , 
  const Gaudi::XYZPoint&      x     , 
  const Gaudi::SymMatrix3x3&  ci    , 
  const double                chi2  ) 
{
  const Gaudi::Vector3 xx ( x.X () , x.Y() , x.Z() ) ;
  return step ( entry , xx , ci , chi2 ) ;
}
// ============================================================================
// make one step of Kalman filter 
// ============================================================================
StatusCode LoKi::KalmanFilter::step 
( LoKi::KalmanFilter::Entry&  entry , 
  const Gaudi::Vector3&       x     , 
  const Gaudi::SymMatrix3x3&  ci    , 
  const double                chi2  ) 
{
  // OK ! 
  /// \f$ C^{-1}_k=C^{-1}_{k-1}+A^TG_kA =  C^{-1}_{k-1}+ V^{-1}_{k} \f$
  entry.m_ci = ci + entry.m_vxi  ; 
  // OK ! 
  int ifail = 0 ;
  /// \f$ C_k = \left( C^{-1}_{k} \right)^{-1}\f$ 
  entry.m_c  = entry.m_ci.Inverse( ifail ) ; 
  if ( 0 != ifail ) 
  { return StatusCode ( ErrorInMatrixInversion3 , true ) ; }
  // OK ! 
  /// \f$\vec{x}_k\f$
  entry.m_x = entry.m_c * 
    ( ci*x + entry.m_vxi * entry.m_parx  ) ; 
  // OK ! 
  const Gaudi::Vector3 dx = entry.m_parx - entry.m_x ;  
  // OK !
  entry.m_q = entry.m_parq - entry.m_p.posMomCovMatrix() * entry.m_vxi * dx ; 
  // OK ! 
  const double dchi2 = 
    ROOT::Math::Similarity ( entry.m_vxi  , dx            ) + 
    ROOT::Math::Similarity ( ci           , entry.m_x - x ) ;
  //
  // update chi2 
  entry.m_chi2 = chi2 + dchi2 ;
  //
  return StatusCode::SUCCESS ;
}
// ========================================================================    
// kalman smoothing  
// ============================================================================
StatusCode LoKi::KalmanFilter::smooth
( LoKi::KalmanFilter::Entries& entries ) 
{
  if ( entries.empty() ) { return StatusCode::FAILURE ; }
  //
  const Entry& last = entries.back() ;
  for ( Entries::iterator entry = entries.begin() ; 
        entries.end() != entry ; ++entry ) 
  {
    /// \f$ \vec{x}^{n}_k = \vec{x}_{n}\f$ 
    entry -> m_x  = last.m_x ;
    const Gaudi::Vector3 dx = entry->m_parx - entry->m_x ;
    /// \f$ \vec{q}^{n}_k = W_kB^T_{k}G_k\left[\vec{p}_k-A_k\vec{x}_{n}\right]\f$ 
    entry -> m_q = entry -> m_parq 
      - entry -> m_p.posMomCovMatrix() * entry -> m_vxi * dx ; 
  }
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// evaluate all covariances properly 
// ============================================================================
StatusCode LoKi::KalmanFilter::evalCov 
( LoKi::KalmanFilter::Entries& entries ) 
{
  if ( entries.empty() ) { return StatusCode::FAILURE ; }
  
  using namespace ROOT::Math ;
  
  const Entry& last = entries.back() ;
  for ( Entries::iterator entry = entries.begin() ; 
        entries.end() != entry ; ++entry ) 
  {
    /// \f$ C^n_k = C_n \f$  
    entry -> m_c = last.m_c ;
    /// \f$ F_k = G_{p}^{-1}G^T{xp} = - V^T_{xp}V^{-1}_x \f$ 
    entry -> m_f = -1.0 * entry->m_p.posMomCovMatrix()*entry->m_vxi   ;
    /// \f$ E_k = - F_k C_n \f$ 
    entry -> m_e = -1.0 * entry->m_f * entry->m_c ;
    /// \f$ D_k = W_k - E^{n}_kF^{T}_{k} = V_p - V^T_{xp}V^{-1}_{x}V_{xp} + F_kC_nF_k^T \f$ 
    entry->m_d = entry->m_p.momCovMatrix() 
      - Similarity ( entry -> m_p.posMomCovMatrix() , entry -> m_vxi ) 
      + Similarity ( entry -> m_f                   , entry -> m_c   ) ;
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
// construct the seed from the data 
// ============================================================================
StatusCode LoKi::KalmanFilter::seed 
( const LoKi::KalmanFilter::Entries& entries ,
  Gaudi::Vector3&                    x       , 
  Gaudi::SymMatrix3x3&               ci      , 
  const double                       scale   ) 
{
  if ( entries.empty() ) { return StatusCode::FAILURE ; }
  /// construct the seed from the data 
  Gaudi::Math::setToScalar ( ci , 0.0 ) ;
  Gaudi::Vector3 seed ;
  for ( Entries::const_iterator it = entries.begin() ; 
        entries.end() != it ; ++it ) 
  {
    ci   += it->m_vxi                ;
    seed += it->m_vxi * it -> m_parx ;
  }
  int ifail =  0  ;
  Gaudi::SymMatrix3x3  c = ci.Inverse ( ifail ) ;
  //
  if ( 0 != ifail ) 
  { return StatusCode ( ErrorInMatrixInversion4 , true ) ; } // RETURN 
  //
  x = c * seed ; 
  // 
  Gaudi::Math::scale ( ci , scale ) ; // scale the gain matrix 
  //
  return StatusCode::SUCCESS ;
}
// ========================================================================      
// construct the seed from the entries 
// ========================================================================      
StatusCode LoKi::KalmanFilter::seed 
( const LoKi::KalmanFilter::Entries& entries ,
  Gaudi::XYZPoint&                   x       , 
  Gaudi::SymMatrix3x3&               ci      , 
  const double                       scale   ) 
{
  Gaudi::Vector3      xx ;
  Gaudi::Math::geo2LA (  x , xx ) ; //  3D-point    -> 3-vector-LA
  StatusCode sc = seed ( entries , xx , ci , scale ) ;
  Gaudi::Math::la2geo ( xx ,  x ) ; //  3-vector-LA -> 3D-point
  return sc ;
}  
// ========================================================================      


// ============================================================================
// The END 
// ============================================================================
