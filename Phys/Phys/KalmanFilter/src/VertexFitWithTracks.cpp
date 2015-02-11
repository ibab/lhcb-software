// $Id$ 
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <array>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/LHCbMath.h"
#include "LHCbMath/Power.h"
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/MatrixTransforms.h"
// ============================================================================
// Event/TrackEvent 
// ============================================================================
#include "Event/State.h"
#include "Event/Track.h"
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
 *  By usage of this code one clearly states the disagreement 
 *    with the smear campain by Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2010-12-02
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$ 
 */
// ============================================================================
namespace // local anonymous namespace to keep local functions 
{
  // ==========================================================================
  /// comparison of double numbers 
  const LHCb::Math::Equal_To<double>  s_equal ;
  // ==========================================================================
  /// inverse "large" error in position: used to avoid singularity
  const std::array<double,3> s_ERROR2_i  = { { 
      1.0 / Gaudi::Math::pow ( 10.0 * Gaudi::Units::cm , 2 ) ,
      1.0 / Gaudi::Math::pow ( 10.0 * Gaudi::Units::cm , 2 ) ,
      1.0 / Gaudi::Math::pow ( 90.0 * Gaudi::Units::cm , 2 ) } } ;
  // 
  // ==========================================================================
  /// "smooth" the singular values 
  template <class T, unsigned int D>
  inline 
  void _smooth 
  ( ROOT::Math::SMatrix<T,D,D,ROOT::Math::MatRepSym<T,D> >& mtrx       , 
    const double                                            error = -1 ) 
  {
    //
    const double err2 = error * error ;
    for ( unsigned int i = 0 ; i < D ; ++i ) 
    { 
      mtrx(i,i) += 
        error <= 0.0 ? 
        s_ERROR2_i [ i ]  :
        std::min ( 1.0 / err2 , s_ERROR2_i [ i ] ) ;  
    }
  }
  // ==========================================================================
} //                   end of local anonymous namespace to keep local functions 
// ============================================================================
// make one step of Kalman filter 
// ============================================================================
StatusCode LoKi::KalmanFilter::step 
( LoKi::KalmanFilter::TrEntry4&     entry  , 
  const Gaudi::XYZPoint&            x      , 
  const Gaudi::SymMatrix3x3&        ci     , 
  const double                      chi2   )
{
  const Gaudi::Vector3 xx ( x.X () , x.Y() , x.Z() ) ;
  return step ( entry , xx , ci , chi2 ) ;
}
// ============================================================================
// make one step of Kalman filter q
// ============================================================================
StatusCode LoKi::KalmanFilter::step 
( LoKi::KalmanFilter::TrEntry4&     entry  , 
  const Gaudi::Vector3&             x      , 
  const Gaudi::SymMatrix3x3&        ci     , 
  const double                      chi2   )
{
  // regular case: 
  // OK !
  /// \f$ C^{-1}_k=C^{-1}_{k-1}+A^TG_kA =  C^{-1}_{k-1}+ V^{-1}_{k} \f$
  entry.m_ci = ci + entry.m_weight * entry.m_vxi  ; 
  //
  /// \f$ C_k = \left( C^{-1}_{k} \right)^{-1}\f$ 
  if ( 0 != Gaudi::Math::inverse ( entry.m_ci, entry.m_c ) )  
  { return StatusCode ( ErrorInMatrixInversion3 , true ) ; }
  // OK ! 
  /// \f$\vec{x}_k\f$
  entry.m_x = entry.m_c * ( ci*x + entry.m_weight * entry.m_vxi * entry.m_parx  ) ; 
  // OK ! 
  const Gaudi::Vector3 dx = entry.m_parx - entry.m_x ;  
  // OK !
  entry.m_q = entry.m_parq - entry.m_qvsx * entry.m_vxi * dx * entry.m_weight ; 
  // OK ! 
  const double dchi2 = 
    ROOT::Math::Similarity ( entry.m_vxi  , dx            ) * entry.m_weight + 
    ROOT::Math::Similarity ( ci           , entry.m_x - x ) ;
  //
  // update chi2 
  entry.m_chi2   = chi2 + dchi2  ;    // update chi2
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  construct the seed from the entries 
 *  @param entries (input)  the list of entries 
 *  @param x       (output) the position of "seed"
 *  @param ci      (output) the gain matrix for the seed 
 *  @param scale   (input)  the scale factor for gain matrix
 *  @return status code 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2014-03-15
 */
// ============================================================================
StatusCode LoKi::KalmanFilter::seed 
( LoKi::KalmanFilter::TrEntries4&   entries  , 
  Gaudi::XYZPoint&                  x        , 
  Gaudi::SymMatrix3x3&              ci       , 
  const double                      scale    ) 
{
  //
  Gaudi::Vector3 v ;
  Gaudi::Math::geo2LA  ( x , v ) ; //  3D-point    -> 3-vector-LA
  StatusCode sc = seed ( entries , v , ci , scale ) ;
  Gaudi::Math::la2geo  ( v , x ) ; //  3-vector-LA -> 3D-point
  //
  return sc ;
}
// ============================================================================
/*  construct the seed from the entries 
 *  @param entries (input)  the list of entries 
 *  @param x       (output) the position of "seed"
 *  @param ci      (output) the gain matrix for the seed 
 *  @param scale   (input)  the scale factor for gain matrix
 *  @return status code 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2014-03-15
 */
// ============================================================================
StatusCode LoKi::KalmanFilter::seed 
( LoKi::KalmanFilter::TrEntries4&   entries  , 
  Gaudi::Vector3&                   x        , 
  Gaudi::SymMatrix3x3&              ci       , 
  const double                      scale    ) 
{
  //
  if ( entries.empty() ) { return StatusCode::FAILURE ; }
  //
  /// construct the seed from the data 
  Gaudi::Math::setToScalar ( ci , 0.0 ) ;
  Gaudi::Vector3 seed ;
  //
  typedef TrEntries4::iterator      IT ;
  //
  for ( IT ientry = entries.begin() ; entries.end() != ientry ; ++ientry ) 
  { 
    //
    if ( s_equal ( ientry->m_weight , 0 ) ) { continue ; }
    //
    ci   += ientry -> m_weight * ( ientry->m_vxi                  ) ; 
    seed += ientry -> m_weight * ( ientry->m_vxi * ientry->m_parx ) ; 
    //
  }
  //
  Gaudi::SymMatrix3x3 c ; 
  if ( 0 != Gaudi::Math::inverse ( ci , c ) )
  { 
    // try to recover using "soft" constraints 
    _smooth ( ci ) ;
    if ( 0 != Gaudi::Math::inverse ( ci , c ) )
    { return StatusCode ( ErrorInMatrixInversion4 , true ) ; } 
  }
  //
  x = c * seed ;
  //
  Gaudi::Math::scale ( ci , scale ) ; // scale the gain matrix 
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// make "multi-step" step of Kalman filter 
// ============================================================================
StatusCode LoKi::KalmanFilter::step 
( LoKi::KalmanFilter::TrEntries4&   entries , 
  const Gaudi::XYZPoint&            x       , 
  const Gaudi::SymMatrix3x3&        ci      , 
  const double                      chi2    )
{
  const Gaudi::Vector3 xx ( x.X () , x.Y() , x.Z() ) ;
  return step ( entries , xx , ci , chi2 ) ;
}
// ============================================================================
// make one step of Kalman filter q
// ============================================================================
#include "LHCbMath/MatrixUtils.h"
StatusCode LoKi::KalmanFilter::step 
( LoKi::KalmanFilter::TrEntries4&   entries , 
  const Gaudi::Vector3&             x       , 
  const Gaudi::SymMatrix3x3&        ci      , 
  const double                      chi2    )
{
  //
  /// \f$ C^{-1}_k=C^{-1}_{k-1}+A^TG_kA =  C^{-1}_{k-1}+ V^{-1}_{k} \f$
  //
  if ( entries.empty() ){ return StatusCode::SUCCESS ; }
  //
  typedef TrEntries4::iterator      IT ;
  //
  Gaudi::SymMatrix3x3 _ci ( ci      ) ;
  Gaudi::Vector3      _x  ( ci * x  ) ;
  //
  for ( IT ientry = entries.begin() ; entries.end() != ientry ; ++ientry ) 
  { 
    //
    if ( s_equal ( ientry->m_weight , 0 ) ) { continue ; }
    //
    _ci += ( ientry->m_vxi                  ) * ientry->m_weight ; 
    _x  += ( ientry->m_vxi * ientry->m_parx ) * ientry->m_weight ; 
    //
  }
  //
  Gaudi::SymMatrix3x3 _c ; 
  if ( 0 != Gaudi::Math::inverse ( _ci , _c ) )
  { return StatusCode ( ErrorInMatrixInversion3 , true ) ; }
  //
  _x  = Gaudi::Vector3 ( _c * _x ) ;
  //
  double dchi2 = chi2 + ROOT::Math::Similarity ( ci , _x - x ) ;
  //
  for ( IT ientry = entries.begin() ; entries.end() != ientry ; ++ientry ) 
  {
    //
    ientry->m_ci = _ci ;
    ientry->m_c  = _c  ;
    ientry->m_x  = _x  ;
    //
    const Gaudi::Vector3 dx = ientry->m_parx - ientry->m_x ; 
    //
    ientry->m_q = ientry->m_parq  - ientry->m_qvsx * ( ientry->m_vxi * dx ) ;
    //
    dchi2      += ientry->m_weight * ROOT::Math::Similarity ( ientry->m_vxi , dx ) ;
    //
    ientry->m_chi2  = dchi2 ;
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// make one step of Kalman filter q
// ============================================================================
StatusCode LoKi::KalmanFilter::step 
( LoKi::KalmanFilter::TrEntries4& entries ) 
{
  //
  /// \f$ C^{-1}_k=C^{-1}_{k-1}+A^TG_kA =  C^{-1}_{k-1}+ V^{-1}_{k} \f$
  //
  if ( entries.empty() ){ return StatusCode::SUCCESS ; }
  //
  typedef TrEntries4::iterator      IT ;
  //
  Gaudi::SymMatrix3x3 _ci ; 
  Gaudi::Vector3      _x  ;
  //
  for ( IT ientry = entries.begin() ; entries.end() != ientry ; ++ientry ) 
  { 
    //
    if ( s_equal ( ientry->m_weight , 0 ) ) { continue ; }
    //
    _ci += ( ientry->m_vxi                  ) * ientry->m_weight ; 
    _x  += ( ientry->m_vxi * ientry->m_parx ) * ientry->m_weight ; 
    //
  }
  //
  Gaudi::SymMatrix3x3 _c ; 
  if ( 0 != Gaudi::Math::inverse ( _ci , _c ) ) 
  { return StatusCode ( ErrorInMatrixInversion3 , true ) ; }
  //
  _x = Gaudi::Vector3 ( _c * _x ) ;
  //
  double dchi2 = 0 ;
  //
  for ( IT ientry = entries.begin() ; entries.end() != ientry ; ++ientry ) 
  {
    //
    ientry -> m_ci = _ci ;
    ientry -> m_c  = _c  ;
    ientry -> m_x  = _x  ;
    //
    const Gaudi::Vector3 dx = ientry->m_parx - ientry->m_x ; 
    //
    ientry->m_q = ientry->m_parq  - ientry->m_qvsx * ( ientry->m_vxi * dx ) ;
    //
    dchi2      += ientry->m_weight * ROOT::Math::Similarity ( ientry->m_vxi , dx ) ;
    //
    ientry->m_chi2  = dchi2 ;
  }
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
  if ( 0 != Gaudi::Math::inverse ( entry1.m_ci , entry1.m_c ) )
  { return StatusCode ( ErrorInMatrixInversion3 , true ) ; }
  entry2.m_c  = entry1.m_c ;
  
  /// \f$\vec{x}_k\f$
  entry1.m_x = entry1.m_c * ( entry1.m_vxi * entry1.m_parx + 
                              entry2.m_vxi * entry2.m_parx ) ;
  entry2.m_x = entry1.m_x ;
  // OK ! 
  const Gaudi::Vector3 dx1 =   entry1.m_parx -    entry1.m_x ;  
  entry1.m_q = entry1.m_parq - entry1.m_qvsx *  ( entry1.m_vxi * dx1 ) ; 
  // OK !
  const Gaudi::Vector3 dx2 =   entry2.m_parx -    entry2.m_x ;  
  entry2.m_q = entry2.m_parq - entry2.m_qvsx *  ( entry2.m_vxi * dx2 ) ; 
  // OK ! 
  const double dchi2_1 = ROOT::Math::Similarity ( entry1.m_vxi , dx1 ) ;
  //
  // update chi2 
  entry1.m_chi2  = chi2          + dchi2_1 ;
  //
  const double dchi2_2 = ROOT::Math::Similarity ( entry2.m_vxi , dx2 ) ;
  //
  entry2.m_chi2  = entry1.m_chi2 + dchi2_2 ;
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
  if ( 0 != Gaudi::Math::inverse ( entry1.m_ci , entry1.m_c ) ) 
  { return StatusCode ( ErrorInMatrixInversion3 , true ) ; }
  entry2.m_c  = entry1.m_c ;
  entry3.m_c  = entry1.m_c ;
  
  /// \f$\vec{x}_k\f$
  entry1.m_x = entry1.m_c * ( entry1.m_vxi * entry1.m_parx + 
                              entry2.m_vxi * entry2.m_parx +
                              entry3.m_vxi * entry3.m_parx ) ;
  
  entry2.m_x = entry1.m_x ;
  entry3.m_x = entry1.m_x ;
  // OK ! 
  const Gaudi::Vector3 dx1 =   entry1.m_parx -   entry1.m_x ;  
  entry1.m_q = entry1.m_parq - entry1.m_qvsx * ( entry1.m_vxi * dx1 ) ; 
  // OK !
  const Gaudi::Vector3 dx2 =   entry2.m_parx -   entry2.m_x ;  
  entry2.m_q = entry2.m_parq - entry2.m_qvsx * ( entry2.m_vxi * dx2 ) ; 
  // OK ! 
  const Gaudi::Vector3 dx3 =   entry3.m_parx -   entry3.m_x ;  
  entry3.m_q = entry3.m_parq - entry3.m_qvsx * ( entry3.m_vxi * dx3 ) ; 
  
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
  if ( 0 != Gaudi::Math::inverse ( entry1.m_ci , entry1.m_c ) ) 
  { return StatusCode ( ErrorInMatrixInversion3 , true ) ; }
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
  const Gaudi::Vector3 dx1 =   entry1.m_parx -   entry1.m_x ;  
  entry1.m_q = entry1.m_parq - entry1.m_qvsx * ( entry1.m_vxi * dx1 ) ; 
  // OK !
  const Gaudi::Vector3 dx2 =   entry2.m_parx -   entry2.m_x ;  
  entry2.m_q = entry2.m_parq - entry2.m_qvsx * ( entry2.m_vxi * dx2 ) ; 
  // OK ! 
  const Gaudi::Vector3 dx3 =   entry3.m_parx -   entry3.m_x ;  
  entry3.m_q = entry3.m_parq - entry3.m_qvsx * ( entry3.m_vxi * dx3 ) ; 
  // OK ! 
  const Gaudi::Vector3 dx4 =   entry4.m_parx -   entry4.m_x ;  
  entry4.m_q = entry4.m_parq - entry4.m_qvsx * ( entry4.m_vxi * dx4 )  ; 
  
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
( const LHCb::State*            state  , 
  LoKi::KalmanFilter::TrEntry4& entry  , 
  const double                  weight ) 
{
  /// reset the state 
  entry.m_state  = 0 ;                                       // reset the state 
  entry.m_weight = 1 ;
  entry.m_track  = 0 ;
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
      entry.m_qvsx ( j , i ) = cov ( i , j + 2 ) ; // cov ( q , x ) 
    }
  }  
  //
  // Try to invert non-invertible matrix :-) 
  //
  Gaudi::SymMatrix2x2 cixy ;
  cixy ( 0 , 0 ) = cov ( 0, 0 ) ; // cov(x,x)
  cixy ( 0 , 1 ) = cov ( 0, 1 ) ; // cov(x,y)
  cixy ( 1 , 1 ) = cov ( 1, 1 ) ; // cov(y,y)
  if ( !cixy.InvertChol() && !cixy.Invert() )  
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
  const Gaudi::Vector2 cslope ( cixy   * slopes ) ; // reduced slopes!!!
  //
  entry.m_vxi ( 0 , 2 ) = -1 * cslope ( 0 ) ;
  entry.m_vxi ( 1 , 2 ) = -1 * cslope ( 1 ) ;
  entry.m_vxi ( 2 , 2 ) = ROOT::Math::Similarity ( slopes , cixy ) ;
  //
  entry.m_state  = state  ;  
  entry.m_weight = weight ; 
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  calculate number of active tracks  
 *  @param entries (input) list of entries 
 *  @param weight  (input) the minimum weigth to be considered 
 *  @return number of tarcks 
 */
// ============================================================================
unsigned int 
LoKi::KalmanFilter::nTracks
( const LoKi::KalmanFilter::TrEntries4& entries , 
  const double                          weight  )
{
  //
  unsigned int n = 0 ;
  for ( TrEntries4::const_iterator ie = entries.begin() ; 
        entries.end() != ie ; ++ie ) 
  { if ( weight <= ie->m_weight ) { ++n ; } }
  //
  return n ;
}
// ========================================================================

// ============================================================================
// The END 
// ============================================================================
