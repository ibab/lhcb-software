// $Id: MuonProjector.h,v 1.1 2007-04-27 20:40:09 polye Exp $
#ifndef TRACKPROJECTORS_MUONPROJECTOR_H 
#define TRACKPROJECTORS_MUONPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackProjector.h"

// from TrackEvent
#include "Event/State.h"
#include "Event/MuonMeasurement.h"

/** @class MuonProjector MuonProjector.h MuonProjectors/MuonProjector.h
 *
 *  MuonProjector is the base class implementing methods
 *  from the IMuonProjector interface.
 *
 *  @author Jose Hernando
 *  @author Miriam Gandelman
 *  @author Erica Polycarpo 
 *  @date   2007-27-04
 */

class MuonProjector : public GaudiTool,
                       virtual public ITrackProjector {

public:
  /// Standard constructor
  MuonProjector( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent );

  /// Destructor
  virtual ~MuonProjector();

  /// Project a state onto a measurement.
  virtual StatusCode project( const LHCb::State& state,
	                            LHCb::Measurement& meas );
  StatusCode project( const LHCb::State& state, 
                            LHCb::MuonMeasurement& meas );

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
  double  m_distx;
  double  m_disty;
  double  m_errx2;
  double  m_erry2;
  double  m_chi2;
  Gaudi::TrackProjectionMatrix m_H;

};
#endif // TRACKPROJECTORS_MUONPROJECTOR_H
