// $Id: $
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

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
 
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
  bool m_requireFirstState;
  LHCb::Tracks * tracks; 
  double m_CloneOverlapTreshold;
  unsigned int m_MinimalHits ;
  LHCb::HltObjectSummary * m_HltObjectSummary;
  ObjectVector<LHCb::HltObjectSummary>* m_HltObjectSummarys ;
  void executeRecursive(const LHCb::HltObjectSummary* SelRep);
  LHCb::Track::Types  SetTrackType(LHCb::Track*);
  void RemoveClones(LHCb::Tracks *);
  StatusCode initializeTriggerLists();
  class TrackComperator
  {
  public:
    bool operator()(LHCb::Track* a ,LHCb::Track*b)
    {
      return a->nLHCbIDs() > b->nLHCbIDs();
    }
  }tc;
  class AssignNumInfos
  {
  public:
    std::string m_name;
    unsigned int m_prime;
  AssignNumInfos(std::string name,unsigned int prime)
    :m_name(name),m_prime(prime)
    {}
    AssignNumInfos(){}
  };
 std::map<std::string,AssignNumInfos> requiredNumericalInformation;
};
#endif // HLTTRACKCONVERTER_H
 
