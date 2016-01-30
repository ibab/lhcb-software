#ifndef TRACKPARABOLICEXTRAPOLATOR_H
#define TRACKPARABOLICEXTRAPOLATOR_H 1

// Include files
#include "TrackFieldExtrapolatorBase.h"
#include <string>

// Forward declaration
class IMagneticFieldSvc;


/** @class TrackParabolicExtrapolator TrackParabolicExtrapolator.h \
 *         "TrackParabolicExtrapolator.h"
 *
 *  A TrackParabolicExtrapolator is a ITrackExtrapolator that does a transport 
 *  using a parabolic expansion of the trajectory. It doesn't take into
 *  account Multiple Scattering.
 *
 *  @author Edwin Bos (added extrapolation methods)
 *  @date   05/07/2005
 *  @author Jose A. Hernando (13-03-2005)
 *  @author Matt Needham
 *  @date   22-04-2000
 */

class TrackParabolicExtrapolator: public TrackFieldExtrapolatorBase
{

public:
  /// Constructor
  TrackParabolicExtrapolator( const std::string& type, 
                              const std::string& name, 
                              const IInterface* parent);
  
  /// Destructor
  ~TrackParabolicExtrapolator() override;

  using TrackExtrapolator::propagate;
  
  /// Propagate a state vector from zOld to zNew
  /// Transport matrix is calulated when transMat pointer is not NULL
  StatusCode propagate( Gaudi::TrackVector& stateVec,
                        double zOld,
                        double zNew,
                        Gaudi::TrackMatrix* transMat,
                        LHCb::ParticleID pid = LHCb::ParticleID(211) ) const  override;
  
  /// Propagate a state to the closest position to the specified point
  StatusCode propagate( LHCb::State& state,
                        const Gaudi::XYZPoint& point,
                        LHCb::ParticleID pid = LHCb::ParticleID(211) ) const  override;

private:

  /// update transport matrix
  void updateTransportMatrix( const double dz,
			      Gaudi::TrackVector& stateVec,
			      Gaudi::TrackMatrix& transMat,
			      const Gaudi::XYZVector& B,
			      double ax, double ay) const ;
};

#endif // TRACKPARABOLICEXTRAPOLATOR_H
