#include "STDet/DeUTDetector.h"
#include "DetDesc/IGeometryInfo.h"

#include "STDet/DeUTStation.h"
#include "STDet/DeUTLayer.h"
#include "STDet/DeUTModule.h"
#include "STDet/DeUTSector.h"

#include "Kernel/UTNames.h"

using namespace LHCb;

/** @file DeUTDetector.cpp
 *
 *  Implementation of class: DeUTDetector
 *
 *  All UT elements are modification for TT element classes
 *  that were originally written by Matt Needham)
 *
 *  @author Jianchun Wang, Matt Needham
 *  @date   2012-07-10
 *
 */

DeUTDetector::DeUTDetector( const std::string& name ) :
  DeSTDetector( name )
{
  m_sectors.clear();
}


DeUTDetector::~DeUTDetector() = default;


const CLID& DeUTDetector::clID () const
{
  return DeUTDetector::classID() ;
}


StatusCode DeUTDetector::initialize()
{
  // init the base class
  StatusCode sc = DeSTDetector::initialize();
  if ( sc.isFailure() ){
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg;
  }
  else {
    // make a flat list of sectors
    flatten();
    if ( !m_sectors.empty() ) {
      setNstrip(m_sectors.front()->nStrip()* m_sectors.size());
      m_sMap.reserve(m_sectors.size());
      for (Sectors::const_iterator iter = m_sectors.begin(); iter != m_sectors.end(); ++iter){
        m_sMap.insert((*iter)->elementID().uniqueSector(), *iter);
      }
    }
  }
  return sc;

}


DeSTSector* DeUTDetector::findSector(const Gaudi::XYZPoint& aPoint) const
{
  DeSTSector* aSector = nullptr;
  DeSTStation* tStation = findStation(aPoint);
  if ( tStation ){
    DeUTStation* aStation = dynamic_cast<DeUTStation*>(tStation);
    DeUTLayer* aLayer = aStation->findLayer(aPoint);
    if ( aLayer ){
      DeUTModule* aModule = aLayer->findModule(aPoint);
      if ( aModule ){
        aSector = aModule->findSector(aPoint);
      }
    }
  }
  return aSector;
}


DeSTBaseElement* DeUTDetector::findTopLevelElement(const std::string& nickname) const
{
  if ( nickname.find("UT") == std::string::npos ) return nullptr;

  const STChannelID chan = UTNames().stringToChannel(nickname);
  if ( chan.sector() != 0 )  return this->DeSTDetector::findSector(chan);
  if ( chan.layer() != 0 )   return findLayer(chan);
  if ( chan.station() != 0u) return findStation(chan);

  return nullptr;
}


void DeUTDetector::flatten()
{
  m_sectors.reserve(1200);
  std::vector<DeSTStation*>::iterator iterStation = m_stations.begin();
  for (; iterStation != m_stations.end() ; ++iterStation){
    DeUTStation* tStation =  dynamic_cast<DeUTStation*>(*iterStation);
    DeUTStation::Children::const_iterator iterLayer = tStation->layers().begin();
    for (;iterLayer != tStation->layers().end(); ++iterLayer){
      DeUTLayer* tLayer = *iterLayer;
      m_layers.push_back(tLayer);
      const DeSTLayer::Sectors& tSectors = tLayer->sectors();
      m_sectors.insert(m_sectors.begin(),tSectors.begin(),tSectors.end());
    }
  }
}
