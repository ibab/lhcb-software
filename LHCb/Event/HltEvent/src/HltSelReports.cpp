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
  ss << " HltSelReports : { " << std::endl;
  for(Container::const_iterator i=m_selReports.begin();
      i!=m_selReports.end(); ++i){
    ss << " selectionName : " << i->first
       << " HltObjectSummary : " << i->second
       << std::endl;
  }
  ss << " }" << std::endl;
  return ss;
}
