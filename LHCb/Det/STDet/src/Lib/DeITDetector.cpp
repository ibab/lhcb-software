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

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
using namespace boost::lambda;

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

DeSTSector* DeITDetector::findSector(const Gaudi::XYZPoint& aPoint){

  DeSTSector* aSector = 0;
  DeSTStation* tStation = findStation(aPoint);
  if (0 != tStation){
    DeITStation* aStation = static_cast<DeITStation*>(tStation);
    DeITBox* aBox = aStation->findBox(aPoint);    
    if (0 != aBox){
      DeITLayer* aLayer = aBox->findLayer(aPoint);
      if (0 != aLayer){
        DeITLadder* aLadder = aLayer->findLadder(aPoint);
        if (0 != aLadder){
          if (aLadder->isInside(aPoint) == true) aSector=aLadder->sector();
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


DeITBox* DeITDetector::findBox(const STChannelID aChannel)
{
  // return pointer to the layer from channel
  std::vector<DeITBox*>::iterator iter = std::find_if(m_boxes.begin() , 
                                                      m_boxes.end(), bind(&DeITBox::contains, _1, aChannel));
  return (iter != m_boxes.end() ? *iter: 0);
}

DeITBox* DeITDetector::findBox(const Gaudi::XYZPoint& point){
  // return pointer to a box from point
  std::vector<DeITBox*>::iterator iter = 
    std::find_if( m_boxes.begin(), m_boxes.end(), 
                 bind(&DeITBox::isInside, _1, point));
  return (iter != m_boxes.end() ? *iter: 0);
}

DeSTBaseElement* DeITDetector::findTopLevelElement(const std::string& nickname){

  // sanity check...
  if (nickname.find("IT") == std::string::npos) return 0;

  const STChannelID chan = ITNames().stringToChannel(nickname);
  if (chan.sector() != 0){
    // its a sector
    return this->DeSTDetector::findSector(chan);
  }
  else if (chan.layer() != 0u){
    // its a layer
    return findLayer(chan);
  }
  else if (chan.detRegion() != 0u){
    // its a box
    return findBox(chan);
  } 
  else if (chan.station() != 0u) {
    // its a station
    return findStation(chan);
  } 
  else {
    // too bad 
  }
  return 0;
}
