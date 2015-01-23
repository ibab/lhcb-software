#include "Kernel/OTChannelID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTChannelID
//
// 2007-07-11 : Chris Jones
//-----------------------------------------------------------------------------

std::ostream& LHCb::OTChannelID::fillStream(std::ostream& s) const
{
  return s << "{ OTChannelID : " << channelID()
           << " : TDCtime=" << tdcTime()
           << " straw=" << straw()
           << " module=" << module()
           << " quarter=" << quarter()
           << " layer=" << layer()
           << " station=" << station()
           << " uniqueStraw=" << uniqueStraw()
           << " uniqueModule=" << uniqueModule()
           << " uniqueQuarter=" << uniqueQuarter()
           << " uniqueLayer=" << uniqueLayer()
           << " }";
}
