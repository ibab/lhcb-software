// ============================================================================
// Include files 
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/MatrixTransforms.h"
// ============================================================================
// KalmanFilter 
// ============================================================================
#include "KalmanFilter/FastVertex.h"
#include "KalmanFilter/VertexFitWithTracks.h"
// ============================================================================
/** @file 
 *
 *  Utilities for fast vertex fitting & related math 
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2010-12-02
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  typedef LoKi::FastVertex::Line                                        Line_ ;
  // ==========================================================================
  /// construct the line trajectory from the track
  inline 
  Line_  line   ( const LHCb::State&    state    )
  { return Line_
      ( Gaudi::XYZPoint  ( state.x () , state.y () , state.z() ) , 
        Gaudi::XYZVector ( state.tx() , state.ty() , 1.0       ) ) ; 
  }
  /// get the state from the track 
  inline 
  const LHCb::State* state ( const LHCb::Track& track  )
  {
    const LHCb::State* s = track.stateAt( LHCb::State::ClosestToBeam ) ;
    if ( 0 != s ) { return s ; }
    return &track.firstState();
  }
  /// get the state from the track.closest to certain Z  
  inline 
  const LHCb::State* state ( const LHCb::Track& t , 
                             const double       z ) 
  { return &t.closestState ( z ) ; }
  /// get the central point 
  inline Gaudi::XYZPoint center 
  ( const Gaudi::XYZPoint& p1 , 
    const Gaudi::XYZPoint& p2 ) 
  {
    return Gaudi::XYZPoint ( 0.5 * ( p1.X () + p2.X () ) , 
                             0.5 * ( p1.Y () + p2.Y () ) , 
                             0.5 * ( p1.Z () + p2.Z () ) ) ;
  }                                 
  // ==========================================================================
} //                                                end of anonymous namesapace 
// ============================================================================
/*  make 2-prong vertex from two tracks 
 *  @param track1  (INPUT) the first  track
 *  @param track2  (INPUT) the second track
 *  @param docamax (INPUT) maximal value of      DOCA   (negative: no cut)
 *  @param chi2max (INPUT) maximal value of chi2(DOCA)  (negative: no cut)
 *  @param doca    (OUTPUT) value of      DOCA   
 *  @param chi2    (OUTPUT) value of chi2(DOCA) 
 *  @param iterate (INPUT) iterate?
 *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
 *  @date   2010-12-03
 */
