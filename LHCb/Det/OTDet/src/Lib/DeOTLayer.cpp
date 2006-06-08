// $Id: DeOTLayer.cpp,v 1.10 2006-06-08 12:24:03 janos Exp $

/// GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"

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
  DetectorElement(name),
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
  IDetectorElement::IDEContainer::const_iterator iQ;
  for (iQ = this->childBegin(); iQ != this->childEnd(); ++iQ) {  
    DeOTQuarter* quarter = dynamic_cast<DeOTQuarter*>(*iQ);
    if (quarter) m_quarters.push_back(quarter);
  } /// iQ
  
  IDetectorElement* station = this->parentIDetectorElement();
  m_stationID = (unsigned int) station->params()->param<int>("stationID");
  m_layerID = (unsigned int) param<int>("layerID");
  OTChannelID aChan(m_stationID, m_layerID, 0u, 0u, 0u, 0u);
  setElementID(aChan);

  m_stereoAngle = param<double>("stereoAngle");

  /// cache planes
  cachePlane();

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
  /// Find the quarter and return a pointer to the quarter from channel
  // FIXME: isInsideEfficient is really efficient. So efficient that it's throwing
  //        away good hits in the stereo layers :-(
  // Quarters::const_iterator iter = std::find_if(m_quarters.begin(), m_quarters.end(),
  // 					       bind(&DeOTQuarter::isInsideEfficient, _1, aPoint));
  Quarters::const_iterator iter = std::find_if(m_quarters.begin(), m_quarters.end(),
   					       bind(&DetectorElement::isInside, _1, aPoint));
  return (iter != m_quarters.end() ? (*iter) : 0);
}

void DeOTLayer::cachePlane() {
  Gaudi::XYZPoint g1 = this->geometry()->toGlobal(Gaudi::XYZPoint(0.0, 0.0, 0.0));
  Gaudi::XYZPoint g2 = this->geometry()->toGlobal(Gaudi::XYZPoint(3.5*Gaudi::Units::m, 0.0, 0.0));
  Gaudi::XYZPoint g3 = this->geometry()->toGlobal(Gaudi::XYZPoint(0.0, 2.5*Gaudi::Units::m, 0.0));
  
  m_plane = Gaudi::Plane3D(g1, g2, g3);
}

/// Find the module for a given XYZ point
/// Slightly faster way to check if a point is inside a quarter
// DeOTModule* DeOTLayer::findModule(const Gaudi::XYZPoint& aPoint) const {
//   DeOTModule* module = 0;
//   bool found = false;
//   std::vector<DeOTQuarter*>::const_iterator iQuarter = m_quarters.begin();
//   while (iQuarter != m_quarters.end() && !found) {
//     if ((*iQuarter)->isInside(aPoint)) {
//       module = (*iQuarter)->findModule(aPoint );
//       if (!module) found = true;
//     }
//     ++iQuarter;
//   }
//   return module;
// }
