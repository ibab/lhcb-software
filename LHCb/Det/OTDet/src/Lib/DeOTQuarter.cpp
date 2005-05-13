// $Id: DeOTQuarter.cpp,v 1.2 2005-05-13 16:09:41 marcocle Exp $

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

// OTDet
#include "OTDet/DeOTQuarter.h"

/** @file DeOTQuarter.cpp
 *
 *  Implementation of class :  DeOTQuarter
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 */

DeOTQuarter::DeOTQuarter( const std::string& name ) :
  DetectorElement( name ),
  m_quarterID( 0 ),
  m_stereoAngle( 0.0 ),
  m_xMin(  10.0*km ),
  m_yMin(  10.0*km ),
  m_zMin(  10.0*km ),
  m_xMax( -10.0*km ),
  m_yMax( -10.0*km ),
  m_zMax( -10.0*km ),
  m_modules()
{ }

DeOTQuarter::~DeOTQuarter()
{
}

const CLID& DeOTQuarter::clID () const 
{ 
  return DeOTQuarter::classID() ; 
}

StatusCode DeOTQuarter::initialize() 
{
  IDetectorElement* layer = this->parentIDetectorElement();

  m_quarterID = (unsigned int) param<int>("quarterID");
  m_stereoAngle = layer->params()->param<double>("stereoAngle");

  //loop over modules
  IDetectorElement::IDEContainer::const_iterator iModule;
  for (iModule = this->childBegin(); iModule != this->childEnd();
       ++iModule) {  
    DeOTModule* otModule = dynamic_cast<DeOTModule*>(*iModule);
    if ( otModule) {
      m_modules.push_back(otModule);

      // Calculate cover box of quarter (this function should be in IVolume!)
      const IGeometryInfo* gi = otModule->geometry();
      const ISolid* solid = gi->lvolume()->solid();
      const SolidBox* mainBox = 
        dynamic_cast<const SolidBox*>( solid->cover() );
      if ( mainBox ) {
        double dx = mainBox->xHalfLength();
        double dy = mainBox->yHalfLength();
        double dz = mainBox->zHalfLength();
        int i, j, k; 
        for ( i = 0 ; i < 2 ; ++i ) {
          for ( j = 0 ; j < 2 ; ++j ) {
            for ( k = 0 ; k < 2 ; ++k ) {          
              HepPoint3D point( pow(-1.,i)*dx, pow(-1.,j)*dy, pow(-1.,k)*dz );
              HepPoint3D cornerPoint = gi->toGlobal( point );
              if ( cornerPoint.x() < m_xMin ) m_xMin = cornerPoint.x();
              if ( cornerPoint.y() < m_yMin ) m_yMin = cornerPoint.y();
              if ( cornerPoint.z() < m_zMin ) m_zMin = cornerPoint.z();
              if ( cornerPoint.x() > m_xMax ) m_xMax = cornerPoint.x();
              if ( cornerPoint.y() > m_yMax ) m_yMax = cornerPoint.y();
              if ( cornerPoint.z() > m_zMax ) m_zMax = cornerPoint.z();
            } // k
          } // j
        } // i
      } // if mainBox
    } // if otModule
  }
  return StatusCode::SUCCESS;
}

DeOTModule* DeOTQuarter::module(unsigned int moduleID) const
{
  DeOTModule* otModule = 0;
  std::vector<DeOTModule*>::const_iterator iterModule = m_modules.begin();
  while ( iterModule != m_modules.end() &&
          !( (*iterModule)->moduleID() == moduleID ) ) ++iterModule;

  if ( iterModule != m_modules.end()) otModule = *iterModule;
  return otModule;
}

DeOTModule* DeOTQuarter::module(const HepPoint3D& point) const
{
  DeOTModule* otModule = 0;
  std::vector<DeOTModule*>::const_iterator iterModule = m_modules.begin();
  while ( iterModule != m_modules.end() &&
          !( (*iterModule)->isInside(point) ) ) iterModule++;
  if ( iterModule != m_modules.end() ) otModule = *iterModule;
  return otModule;
}
