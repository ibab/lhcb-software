// $Id: TrackKalmanFilter.cpp 114343 2010-12-03 11:16:31Z ibelyaev $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
// #include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/MatrixTransforms.h"
// ============================================================================
// Event/TrackEvent 
// ============================================================================
#include "Event/State.h"
// ============================================================================
// Local
// ============================================================================
#include "KalmanFilter/VertexFitWithTracks.h"
#include "KalmanFilter/ErrorCodes.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for functions from namespace LoKi::KalmanFilter
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
 *  @author Vanya BELYAEV ibelyaev@cern.ch
 *  @date   2010-12-02
 *  
 *                    $Revision: 114343 $
 *  Last modification $Date: 2010-12-03 12:16:31 +0100 (Fri, 03 Dec 2010) $
 *                 by $Author: ibelyaev $
 * 
 */
// ============================================================================
// make one step of Kalman filter 
// ============================================================================
StatusCode LoKi::KalmanFilter::step 
( LoKi::KalmanFilter::TrEntry4&     entry , 
  const Gaudi::XYZPoint&            x     , 
  const Gaudi::SymMatrix3x3&        ci    , 
  const double                      chi2  ) 
{
  const Gaudi::Vector3 xx ( x.X () , x.Y() , x.Z() ) ;
  return step ( entry , xx , ci , chi2 ) ;
}
// ============================================================================
// make one step of Kalman filter q
// ============================================================================
StatusCode LoKi::KalmanFilter::step 
( LoKi::KalmanFilter::TrEntry4&     entry , 
  const Gaudi::Vector3&             x     , 
  const Gaudi::SymMatrix3x3&        ci    , 
  const double                      chi2  ) 
{
  // regular case: 
  // OK !
  /// \f$ C^{-1}_k=C^{-1}_{k-1}+A^TG_kA =  C^{-1}_{k-1}+ V^{-1}_{k} \f$
  entry.m_ci = ci + entry.m_vxi  ; 
  //
  // OK ! 
  int ifail = 0 ;
  /// \f$ C_k = \left( C^{-1}_{k} \right)^{-1}\f$ 
  entry.m_c  = entry.m_ci.Inverse( ifail ) ; 
  if ( 0 != ifail ) 
  { return StatusCode ( ErrorInMatrixInversion3 , true ) ; }
  // OK ! 
  /// \f$\vec{x}_k\f$
  entry.m_x = entry.m_c * ( ci*x + entry.m_vxi * entry.m_parx  ) ; 
  // OK ! 
  const Gaudi::Vector3 dx = entry.m_parx - entry.m_x ;  
  // OK !
  entry.m_q = entry.m_parq - entry.m_qvsx * entry.m_vxi * dx ; 
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
// ============================================================================
/*  make one step of Kalman filter (similar to seeding)
 *  @param entry1 (update)       measurements to be updated 
 *  @param entry2 (update)       measurements to be updated 
 *  @param chi2  (input)        the initial chi2 
 *  @return status code 
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2008-03-06
 */
// ============================================================================
StatusCode LoKi::KalmanFilter::step 
( LoKi::KalmanFilter::TrEntry4&     entry1   ,
  LoKi::KalmanFilter::TrEntry4&     entry2   , 
  const double                      chi2     ) 
{
  //
  entry1.m_ci = entry1.m_vxi + entry2.m_vxi ; 
  entry2.m_ci = entry1.m_ci ;
  //
  /// \f$ C_k = \left( C^{-1}_{k} \right)^{-1}\f$ 
  int ifail = 0 ;
  entry1.m_c  = entry1.m_ci.Inverse( ifail ) ; 
  if ( 0 != ifail ) { return StatusCode ( ErrorInMatrixInversion3 , true ) ; }
  entry2.m_c  = entry1.m_c ;
  
  /// \f$\vec{x}_k\f$
  entry1.m_x = entry1.m_c * ( entry1.m_vxi * entry1.m_parx + 
                              entry2.m_vxi * entry2.m_parx ) ;
  
  entry2.m_x = entry1.m_x ;
  // OK ! 
  const Gaudi::Vector3 dx1 =   entry1.m_parx - entry1.m_x ;  
  entry1.m_q = entry1.m_parq - entry1.m_qvsx * entry1.m_vxi * dx1 ; 
  // OK !
  const Gaudi::Vector3 dx2 =   entry2.m_parx - entry2.m_x ;  
  entry2.m_q = entry2.m_parq - entry2.m_qvsx * entry2.m_vxi * dx2 ; 
  // OK ! 
  const double dchi2_1 = ROOT::Math::Similarity ( entry1.m_vxi , dx1 ) ;
  //
  // update chi2 
  entry1.m_chi2 = chi2          + dchi2_1 ;
  //
  const double dchi2_2 = ROOT::Math::Similarity ( entry2.m_vxi , dx2 ) ;
  //
  entry2.m_chi2 = entry1.m_chi2 + dchi2_2 ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  make one step of Kalman filter (similar to seeding)
 *  @param entry1 (update)       measurements to be updated 
 *  @param entry2 (update)       measurements to be updated 
 *  @param entry3 (update)       measurements to be updated 
 *  @param chi2   (input)        the initial chi2 
 *  @return status code 
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-11-10
 */
// ============================================================================
StatusCode LoKi::KalmanFilter::step 
( LoKi::KalmanFilter::TrEntry4&  entry1   ,
  LoKi::KalmanFilter::TrEntry4&  entry2   , 
  LoKi::KalmanFilter::TrEntry4&  entry3   , 
  const double                   chi2     ) 
{
  //
  entry1.m_ci  = entry1.m_vxi ;
  entry1.m_ci += entry2.m_vxi ; 
  entry1.m_ci += entry3.m_vxi ; 
  //
  entry2.m_ci = entry1.m_ci ;
  entry3.m_ci = entry1.m_ci ;
  //
  /// \f$ C_k = \left( C^{-1}_{k} \right)^{-1}\f$ 
  int ifail = 0 ;
  entry1.m_c  = entry1.m_ci.Inverse( ifail ) ; 
  if ( 0 != ifail ) { return StatusCode ( ErrorInMatrixInversion3 , true ) ; }
  entry2.m_c  = entry1.m_c ;
  entry3.m_c  = entry1.m_c ;
  
  /// \f$\vec{x}_k\f$
  entry1.m_x = entry1.m_c * ( entry1.m_vxi * entry1.m_parx + 
                              entry2.m_vxi * entry2.m_parx +
                              entry3.m_vxi * entry3.m_parx ) ;
  
  entry2.m_x = entry1.m_x ;
  entry3.m_x = entry1.m_x ;
  // OK ! 
  const Gaudi::Vector3 dx1 =   entry1.m_parx - entry1.m_x ;  
  entry1.m_q = entry1.m_parq - entry1.m_qvsx * entry1.m_vxi * dx1 ; 
  // OK !
  const Gaudi::Vector3 dx2 =   entry2.m_parx - entry2.m_x ;  
  entry2.m_q = entry2.m_parq - entry2.m_qvsx * entry2.m_vxi * dx2 ; 
  // OK ! 
  const Gaudi::Vector3 dx3 = entry3.m_parx - entry3.m_x ;  
  entry3.m_q = entry3.m_parq - entry3.m_qvsx * entry3.m_vxi * dx3 ; 
  
  //
  // update chi2 
  //
  const double dchi2_1 = ROOT::Math::Similarity ( entry1.m_vxi , dx1 ) ;
  entry1.m_chi2 = chi2          + dchi2_1 ;
  //
  const double dchi2_2 = ROOT::Math::Similarity ( entry2.m_vxi , dx2 ) ;
  entry2.m_chi2 = entry1.m_chi2 + dchi2_2 ;
  //
  const double dchi2_3 = ROOT::Math::Similarity ( entry3.m_vxi , dx3 ) ;
  entry3.m_chi2 = entry2.m_chi2 + dchi2_3 ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  make one step of Kalman filter (similar to seeding)
 *  @param entry1 (update)       measurements to be updated 
 *  @param entry2 (update)       measurements to be updated 
 *  @param entry3 (update)       measurements to be updated 
 *  @param entry4 (update)       measurements to be updated 
 *  @param chi2   (input)        the initial chi2 
 *  @return status code 
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-12-02
 */
// ============================================================================
StatusCode LoKi::KalmanFilter::step 
( LoKi::KalmanFilter::TrEntry4&  entry1   ,
  LoKi::KalmanFilter::TrEntry4&  entry2   , 
  LoKi::KalmanFilter::TrEntry4&  entry3   , 
  LoKi::KalmanFilter::TrEntry4&  entry4   , 
  const double                   chi2     ) 
{
  //
  entry1.m_ci  = entry1.m_vxi ;
  entry1.m_ci += entry2.m_vxi ; 
  entry1.m_ci += entry3.m_vxi ; 
  entry1.m_ci += entry4.m_vxi ; 
  //
  entry2.m_ci = entry1.m_ci ;
  entry3.m_ci = entry1.m_ci ;
  entry4.m_ci = entry1.m_ci ;
  //
  /// \f$ C_k = \left( C^{-1}_{k} \right)^{-1}\f$ 
  int ifail = 0 ;
  entry1.m_c  = entry1.m_ci.Inverse( ifail ) ; 
  if ( 0 != ifail ) { return StatusCode ( ErrorInMatrixInversion3 , true ) ; }
  entry2.m_c  = entry1.m_c ;
  entry3.m_c  = entry1.m_c ;
  entry4.m_c  = entry1.m_c ;
  
  /// \f$\vec{x}_k\f$
  entry1.m_x = entry1.m_c * ( entry1.m_vxi * entry1.m_parx + 
                              entry2.m_vxi * entry2.m_parx +
                              entry3.m_vxi * entry3.m_parx +
                              entry4.m_vxi * entry4.m_parx ) ;
  
  entry2.m_x = entry1.m_x ;
  entry3.m_x = entry1.m_x ;
  entry4.m_x = entry1.m_x ;
  // OK ! 
  const Gaudi::Vector3 dx1 =   entry1.m_parx - entry1.m_x ;  
  entry1.m_q = entry1.m_parq - entry1.m_qvsx * entry1.m_vxi * dx1 ; 
  // OK !
  const Gaudi::Vector3 dx2 =   entry2.m_parx - entry2.m_x ;  
  entry2.m_q = entry2.m_parq - entry2.m_qvsx * entry2.m_vxi * dx2 ; 
  // OK ! 
  const Gaudi::Vector3 dx3 =   entry3.m_parx - entry3.m_x ;  
  entry3.m_q = entry3.m_parq - entry3.m_qvsx * entry3.m_vxi * dx3 ; 
  // OK ! 
  const Gaudi::Vector3 dx4 =   entry4.m_parx - entry4.m_x ;  
  entry4.m_q = entry4.m_parq - entry4.m_qvsx * entry4.m_vxi * dx4 ; 
  
  //
  // update chi2 
  //
  const double dchi2_1 = ROOT::Math::Similarity ( entry1.m_vxi , dx1 ) ;
  entry1.m_chi2 = chi2          + dchi2_1 ;
  //
  const double dchi2_2 = ROOT::Math::Similarity ( entry2.m_vxi , dx2 ) ;
  entry2.m_chi2 = entry1.m_chi2 + dchi2_2 ;
  //
  const double dchi2_3 = ROOT::Math::Similarity ( entry3.m_vxi , dx3 ) ;
  entry3.m_chi2 = entry2.m_chi2 + dchi2_3 ;
  //
  const double dchi2_4 = ROOT::Math::Similarity ( entry4.m_vxi , dx4 ) ;
  entry4.m_chi2 = entry3.m_chi2 + dchi2_4 ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// kalman smoothing  
// ============================================================================
StatusCode LoKi::KalmanFilter::smooth
( LoKi::KalmanFilter::TrEntries4& entries ) 
{
  if ( entries.empty() ) { return StatusCode::FAILURE ; }
  //
  const TrEntry4& last = entries.back() ;
  for ( TrEntries4::iterator entry = entries.begin() ; 
        entries.end() != entry ; ++entry ) 
  {
    /// \f$ \vec{x}^{n}_k = \vec{x}_{n}\f$ 
    entry -> m_x  = last.m_x ;
    //
    const Gaudi::Vector3 dx = entry->m_parx - entry->m_x ;
    /// \f$ \vec{q}^{n}_k = W_kB^T_{k}G_k\left[\vec{p}_k-A_k\vec{x}_{n}\right]\f$ 
    entry -> m_q = entry -> m_parq - entry -> m_qvsx * entry -> m_vxi * dx ; 
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// load the data from the state 
// ============================================================================
StatusCode LoKi::KalmanFilter::load
( const LHCb::State* state , LoKi::KalmanFilter::TrEntry4& entry ) 
{
  /// reset the state 
  entry.m_state = 0 ;                                       // reset the state 
  //
  if ( 0 == state ) { return StatusCode::FAILURE ; }        // RETURN 
  //
  const Gaudi::TrackVector&    vct = state -> stateVector () ;
  // Retrieve const  the state covariance matrix 
  // (indexes 0,...,4 for x, y, tx, ty, Q/p)
  const Gaudi::TrackSymMatrix& cov = state -> covariance  () ;
  //
  entry.m_parx[0] = vct ( 0 )  ;     // X 
  entry.m_parx[1] = vct ( 1 )  ;     // Y 
  entry.m_parx[2] = state->z() ;     // Z 
  //
  entry.m_parq[0] = vct ( 2 )  ;     // tx 
  entry.m_parq[1] = vct ( 3 )  ;     // ty 
  //
  // correlations:  ????
  for ( int i = 0 ; i < 2 ; ++i )  
  { 
    for ( int j = 0 ; j < 2 ; ++j ) 
    { 
      /// cov ( q , x ) 
      entry.m_qvsx ( i , j ) = cov ( i , j + 2 ) ; // cov ( q , x ) 
    } 
  }
  //
  // Try to invert non-invertible matrix :-) 
  //
  Gaudi::SymMatrix2x2 cixy ;
  cixy ( 0 , 0 ) = cov ( 0, 0 ) ; // cov(x,x)
  cixy ( 0 , 1 ) = cov ( 0, 1 ) ; // cov(x,y)
  cixy ( 1 , 1 ) = cov ( 1, 1 ) ; // cov(y,y)
  if ( !cixy.Invert() )  
  { return StatusCode 
      ( LoKi::KalmanFilter::ErrorInMatrixInversion1 , true ) ; }  // RETURN 
  //
  // The most tricky part I
  //
  entry.m_vxi ( 0 , 0 ) = cixy ( 0 , 0 ) ;
  entry.m_vxi ( 0 , 1 ) = cixy ( 0 , 1 ) ;
  entry.m_vxi ( 1 , 1 ) = cixy ( 1 , 1 ) ;
  //
  const Gaudi::Vector2 slopes ( vct(2) , vct(3) ) ; // slopes!!! 
  const Gaudi::Vector2 cslope ( cixy * slopes )   ; // reduced slopes!!!
  //
  entry.m_vxi ( 0 , 2 ) = -1 * cslope ( 0 ) ;
  entry.m_vxi ( 1 , 2 ) = -1 * cslope ( 1 ) ;
  entry.m_vxi ( 2 , 2 ) = ROOT::Math::Similarity ( slopes , cixy ) ;
  //
  // 
  entry.m_state = state ;
  //
  return StatusCode::SUCCESS ;
}


// ============================================================================
// The END 
// ============================================================================
