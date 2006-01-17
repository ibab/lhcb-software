// $Id: ParabolaTraj.h,v 1.2 2006-01-17 09:34:48 ebos Exp $
#ifndef LHCbKernel_ParabolaTraj_H
#define LHCbKernel_ParabolaTraj_H 1

// Include files
#include "Kernel/Trajectory.h"

/** @class ParabolaTraj ParabolaTraj.h
 *
 * Trajectory parameterized as a parabola
 *
 * @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
 * @date   09/01/2006
 * 
 */

class ParabolaTraj: public Trajectory {
    
public:

  /// Default Constructor
  ParabolaTraj() {};

  /// Default Destructor
  virtual ~ParabolaTraj() {};
    
  /// Constructor from a (middle) point, a unit direction vector
  ParabolaTraj( const HepPoint3D& middle,
                const HepVector3D& dir,
                const HepVector3D& curv );
  
  /// Point on trajectory where parabolic approximation is made
  virtual HepPoint3D position( const double& arclength ) const;
    
  /// First derivative of the trajectory at the approximation point
  virtual HepVector3D direction( const double& arclength ) const;
    
  /// Second derivative of the trajectory at the approximation point,
  /// used as the constant value of the curvature of the parabolic approximation
  virtual HepVector3D curvature() const;
    
  /// Create a parabolic approximation to the trajectory
  virtual void expansion( const double& arclength,
                          HepPoint3D& p,
                          HepVector3D& dp,
                          HepVector3D& ddp ) const;
    
  /// Retrieve the derivative of the parabolic approximation to the trajectory
  /// with respect to the state parameters
  virtual const HepMatrix derivative( const double& arclength ) const;

  /// Number of arclengths until deviation of the trajectory from the expansion
  /// reaches the given tolerance (does not account for the curvature).
  virtual double distTo1stError( double& arclength,
                                 const double& tolerance, 
                                 int pathDirection = +1 ) const;

  /// Number of arclengths until deviation of the trajectory from the expansion
  /// reaches the given tolerance (accounts for the curvature).
  virtual double distTo2ndError( double& arclength,
                                 const double& tolerance, 
                                 int pathDirection = +1 ) const;

  /// Range in arclength over which the trajectory is valid
  virtual std::pair<double,double> range() const;

private:

  HepPoint3D  m_pos; ///< Starting point on the trajectory
  HepVector3D m_dir; ///< Unit direction vector of the trajectory
  HepVector3D m_curv;///< Curvature of the trajectory

}; // class ParabolaTraj

#endif /// LHCbKernel_ParabolaTraj_H
