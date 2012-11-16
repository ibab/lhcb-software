#ifndef TRACKFROMDST_H 
#define TRACKFROMDST_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from TrackEvent
#include "Event/Track.h"

typedef std::map< const LHCb::Track::History,
                  std::vector<LHCb::Track*> > TracksMap;

/** @class TrackFromDST TrackFromDST.h
 *  
 *  Algorithm to classify the tracks given as input according to
 *  their History / pattern recognition algorithms.
 *  Typically, this algorithm takes the tracks from the "best" container
 *  and remakes the original containers that were originally put together
 *  in the best container at the end of the tracking sequence in Brunel.
 *
 *  @author Eduardo Rodrigues
 *  @date   2006-09-18
 */
class TrackFromDST : public GaudiAlgorithm {

public: 
  /// Standard constructor
  TrackFromDST( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackFromDST( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  bool areSettingsConsistent() const;

  // Classify the input tracks according to their History
  TracksMap classifyTracks( LHCb::Tracks*& inTracks );
  
  // Output a given vector of tracks to its corresponding TES path
  void outputTracks( const std::vector<LHCb::Track*>& tracks,
                     const std::string path );
  
private:
  bool m_debugLevel;
  std::map<const LHCb::Track::History,std::string> m_map;
  
  // job options
  // -----------
  // input Track container path
  std::string m_tracksInContainer;
};
#endif // TRACKFROMDST_H
