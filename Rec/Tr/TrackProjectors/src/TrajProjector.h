// $Id: TrajProjector.h,v 1.5 2007-11-30 14:32:44 wouter Exp $
#ifndef TRAJPROJECTOR_H 
#define TRAJPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"

#include "TrackProjector.h"

class IMagneticFieldSvc;
class ITrajPoca;

/** @class TrajProjector TrajProjector.h TrajProjector.h
 *  
 *  Projects into the measurement space
 *
 *  @author Jeroen van Tilburg
 *  @date   2006-02-21
 */

template <typename T>
class TrajProjector : public TrackProjector {

public: 
    /// FIXME/TODO: project given a traj instead of a state

  /// Project a state-vector onto a measurement
  StatusCode project( const LHCb::StateVector& state, const LHCb::Measurement& meas );

  /// Project a state onto a measurement
  StatusCode project( const LHCb::State& state, const LHCb::Measurement& meas );

  ITrackProjector::Derivatives alignmentDerivatives(const LHCb::StateVector& state,
                                                    const LHCb::Measurement& meas, const Gaudi::XYZPoint& pivot) const;

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrajProjector( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent );
  /// Destructor
  virtual ~TrajProjector();

private:

  IMagneticFieldSvc* m_pIMF; ///< Pointer to the magn. field service
  ITrajPoca*         m_poca; ///< Pointer to the ITrajPoca interface

  // job option
  double             m_tolerance;     ///< Required accuracy of the projection

};

#endif // TRAJPROJECTOR_H
