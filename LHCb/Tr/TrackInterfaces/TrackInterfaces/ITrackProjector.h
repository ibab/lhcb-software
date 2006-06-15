#ifndef TRACKINTERFACES_ITRACKPROJECTOR_H 
#define TRACKINTERFACES_ITRACKPROJECTOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Kernel/TrackTypes.h"

// Forward declarations
namespace LHCb {
  class State;
  class Measurement;
};

static const InterfaceID IID_ITrackProjector ( "ITrackProjector", 1, 0 );

/** @class ITrackProjector ITrackProjector.h
 *  
 *  Interface for tracking projector tools
 *
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-03-15
 */
class ITrackProjector : virtual public IAlgTool {
public:
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackProjector; }

  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual StatusCode project( const LHCb::State& state,
                              LHCb::Measurement& meas ) = 0;

  /// Retrieve the projection matrix H of the (last) projection
  virtual const Gaudi::TrackProjectionMatrix& projectionMatrix() const = 0;

  /// Retrieve the chi squared of the (last) projection
  virtual double chi2() const = 0;

  /// Retrieve the residual of the (last) projection
  virtual double residual() const = 0;

  /// Retrieve the error on the residual of the (last) projection
  virtual double errResidual() const = 0;

  /// Retrieve the error on the measure of the (last) projection
  virtual double errMeasure() const = 0;

};

//==============================================================================
//   end of class
//==============================================================================

#endif // TRACKINTERFACES_ITRACKPROJECTOR_H
