// $Id: DeOTLayer.cpp,v 1.6 2006-03-30 21:45:34 janos Exp $

/// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

/// OTDet
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTQuarter.h"

/// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

/** @file DeOTLayer.cpp
 *
 *  Implementation of class :  DeOTLayer
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 */

using namespace boost::lambda;
using namespace LHCb;

DeOTLayer::DeOTLayer(const std::string& name) :
  DetectorElement( name ),
  m_stationID(0u),
  m_layerID(0u),
  m_elementID(0u),
  m_stereoAngle(0.0),
  m_quarters()
{ }

DeOTLayer::~DeOTLayer() {
}

const CLID& DeOTLayer::clID() const { 
  return DeOTLayer::classID() ; 
}

StatusCode DeOTLayer::initialize() {
  
  /// Loop over quarters
  IDetectorElement::IDEContainer::const_iterator iQuarter;
  for (iQuarter = this->childBegin(); iQuarter != this->childEnd();
       ++iQuarter) {  
    DeOTQuarter* quarter = dynamic_cast<DeOTQuarter*>(*iQuarter);
    if (quarter) m_quarters.push_back(quarter);
  } /// iQuarter
  
  IDetectorElement* station = this->parentIDetectorElement();
  m_stationID = (unsigned int) station->params()->param<int>("stationID");
  m_layerID = (unsigned int) param<int>("layerID");
  OTChannelID aChan(m_stationID, m_layerID, 0u, 0u, 0u, 0u);
  setElementID(aChan);

  m_stereoAngle = param<double>("stereoAngle");

  return StatusCode::SUCCESS;
}

/// Find the quarter for a given channel
DeOTQuarter* DeOTLayer::findQuarter(const OTChannelID aChannel)  const {  
  /// Find the quarter and return a pointer to the layer from channel
  Quarters::const_iterator iter = std::find_if(m_quarters.begin(), m_quarters.end(),
					       bind(&DeOTQuarter::contains, _1, aChannel));
  return (iter != m_quarters.end() ? (*iter) : 0);
}

/// Find the quarter for a given XYZ point
DeOTQuarter* DeOTLayer::findQuarter(const Gaudi::XYZPoint& aPoint) const {  
  /// Find the layer and return a pointer to the layer from channel
  Quarters::const_iterator iter = std::find_if(m_quarters.begin(), m_quarters.end(),
					       bind(&DeOTQuarter::isInsideEfficient, _1, aPoint));
  return (iter != m_quarters.end() ? (*iter) : 0);
}

/// Find the module for a given XYZ point
/// Slightly faster way to check if a point is inside a quarter
DeOTModule* DeOTLayer::findModule(const Gaudi::XYZPoint& aPoint) const {
  DeOTModule* module = 0;
  bool found = false;
  std::vector<DeOTQuarter*>::const_iterator iQuarter = m_quarters.begin();
  while (iQuarter != m_quarters.end() && !found) {
    if ((*iQuarter)->isInside(aPoint)) {
      module = (*iQuarter)->findModule(aPoint );
      if (!module) found = true;
    }
    ++iQuarter;
  }
  return module;
}
