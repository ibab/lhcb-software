// $Id: DeVeloRType.cpp,v 1.25 2006-04-27 14:16:32 cattanem Exp $
//==============================================================================
#define VELODET_DEVELORTYPE_CPP 1
//==============================================================================
// Include files 

// from Kernel
#include "Kernel/SystemOfUnits.h"

// From Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
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
 *  @author Kurt Rinnert kurt.rinnert@cern.ch
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
  m_stripsInZone = numberOfStrips() / m_numberOfZones;

  m_innerPitch = param<double>("InnerPitch");
  m_outerPitch = param<double>("OuterPitch");

  m_overlapInX = param<double>("ROverlapInX");
  
  // the resolution of the sensor
  m_resolution.first = param<double>("RResGrad");
  m_resolution.second = param<double>("RResConst");
  
  /// Calculate the strip radii/phi limits 
  calcStripLimits();

  /// Build up map of strips to routing lines
  BuildRoutingLineMap();
  
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Calculate the nearest channel to a 3-d point.
//==============================================================================
StatusCode DeVeloRType::pointToChannel(const Gaudi::XYZPoint& point,
                                       LHCb::VeloChannelID& channel,
                                       double& fraction,
                                       double& pitch) const
{
  MsgStream msg(msgSvc(), "DeVeloRType");
  Gaudi::XYZPoint localPoint(0,0,0);
  StatusCode sc = globalToLocal(point,localPoint);

  if(!sc.isSuccess()) return sc;

  // Check boundaries...
  sc = isInActiveArea(localPoint);
  if(!sc.isSuccess()) return sc;

  // work out closet channel....
  unsigned int closestStrip=0;
  double logarithm, strip;
  double radius=localPoint.Rho();
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
  
  unsigned int sensor=sensorNumber();
  // set VeloChannelID....
  channel.setSensor(sensor);
  channel.setStrip(closestStrip);
  if(isR()) {
    channel.setType(LHCb::VeloChannelID::RType);
  } else if( isPileUp() ) {
    channel.setType(LHCb::VeloChannelID::PileUpType);
  }
  // calculate pitch....
  pitch = rPitch(channel.strip());
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Checks if local point is inside sensor
//==============================================================================
StatusCode DeVeloRType::isInActiveArea(const Gaudi::XYZPoint& point) const
{
  MsgStream msg(msgSvc(), "DeVeloRType");
  // check boundaries....  
  double radius=point.Rho();
  if(innerRadius() >= radius || outerRadius() <= radius) {
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
  bool cutOff=isCutOff(point.x(),point.y());
  if(cutOff) {
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
  double epsilon=1.E-9;
  if(m_cornerX1 - epsilon > x) return true;
  if(m_cornerX1 - epsilon <= x && x <= m_cornerX2 + epsilon) {
    y = fabs(y);
    double yMax=m_gradCutOff*x+m_intCutOff;
    if(yMax > y) {
      return true;
    }
  }
  return false;
}
//==============================================================================
/// Get the nth nearest neighbour within a sector for a given channel
//==============================================================================
StatusCode DeVeloRType::neighbour(const LHCb::VeloChannelID& start, 
                                  const int& nOffset, 
                                  LHCb::VeloChannelID& channel) const
{
  unsigned int strip=0;
  strip = start.strip();
  unsigned int startZone;
  startZone = zoneOfStrip(strip);
  strip += nOffset;
  unsigned int endZone;
  endZone = zoneOfStrip(strip);
  // Check boundaries
  if(numberOfStrips() <= strip) return StatusCode::FAILURE;
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
StatusCode DeVeloRType::residual(const Gaudi::XYZPoint& point, 
                                 const LHCb::VeloChannelID& channel,
                                 double &residual,
                                 double &chi2) const
{
  MsgStream msg(msgSvc(), "DeVeloRType");
  Gaudi::XYZPoint localPoint(0,0,0);
  StatusCode sc = DeVeloSensor::globalToLocal(point,localPoint);
  
  if(!sc.isSuccess()) return sc;
  
  // Check boundaries...
  sc = isInActiveArea(localPoint);
  if(!sc.isSuccess()) return sc;

  unsigned int strip=channel.strip();

  double rPoint = localPoint.Rho();
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
StatusCode DeVeloRType::residual(const Gaudi::XYZPoint& /*point*/,
                                   const LHCb::VeloChannelID& /*channel*/,
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
  m_innerR = innerRadius() + m_innerPitch / 2;
  m_outerR = outerRadius() - m_outerPitch / 2;

  m_pitchSlope = (m_outerPitch - m_innerPitch) / 
    (m_outerR - m_innerR);

  /// Dead region from bias line
  m_phiGap = param<double>("RPhiGap") / 2;

  /// Corner cut offs
  cornerLimits();
  
  /// Angular limits for zones...
  phiZoneLimits();

  m_rStrips.clear();
  m_rPitch.clear();
  m_stripPhiLimits.clear();
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
      double x1=0,y1=0,x2=0,y2=0;
      phiMin=phiMinZone(zone,radius);
      phiMax=phiMaxZone(zone,radius);
      x1 = radius*cos(phiMin);
      y1 = radius*sin(phiMin);
      x2 = radius*cos(phiMax);
      y2 = radius*sin(phiMax);
      bool cutOff=false;
      if(zone == 0) cutOff = isCutOff(x1,y1);
      else if(zone == 3) cutOff = isCutOff(x2,y2);
      if(cutOff){
        double x,y;
        intersectCutOff(radius,x,y);
        if(phiMin < 0 || phiMax < 0) y = -y;
        double phiInt=atan2(y,x);
        if(0 == zone) {
          phiMin=phiInt;
        } else if(3 == zone) {
          phiMax=phiInt;
        }
      }
      m_stripPhiLimits.push_back(std::pair<double,double>(phiMin,phiMax));
      Gaudi::XYZPoint begin(radius*cos(phiMin),radius*sin(phiMin),0.);
      Gaudi::XYZPoint end(radius*cos(phiMax),radius*sin(phiMax),0.);
      m_stripLimits.push_back(std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint>(begin,end));
    }
  }
  for(unsigned int i=0; i < m_phiMin.size(); i++){
    msg << MSG::DEBUG << "Zone limits; zone " << i << " min " << m_phiMin[i]
        << " max " << m_phiMax[i] << " phiMin " 
        << phiMinZone(i,innerRadius()) 
        << " max " << phiMaxZone(i,innerRadius()) << endmsg;
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
  MsgStream msg( msgSvc(), "DeVeloRType" );
  msg << MSG::VERBOSE << "cornerLimits" << endreq;
  // Cut offs defined by line
  m_cornerXInt = param<double>("RCornerXIntercept");
  m_cornerYInt = param<double>("RCornerYIntercept");
  double m = (m_cornerYInt) /  (-1.*m_cornerXInt);
  m_gradCutOff = m;
  double c = m_cornerYInt;
  m_intCutOff = c;
  m_cornerX1 = m_overlapInX;
  m_cornerY1 = m*m_cornerX1+c;
  intersectCutOff(innerRadius(),m_cornerX2,m_cornerY2);
  msg << MSG::DEBUG << "Cut off starts at x=" << m_cornerX1 << ",y=" << m_cornerY1
      << " and ends at x=" << m_cornerX2 << ",y=" << m_cornerY2 
      << " gradient=" << m_gradCutOff << " intercept=" << m_intCutOff << endreq;
}
//==============================================================================
// For a given radius, calculate point where circle crosses corner cut offs
//==============================================================================
void DeVeloRType::intersectCutOff(const double radius, double& x, double& y){
  MsgStream msg( msgSvc(), "DeVeloRType" );
  double m=m_gradCutOff;
  double c=m_intCutOff;
  double QuadA=(1+m*m);
  double QuadB=(2*m*c);
  double r=radius;
  double QuadC=c*c-r*r;
  double B2Minus4AC=QuadB*QuadB-4*QuadA*QuadC;
  if(B2Minus4AC > 0) {
    double x1 = ( -QuadB+sqrt(B2Minus4AC) ) / (2*QuadA);
    double y1 = m*x1+c;
    double x2 =  ( -QuadB-sqrt(B2Minus4AC) ) / (2*QuadA);
    double y2 = m*x2+c;
    if(x1 < x2) {
      x=x1;
      y=y1;
    } else {
      x=x2;
      y=y2;
    }
    msg << MSG::VERBOSE << "a=" << QuadA << ",b=" << QuadB << ",c=" << QuadC 
        << " Solution 1: x=" << x1 << " y=" << y1
        << " Solution 2: x=" << x2 << " y=" << y2
        << " Chose: x=" << x << " y=" << y
        << endreq;
  } else {
    msg << MSG::ERROR << "Unable to calculate corner intersect at r = " << r << endreq;
  }
}
//==============================================================================
/// Store the angular limits of zones sensor at +ve x
//==============================================================================
void DeVeloRType::phiZoneLimits()
{
  m_halfAngle     = 90.0 * degree;
  m_quarterAngle  = .5 * m_halfAngle;

  double phi;
  phi = acos(m_overlapInX/outerRadius());

  m_phiMin.clear();
  m_phiMax.clear();

  m_phiMin.push_back(-phi);
  m_phiMin.push_back(-m_quarterAngle);
  m_phiMin.push_back(0);
  m_phiMin.push_back(m_quarterAngle);
  
  m_phiMax.push_back(-m_quarterAngle);
  m_phiMax.push_back(0);
  m_phiMax.push_back(m_quarterAngle);
  m_phiMax.push_back(phi);
}
//==============================================================================
/// Build up routing line map
//==============================================================================
void DeVeloRType::BuildRoutingLineMap(){
  MsgStream msg( msgSvc(), "DeVeloRType" );
  /*  msg << MSG::DEBUG << "Building routing line map for sensor " 
      << (this->sensorNumber()) << endreq;*/
  for(unsigned int routLine=m_minRoutingLine;routLine<=m_maxRoutingLine/2;routLine++){
    unsigned int routArea=RoutingLineArea(routLine);
    unsigned int strip=RoutLineToStrip(routLine,routArea);
    // Sector 1
    m_mapStripToRoutingLine[strip]=routLine;
    m_mapRoutingLineToStrip[routLine]=strip;
    // sector 3
    m_mapStripToRoutingLine[strip+1024]=routLine+1024;
    m_mapRoutingLineToStrip[routLine+1024]=strip+1024;
    msg << MSG::VERBOSE << "Routing line " << routLine 
        << " area " << routArea
        << " strip " << m_mapRoutingLineToStrip[routLine]
        << " +1024 line " << routLine+1024
        << " +1024 strip " << m_mapRoutingLineToStrip[routLine+1024]
        << endreq;
    msg << MSG::VERBOSE << "Routing line " << routLine 
        << " strip " << RoutingLineToStrip(routLine)
        << " chip channel " << RoutingLineToChipChannel(routLine)
        << " and back " << ChipChannelToRoutingLine(RoutingLineToChipChannel(routLine))
        << " from strip " << endreq;
  }
}
//=============================================================================
// Select routing line area (pattern repeats for chips 7 to 0)
// (0) Chips 15 to 13 
// (1) Chip 12
// (2) Chip 11 
// (3) Chips 10 to 8
//=============================================================================
unsigned int DeVeloRType::RoutingLineArea(unsigned int routingLine){
  if(m_maxRoutingLine/2 < routingLine) routingLine /= 2;
  if(routingLine <= m_nChan0){
    return 0;
  } else if(routingLine <= m_nChan0+m_nChan1){
    return 1;
  } else if(routingLine <= m_nChan0+m_nChan1+m_nChan2){
    return 2;
  } else if (routingLine <= m_nChan0+m_nChan1+m_nChan2+m_nChan3){
    return 3;
  }
  return 999;
}
//=============================================================================
// Return strip from routing line area (first 1024 strips only)
//=============================================================================
unsigned int DeVeloRType::RoutLineToStrip(unsigned int routLine, unsigned int routArea){
  unsigned int strip;
  //  std::cout << "routLine " << routLine << " area " << routArea << std::endl;
  if(0 == routArea){
    strip = (m_nChan1+routLine-1);
  } else if(1 == routArea) {
    strip = m_nChan0+m_nChan1-routLine;
  } else if(2 == routArea){
    strip = routLine-1;
  } else if(3 == routArea){
    strip = (m_maxRoutingLine-m_nChan3-routLine);
  } else strip=9999;
  //  std::cout << "strip " << strip << " scram " << ScrambleStrip(strip) 
  //            << " rl " << routLine << " scram " << ScrambleStrip(routLine)
  //        << std::endl;
  return ScrambleStrip(strip);
}
//==============================================================================
// Return a trajectory (for track fit) from strip + offset
//==============================================================================
std::auto_ptr<LHCb::Trajectory> DeVeloRType::trajectory(const LHCb::VeloChannelID& id, 
                                                        const double offset) const {
    // r type is a circle
    double z = 0.;
    double radius = 0.;
    double phiMin = 0.;
    double phiMax = 0.;
    unsigned int strip = id.strip();
    StatusCode sc = stripLimits( strip, radius, phiMin, phiMax );
    if(!sc){
      throw GaudiException( "The trajectory could not be made",
                            "DeVELO.cpp",StatusCode::FAILURE );
    }
    // offset is offset on R
    radius += rPitch(strip) * offset;
    
    // start with coords of center and both ends in local frame
    Gaudi::XYZPoint lOrigin(0.,0.,0.);
    Gaudi::XYZPoint lEnd1(radius*cos(phiMin),radius*sin(phiMin),z);
    Gaudi::XYZPoint lEnd2(radius*cos(phiMax),radius*sin(phiMax),z);
    
    // move to global frame
    Gaudi::XYZPoint gOrigin, gEnd1, gEnd2;
    localToGlobal(lOrigin, gOrigin);
    localToGlobal(lEnd1, gEnd1);
    localToGlobal(lEnd2, gEnd2);
    
    // put into trajectory
    LHCb::Trajectory* tTraj = new LHCb::CircleTraj(gOrigin,gEnd1-gOrigin,gEnd2-gOrigin,radius);

    std::auto_ptr<LHCb::Trajectory> autoTraj(tTraj);
    
    return autoTraj;  

}
