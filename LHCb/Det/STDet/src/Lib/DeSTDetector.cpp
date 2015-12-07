// $Id: DeSTDetector.cpp,v 1.20 2009-07-30 08:01:09 mneedham Exp $

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTStation.h"
#include "DetDesc/IGeometryInfo.h"
#include "STDet/DeTTStation.h"
#include "STDet/STDetFun.h"

//STL
#include <algorithm>

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>


#include "GaudiKernel/GaudiException.h"

#include "Kernel/Trajectory.h"

using namespace boost::lambda;
using namespace LHCb;

/** @file DeSTDetector.cpp
*
*  Implementation of class :  DeSTDetector
*
*    @author Matthew Needham
*/


DeSTDetector::DeSTDetector( const std::string& name ) :
  DetectorElement( name ),
  m_firstStation(0u),
  m_nStrip(0u)
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
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg; 
  }
  else {
    // get the children
    IDetectorElement::IDEContainer::const_iterator iStation =this->childBegin();
    for ( ; this->childEnd() != iStation; ++iStation ) {
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
      msg <<   MSG::ERROR << "No child elements !" << endmsg;
      sc = StatusCode::FAILURE; 
    } // empty
  }

  return sc;
}

void DeSTDetector::setFirstStation(const unsigned int iStation){
  m_firstStation = iStation;
}

int DeSTDetector::sensitiveVolumeID(const Gaudi::XYZPoint& point) const 
{
  DeSTDetector* nonConstThis = const_cast<DeSTDetector*>(this);
  DeSTSector* sector = nonConstThis->findSector( point );
  if ( sector == 0 ) {
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "sensitiveVolumeID: no sensitive volume at " 
        << point << endmsg;
    return -1;
  }
  return sector->elementID();
}

DeSTStation* DeSTDetector::findStation(const STChannelID aChannel){

  // return pointer to the station from channel
  std::vector<DeSTStation*>::iterator iter = std::find_if(m_stations.begin(),
                                                          m_stations.end(),
                                    bind(&DeSTStation::contains, _1, aChannel));
  return (iter != m_stations.end() ? *iter: 0);
}

DeSTStation* DeSTDetector::findStation(const Gaudi::XYZPoint& point) {

  // return pointer to the station from point in global system
  std::vector<DeSTStation*>::iterator iter = std::find_if(m_stations.begin(),
                                                          m_stations.end(), 
                                                          bind(&DeSTStation::isInside, _1, point));

  return (iter != m_stations.end() ? *iter: 0);
}

DeSTLayer* DeSTDetector::findLayer(const STChannelID aChannel)
{
  // return pointer to the layer from channel
  std::vector<DeSTLayer*>::iterator iter = m_layers.begin();
  while (iter != m_layers.end() && (*iter)->contains(aChannel) == false){
    ++iter;
  } // iter
  return (iter != m_layers.end() ? *iter: 0);
}

DeSTLayer* DeSTDetector::findLayer(const Gaudi::XYZPoint& point){
  // return pointer to the layer from point
  std::vector<DeSTLayer*>::iterator iter = 
    std::find_if( m_layers.begin(), m_layers.end(), 
                 bind(&DeSTLayer::isInside, _1, point));
  return (iter != m_layers.end() ? *iter: 0);
}


DeSTSector* DeSTDetector::findSector(const std::string& nickname){
  // return pointer to the sector from the nickname
  std::vector<DeSTSector*>::iterator iter = 
    std::find_if( m_sectors.begin(), m_sectors.end(), STDetFun::equal_by_name<DeSTSector*>(nickname));
  return (iter != m_sectors.end() ? *iter: 0);
}

DeSTLayer* DeSTDetector::findLayer(const std::string& nickname){
  // return pointer to the sector from the nickname
  std::vector<DeSTLayer*>::iterator iter = 
    std::find_if( m_layers.begin(), m_layers.end(), STDetFun::equal_by_name<DeSTLayer*>(nickname));
  return (iter != m_layers.end() ? *iter: 0);
}