// ============================================================================
LHCb::RecVertexHolder LoKi::FastVertex::makeVertex 
( const LHCb::Track*   track1   , 
  const LHCb::Track*   track2   ,
  const double         docamax  ,
  const double         chi2max  ,  
  double&              doca     ,
  double&              chi2     , 
  const bool           iterate  )  
{
  //
  doca = -1.e+99 ;
  chi2 = -1.e+99 ;
  //
  if ( 0      == track1 ||
       0      == track2 || 
       track1 == track2 ) { return 0 ; } // RETURN 
  //
  double mu1 = 0 ;
  double mu2 = 0 ;
  //
  const LHCb::State* state1 = state ( *track1 ) ;
  const LHCb::State* state2 = state ( *track2 ) ;
  //
  Line line1 ( line ( *state1 ) ) ;
  Line line2 ( line ( *state2 ) ) ;
  //
  // (re)use the nice functions by Matt&Juan
  Gaudi::Math::closestPointParams ( line1 , line2 , mu1 , mu2 ) ;
  //
  Gaudi::XYZPoint point1 = line1 ( mu1 ) ; // the point on the first  trajectory
  Gaudi::XYZPoint point2 = line2 ( mu2 ) ; // the point on the second trajectory
  //
  if ( iterate ) 
  {
    //
    const LHCb::State* state1_ = state ( *track1 , point1.Z () ) ;
    const LHCb::State* state2_ = state ( *track2 , point2.Z () ) ;
    //
    if ( state1_ != state1 || state2_ != state2 ) 
    {
      if ( state1_ != state1 ) 
      { 
        state1 = state1_ ;
        line1  = line ( *state1 ) ;
      }
      if ( state2_ != state2 ) 
      { 
        state2 = state2_ ;
        line2  = line ( *state2_ ) ; 
      }
      //
      // (re)use the nice functions by Matt&Juan
      Gaudi::Math::closestPointParams ( line1 , line2 , mu1 , mu2 ) ;
      //
      point1 = line1 ( mu1 ) ; // the point on the first  trajectory
      point2 = line2 ( mu2 ) ; // the point on the second trajectory
      //
    } 
  }
  //
  // apply DOCA cut (if needed) 
  //
  doca = ( point1 - point2 ).R() ;
  if ( 0 < docamax && docamax < doca ) { return 0 ; } // RETURN 
  //
  if ( chi2max <= 0 ) 
  {
    //
    LHCb::RecVertex*      vertex = new LHCb::RecVertex()  ;
    LHCb::RecVertexHolder holder ( vertex ) ;
    //
    // set the invalid chi2 
    vertex -> setChi2AndDoF ( -1000 , 2*2 - 3 ) ;
    //
    vertex -> setPosition   ( center ( point1 , point2 ) ) ;
    //
    vertex -> addToTracks   ( track1 ) ;
    vertex -> addToTracks   ( track2 ) ;
    //
    return holder ;                                                // RETURN 
  }
  //
  // normal vertex fit 
  //
  LoKi::KalmanFilter::TrEntry4 entry1 ;
  LoKi::KalmanFilter::load ( state1 , entry1 );
  LoKi::KalmanFilter::TrEntry4 entry2 ;
  LoKi::KalmanFilter::load ( state2 , entry2 );
  StatusCode sc = LoKi::KalmanFilter::step ( entry1 , entry2 , 0 ) ;
  //
  if ( sc.isFailure()          ) { return LHCb::RecVertexHolder () ; } // RETURN 
  // 
  // apply chi2-cut
  //
  chi2 = entry2.m_chi2 ;
  if ( chi2max < entry2.m_chi2 ) { return LHCb::RecVertexHolder () ; } // RETURN 
  //
  LHCb::RecVertex*      vertex  = new LHCb::RecVertex()  ;
  LHCb::RecVertexHolder holder ( vertex ) ;
  //
  vertex -> setCovMatrix  ( entry2.m_c              ) ;
  vertex -> setChi2AndDoF ( entry2.m_chi2 , 2*2 - 3 ) ;
  //
  Gaudi::XYZPoint position ;
  Gaudi::Math::la2geo     ( entry2.m_x    , position ) ;
  //
  vertex -> setPosition   ( position   ) ;
  //
  vertex -> addToTracks   ( track1 ) ;
  vertex -> addToTracks   ( track2 ) ;
  //
  return holder ;
}
// ============================================================================
/** make 2-prong vertex from two tracks 
 *  @param track1  (INPUT) the first  track
 *  @param track2  (INPUT) the second track
 *  @param docamax (INPUT) maximal value of      DOCA   (negative: no cut)
 *  @param chi2max (INPUT) maximal value of chi2(DOCA)  (negative: no cut)
 *  @param iterate (INPUT) iterate?
 *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
 *  @date   2010-12-03
 */
// ============================================================================
LHCb::RecVertexHolder LoKi::FastVertex::makeVertex 
( const LHCb::Track*   track1   , 
  const LHCb::Track*   track2   ,
  const double         docamax  ,
  const double         chi2max  , 
  const bool           iterate  ) 
{
  //
  double doca = -1.e+99 ;
  double chi2 = -1.e+99 ;
  //
  return makeVertex 
    ( track1  , track2  , 
      docamax , chi2max ,
      doca    , chi2    , iterate ) ;
}
// ============================================================================
/*  evaluate the distance between track & vertex ("impact-parameter")
 *  @param track   (INPUT) track 
 *  @param vertex  (INPUT) vertex 
 *  @param iterate (INPUT) iterate?
 *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
 *  @date   2010-12-03
 */
// =============================================================================
StatusCode LoKi::FastVertex::distance 
( const LHCb::Track*      track   , 
  const LHCb::VertexBase* vertex  , 
  Gaudi::XYZVector&       impact  , 
  const bool              iterate ) 
{
  //
  if ( 0 == track || 0 == vertex ) { return StatusCode::FAILURE ; }
  //
  const Gaudi::XYZPoint& point = vertex -> position() ;
  //
  const LHCb::State* s = state ( *track , point.Z() ) ;
  //
  // (re)use the nice functions by Matt&Juan
  impact = Gaudi::Math::closestPoint ( point , line ( *s ) ) - point ;
  //
  if ( iterate ) 
  {
    const LHCb::State* s1 = state ( *track , point.Z() + impact.Z() ) ;
    if ( s1 != s ) 
    {
      s = s1 ;
      // (re)use the nice functions by Matt&Juan
      impact = Gaudi::Math::closestPoint ( point , line ( *s ) ) - point ;
    }
  }
  //
  return StatusCode::SUCCESS ;
}
// ========================================================================
/*  evaluate the distance between the track & the vertex ("impact-parameter")
 *  @param track   (INPUT)  track 
 *  @param vertex  (INPUT)  vertex 
 *  @param imppar  (OUTPUT) impact parameter  
 *  @param chi2    (OUTPUT) chi2 
 *  @param iterate (INPUT)  iterate?
 *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
 *  @date   2010-12-03
 */
