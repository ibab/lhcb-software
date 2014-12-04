// $Id: $
// Include files 

// local
#include "Event/HltDecReports.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltDecReports
//
// 2011-02-12 : Chris Jones
//-----------------------------------------------------------------------------

std::ostream& LHCb::HltDecReports::fillStream(std::ostream& s) const
{
  s << " HltDecReports : configuredTCK=" << m_configuredTCK 
    << " {\n" ;
  for(const auto& i : m_decReports )
  {
    s << " decisionName :	" << i.first
      << " HltDecReport :	" << i.second 
      << "\n";
  }
  s << " }" << std::endl;
  return s;
}
