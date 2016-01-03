#include "STDet/DeITLayer.h"
#include "STDet/DeITLadder.h"
#include "STDet/DeITBox.h"

/** @file DeITLayer.cpp
*
*  Implementation of class : DeITLayer
*
*    @author Matthew Needham
*/


//STL
#include <numeric>
#include <algorithm>

#include "Kernel/ITNames.h"

using namespace LHCb;

DeITLayer::DeITLayer( const std::string& name ) :
  DeSTLayer( name ),
  m_parent(NULL),
  m_firstSector(NULL),
  m_lastSector(NULL)
{ 
  // constructor
}

DeITLayer::~DeITLayer() {

}

const CLID& DeITLayer::clID () const
{
  return DeITLayer::classID() ;
}

StatusCode DeITLayer::initialize() {

  // initialize
  MsgStream msg(msgSvc(), name() );
  StatusCode sc = DeSTLayer::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg; 
  }
  else {
    m_parent = getParent<DeITLayer>();
    STChannelID parentID = m_parent->elementID();
    STChannelID chan(STChannelID::typeIT, parentID.station(), id(), parentID.detRegion(), 0, 0);
    setElementID(chan);
    m_nickname = ITNames().UniqueLayerToString(chan);
    m_ladders = getChildren<DeITLayer>();
   // set the first and last layers
    auto start = m_ladders.begin();
    auto childEnd = m_ladders.end();
    m_firstSector = (*start)->sector(); m_lastSector = (*start)->sector();
    for (; start != childEnd; ++start){
      unsigned int testID = (*start)->id();
      if (testID < m_firstSector->id()) m_firstSector = (*start)->sector();
      if (testID > m_lastSector->id()) m_lastSector = (*start)->sector();
    }
    flatten();
  }

  return sc;
}

DeITLadder* DeITLayer::findLadder(const STChannelID aChannel) const{

  // return pointer to the station from channel
  auto iter = std::find_if(m_ladders.begin() , m_ladders.end(), 
                           [&](const DeITLadder* l) { return l->contains(aChannel); });
  return iter != m_ladders.end() ? *iter: nullptr;
}

DeITLadder* DeITLayer::findLadder(const Gaudi::XYZPoint& point)  const{

  // find the half module 
  auto iter = std::find_if(m_ladders.begin(), m_ladders.end(), 
                           [&](const DeITLadder* l) { return l->isInside(point); });
  return iter != m_ladders.end() ? *iter: nullptr;
}


void DeITLayer::flatten() {

 std::transform( m_ladders.begin(), m_ladders.end(),
                 std::back_inserter(m_sectors),
                 [](const child_type* i) {
                     return i->sector();
 });
}

double DeITLayer::fractionActive() const {

  return std::accumulate(m_ladders.begin(), m_ladders.end(), 0.0,  
                         [](double fa, const DeITLadder* l) {
                             return fa + l->fractionActive();
                         }) / double(m_ladders.size());   
}

bool DeITLayer::contains(const LHCb::STChannelID aChannel) const{
  return (elementID().layer() == aChannel.layer() && 
         (m_parent->contains(aChannel))); 
}


