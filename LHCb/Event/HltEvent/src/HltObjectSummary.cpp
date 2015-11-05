// $Id: $
// Include files

// local
#include "Event/HltObjectSummary.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltObjectSummary
//
// 2011-02-12 : Chris Jones
//-----------------------------------------------------------------------------

std::ostream& LHCb::HltObjectSummary::fillStream(std::ostream& ss) const
{
  ss << "{ " << " summarizedObjectCLID : " << summarizedObjectCLID() ;
  ss << " numericalInfo : { ";
  for( const auto& i : numericalInfo() ) {
    ss << i.first << ":" << i.second << " , ";
  }
  ss << " } ";
  ss << " substructure : { ";
  for( const auto& j : substructure() ){
    ss <<  j->index() << ":" << j->summarizedObjectCLID() << " , ";
  }
  ss << " } ";
  ss << " substructureExtended : { ";
  for( const auto& j : substructureExtended() ){
    ss <<  j->index() << ":" << j->summarizedObjectCLID() << " , ";
  }
  ss << " } ";
  ss << " lhcbIDs : { ";
  for( const auto& k : lhcbIDs() ){
    ss <<  k.lhcbID() << " , ";
  }
  ss << " } ";
  ss << " } ";
  return ss;
}
