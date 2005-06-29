// $Id: TrackEventFitter.h,v 1.1 2005-06-29 15:35:02 erodrigu Exp $
#ifndef TRACKFITTER_TRACKEVENTFITTER_H 
#define TRACKFITTER_TRACKEVENTFITTER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/IMeasurementProvider.h"

/** @class TrackEventFitter TrackEventFitter.h
 *  
 *
 *  @author Eduardo Rodrigues
 *  @date   2005-05-30
 */
class TrackEventFitter : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TrackEventFitter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackEventFitter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  /// Get a seed State from the Track
  /// (State with highest/lowest z in case of upstream/downstream fitting)
  State& seedState( Track& track );

  /// Register the tracks in the Transient event store
  StatusCode registerTracks( Tracks* tracksCont );

  /// interface to tracks fitter tool
  ITrackFitter* m_tracksFitter;

  /// measurement provider tool
  IMeasurementProvider* m_measProvider;

  // job options
  // -----------
  /// Tracks input container path in TES
  std::string m_tracksInContainer;

  /// Tracks output container path in TES
  std::string m_tracksOutContainer;

  /// The fitter tool name
  std::string m_fitterName;

  /// Perform the fit upstream/downstream?
  bool m_fitUpstream;

};
#endif // TRACKFITTER_TRACKEVENTFITTER_H
