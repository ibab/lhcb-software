#include "Kernel/VLChannelID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VLChannelID
//
//-----------------------------------------------------------------------------

std::ostream& LHCb::VLChannelID::fillStream(std::ostream& s) const {
  s << "{ " << type()
    << " VLChannelID: " << channelID()
    << " : strip=" << strip()
    << " sensor=" << sensor();
  return s << " }";
}
