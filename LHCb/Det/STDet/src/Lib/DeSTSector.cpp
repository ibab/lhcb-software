// $Id: DeSTSector.cpp,v 1.27 2007-02-09 12:36:20 mneedham Exp $
#include "STDet/DeSTSector.h"

#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

#include <algorithm>

// Kernel
#include "Kernel/LineTraj.h"
#include "Kernel/LHCbID.h"

#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/GaudiException.h"

#include "GaudiKernel/IUpdateManagerSvc.h"

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
  m_midTraj(0),
  m_xInverted(false),
  m_yInverted(false)
{ 
    // constructer (first strip means we number from 1)
}

DeSTSector::~DeSTSector() {
  // destructer
  clear();
}

void DeSTSector::clear() {
  
  if (m_midTraj !=0 ) delete m_midTraj;
}

std::ostream& DeSTSector::printOut( std::ostream& os ) const{

  // stream to cout  

   const ILVolume* lv = this->geometry()->lvolume();
   const SolidBox* mainBox = dynamic_cast<const SolidBox*>(lv->solid());

   os << " Sector :  "  << name()
      << "\n ID " << id() 
      << "\n layer " << elementID().layer()
     << "\n type  " << type() 
     << "\n pitch " << m_pitch 
     << "\n strip " << m_nStrip
     << "\n capacitance " << m_capacitance/Gaudi::Units::picofarad
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
     << " capacitance \n " << m_capacitance/Gaudi::Units::picofarad
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
    m_deadWidth = guardRing + 0.5*param<double>("bondGap");

    // geometry: uMin, uMax
    const ILVolume* lv = this->geometry()->lvolume();
    const SolidBox* mainBox = dynamic_cast<const SolidBox*>(lv->solid());

    m_uMaxLocal = 0.5*(m_pitch*m_nStrip);
    m_uMinLocal = -m_uMaxLocal;

    // and vMin, vMax
    m_vMaxLocal = 0.5*mainBox->ysize() - guardRing;
    m_vMinLocal = -m_vMaxLocal;

    m_stripLength = fabs(m_vMaxLocal - m_vMinLocal);

    double height = mainBox->ysize()/nSensors;
    for (unsigned int iSensor = 1u ; iSensor < nSensors; ++iSensor){
      double vDead = m_vMinLocal - m_deadWidth + (height*(double)iSensor);
      m_deadRegions.push_back(vDead);
    } //i

    // thickness 
    m_thickness = mainBox->zsize();

    // sense in x and y...
    determineSense();

    // cache trajectories
    try {
      updMgrSvc()->invalidate(this);
      msg << MSG::DEBUG << "Registering conditions" << endmsg;
      updMgrSvc()->registerCondition(this,this->geometry(),&DeSTSector::cacheInfo);
      msg << MSG::DEBUG << "Start first update" << endmsg;
      sc = updMgrSvc()->update(this);
    } 
    catch (DetectorElementException &e) {
     msg << MSG::ERROR << e << endmsg;
     return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

unsigned int DeSTSector::localUToStrip(const double u) const{

  // convert local u to a strip
  unsigned int strip;
  if (m_xInverted == true){
    strip = (unsigned int)floor(((m_uMaxLocal-u)/m_pitch) + 0.5);
  }
  else {
    strip = (unsigned int)floor(((u-m_uMinLocal)/m_pitch) + 0.5 );
  }

  return (isStrip(strip) ? strip : 0);

}

double DeSTSector::localU(const unsigned int strip, 
                          const double offset) const
{ 
  // strip to local  

  double u = -999.;
  if (isStrip(strip) == true){
    double tStrip = strip + offset;
    if (m_xInverted  == true){
      u = m_uMaxLocal + m_pitch*(0.5 - tStrip );
    }
    else {
      u = m_uMinLocal + m_pitch*( tStrip - 0.5);

    }
  } // strip
  
  return u;
}

bool DeSTSector::localInActive( const Gaudi::XYZPoint& point,
                                Gaudi::XYZPoint tol) const 
{
  const double u = point.x();
  const double v = point.y();
  return(localInBox(u,v,tol.X(), tol.Y())&&(!localInBondGap(v, tol.Y())));
}

bool DeSTSector::globalInActive( const Gaudi::XYZPoint& gpoint,
                                 Gaudi::XYZPoint tol) const
{
  Gaudi::XYZPoint lPoint = toLocal(gpoint);
  return localInActive(lPoint,tol);
};


bool DeSTSector::globalInBondGap(const Gaudi::XYZPoint& gpoint, 
                                 double tol) const
{ 
  Gaudi::XYZPoint lPoint = toLocal(gpoint);
  return localInBondGap(lPoint.Y(),tol);
};


bool DeSTSector::globalInBox( const Gaudi::XYZPoint& gpoint,
                              Gaudi::XYZPoint tol ) const
{
  Gaudi::XYZPoint lPoint = toLocal(gpoint);
  return localInBox(lPoint.X(), lPoint.Y(),tol.X(), tol.Y());
};

bool DeSTSector::localInBondGap( const double v, double tol) const{

  std::vector<double>::const_iterator iterD = m_deadRegions.begin();
  while ( (iterD != m_deadRegions.end()) && 
          (fabs(v-*iterD)> (tol + m_deadWidth)) ){
    ++iterD;
  }
  return (iterD != m_deadRegions.end() ? true : false);
}

bool DeSTSector::localInBox( const double u, const double v, 
                             double uTol, double vTol) const
{
  return ((u + uTol) <(m_uMaxLocal+(0.5*m_pitch)) &&
          (u - uTol)>(m_uMinLocal-(0.5*m_pitch)) &&
          ((v + vTol)<m_vMaxLocal) &&((v-vTol) > m_vMinLocal));
}

std::auto_ptr<LHCb::Trajectory> 
DeSTSector::trajectory(const STChannelID& aChan, const double offset) const 
{
  
  if (contains(aChan) == false){
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to link " << aChan.uniqueSector() << " " 
        << elementID().uniqueSector() << endmsg; 
    throw GaudiException( "Failed to make trajectory",
                           "DeSTSector.cpp", StatusCode::FAILURE );
  }
  
  const double arclen = (offset + aChan.strip() - m_firstStrip)*m_pitch ;
  
  Gaudi::XYZPoint midPoint = m_midTraj->position( arclen + 
                                                    m_midTraj->beginRange());
  
  return std::auto_ptr<LHCb::Trajectory>( new LineTraj(midPoint,m_direction,m_range, true));
}

std::auto_ptr<LHCb::Trajectory> DeSTSector::trajectoryFirstStrip() const 
{
  Gaudi::XYZPoint begPoint = m_midTraj->position(m_midTraj->beginRange());
  return std::auto_ptr<LHCb::Trajectory>( new LineTraj(begPoint,m_direction,m_range, true));
}

std::auto_ptr<LHCb::Trajectory> DeSTSector::trajectoryLastStrip() const 
{
  Gaudi::XYZPoint endPoint = m_midTraj->position( m_midTraj->endRange() );
  return std::auto_ptr<LHCb::Trajectory>( new LineTraj(endPoint,m_direction,m_range, true));
}

void DeSTSector::determineSense()
{
  Gaudi::XYZPoint g1 = globalPoint(m_uMinLocal , m_vMinLocal, 0.);
  Gaudi::XYZPoint g2 = globalPoint(m_uMaxLocal , m_vMinLocal, 0.);
  if (g1.x() > g2.x()) { 
    m_xInverted = true;
  }

  Gaudi::XYZPoint g3 = globalPoint(m_uMinLocal , m_vMaxLocal, 0.);
  if (g1.y() > g3.y()) {m_yInverted = true;}
}

StatusCode DeSTSector::cacheInfo()
{

  clear();

  double yUpper =  m_vMaxLocal;
  double yLower =  m_vMinLocal;
  if ( m_yInverted ) std::swap(yUpper,yLower);

  double xUpper =  m_uMaxLocal - 0.5*m_pitch;
  double xLower =  m_uMinLocal + 0.5*m_pitch;
  if ( m_xInverted ) std::swap(xUpper,xLower);


  // direction
  Gaudi::XYZPoint g1 = globalPoint(xLower, yLower, 0.);
  Gaudi::XYZPoint g2 = globalPoint(xLower, yUpper, 0.);
  m_direction = g2 - g1;
  m_direction = m_direction.Unit();

  // trajectory of middle  
  Gaudi::XYZPoint g3 = globalPoint(xLower, 0., 0.);
  Gaudi::XYZPoint g4 = globalPoint(xUpper, 0., 0.);
  m_midTraj = new LineTraj(g3,g4);

  // range ---> strip Length
  m_range = std::make_pair(-0.5*m_stripLength,0.5*m_stripLength);

  // plane
  m_plane =  Gaudi::Plane3D(g1,g2,g4);
  
  m_entryPlane = Gaudi::Plane3D(m_plane.Normal(), globalPoint(0.,0.,-0.5*m_thickness));
  m_exitPlane = Gaudi::Plane3D(m_plane.Normal(), globalPoint(0.,0., 0.5*m_thickness));

  return StatusCode::SUCCESS;
}

STChannelID DeSTSector::nextLeft(const STChannelID testChan) const
{
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

STChannelID DeSTSector::nextRight(const LHCb::STChannelID testChan) const
{
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


