// $Id: TrackProjector.h,v 1.7 2007-06-22 13:54:13 ebos Exp $
#ifndef TRACKPROJECTORS_TRACKPROJECTOR_H 
#define TRACKPROJECTORS_TRACKPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackProjector.h"

// from TrackEvent
#include "Event/State.h"
#include "Event/Measurement.h"

/** @class TrackProjector TrackProjector.h TrackProjectors/TrackProjector.h
 *
 *  TrackProjector is the base class implementing methods
 *  from the ITrackProjector interface.
 *
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-03-10
 */

class TrackProjector : public GaudiTool,
                       virtual public ITrackProjector {

public:
  /// Standard constructor
  TrackProjector( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent );

  /// Destructor
  virtual ~TrackProjector();

  /// Retrieve the projection matrix H of the (last) projection
  virtual const Gaudi::TrackProjectionMatrix& projectionMatrix() const;

  /// Retrieve the chi squared of the (last) projection
  double chi2() const;

  /// Retrieve the residual of the (last) projection
  double residual() const;

  /// Retrieve the error on the residual of the (last) projection
  double errResidual() const;

  /// Retrieve the error on the measurement of the (last) projection
  double errMeasure() const;

protected:

  double  m_residual;
  double  m_errResidual;
  double  m_errMeasure;
  Gaudi::TrackProjectionMatrix m_H;
  /// Create StateTraj with or without BField information.
  bool m_useBField;
};
#endif // TRACKPROJECTORS_TRACKPROJECTOR_H
