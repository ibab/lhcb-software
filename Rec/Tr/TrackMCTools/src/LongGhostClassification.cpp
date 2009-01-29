// $Id: LongGhostClassification.cpp,v 1.5 2009-01-29 12:28:48 mneedham Exp $
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

#include "Kernel/VeloChannelID.h"

#include "LongGhostClassification.h"

#include "Event/Track.h"


DECLARE_TOOL_FACTORY(LongGhostClassification)

using namespace LHCb;

LongGhostClassification::LongGhostClassification(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  TrackGhostClassificationBase(type, name, parent){

};

LongGhostClassification::~LongGhostClassification(){
  // destructer
}

void LongGhostClassification::specific(LHCbIDs::const_iterator& start, 
                                       LHCbIDs::const_iterator& stop, 
                                       LHCb::GhostTrackInfo& tinfo) const{

  // split into velo and T hits
  LHCbIDs::const_iterator iter = start;
  LHCbIDs tHits;  tHits.reserve(20);
  LHCbIDs vHits;  vHits.reserve(20);
  for (; iter != stop; ++iter){
    if (iter->detectorType() == LHCbID::OT || 
        iter->detectorType() == LHCbID::IT){
      tHits.push_back(*iter); 
    }
    else if (iter->detectorType() == LHCbID::Velo){
      vHits.push_back(*iter);
    }
  } // for iter

  // match the T Hits
  LHCb::GhostTrackInfo::LinkPair tMatch = bestPair(tHits);

  // match the velo Hits
  LHCb::GhostTrackInfo::LinkPair vMatch = bestPair(vHits);

  if (tMatch.first == 0 || vMatch.first == 0 || tMatch.second < m_purityCut || vMatch.second < m_purityCut){
    tinfo.setClassification(GhostTrackInfo::GhostParent);  
  }

  if (isMatched(vMatch) && isMatched(tMatch) && vMatch.first != tMatch.first){
     tinfo.setClassification(LHCb::GhostTrackInfo::InconsistentParts);
  }

  return;
}

bool LongGhostClassification::isGhost(TrackGhostClassificationBase::LHCbIDs::const_iterator& start, 
                                           TrackGhostClassificationBase::LHCbIDs::const_iterator& stop) const{

  LHCbIDs::const_iterator iter = start;
  LHCbIDs tHits;  tHits.reserve(20);
  LHCbIDs vHits;  vHits.reserve(20);
  for (; iter != stop; ++iter){
    if (iter->detectorType() == LHCbID::OT || 
        iter->detectorType() == LHCbID::IT){
      tHits.push_back(*iter); 
    }
    else if (iter->detectorType() == LHCbID::Velo){
      vHits.push_back(*iter);
    }
  } // for iter


  // match the T Hits
  LHCb::GhostTrackInfo::LinkPair tMatch = bestPair(tHits);
  if (isReal(tMatch) == false) return true;

  // match the velo Hits
  LHCb::GhostTrackInfo::LinkPair vMatch = bestPair(vHits);
  return isReal(vMatch) ? false : true;
    
}


