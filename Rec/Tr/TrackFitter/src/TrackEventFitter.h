// $Id: TrackEventFitter.h,v 1.5 2007-05-03 11:12:07 cattanem Exp $
#ifndef TRACKFITTER_TRACKEVENTFITTER_H 
#define TRACKFITTER_TRACKEVENTFITTER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackFitter.h"

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

  /// interface to tracks fitter tool
  ITrackFitter* m_tracksFitter;

  // job options
  // -----------
  /// Tracks input container path in TES
  std::string m_tracksInContainer;

  /// Tracks output container path in TES (can be the same as input)
  std::string m_tracksOutContainer;

  /// The fitter tool name
  std::string m_fitterName;

  /// Switch in case input and output container are the same or different
  bool m_makeNewContainer;

};
#endif // TRACKFITTER_TRACKEVENTFITTER_H
