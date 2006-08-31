// $Id: lcgStl.h,v 1.25 2006-08-31 14:53:26 erodrigu Exp $
#ifndef KERNEL_LCGSTL_H 
#define KERNEL_LCGSTL_H 1

// Additional classes to be added to automatically generated lcgdict

// begin include files
#include <vector>
#include <utility>
#include "GaudiKernel/VectorMap.h"
#include "Kernel/VeloChannelID.h"
#include "Kernel/STChannelID.h"
#include "Kernel/OTChannelID.h"
#include "Kernel/CaloCellID.h"
#include "Kernel/MuonTileID.h"
#include "Kernel/Trajectory.h"
#include "Kernel/CircleTraj.h"
#include "Kernel/LineTraj.h"
#include "Kernel/ParabolaTraj.h"
#include "Kernel/SiPositionInfo.h"

#include "Kernel/ISTClusterPosition.h"
#include "Kernel/IVeloClusterPosition.h"

// end include files

namespace {
  struct _Instantiations {
    // begin instantiations
    GaudiUtils::VectorMap<int,double>                m_vector_map_int_double;
    std::pair<LHCb::VeloChannelID,int>               m_std_pair_VeloChannelID_int;
    std::pair<LHCb::STChannelID,int>                 m_std_pair_STChannelID_int;
    std::pair<LHCb::OTChannelID,int>                 m_std_pair_OTChannelID_int;
    std::pair<double,LHCb::CaloCellID>               m_std_pair_double_CaloCellID;
    std::pair<LHCb::MuonTileID,int>                  m_std_pair_MuonTileID_int;
    std::vector<std::pair<LHCb::VeloChannelID,int> > m_std_vector_std_pair_VeloChannelID_int;
    std::vector<std::pair<LHCb::STChannelID,int> >   m_std_vector_std_pair_STChannelID_int;
    std::vector<std::pair<LHCb::OTChannelID,int> >   m_std_vector_std_pair_OTChannelID_int;
    std::vector<std::pair<double,LHCb::CaloCellID> > m_std_vector_std_pair_double_CaloCellID;
    std::vector<std::pair<LHCb::MuonTileID,int> >    m_std_vector_std_pair_MuonTileID_int;
    std::auto_ptr<LHCb::Trajectory>                  m_auto_ptr_Trajectory;
    LHCb::SiPositionInfo<LHCb::STChannelID>          m_SiPositionInfo_STChannelID;
    LHCb::SiPositionInfo<LHCb::VeloChannelID>        m_SiPositionInfo_VeloChannelID;
    // end instantiations
  };
}

#endif // KERNEL_LCGSTL_H
