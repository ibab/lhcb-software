#include "STDet/DeITDetector.h"
#include "DetDesc/IGeometryInfo.h"

#include "STDet/DeITStation.h"
#include "STDet/DeITLayer.h"
#include "STDet/DeITBox.h"
#include "Kernel/ITNames.h"

/** @file DeITDetector.cpp
*
*  Implementation of class: DeTTDetector
*
*    @author Matthew Needham
*/

using namespace LHCb;

DeITDetector::DeITDetector( const std::string& name ) :
  DeSTDetector( name )
{ 
  // constructer
  m_sectors.clear();
}

DeITDetector::~DeITDetector() {
  // initialize
}

const CLID& DeITDetector::clID () const
{
  return DeITDetector::classID() ;
}

StatusCode DeITDetector::initialize() {
  
  // initialize
  StatusCode sc = DeSTDetector::initialize();
  if (sc.isFailure() ){
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg; 
  }
  else {
    flatten();
    setNstrip(m_sectors.front()->nStrip()*m_sectors.size());
    m_sMap.reserve(m_sectors.size());
    for (Sectors::const_iterator iter = m_sectors.begin(); iter != m_sectors.end(); ++iter){
      m_sMap.insert((*iter)->elementID().uniqueSector(),*iter);
    } // iter
  }
  return sc;
  
}

DeSTSector* DeITDetector::findSector(const Gaudi::XYZPoint& aPoint) const {

  DeSTSector* aSector = nullptr;
  DeSTStation* tStation = findStation(aPoint);
  if (tStation){
    DeITStation* aStation = static_cast<DeITStation*>(tStation);
    DeITBox* aBox = aStation->findBox(aPoint);    
    if (aBox){
      DeITLayer* aLayer = aBox->findLayer(aPoint);
      if (aLayer){
        DeITLadder* aLadder = aLayer->findLadder(aPoint);
        if (aLadder){
          if (aLadder->isInside(aPoint)) aSector=aLadder->sector();
        }
      } // module   
    } // layer
  }   // station
  return aSector;
}

void DeITDetector::flatten(){

  m_sectors.reserve(400);
  std::vector<DeSTStation*>::iterator iterStation = m_stations.begin();
  for (; iterStation != m_stations.end() ; ++iterStation){
    DeITStation* tStation =  dynamic_cast<DeITStation*>(*iterStation);
    DeITStation::Children::const_iterator iterBox = tStation->boxes().begin();
    for (;iterBox != tStation->boxes().end(); ++iterBox){
      DeITBox* tBox = *iterBox;
      m_boxes.push_back(tBox);
      DeITBox::Children::const_iterator iterLayer = tBox->layers().begin();
      for (;iterLayer != tBox->layers().end(); ++iterLayer){
        DeITLayer* tLayer = *iterLayer;
        m_layers.push_back(tLayer);
	const DeSTLayer::Sectors& tSectors = tLayer->sectors();
        m_sectors.insert(m_sectors.begin(),tSectors.begin(),tSectors.end());
       }  // iterLayer
    }  // box
  } // iterStation
}


DeITBox* DeITDetector::findBox(const STChannelID aChannel) const
{
  // return pointer to the layer from channel
  auto iter = std::find_if(m_boxes.begin() , m_boxes.end(), 
                           [&](const DeITBox* b) { 
                              return b->contains(aChannel);
  });
  return iter != m_boxes.end() ? *iter: nullptr;
}

DeITBox* DeITDetector::findBox(const Gaudi::XYZPoint& point) const {
  // return pointer to a box from point
  auto iter = std::find_if( m_boxes.begin(), m_boxes.end(), 
                            [&](const DeITBox* b) { 
                                return b->isInside(point);
  });
  return iter != m_boxes.end() ? *iter: nullptr;
}

DeSTBaseElement* DeITDetector::findTopLevelElement(const std::string& nickname) const {

  // sanity check...
  if (nickname.find("IT") == std::string::npos) return nullptr;

  const STChannelID chan = ITNames().stringToChannel(nickname);
  if (chan.sector()){
    // its a sector
    return this->DeSTDetector::findSector(chan);
  }
  if (chan.layer() != 0u){
    // its a layer
    return findLayer(chan);
  }
  if (chan.detRegion() != 0u){
    // its a box
    return findBox(chan);
  } 
  if (chan.station() != 0u) {
    // its a station
    return findStation(chan);
  } 
  // too bad 
  return nullptr;
}
