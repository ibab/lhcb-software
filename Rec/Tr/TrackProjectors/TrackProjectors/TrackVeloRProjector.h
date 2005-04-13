#ifndef TRACKPROJECTORS_TRACKVELORPROJECTOR_H 
#define TRACKPROJECTORS_TRACKVELORPROJECTOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackProjector.h"            // Interface

// from VeloDet
#include"VeloDet/DeVelo.h"

/** @class TrackVeloRProjector TrackVeloRProjector.h TrackProjectors/TrackVeloRProjector.h
 *  
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-04-13
 */
class TrackVeloRProjector : public TrackProjector {
public:
  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual double project( const State& state,
                          Measurement& meas );

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrackVeloRProjector( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~TrackVeloRProjector( ); ///< Destructor

protected:

private:
  DeVelo* m_det;

  std::string m_veloPath;   ///< Name of the Velo XML geom path
};
#endif // TRACKPROJECTORS_TRACKVELORPROJECTOR_H
