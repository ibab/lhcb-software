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
  m_prodID(0),
  m_prodIDString("ProdID")
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
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg;
    return sc;
  }
  else {
    // get the parent
    m_parent = getParent<DeITSector>();

    const STChannelID parentID = m_parent->elementID();
    setElementID(parentID);
    m_nickname = ITNames().UniqueSectorToString(parentID);
        
    // see if stereo
    m_isStereo = false;
    if (elementID().layer() == ITNames::V || elementID().layer() == ITNames::U) m_isStereo = true;

    // build the id
    setID(parentID.sector());
    
    std::vector<DeITSensor*> sensors = getChildren<DeITSector>();
    std::sort(sensors.begin(),sensors.end(),STDetFun::SortByY());
    m_sensors.reserve(sensors.size());
    m_sensors.insert(m_sensors.begin(), sensors.begin(), sensors.end());
    m_thickness = m_sensors.front()->thickness();

    // Can't test the version string, it's unfortunalety not trustable
    // it exists a DC07 version (why?) that doesn't contain ProdID
    if (versionString() != "DC06" && versionString() != "DC07")
    {
      sc = registerCondition(this, m_prodIDString,
                             &DeITSector::updateProdIDCondition, true);
      if (sc.isFailure() ){
        msg << MSG::ERROR << "Failed to register prodID conditions" << endmsg;
        return StatusCode::FAILURE; 
      }
    }

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

  name.replace(0, 2, "ITT");

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

StatusCode DeITSector::updateProdIDCondition()
{
  const Condition* aCon = condition(m_prodIDString);
  if (aCon == 0){
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to find condition" << endmsg;
    return StatusCode::FAILURE; 
  }
  m_prodID = aCon->param<int>("ProdID");

  return StatusCode::SUCCESS;
}
