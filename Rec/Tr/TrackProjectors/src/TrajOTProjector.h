// $Id: TrajOTProjector.h,v 1.4 2006-02-20 18:37:31 jvantilb Exp $
#ifndef TRACKPROJECTORS_TRAJOTPROJECTOR_H 
#define TRACKPROJECTORS_TRAJOTPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// local
#include "TrackProjector.h"

// Forward declarations
class DeOTDetector;
class IMagneticFieldSvc;
class ITrajPoca;

using namespace LHCb;

/** @class TrajOTProjector TrajOTProjector.h TrajOTProjector.h
 *  
 *  @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
 *  @date   2005-11-24
 */

class TrajOTProjector : public TrackProjector {

public:

  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual StatusCode project( const State& state,
                              Measurement& meas );

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrajOTProjector( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent );

  /// Deestructor
  virtual ~TrajOTProjector();

private:

  DeOTDetector*      m_det;
  std::string        m_otTrackerPath; ///< Name of the OT XML geom path
  IMagneticFieldSvc* m_pIMF;          ///< Pointer to the magn. field service
  ITrajPoca*         m_poca;          ///< Pointer to the ITrajPoca interface

};
#endif // TRACKPROJECTORS_TRAJOTPROJECTOR_H
