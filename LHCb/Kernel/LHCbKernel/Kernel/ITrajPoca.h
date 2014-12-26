#ifndef KERNEL_ITRAJPOCA_H 
#define KERNEL_ITRAJPOCA_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Vector3DTypes.h"

#include "Trajectory.h"

static const InterfaceID IID_ITrajPoca ( "ITrajPoca", 1, 0 );

/** @class ITrajPoca ITrajPoca.h
 *  
 *  Interface for tools calculating points of closest approach
 *  between two trajectories
 *
 *  @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
 *  @date   2005-11-22
 */

class ITrajPoca : virtual public IAlgTool {

public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrajPoca; }

  

  /// Find points along trajectories at which the distance between the
  /// trajectories is at its minimum. The precision parameter is the desired
  /// numerical accuracy of mu1 and mu2. If the restrictrange flag is true, mu
  /// is restricted to the range of the trajectory.
  virtual StatusCode minimize( const LHCb::Trajectory& traj1,
                               double& mu1, 
                               bool restrictRange1,
                               const LHCb::Trajectory& traj2,
                               double& mu2, 
                               bool restrictRange2, 
                               Gaudi::XYZVector& distance,
                               double precision ) const = 0 ;

  /// Find point along trajectory at which the distance to point 'p'
  /// is minimum. The precision parameter is the desired numerical accuracy of
  /// the expansion parameter mu. If the restrictrange flag is true, mu is
  /// restricted to the range of the trajectory.
  virtual StatusCode minimize( const LHCb::Trajectory& traj,
                               double& mu,
                               bool restrictRange,
                               const Gaudi::XYZPoint& pt,
                               Gaudi::XYZVector& distance,
                               double precision ) const = 0;
  
  /// Find points along trajectories at which the distance between the
  /// trajectories is at its minimum. The precision parameter is the desired
  /// numerical accuracy of the expansion parameters mu1 and mu2.
  StatusCode minimize( const LHCb::Trajectory& traj1,
                       double& mu1, 
                       const LHCb::Trajectory& traj2,
                       double& mu2,
                       Gaudi::XYZVector& distance,
                       double precision ) const {
    return minimize( traj1, mu1, false, traj2, mu2, false,distance, precision); 
  }
  
  
  /// Find point along trajectory at which the distance to point 'p'
  /// is minimum. The precision parameter is the desired 
  /// numerical accuracy of the expansion parameter mu.
  StatusCode minimize( const LHCb::Trajectory& traj,
                       double& mu,
                       const Gaudi::XYZPoint& pt,
                       Gaudi::XYZVector& distance, 
                       double precision ) const {
    return minimize( traj, mu, false, pt, distance, precision); 
  }
};
#endif // KERNEL_ITRAJPOCA_H
