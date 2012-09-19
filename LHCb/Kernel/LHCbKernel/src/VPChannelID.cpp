// local
#include "Kernel/VPChannelID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VPChannelID
//
// 2009-05-11 : Victor Coco
//-----------------------------------------------------------------------------

std::ostream& LHCb::VPChannelID::fillStream(std::ostream& s) const
{
  s << "{ " << type()
    << " VPChannelID : " << channelID()
    << " : pixel high precision=" << pixel_hp()
    << " pixel low precision=" << pixel_lp()
    << " chip=" << chip()
    << " ladder Z position wrt sensor Z=" << zpos()
    << " side position=" << sidepos()
    << " station=" << station();
  return s << " }";
}
