// $Id: TrajVeloRProjector.h,v 1.1 2006-02-27 19:56:04 jvantilb Exp $
#ifndef TRAJVELORPROJECTOR_H 
#define TRAJVELORPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackProjector.h"

class IMagneticFieldSvc;
class ITrajPoca;

using namespace LHCb;

/** @class TrajVeloRProjector TrajVeloRProjector.h TrajVeloRProjector.h
 *  
 *  Projects into the Velo r measurement space
 *
 *  @author Jeroen van Tilburg
 *  @date   2006-02-21
 */

class TrajVeloRProjector : public TrackProjector {

public: 

  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual StatusCode project( const State& state,
                              Measurement& meas );

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrajVeloRProjector( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent );
  /// Destructor
  virtual ~TrajVeloRProjector();

private:

  IMagneticFieldSvc* m_pIMF; ///< Pointer to the magn. field service
  ITrajPoca*         m_poca; ///< Pointer to the ITrajPoca interface

};
#endif // TRAJVELORPROJECTOR_H
