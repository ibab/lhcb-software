#ifndef TRACKPROJECTORS_TRACKSTPROJECTOR_H 
#define TRACKPROJECTORS_TRACKSTPROJECTOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackProjector.h"            // Interface

// from STDet
#include"STDet/DeSTDetector.h"

/** @class TrackSTProjector TrackSTProjector.h TrackProjectors/TrackSTProjector.h
 *  
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-04-08
 */
class TrackSTProjector : public TrackProjector {
public: 
  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual StatusCode project( const State& state,
                              Measurement& meas );

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrackSTProjector( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~TrackSTProjector( ); ///< Destructor

protected:

private:
  DeSTDetector* m_det;

  std::string m_itTrackerPath;   ///< Name of the IT XML geom path
};
#endif // TRACKPROJECTORS_TRACKSTPROJECTOR_H
