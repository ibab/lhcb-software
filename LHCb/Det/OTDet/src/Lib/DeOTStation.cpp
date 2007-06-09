// $Id: DeOTStation.cpp,v 1.8 2007-06-09 13:56:49 janos Exp $

/// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

/// OTDet
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTLayer.h"

/// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

/** @file DeOTStation.cpp
 *
 *  Implementation of class :  DeOTStation
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 */

using namespace LHCb;
using namespace boost::lambda;

DeOTStation::DeOTStation(const std::string& name) :
  DetectorElement( name ),
  m_stationID(0u),
  m_elementID(0u),
  m_layers()
{ /// constructor 
  m_layers.reserve(4);
}

DeOTStation::~DeOTStation() { 
  /// destrcutor 
}

const CLID& DeOTStation::clID() const { 
  return DeOTStation::classID() ; 
}

StatusCode DeOTStation::initialize() {  
  /// Loop over layers
  IDetectorElement::IDEContainer::const_iterator iL;
  for (iL = this->childBegin(); iL != this->childEnd(); ++iL) {  
    DeOTLayer* layer = dynamic_cast<DeOTLayer*>(*iL);
    if (layer) {
      m_layers.push_back(layer);
      m_mapLayers.insert((layer->elementID()).layer(), layer);
    }
  }/// iLayer

  m_stationID = (unsigned int) param<int>("stationID");
  OTChannelID aChan(m_stationID, 0u, 0u, 0u, 0u, 0u);
  setElementID(aChan);
  
  return StatusCode::SUCCESS;
}

/// Find the layer for a given XYZ point
const DeOTLayer* DeOTStation::findLayer(const Gaudi::XYZPoint& aPoint) const {  
  /// Find the layer and return a pointer to the layer from XYZ point
  Layers::const_iterator iter = std::find_if(m_layers.begin(), m_layers.end(),
                                             bind(&DetectorElement::isInside, _1, aPoint));
  return (iter != m_layers.end() ? (*iter) : 0);
}
