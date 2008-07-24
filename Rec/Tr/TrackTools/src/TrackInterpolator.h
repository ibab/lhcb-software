#ifndef TRACKTOOLS_TRACKINTERPOLATOR_H 
#define TRACKTOOLS_TRACKINTERPOLATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"            
#include "TrackInterfaces/ITrackInterpolator.h"

// From TrackEvent
#include "Event/Track.h"

/** @class TrackInterpolator TrackInterpolator.h
 *
 *  This tool finds the two nearest nodes and interpolates between the nodes
 *  to get the best estimate of an intermediate state at the given z-position.
 *  It extrapolates the two filtered states to the intermediate z-position and
 *  calculated the weighted mean.
 *  The current implemtation also applies the Kalman filter step because only
 *  the result from the prediction step is stored in the node (not the result
 *  of the filtered step).
 *
 *  @author Jeroen van Tilburg
 *  @date   2006-10-06
 */

class TrackInterpolator : public GaudiTool,
                          virtual public ITrackInterpolator {
public:
  /// Standard constructor
  TrackInterpolator( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent );

  virtual ~TrackInterpolator( );  ///< Destructor

  StatusCode initialize();

  /// Interpolate between the two nearest nodes to get a state
  virtual StatusCode interpolate( const LHCb::Track& track,
                                  double z, 
                                  LHCb::State& state );

private:

  /// filter this node
  StatusCode filter( const LHCb::FitNode& node, LHCb::State& state );

  /// extrapolator
  ITrackExtrapolator* m_extrapolator;

  // job options
  std::string m_extrapolatorName;   ///<  name of the extrapolator

};
#endif // TRACKTOOLS_TRACKINTERPOLATOR_H
