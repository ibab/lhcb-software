// $Id:$ 
// ============================================================================
// Include files 
// ============================================================================
// STD& STL
// ============================================================================
#include <cmath>
// ============================================================================
// Event/TrackEvent
// ============================================================================
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/TrackParameters.h"
// ============================================================================
// LHCbMath 
// ============================================================================
#include "LHCbMath/LHCbMath.h"
#include "LHCbMath/ValueWithError.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/TrackFunctions.h"
// ============================================================================
/** @file
 *  implemenation of functions from the file LoKi/TrackFunnctions.h
 *                                              \
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2016-03-05
 *  Version           $Revision:$
 *  Last modification $Date:$
 *  by                $Author:$
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  // is value close to zero ? 
  const LHCb::Math::Zero<double> s_zero ;
  // ==========================================================================
}
// ============================================================================
/* get the transverse momentum (and uncertainty) from the state
 *  @param state (INPUT) the  state 
 *  @return transverse momentum (and uncertainty) from the given state 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2016-03-05
 */
// ============================================================================
Gaudi::Math::ValueWithError
LoKi::Tracks::state_pt 
( const LHCb::State& state ) 
{
  /// get pt itself 
  const double pt =            state.pt()      ;
  //
  const double qP = std::abs ( state.qOverP () ) ; 
  //
  if      ( qP < TrackParameters::lowTolerance ) 
  { return Gaudi::Math::ValueWithError ( pt , pt ) ; }
  else if ( s_zero ( pt )  ) 
  { return Gaudi::Math::ValueWithError ( 0 ) ; }
  //
  const double tx =            state.tx     () ;
  const double ty =            state.ty     () ;
  //
  const double c0 = tx * tx + ty  * ty ;
  if ( s_zero ( c0 ) ) { return Gaudi::Math::ValueWithError ( pt ) ; }
  //
  const double c1 = 1 + c0 ;
  const double c2 = c0 / c1 ;
  const double c3 = std::sqrt ( c0 * c1 ) ;
  //
  // get the vector d(pt)/dp_i :
  ROOT::Math::SVector<double,5> dPt_dP;
  dPt_dP  [0] =  0 ;
  dPt_dP  [1] =  0 ;
  dPt_dP  [2] =  tx * ( 1 - c2 ) / ( c3 * qP ) ;
  dPt_dP  [3] =  ty * ( 1 - c2 ) / ( c3 * qP ) ;
  dPt_dP  [4] = -pt              /        qP   ;
  //
  const double s2pt = ROOT::Math::Similarity ( state.covariance() , dPt_dP ) ;
  //
  return 
    s2pt <= 0 || s_zero ( s2pt ) ?
    Gaudi::Math::ValueWithError ( pt        ) :
    Gaudi::Math::ValueWithError ( pt , s2pt ) ;
}
// ============================================================================
/*  get the phi angle (and uncertainty) from the state
 *  @param state (INPUT) the  state 
 *  @return phi angle  (and uncertainty) from the given state 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2016-03-05
 */
// ============================================================================
Gaudi::Math::ValueWithError 
LoKi::Tracks::state_phi ( const LHCb::State& state ) 
{
  const double tx  = state.tx () ;
  const double ty  = state.ty () ;
  //
  /// get phi itself 
  const double cxx =  state.covariance()(2,2) ;
  const double cxy =  state.covariance()(2,3) ;
  const double cyy =  state.covariance()(3,3) ;
  //
  const bool x_err = 0 < cxx  && !s_zero ( cxx ) ;
  const bool y_err = 0 < cyy  && !s_zero ( cyy ) ;
  //
  if ( !x_err && !y_err ) { return std::atan2 ( ty , tx ) ; }
  //
  return Gaudi::Math::atan2 
    ( Gaudi::Math::ValueWithError  ( ty , y_err ? cyy : 0.0 ) , 
      Gaudi::Math::ValueWithError  ( tx , x_err ? cxx : 0.0 ) , 
      !x_err || !y_err ? 0.0 : cxy / std::sqrt ( cxx * cyy ) ) ;
}
// ============================================================================
/*  get the theta angle (and uncertainty) from the state
 *  @param state (INPUT) the  state 
 *  @return theta angle (and uncertainty) from the given state 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2016-03-05
 */
// ============================================================================
Gaudi::Math::ValueWithError 
LoKi::Tracks::state_theta    ( const LHCb::State& state ) 
{
  //
  const double tx     = state.tx () ;
  const double ty     = state.ty () ;
  const double tanth2 = tx * tx + ty * ty ;
  const double tanth  = std::sqrt ( tanth2 );
  const double theta  = std::atan ( tanth  ) ;
  //
  if ( s_zero ( tanth2 ) ) { return Gaudi::Math::ValueWithError ( theta ) ;}
  
  const double c = 1 / ( ( 1 + tanth2 ) *  tanth ) ;
  
  // get the vector d(theta)/dp_i :
  ROOT::Math::SVector<double,5> dTheta_dP;
  dTheta_dP  [0] =  0 ;
  dTheta_dP  [1] =  0 ;
  dTheta_dP  [2] =  tx * c ;
  dTheta_dP  [3] =  ty * c ;
  dTheta_dP  [4] =  0 ;
  //
  const double s2theta = ROOT::Math::Similarity ( state.covariance() , dTheta_dP ) ;
  //
  return 
    s2theta <= 0 || s_zero ( s2theta ) ?
    Gaudi::Math::ValueWithError ( theta           ) :
    Gaudi::Math::ValueWithError ( theta , s2theta ) ; 
}
// ============================================================================
/*  get the pseudorapidity (and uncertainty) from the state
 *  @param state (INPUT) the  state 
 *  @return pseudorapidity (and uncertainty) from the given state 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2016-03-05
 */
// ============================================================================
Gaudi::Math::ValueWithError 
LoKi::Tracks::state_eta      ( const LHCb::State& state ) 
{
  const Gaudi::Math::ValueWithError theta = state_theta ( state ) ;
  return - log ( tan ( 0.5 * theta ) ) ;
}
// ============================================================================
// The END 
// ============================================================================
