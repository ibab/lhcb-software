// $Id: TrackFastParabolicExtrapolator.h,v 1.2 2005-12-14 14:16:48 erodrigu Exp $
#ifndef TRACKFASTPARABOLICEXTRAPOLATOR_H
#define TRACKFASTPARABOLICEXTRAPOLATOR_H 1

// Include file
#include "TrackParabolicExtrapolator.h"

/** @class TrackFastParabolicExtrapolator TrackFastParabolicExtrapolator.h \
 *         "TrackFastParabolicExtrapolator.h"
 *
 *  A TrackFastParabolicExtrapolator is a TrackExtrapolator which does a 
 *  'parabolic' extrapolation of a State. 
 *  Fast means the transport matrix is calculated approximately.
 *  It doesn't take into account Multiple Scattering.
 *
 *  @author Matt Needham
 *  @date   22-04-2000
 */

class TrackFastParabolicExtrapolator: public TrackParabolicExtrapolator {

public:
  /// Constructor
  TrackFastParabolicExtrapolator( const std::string& type, 
                                  const std::string& name, 
                                  const IInterface* parent);
  /// destructor
  virtual ~TrackFastParabolicExtrapolator();

private:

  /// update tranport matrix (Q/p)
  void updateTransportMatrix( const double dz, 
                              State& pState);

};
 
#endif // TRACKFASTPARABOLICEXTRAPOLATOR_H





