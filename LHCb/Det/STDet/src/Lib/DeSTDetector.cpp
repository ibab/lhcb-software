// $Id: DeSTDetector.cpp,v 1.20 2009-07-30 08:01:09 mneedham Exp $

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTStation.h"
#include "DetDesc/IGeometryInfo.h"
#include "STDet/DeTTStation.h"
#include "STDet/STDetFun.h"

//STL
#include <algorithm>

#include "GaudiKernel/GaudiException.h"

#include "Kernel/Trajectory.h"

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


DeSTDetector::~DeSTDetector() = default;

StatusCode DeSTDetector::initialize() {

  MsgStream msg(msgSvc(), name() );

  StatusCode sc = DetectorElement::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg; 
  }
  else {
    // get the children
    
    for (auto iStation =this->childBegin(); this->childEnd() != iStation; ++iStation ) {
      DeSTStation* tStation = dynamic_cast<DeSTStation*>(*iStation);
      if (tStation) { 
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
  if ( !sector ) {
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "sensitiveVolumeID: no sensitive volume at " 
        << point << endmsg;
    return -1;
  }
  return sector->elementID();
}

DeSTStation* DeSTDetector::findStation(const STChannelID aChannel) const {

  // return pointer to the station from channel
  auto iter = std::find_if(m_stations.begin(), m_stations.end(),
                           [&](const DeSTStation* s) { 
                               return s->contains(aChannel); 
  });
  return iter != m_stations.end() ? *iter: nullptr;
}

DeSTStation* DeSTDetector::findStation(const Gaudi::XYZPoint& point) const {

  // return pointer to the station from point in global system
  auto iter = std::find_if(m_stations.begin(), m_stations.end(), 
                           [&](const DeSTStation *s) {
                           return s->isInside(point);
  });
  return iter != m_stations.end() ? *iter: nullptr;
}

DeSTLayer* DeSTDetector::findLayer(const STChannelID aChannel) const
{
  // return pointer to the layer from channel
  auto iter = std::find_if( m_layers.begin(), m_layers.end(),
                            [&](const DeSTLayer* l) { return l->contains(aChannel); } );
  return iter != m_layers.end() ? *iter: nullptr;
}

DeSTLayer* DeSTDetector::findLayer(const Gaudi::XYZPoint& point) const {
  // return pointer to the layer from point
  auto iter = std::find_if( m_layers.begin(), m_layers.end(), 
                            [&](const DeSTLayer* l) { 
                                return l->isInside(point);
  });
  return iter != m_layers.end() ? *iter: nullptr;
}


DeSTSector* DeSTDetector::findSector(const std::string& nickname) const {
  // return pointer to the sector from the nickname
  auto iter = std::find_if( m_sectors.begin(), m_sectors.end(), 
                            STDetFun::equal_by_name<DeSTSector*>(nickname));
  return iter != m_sectors.end() ? *iter: nullptr;
}

DeSTLayer* DeSTDetector::findLayer(const std::string& nickname) const {
  // return pointer to the sector from the nickname
  auto iter = std::find_if( m_layers.begin(), m_layers.end(), 
                            STDetFun::equal_by_name<DeSTLayer*>(nickname));
  return iter != m_layers.end() ? *iter: nullptr;
}

DeSTStation* DeSTDetector::findStation(const std::string& nickname) const {
  // return pointer to the sector from the nickname
  auto iter = std::find_if( m_stations.begin(), m_stations.end(), 
                            STDetFun::equal_by_name<DeSTStation*>(nickname));
  return iter != m_stations.end() ? *iter: nullptr;
}

DeSTDetector::Sectors DeSTDetector::disabledSectors() const {
  Sectors disabled;
  const auto& vec = sectors();
  std::copy_if(vec.begin(), vec.end(), std::back_inserter(disabled),
               [](const DeSTSector* s) {
    return s->sectorStatus() == DeSTSector::ReadoutProblems;
  });
  return disabled; 
} 

std::vector<LHCb::STChannelID> DeSTDetector::disabledBeetles() const{

  std::vector<LHCb::STChannelID> disabledBeetles;
  const Sectors& vec = sectors();
  for (auto iterS = vec.begin(); iterS != vec.end(); ++iterS){
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

std::unique_ptr<LHCb::Trajectory> DeSTDetector::trajectory(const LHCb::LHCbID& id, 
                                                         const double offset) const 
{
  // look up the trajectory
  if ( !id.isST()){
     throw GaudiException( "The LHCbID is not of ST type!",
                           "DeSTDetector.cpp",
                           StatusCode::FAILURE );
  }
  DeSTSector* aSector = findSector(id.stID());
  if (!aSector){
    throw GaudiException( "Failed to find sector",
                          "DeSTDetector.cpp",
			  StatusCode::FAILURE );
  }
  return aSector->trajectory(id.stID(), offset);
}

std::unique_ptr<LHCb::Trajectory> DeSTDetector::trajectoryFirstStrip(const LHCb::LHCbID& id) const
{
  if ( !id.isST()){
     throw GaudiException( "The LHCbID is not of ST type!",
                           "DeSTDetector.cpp",
                           StatusCode::FAILURE );
  }
  
  DeSTSector* aSector = findSector(id.stID());
  if (!aSector){
    throw GaudiException( "Failed to find sector",
                          "DeSTDetector.cpp",
			  StatusCode::FAILURE );
  }
  return aSector->trajectoryFirstStrip();
}

std::unique_ptr<LHCb::Trajectory> DeSTDetector::trajectoryLastStrip(const LHCb::LHCbID& id) const
{
  if ( !id.isST()){
    throw GaudiException( "The LHCbID is not of ST type!",
                          "DeSTDetector.cpp",
                          StatusCode::FAILURE );
  }
  DeSTSector* aSector = findSector(id.stID());
  if (!aSector){
    throw GaudiException( "Failed to find sector",
                          "DeSTDetector.cpp",
                          StatusCode::FAILURE );
  }
  return aSector->trajectoryLastStrip();
}

double DeSTDetector::fractionActive() const {
  return std::accumulate(m_sectors.begin(), m_sectors.end(), 0.0,
                         [&](double fA, const DeSTSector* s) {
                             return fA + s->fractionActive();
                         }) / double(m_sectors.size());   
}
