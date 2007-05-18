// $Id: VeloGhostClassification.cpp,v 1.2 2007-05-18 09:29:59 cattanem Exp $

// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

// LHCbKernel
#include "Kernel/VeloChannelID.h"

// TrackMCInterfaces
#include "TrackMCInterfaces/ILHCbIDsToMCParticles.h"

// Event model
#include "Event/Track.h"

// local
#include "VeloGhostClassification.h"


DECLARE_TOOL_FACTORY(VeloGhostClassification)

using namespace LHCb;

VeloGhostClassification::VeloGhostClassification(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  TrackGhostClassificationBase(type, name, parent){

};

VeloGhostClassification::~VeloGhostClassification(){
  // destructer
}

void VeloGhostClassification::specific(LHCbIDs::const_iterator& start, 
                                       LHCbIDs::const_iterator& stop, 
                                       LHCb::GhostTrackInfo& tinfo) const{

  LHCbIDs rHits;  rHits.reserve(20);
  LHCbIDs phiHits; phiHits.reserve(20);
  for (LHCbIDs::const_iterator iter = start; iter != stop; ++iter){
    if (iter->detectorType() == LHCbID::Velo){
      VeloChannelID vChan = iter->veloID();
      if (vChan.isRType() == true)  {
        rHits.push_back(vChan);
      } 
      else if (vChan.isPhiType() == true) {
	phiHits.push_back(vChan);
      } 
    }
  } // for iter

  // match the rHits
  LHCb::GhostTrackInfo::LinkPair rMatch = bestPair(rHits);

  // match the phiHits
  LHCb::GhostTrackInfo::LinkPair phiMatch = bestPair(phiHits);

  if (isMatched(rMatch) && isMatched(phiMatch) && phiMatch.first != rMatch.first){
    tinfo.setClassification(LHCb::GhostTrackInfo::InconsistentParts);
  }

}

