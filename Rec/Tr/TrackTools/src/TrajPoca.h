#ifndef TRACKTOOLS_TRAJPOCA_H 
#define TRACKTOOLS_TRAJPOCA_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Tr/TrackInterfaces
#include "TrackInterfaces/ITrajPoca.h"

// from Event/TrackEvent
#include "Event/Trajectory.h"

static const InterfaceID IID_TrajPoca ( "TrajPoca", 1, 0 );

/** @class TrajPoca TrajPoca.h
 *  
 *  Calculates points of closest approach
 *  between two trajectories
 *
 *  @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
 *  @date   2005-11-23
 */

class TrajPoca : public GaudiTool,
                 virtual public ITrajPoca {

public:
  // Standard constructor
  TrajPoca( const std::string& type, 
            const std::string& name,
            const IInterface* parent );

  virtual ~TrajPoca( ); ///< Destructor

  // Tool initialization
  virtual StatusCode initialize();

  // Tool finalization
  virtual StatusCode finalize();

  // Find arclengths along trajectories
  // having a distance smaller than tolerance
  StatusCode minimize( const Trajectory& traj1, const Trajectory& traj2,
                       double& arclength1, double& arclength2,
                       HepVector3D& distance );

  // Retrieve the derivative with respect to the reference point
  // of the first ("1") trajectory
  const HepVector3D derivative1() const; 

  // Retrieve the derivative with respect to the reference point
  // of the second ("2") trajectory
  const HepVector3D derivative2() const;

private:
  HepVector3D m_distance;

  // jobOptions
  double m_tolerance;

};

inline const HepVector3D TrajPoca::derivative1( ) const {
  return HepVector3D( m_distance.unit() );
}

inline const HepVector3D TrajPoca::derivative2( ) const {
  return HepVector3D( -m_distance.unit() );
}

#endif // TRACKTOOLS_TRAJPOCA_H