DeSTStation* DeSTDetector::findStation(const std::string& nickname){
  // return pointer to the sector from the nickname
  std::vector<DeSTStation*>::iterator iter = 
    std::find_if( m_stations.begin(), m_stations.end(), STDetFun::equal_by_name<DeSTStation*>(nickname));
  return (iter != m_stations.end() ? *iter: 0);
}

DeSTDetector::Sectors DeSTDetector::disabledSectors() const{

  Sectors disabled;
  const Sectors& vec = sectors();
  Sectors::const_iterator iterS = vec.begin();
  for (; iterS != vec.end(); ++iterS){
    if ((*iterS)->sectorStatus() == DeSTSector::ReadoutProblems) disabled.push_back(*iterS);
  } // iterS
  return disabled; 
} 

std::vector<LHCb::STChannelID> DeSTDetector::disabledBeetles() const{

  std::vector<LHCb::STChannelID> disabledBeetles;
  const Sectors& vec = sectors();
  Sectors::const_iterator iterS = vec.begin();
  for (; iterS != vec.end(); ++iterS){
    std::vector<DeSTSector::Status> bStatus = (*iterS)->beetleStatus();
    for (unsigned int i = 0; i < bStatus.size(); ++i) {
      if (bStatus[i] == DeSTSector::ReadoutProblems){
	const unsigned int firstStripOnBeetle = (i*LHCbConstants::nStripsInBeetle) + 1;
        disabledBeetles.push_back((*iterS)->stripToChan(firstStripOnBeetle)); 
      }
    } //i 
  } // iterS
  return disabledBeetles; 
} 

std::auto_ptr<LHCb::Trajectory> DeSTDetector::trajectory(const LHCb::LHCbID& id, 
                                                         const double offset) 
{
  // look up the trajectory
 
 std::auto_ptr<LHCb::Trajectory> tTraj;

  if ( !id.isST()){
     throw GaudiException( "The LHCbID is not of ST type!",
                           "DeSTDetector.cpp",
                           StatusCode::FAILURE );
  }
  
  DeSTSector* aSector = findSector(id.stID());
  if (aSector != 0){
    tTraj = aSector->trajectory(id.stID(), offset);
  }
  else {
    throw GaudiException( "Failed to find sector",
                          "DeSTDetector.cpp",
			  StatusCode::FAILURE );
  }
  return tTraj;
}

std::auto_ptr<LHCb::Trajectory> DeSTDetector::trajectoryFirstStrip(const LHCb::LHCbID& id) 
{
  std::auto_ptr<LHCb::Trajectory> tTraj;

  if ( !id.isST()){
     throw GaudiException( "The LHCbID is not of ST type!",
                           "DeSTDetector.cpp",
                           StatusCode::FAILURE );
  }
  
  DeSTSector* aSector = findSector(id.stID());
  if (aSector != 0){
    tTraj = aSector->trajectoryFirstStrip();
  }
  else {
    throw GaudiException( "Failed to find sector",
                          "DeSTDetector.cpp",
			  StatusCode::FAILURE );
  }
  return tTraj;  
}

std::auto_ptr<LHCb::Trajectory> DeSTDetector::trajectoryLastStrip(const LHCb::LHCbID& id) 
{
  std::auto_ptr<LHCb::Trajectory> tTraj;

  if ( !id.isST()){
     throw GaudiException( "The LHCbID is not of ST type!",
                           "DeSTDetector.cpp",
                           StatusCode::FAILURE );
  }
  
  DeSTSector* aSector = findSector(id.stID());
  if (aSector != 0){
    tTraj = aSector->trajectoryLastStrip();
  }
  else {
    throw GaudiException( "Failed to find sector",
                          "DeSTDetector.cpp",
			  StatusCode::FAILURE );
  }
  return tTraj;  
}

double DeSTDetector::fractionActive() const {

  return std::accumulate(m_sectors.begin(), m_sectors.end(), 0.0,  _1 + bind(&DeSTSector::fractionActive,_2))/double(m_sectors.size());   
}


