// $Id: DeOTLayer.cpp,v 1.1 2003-06-11 11:49:36 cattanem Exp $

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

// OTDet
#include "OTDet/DeOTLayer.h"

/** @file DeOTLayer.cpp
 *
 *  Implementation of class :  DeOTLayer
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 */

DeOTLayer::DeOTLayer( const std::string& name ) :
  DetectorElement( name ),
  m_layerID(0),
  m_stereoAngle(0.0),
  m_z(0.0),
  m_zSize(0.0),
  m_modules()
{ }

DeOTLayer::~DeOTLayer()
{
}

const CLID& DeOTLayer::clID () const 
{ 
  return DeOTLayer::classID() ; 
}

StatusCode DeOTLayer::initialize() 
{
  m_layerID = (unsigned int) this->userParameterAsInt("layerID");
  m_stereoAngle = this->userParameterAsDouble("stereoAngle");

  const IGeometryInfo* gi = this->geometry();
  HepPoint3D middlePoint(0,0,0);
  HepPoint3D globalPoint = gi->toGlobal(middlePoint);
  m_z = globalPoint.z();

  const ISolid* solid = gi->lvolume()->solid();
  const SolidBox* mainBox = dynamic_cast<const SolidBox*>( solid->cover() );
  if ( mainBox ) m_zSize = mainBox->zsize();  

  //loop over modules
  IDetectorElement::IDEContainer::const_iterator iModule;
  for (iModule = this->childBegin(); iModule != this->childEnd();
       ++iModule) {  
    DeOTModule* otModule = dynamic_cast<DeOTModule*>(*iModule);
    if ( otModule) {
      m_modules.push_back(otModule);
    }
  }
  return StatusCode::SUCCESS;
}

DeOTModule* DeOTLayer::module(unsigned int moduleID) const
{
  DeOTModule* otModule = 0;
  std::vector<DeOTModule*>::const_iterator iterModule = m_modules.begin();
  while ( iterModule != m_modules.end() &&
          !( (*iterModule)->moduleID() == moduleID ) ) iterModule++;

  if ( iterModule != m_modules.end()) otModule = *iterModule;
  return otModule;
}

DeOTModule* DeOTLayer::module(const HepPoint3D& point) const
{
  DeOTModule* otModule = 0;
  std::vector<DeOTModule*>::const_iterator iterModule = m_modules.begin();
  while ( iterModule != m_modules.end() &&
          !( (*iterModule)->geometry()->isInside(point) ) ) iterModule++;
  if ( iterModule != m_modules.end() ) otModule = *iterModule;
  return otModule;
}
