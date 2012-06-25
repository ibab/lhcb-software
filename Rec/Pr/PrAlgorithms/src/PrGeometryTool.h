#ifndef PRGEOMETRYTOOL_H 
#define PRGEOMETRYTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ILHCbMagnetSvc.h"
#include "PrSeedTrack.h"
#include "PrForwardTrack.h"

static const InterfaceID IID_PrGeometryTool ( "PrGeometryTool", 1, 0 );

/** @class PrGeometryTool PrGeometryTool.h
 *  Holds the geometry information, namely the field parameterisation
 *
 *  @author Olivier Callot
 *  @date   2012-03-22
 */
class PrGeometryTool : public GaudiTool {
public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PrGeometryTool; }

  /// Standard constructor
  PrGeometryTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  virtual ~PrGeometryTool( ); ///< Destructor

  virtual StatusCode initialize();

  float xAtReferencePlane( PrForwardTrack& track, PrHit* hit );  ///< computes teh projection to the hough plane

  void  setTrackParameters( PrForwardTrack& track, float xAtRef ); ///< Compute the track parameters for a given x

  float qOverP( const PrForwardTrack& track );       ///< Momentum estimate for long tracks

  float qOverP( const PrSeedTrack& track );          ///< Momentum estimate for seed

  float zMagnet( const PrForwardTrack& track );      ///< Initial value for teh z at centre of magnet

  float zReference() const { return m_zReference; }  ///< fixed z for all parameterizations

  Gaudi::TrackSymMatrix covariance( float qOverp );  ///< Default (large) covariance matrix

protected:

private:
  ILHCbMagnetSvc*    m_magFieldSvc;
  std::vector<float> m_zMagnetParams;
  std::vector<float> m_xParams;
  std::vector<float> m_yParams;
  std::vector<float> m_momentumParams;
  std::vector<float> m_covarianceValues;

  float              m_zReference;
};
#endif // PRGEOMETRYTOOL_H