// ========================================================================
StatusCode LoKi::FastVertex::distance 
( const LHCb::Track*      track   , 
  const LHCb::VertexBase* vertex  , 
  double&                 imppar  , 
  double&                 chi2    , 
  const bool              iterate ) 
{
  //
  if ( 0 == track || 0 == vertex ) { return StatusCode::FAILURE ; }
  //
  const Gaudi::XYZPoint& point = vertex -> position() ;
  //
  const LHCb::State* s = state ( *track , point.Z() ) ;
  //
  // (re)use the nice functions by Matt&Juan
  Gaudi::XYZVector impact = 
    Gaudi::Math::closestPoint ( point , line ( *s ) ) - point ;
  //
  if ( iterate ) 
  {
    const LHCb::State* s1 = state ( *track , point.Z() + impact.Z() ) ;
    if ( s1 != s ) 
    {  
      s = s1 ;
      // (re)use the nice functions by Matt&Juan
      impact = Gaudi::Math::closestPoint ( point , line ( *s ) ) - point ;
    }
  }
  //
  imppar = impact.R() ;
  //
  // Evaluate chi2 
  // 
  // prepare the Kalman Filter machinery 
  LoKi::KalmanFilter::TrEntry4  entry ;
  StatusCode sc = LoKi::KalmanFilter::load ( s , entry ) ;
  if ( sc.isFailure() ) { return sc ; }                // RETURN 
  // get the "the previus" Kalman Filter estimate == vertex
  Gaudi::SymMatrix3x3 ci = vertex->covMatrix() ; // the gain matrix 
  if ( !ci.Invert()   ) { return StatusCode::FAILURE ; } // RETURN 
  // make one step of Kalman filter 
  sc = LoKi::KalmanFilter::step ( entry , vertex->position() , ci , 0 ) ;
  if ( sc.isFailure() ) { return sc ; }                // RETURN 
  // get the chi2 
  chi2 = entry.m_chi2 ;
  //
  return StatusCode::SUCCESS ; 
}
// ============================================================================
/*  check the distance between the track & the vertex ("impact-parameter")
 *  @param track   (INPUT)  track 
 *  @param vertex  (INPUT)  vertex 
 *  @param ipmax   (INPUT) maximal value of      DOCA   (negative: no cut)
 *  @param chi2max (INPUT) maximal value of chi2(DOCA)  (negative: no cut)
 *  @param iterate (INPUT)  iterate?
 *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
 *  @date   2010-12-03
 */
// ============================================================================
bool LoKi::FastVertex::checkDistance 
( const LHCb::Track*      track   , 
  const LHCb::VertexBase* vertex  ,
  const double            ipmax   , 
  const double            chi2max ,
  const bool              iterate ) 
{
  //
  if ( 0 == track || 0 == vertex ) { return false ; } // RETURN 
  //
  if ( ipmax < 0 && chi2max < 0  ) { return true  ; } // RETURN 
  //
  const Gaudi::XYZPoint& point = vertex -> position() ;
  //
  const LHCb::State* s = state ( *track , point.Z() ) ;
  //
  // (re)use the nice functions by Matt&Juan
  Gaudi::XYZVector impact = 
    Gaudi::Math::closestPoint ( point , line ( *s ) ) - point ;
  //
  if ( iterate ) 
  {
    const LHCb::State* s1 = state ( *track , point.Z() + impact.Z() ) ;
    if ( s1 != s ) 
    {
      // (re)use the nice functions by Matt&Juan
      s = s1 ;
      impact = Gaudi::Math::closestPoint ( point , line ( *s ) ) - point ;
    }
  }
  //
  // check IP 
  //
  if ( 0 < ipmax && ipmax*ipmax < impact.Mag2() ) { return false ; } // RETURN 
  //
  // Evaluate chi2 
  // 
  // prepare the Kalman Filter machinery 
  LoKi::KalmanFilter::TrEntry4  entry ;
  StatusCode sc = LoKi::KalmanFilter::load ( s , entry ) ;
  if ( sc.isFailure() ) { return false ; }                         // RETURN 
  // get the "the previus" Kalman Filter estimate == vertex
  Gaudi::SymMatrix3x3 ci = vertex->covMatrix() ; // the gain matrix 
  if ( !ci.Invert()   ) { return false ; }                         // RETURN 
  // make one step of Kalman filter 
  sc = LoKi::KalmanFilter::step ( entry , vertex->position() , ci , 0 ) ;
  if ( sc.isFailure() ) { return false ; }                         // RETURN 
  // get the chi2 
  if ( 0 <= chi2max && chi2max <  entry.m_chi2 ) { return false ; }
  //
  return true ;
}
// ============================================================================
/** check the distance between the track & the track ("doca")
 *  @param track1  (INPUT) the first  track
 *  @param track2  (INPUT) the second track
 *  @param docamax (INPUT) maximal value of      DOCA   (negative: no cut)
 *  @param chi2max (INPUT) maximal value of chi2(DOCA)  (negative: no cut)
 *  @param iterate (INPUT) iterate?
 *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
 *  @date   2010-12-03
 */
