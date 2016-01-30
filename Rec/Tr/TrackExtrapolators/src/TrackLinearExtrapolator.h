#ifndef TRACKEXTRAPOLATORS_TRLINEAREXTRAPOLATOR_H 
#define TRACKEXTRAPOLATORS_TRLINEAREXTRAPOLATOR_H 1

// Include files
// -------------
// from TrackExtrapolators
#include "TrackExtrapolator.h"

/** @class TrackLinearExtrapolator TrackLinearExtrapolator.h TrackExtrapolators/TrackLinearExtrapolator.h
 *
 *  A TrackLinearExtrapolator is a ITrackExtrapolator which does a 'linear'
 *  (i.e. straight line) extrapolation of a State. It doesn't take into
 *  account MS.
 *
 *  @author Edwin Bos (added extrapolation method)
 *  @date   05/07/2005
 *
 *  @author Eduardo Rodrigues (changes and new features for new track event model)
 *  @date   25/11/2004
 *
 *  @author Rutger van der Eijk
 *  @date   07-04-1999
 */

class TrackLinearExtrapolator: public TrackExtrapolator {

public:
  /// constructor
  using TrackExtrapolator::TrackExtrapolator;

  using TrackExtrapolator::propagate;

  /// Propagate a state vector from zOld to zNew
  /// Transport matrix is calulated when transMat pointer is not NULL
  StatusCode propagate( Gaudi::TrackVector& stateVec,
                        double zOld,
                        double zNew,
                        Gaudi::TrackMatrix* transMat,
                        LHCb::ParticleID pid = LHCb::ParticleID(211) ) const override;
  
  /// Propagate a state to the closest position to the specified point
  StatusCode propagate( LHCb::State& state,
                        const Gaudi::XYZPoint& point,
                        LHCb::ParticleID pid = LHCb::ParticleID(211) ) const override;
  
};

#endif // TRACKEXTRAPOLATORS_TRLINEAREXTRAPOLATOR_H
