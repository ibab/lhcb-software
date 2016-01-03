#include "STDet/DeTTDetector.h"
#include "DetDesc/IGeometryInfo.h"

#include "STDet/DeTTStation.h"
#include "STDet/DeTTLayer.h"
#include "STDet/DeTTHalfModule.h"
#include "STDet/DeTTSector.h"

#include "Kernel/TTNames.h"

/** @file DeTTDetector.cpp
*
*  Implementation of class: DeTTDetector
*
*    @author Matthew Needham
*/

using namespace LHCb;

DeTTDetector::DeTTDetector( const std::string& name ) :
  DeSTDetector( name )  
{ 
  // constructer
  m_sectors.clear();
}

DeTTDetector::~DeTTDetector() = default;

const CLID& DeTTDetector::clID () const
{
  return DeTTDetector::classID() ;
}

StatusCode DeTTDetector::initialize() {

  // init the base class
 
  StatusCode sc = DeSTDetector::initialize();
  if (sc.isFailure() ){
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg; 
  }
  else {
    // make a flat list of sectors
    flatten();
    if (!m_sectors.empty()) {
      setNstrip(m_sectors.front()->nStrip()* m_sectors.size());
      m_sMap.reserve(m_sectors.size());
      for (Sectors::const_iterator iter = m_sectors.begin(); iter != m_sectors.end(); ++iter){
         m_sMap.insert((*iter)->elementID().uniqueSector(),*iter);
      } // iter
    }
  }
  return sc;

}

DeSTSector* DeTTDetector::findSector(const Gaudi::XYZPoint& aPoint) const {

  DeSTSector* aSector = nullptr;
  DeSTStation* tStation = findStation(aPoint);
  if (tStation){
    DeTTStation* aStation = static_cast<DeTTStation*>(tStation);
    DeTTLayer* aLayer = aStation->findLayer(aPoint);    
    if (aLayer){
      DeTTHalfModule* aModule = aLayer->findHalfModule(aPoint);
      if (aModule){
        aSector = aModule->findSector(aPoint);
      } // module   
    } // layer
  }   // station
  return aSector;
}

DeSTBaseElement* DeTTDetector::findTopLevelElement(const std::string& nickname) const {
  if (nickname.find("TT") == std::string::npos) return nullptr;

  const STChannelID chan = TTNames().stringToChannel(nickname);
  if (chan.sector()){
    // its a sector
    return this->DeSTDetector::findSector(chan);
  }
  if (chan.layer()){
    return findLayer(chan);
  }
  if (chan.station() != 0u) {
    // its a station
    return findStation(chan);
  }
  // too bad 
  return nullptr;
}

void DeTTDetector::flatten(){

  m_sectors.reserve(400);
  for (auto& station : m_stations) {
    DeTTStation* tStation =  dynamic_cast<DeTTStation*>(station);
    for (auto& tLayer : tStation->layers()) {
      m_layers.push_back(tLayer);
      const DeSTLayer::Sectors& tSectors = tLayer->sectors();
      m_sectors.insert(m_sectors.begin(),tSectors.begin(),tSectors.end());
    }
  }
}
