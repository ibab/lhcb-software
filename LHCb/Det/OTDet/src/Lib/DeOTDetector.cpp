// $Id: DeOTDetector.cpp,v 1.6 2002-08-07 15:38:26 jvantilb Exp $

// CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"

// DetDesc
#include "DetDesc/IGeometryInfo.h"
//#include "DetDesc/ILVolume.h"
//#include "DetDesc/ISolid.h"
//#include "DetDesc/SolidBox.h"
//#include "DetDesc/SolidSubtraction.h"

// OTDet
#include "OTDet/DeOTDetector.h"

/** @file DeOTDetector.cpp
 *
 *  Implementation of class :  DeOTDetector
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 */

DeOTDetector::DeOTDetector( const std::string& name ) :
  DetectorElement( name )
{ }


DeOTDetector::~DeOTDetector()
{
  std::vector<OTLayer*>::iterator iLayer;
  for (iLayer = m_layers.begin(); iLayer != m_layers.end(); ++iLayer) {
    delete *iLayer;
    m_layers.erase(iLayer);
  }

}

const CLID& DeOTDetector::clID () const 
{ 
  return DeOTDetector::classID() ; 
}

StatusCode DeOTDetector::initialize() 
{
  MsgStream log( msgSvc(), name() );

  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    log << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc ; 
  }

  m_numStations = this->userParameterAsInt("numStations");
  m_firstOTStation =  this->userParameterAsInt("firstOTStation");

  unsigned int stationNum = m_firstOTStation;
  // loop over station and layer and fill layer container
  IDetectorElement::IDEContainer::const_iterator iStation;
  for (iStation = this->childBegin(); this->childEnd() != iStation; 
       ++iStation ) {

    double xCen = (*iStation)->userParameterAsDouble("xCen");
    double yCen = (*iStation)->userParameterAsDouble("yCen");
    //double zCen = (*iStation)->userParameterAsDouble("zCen");
    double xSide = (*iStation)->userParameterAsDouble("xSide");
    double ySide = (*iStation)->userParameterAsDouble("ySide");
    double xOut = (*iStation)->userParameterAsDouble("xOut");
    double yOut = (*iStation)->userParameterAsDouble("yOut");
    int nStandStraw = (*iStation)->userParameterAsInt("nStraw");
    double pitchStraw = (*iStation)->userParameterAsDouble("pitch");
    //int numLayers =  (*iStation)->userParameterAsInt("numLayers");
    //double dPlane = (*iStation)->userParameterAsDouble("layerWidth");

    //loop over layers
    IDetectorElement::IDEContainer::const_iterator iLayer;
    int layerNum = 1;
    
    for( iLayer = (*iStation)->childBegin(); (*iStation)->childEnd() != iLayer;
         ++iLayer ) {

      //double zLayer = zCen + ((double) (2*layerNum-numLayers-1))*dPlane/2.;
      HepPoint3D localPoint(0,0,0);
      IGeometryInfo* geomInfo = (*iLayer)->geometry();
      HepPoint3D layerPoint = geomInfo->toGlobal(localPoint);
      double zLayer = layerPoint.z();
      double stereoAngle = (*iLayer)->userParameterAsDouble("stereoAngle");

      /* This may be a way to get the cross parameters from the geometry
      const ILVolume* lv = geomInfo->lvolume();
      const ISolid* solid = lv->solid(); // " generic solid interface"
      const SolidSubtraction* ix = 
        dynamic_cast<const SolidSubtraction*> (solid);
      if( 0 == ix ) { 
        log << MSG::ERROR << "This is no solid subtraction!" << endreq;
      }
      const SolidBox* mainBox = dynamic_cast<const SolidBox*>( ix->first() );
      log << MSG::DEBUG << " Main box xOut=" << mainBox->xsize()
          << "  yOut=" << mainBox->ysize()
          << "  z=" << mainBox->zsize() << endreq;

      SolidBoolean::SolidChildrens::const_iterator iChild;
      for( iChild = ix->childBegin() ; ix->childEnd() != iChild; ++iChild ) {
        const SolidChild* child = (*iChild) ;
        const ISolid*     s     = child->solid() ;
        const SolidBox*   box   = dynamic_cast<const SolidBox*>( s );
        const double xsize = box->xsize() ;
        const double ysize = box->ysize() ;
        const double zsize = box->zsize() ;
        log << MSG::DEBUG << "    subtracted box xCen/Side= " << xsize 
            << "   yCen/Side=" << ysize << "   z=" << zsize << endreq;
      }*/
      

      OTLayer* newLayer = new OTLayer(layerNum, stationNum, zLayer, 
                                      xCen, yCen, xSide, ySide,
                                      xOut, yOut,
                                      nStandStraw, pitchStraw,
                                      stereoAngle);
      m_layers.push_back(newLayer);
      layerNum++;
    } // loop layers
    stationNum++;
  } // loop stations


  log << MSG::DEBUG << "initialize DetectorElement succeeded!!!" << endreq;

  return sc;
}


