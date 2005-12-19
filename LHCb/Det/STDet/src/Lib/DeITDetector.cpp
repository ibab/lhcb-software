#include "STDet/DeITDetector.h"
#include "DetDesc/IGeometryInfo.h"

#include "STDet/DeITStation.h"
#include "STDet/DeITLayer.h"
#include "STDet/DeITBox.h"
#include "STDet/DeITSector.h"

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
  MsgStream msg(msgSvc(), name() );
  StatusCode sc = DeSTDetector::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq; 
  }
  else {
    flatten();
    setPitch(m_sectors.front()->pitch()); 
    setNstrip(m_sectors.front()->nStrip());
  }
  return sc;
  
}

DeSTSector* DeITDetector::findSector(const Gaudi::XYZPoint& aPoint){

  DeSTSector* aSector = 0;
  DeSTStation* tStation = findStation(aPoint);
  if (0 != tStation){
    DeITStation* aStation = dynamic_cast<DeITStation*>(tStation);
    DeITBox* aBox = aStation->findBox(aPoint);    
    if (0 != aBox){
      DeITLayer* aLayer = aBox->findLayer(aPoint);
      if (0 != aLayer){
        aSector = aLayer->findSector(aPoint);
      } // module   
    } // layer
  }   // station
  return aSector;
}

DeSTSector* DeITDetector::findSector(const STChannelID aChannel){

  DeSTSector* aSector = 0;
  DeSTStation* tStation = findStation(aChannel);
  if (0 != tStation){
    DeITStation* aStation = dynamic_cast<DeITStation*>(tStation);
    DeITBox* aBox = aStation->findBox(aChannel);    
    if (0 != aBox){
      DeITLayer* aLayer = aBox->findLayer(aChannel);
      if (0 != aLayer){
        aSector = aLayer->findSector(aChannel);
      } // module   
    } // layer
  }   // station
  return aSector;
}

void DeITDetector::flatten(){

  std::vector<DeSTStation*>::iterator iterStation = m_stations.begin();
  for (; iterStation != m_stations.end() ; ++iterStation){
    DeITStation* tStation =  dynamic_cast<DeITStation*>(*iterStation);
    DeITStation::Children::const_iterator iterBox = tStation->boxes().begin();
    for (;iterBox != tStation->boxes().end(); ++iterBox){
      DeITBox* tBox = *iterBox;
      DeITBox::Children::const_iterator iterLayer = tBox->layers().begin();
      for (;iterLayer != tBox->layers().end(); ++iterLayer){
        DeITLayer* tLayer = *iterLayer;
        m_layers.push_back(tLayer);
        DeITLayer::Children::const_iterator iterSector = tLayer->sectors().begin();
        for ( ; iterSector !=  tLayer->sectors().end() ; ++iterSector ){
          DeSTSector* tSector = *iterSector;
          m_sectors.push_back(tSector);  
	} //sectors     
      } // half module
    }  // iterLayer
  } // iterStation
}
