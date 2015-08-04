// Event
#include "Kernel/STChannelID.h"

#include "STSelectChannelIDByStatus.h"
#include "STDet/StatusMap.h"

//boost
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>

DECLARE_TOOL_FACTORY( STSelectChannelIDByStatus)
 
STSelectChannelIDByStatus::STSelectChannelIDByStatus( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent)
{
  setForcedInit();
  std::vector<std::string> tmp = boost::assign::list_of("OK");
  declareProperty("allowedStatus", m_statusNames = tmp);
  declareInterface<ISTChannelIDSelector>(this);
}

STSelectChannelIDByStatus::~STSelectChannelIDByStatus()
{
  //destructer
}

StatusCode STSelectChannelIDByStatus::initialize() {

  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // to save time convert the strings to a list of status
  const ::Status::StatusToStringMap& statusMap = ::Status::statusDescription();
  BOOST_FOREACH(std::string statusString, m_statusNames) {
    info() << "Adding status " << statusString << " to list" << endmsg;
    // try to find the status in the map
	  ::Status::StatusToStringMap::const_iterator iterMap = statusMap.begin();
    while (iterMap != statusMap.end()){
      if (iterMap->second == statusString) break;
      ++iterMap;
    } // iterMap    

    if (iterMap == statusMap.end()){
      return Error( "Failed to find status " +statusString + " in map", StatusCode::FAILURE);
    }
    else {
      m_statusList.push_back(iterMap->first);
    }
  } // for each in string vector

  return StatusCode::SUCCESS;
}

bool STSelectChannelIDByStatus::select( const LHCb::STChannelID
& id ) const{
  return (*this) (id);  
}
  
bool STSelectChannelIDByStatus::operator()( const LHCb::STChannelID& id ) const{
  const DeSTSector::Status theStatus = findSector(id)->stripStatus(id);
  return (std::find(m_statusList.begin(), m_statusList.end(), theStatus) 
          !=  m_statusList.end() ? true: false );
}
