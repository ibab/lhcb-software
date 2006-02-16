// $Id: TrackVeloPhiProjector.h,v 1.2 2006-02-16 10:50:53 ebos Exp $
#ifndef TRACKPROJECTORS_TRACKVELOPHIPROJECTOR_H 
#define TRACKPROJECTORS_TRACKVELOPHIPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// local
#include "TrackProjector.h"

// from VeloDet
#include"VeloDet/DeVelo.h"

using namespace LHCb;

/** @class TrackVeloPhiProjector TrackVeloPhiProjector.h TrackProjectors/TrackVeloPhiProjector.h
 *  
 *  Projects into VeloPhi measurement space
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
                         const IInterface* parent );

  /// Destructor
  virtual ~TrackVeloPhiProjector();

private:

  DeVelo* m_det;

  std::string m_veloPath;   ///< Name of the Velo XML geom path

};
#endif // TRACKPROJECTORS_TRACKVELOPHIPROJECTOR_H
