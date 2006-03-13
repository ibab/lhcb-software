// $Id: 

#include "VeloEvent/InternalVeloCluster.h"
 
#include "InternalVeloClusterPtrLessThan.h"

bool VeloDAQ::internalVeloClusterPtrLessThan(const LHCb::InternalVeloCluster* lhs, const LHCb::InternalVeloCluster* rhs)
{
  const LHCb::VeloChannelID& lhsId = lhs->channelID(0);
  const LHCb::VeloChannelID& rhsId = rhs->channelID(0);

  if (lhsId.sensor() == rhsId.sensor())
    return lhsId.strip() < rhsId.strip();
  else
    return lhsId.sensor() < rhsId.sensor();
}

