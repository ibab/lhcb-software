// $Id: TrackFilterAlg.h,v 1.1 2007-07-20 17:26:53 janos Exp $
#ifndef TALIGNMENT_TRACKFILTERALG_H 
#define TALIGNMENT_TRACKFILTERALG_H 1

// Include files
// from STD
#include <string>
#include <map>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from TrackEvent
#include "Event/Track.h"

// from AlignmentInterfaces
#include "AlignmentInterfaces/IAlignSelTool.h"

/** @class TrackFilterAlg TrackFilterAlg.h
 *  A small algorithm that filters tracks of a certain type. 
 *
 *  @author Jan Amoraal
 *  @date   2007-07-11
 */

class TrackFilterAlg : public GaudiAlgorithm {

public: 
  /// Standard constructor
  TrackFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackFilterAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:
 
private:
    
  void filterTracks(const LHCb::Tracks* tracks);

  void filterTrack(LHCb::Track* track, LHCb::Tracks* outputContainer);
  
  std::string                               m_tracksInputContainer;  ///< Tracks input container
  std::string                               m_tracksOutputContainer; ///< Tracks output container
  std::string                               m_trackType;             ///< Type of tracks to filter
  std::map<std::string, LHCb::Track::Types> m_stringToTrackTypeMap;  ///< Map of string to track type enum
  std::map<LHCb::Track::Types, std::string> m_trackTypeToStringMap;  ///< Map of track type enum to string
  std::string                               m_trackSelectorName;     ///< Name of track selector for alignment
  IAlignSelTool*                            m_trackSelector;         ///< Pointer to track selector tool for alignment
};

#endif // TALIGNMENT_TRACKFILTERALG_H
