// $Id: StateTraj.h,v 1.7 2006-04-06 06:45:29 ebos Exp $
#ifndef TRACKFITEVENT_STATETRAJ_H
#define TRACKFITEVENT_STATETRAJ_H 1

// Include files
// -------------
// from Kernel/LHCbDefinitions
#include "Kernel/PhysicalConstants.h"

// from LHCbKernel
#include "Kernel/DifTraj.h"

// from TrackEvent
#include "Event/TrackParameters.h"
#include "Event/State.h"

/** @class StateTraj StateTraj.h
 *
 * Trajectory created from a State. 
 *
 * @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
 * @date   01/12/2005
 * 
 */

namespace LHCb
{
  
  class StateTraj: public DifTraj<5> {
    
  public:
    
    /// Enum providing number of colums in derivative matrix
    enum { kSize = 5 };

    /// get me another one of these!
    std::auto_ptr<Trajectory> clone() const;
    
    /// Default Destructor
    virtual ~StateTraj() {};
    
    /// Constructor from a State and the magnetic field at the State position
    StateTraj( const State& state,
               const Gaudi::XYZVector& bField );
    
    /// Constructor from a StateVector and the magnetic field at State position
    StateTraj( const Gaudi::TrackVector& stateVector,
               double z,
               const Gaudi::XYZVector& bField );
    
    /// Point on trajectory where parabolic approximation is made
    virtual Gaudi::XYZPoint position( double arclength ) const;
    
    /// First derivative of the trajectory at the approximation point
    virtual Gaudi::XYZVector direction( double arclength ) const;
    
    /// Second derivative of the trajectory at the approximation point,
    /// used as the constant value of the curvature of the parabolic approximation
    virtual Gaudi::XYZVector curvature( double arclength) const;

    
    /// Create a parabolic approximation to the trajectory
    virtual void expansion( double arclength,
                            Gaudi::XYZPoint& p,
                            Gaudi::XYZVector& dp,
                            Gaudi::XYZVector& ddp ) const;
    
    /// Retrieve the derivative of the parabolic approximation to the
    /// trajectory with respect to the state parameters
    virtual Derivative derivative( double arclength ) const;

    /// give arclength where this trajectory is closest to the
    /// specified point
    virtual double arclength( const Gaudi::XYZPoint& point) const;
    
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
    
    /// Range in arclength over which the trajectory is valid
    virtual Trajectory::Range range() const;
    
  private:
    
    Gaudi::XYZPoint  m_pos;    ///< the position of the State
    Gaudi::XYZVector m_dir;    ///< the unit direction of the State
    Gaudi::XYZVector m_curv;   ///< constant value of parabola's curvature
    double           m_qOverP; ///< the charge-over-momentum Q/P of the State
    Gaudi::XYZVector m_bField; ///< the magnetic field vector at the State position
    
  }; // class StateTraj
  
} // namespace LHCb

#endif /// TRACKFITEVENT_STATETRAJ_H
