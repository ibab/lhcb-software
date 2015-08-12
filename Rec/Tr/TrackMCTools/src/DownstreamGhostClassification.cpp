#include "Kernel/VeloChannelID.h"

#include "DownstreamGhostClassification.h"

#include "Event/Track.h"


DECLARE_TOOL_FACTORY(DownstreamGhostClassification)

using namespace LHCb;

DownstreamGhostClassification::DownstreamGhostClassification(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  TrackGhostClassificationBase(type, name, parent){

}

DownstreamGhostClassification::~DownstreamGhostClassification(){
  // destructer
}

StatusCode DownstreamGhostClassification::specific(LHCbIDs::const_iterator& start, 
                                       LHCbIDs::const_iterator& stop, 
                                       LHCb::GhostTrackInfo& tinfo) const{

  // split into velo and T hits
  LHCbIDs::const_iterator iter = start;
  LHCbIDs ttHits;  ttHits.reserve(20);
  LHCbIDs tHits;  tHits.reserve(20);
  for (; iter != stop; ++iter){
    if (iter->detectorType() == LHCbID::TT){
      ttHits.push_back(*iter); 
    }
    else if (iter->detectorType() == LHCbID::OT || 
             iter->detectorType() == LHCbID::IT ){
      tHits.push_back(*iter);
    }
  } // for iter


  // match the T Hits
  LHCb::GhostTrackInfo::LinkPair tMatch = bestPair(tHits);

  // match the velo Hits
  LHCb::GhostTrackInfo::LinkPair ttMatch = bestPair(ttHits);

  if (tMatch.first == 0 || tMatch.second < m_purityCut){
     tinfo.setClassification(GhostTrackInfo::GhostParent);  
  }

  if (isMatched(tMatch) && isMatched(ttMatch) && tMatch.first != ttMatch.first){
     tinfo.setClassification(LHCb::GhostTrackInfo::InconsistentParts);
  }

  return StatusCode::SUCCESS;
}
