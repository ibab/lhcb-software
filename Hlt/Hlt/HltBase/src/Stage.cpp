// $Id: Stage.cpp,v 1.2 2010-07-14 15:14:54 amazurov Exp $ 
// ============================================================================
#include "Event/HltStage.h"
#include <cassert>

namespace Hlt {
Stage::Lock::Lock (  Stage* stage , const INamedInterface* locker) { 
    if (0 == stage || 0 == locker) { 
        throw GaudiException("Stage or locker is null","Stage::Lock::Lock",
            StatusCode::FAILURE);
    }
    m_stage = stage;
    m_locker = const_cast<INamedInterface*>(locker);
    stage->_lock(locker) ;
}
    
Stage::Lock::~Lock (){
    m_stage->_unlock (m_locker);
}

void Stage::_checkLock() const {
    if (!locked()) {
        throw GaudiException("Not locked","Stage::_checkLock",StatusCode::FAILURE);
    }
}
  
void Stage::SetAllToNull() {
    _checkLock();
    m_track = 0;
    m_rec_vertex = 0;
    m_l0_calo_candidate = 0;
    m_l0_muon_candidate = 0;
    m_multitrack = 0;
    m_l0_dimuon_candidate = 0;
}

const ContainedObject* Stage::_get() const {
  if (is<LHCb::Track> ()) return get<LHCb::Track>();
  if (is<LHCb::RecVertex> ()) return get<LHCb::RecVertex>();
  if (is<LHCb::L0CaloCandidate> ()) return get<LHCb::L0CaloCandidate>();
  if (is<LHCb::L0MuonCandidate> ()) return  get<LHCb::L0MuonCandidate>();
  if (is<Hlt::MultiTrack> ()) return  get<Hlt::MultiTrack>();
  if (is<Hlt::L0DiMuonCandidate> ()) return get<Hlt::L0DiMuonCandidate>();
  return 0;
}
void Stage::_lock (const INamedInterface* locker) {
    if (locked()) { 
        throw GaudiException("Stage already locked","Stage::_lock",
        StatusCode::FAILURE); 
    }
    m_locker = const_cast<INamedInterface*> (locker) ;
    m_history.push_back (locker->name());
}

void Stage::_unlock (const INamedInterface* locker) {
    if (!locked()) { 
        throw GaudiException("Stage  not locked","Stage::_unlock",
        StatusCode::FAILURE);
    }
    if (locker != locked()) { 
        throw GaudiException("Wrong locker","Stage::_unlock",
            StatusCode::FAILURE);
    }
    m_locker = 0 ;
}

std::ostream& Stage::fillStream(std::ostream& s) const {
  if (is<LHCb::Track> ())
    s << "Track " << get<LHCb::Track> ();
  if (is<LHCb::RecVertex> ())
    s << "RecVertex " << get<LHCb::RecVertex> ();
  if (is<LHCb::L0CaloCandidate> ())
    s << "L0CaloCandidate " << get<LHCb::L0CaloCandidate> ();
  if (is<LHCb::L0MuonCandidate> ())
    s << "L0MuonCandidate  " << get<LHCb::L0MuonCandidate> ();
  if (is<Hlt::MultiTrack> ())
    s << "MultiTrack " << get<Hlt::MultiTrack> ();
  if (is<Hlt::L0DiMuonCandidate> ())
    s << "L0DiMuonCandidate " << get<Hlt::L0DiMuonCandidate> ();

  s <<  ", " << m_cache;
  s << ", history: [";
  std::string delim="";
  for(History::const_iterator cur = m_history.begin();
     cur != m_history.end(); cur++) {
    s << delim << "'" <<  *cur << "'";
    delim = ", ";
  }
  s << "]";
  return s;
}
} // namespace Hlt
