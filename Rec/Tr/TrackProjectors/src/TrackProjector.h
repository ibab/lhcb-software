// $Id: TrackProjector.h,v 1.9 2008-10-03 13:58:22 wouter Exp $
#ifndef TRACKPROJECTORS_TRACKPROJECTOR_H 
#define TRACKPROJECTORS_TRACKPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackProjector.h"

class IMagneticFieldSvc;
class ITrajPoca;

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

  /// ::initialize
  StatusCode initialize() ;

  /// Project the reference vector
  virtual StatusCode project( const LHCb::StateVector& state, const LHCb::Measurement& meas ) ;

  /// Project a state onto a measurement.
  virtual StatusCode project( const LHCb::State& state, const LHCb::Measurement& meas ) ;

  /// Project the state vector in this fitnode and update projection matrix and reference residual
  virtual StatusCode projectReference( LHCb::FitNode& node ) const ;

  /// Retrieve the derivative of the residual wrt. the alignment parameters
  /// of the measurement. The details of the alignment transformation are 
  /// defined in AlignTraj.
  virtual Derivatives alignmentDerivatives(const LHCb::StateVector& state, 
					   const LHCb::Measurement& meas,
                                           const Gaudi::XYZPoint& pivot) const ;

  /// Retrieve the projection matrix H of the (last) projection
  const Gaudi::TrackProjectionMatrix& projectionMatrix() const { return m_H ; }

  /// Retrieve the chi squared of the (last) projection
  double chi2() const;

  /// Retrieve the residual of the (last) projection
  double residual() const { return m_residual ; }

  /// Retrieve the error on the residual of the (last) projection
  double errResidual() const { return m_errResidual; }

  /// Retrieve the error on the measurement of the (last) projection
  double errMeasure() const { return m_errMeasure ; }

protected:
  mutable double m_residual;
  mutable double m_errResidual;
  mutable double m_errMeasure;
  mutable Gaudi::TrackProjectionMatrix m_H;
  mutable double m_sMeas  ;
  mutable double m_sState ;
  mutable double m_doca ;
  mutable Gaudi::XYZVector m_unitPocaVector ;
  bool m_useBField;                   /// Create StateTraj with or without BField information.
  double             m_tolerance;     ///< Required accuracy of the projection
  IMagneticFieldSvc* m_pIMF; ///< Pointer to the magn. field service
  ITrajPoca*         m_poca; ///< Pointer to the ITrajPoca interface
  
};
#endif // TRACKPROJECTORS_TRACKPROJECTOR_H
