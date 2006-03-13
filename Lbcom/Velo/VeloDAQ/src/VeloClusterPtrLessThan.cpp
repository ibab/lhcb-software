// $Id: VeloClusterPtrLessThan.cpp,v 1.2 2006-03-13 18:58:46 krinnert Exp $

#include "Event/VeloCluster.h"
 
#include "VeloClusterPtrLessThan.h"

bool VeloDAQ::veloClusterPtrLessThan(const LHCb::VeloCluster* lhs, const LHCb::VeloCluster* rhs)
{
  const LHCb::VeloChannelID& lhsId = lhs->channelID();
  const LHCb::VeloChannelID& rhsId = rhs->channelID();

  if (lhsId.sensor() == rhsId.sensor())
    return lhsId.strip() < rhsId.strip();
  else
    return lhsId.sensor() < rhsId.sensor();
}

