// $Id: DeVeloRType.cpp,v 1.14 2005-06-30 14:47:45 dhcroft Exp $
//==============================================================================
#define VELODET_DEVELORTYPE_CPP 1
//==============================================================================
// Include files 

// from CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// From Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"

// From LHCb
#include "Kernel/LHCbMath.h"

// From Velo
#include "VeloDet/DeVeloRType.h"

/** @file DeVeloRType.cpp
 *
 *  Implementation of class : DeVeloRType
 *
 *  @author Mark Tobin  Mark.Tobin@cern.ch
 */

//==============================================================================
/// Standard constructor
//==============================================================================
DeVeloRType::DeVeloRType(const std::string& name) : DeVeloSensor(name)
{
}
//==============================================================================
/// Destructor
//==============================================================================
DeVeloRType::~DeVeloRType()
{
}
//==============================================================================
/// Object identification
//==============================================================================
const CLID& DeVeloRType::clID() 
  const { return DeVeloRType::classID(); }
//==============================================================================
/// Initialisation method
//==============================================================================
StatusCode DeVeloRType::initialize() 
{
  // Trick from old DeVelo to set the output level
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel=0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  jobSvc->setMyProperties("DeVeloRType", pmgr);
  if ( 0 < outputLevel ) {
    msgSvc()->setOutputLevel("DeVeloRType", outputLevel);
  }
  delete pmgr;
  MsgStream msg(msgSvc(), "DeVeloRType");
  
  sc = DeVeloSensor::initialize();
  if(!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DeVeloSensor" << endreq;
    return sc;
  }

  m_numberOfZones = 4;
  m_stripsInZone = this->numberOfStrips() / m_numberOfZones;

  m_innerPitch = this->param<double>("InnerPitch");
  m_outerPitch = this->param<double>("OuterPitch");

  m_overlapInX = this->param<double>("ROverlapInX");
  
  // the resolution of the sensor
  m_resolution.first = this->param<double>("RResGrad");
  m_resolution.second = this->param<double>("RResConst");
  
  /// Calculate the strip radii/phi limits 
  calcStripLimits();

  return StatusCode::SUCCESS;
}
//==============================================================================
/// Calculate the nearest channel to a 3-d point.
//==============================================================================
StatusCode DeVeloRType::pointToChannel(const HepPoint3D& point,
                                       VeloChannelID& channel,
                                       double& fraction,
                                       double& pitch) const
{
  MsgStream msg(msgSvc(), "DeVeloRType");
  HepPoint3D localPoint(0,0,0);
  StatusCode sc = this->globalToLocal(point,localPoint);

  if(!sc.isSuccess()) return sc;

  // Check boundaries...
  sc = isInside(localPoint);
  if(!sc.isSuccess()) return sc;

  // work out closet channel....
  unsigned int closestStrip=0;
  double logarithm, strip;
  double radius=localPoint.perp();
  logarithm = (m_pitchSlope*(radius - m_innerR)+m_innerPitch) / 
    m_innerPitch;
  strip = log(logarithm)/m_pitchSlope;
  closestStrip = LHCbMath::round(strip);
  fraction = strip - closestStrip; 
 
  // Which zone is the strip in?
  double phi=localPoint.phi();
  unsigned int zone=zoneOfPhi(phi);
  
  // minimum strip in zone
  closestStrip += firstStrip(zone);
  
  unsigned int sensor=this->sensorNumber();
  // set VeloChannelID....
  channel.setSensor(sensor);
  channel.setStrip(closestStrip);
  if(this->isR()) {
    channel.setType(VeloChannelID::RType);
  } else if( this->isPileUp() ) {
    channel.setType(VeloChannelID::PileUpType);
  }
  // calculate pitch....
  pitch = rPitch(channel.strip());
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Checks if local point is inside sensor
//==============================================================================
StatusCode DeVeloRType::isInside(const HepPoint3D& point) const
{
  MsgStream msg(msgSvc(), "DeVeloRType");
  // check boundaries....  
  double radius=point.perp();
  if(this->innerRadius() >= radius || this->outerRadius() <= radius) {
    msg << MSG::VERBOSE << "Outside active radii " << radius << endreq;
    return StatusCode::FAILURE;
  }
  // Dead region from bias line
  double y=point.y();
  if (m_phiGap > y && -m_phiGap < y) {
    msg << MSG::VERBOSE << "Inside dead region from bias line " << y << endreq;
    return StatusCode::FAILURE;
  }
  // corner cut-offs
  bool isCutOff=this->isCutOff(point.x(),point.y());
  if(isCutOff) {
    msg << MSG::VERBOSE << "cut off: x,y " << point.x() << "," << point.y()
        << endreq;    
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Is the point in the corner cut-off?
//==============================================================================
bool DeVeloRType::isCutOff(double x, double y) const
{
  if(m_cornerX1 > x) return true;
  y = fabs(y);
  if(m_cornerX1 <= x && m_cornerX2 >= x) {
    double yMax=m_corners[0]*x+m_corners[1];
    if(yMax > y) {
      return true;
    }
  }
  return false;
}
//==============================================================================
/// Get the nth nearest neighbour within a sector for a given channel
//==============================================================================
StatusCode DeVeloRType::neighbour(const VeloChannelID& start, 
                                  const int& nOffset, 
                                  VeloChannelID& channel) const
{
  unsigned int strip=0;
  strip = start.strip();
  unsigned int startZone;
  startZone = zoneOfStrip(strip);
  strip += nOffset;
  unsigned int endZone;
  endZone = zoneOfStrip(strip);
  // Check boundaries
  if(this->numberOfStrips() < strip) return StatusCode::FAILURE;
  if(startZone != endZone) {
    return StatusCode::FAILURE;
  }
  channel = start;
  channel.setStrip(strip);
  return StatusCode::SUCCESS;
}

//=============================================================================
/// Residual of 3-d point to a VeloChannelID
//=============================================================================
StatusCode DeVeloRType::residual(const HepPoint3D& point, 
                                 const VeloChannelID& channel,
                                 double &residual,
                                 double &chi2) const
{
  MsgStream msg(msgSvc(), "DeVeloRType");
  HepPoint3D localPoint(0,0,0);
  StatusCode sc = DeVeloSensor::globalToLocal(point,localPoint);
  
  if(!sc.isSuccess()) return sc;
  
  // Check boundaries...
  sc = isInside(localPoint);
  if(!sc.isSuccess()) return sc;

  unsigned int strip=channel.strip();

  double rPoint = localPoint.perp();
  double rStrip = rOfStrip(strip);
  residual = rStrip - rPoint;

  double sigma=m_resolution.first*rPitch(strip) - m_resolution.second;
  chi2 = pow(residual/sigma,2);

  msg << MSG::VERBOSE << "Residual; rPoint = " << rPoint << " strip " << strip 
      << " rStrip = " << rStrip << " residual " << residual
      << " sigma = " << sigma
      << " chi2 = " << chi2 << endreq;
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Residual [see DeVelo for explanation]
//==============================================================================
StatusCode DeVeloRType::residual(const HepPoint3D& /*point*/,
                                   const VeloChannelID& /*channel*/,
                                   const double /*localOffset*/,
                                   const double /*width*/,
                                   double &/*residual*/,
                                   double &/*chi2*/) const
{

  // Perpendicular distance to strip.....

  return StatusCode::SUCCESS;
}
//==============================================================================
/// Store the local radius and phi limits for each strip in the sensor
//==============================================================================
void DeVeloRType::calcStripLimits()
{
  MsgStream msg( msgSvc(), "DeVeloRType" );
  msg << MSG::VERBOSE << "calcStripLimits" << endreq;
  m_innerR = this->innerRadius() + m_innerPitch / 2;
  m_outerR = this->outerRadius() - m_outerPitch / 2;

  m_pitchSlope = (m_outerPitch - m_innerPitch) / 
    (m_outerR - m_innerR);

  /// Dead region from bias line
  m_phiGap = this->param<double>("RPhiGap") / 2;

  /// Corner cut offs
  cornerLimits();
  
  /// Angular limits for zones...
  phiZoneLimits();

  m_rStrips.clear();
  m_rPitch.clear();
  m_stripLimits.clear();
  double radius,pitch;
  for(unsigned int zone=0; zone<m_numberOfZones; zone++) {
    for(unsigned int istrip=0; istrip<m_stripsInZone; istrip++){
      radius = (exp(m_pitchSlope*istrip)*m_innerPitch - 
                (m_innerPitch-m_pitchSlope*m_innerR)) /
      m_pitchSlope;
      m_rStrips.push_back(radius);
      pitch = exp(m_pitchSlope*istrip)*m_innerPitch;
      m_rPitch.push_back(pitch);
      double phiMin=0;
      double phiMax=0;
      double phiLimit=0;
      double x=0,y=0;
      phiMin=phiMinZone(zone,radius);
      phiMax=phiMaxZone(zone,radius);
      if(0 == zone){
        phiLimit = phiMin;
        x = radius*cos(phiMin);
        y = radius*sin(phiMin);
      } else if(3 == zone){
        phiLimit = phiMax;
        x = radius*cos(phiMax);
        y = radius*sin(phiMax);
      }
      // Work out point where strip crosses cut off
      if(zone == 0 || zone == 3){
        if(isCutOff(x,y)){
          double a = pow(m_corners[0],2)+1;
          double b = 2*m_corners[0]*m_corners[1];
          double c = pow(m_corners[1],2) - pow(radius,2);
          double x1 = (-b + sqrt(b*b - (4*a*c))) / (2*a);
          double x2 = (-b - sqrt(b*b - (4*a*c))) / (2*a);
          if(m_cornerX1 <= x1 && m_cornerX2 >= x1) {
            phiLimit = -acos(x1/radius);
          } else if(m_cornerX1 <= x2 && m_cornerX2 >= x2) {
            phiLimit = -acos(x2/radius);
          }
          if(zone == 0){
            phiMin = phiLimit;
          } else if(zone == 3){
            if(phiMax != phiLimit) phiMax = -phiLimit;
          }
        }
      }
      m_stripLimits.push_back(std::pair<double,double>(phiMin,phiMax));
    }
  }
  for(unsigned int i=0; i < m_phiMin.size(); i++){
    msg << MSG::DEBUG << "Zone limits; zone " << i << " min " << m_phiMin[i]
        << " max " << m_phiMax[i] << " phiMin " 
        << phiMinZone(i,this->innerRadius()) 
        << " max " << phiMaxZone(i,this->innerRadius()) << endmsg;
  }
  msg << MSG::DEBUG << "Radius of first strip is " << m_rStrips[0] 
      << " last strip " << m_rStrips[m_rStrips.size()-1] << endmsg;
  msg << MSG::DEBUG << "Pitch; inner " << m_rPitch[0] << " outer " 
      << m_rPitch[m_rPitch.size()-1] << " Radius; inner " << m_innerR
      << " outer " << m_outerR 
      << " slope " << m_pitchSlope << endmsg;
}
//==============================================================================
/// Store line defining corner cut-offs
//==============================================================================
void DeVeloRType::cornerLimits()
{
  m_cornerX1 = this->param<double>("RCornerX1");
  m_cornerY1 = this->param<double>("RCornerY1");
  m_cornerX2 = this->param<double>("RCornerX2");
  m_cornerY2 = this->param<double>("RCornerY2");
  
  m_corners.clear();
  double gradient;
  gradient = (m_cornerY2 - m_cornerY1) /  (m_cornerX2 - m_cornerX1);
  m_corners.push_back(gradient);
  double intercept;
  intercept = m_cornerY2 - (gradient*m_cornerX2);
  m_corners.push_back(intercept);
}
//==============================================================================
/// Store the angular limits of zones sensor at +ve x
//==============================================================================
void DeVeloRType::phiZoneLimits()
{
  m_halfAngle     = 90.0 * degree;
  m_quarterAngle  = .5 * m_halfAngle;

  double phi;
  phi = acos(m_overlapInX/this->outerRadius());

  m_phiMin.clear();
  m_phiMax.clear();

  if(!this->isDownstream()){
    // upstream sensor
    m_phiMin.push_back(-phi);
    m_phiMin.push_back(-m_quarterAngle);
    m_phiMin.push_back(0);
    m_phiMin.push_back(m_quarterAngle);
    
    m_phiMax.push_back(-m_quarterAngle);
    m_phiMax.push_back(0);
    m_phiMax.push_back(m_quarterAngle);
    m_phiMax.push_back(phi);

  }else{
    //downstream sensor (i.e. 180deg flip and sectors order in phi reversed)
    m_phiMin.push_back(m_quarterAngle);
    m_phiMin.push_back(0);
    m_phiMin.push_back(-m_quarterAngle);
    m_phiMin.push_back(-phi);
    
    m_phiMax.push_back(phi);
    m_phiMax.push_back(m_quarterAngle);
    m_phiMax.push_back(0);
    m_phiMax.push_back(-m_quarterAngle);
  }    

}
//==============================================================================
/// Return the capacitance of the strip
//==============================================================================
double DeVeloRType::stripCapacitance(unsigned int /*strip*/) const
{
  double C=0.0;
  return C;
}