// ============================================================================
bool LoKi::FastVertex::checkDistance
( const LHCb::Track*   track1   , 
  const LHCb::Track*   track2   ,
  const double         docamax  ,
  const double         chi2max  , 
  const bool           iterate  ) 
{
  //
  if ( 0 == track1 || 0 == track2 ) { return false ; } 
  if ( track1 == track2           ) { return true  ; }  
  //
  double mu1 = 0 ;
  double mu2 = 0 ;
  //
  const LHCb::State* state1 = state ( *track1 ) ;
  const LHCb::State* state2 = state ( *track2 ) ;
  //
  Line line1 ( line ( *state1 ) ) ;
  Line line2 ( line ( *state2 ) ) ;
  //
  // (re)use the nice functions by Matt&Juan
  Gaudi::Math::closestPointParams ( line1 , line2 , mu1 , mu2 ) ;
  //
  Gaudi::XYZPoint point1 = line1 ( mu1 ) ; // the point on the first  trajectory
  Gaudi::XYZPoint point2 = line2 ( mu2 ) ; // the point on the second trajectory
  //
  if ( iterate ) 
  {
    //
    const LHCb::State* state1_ = state ( *track1 , point1.Z() ) ;
    const LHCb::State* state2_ = state ( *track2 , point2.Z() ) ;
    //
    if ( state1 != state1_ || state2 != state2_ ) 
    {
      if ( state1 != state1_ ) 
      {
        state1 = state1_ ;  
        line1  = line ( *state1 )  ;
      }
      if ( state2 != state2_ ) 
      {
        state2 = state2_ ;  
        line2  = line ( *state2 )  ;
      }
      //
      // (re)use the nice functions by Matt&Juan
      Gaudi::Math::closestPointParams ( line1 , line2 , mu1 , mu2 ) ;
      //
      point1 = line1 ( mu1 ) ; // the point on the first  trajectory
      point2 = line2 ( mu2 ) ; // the point on the second trajectory
      //
    }    
  }
  //
  // apply DOCA cut (if needed) 
  //
  if ( 0 < docamax && docamax*docamax < (point1-point2).Mag2() ) { return false ; }
  //
  // chi2 ? 
  if ( chi2max < 0 ) { return true ;}
  //
  // normal vertex fit 
  //
  LoKi::KalmanFilter::TrEntry4 entry1 ;
  LoKi::KalmanFilter::load ( state1 , entry1 );
  LoKi::KalmanFilter::TrEntry4 entry2 ;
  LoKi::KalmanFilter::load ( state2 , entry2 );
  StatusCode sc = LoKi::KalmanFilter::step ( entry1 , entry2 , 0 ) ;
  //
  if ( sc.isFailure()          ) { return false ; } // RETURN 
  // 
  // apply chi2-cut
  //
  if ( chi2max < entry2.m_chi2 ) { return false ; } // RETURN 
  //
  return true ;
}
// ============================================================================
/** evaluate the distance between the tracks ("DOCA")
 *  @param track1  (INPUT)  the first  track 
 *  @param track2  (INPUT)  the second track 
 *  @param doca    (OUTPUT) DOCA
 *  @param iterate (INPUT)  iterate?
 *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
 *  @date   2010-12-03
 */
