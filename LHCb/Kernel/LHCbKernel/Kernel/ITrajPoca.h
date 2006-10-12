#ifndef TRACKINTERFACES_ITRAJPOCA_H 
#define TRACKINTERFACES_ITRAJPOCA_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from LHCbDefinitions
#include "Kernel/Vector3DTypes.h"

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

  // Find arclengths along trajectories
  // having a distance smaller than tolerance
  virtual StatusCode minimize( const LHCb::Trajectory& traj1,
                               double& arclength1, 
                               bool restrictRange1,
                               const LHCb::Trajectory& traj2,
                               double& arclength2, 
                               bool restrictRange2,
                               Gaudi::XYZVector& distance,
                               double precision ) = 0;
  
  virtual StatusCode minimize( const LHCb::Trajectory& traj,
                               double& arclength,
                               bool restrictRange,
                               const Gaudi::XYZPoint& pt,
                               Gaudi::XYZVector& distance,
                               double precision ) = 0;

  StatusCode minimize( const LHCb::Trajectory& traj1,
                       double& arclength1, 
                       const LHCb::Trajectory& traj2,
                       double& arclength2,
                       Gaudi::XYZVector& distance,
                       double precision )  
  { return minimize( traj1, arclength1, false,
                     traj2, arclength2, false,
                     distance, precision); }
  
  StatusCode minimize( const LHCb::Trajectory& traj,
                       double& arclength,
                       const Gaudi::XYZPoint& pt,
                       Gaudi::XYZVector& distance, 
                       double precision )
  { return minimize( traj, arclength, false, pt, distance, precision); }

};
#endif // TRACKINTERFACES_ITRAJPOCA_H
