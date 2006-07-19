// $Id: lcgStl.h,v 1.24 2006-07-19 13:09:16 cattanem Exp $
#ifndef KERNEL_LCGSTL_H 
#define KERNEL_LCGSTL_H 1

// Additional classes to be added to automatically generated lcgdict

// begin include files
#include <vector>
#include <utility>
#include "Kernel/VeloChannelID.h"
#include "Kernel/STChannelID.h"
#include "Kernel/OTChannelID.h"
#include "Kernel/CaloCellID.h"
#include "Kernel/MuonTileID.h"
#include "Kernel/Trajectory.h"
#include "Kernel/CircleTraj.h"
#include "Kernel/LineTraj.h"
#include "Kernel/ParabolaTraj.h"
#include "GaudiKernel/VectorMap.h"

// end include files

namespace {
  struct _Instantiations {
    // begin instantiations
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
    GaudiUtils::VectorMap<int,double>                m_vector_map_int_double;
    // end instantiations
  };
}

namespace { // Anonymous namespace 
  struct _instances {
    LHCb::CircleTraj m_CircleTraj; // class gets instantiated  
  };
}

#endif // KERNEL_LCGSTL_H
