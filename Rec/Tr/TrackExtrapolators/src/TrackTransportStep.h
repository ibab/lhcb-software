// $Id: TrackTransportStep.h,v 1.1 2005-03-16 14:10:05 hernando Exp $
#ifndef TRACKTRANSPORTSTEP_H 
#define TRACKTRANSPORTSTEP_H 1

// Forward declaration
#include "Tools/ITrackExtrapolator.h"

/** @class TrackTransportStep TrackTransportStep.h "TrackTransportStep.h"
 *
 *  Small (helper class for clever extrapolators).
 *
 *  @author Matt Needham
 *  @date   28-04-2000
 */

class TrackTransportStep{ 

public:

  /// constructer
  TrackTransportStep( ITrackExtrapolator* extrapolator, double zStop)
    : m_extrapolator( extrapolator )
    , m_Z( zStop ) 
  { 
  };

  /// destructer
  ~TrackTransportStep(){};

  /// get extrapolator to use
  ITrackExtrapolator* extrapolator() const;

  /// get target z
  double z() const;

private:

  ITrackExtrapolator* m_extrapolator;  ///< extrapolator to use
  double m_Z;                      ///< z to extrapolate to

};

inline ITrackExtrapolator* TrackTransportStep::extrapolator() const{
  return m_extrapolator;
}

inline double TrackTransportStep::z() const{
  return m_Z;
}

#endif // TRACKTRANSPORTSTEP_H











