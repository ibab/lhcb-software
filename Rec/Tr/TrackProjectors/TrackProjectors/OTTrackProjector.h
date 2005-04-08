// $Id: OTTrackProjector.h,v 1.1 2005-04-08 15:45:46 erodrigu Exp $
#ifndef TRACKPROJECTORS_OTTRACKPROJECTOR_H 
#define TRACKPROJECTORS_OTTRACKPROJECTOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackProjector.h"            // Interface

// from OTDet
#include"OTDet/DeOTDetector.h"

/** @class OTTrackProjector OTTrackProjector.h TrackProjectors/OTTrackProjector.h
 *  
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-04-08
 */
class OTTrackProjector : public TrackProjector {
public:
  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual double project( const State& state,
                          Measurement& meas );

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  OTTrackProjector( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~OTTrackProjector( ); ///< Destructor

protected:

private:
  DeOTDetector* m_det;

};
#endif // TRACKPROJECTORS_OTTRACKPROJECTOR_H
