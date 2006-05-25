// $Id: TrackSTProjector.h,v 1.3 2006-05-25 12:54:33 erodrigu Exp $
#ifndef TRACKPROJECTORS_TRACKSTPROJECTOR_H 
#define TRACKPROJECTORS_TRACKSTPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackProjector.h"

// from STDet
#include"STDet/DeSTDetector.h"

/** @class TrackSTProjector TrackSTProjector.h TrackProjectors/TrackSTProjector.h
 *  
 *  Projects into the ST measurement space
 *
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-04-08
 */

class TrackSTProjector : public TrackProjector {

public: 

  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual StatusCode project( const LHCb::State& state,
                              LHCb::Measurement& meas );

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrackSTProjector( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent );
  /// Destructor
  virtual ~TrackSTProjector();

private:

  DeSTDetector* m_itdet;
  DeSTDetector* m_ttdet;

  std::string m_ttTrackerPath;   ///< Name of the TT XML geom path
  std::string m_itTrackerPath;   ///< Name of the IT XML geom path

};
#endif // TRACKPROJECTORS_TRACKSTPROJECTOR_H
