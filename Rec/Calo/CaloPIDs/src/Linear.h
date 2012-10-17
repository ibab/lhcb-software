// ============================================================================
#ifndef CALOPIDS_LINEAR_H 
#define CALOPIDS_LINEAR_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/State.h"
#include "Event/TrackTypes.h"
// ============================================================================
// LHCbMath 
// ============================================================================
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
// ============================================================================
// Math Definitions
// ============================================================================
#include "GaudiKernel/Plane3DTypes.h"
// ============================================================================

namespace Utils
{
  namespace Linear
  {  
    /** make a linear extrapiolation of the state to a certain Z 
     *  @param state (input/output) state to be extrapolated 
     *  @param newZ (input) z-position
     *  @return status code 
     */
    inline 
    StatusCode propagate 
    ( LHCb::State& state , 
      const double newZ  )
    {
      // make a linear extrapolation of state to new Z-position 
      if ( newZ == state.z() ) { return StatusCode::SUCCESS ; }
      // 
      const double dz = newZ - state.z() ;
      // correct the positions:
      state.setZ ( newZ ) ;
      Gaudi::TrackVector& vct = state.stateVector();

      vct[0] += dz*vct[2] ;
      vct[1] += dz*vct[3];

      // correct the covarinaces:
      Gaudi::TrackSymMatrix& cov = state.covariance() ;
      // 
      const double dz2  = dz * dz ;
      //
      enum { 
        X  = 0 , 
        Y  = 1 , 
        TX = 2 , 
        TY = 3 , 
        QP = 4 
      } ;
      //
      const double _cxx    = cov (  X ,  X ) ;
      const double _cxy    = cov (  X ,  Y ) ;
      const double _cxtx   = cov (  X , TX ) ;
      const double _cxty   = cov (  X , TY ) ;
      const double _cxqp   = cov (  X , QP ) ;
      //
      const double _cyy    = cov (  Y ,  Y ) ;
      const double _cytx   = cov (  Y , TX ) ;
      const double _cyty   = cov (  Y , TY ) ;
      const double _cyqp   = cov (  Y , QP ) ;
      //
      const double _ctxtx  = cov ( TX , TX ) ;
      const double _ctxty  = cov ( TX , TY ) ;
      const double _ctxqp  = cov ( TX , QP ) ;
      //
      const double _ctyty  = cov ( TY , TY ) ;
      const double _ctyqp  = cov ( TY , QP ) ;
      //
      // const double _cqpqp  = cov ( QP , QP ) ;
      //
      // update the matrix 
      //
      cov (  X ,  X ) = _cxx   + 2*dz*_cxtx       + dz2 * _ctxtx ;
      cov (  X ,  Y ) = _cxy   + dz*(_cxty+_cytx) + dz2 * _ctxty ;
      cov (  X , TX ) = _cxtx  +                    dz  * _ctxtx ;
      cov (  X , TY ) = _cxty  +                    dz  * _ctxty ;
      cov (  X , QP ) = _cxqp  +                    dz  * _ctxqp ;
      //
      cov (  Y ,  Y ) = _cyy   + 2*dz*_cyty       + dz2 * _ctyty ;
      cov (  Y , TX ) = _cytx  +                    dz  * _ctxty ;
      cov (  Y , TY ) = _cyty  +                    dz  * _ctyty ;
      cov (  Y , QP ) = _cyqp  +                    dz  * _ctyqp ;
      //
      //
      return StatusCode::SUCCESS ;
    }
    /** make a linear extrapiolation of the state to a certain Z 
     *  @param s1 (input) state to be extrapolated 
     *  @param newZ (input) z-position
     *  @param s2 (output) extrapoalted state 
     *  @return status code 
     */
    inline StatusCode propagate 
    ( const LHCb::State& s1   , 
      const double       newZ ,
      LHCb::State&       s2   ) 
    {
      // copy the state 
      s2 = s1 ;
      // propagate it 
      return propagate ( s2 , newZ ) ;
    }
    /** make a linear extrapolation of the state to a certain 3D-plane 
     *  @param state (input/output) state to be extrapolated 
     *  @param plane (input) plane 
     *  @return status code 
     */    
    inline StatusCode propagate 
    ( LHCb::State&          state , 
      const Gaudi::Plane3D& plane ) 
    {
      typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> Line ;
      // construct the line form the state 
      const Line line = Line( state.position() , state.slopes () ) ;
      Gaudi::XYZPoint point ;
      double mu = 0 ;
      // find the interseciton of the line with plane 
      if ( !Gaudi::Math::intersection ( line , plane , point , mu ) )
      { return StatusCode::FAILURE ; }
      // propagate to a certain Z 
      return propagate ( state , point.Z() ) ;
    }
    /** make a linear extrapolation of the state to a certain 3D-plane 
     *  @param s1 (input) state to be extrapolated 
     *  @param pl (input) plane 
     *  @param s2 (output) extrapoalted state 
     *  @return status code 
     */
    inline StatusCode propagate 
    ( const LHCb::State&    s1 , 
      const Gaudi::Plane3D& pl ,
      LHCb::State&          s2 )  
    {
      // copy the state 
      s2 = s1 ;
      // propagate it 
      return propagate ( s2 , pl ) ;
    }
  }
}

// ============================================================================
// The END 
// ============================================================================
#endif // LINEAR_H
// ============================================================================
