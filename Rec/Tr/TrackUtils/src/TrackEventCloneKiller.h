// $Id: TrackEventCloneKiller.h,v 1.5 2008-05-06 10:01:10 aperiean Exp $
#ifndef TRACKEVENTCLONEKILLER_H 
#define TRACKEVENTCLONEKILLER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackCloneFinder.h"

/** @class TrackEventCloneKiller TrackEventCloneKiller.h
 *  
 *
 *  @author Eduardo Rodrigues
 *  @date   2006-03-01
 *  Based on the clone killer algorithm of Rutger van der Eijk (2002-06-17)
 *  Update for speed and clone rate
 *  @author Adrian Perieanu
 *  @date   2008-05-05
 */
class TrackEventCloneKiller : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TrackEventCloneKiller( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackEventCloneKiller( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  
private:
  ITrackCloneFinder* m_cloneFinder; ///< interface to clone finder tool
  bool m_debugLevel;

  // Retrieve the input tracks from specified containers
  void getInputTracks( std::vector<LHCb::Track*>& allTracks);

  // job options
  // -----------
  // input Track container paths
  std::vector<std::string> m_tracksInContainers;
  // output Track container path
  std::string m_tracksOutContainer;
  // list of track types not to be stored
  std::vector<int> m_ignoredTrackTypes;
  // flags whether to store all tracks including clones
  bool m_storeCloneTracks;
  // do not do a clone compare for tracks from a same container
  bool m_skipSameContainerTracks;
  bool m_compareInSameContainerForwardUpstream;
  // clone finder tool name
  std::string m_cloneFinderName;

};
#endif // TRACKEVENTCLONEKILLER_H
