// $Id: CubicStateVectorInterpolationTraj.h,v 1.1 2009-07-08 13:33:45 wouter Exp $
#ifndef TRACKKERNEL_CubicStateVectorInterpolationTraj_H
#define TRACKKERNEL_CubicStateVectorInterpolationTraj_H 1

// Includes
#include <memory>
#include <cmath>
#include "Event/State.h"
#include "Event/StateVector.h"
#include "TrackKernel/ZTrajectory.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "LHCbMath/FastRoots.h"

namespace LHCb 
{
  /** @class CubicStateVectorInterpolationTraj CubicStateVectorInterpolationTraj.h
   * 
   * Cubic interpolating between two state vectors. Used for
   * TrackTraj. Eventuall, we should turn this into a trajectory.
   *
   * @author Wouter Hulsbergen
   * @date   10/10/2007
   * 
   */
  class CubicStateVectorInterpolationTraj : public ZTrajectory {
  public:
    typedef Gaudi::XYZPoint Point ;
    typedef Gaudi::XYZVector Vector ;

    /// Default constructor. Does nothing.
    CubicStateVectorInterpolationTraj() : ZTrajectory() 
    {
      m_cx[0] = m_cx[1] = m_cx[2] = m_cx[3] = 0 ;
      m_cy[0] = m_cy[1] = m_cy[2] = m_cy[3] = 0 ;
      m_qopbegin = m_qopend = 0 ;
    }
 
    /// Constructor from two states of state-vectors
    template<class StateT>
    CubicStateVectorInterpolationTraj( const StateT& begin,
                                       const StateT& end ) : ZTrajectory() { init(begin,end) ; }
    
    /// Constructor from single state(vector) and B-field
    template<class StateT>
    CubicStateVectorInterpolationTraj( const StateT& state,
                                       const Gaudi::XYZVector& bfield ) : ZTrajectory() { init(state,bfield) ; }

    /// Initialize with two states 
    template<class StateT>
    void init( const StateT& begin, const StateT& end ) ;

    /// Initialize with singel state and B field
    template<class StateT>
    void init( const StateT& state, const Gaudi::XYZVector& bfield ) ;
    

  public: // trajectory interface

    /// Clone this trajectory
    std::unique_ptr<Trajectory> clone() const { return std::unique_ptr<Trajectory>(new CubicStateVectorInterpolationTraj(*this)) ; }
    /// Position at location z
    Point position( double z ) const { return Point(x(z),y(z),z) ; }
    /// First derivative of position to z
    Vector direction( double z ) const { return Vector(tx(z),ty(z),1) ; }
    /// Second derivative of position to z
    Vector curvature( double z ) const { return Vector(omegax(z),omegay(z),0) ; }
    /// Distance in z until the deviation from the linear
    /// approximation differs from this trajectory by a given tolerance.
    double distTo1stError( double z, double tolerance, int pathDirection = +1 ) const ;
    /// Distance in z until the deviation from the quadratic
    /// approximation differs from this trajectory by a given tolerance.
    double distTo2ndError( double z, double tolerance, int pathDirection = +1 ) const ;
    /// Create a parabolic approximation to the trajectory
    void expansion(  double z, Gaudi::XYZPoint& p, Gaudi::XYZVector& dp, Gaudi::XYZVector& ddp  ) const ;
    using ZTrajectory::arclength;
    /// Arclength between 2 z -locations (not yet imlpemented)
    double arclength(double z1, double z2) const ;
    /// Estimate for expansion parameter 'z' closest to point
    double muEstimate(const Point& p) const ;
    /// Derivative of arclength to mu
    double dArclengthDMu(double z) const ;

  public: // other public methods
    /// Return x(z)
    double x(double z) const { return polyeval(z-zbegin(),m_cx) ; }
    /// Return dx/dz
    double tx(double z) const { return poly1stderiveval(z-zbegin(),m_cx) ; }
    /// Return d2x/dz2
    double omegax(double z) const { return poly2ndderiveval(z-zbegin(),m_cx) ; }
    /// Return y(z)
    double y(double z) const { return polyeval(z-zbegin(),m_cy) ; }
    /// Return dy/dz
    double ty(double z) const { return poly1stderiveval(z-zbegin(),m_cy) ; }
    /// Return d2y/dz2
    double omegay(double z) const { return poly2ndderiveval(z-zbegin(),m_cy) ; }
    
    /// Return the z of the begin point
    double zbegin() const { return beginRange() ; }
    /// Return the z of the end point
    double zend()   const { return endRange() ; }
    
    /// qop at position z
    double qop(double z) const ;
    /// momentum vector at position z
    Gaudi::XYZVector momentum(double z) const { return std::abs(1/qop(z)) * Gaudi::XYZVector(tx(z),ty(z),1).Unit() ; }
    /// state vector
    virtual StateVector stateVector(double z) const ;

