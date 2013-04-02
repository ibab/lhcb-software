#include "STDet/DeUTSector.h"
#include "STDet/DeUTModule.h"
#include "STDet/DeUTSensor.h"
#include "STDet/DeSTSensor.h"
#include "STDet/STDetFun.h"

#include "Kernel/UTNames.h"

#ifdef __INTEL_COMPILER         // Disable ICC remark from Boost
  #pragma warning(disable:2259) // non-pointer conversion may lose significant bits
#endif
#include "boost/lexical_cast.hpp"

using namespace LHCb;

/** @file DeUTSector.cpp
 *
 *  Implementation of class :  DeUTSector
 *
 *  @author Jianchun Wang, Matt Needham
 *  @date   2012-07-10
 *
 */

DeUTSector::DeUTSector( const std::string& name ) :
  DeSTSector( name ),
  m_parent(NULL),
  m_row(0u)
{ 
}


DeUTSector::~DeUTSector()
{
}


const CLID& DeUTSector::clID () const
{
  return DeUTSector::classID() ;
}


std::string DeUTSector::hybridType() const
{
  return m_hybridType;
}


StatusCode DeUTSector::initialize()
{
  // initialize method
  MsgStream msg(msgSvc(), name() );

  StatusCode sc = DeSTSector::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg;
    return sc;
  }
  else {
    // get the parent
    m_parent = getParent<DeUTSector>();

    unsigned int tSize = m_parent->numSectorsExpected();

    // sub id
    const unsigned subID = param<int>("subID");

    // sector number needs info from mother
    if (m_parent->moduleRotZ() == "No") {
      setID(m_parent->firstSector() + subID - 1);
    } else {
      setID(m_parent->firstSector() + tSize - subID);
    }
   
    // row..
    m_row = id() - m_parent->firstSector() + 1;

    // build the id
    const STChannelID parentID = m_parent->elementID();
    STChannelID chan(STChannelID::typeUT,parentID.station(),parentID.layer(), 
                     parentID.detRegion(),  id(), 0);
    setElementID(chan);

    // get the nickname
    m_nickname = UTNames().UniqueSectorToString(chan); 

    // see if stereo
    const unsigned int layer = chan.layer();
    m_isStereo = false;
    if ( (chan.station() == UTNames::UTa && layer == 2) ||
         (chan.station() == UTNames::UTb && layer == 1))  m_isStereo = true; 

    // get the attached sensors
    std::vector<DeUTSensor*> sensors = getChildren<DeUTSector>();
    std::sort(sensors.begin(),sensors.end(),STDetFun::SortByY());
    m_sensors.reserve(sensors.size());
    m_sensors.insert(m_sensors.begin(), sensors.begin(), sensors.end());
    m_thickness = m_sensors.front()->thickness();

    m_hybridType = "N";
    if ( DeSTSector::type() == "Dual" ) m_hybridType = "D";
    if ( DeSTSector::type() == "Quad" ) m_hybridType = "Q";
        
    using namespace boost;
    std::string region = lexical_cast<std::string>(chan.detRegion());
    std::string col    = moduleNumber( chan.detRegion(), parentID.station());
    std::string sector = lexical_cast<std::string>(subID);
    
    m_conditionPathName = UTNames().UniqueLayerToString(chan) + "LayerR"
      + region + "Module" + col + "Sector" + sector;

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

//   std::cout << " Sector :  "  << name()
//             << "\n Chan " << elementID()
//             << "\n Nickname: " << nickname()
//             << "\n ID " << id() 
//             << "\n type  " << type() 
//             << "\n pitch " << pitch() 
//             << "\n strip " << nStrip()
//             << "\n capacitance " << capacitance()/Gaudi::Units::picofarad
//             << "\n thickness " << thickness()
//             << "\n measEff " << measEff()
//             << "\n dead width " << deadWidth()
//             << "\n center " << globalCentre()
//             << "\n Sector status " << sectorStatus() 
//             << "\n fraction active " << fractionActive() 
//             << "\n column " << column()
//             << "\n row " << row()
//             << "\n prodID " << prodID()
//             << "\n conditionsPath " << conditionsPathName()
//             << "\n moduleType " << moduleType()
//             << std::endl; 

  return StatusCode::SUCCESS;
}


unsigned int DeUTSector::prodID() const
{
  return m_parent->prodID();
}


std::string DeUTSector::conditionsPathName() const
{
  return m_conditionPathName;
}


std::string DeUTSector::moduleNumber(const unsigned int& reg, const unsigned int& station) const
{
  using namespace boost;

  int col = 0;

  // UTaX or UTaU
  if ( station == 1 ) {
    if ( reg == 1 ) col = column();
    if ( reg == 2 ) col = column() - 6;
    if ( reg == 3 ) col = column() - 6 - 6;
  }

  // UTbV or UTbX
  if ( station == 2 ) {
    if ( reg == 1 ) col = column();
    if ( reg == 2 ) col = column() - 7;
    if ( reg == 3 ) col = column() - 7 - 6;
  }

  return lexical_cast<std::string>(col);
}
