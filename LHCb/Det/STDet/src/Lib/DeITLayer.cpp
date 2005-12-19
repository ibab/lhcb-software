
#include "STDet/DeITLayer.h"
#include "STDet/DeITSector.h"
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
    m_sectors = getChildren<DeITLayer>();
    m_parent = getParent<DeITLayer>();
  }

  return sc;
}

DeITSector* DeITLayer::findSector(const STChannelID aChannel){

  // return pointer to the station from channel
  Children::iterator iter = std::find_if(m_sectors.begin() , m_sectors.end(), bind(&DeSTSector::contains, _1, aChannel));
  return (iter != m_sectors.end() ? *iter: 0);
}

DeITSector* DeITLayer::findSector(const Gaudi::XYZPoint& point) {

  // find the half module 
  Children::iterator iter = std::find_if(m_sectors.begin(), m_sectors.end(), bind(&DeSTSector::isInside, _1, point)); 
  return (iter != m_sectors.end() ? *iter: 0);
}



