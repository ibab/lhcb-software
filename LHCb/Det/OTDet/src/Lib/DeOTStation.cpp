// $Id: DeOTStation.cpp,v 1.7 2007-02-02 09:25:04 janos Exp $

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

using namespace boost::lambda;
using namespace LHCb;

DeOTStation::DeOTStation(const std::string& name) :
  DetectorElement( name ),
  m_stationID(0u),
  m_elementID(0u),
  m_layers() { /// constructor 
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
      m_mapIDLayer.insert((layer->elementID()).uniqueLayer(), layer);
    }
  }/// iLayer
  
  m_stationID = (unsigned int) param<int>("stationID");
  OTChannelID aChan(m_stationID, 0u, 0u, 0u, 0u, 0u);
  setElementID(aChan);
  
  return StatusCode::SUCCESS;
}

/// Find the layer for a given channel
DeOTLayer* DeOTStation::findLayer(const OTChannelID aChannel) {  
  /// Find the layer and return a pointer to the layer from channel
  MapIDLayer::iterator iL = m_mapIDLayer.find(aChannel.uniqueLayer());
  return (iL != m_mapIDLayer.end() ? iL->second: 0);
}

/// Find the layer for a given XYZ point
DeOTLayer* DeOTStation::findLayer(const Gaudi::XYZPoint& aPoint) {  
  /// Find the layer and return a pointer to the layer from XYZ point
  Layers::const_iterator iter = std::find_if(m_layers.begin(), m_layers.end(),
					     bind(&DetectorElement::isInside, _1, aPoint));
  return (iter != m_layers.end() ? (*iter) : 0);
}
