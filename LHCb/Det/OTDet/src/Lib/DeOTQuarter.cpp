// $Id: DeOTQuarter.cpp,v 1.8 2007-02-02 09:25:04 janos Exp $

/// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

/// GaudiKernel
//#include "GaudiKernel/SystemOfUnits.h"

/// OTDet
#include "OTDet/DeOTQuarter.h"
#include "OTDet/DeOTModule.h"

/// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

/** @file DeOTQuarter.cpp
 *
 *  Implementation of class :  DeOTQuarter
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 */

using namespace boost::lambda;
using namespace LHCb;

DeOTQuarter::DeOTQuarter(const std::string& name) :
  DetectorElement(name),
  m_stationID(0u),
  m_layerID(0u),
  m_quarterID(0u),
  m_stereoAngle(0.0),
  m_modules() { 
  /// Constructor
}

DeOTQuarter::~DeOTQuarter() {
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
      m_mapIDModule.insert((module->elementID()).uniqueModule(), module);
    }
  } /// iModule
    
  IDetectorElement* layer = this->parentIDetectorElement();
  IDetectorElement* station = layer->parentIDetectorElement();
  m_stationID = (unsigned int) station->params()->param<int>("stationID");
  m_layerID = (unsigned int) layer->params()->param<int>("layerID");
  m_quarterID = (unsigned int) param<int>("quarterID");
  OTChannelID aChan(m_stationID, m_layerID, m_quarterID, 0u, 0u, 0u);
  setElementID(aChan);

  m_stereoAngle = layer->params()->param<double>("stereoAngle");
  
  return StatusCode::SUCCESS;
}

/// Find the module for a given channelID
DeOTModule* DeOTQuarter::findModule(const OTChannelID aChannel) const {
  /// Find the module and return a pointer to the module from channel
  MapIDModule::iterator iM = m_mapIDModule.find(aChannel.uniqueModule());
  return (iM != m_mapIDModule.end() ? iM->second: 0);
}

/// Find the module for a given XYZ point
DeOTModule* DeOTQuarter::findModule(const Gaudi::XYZPoint& aPoint) const {
 /// Find the modules and return a pointer to the modules from channel
  Modules::const_iterator iM = std::find_if(m_modules.begin(), m_modules.end(),
                                            bind(&DetectorElement::isInside, _1, aPoint));
  return (iM != m_modules.end() ? (*iM) : 0);
}
