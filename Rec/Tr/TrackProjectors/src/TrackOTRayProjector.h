#ifndef TRACKPROJECTORS_TRACKOTRAYPROJECTOR_H 
#define TRACKPROJECTORS_TRACKOTRAYPROJECTOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackProjector.h"            // Interface
#include "RayIntersector.h"

// from OTDet
#include"OTDet/DeOTDetector.h"

/** @class TrackOTRayProjector TrackOTRayProjector.h TrackProjectors/TrackOTRayProjector.h
 *  
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-11-10
 */
class TrackOTRayProjector : public TrackProjector {
public:
  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual StatusCode project( const State& state,
                              Measurement& meas );

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrackOTRayProjector( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~TrackOTRayProjector( ); ///< Destructor

protected:

private:

  DeOTDetector* m_det;

  RayIntersector m_intersector;

  std::string m_otTrackerPath;   ///< Name of the OT XML geom path
};
#endif // TRACKPROJECTORS_TRACKOTRAYPROJECTOR_H
