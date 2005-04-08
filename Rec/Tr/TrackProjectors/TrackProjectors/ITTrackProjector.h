// $Id: ITTrackProjector.h,v 1.1 2005-04-08 15:45:46 erodrigu Exp $
#ifndef TRACKPROJECTORS_ITTRACKPROJECTOR_H 
#define TRACKPROJECTORS_ITTRACKPROJECTOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackProjector.h"            // Interface

// from STDet
#include"STDet/DeSTDetector.h"
#include"STDet/STDetectionLayer.h"

/** @class ITTrackProjector ITTrackProjector.h TrackProjectors/ITTrackProjector.h
 *  
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-04-08
 */
class ITTrackProjector : public TrackProjector {
public: 
  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual double project( const State& state,
                          Measurement& meas );

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  ITTrackProjector( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~ITTrackProjector( ); ///< Destructor

protected:

private:
  DeSTDetector* m_det;

};
#endif // TRACKPROJECTORS_ITTRACKPROJECTOR_H
