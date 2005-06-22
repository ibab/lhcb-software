// $Id: lcgStl.h,v 1.8 2005-06-22 11:43:26 cattanem Exp $
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
#include "Kernel/LHCbID.h"
#include "Kernel/ParticleID.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

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
  std::vector<HepGeom::Vector3D<double> >    l;
  std::vector<HepGeom::Point3D<double> >     m;
  std::vector<CaloCellID>    v0;
  std::vector<ITChannelID>   v1;
  std::vector<LHCbID>        v2;
  std::vector<OTChannelID>   v3;
  std::vector<ParticleID>    v4;
  std::vector<RichSmartID>   v5;
  std::vector<VeloChannelID> v6;
}

#endif // KERNEL_LCGSTL_H
