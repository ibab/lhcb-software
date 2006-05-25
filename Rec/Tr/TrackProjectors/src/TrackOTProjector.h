// $Id: TrackOTProjector.h,v 1.4 2006-05-25 12:54:33 erodrigu Exp $
#ifndef TRACKPROJECTORS_TRACKOTPROJECTOR_H 
#define TRACKPROJECTORS_TRACKOTPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from OTDet
#include"OTDet/DeOTDetector.h"

// local
#include "TrackProjector.h"

/** @class TrackOTProjector TrackOTProjector.h TrackProjectors/TrackOTProjector.h
 *  
 *  Projects into the OT measurement space
 *
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-04-08
 */

class TrackOTProjector : public TrackProjector {

public:

  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual StatusCode project( const LHCb::State& state,
                              LHCb::Measurement& meas );

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrackOTProjector( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent );

 /// Destructor
  virtual ~TrackOTProjector();

private:

  DeOTDetector* m_det;

  std::string m_otTrackerPath; ///< Name of the OT XML geom path

};
#endif // TRACKPROJECTORS_TRACKOTPROJECTOR_H
