// $Id: STChannelID.cpp,v 1.1 2007-07-11 13:33:34 jonrob Exp $

// local
#include "Kernel/STChannelID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STChannelID
//
// 2007-07-11 : Chris Jones
//-----------------------------------------------------------------------------

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
  if ( uniqueLayer()     ) s << " uniqueLayer";
  if ( uniqueDetRegion() ) s << " uniqueDetRegion";
  if ( uniqueSector()    ) s << " uniqueSector";
  return s << " }";
}
