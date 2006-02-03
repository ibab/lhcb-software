// $Id: StateTraj.h,v 1.2 2006-02-03 10:00:51 ebos Exp $
#ifndef TRACKFITEVENT_STATETRAJ_H
#define TRACKFITEVENT_STATETRAJ_H 1

// Include files
#include "Event/State.h"

// from LHCbKernel
#include "Kernel/Trajectory.h"

// fomr Kernel/LHCbDefinitions
#include "Kernel/PhysicalConstants.h"

using namespace Gaudi;
using namespace LHCb;

/** @class StateTraj StateTraj.h
 *
 * Trajectory created from a State. 
 *
 * @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
 * @date   01/12/2005
 * 
 */

class StateTraj : public Trajectory
{

public:

  /// Default Constructor
  StateTraj() {}

  /// Default Destructor
  virtual ~StateTraj() {};

  /// Constructor from a State and the magnetic field at the State position
  StateTraj( const State& state,
             const XYZVector& bField );
  
  /// Constructor from a StateVector and the magnetic field at State position
  StateTraj( const TrackVector& stateVector,
             const double& z,
             const XYZVector& bField );

  /// Point on trajectory where parabolic approximation is made
  virtual XYZPoint position( const double& arclength ) const;
    
  /// First derivative of the trajectory at the approximation point
  virtual XYZVector direction( const double& arclength ) const;
    
  /// Second derivative of the trajectory at the approximation point,
  /// used as the constant value of the curvature of the parabolic approximation
  virtual XYZVector curvature() const;

  /// Create a parabolic approximation to the trajectory
  virtual void expansion( const double& arclength,
                          XYZPoint& p,
                          XYZVector& dp,
                          XYZVector& ddp ) const;

  /// Retrieve the derivative of the parabolic approximation to the trajectory
  /// with respect to the state parameters
  virtual const ROOT::Math::SMatrix<double, 3, 5> derivative( const double& arclength ) const;

  /// Number of arclengths until deviation of the trajectory from the expansion
  /// reaches the given tolerance (does not account for the curvature).
  virtual double distTo1stError( double& arclength,
                                 const double& tolerance, 
                                 int pathDirection = +1 ) const;

  /// Number of arclengths until deviation of the trajectory from the expansion
  /// reaches the given tolerance (accounts for the curvature).
  virtual double distTo2ndError( double& arclen,
                                 const double& tolerance, 
                                 int pathDirection = +1 ) const;

  /// Range in arclength over which the trajectory is valid
  virtual std::pair<double,double> range() const;

private:

  XYZPoint  m_pos;    ///< the position of the State
  XYZVector m_dir;    ///< the unit direction of the State
  XYZVector m_curv;   ///< constant value of parabola's curvature
  double    m_qOverP; ///< the charge-over-momentum Q/P of the State
  XYZVector m_bField; ///< the magnetic field vector at the State position

  /// Proportionality factor for calculating equation of motion in B-field
  static const double kappa = 0.3 * GeV / ( c_light * tesla * meter );

}; // class StateTraj

#endif /// TRACKFITEVENT_STATETRAJ_H
