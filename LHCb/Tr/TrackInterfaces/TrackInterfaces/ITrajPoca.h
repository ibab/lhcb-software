#ifndef TRACKINTERFACES_ITRAJPOCA_H 
#define TRACKINTERFACES_ITRAJPOCA_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from CLHEP
#include "CLHEP/Geometry/Vector3D.h"

// forward declaration
class Trajectory;

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
  virtual StatusCode minimize( const Trajectory& traj1, const Trajectory& traj2,
                               double& arclength1, double& arclength2,
                               HepVector3D& distance ) const;

  // Retrieve the derivative with respect to the reference point
  // of the first ("1") trajectory
  const HepVector3D derivative1() const;

  // Retrieve the derivative with respect to the reference point
  // of the second ("2") trajectory
  const HepVector3D derivative2() const;

};
#endif // TRACKINTERFACES_ITRAJPOCA_H
