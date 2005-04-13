#ifndef TRACKPROJECTORS_TRACKITPROJECTOR_H 
#define TRACKPROJECTORS_TRACKITPROJECTOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackProjector.h"            // Interface

// from STDet
#include"STDet/DeSTDetector.h"
#include"STDet/STDetectionLayer.h"

/** @class TrackITProjector TrackITProjector.h TrackProjectors/TrackITProjector.h
 *  
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-04-08
 */
class TrackITProjector : public TrackProjector {
public: 
  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual double project( const State& state,
                          Measurement& meas );

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrackITProjector( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~TrackITProjector( ); ///< Destructor

protected:

private:
  DeSTDetector* m_det;

  std::string m_itTrackerPath;   ///< Name of the IT XML geom path
};
#endif // TRACKPROJECTORS_TRACKITPROJECTOR_H