  private:
    void interpolate1D( double za, double xa, double txa, double zb, double xb, double txb,
			double coeff[4] ) const ;
    double polyeval( double dz, const double c[4]) const { return c[0] + dz*(c[1] + dz * (c[2] + dz*c[3])) ; }
    double poly1stderiveval( double dz, const double c[4]) const { return c[1] + dz*(2*c[2] + dz*3*c[3]) ; }
    double poly2ndderiveval( double dz, const double c[4]) const { return 2*c[2] + dz*6*c[3] ; }
    
  private:
    double m_cx[4] ;    ///< parameters of x(z)
    double m_cy[4] ;    ///< parameters of y(z)
    double m_qopbegin ; ///< q/p at first state
    double m_qopend ;   ///< q/p at second state
  } ;

  /*************************************************************************************************/
  // inline functions
  /*************************************************************************************************/

  template<class StateT> void  
  CubicStateVectorInterpolationTraj::init( const StateT& begin,
					   const StateT& end )
  {
    setRange( begin.z(), end.z() ) ;
    m_qopbegin = begin.qOverP() ;
    m_qopend   = end.qOverP() ;
    interpolate1D(begin.z(), begin.x(), begin.tx(), end.z(), end.x(), end.tx(), m_cx) ;
    interpolate1D(begin.z(), begin.y(), begin.ty(), end.z(), end.y(), end.ty(), m_cy) ;
  }
  
  template<class StateT> void  
  CubicStateVectorInterpolationTraj::init( const StateT& state,
                                       const Gaudi::XYZVector& bfield)
  {
    setRange( state.z(), state.z() ) ;
    m_qopbegin = m_qopend = state.qOverP() ;
    double n = std::sqrt( 1 + state.tx()*state.tx() + state.ty()*state.ty()) ;
    m_cx[0] = state.x() ;
    m_cx[1] = state.tx() ;
    m_cx[2] = n * Gaudi::Units::c_light * m_qopbegin * ( -bfield.y() + state.ty()* bfield.z() ) ;
    m_cx[3] = 0 ;
    m_cy[0] = state.y() ;
    m_cy[1] = state.ty() ;
    m_cy[2] = n * Gaudi::Units::c_light * m_qopbegin * (  bfield.x() - state.tx()* bfield.z() ) ;
    m_cy[3] = 0 ;
  }

#ifdef __INTEL_COMPILER         // Disable ICC remark
  #pragma warning(disable:1572) // Floating-point equality and inequality comparisons are unreliable
  #pragma warning(push)
#endif
  
  inline double CubicStateVectorInterpolationTraj::distTo1stError( double z, double tolerance, int pathDirection ) const 
  {
    double deriv = poly2ndderiveval(z-zbegin(),m_cx) ;
    // validity range of linear approximation cannot be larger than that of parabolic!
    return std::min( deriv!=0 ? std::sqrt(std::abs(2*tolerance/deriv)) :10*Gaudi::Units::km,
                     distTo2ndError(z,tolerance,pathDirection)) ;
  }
  
  inline double CubicStateVectorInterpolationTraj::distTo2ndError( double /*z*/, double tolerance, int /*pathDirection*/ ) const 
  {
    // need to replace this with a sensible check on size of c3
    return m_cx[3]!=0 ? FastRoots::cbrt(std::abs(tolerance/m_cx[3])) : 10*Gaudi::Units::km ;
  }
#ifdef __INTEL_COMPILER         // End disable ICC remark
  #pragma warning(pop)
#endif
  
  inline double CubicStateVectorInterpolationTraj::qop(double z) const 
  { 
    return z<=zbegin() ? m_qopbegin : 
      ( z>=zend() ? m_qopend : m_qopbegin + (m_qopend-m_qopbegin)*(z-zbegin())/(zend()-zbegin()) ) ; 
  }
  
  inline void CubicStateVectorInterpolationTraj::expansion( double z, Gaudi::XYZPoint& p, Gaudi::XYZVector& dp,
                                                        Gaudi::XYZVector& ddp ) const {
    p.SetX(   x(z)  ) ;
    p.SetY(   y(z)  ) ;
    p.SetZ(   z     ) ;
    dp.SetX(  tx(z) ) ;
    dp.SetY(  ty(z) ) ;
    dp.SetZ(  1     ) ;
    ddp.SetX( omegax(z) ) ;
    ddp.SetY( omegay(z) ) ;
    ddp.SetZ( 0         ) ;
  }   

  inline void CubicStateVectorInterpolationTraj::interpolate1D( double za, double xa, double txa, 
                                                                double zb, double xb, double txb,
                                                                double c[4] ) const
  {
    c[0] = xa ;
    c[1] = txa ;
    double d  = zb-za ;
    c[2] = (3*(xb-xa)-d*(2*txa+txb))/(d*d) ;
    c[3] = (txb-txa-2*c[2]*d)/(3*d*d) ;
  }

  inline double CubicStateVectorInterpolationTraj::dArclengthDMu(double z) const 
  {
    double thistx = tx(z) ;
    double thisty = ty(z) ;
    return std::sqrt( 1 + thistx*thistx + thisty*thisty ) ;
  }
}
#endif
