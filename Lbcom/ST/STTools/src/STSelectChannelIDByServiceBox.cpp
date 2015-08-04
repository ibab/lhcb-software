#include "STSelectChannelIDByServiceBox.h"

#include <vector>
#include <algorithm>
#include <boost/foreach.hpp>

#include "Kernel/STChannelID.h"
#include "Kernel/ISTReadoutTool.h"

DECLARE_TOOL_FACTORY( STSelectChannelIDByServiceBox )
 
STSelectChannelIDByServiceBox::STSelectChannelIDByServiceBox( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent)
{
  setForcedInit();
  declareProperty("serviceBoxes", m_serviceBoxes); 
  declareInterface<ISTChannelIDSelector>(this);
}

STSelectChannelIDByServiceBox::~STSelectChannelIDByServiceBox()
{
  //destructer
}

StatusCode STSelectChannelIDByServiceBox::initialize() {
  
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

bool STSelectChannelIDByServiceBox::select( const LHCb::STChannelID& id ) const{
  return (*this) (id);  
}
  
bool STSelectChannelIDByServiceBox::operator()( const LHCb::STChannelID& id) const{
  return std::binary_search(m_sectors.begin(), m_sectors.end(), id.uniqueSector());
}
