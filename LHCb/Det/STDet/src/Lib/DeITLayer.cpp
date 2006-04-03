
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

using namespace LHCb;
using namespace boost::lambda;

DeITLayer::DeITLayer( const std::string& name ) :
  DeSTLayer( name )
{ 
  // constructer
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
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq; 
  }
  else {
    m_parent = getParent<DeITLayer>();
    STChannelID parentID = m_parent->elementID();
    STChannelID chan(STChannelID::typeIT, parentID.station(), id(), parentID.detRegion(), 0, 0);
    setElementID(chan);
    m_ladders = getChildren<DeITLayer>();
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
