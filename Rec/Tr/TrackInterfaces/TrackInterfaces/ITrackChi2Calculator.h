#ifndef TRACKINTERFACES_ITRACKCHI2CALCULATOR_H 
#define TRACKINTERFACES_ITRACKCHI2CALCULATOR_H 1

// Include files
// -------------
#include "GaudiKernel/IAlgTool.h"

// From TrackEvent
#include "Event/TrackTypes.h"

/** @class ITrackChi2Calculator ITrackChi2Calculator.h TrackInterfaces/ITrackChi2Calculator.h
 * 
 *  Interface class for the TrackChi2Calculator tool.
 *  This is used to calculate the chi2-distance between to track states.
 *
 *  @author Jeroen van Tilburg
 *  @date   2003-09-18
 */

static const InterfaceID IID_ITrackChi2Calculator( "ITrackChi2Calculator", 1, 0 );

class ITrackChi2Calculator : virtual public IAlgTool 
{
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackChi2Calculator; }

  /** Calculate the chi2 distance between two track vectors.
   *  The track vectors must be given in (x,y,tx,ty,q/p).
   *  @return StatusCode:   Failure if matrix inversion failed  
   *  @param  trackVector1: input 1st track HepVector
   *  @param  trackCov1:    input covariance matrix corresponding to 1st vector
   *  @param  trackVector2: input 2nd track HepVector
   *  @param  trackCov2:    input covariance matrix corresponding to 2nd vector
   *  @param  chi2:         output chi2 distance between the two vectors
   */
  virtual StatusCode calculateChi2( const Gaudi::TrackVector& trackVector1,
                                    const Gaudi::TrackSymMatrix& trackCov1,
                                    const Gaudi::TrackVector& trackVector2,
                                    const Gaudi::TrackSymMatrix& trackCov2,
                                    double& chi2 ) const = 0;

};

#endif // TRACKINTERFACES_ITRACKCHI2CALCULATOR_H
