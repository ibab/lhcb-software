// $Id: TrajSTProjector.h,v 1.1 2006-02-21 18:28:25 jvantilb Exp $
#ifndef TRAJSTPROJECTOR_H 
#define TRAJSTPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackProjector.h"

// from STDet
#include"STDet/DeSTDetector.h"

class IMagneticFieldSvc;
class ITrajPoca;
class ISTClusterPosition;

using namespace LHCb;

/** @class TrajSTProjector TrajSTProjector.h TrajSTProjector.h
 *  
 *  Projects into the ST measurement space
 *
 *  @author Jeroen van Tilburg
 *  @date   2006-02-21
 */

class TrajSTProjector : public TrackProjector {

public: 

  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual StatusCode project( const State& state,
                              Measurement& meas );

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrajSTProjector( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent );
  /// Destructor
  virtual ~TrajSTProjector();

private:

  DeSTDetector* m_itdet;
  DeSTDetector* m_ttdet;

  std::string m_ttTrackerPath;           ///< Name of the TT XML geom path
  std::string m_itTrackerPath;           ///< Name of the IT XML geom path
  IMagneticFieldSvc* m_pIMF;             ///< Pointer to the magn. field service
  ITrajPoca*         m_poca;             ///< Pointer to the ITrajPoca interface
  ISTClusterPosition* m_stPositionTool ; ///< STClusterPosition tool

};
#endif // TRAJSTPROJECTOR_H