OTLayer* DeOTDetector::layer(const double z) const 
{
   // find closest layer corresponding to z position
 
  OTLayer* closestLayer = 0;
  std::vector<OTLayer*>::const_iterator iLayer;
  double dist = 99999.0;
  for ( iLayer = m_layers.begin(); m_layers.end() != iLayer; ++iLayer) {
    double newDist = fabs( (*iLayer)->z() - z );
    if ( newDist < dist ) {
      dist = newDist;
      closestLayer = *iLayer;
    }
  }

  if ( dist > 99998.0 ) {
    MsgStream log(msgSvc(), name());
    log << MSG::ERROR << " Failed to find detection layer  "<< endreq;
  }

  return closestLayer;
}


OTLayer* DeOTDetector::layer(OTChannelID aChannel) const 
{
  const unsigned int iStation = aChannel.station();
  const unsigned int iLayer   = aChannel.layer();
  if ((iStation < m_firstOTStation) || (iStation > m_numStations)) {
    return 0;
  }

  // Find the layer
  std::vector<OTLayer*>::const_iterator iterLayer = m_layers.begin();
  while ( iterLayer != m_layers.end() &&
         !( (*iterLayer)->stationID() == iStation &&
            (*iterLayer)->layerID() == iLayer        ) ) iterLayer++;
  OTLayer* hitLayer = (*iterLayer);

  return hitLayer;
}


double DeOTDetector::distanceAlongWire(OTChannelID channelID,
                                       double xHit, double yHit) const 
{
  // distance along wire
  OTLayer* hitLayer = this->layer(channelID);
  if ( 0 == hitLayer ) {
    MsgStream log(msgSvc(), name());
    log << MSG::WARNING
        << "Propagation delay requested for layer that does not exist "
        << endreq;
    return 0.0;
  }

  const int iModule  = channelID.module();

  return hitLayer->distanceAlongWire(iModule, xHit, yHit);
}


OTChannelID DeOTDetector::nextChannelRight(OTChannelID aChannel) const
{
  const int iModule  = aChannel.module();
  const int iLayer   = aChannel.layer();
  const int iStation = aChannel.station();
  const int iStraw = aChannel.straw();

  int rStraw;
  int rMod;

  OTLayer* hitLayer = this->layer(aChannel);
  hitLayer->nextRightStraw(iStraw,iModule,rStraw,rMod);

  if (iModule == rMod){ 
    OTChannelID nextRight = OTChannelID(iStation,iLayer,rMod,rStraw);
    return nextRight;
  }
  else {
    // no left neighbour null channel
    OTChannelID nextRight = OTChannelID(0,0,0,0);
    return nextRight;
  }

} 


OTChannelID DeOTDetector::nextChannelLeft(OTChannelID aChannel) const
{
  const int iModule  = aChannel.module();
  const int iLayer   = aChannel.layer();
  const int iStation = aChannel.station();
  const int iStraw = aChannel.straw();

  int lStraw;
  int lMod;

  OTLayer* hitLayer = this->layer(aChannel);
  hitLayer->nextLeftStraw(iStraw,iModule,lStraw,lMod);
  
  if (iModule == lMod){ 
    OTChannelID nextLeft = OTChannelID(iStation,iLayer,lMod,lStraw);
    return nextLeft;
  }
  else {
    // no left neighbour null channel
    OTChannelID nextLeft = OTChannelID(0,0,0,0);
    return nextLeft;
  }
} 
