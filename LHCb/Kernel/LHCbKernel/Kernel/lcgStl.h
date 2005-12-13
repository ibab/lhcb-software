// $Id: lcgStl.h,v 1.16 2005-12-13 12:38:05 cattanem Exp $
#ifndef KERNEL_LCGSTL_H 
#define KERNEL_LCGSTL_H 1

// Additional classes to be added to automatically generated lcgdict

// Reflex include files
#include "Reflex/Builder/ReflexBuilder.h"

// begin include files
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "Kernel/VeloChannelID.h"
#include "Kernel/STChannelID.h"
#include "Kernel/OTChannelID.h"
#include "Kernel/CaloCellID.h"
#include "Kernel/MuonTileID.h"

// end include files

namespace {
  struct _Instantiations {
    // begin instantiations
    std::pair<int,int>                         m_std_pair_int_int;
    std::pair<long,double>                     m_std_pair_long_double;
    std::pair<int,double>                      m_std_pair_int_double;
    std::pair<double,double>                   m_std_pair_double_double;
    std::pair<std::string,int>                 m_std_pair_string_int;
    std::vector<std::pair<int,int> >           m_std_vector_std_pair_int_int;
    std::vector<std::pair<long,double> >       m_std_vector_std_pair_long_double;
    std::vector<std::pair<int,double> >        m_std_vector_std_pair_int_double;
    std::vector<std::pair<double,double> >     m_std_vector_std_pair_double_double;
    std::vector<std::pair<std::string,int> >   m_std_vector_std_pair_string_int;
    std::map<int,int>                          m_std_map_int_int;
    std::pair<LHCb::VeloChannelID,int>         m_std_pair_VeloChannelID_int;
    std::pair<LHCb::STChannelID,int>           m_std_pair_STChannelID_int;
    std::pair<LHCb::OTChannelID,int>           m_std_pair_OTChannelID_int;
    std::pair<double,LHCb::CaloCellID>               m_std_pair_double_CaloCellID;
    std::pair<LHCb::MuonTileID,int>           m_std_pair_MuonTileID_int;
    std::vector<std::pair<LHCb::VeloChannelID,int> > m_std_vector_std_pair_VeloChannelID_int;
    std::vector<std::pair<LHCb::STChannelID,int> >   m_std_vector_std_pair_STChannelID_int;
    std::vector<std::pair<LHCb::OTChannelID,int> >   m_std_vector_std_pair_OTChannelID_int;
    std::vector<std::pair<double,LHCb::CaloCellID> > m_std_vector_std_pair_double_CaloCellID;
    std::vector<std::pair<LHCb::MuonTileID,int> >    m_std_vector_std_pair_MuonTileID_int;
    // end instantiations
  };
}

#endif // KERNEL_LCGSTL_H
