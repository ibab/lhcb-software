// $Id: DeVeloPhiType.cpp,v 1.6 2004-02-24 18:27:09 mtobin Exp $
//==============================================================================
#define VELODET_DEVELOPHITYPE_CPP 1
//==============================================================================
// Include files 

// from CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

// From Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"

// From Velo
#include "VeloDet/DeVeloPhiType.h"
#include "VeloKernel/VeloRound.h"

/** @file DeVeloPhiType.cpp
 *
 *  Implementation of class : DeVeloPhiType
 *
 *  @author Mark Tobin  Mark.Tobin@cern.ch
 */

//==============================================================================
/// Standard constructor
//==============================================================================
DeVeloPhiType::DeVeloPhiType(const std::string& name) : DeVeloSensor(name)
{
}
//==============================================================================
/// Destructor
//==============================================================================
DeVeloPhiType::~DeVeloPhiType() {}
//==============================================================================
/// Object identification
//==============================================================================
const CLID& DeVeloPhiType::clID() 
  const { return DeVeloPhiType::classID(); }
//==============================================================================
/// Initialisation method
//==============================================================================
StatusCode DeVeloPhiType::initialize() 
{
  // Trick from old DeVelo to set the output level
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel=0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  jobSvc->setMyProperties("DeVeloPhiType", pmgr);
  if ( 0 < outputLevel ) {
    msgSvc()->setOutputLevel("DeVeloPhiType", outputLevel);
  }
  delete pmgr;
  MsgStream msg(msgSvc(), "DeVeloPhiType");
  
  sc = DeVeloSensor::initialize();
  if(!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DeVeloSensor" << endreq;
    return sc;
  }
  m_numberOfZones=2;
  m_numberOfStrips=this->numberOfStrips();
  m_nbInner = this->userParameterAsInt("NbPhiInner");
  m_stripsInZone.clear();
  m_stripsInZone.push_back(m_nbInner);
  m_stripsInZone.push_back(m_numberOfStrips-m_nbInner);
  m_innerRadius = this->innerRadius();
  m_outerRadius = this->outerRadius();
  m_middleRadius = this->userParameterAsDouble("PhiBoundRadius"); // PhiBound
  // Set the stereo angle
  m_isDownstream=false;
  if(this->userParameterAsString("Type") == "PhiDL" || 
     this->userParameterAsString("Type") == "PhiDR") m_isDownstream=true;
  // Point where strips of inner/outer regions cross
  m_phiOrigin = this->userParameterAsDouble("PhiOrigin");
  m_phiOrigin -= halfpi;
  /* Inner strips (dist. to origin defined by angle between 
     extrapolated strip and phi)*/
  m_innerDistToOrigin = this->userParameterAsDouble("InnerDistToOrigin");
  m_innerTilt = asin(m_innerDistToOrigin/m_innerRadius);
  m_innerTilt += m_phiOrigin;
  // Outer strips
  m_outerDistToOrigin = this->userParameterAsDouble("OuterDistToOrigin");
  m_outerTilt = asin(m_outerDistToOrigin/m_middleRadius);
  double phiAtBoundary   = m_innerTilt - 
    asin( m_innerDistToOrigin / m_middleRadius );
  m_outerTilt += phiAtBoundary;
  double phi = m_outerTilt - asin( m_outerDistToOrigin/m_outerRadius );
  msg << MSG::DEBUG << "Phi (degree) inner "    << m_phiOrigin/degree
      << " at boundary " << phiAtBoundary/degree
      << " and outside " << phi/degree
      << endreq;
  
  // Angular coverage
  m_innerCoverage = this->userParameterAsDouble("InnerCoverage");
  m_halfCoverage = 0.5*m_innerCoverage;
  m_innerPitch = m_innerCoverage / m_stripsInZone[0];
  m_outerCoverage = this->userParameterAsDouble("OuterCoverage");
  m_outerPitch = m_outerCoverage / m_stripsInZone[1];
  
  // Dead region
  m_rGap = this->userParameterAsDouble("PhiRGap");
  /// Corner cut offs
  cornerLimits();

  // the resolution of the sensor
  m_resolution.first = this->userParameterAsDouble("PhiResGrad");
  m_resolution.second = this->userParameterAsDouble("PhiResConst");
  
  /// Parametrize strips as lines
  calcStripLines();

  return StatusCode::SUCCESS;
}
//==============================================================================
/// Calculate the equation of a line for each strip
//==============================================================================
void DeVeloPhiType::calcStripLines()
{
  m_stripLines.clear();
  double x1,y1,x2,y2;
  for(unsigned int strip=0; strip<m_numberOfStrips; strip++){
    if(m_nbInner > strip) {
      x1 = m_innerRadius * cos(phiOfStrip(strip,0.,m_innerRadius));
      y1 = m_innerRadius * sin(phiOfStrip(strip,0.,m_innerRadius));
      x2 = m_middleRadius * cos(phiOfStrip(strip,0.,m_middleRadius-m_rGap/2));
      y2 = m_middleRadius * sin(phiOfStrip(strip,0.,m_middleRadius-m_rGap/2));
    } else {
      x1 = m_middleRadius * cos(phiOfStrip(strip,0.,m_middleRadius+m_rGap/2));
      y1 = m_middleRadius * sin(phiOfStrip(strip,0.,m_middleRadius+m_rGap/2));
      x2 = m_outerRadius * cos(phiOfStrip(strip,0.,m_outerRadius));
      y2 = m_outerRadius * sin(phiOfStrip(strip,0.,m_outerRadius));
    }
    double gradient;
    gradient = (y2 - y1) /  (x2 - x1);
    double intercept;
    intercept = y2 - (gradient*x2);
    m_stripLines.push_back(std::pair<double,double>(gradient,intercept));
    // Store strip limits in vector
    if(isCutOff(x1,y1)){
      if(0 < y1){
        x1 = (intercept - m_cutOffs[0].second) / 
          (m_cutOffs[0].first - gradient);
      } else {
        x1 = (intercept - m_cutOffs[1].second) / 
          (m_cutOffs[1].first - gradient);
      }
      y1 = gradient*x1 + intercept;
    }
    HepPoint3D begin(x1,y1,0);
    HepPoint3D end(x2,y2,0);
    //HepPoint3D begin;
    //HepPoint3D end;
    //StatusCode sc=this->localToGlobal(HepPoint3D(x1,y1,0),begin);
    //sc=this->localToGlobal(HepPoint3D(x2,y2,0),end);
    m_stripLimits.push_back(std::pair<HepPoint3D,HepPoint3D>(begin,end));
  }
}
//==============================================================================
/// Store line defining corner cut-offs
//==============================================================================
void DeVeloPhiType::cornerLimits()
{
  m_cutOffs.clear();
  /// First corner
  m_corner1X1 = this->userParameterAsDouble("PhiCorner1X1");
  m_corner1Y1 = this->userParameterAsDouble("PhiCorner1Y1");
  m_corner1X2 = this->userParameterAsDouble("PhiCorner1X2");
  m_corner1Y2 = this->userParameterAsDouble("PhiCorner1Y2");
  double gradient;
  gradient = (m_corner1Y2 - m_corner1Y1) /  (m_corner1X2 - m_corner1X1);
  double intercept;
  intercept = m_corner1Y2 - (gradient*m_corner1X2);
  m_cutOffs.push_back(std::pair<double,double>(gradient,intercept));
  /// Second corner
  m_corner2X1 = this->userParameterAsDouble("PhiCorner2X1");
  m_corner2Y1 = this->userParameterAsDouble("PhiCorner2Y1");
  m_corner2X2 = this->userParameterAsDouble("PhiCorner2X2");
  m_corner2Y2 = this->userParameterAsDouble("PhiCorner2Y2");
  gradient = (m_corner2Y2 - m_corner2Y1) /  (m_corner2X2 - m_corner2X1);
  intercept = m_corner2Y2 - (gradient*m_corner2X2);
  m_cutOffs.push_back(std::pair<double,double>(gradient,intercept));
}
//==============================================================================
/// Calculate the nearest channel to a 3-d point.
//==============================================================================
StatusCode DeVeloPhiType::pointToChannel(const HepPoint3D& point,
                          VeloChannelID& channel,
                          double& fraction,
                          double& pitch)
{
  MsgStream msg(msgSvc(), "DeVeloPhiType");
  HepPoint3D localPoint(0,0,0);
  StatusCode sc = this->globalToLocal(point,localPoint);
  if(!sc.isSuccess()) return sc;
  double radius=localPoint.perp();

  // Check boundaries...
  sc = isInside(localPoint);
  if(!sc.isSuccess()) return sc;

  // Use symmetry to handle second stereo...
  double phi=localPoint.phi();
  if(m_isDownstream) {
    //    phi = -phi;
  }
  
  // Calculate nearest channel....
  unsigned int closestStrip;
  double strip=0;
  phi -= phiOffset(radius);
  if(m_middleRadius > radius) {
    strip = phi / m_innerPitch;
  } else {
    strip = phi / m_outerPitch;
    strip += m_nbInner;
  }
  closestStrip = VeloRound::round(strip);
  fraction = strip - closestStrip;

  pitch = phiPitch(radius);
  unsigned int sensor=this->sensorNumber();
  
  // set VeloChannelID....
  channel.setSensor(sensor);
  channel.setStrip(closestStrip);
  channel.setType(VeloChannelID::PhiType);

  msg << MSG::VERBOSE << "pointToChannel; local phi " << localPoint.phi()/degree
      << " radius " << localPoint.perp() 
      << " phiOffset " << phiOffset(radius)/degree
      << " phi corrected " << phi/degree << endreq;
  msg << MSG::VERBOSE << " strip " << strip << " closest strip " << closestStrip
      << " fraction " << fraction <<endreq;
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Get the nth nearest neighbour for a given channel
//==============================================================================
StatusCode DeVeloPhiType::neighbour(const VeloChannelID& start, 
                                    const int& nOffset, 
                                    VeloChannelID& channel)
{
  unsigned int strip=0;
  strip = start.strip();
  unsigned int startZone=0;
  startZone = zoneOfStrip(strip);
  strip += nOffset;
  unsigned int endZone=0;
  endZone = zoneOfStrip(strip);
  // put in some checks for boundaries etc...
  if(m_numberOfStrips < strip) return StatusCode::FAILURE;
  if(startZone != endZone) {
    return StatusCode::FAILURE;
  }
  channel = start;
  channel.setStrip(strip);
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Checks if local point is inside sensor
//==============================================================================
StatusCode DeVeloPhiType::isInside(const HepPoint3D& point)
{
  MsgStream msg(msgSvc(), "DeVeloPhiType");
  // check boundaries....  
  double radius=point.perp();
  if(m_innerRadius > radius || m_outerRadius < radius) {
    msg << MSG::VERBOSE << "Outside active radii " << radius << endreq;
    return StatusCode::FAILURE;
  }
  bool isInner=true;
  if(m_middleRadius < radius) {
    isInner=false;
  }
  // Is it inside angular coverage
  double xCross=m_middleRadius*cos(phiOfStrip(0,0.,m_middleRadius));
  if(xCross > point.x()) {
    msg << MSG::VERBOSE << "Inner " << isInner 
        << " Outside angular coverage: x, xmax " << point.x()
        << "," << xCross << endreq;
    return StatusCode::FAILURE;
  }

  // Dead region
  if(m_middleRadius+m_rGap > radius && m_middleRadius-m_rGap < radius){
    msg << MSG::VERBOSE << "Inner " << isInner 
        << " Inside dead region from bias line: " << radius 
        << endreq;
    return StatusCode::FAILURE;
  }
  // Corner cut-offs
  //  if(m_isDownstream) y = -y;
  double x=point.x();
  double y=point.y();
  bool isCutOff=this->isCutOff(x,y);
  if(isCutOff) {
    msg << MSG::VERBOSE << "Inner " << isInner << " Inside corner cut-off " 
        << x << "," << y << endreq;
    return StatusCode::FAILURE;
  }
  // Work out if x/y is outside first/last strip in zone..
  unsigned int endStrip;
  if(isInner){
    endStrip = 0;
    if(0 < y) endStrip = stripsInZone(0)-1;
  } else {
    endStrip = stripsInZone(0);
    if(0 < y) endStrip = numberOfStrips()-1;
  }
  // Work out if point is outside active region
  double gradient=m_stripLines[endStrip].first;
  double intercept=m_stripLines[endStrip].second;
  double xMin=m_stripLimits[endStrip].first.x();
  double xMax=m_stripLimits[endStrip].second.x();
  if(xMin > xMax) {
    double xTemp=xMax;
    xMax = xMin;
    xMin = xTemp;
  }
  if(xMax < x) return StatusCode::SUCCESS;
  double yMin=m_stripLimits[endStrip].first.y();
  double yMax=m_stripLimits[endStrip].second.y();
  if(yMin > yMax) {
    double yTemp=yMax;
    yMax = yMin;
    yMin = yTemp;
  }
  double yAtX=gradient*x+intercept;
  if(0 < y && isInner) {
    if(0 < gradient && yAtX < y) {
      msg << MSG::VERBOSE << "end strip " << endStrip << " +ve gradient"
          << " yAtX " << yAtX << " y " << y << " inner " << isInner << endreq;
      return StatusCode::FAILURE;
    }
  } else if(0 < y && !isInner) {
    if(0 > gradient && yAtX > y) {
      msg << MSG::VERBOSE << "end strip " << endStrip << " -ve gradient"
          << " yAtX " << yAtX << " y " << y << " inner " << isInner << endreq;
      return StatusCode::FAILURE;
    }
  } else if(0 > y && isInner) {
    if(0 < gradient && yAtX < y) {
      msg << MSG::VERBOSE << "end strip " << endStrip << " +ve gradient"
          << " yAtX " << yAtX << " y " << y << " inner " << isInner << endreq;
      return StatusCode::FAILURE;
    }    
  } else if(0 > y && !isInner) {
    if(0 > gradient && yAtX > y) {
      msg << MSG::VERBOSE << "end strip " << endStrip << " -ve gradient"
          << " yAtX " << yAtX << " y " << y << " inner " << isInner << endreq;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Is the local point in the corner cut-off?
//==============================================================================
bool DeVeloPhiType::isCutOff(double x, double y)
{
  // Use symmetry to handle second stereo...
  if(m_isDownstream) {
    //    y = -y;
  }
  // if(m_corner1X1 > x) return true;
  double radius=sqrt(x*x+y*y);
  if(m_middleRadius < radius) return false;
  if(0 < y) {
    // First corner
    if(/*m_corner1X1 < x &&*/ m_corner1X2 > x) {
      double yMax=m_cutOffs[0].first*x+m_cutOffs[0].second;
      if(yMax > y) {
        return true;
      }
    }
  } else {
    // Second corner
    if(/*m_corner2X1 < x &&*/ m_corner2X2 > x) {
      double yMin=m_cutOffs[1].first*x+m_cutOffs[1].second;
      if(yMin < y) {
        return true;
      }
    }
  }
  return false;
}
//==============================================================================
/// Returns the number of channels between two channels
//==============================================================================
StatusCode DeVeloPhiType::channelDistance(const VeloChannelID& start,
                                          const VeloChannelID& end,
                                          int& nOffset)
{
  nOffset = 0;
  unsigned int startStrip=0;
  unsigned int endStrip=0;
  startStrip = start.strip();
  endStrip = end.strip();
  if(m_numberOfStrips<startStrip || m_numberOfStrips<endStrip) {
    return StatusCode::FAILURE;
  }
  // put in some checks for boundaries etc...
  unsigned int startZone;
  startZone = zoneOfStrip(startStrip);
  unsigned int endZone;
  endZone = zoneOfStrip(endStrip);
  if(startZone != endZone) {
    return StatusCode::FAILURE;
  }
  nOffset = endStrip-startStrip;
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Residual of 3-d point to a VeloChannelID
//==============================================================================
StatusCode DeVeloPhiType::residual(const HepPoint3D& point, 
                                   const VeloChannelID& channel,
                                   double &residual,
                                   double &chi2)
{
  MsgStream msg(msgSvc(), "DeVeloPhiType");
  HepPoint3D localPoint(0,0,0);
  StatusCode sc=DeVeloSensor::globalToLocal(point,localPoint);
  if(!sc.isSuccess()) return sc;
  sc = isInside(localPoint);
  if(!sc.isSuccess()) return sc;

  double x=localPoint.x();
  double y=localPoint.y();
  //  if(m_isDownstream) y = -y;
  // Work out closest point on line
  unsigned int strip;
  strip=channel.strip();
  double xNear = (m_stripLines[strip].first*y+x-
                  m_stripLines[strip].first*m_stripLines[strip].second);
  xNear /= (1+pow(m_stripLines[strip].first,2));
  
  double yNear = m_stripLines[strip].first*xNear + m_stripLines[strip].second;
  
  residual = sqrt(pow(xNear-x,2)+pow(yNear-y,2));
  double radius = localPoint.perp();
  double sigma = m_resolution.first*phiPitch(radius) - m_resolution.second;
  chi2 = pow(residual/sigma,2);
  
  msg << MSG::VERBOSE << "Residual; sensor " << channel.sensor()
      << " strip " << strip 
      << " x " << x << " y " << y << endreq;
  msg << MSG::VERBOSE << " xNear " << xNear << " yNear " << yNear 
      << " residual " << residual << " sigma = " << sigma
      << " chi2 = " << chi2 << endreq;
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Residual [see DeVelo for explanation]
//==============================================================================
StatusCode DeVeloPhiType::residual(const HepPoint3D& /*point*/,
                                   const VeloChannelID& /*channel*/,
                                   const double /*localOffset*/,
                                   const double /*width*/,
                                   double &/*residual*/,
                                   double &/*chi2*/)
{
  return StatusCode::SUCCESS;
}
//==============================================================================
/// The minimum radius for a given zone of the sensor
//==============================================================================
double DeVeloPhiType::rMin(const unsigned int zone)
{
  double rMin=0;
  if(zone == 0) {
    rMin = m_innerRadius;
  } else if (zone == 1) {
    rMin = m_middleRadius;
  }
  return rMin;
}
//==============================================================================
/// The maximum radius for a given zone of the sensor
//==============================================================================
double DeVeloPhiType::rMax(const unsigned int zone)
{
  double rMax=0;
  if(zone == 0) {
    rMax = m_middleRadius;
  } else if (zone == 1) {
    rMax = m_outerRadius;
  }
  return rMax;
}
//==============================================================================
/// The minimum phi of the sensor for a given radius
//==============================================================================
double DeVeloPhiType::phiMin(const double radius)
{
  double minPhi; // [NEED] Put correct values in
  if (m_middleRadius > radius) {
    minPhi = 0;
  } else {
    minPhi = m_halfCoverage;
  }
  return minPhi;
}
//==============================================================================
/// The maximum phi of the sensor for a given radius
//==============================================================================
double DeVeloPhiType::phiMax(const double radius)
{
  double maxPhi; // [NEED] Put correct values in
  if (m_middleRadius > radius) {
    maxPhi = 0;
  } else {
    maxPhi = m_halfCoverage;
  }
  return maxPhi;
}
//==============================================================================
/// Return the capacitance of the strip
//==============================================================================
double DeVeloPhiType::stripCapacitance(unsigned int /*strip*/)
{
  double C=0.0;
  return C;
}
