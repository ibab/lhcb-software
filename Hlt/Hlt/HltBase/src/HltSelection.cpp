#include "HltBase/HltSelection.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/L0CaloCandidate.h"
#include "Event/L0MuonCandidate.h"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/construct.hpp"
#include <algorithm>



template <typename T>
void Hlt::TSelection<T>::clean() 
{ 
    Selection::clean(); 
    m_candidates.clear();
}

template <typename T>
Hlt::TSelection<T>::~TSelection()
{
   // assert(m_candidates.empty());
}

// explicityly instantiate allowed instances...
template class Hlt::TSelection<LHCb::Track>;
template class Hlt::TSelection<LHCb::RecVertex>;
template class Hlt::TSelection<LHCb::L0CaloCandidate>;
template class Hlt::TSelection<LHCb::L0MuonCandidate>;
//FIXME: why do we need this? (in HltSelectionFilter...)
template class Hlt::TSelection<Hlt::Selection>;

