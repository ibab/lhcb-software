// $Id: DeOTLayer.cpp,v 1.12 2007-02-13 11:56:18 janos Exp $

/// GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

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
    if (quarter) {
      m_quarters.push_back(quarter);
      m_mapIDQuarter.insert((quarter->elementID()).uniqueQuarter(), quarter);
    }
  } /// iQ
  
  IDetectorElement* station = this->parentIDetectorElement();
  m_stationID = (unsigned int) station->params()->param<int>("stationID");
  m_layerID = (unsigned int) param<int>("layerID");
  OTChannelID aChan(m_stationID, m_layerID, 0u, 0u, 0u, 0u);
  setElementID(aChan);

  m_stereoAngle = param<double>("stereoAngle");
  
  /// Update and chache planes
  MsgStream msg(msgSvc(), name() );
  try {
    msg << MSG::DEBUG << "Registering conditions" << endmsg;
    updMgrSvc()->registerCondition(this,this->geometry(),&DeOTLayer::cachePlane);
    msg << MSG::DEBUG << "Start first update" << endmsg;
    StatusCode sc = updMgrSvc()->update(this);
    if ( !sc.isSuccess() ) {
      return sc;
    }
  } catch (DetectorElementException &e) {
    msg << MSG::ERROR << e << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/// Find the quarter for a given channel
DeOTQuarter* DeOTLayer::findQuarter(const OTChannelID aChannel)  const {  
  /// Find the quarter and return a pointer to the layer from channel
  MapIDQuarter::iterator iQ = m_mapIDQuarter.find(aChannel.uniqueQuarter());
  return (iQ != m_mapIDQuarter.end() ? iQ->second: 0);
}

/// Find the quarter for a given XYZ point
DeOTQuarter* DeOTLayer::findQuarter(const Gaudi::XYZPoint& aPoint) const {  
  Quarters::const_iterator iQ = std::find_if(m_quarters.begin(), m_quarters.end(),
                                             bind(&DetectorElement::isInside, _1, aPoint));
  return (iQ != m_quarters.end() ? (*iQ) : 0);
}

StatusCode DeOTLayer::cachePlane() {
  Gaudi::XYZPoint g1 = this->geometry()->toGlobal(Gaudi::XYZPoint(0.0, 0.0, 0.0));
  Gaudi::XYZPoint g2 = this->geometry()->toGlobal(Gaudi::XYZPoint(3.5*Gaudi::Units::m, 0.0, 0.0));
  Gaudi::XYZPoint g3 = this->geometry()->toGlobal(Gaudi::XYZPoint(0.0, 2.5*Gaudi::Units::m, 0.0));
  
  m_plane = Gaudi::Plane3D(g1, g2, g3);

  return StatusCode::SUCCESS;
}
