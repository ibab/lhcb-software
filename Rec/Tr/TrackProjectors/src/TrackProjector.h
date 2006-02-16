// $Id: TrackProjector.h,v 1.2 2006-02-16 10:50:31 ebos Exp $
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

using namespace Gaudi;
using namespace LHCb;

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
  virtual StatusCode project( const State& state,
                              Measurement& meas );
  
  /// Retrieve the projection matrix H of the (last) projection
  virtual const TrackVector& projectionMatrix() const;

  /// Retrieve the chi squared of the (last) projection
  double chi2() const;

  /// Retrieve the residual of the (last) projection
  double residual() const;

  /// Retrieve the error on the residual of the (last) projection
  double errResidual() const;

  /// Standard constructor
  TrackProjector( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent );

  /// Destructor
  virtual ~TrackProjector();

protected:

  void computeResidual( const State& state, const Measurement& meas );

  void computeErrorResidual( const State& state, const Measurement& meas );

  double  m_residual;
  double  m_errResidual;
  TrackVector m_H;

};
#endif // TRACKPROJECTORS_TRACKPROJECTOR_H
