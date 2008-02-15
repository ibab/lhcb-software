// $Id: LHCbKernelDict.h,v 1.9 2008-02-15 14:40:08 cattanem Exp $
#ifndef DICT_LHCBKERNELDICT_H 
#define DICT_LHCBKERNELDICT_H 1

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
#include "Kernel/MuonLayout.h"
#include "Kernel/Trajectory.h"
#include "Kernel/CircleTraj.h"
#include "Kernel/LineTraj.h"
#include "Kernel/ParabolaTraj.h"
#include "Kernel/SiPositionInfo.h"

#include "Kernel/IAlgorithmCorrelations.h"
#include "Kernel/IBdlTool.h"
#include "Kernel/IBIntegrator.h"
#include "Kernel/ICheckTool.h"
#include "Kernel/ICondDBInfo.h"
#include "Kernel/IDetElemFinder.h"
#include "Kernel/IEventTimeDecoder.h"
#include "Kernel/IEvtTypeSvc.h"
#include "Kernel/IGenericTool.h"
#include "Kernel/IHltSummaryTool.h"
#include "Kernel/IInAcceptance.h"
#include "Kernel/IMCVeloFEType.h"
#include "Kernel/INormalizeTool.h"
#include "Kernel/ISiAmplifierResponse.h"
#include "Kernel/ISTClusterPosition.h"
#include "Kernel/ITrajPoca.h"
#include "Kernel/IUsedLHCbID.h"

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
    std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint>       m_std_pair_XYZPoint_XYZPoint;
    std::vector<LHCb::VeloChannelID>                 m_std_vector_VeloChannelID;
    std::vector<LHCb::LHCbID>::const_iterator        m_std_vector_LHCbID_const_iterator;
    // end instantiations
  };
}

#endif // DICT_LHCBKERNELDICT_H
