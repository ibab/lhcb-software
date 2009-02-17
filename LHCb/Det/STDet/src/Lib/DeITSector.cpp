#include "STDet/DeITSector.h"
#include "STDet/DeITSensor.h"
#include "STDet/DeSTSensor.h"
#include "STDet/DeITLadder.h"
#include "STDet/STDetFun.h"

#include "Kernel/ITNames.h"

/** @file DeITSector.cpp
*
*  Implementation of class :  DeITSector
*
*    @author Matthew Needham
*/



using namespace LHCb;

DeITSector::DeITSector( const std::string& name ) :
  DeSTSector( name ),
  m_parent(0),
  m_prodID(0)
{ 
  // constructer
}

DeITSector::~DeITSector() {
  // destructer
}

const CLID& DeITSector::clID () const
{
  return DeITSector::classID() ;
}

StatusCode DeITSector::initialize() {
  
  // initialize method
  MsgStream msg(msgSvc(), name() );

  StatusCode sc = DeSTSector::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq;
    return sc;
  }
  else {
    // get the parent
    m_parent = getParent<DeITSector>();

    const STChannelID parentID = m_parent->elementID();
    setElementID(parentID);
    m_nickname = ITNames().UniqueSectorToString(parentID);

    // build the id
    setID(parentID.sector());
    
    std::vector<DeITSensor*> sensors = getChildren<DeITSector>();
    std::sort(sensors.begin(),sensors.end(),STDetFun::SortByY());
    m_sensors.reserve(sensors.size());
    m_sensors.insert(m_sensors.begin(), sensors.begin(), sensors.end());
    m_thickness = m_sensors.front()->thickness();

    sc = registerConditionsCallbacks();
    if (sc.isFailure()){
      msg << MSG::ERROR << "Failed to registerConditions call backs" << endmsg;
      return sc;
    }
    sc = cacheInfo();
    if (sc.isFailure()){
      msg << MSG::ERROR << "Failed to cache geometry" << endmsg;
      return sc;
    }

  }
  return StatusCode::SUCCESS;
}

unsigned int DeITSector::prodID() const {
  return m_prodID;
}

std::string DeITSector::conditionsPathName() const
{
  std::string keys[3] =
    {
      "Top",
      "Bottom",
      "Side"
    }, name( nickname() );

  size_t loc;

  for (unsigned int i(0); i < 3; i++)
  {
    loc =  name.find(keys[i]);
    if ( loc < std::string::npos )
    {
      name.insert(loc + keys[i].length(), "Layer");
      break;
    }
  }

  return name;
}
