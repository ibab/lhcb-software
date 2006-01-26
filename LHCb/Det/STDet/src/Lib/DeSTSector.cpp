
#include "STDet/DeSTSector.h"


#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

/** @file DeSTSector.cpp
*
*  Implementation of class :  DeSTSector
*
*    @author Matthew Needham
*/

using namespace LHCb;

DeSTSector::DeSTSector( const std::string& name ) :
  DeSTBaseElement( name ),
  m_firstStrip(1) 
{ 
  // constructer (first strip means we number from 1)
}

DeSTSector::~DeSTSector() {
  // destructer
}


std::ostream& DeSTSector::printOut( std::ostream& os ) const{

  // stream to cout
  os << " Sector : "  << name()
     << "type " << type() 
     << " pitch " << m_pitch 
     << "n strip" << m_nStrip
     << " capacitance " << m_capacitance
     << " u min " << m_uMinLocal 
     << " u max " << m_uMaxLocal
     << " v min " << m_vMinLocal 
     << " v max " << m_vMaxLocal
     << "dead width " << m_deadWidth
     << std::endl;

  return os;
}

MsgStream& DeSTSector::printOut( MsgStream& os ) const{

  // stream to Msg service
  os << " Sector : "  << name()
     << "type " << type() 
     << " pitch " << m_pitch 
     << "n strip" << m_nStrip
     << " capacitance " << m_capacitance
     << " u min " << m_uMinLocal 
     << " u max " << m_uMaxLocal
     << " v min " << m_vMinLocal 
     << " v max " << m_vMaxLocal
     << "dead width " << m_deadWidth
     << std::endl;

  return os;
}

StatusCode DeSTSector::initialize() {

  // initialize method
  MsgStream msg(msgSvc(), name() );

  StatusCode sc = DeSTBaseElement::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq; 
  }
  else {
   
    m_pitch = param<double>("pitch");
    m_nStrip =  param<int>("numStrips");
    m_capacitance = param<double>("capacitance");
    m_type = param<std::string>("type");
    unsigned int nSensors = param<int>("nSensors");

    // guard ring
    double m_deadWidth = param<double>("verticalGuardRing");  

    // geometry: uMin, uMax
    const ILVolume* lv = this->geometry()->lvolume();
    const SolidBox* mainBox = dynamic_cast<const SolidBox*>(lv->solid());
    m_uMaxLocal = 0.5*(mainBox->xsize() - m_pitch*m_nStrip);
    m_uMinLocal = -m_uMaxLocal;

    // and vMin, vMax
    m_vMaxLocal = 0.5*(mainBox->ysize() - m_deadWidth);
    m_vMinLocal = -m_vMaxLocal;

 
    double height = mainBox->ysize()/nSensors;
    for (unsigned int iSensor = 1u ; iSensor < nSensors; ++iSensor){
      double vDead = m_vMinLocal - m_deadWidth + (height*(double)iSensor);
      m_deadRegions.push_back(vDead);
    } //i
    
  }

  return StatusCode::SUCCESS;
}

STChannelID DeSTSector::localUToStrip(const double u) const{

  // convert local u to a strip
  unsigned int strip;
  if (u<m_uMaxLocal && u>m_uMinLocal) {
    strip = m_firstStrip + (unsigned int)floor((u - m_uMinLocal)/m_pitch);
  }
  else if (u>=m_uMaxLocal) {
    strip = m_firstStrip + m_nStrip - 1u;
  }
  else {
    strip = m_firstStrip;
  }
  return STChannelID(elementID().type(),
                     elementID().station(), 
                     elementID().layer(), 
                     elementID().detRegion(),
                     elementID().sector(), strip);
}

bool DeSTSector::localInActive(const Gaudi::XYZPoint& point) const{

  const double u = point.x();
  const double v= point.y();
 
  bool isInside = true;
  if (u<(m_uMaxLocal+(0.5*m_pitch)) && u>(m_uMinLocal-(0.5*m_pitch)
      && (v<m_vMaxLocal && v>m_vMinLocal))){
    std::vector<double>::const_iterator iterD = m_deadRegions.begin();
    while ((iterD != m_deadRegions.end())&&(isInside == true)){
      if (fabs(v-*iterD)<m_deadWidth){
        isInside = false;
      }
      ++iterD;
    } // iterD
  }
  return isInside;
}

void DeSTSector::trajectory(const STChannelID& aChan) const{
  
  if (contains(aChan) == true){

    Gaudi::XYZPoint start(localU(aChan.strip()), m_vMinLocal, 0.);
    Gaudi::XYZPoint stop(localU(aChan.strip()), m_vMaxLocal, 0.);

    Gaudi::XYZPoint globalStart = toGlobal(start);
    Gaudi::XYZPoint globalStop = toGlobal(stop);

  } 

  return;
}


