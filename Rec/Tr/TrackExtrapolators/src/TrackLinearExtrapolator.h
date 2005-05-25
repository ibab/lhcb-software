// $Id: TrackLinearExtrapolator.h,v 1.3 2005-05-25 14:24:35 cattanem Exp $
#ifndef TRACKEXTRAPOLATORS_TRLINEAREXTRAPOLATOR_H 
#define TRACKEXTRAPOLATORS_TRLINEAREXTRAPOLATOR_H 1

// Include files

// from TrackExtrapolators
#include "TrackExtrapolator.h"

// Forward declaration
class Track;
class State;

/** @class TrackLinearExtrapolator TrackLinearExtrapolator.h TrackExtrapolators/TrackLinearExtrapolator.h
 *
 *  A TrackLinearExtrapolator is a ITrackExtrapolator which does a 'linear'
 *  (i.e. straight line) extrapolation of a State. It doesn't take into
 *  account MS.
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
  TrackLinearExtrapolator( const std::string& type, 
                        const std::string& name, 
                        const IInterface* parent );

  /// destructor
  virtual ~TrackLinearExtrapolator();

  /// Propagate a State to a given z-position
  StatusCode propagate( State& state,
                        double z,
                        ParticleID pid = ParticleID(211) );

  /// Propagate a State to the intersection point with a given plane
  StatusCode propagate( State& state,
                        HepPlane3D& plane,
                        ParticleID pid = ParticleID(211) );
};

#endif // TRACKEXTRAPOLATORS_TRLINEAREXTRAPOLATOR_H
