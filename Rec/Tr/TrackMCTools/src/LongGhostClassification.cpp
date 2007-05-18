// $Id: LongGhostClassification.cpp,v 1.2 2007-05-18 09:29:58 cattanem Exp $
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

#include "Kernel/VeloChannelID.h"

#include "LongGhostClassification.h"

#include "TrackMCInterfaces/ILHCbIDsToMCParticles.h"

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

  if (isMatched(vMatch) && isMatched(tMatch) && vMatch.first != tMatch.first){
     tinfo.setClassification(LHCb::GhostTrackInfo::InconsistentParts);
  }

  return;
}
