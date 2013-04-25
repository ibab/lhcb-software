#include "STDet/DeITLayer.h"
#include "STDet/DeITLadder.h"
#include "STDet/DeITBox.h"

/** @file DeITLayer.cpp
*
*  Implementation of class : DeITLayer
*
*    @author Matthew Needham
*/


// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>


//STL
#include <numeric>
#include <algorithm>

#include "Kernel/ITNames.h"

using namespace LHCb;
using namespace boost::lambda;

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
    DeITLayer::Children::const_iterator start = m_ladders.begin();
    DeITLayer::Children::const_iterator childEnd = m_ladders.end();
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

DeITLadder* DeITLayer::findLadder(const STChannelID aChannel){

  // return pointer to the station from channel
  Children::iterator iter = std::find_if(m_ladders.begin() , m_ladders.end(), bind(&DeITLadder::contains, _1, aChannel));
  return (iter != m_ladders.end() ? *iter: 0);
}

DeITLadder* DeITLayer::findLadder(const Gaudi::XYZPoint& point) {

  // find the half module 
  Children::iterator iter = std::find_if(m_ladders.begin(), m_ladders.end(), bind(&DeITLadder::isInside, _1, point)); 
  return (iter != m_ladders.end() ? *iter: 0);
}


void DeITLayer::flatten() {

 Children::const_iterator iterLadder = ladders().begin();
 for ( ; iterLadder !=  ladders().end() ; ++iterLadder ){
   DeSTSector* tSector = (*iterLadder)->sector();
   m_sectors.push_back(tSector);  
 } //sectors     

}

double DeITLayer::fractionActive() const {

  return std::accumulate(m_ladders.begin(), m_ladders.end(), 0.0,  _1 + bind(&DeITLadder::fractionActive,_2))/double(m_ladders.size());   
}

bool DeITLayer::contains(const LHCb::STChannelID aChannel) const{
  return (elementID().layer() == aChannel.layer() && 
         (m_parent->contains(aChannel))); 
}


