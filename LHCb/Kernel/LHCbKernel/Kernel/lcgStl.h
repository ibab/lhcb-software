// $Id: lcgStl.h,v 1.13 2005-11-25 16:15:48 cattanem Exp $
#ifndef KERNEL_LCGSTL_H 
#define KERNEL_LCGSTL_H 1

// Additional classes to be added to automatically generated lcgdict

// Reflex include files
#include "Reflex/Builder/ReflexBuilder.h"

// begin include files
#include <vector>
#include <map>
#include <utility>
#include "Kernel/VeloChannelID.h"
#include "Kernel/STChannelID.h"
#include "Kernel/OTChannelID.h"
#include "Kernel/CaloCellID.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

// end include files

namespace {
  struct _Instantiations {
    // begin instantiations
    std::pair<int,int>                         m_std_pair_int_int;
    std::pair<long,double>                     m_std_pair_long_double;
    std::pair<int,double>                      m_std_pair_int_double;
    std::pair<double,double>                   m_std_pair_double_double;
    std::vector<std::pair<int,int> >           m_std_vector_std_pair_int_int;
    std::vector<std::pair<long,double> >       m_std_vector_std_pair_long_double;
    std::vector<std::pair<int,double> >        m_std_vector_std_pair_int_double;
    std::vector<std::pair<double,double> >     m_std_vector_std_pair_double_double;
    std::map<int,int>                          m_std_map_int_int;
    std::pair<LHCb::VeloChannelID,int>         m_std_pair_VeloChannelID_int;
    std::pair<LHCb::STChannelID,int>           m_std_pair_STChannelID_int;
    std::pair<LHCb::OTChannelID,int>           m_std_pair_OTChannelID_int;
    std::pair<double,LHCb::CaloCellID>               m_std_pair_double_CaloCellID;
    std::vector<std::pair<LHCb::VeloChannelID,int> > m_std_vector_std_pair_VeloChannelID_int;
    std::vector<std::pair<LHCb::STChannelID,int> >   m_std_vector_std_pair_STChannelID_int;
    std::vector<std::pair<LHCb::OTChannelID,int> >   m_std_vector_std_pair_OTChannelID_int;
    std::vector<std::pair<double,LHCb::CaloCellID> > m_std_vector_std_pair_double_CaloCellID;
    std::vector<HepGeom::Vector3D<double> >    m_std_vector_HepGeom_Vector3D_double;
    std::vector<HepGeom::Point3D<double> >     m_std_vector_HepGeom_Point3D_double;
    // end instantiations
  };
}

#endif // KERNEL_LCGSTL_H
