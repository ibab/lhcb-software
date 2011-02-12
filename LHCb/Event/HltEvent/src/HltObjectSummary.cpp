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
  for( Info::const_iterator i=numericalInfo().begin();i!=numericalInfo().end();++i){
    ss << i->first << ":" << i->second << " , ";
  }
  ss << " } ";

  ss << " substructure : { ";
  for( SmartRefVector < LHCb::HltObjectSummary >::const_iterator j=substructure().begin();j!=substructure().end();++j){
    ss <<  (*j)->index() << ":" << (*j)->summarizedObjectCLID() << " , ";
  }
  ss << " } ";

  ss << " lhcbIDs : { ";
  for( std::vector < LHCb::LHCbID  >::const_iterator k=lhcbIDs().begin();k!=lhcbIDs().end();++k){
    ss <<  k->lhcbID() << " , ";
  }
  ss << " } ";

  ss << " } ";

  return ss;

}
