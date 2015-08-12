#include "Kernel/STChannelID.h"
#include "Kernel/OTChannelID.h"

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTLayer.h"
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTLayer.h"

#include "TTrackGhostClassification.h"

#include "Event/Track.h"


DECLARE_TOOL_FACTORY(TTrackGhostClassification)

using namespace LHCb;

TTrackGhostClassification::TTrackGhostClassification(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  TrackGhostClassificationBase(type, name, parent){

}

StatusCode TTrackGhostClassification::initialize(){

  StatusCode sc = TrackGhostClassificationBase::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }


  m_oTracker = getDet<DeOTDetector>( DeOTDetectorLocation::Default);
  m_iTracker = getDet<DeSTDetector>( DeSTDetLocation::location("IT"));

  return StatusCode::SUCCESS;
}


TTrackGhostClassification::~TTrackGhostClassification(){
  // destructer
}


StatusCode TTrackGhostClassification::specific(LHCbIDs::const_iterator& start, 
                                     LHCbIDs::const_iterator& stop,
                                     LHCb::GhostTrackInfo& tinfo) const{
  
  // collect r and phi hits...
  LHCbIDs xHits;  xHits.reserve(20);
  LHCbIDs stereoHits; stereoHits.reserve(20);
  for (LHCbIDs::const_iterator iter = start; iter != stop; ++iter){
    if (iter->detectorType() == LHCbID::OT){
      OTChannelID otChan = iter->otID();
      stereoOT(otChan) == true ? stereoHits.push_back(*iter) : xHits.push_back(*iter);
    }      
    else if (iter->detectorType() == LHCbID::IT) {
      STChannelID itChan = iter->stID();
      stereoIT(itChan) == true ? stereoHits.push_back(*iter) : xHits.push_back(*iter);
    }
  } // for iter

  // match the rHits
  LHCb::GhostTrackInfo::LinkPair xMatch = bestPair(xHits);

  // match the phiHits
  LHCb::GhostTrackInfo::LinkPair stereoMatch = bestPair(stereoHits);

  if (isMatched(stereoMatch) && isMatched(xMatch) && stereoMatch.first != xMatch.first){
      tinfo.setClassification(LHCb::GhostTrackInfo::InconsistentParts);
  }

  return StatusCode::SUCCESS;
 }

bool TTrackGhostClassification::stereoOT(const OTChannelID& chan) const{
//
 DeOTLayer* layer = m_oTracker->findLayer(chan);
 return (fabs(layer->angle()) > 1e-5 ? true : false );
}

bool TTrackGhostClassification::stereoIT(const STChannelID& chan) const{
 DeSTLayer* layer = m_iTracker->findLayer(chan);
 return (fabs(layer->angle()) > 1e-5 ? true : false );  
}
