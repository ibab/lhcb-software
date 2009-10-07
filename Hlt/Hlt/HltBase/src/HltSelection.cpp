#include "HltBase/HltSelection.h"
#include "Event/L0CaloCandidate.h"
#include "Event/L0MuonCandidate.h"



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
//FIXME: why do we need this? (in HltSelectionFilter...)
template class Hlt::TSelection<Hlt::Selection>;

