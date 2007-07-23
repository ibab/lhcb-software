// $Id: DeVeloPhiType.cpp,v 1.36 2007-07-23 01:08:55 krinnert Exp $
//==============================================================================
#define VELODET_DEVELOPHITYPE_CPP 1
//==============================================================================
// Include files 

// From Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// From LHCb
#include "LHCbMath/LHCbMath.h"
#include "gsl/gsl_math.h"

// From Velo
#include "VeloDet/DeVeloPhiType.h"

namespace VeloDet {
/** This function simply provides access to a local static
 *  data which is used to initialize references in each instance
 *  of DeVeloPhiType.
 *  The purpose of this function is to work around
 *  a Microsoft(tm) specific extension in VC++ that makes
 *  awkward to have static data mebers accessed by inline
 *  funtions.
 *
 *  @see DeVeloPhiType
 */
  static std::vector<double>& deVeloPhiTypeStaticStripLengths()
  {
    static std::vector<double> s_stripLengths;
    return s_stripLengths;
  }
}

// used to control initialization
bool DeVeloPhiType::m_staticDataInvalid = true;

std::vector<std::pair<double,double> > DeVeloPhiType::m_stripLines;
//std::vector<std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> > DeVeloPhiType::m_stripLimits;

/** @file DeVeloPhiType.cpp
 *
 *  Implementation of class : DeVeloPhiType
 *
 *  @author Mark Tobin  Mark.Tobin@cern.ch
 *  @author Kurt Rinnert kurt.rinnert@cern.ch
 */

