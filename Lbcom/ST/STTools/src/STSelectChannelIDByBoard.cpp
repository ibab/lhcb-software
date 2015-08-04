#include "STSelectChannelIDByBoard.h"

#include <vector>
#include <algorithm>
#include <boost/foreach.hpp>

#include "Kernel/STChannelID.h"
#include "Kernel/ISTReadoutTool.h"

DECLARE_TOOL_FACTORY( STSelectChannelIDByBoard )
 
STSelectChannelIDByBoard::STSelectChannelIDByBoard( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent)
{
  setForcedInit();
  declareProperty("Boards", m_boards); 
  declareInterface<ISTChannelIDSelector>(this);
}

STSelectChannelIDByBoard::~STSelectChannelIDByBoard()
{
  //destructer
}

StatusCode STSelectChannelIDByBoard::initialize() {
  
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // to save time get a list of all the ids on this box
  m_sectors.reserve(8*m_boards.size());
  BOOST_FOREACH(unsigned int id , m_boards) {
    std::vector<LHCb::STChannelID> boardSectors = readoutTool()->sectorIDs(STTell1ID(id));
    BOOST_FOREACH(LHCb::STChannelID chan, boardSectors){ 
      m_sectors.push_back(chan.uniqueSector());
    } // each sector
  } // for each board
  
  std::sort(m_sectors.begin(), m_sectors.end());

  return StatusCode::SUCCESS;
}

bool STSelectChannelIDByBoard::select( const LHCb::STChannelID& id ) const{
  return (*this) (id);  
}
  
bool STSelectChannelIDByBoard::operator()( const LHCb::STChannelID& id) const{
  return std::binary_search(m_sectors.begin(), m_sectors.end(), id.uniqueSector());
}
