// $Id: VeloRGhostClassification.cpp,v 1.1 2007-05-15 13:04:18 mneedham Exp $
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"



#include "Kernel/VeloChannelID.h"

#include "VeloRGhostClassification.h"

#include "TsaKernel/ILHCbIDsToMCParticles.h"

#include "Event/Track.h"


DECLARE_TOOL_FACTORY(VeloRGhostClassification)

using namespace LHCb;

VeloRGhostClassification::VeloRGhostClassification(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  TrackGhostClassificationBase(type, name, parent){

};

VeloRGhostClassification::~VeloRGhostClassification(){
  // destructer
}


void VeloRGhostClassification::specific(LHCbIDs::const_iterator& start, 
                                            LHCbIDs::const_iterator& stop, 
                                            LHCb::GhostTrackInfo& tinfo) const{

  if (std::distance(start, stop) == 3u){
    LHCb::GhostTrackInfo::LinkPair bMatch = tinfo.bestLink();
    if (bMatch.second < 0.5) tinfo.setClassification(LHCb::GhostTrackInfo::Combinatoric);
  }
  
  return;
}
