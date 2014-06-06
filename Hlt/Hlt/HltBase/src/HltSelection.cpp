#include "HltBase/HltSelection.h"
#include "Event/L0CaloCandidate.h"
#include "Event/L0MuonCandidate.h"

#include "Event/HltL0DiMuonCandidate.h"
#include "Event/HltMultiTrack.h"
#include "Event/HltStage.h"
#include "Event/HltCandidate.h"

template <typename T>
void Hlt::TSelection<T>::clean()
{
    Selection::clean();
    m_candidates.clear();
}

template <typename T>
Hlt::TSelection<T>::~TSelection()
{
}

// explicityly instantiate allowed instances...
template class Hlt::TSelection<LHCb::Track>;
template class Hlt::TSelection<LHCb::RecVertex>;
template class Hlt::TSelection<LHCb::L0CaloCandidate>;
template class Hlt::TSelection<LHCb::L0MuonCandidate>;
template class Hlt::TSelection<LHCb::Particle>;

template class Hlt::TSelection<Hlt::Candidate>;
template class Hlt::TSelection<Hlt::Stage>;
template class Hlt::TSelection<Hlt::MultiTrack>;
template class Hlt::TSelection<Hlt::L0DiMuonCandidate>;
