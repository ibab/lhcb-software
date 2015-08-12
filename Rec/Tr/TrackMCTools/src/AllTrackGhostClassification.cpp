// Event model
#include "Event/Track.h"

// local
#include "AllTrackGhostClassification.h"


DECLARE_TOOL_FACTORY(AllTrackGhostClassification)

using namespace LHCb;

AllTrackGhostClassification::AllTrackGhostClassification(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  TrackGhostClassificationBase(type, name, parent){

}

AllTrackGhostClassification::~AllTrackGhostClassification(){
  // destructer
}

StatusCode AllTrackGhostClassification::initialize() {
  m_downstreamGhostClassification = tool<ITrackGhostClassification>("DownstreamGhostClassification",this);
  m_longGhostClassification       = tool<ITrackGhostClassification>("LongGhostClassification",      this);
  m_tTrackGhostClassification     = tool<ITrackGhostClassification>("TTrackGhostClassification",    this);
  m_upstreamGhostClassification   = tool<ITrackGhostClassification>("UpstreamGhostClassification",  this);
  m_veloGhostClassification       = tool<ITrackGhostClassification>("VeloGhostClassification",      this);
  m_veloRGhostClassification      = tool<ITrackGhostClassification>("VeloRGhostClassification",     this);

  return TrackGhostClassificationBase::initialize();
}

bool AllTrackGhostClassification::isGhost(LHCbIDs::const_iterator& start, 
                       LHCbIDs::const_iterator& stop) const{
  bool hasVeloR(false);
  bool hasVeloPhi(false);
  bool hasTT(false);
  bool hasT(false);
  for (LHCbIDs::const_iterator it = start ; it!=stop ; ++it) {
    hasVeloR = hasVeloR || it->isVeloR() || it->isVP();
    hasVeloPhi = hasVeloPhi || it->isVeloPhi() || it->isVP();
    hasT = hasT || it->isOT() || it->isIT() || it->isFT();
    hasTT = hasTT || it->isTT() || it->isUT();
  }

  if (hasVeloR && hasVeloPhi && hasT) {
    return m_longGhostClassification->isGhost(start,stop);
  }
  if (hasVeloR && hasVeloPhi && !hasT && !hasTT) {
    return m_veloGhostClassification->isGhost(start,stop);
  }
  if (hasVeloR && !hasVeloPhi && !hasT && !hasTT) {
    return m_veloRGhostClassification->isGhost(start,stop);
  }
  if (hasVeloR && hasVeloPhi && !hasT && hasTT) {
    return m_upstreamGhostClassification->isGhost(start,stop);
  }
  if (!hasVeloR && !hasVeloPhi && hasT && hasTT) {
    return m_downstreamGhostClassification->isGhost(start,stop);
  }
  if (!hasVeloR && !hasVeloPhi && hasT && !hasTT) {
    return m_tTrackGhostClassification->isGhost(start,stop);
  }

  return TrackGhostClassificationBase::isGhost(start,stop);
}

/*
StatusCode AllTrackGhostClassification::specific(const LHCb::Track& aTrack,
                                       LHCb::GhostTrackInfo& tinfo) const{
  if (Track::Long == aTrack.type()) {
    return m_longGhostClassification->info(aTrack, tinfo);
  }
  if (Track::Velo == aTrack.type()) {
    return m_veloGhostClassification->info(aTrack, tinfo);
  }
  if (Track::VeloR == aTrack.type()) {
    return m_veloRGhostClassification->info(aTrack, tinfo);
  }
  if (Track::Upstream == aTrack.type()) {
    return m_upstreamGhostClassification->info(aTrack, tinfo);
  }
  if (Track::Downstream == aTrack.type()) {
    return m_downstreamGhostClassification->info(aTrack, tinfo);
  }
  if (Track::Ttrack == aTrack.type()) {
    return m_tTrackGhostClassification->info(aTrack, tinfo);
  }

  StatusCode sc = Error("unexpected track type",StatusCode::SUCCESS, 10);
  if ( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << aTrack << endmsg;
  return sc;
}
*/

StatusCode AllTrackGhostClassification::specific(LHCbIDs::const_iterator& start, 
                                       LHCbIDs::const_iterator& stop, 
                                       LHCb::GhostTrackInfo& tinfo) const{
  bool hasVeloR(false);
  bool hasVeloPhi(false);
  bool hasTT(false);
  bool hasT(false);
  for (LHCbIDs::const_iterator it = start ; it!=stop ; ++it) {
    hasVeloR = hasVeloR || it->isVeloR() || it->isVP();
    hasVeloPhi = hasVeloPhi || it->isVeloPhi() || it->isVP();
    hasT = hasT || it->isOT() || it->isIT() || it->isFT();
    hasTT = hasTT || it->isTT() || it->isUT();
  }

  if (hasVeloR && hasVeloPhi && hasT) {
    return m_longGhostClassification->info(start, stop , tinfo);
  }
  if (hasVeloR && hasVeloPhi && !hasT && !hasTT) {
    return m_veloGhostClassification->info(start, stop , tinfo);
  }
  if (hasVeloR && !hasVeloPhi && !hasT && !hasTT) {
    return m_veloRGhostClassification->info(start, stop , tinfo);
  }
  if (hasVeloR && hasVeloPhi && !hasT && hasTT) {
    return m_upstreamGhostClassification->info(start, stop , tinfo);
  }
  if (!hasVeloR && !hasVeloPhi && hasT && hasTT) {
    return m_downstreamGhostClassification->info(start, stop , tinfo);
  }
  if (!hasVeloR && !hasVeloPhi && hasT && !hasTT) {
    return m_tTrackGhostClassification->info(start, stop , tinfo);
  }

  StatusCode sc = Error("Couldn't determine track type",StatusCode::SUCCESS, 10);
  if ( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    debug() << " these are the LHCbIDs: " << endmsg;
    for (LHCbIDs::const_iterator it = start ; it!=stop ; ++it)
      debug() << *it << endmsg;
  }
  return sc;
}

