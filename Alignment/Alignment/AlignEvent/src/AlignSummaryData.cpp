#include "Event/AlignSummaryData.h"

namespace LHCb 
{
  std::ostream& AlignSummaryData::fillStream(std::ostream& os) const
  {
    return equations().fillStream(os) ;
  }
  
  void AlignSummaryData::add( const AlignSummaryData& rhs )
  {
    m_equations.add(rhs.m_equations) ;
  }
}
