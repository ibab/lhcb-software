// $Id: TrLinearExtrapolator.h,v 1.1.1.1 2005-02-10 19:04:30 erodrigu Exp $
#ifndef TRACKEXTRAPOLATORS_TRLINEAREXTRAPOLATOR_H 
#define TRACKEXTRAPOLATORS_TRLINEAREXTRAPOLATOR_H 1

// Include files

// from TrExtrapolators
#include "TrackExtrapolators/TrExtrapolator.h"

// Forward declaration
class Track;
class State;

/** @class TrLinearExtrapolator TrLinearExtrapolator.h TrExtrapolators/TrLinearExtrapolator.h
 *
 *  A TrLinearExtrapolator is a ITrExtrapolator which does a 'linear'
 *  (i.e. straight line) extrapolation of a State. It doesn't take into
 *  account MS.
 *
 *  @author Eduardo Rodrigues (changes and new features for new track event model)
 *  @date   25/11/2004
 *
 *  @author Rutger van der Eijk
 *  @date   07-04-1999
 */

class TrLinearExtrapolator: public TrExtrapolator {

public:
  /// constructor
  TrLinearExtrapolator( const std::string& type, 
                        const std::string& name, 
                        const IInterface* parent );

  /// destructor
  virtual ~TrLinearExtrapolator();

  /// Propagate a State to a given z-position
  StatusCode propagate( State& state,
                        double z,
                        ParticleID pid = ParticleID(211) );

  /// Propagate a State to the intersection point with a given plane
  StatusCode propagate( State& state,
                        HepPlane3D& plane,
                        ParticleID pid = ParticleID(211) );

private:

  /// transport matrix
  HepMatrix m_F;
};

#endif // TRACKEXTRAPOLATORS_TRLINEAREXTRAPOLATOR_H
