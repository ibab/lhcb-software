
#include "STDet/DeTTLayer.h"
#include "STDet/DeTTHalfModule.h"
#include "STDet/DeTTStation.h"

#include "DetDesc/IGeometryInfo.h"


//STL
#include <algorithm>

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

using namespace LHCb;
using namespace boost::lambda;

/** @file DeTTLayer.cpp
*
*  Implementation of class :  DeTTLayer
*
*    @author Matthew Needham
*/


DeTTLayer::DeTTLayer( const std::string& name ) :
  DeSTLayer( name )
{ 
  // constructer
}


DeTTLayer::~DeTTLayer() {
  // destructer
}

const CLID& DeTTLayer::clID () const
{
  return DeTTLayer::classID() ;
}

StatusCode DeTTLayer::initialize() {

  // initialize method
  MsgStream msg(msgSvc(), name() );
  StatusCode sc = DeSTLayer::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq; 
  }
  else {
    m_modules = getChildren<DeTTLayer>();
    m_parent = getParent<DeTTLayer>();

    STChannelID parentID = m_parent->elementID();
    STChannelID chan(parentID.station(), id(), 0, 0, 0);
    setElementID(chan);
    
  }
  return sc;
}

DeTTHalfModule* DeTTLayer::findHalfModule(const STChannelID aChannel){

  // return pointer to the station from channel
  Children::iterator iter = std::find_if(m_modules.begin() , m_modules.end(), bind(&DeTTHalfModule::contains, _1, aChannel));
  return (iter != m_modules.end() ? *iter: 0);
}

DeTTHalfModule* DeTTLayer::findHalfModule(const Gaudi::XYZPoint& point) {

  // find the half module 
  Children::iterator iter = std::find_if(m_modules.begin(), m_modules.end(), 
                                                            bind(&DeTTHalfModule::isInside, _1, point)); 

  return (iter != m_modules.end() ? *iter: 0);
}












