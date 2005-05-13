// $Id: DeOTStation.cpp,v 1.3 2005-05-13 16:09:41 marcocle Exp $

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
  m_stationID = (unsigned int) param<int>("stationID");

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
