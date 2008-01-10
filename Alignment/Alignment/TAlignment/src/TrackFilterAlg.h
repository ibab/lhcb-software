// $Id: TrackFilterAlg.h,v 1.4 2008-01-10 10:39:54 janos Exp $
#ifndef TALIGNMENT_TRACKFILTERALG_H 
#define TALIGNMENT_TRACKFILTERALG_H 1

// Include files
// from STD
#include <string>
#include <map>
#include <memory>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from Kernel
#include "Kernel/LHCbID.h"

// from TrackEvent
#include "Event/Track.h"

// from AlignmentInterfaces
#include "TrackInterfaces/ITrackSelector.h"

#include "IGetElementsToBeAligned.h"
#include "AlignmentElement.h"

// from BOOST
#include "boost/function.hpp"

/** @class TrackFilterAlg TrackFilterAlg.h
 *  A small algorithm that filters tracks of a certain type. 
 *
 *  @author Jan Amoraal
 *  @date   2007-07-11
 */

class TrackFilterAlg : public GaudiAlgorithm {

public: 
  /// Some handy typedefs
  typedef std::vector<LHCb::LHCbID> LHCBIDS;
  typedef std::map<std::string, boost::function<bool (LHCb::LHCbID)> > LHCbDetChecks;
  typedef std::pair<std::vector<AlignmentElement>::const_iterator,
                    std::vector<AlignmentElement>::const_iterator> Range;
  
  /// Standard constructor
  TrackFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackFilterAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
 
private:
    
  bool printDebug() const {return msgLevel(MSG::DEBUG);};
 
  void strip(const LHCb::LHCbID& anLHCbID,  LHCb::Track* track) {
    if (!(m_lhcbDetChecks[m_detector](anLHCbID))) track->removeFromLhcbIDs(anLHCbID);
  }  
  
  void filterTracks(const LHCb::Tracks* tracks);

  void filterTrack(LHCb::Track* track, LHCb::Tracks* outputContainer);
  
  std::string                               m_tracksInputContainer;  ///< Tracks input container
  std::string                               m_tracksOutputContainer; ///< Tracks output container
  std::string                               m_trackType;             ///< Type of tracks to filter
  std::map<std::string, LHCb::Track::Types> m_stringToTrackTypeMap;  ///< Map of string to track type enum
  std::map<LHCb::Track::Types, std::string> m_trackTypeToStringMap;  ///< Map of track type enum to string
  std::string                               m_trackSelectorName;     ///< Name of track selector for alignment
  bool                                      m_strip;                 ///< Flag to strip unwanted hits   
  std::string                               m_detector;              ///< Sub-detector
  unsigned int                              m_nMinHits;              ///< Min number of hits 
  LHCbDetChecks                             m_lhcbDetChecks;         ///< Map LHCb id det checks methods  
  ITrackSelector*                           m_trackSelector;         ///< Pointer to track selector tool for alignment
  IGetElementsToBeAligned*                  m_elementsToBeAligned;
  Range                                     m_rangeElements;
};

#endif // TALIGNMENT_TRACKFILTERALG_H
