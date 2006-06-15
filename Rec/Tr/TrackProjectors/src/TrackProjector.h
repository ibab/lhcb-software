// $Id: TrackProjector.h,v 1.5 2006-06-15 08:29:26 graven Exp $
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

  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual StatusCode project( const LHCb::State& state,
                              LHCb::Measurement& meas );
  
  /// Retrieve the projection matrix H of the (last) projection
  virtual const Gaudi::TrackProjectionMatrix& projectionMatrix() const;

  /// Retrieve the chi squared of the (last) projection
  double chi2() const;

  /// Retrieve the residual of the (last) projection
  double residual() const;

  /// Retrieve the error on the residual of the (last) projection
  double errResidual() const;

  /// Retrieve the error on the measurement of the (last) projection
  double errMeasure() const { return m_errMeasure; };

  /// Standard constructor
  TrackProjector( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent );

  /// Destructor
  virtual ~TrackProjector();

protected:

  void computeResidual( const LHCb::State& state,
                        const LHCb::Measurement& meas );

  void computeErrorResidual( const LHCb::State& state,
                             const LHCb::Measurement& meas );

  double  m_residual;
  double  m_errResidual;
  double  m_errMeasure;
  Gaudi::TrackProjectionMatrix m_H;

};
#endif // TRACKPROJECTORS_TRACKPROJECTOR_H
