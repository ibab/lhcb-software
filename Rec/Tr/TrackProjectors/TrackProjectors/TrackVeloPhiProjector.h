#ifndef TRACKPROJECTORS_TRACKVELOPHIPROJECTOR_H 
#define TRACKPROJECTORS_TRACKVELOPHIPROJECTOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackProjector.h"            // Interface

// from VeloDet
#include"VeloDet/DeVelo.h"

/** @class TrackVeloPhiProjector TrackVeloPhiProjector.h TrackProjectors/TrackVeloPhiProjector.h
 *  
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-04-13
 */
class TrackVeloPhiProjector : public TrackProjector {
public:
  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual StatusCode project( const State& state,
                              Measurement& meas );

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrackVeloPhiProjector( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);

  virtual ~TrackVeloPhiProjector( ); ///< Destructor

protected:

private:
  DeVelo* m_det;

  std::string m_veloPath;   ///< Name of the Velo XML geom path
};
#endif // TRACKPROJECTORS_TRACKVELOPHIPROJECTOR_H
