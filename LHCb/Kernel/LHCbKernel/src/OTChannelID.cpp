// $Id: OTChannelID.cpp,v 1.1 2007-07-11 13:33:34 jonrob Exp $
// Include files

// local
#include "Kernel/OTChannelID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTChannelID
//
// 2007-07-11 : Chris Jones
//-----------------------------------------------------------------------------

std::ostream& LHCb::OTChannelID::fillStream(std::ostream& s) const
{
  s << "{ OTChannelID : " << channelID()
    << " : TDCtime=" << tdcTime()
    << " straw=" << straw()
    << " module=" << module()
    << " quarter=" << quarter()
    << " layer=" << layer()
    << " station=" << station();
  if ( uniqueLayer()   ) s << " uniqueLayer";
  if ( uniqueQuarter() ) s << " uniqueQuarter";
  if ( uniqueModule()  ) s << " uniqueModule";
  if ( uniqueStraw()   ) s << " uniqueStraw";
  s << " }";
  return s;
}
