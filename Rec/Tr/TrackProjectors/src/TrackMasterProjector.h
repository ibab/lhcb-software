// $Id: TrackMasterProjector.h,v 1.7 2006-06-15 08:29:26 graven Exp $
#ifndef TRACKPROJECTORS_TRACKMASTERPROJECTOR_H 
#define TRACKPROJECTORS_TRACKMASTERPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackProjector.h"

// from TrackEvent
#include "Event/State.h"
#include "Event/Measurement.h"

/** @class TrackMasterProjector TrackMasterProjector.h TrackProjectors/TrackMasterProjector.h
 *
 *  TrackMasterProjector is the master projector that delegates the projection
 *  to the "measurement-type" projector.
 *
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-03-12
 */

class TrackMasterProjector : public GaudiTool,
                             virtual public ITrackProjector {

public:

  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  StatusCode project( const LHCb::State& state,
                      LHCb::Measurement& meas );
  
  /// Retrieve the projection matrix H of the (last) projection
  const Gaudi::TrackProjectionMatrix& projectionMatrix() const;

  /// Retrieve the chi squared of the (last) projection
  double chi2() const;

  /// Retrieve the residual of the (last) projection
  double residual() const;

  /// Retrieve the error on the residual of the (last) projection
  double errResidual() const;

  /// Retrieve the error on the measurement of the (last) projection
  double errMeasure() const;

  /// Standard constructor
  TrackMasterProjector( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent );

  /// Destructor
  virtual ~TrackMasterProjector( );

  StatusCode initialize();

protected:

  StatusCode selectProjector( unsigned int type );

private:

  std::string m_OTProjName;
  std::string m_STProjName;
  std::string m_VeloRProjName;
  std::string m_VeloPhiProjName;

  std::map<unsigned int,ITrackProjector*> m_projectors;

  unsigned int m_selectedMeasType;

  ITrackProjector*  m_selectedProjector;

};
#endif // TRACKPROJECTORS_TRACKMASTERPROJECTOR_H
