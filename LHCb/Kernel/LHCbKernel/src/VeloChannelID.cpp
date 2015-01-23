#include "Kernel/VeloChannelID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloChannelID
//
// 2007-07-11 : Chris Jones
//-----------------------------------------------------------------------------

std::ostream& LHCb::VeloChannelID::fillStream(std::ostream& s) const
{
  s << "{ " << type()
    << " VeloChannelID : " << channelID()
    << " : strip=" << strip()
    << " sensor=" << sensor();
  return s << " }";
}
