// local
#include "Kernel/VeloPixChannelID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloPixChannelID
//
// 2009-05-11 : Victor Coco
//-----------------------------------------------------------------------------

std::ostream& LHCb::VeloPixChannelID::fillStream(std::ostream& s) const
{
  s << "{ " << type()
    << " VeloPixChannelID : " << channelID()
    << " : pixel high precision=" << pixel_hp()
    << " pixel low precision=" << pixel_lp()
    << " chip=" << chip()
    << " ladder Z position wrt sensor Z=" << zpos()
    << " side position=" << sidepos()
    << " station=" << station();
  return s << " }";
}
