// $Id: $
// Include files

// local
#include "Event/HltSelReports.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltSelReports
//
// 2011-02-12 : Chris Jones
//-----------------------------------------------------------------------------

std::ostream& LHCb::HltSelReports::fillStream(std::ostream& ss) const
{
  ss << " HltSelReports : { \n" ;
  for(const auto& i : m_selReports) {
    ss << " selectionName : " << i.first
       << " HltObjectSummary : " << i.second
       << '\n'; 
  }
  ss << " }" << std::endl;
  return ss;
}
