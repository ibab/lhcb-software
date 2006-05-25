// $Id: TrajVeloRProjector.h,v 1.3 2006-05-25 12:54:33 erodrigu Exp $
#ifndef TRAJVELORPROJECTOR_H 
#define TRAJVELORPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackProjector.h"

class IMagneticFieldSvc;
class ITrajPoca;

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
  virtual StatusCode project( const LHCb::State& state,
                              LHCb::Measurement& meas );

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

  // job option
  double             m_tolerance;     ///< Required accuracy of the projection

};
#endif // TRAJVELORPROJECTOR_H
