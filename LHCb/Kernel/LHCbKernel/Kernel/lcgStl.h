// $Id: lcgStl.h,v 1.5 2004-05-05 14:14:01 cattanem Exp $
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

namespace {
  std::vector<std::pair<VeloChannelID,int> > a;
  std::pair<VeloChannelID,int>               b;
  std::vector<std::pair<ITChannelID,int> >   c;
  std::pair<ITChannelID,int>                 d;
  std::vector<std::pair<OTChannelID,int> >   e;
  std::pair<OTChannelID,int>                 f;
  std::vector<std::pair<int,int> >           g;
  std::pair<int,int>                         h;
  std::vector<std::pair<long,double> >       i;
  std::pair<long,double>                     j;
  std::vector<std::pair<int,double> >        k;
  std::pair<int,double>                      l;
  std::map<int,int>                          m;
  SmartRefVector<ContainedObject>            n;
}
#endif // KERNEL_LCGSTL_H
