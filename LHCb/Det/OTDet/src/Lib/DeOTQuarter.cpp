// $Id: DeOTQuarter.cpp,v 1.9 2007-06-09 13:56:48 janos Exp $

/// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

/// GaudiKernel
//#include "GaudiKernel/SystemOfUnits.h"

/// OTDet
#include "OTDet/DeOTQuarter.h"
#include "OTDet/DeOTModule.h"

/** @file DeOTQuarter.cpp
 *
 *  Implementation of class :  DeOTQuarter
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 */

using namespace LHCb;

DeOTQuarter::DeOTQuarter(const std::string& name) :
  DetectorElement(name)
{ 
  /// Constructor
  m_modules.reserve(9);
}

const CLID& DeOTQuarter::clID() const { 
  return DeOTQuarter::classID() ; 
}

StatusCode DeOTQuarter::initialize() {
  
  /// Loop over modules
  IDetectorElement::IDEContainer::const_iterator iM;
  for  (iM = this->childBegin(); iM != this->childEnd(); ++iM) {  
    DeOTModule* module = dynamic_cast<DeOTModule*>(*iM);
    if (module)  {
      m_modules.push_back(module);
      m_mapModules.insert((module->elementID()).module(), module);
    }
  } /// iModule

  IDetectorElement* layer = this->parentIDetectorElement();
  IDetectorElement* station = layer->parentIDetectorElement();
  m_stationID = station->params()->param<int>("stationID");
  m_layerID = layer->params()->param<int>("layerID");
  m_quarterID = param<int>("quarterID");
  OTChannelID aChan(m_stationID, m_layerID, m_quarterID, 0u, 0u, 0u);
  setElementID(aChan);

  m_stereoAngle = layer->params()->param<double>("stereoAngle");
  
  return StatusCode::SUCCESS;
}

/// Find the module for a given XYZ point
const DeOTModule* DeOTQuarter::findModule(const Gaudi::XYZPoint& aPoint) const {
 /// Find the modules and return a pointer to the modules from channel
  auto iM = std::find_if(m_modules.begin(), m_modules.end(),
                         [&](const DetectorElement *e) 
                         { return e->isInside(aPoint); } );
  return iM != m_modules.end() ? *iM : nullptr;
}
