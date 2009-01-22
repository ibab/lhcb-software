// $Id: STChannelID.cpp,v 1.2 2009-01-22 11:42:40 mneedham Exp $

// local
#include "Kernel/STChannelID.h"
#include <sstream>

//-----------------------------------------------------------------------------
// Implementation file for class : STChannelID
//
// 2007-08-11 : M Needham
//-----------------------------------------------------------------------------

std::string  LHCb::STChannelID::toString() const{
  std::ostringstream o;
  fillStream(o);
  return o.str();
}

std::ostream& LHCb::STChannelID::fillStream(std::ostream& s) const
{
  s << "{ ";
  if ( isTT() ) s << "TT ";
  if ( isIT() ) s << "IT ";
  s << "STChannelID : " << channelID()
    << " : type=" << type()
    << " strip=" << strip()
    << " sector=" << sector()
    << " detRegion=" << detRegion()
    << " layer=" << layer()
    << " station=" << station();
  return s << " }";
}
