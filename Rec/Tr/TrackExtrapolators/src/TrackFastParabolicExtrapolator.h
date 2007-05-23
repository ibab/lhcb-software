// $Id: TrackFastParabolicExtrapolator.h,v 1.7 2007-05-23 16:43:22 ebos Exp $
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
                              Gaudi::TrackVector& stateVec,
                              Gaudi::TrackMatrix& transMat );

};
 
#endif // TRACKFASTPARABOLICEXTRAPOLATOR_H
