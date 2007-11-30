// $Id: TrajOTProjector.h,v 1.12 2007-11-30 14:33:17 wouter Exp $
#ifndef TRAJOTPROJECTOR_H 
#define TRAJOTPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/GenericMatrixTypes.h"

// local
#include "TrackProjector.h"

// Forward declarations
class DeOTDetector;
class IMagneticFieldSvc;
class ITrajPoca;

/** @class TrajOTProjector TrajOTProjector.h TrajOTProjector.h
 *  
 *  @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
 *  @date   2005-11-24
 */

class TrajOTProjector : public TrackProjector {

public:

  /// Project a state-vector onto a measurement
  StatusCode project( const LHCb::StateVector& state, const LHCb::Measurement& meas );

  /// Project a state onto a measurement.
  StatusCode project( const LHCb::State& state, const LHCb::Measurement& meas );
  
  typedef Gaudi::Matrix1x6 Derivatives;
  Derivatives alignmentDerivatives(const LHCb::StateVector& state, const LHCb::Measurement& meas, 
                                   const Gaudi::XYZPoint& pivot) const;

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrajOTProjector( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent );

  /// Deestructor
  virtual ~TrajOTProjector();

private:
  StatusCode project( const LHCb::StateVector& state, const LHCb::OTMeasurement& meas );

private:
  DeOTDetector*      m_det;
  IMagneticFieldSvc* m_pIMF;          ///< Pointer to the magn. field service
  ITrajPoca*         m_poca;          ///< Pointer to the ITrajPoca interface

  // job options
  std::string        m_otTrackerPath; ///< Name of the OT XML geom path
  double             m_tolerance;     ///< Required accuracy of the projection
  bool               m_useDrift;      ///< Use measured drift distance

};
#endif // TRACKPROJECTORS_TRAJOTPROJECTOR_H
