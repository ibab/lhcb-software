// $Id: lcgStl.h,v 1.6 2004-11-29 13:59:49 cattanem Exp $
#ifndef KERNEL_LCGSTL_H 
#define KERNEL_LCGSTL_H 1

// Dummy use of complex STL classes used in LHCb event model
// For lcg dictionary generation

// Include files
#include <vector>
#include <map>
#include <utility>
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/ContainedObject.h"
#include "Kernel/VeloChannelID.h"
#include "Kernel/ITChannelID.h"
#include "Kernel/OTChannelID.h"
#include "Kernel/CaloCellID.h"

namespace {
  std::vector<std::pair<int,int> >           a;
  std::vector<std::pair<long,double> >       b;
  std::vector<std::pair<int,double> >        c;
  std::vector<std::pair<double,double> >     d;
  std::map<int,int>                          e;
  SmartRefVector<ContainedObject>            f;
  std::vector<std::pair<VeloChannelID,int> > g;
  std::vector<std::pair<ITChannelID,int> >   h;
  std::vector<std::pair<OTChannelID,int> >   i;
  std::vector<std::pair<double,CaloCellID> > k;
}
#endif // KERNEL_LCGSTL_H
