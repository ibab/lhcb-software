// Event
#include "Event/STCluster.h"

#include "STSelectClustersByChannel.h"

#include "Kernel/STChannelID.h"
#include "Kernel/ISTChannelIDSelector.h"

DECLARE_TOOL_FACTORY( STSelectClustersByChannel)
 
STSelectClustersByChannel::STSelectClustersByChannel( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent)
{
  declareProperty("SelectorType", m_selectorType = "STRndmEffSelector");
  declareProperty("SelectorName", m_selectorName = "Selector");
  declareInterface<ISTClusterSelector>(this);
}

STSelectClustersByChannel::~STSelectClustersByChannel()
{
  //destructer
}

StatusCode STSelectClustersByChannel::initialize() {
  
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  m_selector = tool<ISTChannelIDSelector>(m_selectorType, m_selectorName, this);  

  info() << "Adding Tool: type: " << m_selectorType << " / name: " 
	 << m_selectorName << endmsg;

  return StatusCode::SUCCESS;
}

bool STSelectClustersByChannel::select( const LHCb::STCluster* cluster ) const{
  return (*this) (cluster);  
}
  
bool STSelectClustersByChannel::operator()( const LHCb::STCluster* cluster ) const{
  // just delegate to the channel selector
  return m_selector->select(cluster->channelID());
}
