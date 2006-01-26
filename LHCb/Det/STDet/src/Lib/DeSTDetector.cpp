
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTStation.h"
#include "DetDesc/IGeometryInfo.h"
#include "STDet/DeTTStation.h"
//STL
#include <algorithm>

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>


#include <iostream>
#include <typeinfo>

using namespace boost::lambda;
using namespace LHCb;

/** @file DeSTDetector.cpp
*
*  Implementation of class :  DeSTDetector
*
*    @author Matthew Needham
*/


DeSTDetector::DeSTDetector( const std::string& name ) :
  DetectorElement( name )
{ 
  // constructer
  m_sectors.reserve(512);
}


DeSTDetector::~DeSTDetector() {

}

StatusCode DeSTDetector::initialize() {

  MsgStream msg(msgSvc(), name() );

  StatusCode sc = DetectorElement::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq; 
  }
  else {
    // get the children
    IDetectorElement::IDEContainer::const_iterator iStation;
    for (iStation = this->childBegin(); this->childEnd() != iStation; ++iStation ) {
      DeSTStation* tStation = dynamic_cast<DeSTStation*>(*iStation);
      if (tStation != 0) { 
        m_stations.push_back(tStation); 
      }
    } // iStation

    if (!m_stations.empty()) { 
     setFirstStation(m_stations.front()->id());
    }
    else {
      // no stations - this is an error
      msg <<   MSG::ERROR << "No child elements !" << endreq;
      sc = StatusCode::FAILURE; 
    } // empty
  }
  return sc;
}

void DeSTDetector::setFirstStation(const unsigned int iStation){
  m_firstStation = iStation;
}

DeSTStation* DeSTDetector::findStation(const STChannelID aChannel){

  // return pointer to the station from channel
  std::vector<DeSTStation*>::iterator iter = std::find_if(m_stations.begin() , m_stations.end(), 
                                                          bind(&DeSTStation::contains, _1, aChannel));
  return (iter != m_stations.end() ? *iter: 0);
}

DeSTStation* DeSTDetector::findStation(const Gaudi::XYZPoint& point) {

  // return pointer to the station from point in global system
  std::vector<DeSTStation*>::iterator iter = std::find_if(m_stations.begin(), m_stations.end(), 
                                                        bind(&DeSTStation::isInside, _1, point)); 
  return (iter != m_stations.end() ? *iter: 0);
}
