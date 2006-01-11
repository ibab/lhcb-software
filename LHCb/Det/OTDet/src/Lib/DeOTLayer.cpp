// $Id: DeOTLayer.cpp,v 1.5 2006-01-11 09:29:15 janos Exp $

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
  m_quarters()
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
  m_layerID = (unsigned int) param<int>("layerID");
  m_stereoAngle = param<double>("stereoAngle");

  //loop over quarters
  IDetectorElement::IDEContainer::const_iterator iQuarter;
  for (iQuarter = this->childBegin(); iQuarter != this->childEnd();
       ++iQuarter) {  
    DeOTQuarter* otQuarter = dynamic_cast<DeOTQuarter*>(*iQuarter);
    if ( otQuarter) {
      m_quarters.push_back(otQuarter);
    }
  }
  return StatusCode::SUCCESS;
}

DeOTQuarter* DeOTLayer::quarter(unsigned int quarterID) const
{
  DeOTQuarter* otQuarter = 0;
  std::vector<DeOTQuarter*>::const_iterator iterQuarter = m_quarters.begin();
  while ( iterQuarter != m_quarters.end() &&
          !( (*iterQuarter)->quarterID() == quarterID ) ) ++iterQuarter;

  if ( iterQuarter != m_quarters.end()) otQuarter = *iterQuarter;
  return otQuarter;
}

DeOTQuarter* DeOTLayer::quarter(const Gaudi::XYZPoint& point) const
{
  DeOTQuarter* otQuarter = 0;
  DeOTModule* otModule = 0;
  std::vector<DeOTQuarter*>::const_iterator iterQuarter = m_quarters.begin();
  while ( iterQuarter != m_quarters.end() && otQuarter == 0 ) {
    if ( (*iterQuarter)->isInside(point) ) {
      otModule = (*iterQuarter)->module( point );
      if ( otModule != 0 ) otQuarter = (*iterQuarter);
    }
    ++iterQuarter;
  }
  return otQuarter;
}

DeOTModule* DeOTLayer::module(const Gaudi::XYZPoint& point) const
{
  DeOTModule* otModule = 0;
  bool found = false;
  std::vector<DeOTQuarter*>::const_iterator iterQuarter = m_quarters.begin();
  while ( iterQuarter != m_quarters.end() && !found ) {
    if ( (*iterQuarter)->isInside(point) ) {
      otModule = (*iterQuarter)->module( point );
      if ( otModule != 0 ) found = true;
    }
    ++iterQuarter;
  }
  return otModule;
}
