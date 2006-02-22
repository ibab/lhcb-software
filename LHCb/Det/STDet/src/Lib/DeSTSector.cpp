
#include "STDet/DeSTSector.h"

#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

// Kernel
#include "Kernel/SystemOfUnits.h"
#include "Kernel/LineTraj.h"
#include "Kernel/LHCbID.h"

#include "GaudiKernel/GaudiException.h"

/** @file DeSTSector.cpp
*
*  Implementation of class :  DeSTSector
*
*    @author Matthew Needham
*/

using namespace LHCb;

DeSTSector::DeSTSector( const std::string& name ) :
  DeSTBaseElement( name ),
  m_firstStrip(1), 
  m_lowerTraj(0),
  m_upperTraj(0)
{ 
    // constructer (first strip means we number from 1)
}

DeSTSector::~DeSTSector() {
  // destructer
  clear();
}

void DeSTSector::clear() {
  
  if (m_lowerTraj !=0 ) delete m_lowerTraj;
  if (m_upperTraj !=0 ) delete m_upperTraj;
}

std::ostream& DeSTSector::printOut( std::ostream& os ) const{

  // stream to cout  

   const ILVolume* lv = this->geometry()->lvolume();
   const SolidBox* mainBox = dynamic_cast<const SolidBox*>(lv->solid());

   os << " Sector :  "  << name()
      << "\n ID " << id() 
     << "\n type  " << type() 
     << "\n pitch " << m_pitch 
     << "\n strip " << m_nStrip
     << "\n capacitance " << m_capacitance/picofarad
     << "\n active width" << m_uMaxLocal - m_uMinLocal
     << "\n total width " << mainBox->xsize()
     << "\n active height" << m_vMaxLocal - m_vMinLocal
     << "\n total height " << mainBox->ysize()
     << "\n dead width " << m_deadWidth
     << "\n center " << globalCentre()
     << std::endl; 
  return os;
}

MsgStream& DeSTSector::printOut( MsgStream& os ) const{

  // stream to Msg service
  os << " Sector : \n "  << name()
     << "\n ID " << id() 
     << "type \n " << type() 
     << " pitch \n " << m_pitch 
     << "n strip \n " << m_nStrip
     << " capacitance \n " << m_capacitance/picofarad
     << " u min \n " << m_uMinLocal 
     << " u max \n " << m_uMaxLocal
     << " v min \n " << m_vMinLocal 
     << " v max  \n " << m_vMaxLocal
     << "dead width \n " << m_deadWidth
     << "\n center " << globalCentre()
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
    double guardRing = param<double>("verticalGuardRing");  
    double m_deadWidth = guardRing + 0.5*param<double>("bondGap");

    // geometry: uMin, uMax
    const ILVolume* lv = this->geometry()->lvolume();
    const SolidBox* mainBox = dynamic_cast<const SolidBox*>(lv->solid());
    m_uMaxLocal = 0.5*(m_pitch*m_nStrip);
    m_uMinLocal = -m_uMaxLocal;

    m_stripLength = fabs(m_vMaxLocal - m_vMinLocal);

    // and vMin, vMax
    m_vMaxLocal = 0.5*mainBox->ysize() - guardRing;
    m_vMinLocal = -m_vMaxLocal;

    double height = mainBox->ysize()/nSensors;
    for (unsigned int iSensor = 1u ; iSensor < nSensors; ++iSensor){
      double vDead = m_vMinLocal - m_deadWidth + (height*(double)iSensor);
      m_deadRegions.push_back(vDead);
    } //i

    // cache trajectories
    cacheTrajectory();    
    
  }

  return StatusCode::SUCCESS;
}

unsigned int DeSTSector::localUToStrip(const double u) const{

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
  return strip;
}

bool DeSTSector::localInActive(const Gaudi::XYZPoint& point,
                               Gaudi::XYZPoint tol) const{

  const double u = point.x();
  const double v= point.y();
  return(localInBox(u,v,tol.X(), tol.Y())&&(!localInBondGap(v, tol.Y())));
}

