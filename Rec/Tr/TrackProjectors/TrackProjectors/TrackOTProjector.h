#ifndef TRACKPROJECTORS_TRACKOTPROJECTOR_H 
#define TRACKPROJECTORS_TRACKOTPROJECTOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackProjector.h"            // Interface

// from OTDet
#include"OTDet/DeOTDetector.h"

/** @class TrackOTProjector TrackOTProjector.h TrackProjectors/TrackOTProjector.h
 *  
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-04-08
 */
class TrackOTProjector : public TrackProjector {
public:
  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual double project( const State& state,
                          Measurement& meas );

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrackOTProjector( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~TrackOTProjector( ); ///< Destructor

protected:

private:
  DeOTDetector* m_det;

  std::string m_otTrackerPath;   ///< Name of the OT XML geom path
};
#endif // TRACKPROJECTORS_TRACKOTPROJECTOR_H
