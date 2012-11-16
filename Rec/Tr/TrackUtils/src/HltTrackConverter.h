#ifndef HLTTRACKCONVERTER_H 
#define HLTTRACKCONVERTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


#include "Event/Track.h"
//#include "Event/PackedTrack.h"

/** @class HltTrackConverter HltTrackConverter.h
 *  
 *
 *  @author Albert Frithjof Bursche
 *  @date   2010-05-03
 */
class HltTrackConverter : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltTrackConverter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltTrackConverter( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution
 
 protected:

 private:
  std::string m_HltObjectSummaryLocation; // in TES
  std::string m_ConvertedTracksDestignation; // in TES
  std::vector<std::string> m_UseHitsFrom ; //See LHCb::LHCbID::channelIDype for the values.
  bool m_UseHitsFromLookupTable[9] ; //may cause segfault in case LHCb::LHCbID::channelIDype is changed
  std::vector<std::string> m_HltLines;
  bool m_HltLinesFrom1stEvent;
  bool m_requireTrackClassID;
  bool m_addFirstState;
  double m_CloneOverlapTreshold;
  unsigned int m_MinimalHits ;
  LHCb::HltObjectSummary * m_HltObjectSummary;
  ObjectVector<LHCb::HltObjectSummary>* m_HltObjectSummarys ;
  void executeRecursive(LHCb::Track::Vector& tracks, const LHCb::HltObjectSummary& SelRep) const ;
  LHCb::Track::Types  SetTrackType(const LHCb::Track& ) const ;
  void RemoveClones(LHCb::Track::Vector& tracks) const ;
  StatusCode initializeTriggerLists();
};
#endif // HLTTRACKCONVERTER_H
 
