// $Id: VeloClusterPtrLessThan.cpp,v 1.1 2006-02-23 18:56:35 krinnert Exp $

#include "Event/VeloCluster.h"

#include "VeloClusterPtrLessThan.h"

bool VeloDAQ::veloClusterPtrLessThan(const LHCb::VeloCluster* lhs, const LHCb::VeloCluster* rhs)
{
  return lhs->channelID().strip() < rhs->channelID().strip();
}

