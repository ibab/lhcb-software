// $Id: TrajOTProjector.h,v 1.17 2009-09-02 15:38:57 wouter Exp $
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
  
  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrajOTProjector( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent );

  /// Deestructor
  virtual ~TrajOTProjector();

protected:
  StatusCode project( const LHCb::StateVector& state, const LHCb::OTMeasurement& meas );
  bool useDriftTime() const { return m_useDriftTime ; }
  bool skipDriftTimeZeroAmbiguity() const { return m_skipDriftTimeZeroAmbiguity ; }
  bool fitDriftTime() const { return m_fitDriftTime ; }
private:
  bool m_useDriftTime ;  ///< Use measured drift time 
  bool m_fitDriftTime ;  ///< Fit drift times residuals instead of 'distance' residuals
  bool m_updateAmbiguity ;
  bool m_skipDriftTimeZeroAmbiguity ;
};
#endif // TRACKPROJECTORS_TRAJOTPROJECTOR_H
