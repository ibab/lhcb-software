// $Id: StateZTraj.h,v 1.1 2009-07-08 13:33:45 wouter Exp $
#ifndef TRACKKERNEL_STATEZTRAJ_H
#define TRACKKERNEL_STATEZTRAJ_H 1

// Include files
// -------------

// STL
#include <cmath>
#include <memory>

// from LHCbKernel
#include "Kernel/DifTraj.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "TrackKernel/ZTrajectory.h"

namespace LHCb
{

  /** @class StateZTraj StateZTraj.h TrackKernel/StateZTraj.h
   *
   * Trajectory created from a State, parameterized in z. This still needs some
   * work. It is a DifTraj, but because I cannot use MI, it actually does not
   * derive from DifTraj. Since DifTraj is a tenmplated base anyway, it wouldn't
   * help either, so nobody will care.
   *
   * @author Wouter Hulsbergen (after StateTraj by Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues)
   * @date   15/10/2007
   */

  class StateZTraj: public ZTrajectory {

  public:

    /// Enum providing number of colums in derivative matrix
    enum { kSize = 5 };

    // typedefs
    typedef ROOT::Math::SMatrix<double,3,kSize> Derivative;
    typedef ROOT::Math::SVector<double,kSize>   Parameters;

    /// get me another one of these!
    std::unique_ptr<Trajectory> clone() const { return std::unique_ptr<Trajectory>(new StateZTraj(*this)) ; }
    
    /// Default Destructor
    virtual ~StateZTraj() {};

    /// Constructor from a State or StateVector and the magnetic field at the States position
    template <class StateT> StateZTraj( const StateT& state, const Gaudi::XYZVector& bField ) ;
    
    /// Point on trajectory where parabolic approximation is made
    virtual Gaudi::XYZPoint position( double z ) const { return Gaudi::XYZPoint(x(z),y(z),z) ; }

    /// First derivative of the trajectory at the approximation point
    virtual Gaudi::XYZVector direction( double z ) const { return Gaudi::XYZVector(tx(z),ty(z),1) ; }
    
    /// Second derivative of the trajectory at the approximation point,
    /// used as the constant value of the curvature of the parabolic approximation
    virtual Gaudi::XYZVector curvature( double z) const { return Gaudi::XYZVector(omegax(z),omegay(z),0) ; }

    /// Create a parabolic approximation to the trajectory
    virtual void expansion( double z, Gaudi::XYZPoint& p, Gaudi::XYZVector& dp, Gaudi::XYZVector& ddp ) const;

    /// Retrieve the parameters of this traj...
    Parameters parameters( ) const;

    /// Update the parameters of this traj...
    StateZTraj& operator+=(const Parameters& delta);

    /// Retrieve the derivative of the parabolic approximation to the
    /// trajectory with respect to the state parameters
    Derivative derivative( double mu ) const;

    /// give arclength where this trajectory is closest to the
    /// specified point. (linear only. can be improved)
    virtual double muEstimate( const Gaudi::XYZPoint& point) const ;
        
    /// Number of arclengths until deviation of the trajectory from the expansion
    /// reaches the given tolerance (does not account for the curvature).
    virtual double distTo1stError( double arclength,
                                   double tolerance,
                                   int pathDirection = +1 ) const;

    /// Number of arclengths until deviation of the trajectory from the
    /// expansion reaches the given tolerance (accounts for the curvature).
    virtual double distTo2ndError( double arclen,
                                   double tolerance,
                                   int pathDirection = +1 ) const; 


    using ZTrajectory::arclength;
    /// Distance, along the Trajectory, between position(mu1) and
    /// position(mu2). Trivial because StateZTraj is parameterized in
    /// arclength.
    virtual double arclength(double mu1, double mu2) const { return mu2 - mu1 ; }

    /// return stateVector at position mu
    virtual StateVector stateVector( double mu ) const ;

  private:
    double x(double z) const { return polyeval(z-m_z,m_cx) ; }
    double tx(double z) const { return poly1stderiveval(z-m_z,m_cx) ; }
    double omegax(double z) const { return poly2ndderiveval(z-m_z,m_cx) ; }
    double y(double z) const { return polyeval(z-m_z,m_cy) ; }
    double ty(double z) const { return poly1stderiveval(z-m_z,m_cy) ; }
    double omegay(double z) const { return poly2ndderiveval(z-m_z,m_cy) ; }
    double polyeval( double dz, const double c[3]) const { return c[0] + dz*(c[1] + dz * c[2]) ; }
    double poly1stderiveval( double dz, const double c[3]) const { return c[1] + 2*dz*c[2] ; }
    double poly2ndderiveval( double /*dz*/, const double c[3]) const { return 2*c[2] ; }
    
  private:
    double m_z ;     ///< z-position of this state
    double m_cx[3] ; ///< Coefficients for parabola x(z)
    double m_cy[3] ; ///< Coefficients for parabola y(z)
    double m_qOverP; ///< the charge-over-momentum Q/P of the State
    double m_Bz ;    ///< z-component of B field (needed to calculate derivative)
  }; // class StateZTraj

  /*************************************************************************************************/
  // inline functions
  /*************************************************************************************************/

  template <class StateT>
  StateZTraj::StateZTraj( const StateT& state, const Gaudi::XYZVector& bfield ) 
    : ZTrajectory(),
      m_z(state.z()),
      m_qOverP(state.qOverP()),
      m_Bz(bfield.z())
  {
    double n = std::sqrt( 1 + state.tx()*state.tx() + state.ty()*state.ty()) ;
    m_cx[0] = state.x() ;
    m_cx[1] = state.tx() ;
    m_cx[2] = n * Gaudi::Units::c_light * m_qOverP * ( -bfield.y() + state.ty()*bfield.z() ) ;
    m_cy[0] = state.y() ;
    m_cy[1] = state.ty() ;
    m_cy[2] = n * Gaudi::Units::c_light * m_qOverP * (  bfield.x() - state.tx()*bfield.z() ) ;
  }
  
  inline double StateZTraj::distTo1stError( double z, double tolerance, int /*pathDirection*/ ) const 
  {
    // look only at x (because it curves most)
    double deriv = poly2ndderiveval(z-m_z,m_cx) ;
#ifdef __INTEL_COMPILER         // Disable ICC remark
  #pragma warning(disable:1572) // Floating-point equality and inequality comparisons are unreliable
  #pragma warning(push)
#endif
    return deriv!=0 ? std::sqrt(std::abs(2*tolerance/deriv)) :10*Gaudi::Units::km;
#ifdef __INTEL_COMPILER         // End disable ICC remark
  #pragma warning(pop)
#endif
  }
  
  inline double StateZTraj::distTo2ndError( double /*z*/, double /*tolerance*/, int /*pathDirection*/ ) const 
  {
    return 10*Gaudi::Units::km ;
  }

} // namespace LHCb

#endif /// TRACKFITEVENT_STATETRAJ_H
