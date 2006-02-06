
#include "STDet/DeTTDetector.h"
#include "DetDesc/IGeometryInfo.h"

#include "STDet/DeTTStation.h"
#include "STDet/DeTTLayer.h"
#include "STDet/DeTTHalfModule.h"
#include "STDet/DeTTSector.h"

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

DeTTDetector::~DeTTDetector() {
  // initialize
}

const CLID& DeTTDetector::clID () const
{
  return DeTTDetector::classID() ;
}

StatusCode DeTTDetector::initialize() {

  // init the base class
  MsgStream msg(msgSvc(), name() );
  msg<< MSG::DEBUG << "Init the TT detector " << endreq; 
  StatusCode sc = DeSTDetector::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq; 
  }
  else {
    // make a flat list of sectors
    flatten();
    if (!m_sectors.empty()) {
     setPitch(m_sectors.front()->pitch());
     setNstrip(m_sectors.front()->nStrip());
    }
  }
  return sc;

}

DeSTSector* DeTTDetector::findSector(const Gaudi::XYZPoint& aPoint){

  DeSTSector* aSector = 0;
  DeSTStation* tStation = findStation(aPoint);
  if (0 != tStation){
    DeTTStation* aStation = dynamic_cast<DeTTStation*>(tStation);
    DeTTLayer* aLayer = aStation->findLayer(aPoint);    
    if (0 != aLayer){
      DeTTHalfModule* aModule = aLayer->findHalfModule(aPoint);
      if (0 != aModule){
        aSector = aModule->findSector(aPoint);
      } // module   
    } // layer
  }   // station
  return aSector;
}

DeSTSector* DeTTDetector::findSector(const STChannelID aChannel){

  DeSTSector* aSector = 0;
  DeSTStation* tStation = findStation(aChannel);
  if (0 != tStation){
    DeTTStation* aStation = dynamic_cast<DeTTStation*>(tStation);
    DeTTLayer* aLayer = aStation->findLayer(aChannel);    
    if (0 != aLayer){
      DeTTHalfModule* aModule = aLayer->findHalfModule(aChannel);
      if (0 != aModule){
        aSector = aModule->findSector(aChannel);
      } // module   
    } // layer
  }   // station
  return aSector;
}

void DeTTDetector::flatten(){

  std::vector<DeSTStation*>::iterator iterStation = m_stations.begin();
  for (; iterStation != m_stations.end() ; ++iterStation){
    DeTTStation* tStation =  dynamic_cast<DeTTStation*>(*iterStation);
    DeTTStation::Children::const_iterator iterLayer = tStation->layers().begin();
    for (;iterLayer != tStation->layers().end(); ++iterLayer){
      DeTTLayer* tLayer = *iterLayer;
      m_layers.push_back(tLayer);
      DeTTLayer::Children::const_iterator iterModule = tLayer->halfModules().begin();
      for (;iterModule != tLayer->halfModules().end(); ++iterModule){
        DeTTHalfModule* tModule = *iterModule;
        DeTTHalfModule::Children::const_iterator iterSector = tModule->sectors().begin();
        for ( ; iterSector !=  tModule->sectors().end() ; ++iterSector ){
          DeSTSector* tSector = *iterSector;
          m_sectors.push_back(tSector);  
	} //sectors     
      } // half module
    }  // iterLayer
  } // iterStation


}
