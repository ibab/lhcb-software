#include <algorithm>
#include <unordered_map>

#include "GaudiKernel/AlgFactory.h"
#include "Event/Track.h"
#include "TrackInterfaces/ITrackSelector.h"

#include "RecycleGhostHits.h"
using namespace LHCb;

namespace std {
  template <> struct hash<LHCbID>
  {
    size_t operator()(const LHCbID& x) const
    {
      return hash<unsigned int>()(x.channelID());
    }
  };
}


DECLARE_ALGORITHM_FACTORY( RecycleGhostHits )
RecycleGhostHits::RecycleGhostHits(const std::string& name,
                                   ISvcLocator* pSvcLocator):
GaudiAlgorithm(name, pSvcLocator), m_selector("",this) 
{
  declareProperty("inputLocation",  m_inputLocation = TrackLocation::Velo);
  declareProperty("outputLocation", m_outputLocation = TrackLocation::Default);
  declareProperty("Selector", m_selector);
}

RecycleGhostHits::~RecycleGhostHits() {}

StatusCode RecycleGhostHits::execute()
{
  LHCb::Track::Range inCont = get<LHCb::Track::Range>(m_inputLocation);
  Tracks* outCont = getOrCreate<Tracks,Tracks>(m_outputLocation);

  // Get all the hits in the T stations
  auto hits = m_tHitManager->hits();
  std::unordered_map<LHCbID, PatForwardHit*> lhcbID2hit;
  std::for_each(std::begin(hits), std::end(hits),
                [&lhcbID2hit](PatForwardHit* h) {lhcbID2hit.emplace(h->hit()->lhcbID(),
                                                                    h);});

  for (auto track : inCont) {
    if (m_selector->accept(*track)) {
      Track* aTrack = track->clone();
      outCont->insert(aTrack);
    }
    else {
      for (auto lhcbid : track->lhcbIDs()) {
        auto fwd_hit = lhcbID2hit.find(lhcbid);
        if(fwd_hit != lhcbID2hit.end()) {
          fwd_hit->second->hit()->setStatus(Tf::HitBase::UsedByPatForward, false);
          fwd_hit->second->setIsUsed(false);
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode RecycleGhostHits::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize");
  }

  m_tHitManager = tool<Tf::TStationHitManager<PatForwardHit> >("PatTStationHitManager");
  
  // retrieve the selector if it is set
  if (!m_selector.empty()) {
    sc = m_selector.retrieve();
    if(sc.isFailure())
      error() << "Failed to retrieve selector." << endmsg;
  }
     
  return sc;
}
    
StatusCode RecycleGhostHits::finalize()
{
  if(!m_selector.empty()) m_selector.release().ignore();
  return GaudiAlgorithm::finalize();
}