bool DeSTSector::globalInActive(const Gaudi::XYZPoint& gpoint,
                               Gaudi::XYZPoint tol) const{

  Gaudi::XYZPoint lPoint = toLocal(gpoint);
  return localInActive(lPoint,tol);
};


bool DeSTSector::globalInBondGap(const Gaudi::XYZPoint& gpoint, double tol) const{

  Gaudi::XYZPoint lPoint = toLocal(gpoint);
  return localInBondGap(lPoint.Y(),tol);
};


bool DeSTSector::globalInBox(const Gaudi::XYZPoint& gpoint,Gaudi::XYZPoint tol ) const{

  Gaudi::XYZPoint lPoint = toLocal(gpoint);
  return localInBox(lPoint.X(), lPoint.Y(),tol.X(), tol.Y());
};

bool DeSTSector::localInBondGap( const double v, double tol) const{

  std::vector<double>::const_iterator iterD = m_deadRegions.begin();
  while ((iterD != m_deadRegions.end())&&(fabs(v-*iterD)> (tol + m_deadWidth))){
    ++iterD;
  }
  return (iterD != m_deadRegions.end() ? true : false);
}

bool DeSTSector::localInBox(const double u, const double v, 
                            double uTol, double vTol) const{

  return ((u + uTol) <(m_uMaxLocal+(0.5*m_pitch)) 
	 &&(u - uTol)>(m_uMinLocal-(0.5*m_pitch))
         &&((v + uTol)<m_vMaxLocal) &&((v-vTol) > m_vMinLocal));
}

LHCb::Trajectory* DeSTSector::trajectory(const STChannelID& aChan, 
                                         const double offset) const{

  LineTraj* traj = 0;  

  if (contains(aChan) == true){
    const double arclen = localU(aChan.strip()) + (offset*m_pitch) - m_uMinLocal;
    Gaudi::XYZPoint begPoint =  m_lowerTraj->position( arclen );
    Gaudi::XYZPoint endPoint =  m_upperTraj->position( arclen );
    traj = new LineTraj(begPoint,endPoint);
  } 
  else {
     throw GaudiException( "Failed to make trajectory",
                           "DeSTSector.cpp", StatusCode::FAILURE );
  }

  return traj;
}

void DeSTSector::cacheTrajectory() {

  clear();

  Gaudi::XYZPoint p1(m_uMinLocal - 0.5*m_pitch, m_vMinLocal, 0.);
  Gaudi::XYZPoint p2(m_uMaxLocal + 0.5*m_pitch, m_vMinLocal, 0.);
  Gaudi::XYZPoint g1 = toGlobal(p1);
  Gaudi::XYZPoint g2 = toGlobal(p2);
  m_lowerTraj = new LineTraj(g1,g2);
 
  Gaudi::XYZPoint p3(m_uMinLocal - 0.5*m_pitch, m_vMaxLocal, 0.);
  Gaudi::XYZPoint p4(m_uMaxLocal + 0.5*m_pitch, m_vMaxLocal, 0.);
  Gaudi::XYZPoint g3 = toGlobal(p3);
  Gaudi::XYZPoint g4 = toGlobal(p4);
  m_upperTraj = new LineTraj(g3,g4);
   
}

STChannelID DeSTSector::nextLeft(const STChannelID testChan) const{

  if ((contains(testChan))&& (isStrip(testChan.strip()- 1u) == true)){
    return STChannelID(testChan.type(),
                      testChan.station(),
                      testChan.layer(), 
                      testChan.detRegion(),
                      testChan.sector(), 
                      testChan.strip() - 1u);
   }
   else {
     return LHCb::STChannelID(0u,0u,0u,0u,0u,0u);
   }
}

STChannelID DeSTSector::nextRight(const LHCb::STChannelID testChan) const{

  if ((contains(testChan) == true)&& (isStrip(testChan.strip()+ 1u) == true)){
    return STChannelID(testChan.type(),
                       testChan.station(),
                       testChan.layer(), 
                       testChan.detRegion(),
                       testChan.sector(), 
                       testChan.strip() + 1u);
  }
  else {
    return LHCb::STChannelID(0u,0u,0u,0u,0u,0u);
  }
}



