#include "GaudiKernel/IIncidentSvc.h"

#include "TrackInterfaces/ITrackSelector.h"

#include "TrackUsedLHCbID.h"
#include "Event/Track.h"

#include <algorithm>
#include <exception>

DECLARE_TOOL_FACTORY( TrackUsedLHCbID )

TrackUsedLHCbID::TrackUsedLHCbID(const std::string& type,
    const std::string& name, const IInterface* parent) :
  GaudiTool(type, name, parent), m_flags(0)
{
  declareInterface<IUsedLHCbID>(this);
  /** Define containers and corresponding selectors in same order.
   *  E.g. inputContainers = "Rec/Track/Forward" and selectorNames = "ForwardSelector".
   */
  declareProperty("inputContainers", m_inputs = {""});
  declareProperty("selectorNames", m_names = {""});
}

TrackUsedLHCbID::~TrackUsedLHCbID()
{
  // destructor
}


StatusCode TrackUsedLHCbID::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  if (m_names.size() != m_inputs.size()) {
    if (m_names.size() > m_inputs.size()) {
      Warning("More selector names than input locations, discarding excess selectors", StatusCode::FAILURE, 1).ignore();
    } else {
      Warning("More input locations than selectors, always accepting tracks where selectors are missing.", StatusCode::FAILURE, 1). ignore();
    }
    m_names.resize(m_inputs.size());
  }

  // make the selector tools
  std::transform(std::begin(m_names), std::end(m_names),
      std::back_inserter(m_selectors), [this] (const std::string& name) {
      return name.empty() ? nullptr : tool<ITrackSelector>(name, this);
      });

  // make sure we start from empty BloomFilters
  m_vtx.m_veloHits.clear(), m_bmg.m_ttHits.clear(), m_amg.m_tHits.clear(),
    m_vtx.m_vpHits.clear(), m_bmg.m_utHits.clear(), m_amg.m_ftHits.clear(),
    m_otherHits.clear();
  m_flags = 0;

  incSvc()->addListener(this, IncidentType::BeginEvent);

  if (msgLevel(MSG::DEBUG)) {
    // printout to announce size of differnt BloomFilters
    debug() << "BloomFilters for hits initialised, sizes: [ Velo " <<
      sizeof(m_vtx.m_veloHits) << " TT " << sizeof(m_bmg.m_ttHits) << " T " <<
      sizeof(m_amg.m_tHits) << " VP " << sizeof(m_vtx.m_vpHits) << " UT " <<
      sizeof(m_bmg.m_utHits) << " FT " << sizeof(m_amg.m_ftHits) <<
      " Other " << sizeof(m_otherHits) << "], class size is " <<
      sizeof(*this) << endmsg;
  }

  return StatusCode::SUCCESS;
}

bool TrackUsedLHCbID::used(const LHCb::LHCbID id) const
{
  // get the input - seeds
  if (UNLIKELY(!(m_flags & Initialized))) initEvent();
  switch (id.detectorType()) {
    case LHCb::LHCbID::Velo: return ((~m_flags) & Velo) ? false : m_vtx.m_veloHits.find(id);
    case LHCb::LHCbID::TT:   return ((~m_flags) & TT  ) ? false : m_bmg.m_ttHits.find(id);
    case LHCb::LHCbID::IT:   // fallthrough intended
    case LHCb::LHCbID::OT:   return ((~m_flags) & T   ) ? false : m_amg.m_tHits.find(id);
    case LHCb::LHCbID::VP:   return ((~m_flags) & VP  ) ? false : m_vtx.m_vpHits.find(id);
    case LHCb::LHCbID::UT:   return ((~m_flags) & UT  ) ? false : m_bmg.m_utHits.find(id);
    case LHCb::LHCbID::FT:   return ((~m_flags) & FT  ) ? false : m_amg.m_ftHits.find(id);
    default: return ((~m_flags) & Other) ? false : m_otherHits.find(id);
  };
  return false;
}

void TrackUsedLHCbID::handle(const Incident& incident)
{
  if (IncidentType::BeginEvent == incident.type()) {
      // reset Initialized bit to trigger reading new tracks
      m_flags &= ~Initialized;
  }
}

void TrackUsedLHCbID::initEvent() const
{
  // only clear the BloomFilters which actually need clearing
  if (m_flags & Velo) m_vtx.m_veloHits.clear();
  if (m_flags & TT) m_bmg.m_ttHits.clear();
  if (m_flags & T) m_amg.m_tHits.clear();
  if (m_flags & VP) m_vtx.m_vpHits.clear();
  if (m_flags & UT) m_bmg.m_utHits.clear();
  if (m_flags & FT) m_amg.m_ftHits.clear();
  if (m_flags & Other) m_otherHits.clear();
  m_flags = 0;

  // loop over tracks locations
  auto iterSelector = m_selectors.begin();
  for (auto iterS = m_inputs.begin(); iterS != m_inputs.end();
      ++iterS, ++iterSelector) {
    // get selection tool
    ITrackSelector* tSelector = *iterSelector;
    // get the containers and extract the ids from the track
    auto tCont = getIfExists<LHCb::Track::Range>(*iterS);
    if (tCont.empty()) {
      if (msgLevel(MSG::DEBUG))
	debug() << "Track container '" << *iterS << "' does not exist" << endmsg;
      continue;
    }
    for (auto iterTrack: tCont) { // loop over tracks in container
      if (tSelector && !(tSelector->accept(*iterTrack))) continue;
      // put hits on track into BloomFilters
      for (const LHCb::LHCbID id: iterTrack->lhcbIDs()) {
	switch (id.detectorType()) {
	  case LHCb::LHCbID::Velo: m_flags |= Velo; m_vtx.m_veloHits.insert(id); break;
	  case LHCb::LHCbID::TT:   m_flags |= TT  ; m_bmg.m_ttHits.insert(id); break;
	  case LHCb::LHCbID::IT:   // fallthrough intended
	  case LHCb::LHCbID::OT:   m_flags |= T   ; m_amg.m_tHits.insert(id); break;
	  case LHCb::LHCbID::VP:   m_flags |= VP  ; m_vtx.m_vpHits.insert(id); break;
	  case LHCb::LHCbID::UT:   m_flags |= UT  ; m_bmg.m_utHits.insert(id); break;
	  case LHCb::LHCbID::FT:   m_flags |= FT  ; m_amg.m_ftHits.insert(id); break;
	  default: m_flags |= Other; m_otherHits.insert(id); break;
	};
      }
    } //iterTrack
  }  // iterS

  // make sure nobody mixes current and upgrade detector hits in the same job
  if ((m_flags & Velo && m_flags & VP) || (m_flags & TT && m_flags & UT) ||
      (m_flags & T && m_flags & FT)) {
    throw std::logic_error("Cannot deal with current and upgrade detector hits in the same event.");
  }

  // tracks all read, set Initialized bit
  m_flags |= Initialized;
}
