// $Id: DeOTStation.cpp,v 1.1 2003-06-11 11:49:36 cattanem Exp $

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

// OTDet
#include "OTDet/DeOTStation.h"

/** @file DeOTStation.cpp
 *
 *  Implementation of class :  DeOTStation
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 */

DeOTStation::DeOTStation( const std::string& name ) :
  DetectorElement( name ),
  m_stationID(),
  m_z(0.0),
  m_zSize(0.0),
  m_layers()
{ }

DeOTStation::~DeOTStation()
{
}

const CLID& DeOTStation::clID () const 
{ 
  return DeOTStation::classID() ; 
}

StatusCode DeOTStation::initialize() 
{
  m_stationID = (unsigned int) this->userParameterAsInt("stationID");

  const IGeometryInfo* gi = this->geometry();
  HepPoint3D middlePoint(0,0,0);
  HepPoint3D globalPoint = gi->toGlobal(middlePoint);
  m_z = globalPoint.z();

  const ISolid* solid = gi->lvolume()->solid();
  const SolidBox* mainBox = dynamic_cast<const SolidBox*>( solid->cover() );
  if ( mainBox ) m_zSize = mainBox->zsize();

  //loop over layers
  IDetectorElement::IDEContainer::const_iterator iLayer;
  for (iLayer = this->childBegin(); iLayer != this->childEnd();
       ++iLayer) {  
    DeOTLayer* otLayer = dynamic_cast<DeOTLayer*>(*iLayer);
    if ( otLayer) {
      m_layers.push_back(otLayer);
    }
  }
  return StatusCode::SUCCESS;
}

DeOTLayer* DeOTStation::layer(unsigned int layerID) const
{
  DeOTLayer* otLayer = 0;
  std::vector<DeOTLayer*>::const_iterator iterLayer = m_layers.begin();
  while ( iterLayer != m_layers.end() &&
          !( (*iterLayer)->layerID() == layerID ) )
    iterLayer++;

  if ( iterLayer != m_layers.end()) otLayer = *iterLayer;
  return otLayer;
}

DeOTLayer* DeOTStation::layer(const HepPoint3D& point) const
{
  DeOTLayer* otLayer = 0;
  std::vector<DeOTLayer*>::const_iterator iterLayer = m_layers.begin();
  while ( iterLayer != m_layers.end() &&
          !( (*iterLayer)->isInside(point) ) ) iterLayer++;
  if ( iterLayer != m_layers.end() ) otLayer = *iterLayer;
  return otLayer;
}
