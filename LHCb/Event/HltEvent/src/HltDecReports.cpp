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
  for(Container::const_iterator i=m_decReports.begin();
      i!=m_decReports.end(); ++i)
  {
    s << " decisionName :	" << i->first
      << " HltDecReport :	" << i->second 
      << "\n";
  }
  s << " }" << std::endl;
  return s;
}
