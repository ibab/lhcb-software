// $Id: DeOTQuarter.cpp,v 1.7 2006-06-08 12:24:03 janos Exp $

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

// DeOTQuarter::DeOTQuarter(const std::string& name) :
//   DetectorElement(name),
//   m_stationID(0u),
//   m_layerID(0u),
//   m_quarterID(0u),
//   m_stereoAngle(0.0),
//   m_xMin(10.0*km),
//   m_yMin(10.0*km),
//   m_zMin(10.0*km),
//   m_xMax(-10.0*km),
//   m_yMax(-10.0*km),
//   m_zMax(-10.0*km),
//   m_modules()
// { 
//   /// Constructor
// }

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
    if (module)  m_modules.push_back(module);
  } /// iModule
  
  /// Calculate cover box of quarter (this function should be in IVolume!)
  //  const IGeometryInfo* gi = module->geometry();
  //       const ISolid* solid = gi->lvolume()->solid();
  //       const SolidBox* mainBox = dynamic_cast<const SolidBox*>( solid->cover() );
  // FIXME: This doesn't seem to work for stereo layers
  //  if ( mainBox ) {
  //         double dx = mainBox->xHalfLength();
  //         double dy = mainBox->yHalfLength();
  //         double dz = mainBox->zHalfLength();
  //         int i, j, k; 
  //         for ( i = 0 ; i < 2 ; ++i ) {
  //           for ( j = 0 ; j < 2 ; ++j ) {
  //             for ( k = 0 ; k < 2 ; ++k ) {          
  //               Gaudi::XYZPoint point( pow(-1.,i)*dx, pow(-1.,j)*dy, pow(-1.,k)*dz );
  //               Gaudi::XYZPoint cornerPoint = gi->toGlobal( point );
  //               if ( cornerPoint.x() < m_xMin ) m_xMin = cornerPoint.x();
  //               if ( cornerPoint.y() < m_yMin ) m_yMin = cornerPoint.y();
  //               if ( cornerPoint.z() < m_zMin ) m_zMin = cornerPoint.z();
  //               if ( cornerPoint.x() > m_xMax ) m_xMax = cornerPoint.x();
  //               if ( cornerPoint.y() > m_yMax ) m_yMax = cornerPoint.y();
  //               if ( cornerPoint.z() > m_zMax ) m_zMax = cornerPoint.z();
  //             } /// k
  //           } /// j
  //         } /// i
  //       } /// if mainBox
  //     } /// if otModule
  //   } /// iModule
  
  IDetectorElement* layer = this->parentIDetectorElement();
  IDetectorElement* station = layer->parentIDetectorElement();
  m_stationID = (unsigned int) station->params()->param<int>("stationID");
  m_layerID = (unsigned int) layer->params()->param<int>("layerID");
  m_quarterID = (unsigned int) param<int>("quarterID");
  OTChannelID aChan(m_stationID, m_layerID, m_quarterID, 0u, 0u, 0u);
  setElementID(aChan);

  /// Do people really need this?
  m_stereoAngle = layer->params()->param<double>("stereoAngle");
  
  return StatusCode::SUCCESS;
}

/// Find the module for a given channelID
DeOTModule* DeOTQuarter::findModule(const OTChannelID aChannel) const {
  /// Find the module and return a pointer to the module from channel
  Modules::const_iterator iter = std::find_if(m_modules.begin(), m_modules.end(),
					      bind(&DeOTModule::contains, _1, aChannel));
  return (iter != m_modules.end() ? (*iter) : 0);
}

/// Find the module for a given XYZ point
DeOTModule* DeOTQuarter::findModule(const Gaudi::XYZPoint& aPoint) const {
 /// Find the modules and return a pointer to the modules from channel
  Modules::const_iterator iter = std::find_if(m_modules.begin(), m_modules.end(),
					      bind(&DetectorElement::isInside, _1, aPoint));
  return (iter != m_modules.end() ? (*iter) : 0);
}