// ============================================================================
StatusCode LoKi::FastVertex::distance 
( const LHCb::Track*      track1  , 
  const LHCb::Track*      track2  , 
  double&                 doca    , 
  const bool              iterate ) 
{
  //
  doca = -1.e+99 ;
  //
  if ( 0 == track1 || 0 == track2 ) { return StatusCode::FAILURE ; }
  //
  if ( track1 == track2 ) 
  {
    doca = 0 ;
    return StatusCode::SUCCESS ;                                  // RETURN 
  }
  //
  double mu1 = 0 ;
  double mu2 = 0 ;
  //
  const LHCb::State* state1 = state ( *track1 ) ;
  const LHCb::State* state2 = state ( *track2 ) ;
  //
  Line line1 = line ( *state1 )  ;
  Line line2 = line ( *state2 )  ;
  //
  // (re)use the nice functions by Matt&Juan
  Gaudi::Math::closestPointParams ( line1 , line2 , mu1 , mu2 ) ;
  //
  Gaudi::XYZPoint point1 = line1 ( mu1 ) ; // the point on the first  trajectory
  Gaudi::XYZPoint point2 = line2 ( mu2 ) ; // the point on the second trajectory
  //
  if ( iterate ) 
  {
    //
    const LHCb::State* state1_ = state ( *track1 , point1.Z() ) ;
    const LHCb::State* state2_ = state ( *track2 , point2.Z() ) ;
    //
    if ( state1 != state1_ || state2 != state2_ ) 
    {
      if ( state1 != state1_ ) 
      {
        state1 = state1_ ;
        line1  = line ( *state1 )  ;
      }
      if ( state2 != state2_ ) 
      {
        state2 = state2_ ;
        line2  = line ( *state2 )  ;
      }
      //
      // (re)use the nice functions by Matt&Juan
      Gaudi::Math::closestPointParams ( line1 , line2 , mu1 , mu2 ) ;
      //
      point1 = line1 ( mu1 ) ; // the point on the first  trajectory
      point2 = line2 ( mu2 ) ; // the point on the second trajectory
      //
    }
  }
  //
  // apply DOCA cut (if needed) 
  //
  doca = ( point1 - point2 ).R() ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/** evaluate the distance between the tracks ("DOCA")
 *  @param track1  (INPUT)  the first  track 
 *  @param track2  (INPUT)  the second track 
 *  @param doca    (OUTPUT)      DOCA
 *  @param chi2    (OUTPUT) chi2(DOCA)
 *  @param iterate (INPUT)  iterate?
 *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
 *  @date   2010-12-03
 */
// ============================================================================
StatusCode LoKi::FastVertex::distance 
( const LHCb::Track*      track1  , 
  const LHCb::Track*      track2  , 
  double&                 doca    , 
  double&                 chi2    , 
  const bool              iterate )
{
  //
  doca = -1.e+99 ;
  chi2 =  1.e+99 ;
  //
  if ( 0 == track1 || 0 == track2 ) { return StatusCode::FAILURE ; }
  //
  if ( track1 == track2 ) 
  {
    doca = 0 ;
    chi2 = 0 ;
    return StatusCode::SUCCESS ;                                  // RETURN 
  }
  //
  double mu1 = 0 ;
  double mu2 = 0 ;
  //
  const LHCb::State* state1 = state ( *track1 ) ;
  const LHCb::State* state2 = state ( *track2 ) ;
  //
  Line line1 = line ( *state1 )  ;
  Line line2 = line ( *state2 ) ;
  //
  // (re)use the nice functions by Matt&Juan
  Gaudi::Math::closestPointParams ( line1 , line2 , mu1 , mu2 ) ;
  //
  Gaudi::XYZPoint point1 = line1 ( mu1 ) ; // the point on the first  trajectory
  Gaudi::XYZPoint point2 = line2 ( mu2 ) ; // the point on the second trajectory
  //
  if ( iterate ) 
  {
    //
    const LHCb::State* state1_ = state ( *track1 , point1.Z() ) ;
    const LHCb::State* state2_ = state ( *track2 , point2.Z() ) ;
    //
    if ( state1 != state1_ || state2 != state2_ ) 
    {
      if ( state1 != state1_ ) 
      {
        state1 = state1_ ;
        line1  = line ( *state1 )  ;
      }
      if ( state2 != state2_ ) 
      {
        state2 = state2_ ;
        line2  = line ( *state2 )  ;
      }
      //
      // (re)use the nice functions by Matt&Juan
      Gaudi::Math::closestPointParams ( line1 , line2 , mu1 , mu2 ) ;
      //
      point1 = line1 ( mu1 ) ; // the point on the first  trajectory
      point2 = line2 ( mu2 ) ; // the point on the second trajectory
      //
    } 
  }
  //
  // get DOCA 
  //
  doca = ( point1 - point2 ).R() ;
  //
  //
  // normal vertex fit 
  //
  LoKi::KalmanFilter::TrEntry4 entry1 ;
  LoKi::KalmanFilter::load ( state1 , entry1 );
  LoKi::KalmanFilter::TrEntry4 entry2 ;
  LoKi::KalmanFilter::load ( state2 , entry2 );
  StatusCode sc = LoKi::KalmanFilter::step ( entry1 , entry2 , 0 ) ;
  //
  if ( sc.isFailure() ) { return StatusCode::FAILURE ; }          // RETURN 
  // 
  // get chi2(DOCA)
  //
  chi2 = entry2.m_chi2 ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  evaluate the distance between the track and the line 
 *
 *  @code 
 * 
 *   const LHCb::Track*      track   = ... ;
 *   const Line              line    = ... ;
 *
 *   double doca   ;
 *
 *   StatusCode sc = LoKi::FastVertex::distance 
 *                     ( track  ,   // the track 
 *                       line   ,   // the line  
 *                       doca   ) ; // the distance 
 * 
 *  @endcode 
 *
 *  @param track1  (INPUT)  the track 
 *  @param line2   (INPUT)  the line 
 *  @param doca    (OUTPUT) DOCA
 *  @param iterate (INPUT)  iterate?
 *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
 *  @date   2010-12-03
 */
// ============================================================================
StatusCode LoKi::FastVertex::distance 
( const LHCb::Track*            track1  , 
  const LoKi::FastVertex::Line& line2   , 
  double&                       doca    , 
  const bool                    iterate ) 
{
  //
  doca = -1.e+99 ;
  //
  if ( 0 == track1 ) { return StatusCode::FAILURE ; }
  //
  double mu1 = 0 ;
  double mu2 = 0 ;
  //
  const LHCb::State* state1 = state ( *track1 ) ;
  //
  Line line1 = line ( *state1 )  ;
  //
  // (re)use the nice functions by Matt&Juan
  Gaudi::Math::closestPointParams ( line1 , line2 , mu1 , mu2 ) ;
  //
  Gaudi::XYZPoint point1 = line1 ( mu1 ) ; // the point on the first  trajectory
  Gaudi::XYZPoint point2 = line2 ( mu2 ) ; // the point on the second trajectory
  //
  if ( iterate ) 
  {
    //
    const LHCb::State* state1_ = state ( *track1 , point1.Z() ) ;
    //
    if ( state1_ != state1 ) 
    {
      state1 = state1_ ;
      line1  = line ( *state1 )  ;
      //
      // (re)use the nice functions by Matt&Juan
      Gaudi::Math::closestPointParams ( line1 , line2 , mu1 , mu2 ) ;
      //
      point1 = line1 ( mu1 ) ; // the point on the first  trajectory
      point2 = line2 ( mu2 ) ; // the point on the second trajectory
      //
    }
  }
  //
  // apply DOCA cut (if needed) 
  //
  doca = ( point1 - point2 ).R() ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  evaluate the distance between the track and the line 
 *
 *  @code 
 * 
 *   const Line              line    = ... ;
 *   const LHCb::Track*      track   = ... ;
 *
 *   double doca   ;
 *
 *   StatusCode sc = LoKi::FastVertex::distance 
 *                     ( line   ,   // the line  
 *                       track  ,   // the track 
 *                       doca   ) ; // the distance 
 * 
 *  @endcode 
 *
 *  @param line1   (INPUT)  the line 
 *  @param track2  (INPUT)  the track 
 *  @param doca    (OUTPUT) DOCA
 *  @param iterate (INPUT)  iterate?
 *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
 *  @date   2010-12-03
 */
// ============================================================================
StatusCode LoKi::FastVertex::distance 
( const LoKi::FastVertex::Line& line1   , 
  const LHCb::Track*            track2  , 
  double&                       doca    , 
  const bool                    iterate ) 
{ 
  return distance ( track2 , line1 , doca , iterate ) ; 
}

  
// ============================================================================
// The END 
// ============================================================================
