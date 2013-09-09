// local
#include "Kernel/VPChannelID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VPChannelID
//
// 2009-05-11 : Victor Coco
//-----------------------------------------------------------------------------

std::ostream& LHCb::VPChannelID::fillStream(std::ostream& s) const {
  s << "{ " << " VPChannelID : " << channelID()
    << " : row = " << row() << " col = " << col()
    << " chip = " << chip() << " station = " << station();
  return s << " }";
}
