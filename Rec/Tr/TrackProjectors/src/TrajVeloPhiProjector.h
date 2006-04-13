// $Id: TrajVeloPhiProjector.h,v 1.2 2006-04-13 10:41:05 jvantilb Exp $
#ifndef TRAJVELOPHIPROJECTOR_H 
#define TRAJVELOPHIPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackProjector.h"

class IMagneticFieldSvc;
class ITrajPoca;

using namespace LHCb;

/** @class TrajVeloPhiProjector TrajVeloPhiProjector.h TrajVeloPhiProjector.h
 *  
 *  Projects into the Velo phi measurement space
 *
 *  @author Jeroen van Tilburg
 *  @date   2006-02-21
 */

class TrajVeloPhiProjector : public TrackProjector {

public: 

  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual StatusCode project( const State& state,
                              Measurement& meas );

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrajVeloPhiProjector( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent );
  /// Destructor
  virtual ~TrajVeloPhiProjector();

private:

  IMagneticFieldSvc* m_pIMF; ///< Pointer to the magn. field service
  ITrajPoca*         m_poca; ///< Pointer to the ITrajPoca interface

  // job option
  double             m_tolerance;     ///< Required accuracy of the projection

};
#endif // TRAJVELOPHIPROJECTOR_H
