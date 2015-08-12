#include "Kernel/VeloChannelID.h"

#include "UpstreamGhostClassification.h"

#include "Event/Track.h"


DECLARE_TOOL_FACTORY(UpstreamGhostClassification)

using namespace LHCb;

UpstreamGhostClassification::UpstreamGhostClassification(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  TrackGhostClassificationBase(type, name, parent){

}

UpstreamGhostClassification::~UpstreamGhostClassification(){
  // destructer
}

StatusCode UpstreamGhostClassification::specific(LHCbIDs::const_iterator& start, 
                                       LHCbIDs::const_iterator& stop, 
                                       LHCb::GhostTrackInfo& tinfo) const{

  // split into velo and T hits
  LHCbIDs::const_iterator iter = start;
  LHCbIDs ttHits;  ttHits.reserve(20);
  LHCbIDs vHits;  vHits.reserve(20);
  for (; iter != stop; ++iter){
    if (iter->detectorType() == LHCbID::TT){
      ttHits.push_back(*iter); 
    }
    else if (iter->detectorType() == LHCbID::Velo){
      vHits.push_back(*iter);
    }
  } // for iter


  // match the velo Hits
  LHCb::GhostTrackInfo::LinkPair vMatch = bestPair(vHits);

  // match the velo Hits
  LHCb::GhostTrackInfo::LinkPair ttMatch = bestPair(ttHits);

  if (vMatch.first == 0 || vMatch.second < m_purityCut){
     tinfo.setClassification(GhostTrackInfo::GhostParent);  
  }

  if (isMatched(vMatch) && isMatched(ttMatch) && vMatch.first != ttMatch.first){
     tinfo.setClassification(LHCb::GhostTrackInfo::InconsistentParts);
  }

  return StatusCode::SUCCESS;
}
