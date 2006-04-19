
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
      setNstrip(m_sectors.front()->nStrip()* m_sectors.size());
      m_sMap.reserve(m_sectors.size());
      for (Sectors::const_iterator iter = m_sectors.begin(); iter != m_sectors.end(); ++iter){
         m_sMap.insert((*iter)->elementID().uniqueSector(),*iter);
      } // iter
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

/*
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
*/

void DeTTDetector::flatten(){

  std::vector<DeSTStation*>::iterator iterStation = m_stations.begin();
  for (; iterStation != m_stations.end() ; ++iterStation){
    DeTTStation* tStation =  dynamic_cast<DeTTStation*>(*iterStation);
    DeTTStation::Children::const_iterator iterLayer = tStation->layers().begin();
    for (;iterLayer != tStation->layers().end(); ++iterLayer){
      DeTTLayer* tLayer = *iterLayer;
      m_layers.push_back(tLayer);
      const DeSTLayer::Sectors& tSectors = tLayer->sectors();
      //      for (DeSTLayer::Sectors::const_iterator iter = tSectors.begin(); iter != tSectors.end(); ++iter){
      //   m_sectors.push_back(*iter);
      // } 
      m_sectors.insert(m_sectors.begin(),tSectors.begin(),tSectors.end());
    }  // iterLayer
  } // iterStation

}
