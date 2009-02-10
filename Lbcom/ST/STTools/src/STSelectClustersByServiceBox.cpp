// $Id: STSelectClustersByServiceBox.cpp,v 1.1 2009-02-10 09:18:38 mneedham Exp $
 
// Kernel
#include "GaudiKernel/ToolFactory.h"
 
// Event
#include "Event/STCluster.h"

#include "STSelectClustersByServiceBox.h"

#include <vector>
#include <algorithm>
#include <boost/foreach.hpp>

#include "Kernel/STChannelID.h"
#include "Kernel/ISTReadoutTool.h"

DECLARE_TOOL_FACTORY( STSelectClustersByServiceBox);
 
STSelectClustersByServiceBox::STSelectClustersByServiceBox( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent)
{
  setForcedInit();
  declareProperty("serviceBoxes", m_serviceBoxes); 
  declareInterface<ISTClusterSelector>(this);
}

STSelectClustersByServiceBox::~STSelectClustersByServiceBox()
{
  //destructer
}

StatusCode STSelectClustersByServiceBox::initialize() {
  
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // to save time get a list of all the ids on this box
  m_sectors.reserve(8*m_serviceBoxes.size());
  BOOST_FOREACH(std::string box , m_serviceBoxes) {
    std::vector<LHCb::STChannelID> boxSectors = readoutTool()->sectorIDsOnServiceBox(box);
    BOOST_FOREACH(LHCb::STChannelID chan, boxSectors){ 
      m_sectors.push_back(chan.uniqueSector());
    } // each sector
  } // for each board
  
  std::sort(m_sectors.begin(), m_sectors.end());

  return StatusCode::SUCCESS;
}

bool STSelectClustersByServiceBox::select( const LHCb::STCluster* cluster ) const{
  return (*this) (cluster);  
}
  
bool STSelectClustersByServiceBox::operator()( const LHCb::STCluster* cluster ) const{
  const LHCb::STChannelID chan = cluster->channelID();
  return std::binary_search(m_sectors.begin(), m_sectors.end(), chan.uniqueSector());
}