//==============================================================================
/// Standard constructor
//==============================================================================
DeVeloPhiType::DeVeloPhiType(const std::string& name) 
  : DeVeloSensor(name)
  , m_globalPhi(m_numberOfStrips,0.0)
  , m_halfboxPhi(m_numberOfStrips,0.0)
  , m_idealPhi(m_numberOfStrips,0.0)
  , m_associatedRSensor(0)
  , m_otherSidePhiSensor(0)
  , m_stripLengths(VeloDet::deVeloPhiTypeStaticStripLengths())
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
  if( sc.isSuccess() ) sc = jobSvc->setMyProperties("DeVeloPhiType", pmgr);
  if( !sc ) return sc;
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
  m_debug   = (msgSvc()->outputLevel("DeVeloPhiType") == MSG::DEBUG  ) ;
  m_verbose = (msgSvc()->outputLevel("DeVeloPhiType") == MSG::VERBOSE) ;

  m_numberOfZones=2;
  m_nbInner = param<int>("NbPhiInner");
  m_stripsInZone.clear();
  m_stripsInZone.push_back(m_nbInner);
  m_stripsInZone.push_back(numberOfStrips()-m_nbInner);
  m_middleRadius = param<double>("PhiBoundRadius"); // PhiBound
  // Point where strips of inner/outer regions cross
  m_phiOrigin = param<double>("PhiOrigin");
  //  std::cout << "m_phiOrigin " << m_phiOrigin;
  m_phiOrigin -= Gaudi::Units::halfpi;
  //  std::cout << " m_phiOrigin " << m_phiOrigin;
  /* Inner strips (dist. to origin defined by angle between 
     extrapolated strip and phi)*/
  m_innerDistToOrigin = param<double>("InnerDistToOrigin");
  //  std::cout << " dist2O " << m_innerDistToOrigin;
  m_innerTilt = asin(m_innerDistToOrigin/innerRadius());
  //  std::cout << " tilt " << m_innerTilt;
  m_innerTilt += m_phiOrigin;
  //  std::cout << " tilt " << m_innerTilt;
  //  m_innerTilt = 4.6235;
  //  std::cout << " tilt " << m_innerTilt << std::endl;
  // Outer strips
  m_outerDistToOrigin = param<double>("OuterDistToOrigin");
  m_outerTilt = asin(m_outerDistToOrigin/m_middleRadius);
  double phiAtBoundary   = m_innerTilt - 
    asin( m_innerDistToOrigin / m_middleRadius );
  m_outerTilt += phiAtBoundary;
  double phi = m_outerTilt - asin( m_outerDistToOrigin/outerRadius() );
  msg << MSG::DEBUG << "Phi (degree) inner "    << m_phiOrigin/Gaudi::Units::degree
      << " at boundary " << phiAtBoundary/Gaudi::Units::degree
      << " and outside " << phi/Gaudi::Units::degree
      << endreq;
  
  // Angular coverage
  m_innerCoverage = param<double>("InnerCoverage");
  //  m_halfCoverage = 0.5*m_innerCoverage;
  m_innerPitch = m_innerCoverage / m_stripsInZone[0];
  m_outerCoverage = param<double>("OuterCoverage");
  m_outerPitch = m_outerCoverage / m_stripsInZone[1];
  
  // Dead region
  m_rGap = param<double>("PhiRGap");
  /// Corner cut offs
  cornerLimits();

  // the resolution of the sensor
  m_resolution.first = param<double>("PhiResGrad");
  m_resolution.second = param<double>("PhiResConst");
  
  /// Parametrize strips as lines
  calcStripLines();

  /// Calculated the length of the strips
  calcStripLengths();

  /// Build up map of strips to routing lines
  BuildRoutingLineMap();

  // fill global phi cache
  sc = updateGeometryCache();
  if(!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to update geometry cache." << endreq;
    return sc;
  }
  
  // register geometry conditions, update global r of strip cache
  updMgrSvc()->
    registerCondition(this,this->m_geometry,&DeVeloPhiType::updateGeometryCache);

  return StatusCode::SUCCESS;
}
//==============================================================================
/// Calculate the equation of a line for each strip
//==============================================================================
void DeVeloPhiType::calcStripLines()
{
  m_stripLines.clear();
  double x1,y1,x2,y2;
  for(unsigned int strip=0; strip<numberOfStrips(); ++strip){
    double rInner;
    double rOuter;
    if(m_nbInner > strip) {
      rInner=innerRadius();
      rOuter=m_middleRadius-m_rGap/2;
//       x1 = innerRadius() * cos(phiOfStrip(strip,0.,innerRadius()));
//       y1 = innerRadius() * sin(phiOfStrip(strip,0.,innerRadius()));
//       x2 = (m_middleRadius-m_rGap/2) * cos(phiOfStrip(strip,0.,m_middleRadius-m_rGap/2));
//       y2 = (m_middleRadius-m_rGap/2) * sin(phiOfStrip(strip,0.,m_middleRadius-m_rGap/2));
    } else {
      rInner=m_middleRadius+m_rGap/2;
      rOuter=outerRadius();
//       x1 = (m_middleRadius+m_rGap/2) * cos(phiOfStrip(strip,0.,m_middleRadius+m_rGap/2));
//       y1 = (m_middleRadius+m_rGap/2) * sin(phiOfStrip(strip,0.,m_middleRadius+m_rGap/2));
//       x2 = outerRadius() * cos(phiOfStrip(strip,0.,outerRadius()));
//       y2 = outerRadius() * sin(phiOfStrip(strip,0.,outerRadius()));
    }
    x1 = rInner * cos(phiOfStrip(strip,0.,rInner));
    y1 = rInner * sin(phiOfStrip(strip,0.,rInner));
    x2 = rOuter * cos(phiOfStrip(strip,0.,rOuter));
    y2 = rOuter * sin(phiOfStrip(strip,0.,rOuter));

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
    Gaudi::XYZPoint begin(x1,y1,0);
    Gaudi::XYZPoint end(x2,y2,0);
    if(m_verbose) {
      MsgStream msg(msgSvc(), "DeVeloPhiType");
      msg << MSG::VERBOSE << "Sensor " << sensorNumber() << " " << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
    }
    //Gaudi::XYZPoint begin;
    //Gaudi::XYZPoint end;
    //StatusCode sc=this->localToGlobal(Gaudi::XYZPoint(x1,y1,0),begin);
    //sc=this->localToGlobal(Gaudi::XYZPoint(x2,y2,0),end);
    m_stripLimits.push_back(std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint>(begin,end));
  }
}
//==============================================================================
/// Fill vector of strip lengths
//==============================================================================
void DeVeloPhiType::calcStripLengths()
{
  // we only have to do this once. the strip lengths are 
  // stored in statics, i.e. are technically the same
  // for all instances of DeVeloPhiType
  if (m_staticDataInvalid) {
    m_staticDataInvalid=false;
    std::vector< std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> >::const_iterator iStLi = m_stripLimits.begin(); 
    for( ; iStLi != m_stripLimits.end() ; ++iStLi ) {
      double x1 = iStLi->first.x();
      double y1 = iStLi->first.y();
      double z1 = iStLi->first.z();
      double x2 = iStLi->second.x();
      double y2 = iStLi->second.y();
      double z2 = iStLi->second.z();
      double length = gsl_pow_2(x2-x1) + gsl_pow_2(y2-y1) + gsl_pow_2(z2-z1);
      m_stripLengths.push_back(sqrt(length));
    }
  }
}
//==============================================================================
/// Store line defining corner cut-offs
//==============================================================================
void DeVeloPhiType::cornerLimits()
{
  m_cutOffs.clear();
  /// First corner
  m_corner1X1 = param<double>("PhiCorner1X1");
  m_corner1Y1 = param<double>("PhiCorner1Y1");
  m_corner1X2 = param<double>("PhiCorner1X2");
  m_corner1Y2 = param<double>("PhiCorner1Y2");
  double gradient;
  gradient = (m_corner1Y2 - m_corner1Y1) /  (m_corner1X2 - m_corner1X1);
  double intercept;
  intercept = m_corner1Y2 - (gradient*m_corner1X2);
  m_cutOffs.push_back(std::pair<double,double>(gradient,intercept));
  /// Second corner
  m_corner2X1 = param<double>("PhiCorner2X1");
  m_corner2Y1 = param<double>("PhiCorner2Y1");
  m_corner2X2 = param<double>("PhiCorner2X2");
  m_corner2Y2 = param<double>("PhiCorner2Y2");
  gradient = (m_corner2Y2 - m_corner2Y1) /  (m_corner2X2 - m_corner2X1);
  intercept = m_corner2Y2 - (gradient*m_corner2X2);
  m_cutOffs.push_back(std::pair<double,double>(gradient,intercept));
}
//==============================================================================
/// Calculate the nearest channel to a 3-d point.
//==============================================================================
StatusCode DeVeloPhiType::pointToChannel(const Gaudi::XYZPoint& point,
                          LHCb::VeloChannelID& channel,
                          double& fraction,
                          double& pitch) const
{
  Gaudi::XYZPoint localPoint = globalToLocal(point);
  double radius=localPoint.Rho();

  // Check boundaries...
  StatusCode sc = isInActiveArea(localPoint);
  if(!sc.isSuccess()) return sc;

  // Use symmetry to handle second stereo...
  double phi=localPoint.phi();
//   if(isDownstream()) {
//     //    phi = -phi;
//   }
  
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
  closestStrip = LHCbMath::round(strip);
  fraction = strip - closestStrip;

  pitch = phiPitch(radius);
  unsigned int sensor=sensorNumber();
  
  // set VeloChannelID....
  channel.setSensor(sensor);
  channel.setStrip(closestStrip);
  channel.setType(LHCb::VeloChannelID::PhiType);

  if(m_verbose) {
    MsgStream msg(msgSvc(), "DeVeloPhiType");
    msg << MSG::VERBOSE << "pointToChannel; local phi " << localPoint.phi()/Gaudi::Units::degree
        << " radius " << localPoint.Rho() 
        << " phiOffset " << phiOffset(radius)/Gaudi::Units::degree
        << " phi corrected " << phi/Gaudi::Units::degree << endreq;
    msg << MSG::VERBOSE << " strip " << strip << " closest strip " << closestStrip
        << " fraction " << fraction <<endreq;
  }
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Get the nth nearest neighbour for a given channel
//==============================================================================
StatusCode DeVeloPhiType::neighbour(const LHCb::VeloChannelID& start, 
                                    const int& nOffset, 
                                    LHCb::VeloChannelID& channel) const
{
  unsigned int strip=0;
  strip = start.strip();
  unsigned int startZone=0;
  startZone = zoneOfStrip(strip);
  strip += nOffset;
  unsigned int endZone=0;
  endZone = zoneOfStrip(strip);
  // put in some checks for boundaries etc...
  if(numberOfStrips() <= strip) return StatusCode::FAILURE;
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
StatusCode DeVeloPhiType::isInActiveArea(const Gaudi::XYZPoint& point) const
{
  MsgStream msg(msgSvc(), "DeVeloPhiType");
  msg << MSG::VERBOSE << "isInActiveArea: x=" << point.x() << ",y=" << point.y() 
      << endreq;
  //  check boundaries....  
  double radius=point.Rho();
  if(innerRadius() >= radius || outerRadius() <= radius) {
    //    msg << MSG::VERBOSE << "Outside active radii " << radius << endreq;
    return StatusCode::FAILURE;
  }
  bool isInner=true;
  if(m_middleRadius < radius) {
    isInner=false;
  }
  // Dead region
  if(m_middleRadius+m_rGap > radius && m_middleRadius-m_rGap < radius){
    /*    msg << MSG::VERBOSE << "Inner " << isInner 
          << " Inside dead region from bias line: " << radius 
          << endreq;*/
    return StatusCode::FAILURE;
  }
  // Corner cut-offs
  //  if(m_isDownstream) y = -y;
  double x=point.x();
  double y=point.y();
  bool cutOff=isCutOff(x,y);
  if(cutOff) {
    /*    msg << MSG::VERBOSE << "Inner " << isInner << " Inside corner cut-off " 
          << x << "," << y << endreq;*/
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
  double phi=atan2(y,x);
  double phiStrip=phiOfStrip(endStrip,0.0,radius);
  //  double pitch=phiPitch(radius);
  if(0 > y) {
    //    phiStrip -= pitch/2;
    //    std::cout << ",phiStrip=" << phiStrip/Gaudi::Units::degree << std::endl;
    if(phiStrip > phi) return StatusCode::FAILURE;
  } else {
    //    phiStrip += pitch/2;
    //    std::cout << ",phiStrip=" << phiStrip/Gaudi::Units::degree << std::endl;
    if(phiStrip < phi) return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Is the local point in the corner cut-off?
//==============================================================================
bool DeVeloPhiType::isCutOff(double x, double y) const
{
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
/// Residual of 3-d point to a VeloChannelID
//==============================================================================
StatusCode DeVeloPhiType::residual(const Gaudi::XYZPoint& point, 
                                   const LHCb::VeloChannelID& channel,
                                   double &residual,
                                   double &chi2) const
{
  return this->residual(point, channel, 0.0, residual, chi2);
}
//==============================================================================
/// Residual of 3-d point to a VeloChannelID + interstrip fraction
//==============================================================================
StatusCode DeVeloPhiType::residual(const Gaudi::XYZPoint& point,
                                   const LHCb::VeloChannelID& channel,
                                   const double interStripFraction,
                                   double &residual,
                                   double &chi2) const
{
  MsgStream msg(msgSvc(), "DeVeloPhiType");
  Gaudi::XYZPoint localPoint = DeVeloSensor::globalToLocal(point);

  StatusCode sc = isInActiveArea(localPoint);
  if(!sc.isSuccess()) return sc;

  // Get start/end co-ordinates of channel's strip
  unsigned int strip;
  strip=channel.strip();
  std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> stripLimits = localStripLimits(strip);
  Gaudi::XYZPoint stripBegin = stripLimits.first;
  Gaudi::XYZPoint stripEnd   = stripLimits.second;

  // Add offset a la trajectory
  Gaudi::XYZPoint nextStripBegin, nextStripEnd;
  if(interStripFraction > 0. ){
    stripLimits = localStripLimits(strip+1);
    nextStripBegin = stripLimits.first;
    nextStripEnd   = stripLimits.second;
    stripBegin += (nextStripBegin-stripBegin)*interStripFraction;
    stripEnd   += (nextStripEnd-stripEnd)*interStripFraction;
  }else if(interStripFraction < 0.) { // This should never happen for clusters
    stripLimits = localStripLimits(strip-1);
    nextStripBegin = stripLimits.first;
    nextStripEnd   = stripLimits.second;
    stripBegin += (stripBegin-nextStripBegin)*interStripFraction;
    stripEnd   += (stripEnd-nextStripEnd)*interStripFraction;
  }
  // calculate equation of line for strip
  double gradient = (stripEnd.y()-stripBegin.y()) / (stripEnd.x()-stripBegin.x());
  double intercept = stripBegin.y() - gradient*stripBegin.x();

  double x=localPoint.x();
  double y=localPoint.y();
  double xNear = (gradient*y+x-gradient*intercept);
  xNear /= (1+gsl_pow_2(gradient));
  
  double yNear = gradient*xNear + intercept;
  
  residual = sqrt(gsl_pow_2(xNear-x)+gsl_pow_2(yNear-y));

  // Work out how to calculate the sign!
  Gaudi::XYZPoint localNear(xNear,yNear,0.0);
  Gaudi::XYZPoint globalNear = DeVeloSensor::localToGlobal(localNear);
  if(point.phi() < globalNear.phi()) residual *= -1.;

  double radius = localPoint.Rho();
  double sigma = m_resolution.first*phiPitch(radius) - m_resolution.second;
  chi2 = gsl_pow_2(residual/sigma);
  
  msg << MSG::VERBOSE << "Residual; sensor " << channel.sensor()
      << " strip " << strip 
      << " x " << x << " y " << y << endreq;
  msg << MSG::VERBOSE << " xNear " << xNear << " yNear " << yNear 
      << " residual " << residual << " sigma = " << sigma
      << " chi2 = " << chi2 << endreq;
  return StatusCode::SUCCESS;
}
//==============================================================================
/// The minimum radius for a given zone of the sensor
//==============================================================================
double DeVeloPhiType::rMin(const unsigned int zone) const
{
  double rMin=0;
  if(zone == 0) {
    rMin = innerRadius();
  } else if (zone == 1) {
    rMin = m_middleRadius;
  }
  return rMin;
}
//==============================================================================
/// The maximum radius for a given zone of the sensor
//==============================================================================
double DeVeloPhiType::rMax(const unsigned int zone) const
{
  double rMax=0;
  if(zone == 0) {
    rMax = m_middleRadius;
  } else if (zone == 1) {
    rMax = outerRadius();
  }
  return rMax;
}
//==============================================================================
/// Return the length of a strip
//==============================================================================
double DeVeloPhiType::stripLength(const unsigned int strip) const { 
  return m_stripLengths[strip];
}
//=============================================================================
// Build map of strips to routing line and back
//=============================================================================
void DeVeloPhiType::BuildRoutingLineMap(){
  unsigned int strip=0;
  m_patternConfig.push_back(std::pair<unsigned int,unsigned int>(m_nbInner,4));
  m_patternConfig.push_back(std::pair<unsigned int,unsigned int>(0,2));
  m_patternConfig.push_back(std::pair<unsigned int,unsigned int>(m_nbInner+1,4));
  m_patternConfig.push_back(std::pair<unsigned int,unsigned int>(1,2));
  m_patternConfig.push_back(std::pair<unsigned int,unsigned int>(m_nbInner+2,4));
  m_patternConfig.push_back(std::pair<unsigned int,unsigned int>(m_nbInner+3,4));

  unsigned int count=0;
  MsgStream msg( msgSvc(), "DeVeloPhiType" );
  msg << MSG::DEBUG << "Building routing line map for sensor " 
      << (this->sensorNumber()) << endreq;
  for(unsigned int routLine=m_minRoutingLine;routLine<=m_maxRoutingLine;++routLine,++count){
    if(0 == count%6){
      msg << MSG::DEBUG << "Pattern of six ---------------------------------------\n";
    }
    strip=this->strip(routLine);
    
    m_mapStripToRoutingLine[strip]=routLine;
    m_mapRoutingLineToStrip[routLine]=strip;
    
    msg << MSG::DEBUG << "Routing line " << routLine 
        << " Patttern element " << (patternElement(routLine))
        << " number " << (patternNumber(routLine))
        << " strip " << strip
        << endreq;
  }
}
//==============================================================================
// Return a trajectory (for track fit) from strip + offset
std::auto_ptr<LHCb::Trajectory> DeVeloPhiType::trajectory(const LHCb::VeloChannelID& id, 
                                                          const double offset) const {
  // Trajectory is a line
  Gaudi::XYZPoint lEnd1, lEnd2;
  unsigned int strip=id.strip();
  //StatusCode sc = stripLimits(strip,lEnd1,lEnd2);
  std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> localCoords;
  localCoords=localStripLimits(strip);
  lEnd1 = localCoords.first;
  lEnd2 = localCoords.second;
  // need to also grab next strip in local frame to get offset effect
  Gaudi::XYZPoint lNextEnd1, lNextEnd2;
  // check direction of offset
  // do nothing if offset == 0.
  if(offset > 0. ){
    localCoords = localStripLimits(strip+1);
    lNextEnd1 = localCoords.first;
    lNextEnd2 = localCoords.second;
    lEnd1 += (lNextEnd1-lEnd1)*offset;
    lEnd2 += (lNextEnd2-lEnd2)*offset;
  }else if(offset < 0.) {
    localCoords = localStripLimits(strip-1);
    lNextEnd1 = localCoords.first;
    lNextEnd2 = localCoords.second;
    lEnd1 += (lEnd1-lNextEnd1)*offset;
    lEnd2 += (lEnd2-lNextEnd2)*offset;
  }
  
  Gaudi::XYZPoint gEnd1 = localToGlobal(lEnd1);
  Gaudi::XYZPoint gEnd2 = localToGlobal(lEnd2);

  // put into trajectory
  LHCb::Trajectory* tTraj = new LHCb::LineTraj(gEnd1,gEnd2);

  std::auto_ptr<LHCb::Trajectory> autoTraj(tTraj);
    
  return autoTraj;  

}

StatusCode DeVeloPhiType::updatePhiCache()
{
  for (unsigned int strip=0; strip<m_numberOfStrips; ++ strip) {
    std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint>  limits = localStripLimits(strip);
    double x = (limits.first.x()  + limits.second.x())/2.0;
    double y = (limits.first.y()  + limits.second.y())/2.0;
    
    Gaudi::XYZPoint lp(x,y,0.0);
    m_idealPhi[strip]=localPhiToGlobal(lp.phi());
    
    Gaudi::XYZPoint gp = localToGlobal(lp);
    m_globalPhi[strip] = gp.phi();
    
    Gaudi::XYZPoint hbp = localToVeloHalfBox(lp);
    m_halfboxPhi[strip] = hbp.phi();
  }
  
  return StatusCode::SUCCESS;
}

StatusCode DeVeloPhiType::updateGeometryCache()
{
  MsgStream msg(msgSvc(), "DeVeloPhiType");
  
  StatusCode sc = updatePhiCache();
  if(!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to update phi cache." << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}
