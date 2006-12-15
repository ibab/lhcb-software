// $Id: TrajOTProjector.h,v 1.8 2006-12-15 19:11:39 graven Exp $
#ifndef TRAJOTPROJECTOR_H 
#define TRAJOTPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/GenericMatrixTypes.h"

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

  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual StatusCode project( const LHCb::State& state,
                              LHCb::Measurement& meas );
  StatusCode project( const LHCb::State& state,
                      LHCb::OTMeasurement& meas );

  typedef Gaudi::Matrix1x6 Derivatives;
  Derivatives alignmentDerivatives(const LHCb::Measurement& meas, const Gaudi::XYZPoint& pivot) const;

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrajOTProjector( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent );

  /// Deestructor
  virtual ~TrajOTProjector();

private:
  double driftDistance(const LHCb::OTMeasurement& meas, double arclen ) const;

  DeOTDetector*      m_det;
  IMagneticFieldSvc* m_pIMF;          ///< Pointer to the magn. field service
  ITrajPoca*         m_poca;          ///< Pointer to the ITrajPoca interface

  // job options
  std::string        m_otTrackerPath; ///< Name of the OT XML geom path
  double             m_tolerance;     ///< Required accuracy of the projection

};
#endif // TRACKPROJECTORS_TRAJOTPROJECTOR_H
