// $Id: DeVeloRType.cpp,v 1.49 2008-04-14 15:42:21 krinnert Exp $
//==============================================================================
#define VELODET_DEVELORTYPE_CPP 1
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
#include "Kernel/CircleTraj.h"

#include "gsl/gsl_math.h"

// From Velo
#include "VeloDet/DeVeloRType.h"
#include "Kernel/VeloChannelID.h"

namespace VeloDet {
/** This function simply provides access to a local static
 *  data which is used to initialize references in each instance
 *  of DeVeloRType.
 *  The purpose of this function is to work around
 *  a Microsoft(tm) specific extension in VC++ that makes
 *  awkward to have static data mebers accessed by inline
 *  funtions.
 *
 *  @see DeVeloRType
 */
  static std::vector<double>& deVeloRTypeStaticRStrips()
  {
    static std::vector<double> s_rStrips;
    return s_rStrips;
  }

/** This function simply provides access to a local static
 *  data which is used to initialize references in each instance
 *  of DeVeloRType.
 *  The purpose of this function is to work around
 *  a Microsoft(tm) specific extension in VC++ that makes
 *  awkward to have static data mebers accessed by inline
 *  funtions.
 *
 *  @see DeVeloRType
 */
  static std::vector<double>& deVeloRTypeStaticRPitch()
  {
    static std::vector<double> s_rPitch;
    return s_rPitch;
  }

/** This function simply provides access to a local static
 *  data which is used to initialize references in each instance
 *  of DeVeloRType.
 *  The purpose of this function is to work around
 *  a Microsoft(tm) specific extension in VC++ that makes
 *  awkward to have static data mebers accessed by inline
 *  funtions.
 *
 *  @see DeVeloRType
 */
  static std::vector<double>& deVeloRTypeStaticPhiMin()
  {
    static std::vector<double> s_phiMin;
    return s_phiMin;
  }

/** This function simply provides access to a local static
 *  data which is used to initialize references in each instance
 *  of DeVeloRType.
 *  The purpose of this function is to work around
 *  a Microsoft(tm) specific extension in VC++ that makes
 *  awkward to have static data mebers accessed by inline
 *  funtions.
 *
 *  @see DeVeloRType
 */
  static std::vector<double>& deVeloRTypeStaticPhiMax()
  {
    static std::vector<double> s_phiMax;
    return s_phiMax;
  }

/** This function simply provides access to a local static
 *  data which is used to initialize references in each instance
 *  of DeVeloRType.
 *  The purpose of this function is to work around
 *  a Microsoft(tm) specific extension in VC++ that makes
 *  awkward to have static data mebers accessed by inline
 *  funtions.
 *
 *  @see DeVeloRType
 */
  static std::vector<std::pair<double, double> >& deVeloRTypeStaticStripPhiLimits()
  {
    static std::vector<std::pair<double, double> > s_stripPhiLimits;
    return s_stripPhiLimits;
  }
}

// used to control initialization
bool DeVeloRType::m_staticDataInvalid = true;

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
DeVeloRType::DeVeloRType(const std::string& name) : 
  DeVeloSensor(name),
  m_halfAngle(90.0 * Gaudi::Units::degree),
  m_quarterAngle(.5 * m_halfAngle),
  m_globalR(m_numberOfStrips,0.0),
  m_halfboxR(m_numberOfStrips,0.0),
  m_associatedPhiSensor(0),
  m_otherSideRSensor(0),
  m_otherSidePhiSensor(0),
  m_rStrips(VeloDet::deVeloRTypeStaticRStrips()),
  m_rPitch(VeloDet::deVeloRTypeStaticRPitch()),
  m_phiMin(VeloDet::deVeloRTypeStaticPhiMin()),
  m_phiMax(VeloDet::deVeloRTypeStaticPhiMax()),
  m_stripPhiLimits(VeloDet::deVeloRTypeStaticStripPhiLimits())
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
  if( sc.isSuccess() ) sc = jobSvc->setMyProperties("DeVeloRType", pmgr);
  if( !sc ) return sc;
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
  m_debug   = (msgSvc()->outputLevel("DeVeloRType") == MSG::DEBUG  ) ;
  m_verbose = (msgSvc()->outputLevel("DeVeloRType") == MSG::VERBOSE) ;

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
  
  // geometry conditions, update global r of strip cache
  updMgrSvc()->
    registerCondition(this,this->m_geometry,&DeVeloRType::updateGeometryCache);

  // first update
  sc = updMgrSvc()->update(this);
  if(!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to update geometry cache." << endreq;
    return sc;
  }
  
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
  Gaudi::XYZPoint localPoint = globalToLocal(point);

  // Check boundaries...
  StatusCode sc = isInActiveArea(localPoint);
  if(!sc.isSuccess()) return sc;

  // work out closet channel....
  unsigned int closestStrip=0;
  double logarithm, strip;
  double radius=localPoint.Rho();
  logarithm = (m_pitchSlope*(radius - m_innerR)+m_innerPitch) / 
    m_innerPitch;
  strip = log(logarithm)/m_pitchSlope;
  closestStrip = LHCb::Math::round(strip);
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
  //MsgStream msg(msgSvc(), "DeVeloRType");
  // check boundaries....  
  double radius=point.Rho();
  if(innerRadius() >= radius || outerRadius() <= radius) {
    //msg << MSG::VERBOSE << "Outside active radii " << radius << endreq;
    return StatusCode::FAILURE;
  }
  // Dead region from bias line
  double y=point.y();
  if (m_phiGap > y && -m_phiGap < y) {
    //msg << MSG::VERBOSE << "Inside dead region from bias line " << y << endreq;
    return StatusCode::FAILURE;
  }
  // corner cut-offs
  bool cutOff=isCutOff(point.x(),point.y());
  if(cutOff) {
    /*msg << MSG::VERBOSE << "cut off: x,y " << point.x() << "," << point.y()
      << endreq;    */
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
/// Returns the minimum phi in a zone at given radius
//==============================================================================
double DeVeloRType::phiMinZone(unsigned int zone, double radius) const {
    double phiMin;
    if(0 == zone){
      phiMin = -acos(m_overlapInX/radius);
      double y=radius*sin(phiMin);
      if(isCutOff(m_overlapInX,y)) {
        double x,y;
        intersectCutOff(radius,x,y);
        y = -y;
        phiMin=atan2(y,x);
      }
    } else if(2 == zone){
      phiMin = asin(m_phiGap/radius);
    } else {
      phiMin = phiMinZone(zone);
    }
    return phiMin;
  }
//==============================================================================
/// Returns the maximum phi in a zone at given radius
//==============================================================================
double DeVeloRType::phiMaxZone(unsigned int zone, double radius) const {
  double phiMax;
  if(1 == zone){
    phiMax = asin(-m_phiGap/radius);
  } else if(3 == zone){
    phiMax = acos(m_overlapInX/radius);
    double y=radius*sin(phiMax);
    if(isCutOff(m_overlapInX,y)) {
      double x,y;
      intersectCutOff(radius,x,y);
      phiMax=atan2(y,x);
    }
  } else {
      phiMax = phiMaxZone(zone);
  }
  return phiMax;
}
//==============================================================================
/// Return the length of a strip
//==============================================================================
double DeVeloRType::stripLength(const unsigned int strip) const { 
  double phiMin=m_stripPhiLimits[strip].first;
  double phiMax=m_stripPhiLimits[strip].second;
  if(phiMin < 0) phiMin += 2*Gaudi::Units::pi;
  if(phiMax < 0) phiMax += 2*Gaudi::Units::pi;
  double radius=m_rStrips[strip];
  double length=2*(phiMax-phiMin)*radius;
  return length;
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
  return this->residual(point,channel,0.0,residual,chi2);
}
//==============================================================================
///Residual of 3-d point to a VeloChannelID + interstrip fraction
//==============================================================================
StatusCode DeVeloRType::residual(const Gaudi::XYZPoint& point,
                                   const LHCb::VeloChannelID& channel,
                                   const double interStripFraction,
                                   double &residual,
                                   double &chi2) const

{

  // Perpendicular distance to strip.....

  MsgStream msg(msgSvc(), "DeVeloRType");


  Gaudi::XYZPoint localPoint = DeVeloSensor::globalToLocal(point);
  
  // Check boundaries...
  StatusCode sc = isInActiveArea(localPoint);
  if(!sc.isSuccess()) return sc;
  
  unsigned int strip=channel.strip();
  double offset=interStripFraction * rPitch(strip);
  
  double rPoint = localPoint.Rho();
  double rStrip = rOfStrip(strip);
  residual = rStrip + offset  - rPoint;
  
  double sigma=m_resolution.first*rPitch(strip) - m_resolution.second;
  chi2 = gsl_pow_2(residual/sigma);
  
  if(m_verbose) {
    msg << MSG::VERBOSE << "Residual; rPoint = " << rPoint << " strip " << strip
        << " rStrip = " << rStrip << " offset: " << offset <<  " residual " << residual
        << " sigma = " << sigma
        << " chi2 = " << chi2 << endreq;
  }
  
  

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
  

  // we only have to do this once, strip radii pitches and phi
  // limits are stored in statics, i.e. are technically the same
  // for all instances of DeVeloRType
  if (m_staticDataInvalid) {
    /// Angular limits for zones...
    phiZoneLimits();

    m_rStrips.clear();
    m_rPitch.clear();
    m_stripPhiLimits.clear();
    double radius,pitch;
    for(unsigned int zone=0; zone<m_numberOfZones; ++zone) {
      for(unsigned int istrip=0; istrip<m_stripsInZone; ++istrip){
        radius = (exp(m_pitchSlope*istrip)*m_innerPitch - 
                  (m_innerPitch-m_pitchSlope*m_innerR)) /
          m_pitchSlope;
        pitch = exp(m_pitchSlope*istrip)*m_innerPitch;
        double phiMin=phiMinZone(zone,radius);
        double phiMax=phiMaxZone(zone,radius);
        double x1 = radius*cos(phiMin);
        double y1 = radius*sin(phiMin);
        double x2 = radius*cos(phiMax);
        double y2 = radius*sin(phiMax);

        m_rStrips.push_back(radius);
        m_rPitch.push_back(pitch);
        m_stripPhiLimits.push_back(std::pair<double,double>(phiMin,phiMax));

        // intialize base class members
        Gaudi::XYZPoint begin(x1,y1,0.);
        Gaudi::XYZPoint end(x2,y2,0.);
        m_stripLimits.push_back(std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint>(begin,end));
      }
    }
    m_staticDataInvalid = false;  // these are valid now for all instances
  } else { // statics are valid, initialize base class member only

    std::vector<double>::const_iterator ri = m_rStrips.begin();
    std::vector< std::pair<double,double> >::const_iterator spli = m_stripPhiLimits.begin();
    for ( ;
          spli !=  m_stripPhiLimits.end() && ri != m_rStrips.end();
          ++spli, ++ri) {
      double phiMin = spli->first;
      double phiMax = spli->second;
      double radius = *ri;
      double x1 = radius*cos(phiMin);
      double y1 = radius*sin(phiMin);
      double x2 = radius*cos(phiMax);
      double y2 = radius*sin(phiMax);
      Gaudi::XYZPoint begin(x1,y1,0.);
      Gaudi::XYZPoint end(x2,y2,0.);
      m_stripLimits.push_back(std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint>(begin,end));
    }
  }

  if(m_debug) {
    for(unsigned int i=0; i < m_phiMin.size(); ++i){
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
}
//==============================================================================
/// Store line defining corner cut-offs
//==============================================================================
void DeVeloRType::cornerLimits()
{
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
  if(m_debug) {
    MsgStream msg( msgSvc(), "DeVeloRType" );
    msg << MSG::DEBUG << "Cut off starts at x=" << m_cornerX1 << ",y=" << m_cornerY1
        << " and ends at x=" << m_cornerX2 << ",y=" << m_cornerY2 
        << " gradient=" << m_gradCutOff << " intercept=" << m_intCutOff << endreq;
  }
}
//==============================================================================
// For a given radius, calculate point where circle crosses corner cut offs
//==============================================================================
void DeVeloRType::intersectCutOff(const double radius, double& x, double& y) const {
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
    if(m_verbose) {
      MsgStream msg( msgSvc(), "DeVeloRType" );
      msg << MSG::VERBOSE << "a=" << QuadA << ",b=" << QuadB << ",c=" << QuadC 
          << " Solution 1: x=" << x1 << " y=" << y1
          << " Solution 2: x=" << x2 << " y=" << y2
          << " Chose: x=" << x << " y=" << y
          << endreq;
    }
  } else {
    MsgStream msg( msgSvc(), "DeVeloRType" );
    msg << MSG::ERROR << "Unable to calculate corner intersect at r = " << r << endreq;
  }
}
//==============================================================================
/// Store the angular limits of zones sensor at +ve x
//==============================================================================
void DeVeloRType::phiZoneLimits()
{
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
  for(unsigned int routLine=m_minRoutingLine;routLine<=m_maxRoutingLine/2;++routLine){
    unsigned int routArea=RoutingLineArea(routLine);
    unsigned int strip=RoutLineToStrip(routLine,routArea);
    // Sector 1
    m_mapStripToRoutingLine[strip]=routLine;
    m_mapRoutingLineToStrip[routLine]=strip;
    // sector 3
    m_mapStripToRoutingLine[strip+1024]=routLine+1024;
    m_mapRoutingLineToStrip[routLine+1024]=strip+1024;
    if(m_verbose) {
      MsgStream msg( msgSvc(), "DeVeloRType" );
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
    Gaudi::XYZPoint lBegin(radius*cos(phiMin),radius*sin(phiMin),z);
    Gaudi::XYZPoint lEnd(radius*cos(phiMax),radius*sin(phiMax),z);

    // move to global frame
    Gaudi::XYZPoint gOrigin = localToGlobal(lOrigin);
    Gaudi::XYZPoint gBegin = localToGlobal(lBegin);
    Gaudi::XYZPoint gEnd = localToGlobal(lEnd);
    /* Covert phi range to 0-360 to make sure trajectories run in right direction
       and protect against crossing boundaries */
    double phiBeginTmp=gBegin.phi();
    if(phiBeginTmp < 0) phiBeginTmp += 2*Gaudi::Units::pi;
    double phiEndTmp=gEnd.phi();
    if(phiEndTmp < 0) phiEndTmp += 2*Gaudi::Units::pi;
    if(phiBeginTmp > phiEndTmp){
      Gaudi::XYZPoint gTmp=gBegin;
      gBegin=gEnd;
      gEnd=gTmp;
    }

    // put into trajectory
    LHCb::Trajectory* tTraj = new LHCb::CircleTraj(gOrigin,gBegin-gOrigin,gEnd-gOrigin,radius);

    std::auto_ptr<LHCb::Trajectory> autoTraj(tTraj);
    
    return autoTraj;  

}

StatusCode DeVeloRType::updateStripRCache()
{
  // We do not use the radius at the centre of the strip, because in
  // the global frame this is no longer the occupancy weighted mean.
  // Instead we integrate over the strip wighted by occupancy which is
  // o = const./r.  The constant cancels in the weighted mean.
  for (unsigned int strip=0; strip<m_numberOfStrips; ++strip) {
    double phiMin = phiMaxStrip(strip) > phiMinStrip(strip) ? phiMinStrip(strip) : phiMaxStrip(strip);
    double phiMax = phiMaxStrip(strip) > phiMinStrip(strip) ? phiMaxStrip(strip) : phiMinStrip(strip);
    double rLocal =  rOfStrip(strip);

    double dphi   = (phiMax-phiMin)/10.0;
    double num   = 0.0;
    double hbden = 0.0;
    double gden  = 0.0;
    double phiLocal=phiMin;

    // integrate over strip
    for ( ; phiLocal < phiMax; phiLocal += dphi) {
      Gaudi::XYZPoint lp(rLocal*cos(phiLocal),rLocal*sin(phiLocal),0.0);
      
      num += dphi;
      
      Gaudi::XYZPoint hbp = localToVeloHalfBox(lp);
      hbden += dphi/hbp.rho();
      
      Gaudi::XYZPoint gp = localToGlobal(lp);
      gden += dphi/gp.rho();
    }

    // deal with the last interval, it might be shorter than the original dphi
    dphi = phiMax - phiLocal + dphi;
    num += dphi;
    
    Gaudi::XYZPoint lp(rLocal*cos(phiMax),rLocal*sin(phiMax),0.0);
    
    Gaudi::XYZPoint hbp = localToVeloHalfBox(lp);
    hbden += dphi/hbp.rho();
    
    Gaudi::XYZPoint gp = localToGlobal(lp);
    gden += dphi/gp.rho();

    // store the results
    m_halfboxR[strip] = num/hbden;
    m_globalR [strip] = num/gden;

  }
  
  return StatusCode::SUCCESS;
}

StatusCode DeVeloRType::updateZoneLimits()
{
  for (unsigned int localZone=0; localZone<4; ++localZone) {
    unsigned int minStrip = localZone*512;
    unsigned int maxStrip = minStrip+511;
    unsigned int midStrip = (minStrip+maxStrip)/2;
    unsigned int zone     = (isDownstream() ? 3-localZone : localZone);

    // determine the phi ranges of the zones in global frame
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMin = globalStripLimits(minStrip);
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMax = globalStripLimits(maxStrip);
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMid = globalStripLimits(midStrip);
    std::vector<double> phiLimits;
    phiLimits.push_back(globalLimitsMin.first.phi()); phiLimits.push_back(globalLimitsMin.second.phi());
    phiLimits.push_back(globalLimitsMax.first.phi()); phiLimits.push_back(globalLimitsMax.second.phi());
    phiLimits.push_back(globalLimitsMid.first.phi()); phiLimits.push_back(globalLimitsMid.second.phi());
    // map to [0,2pi] for right hand side sensors
    if (isRight()) {
      for (unsigned int i=0; i<phiLimits.size(); ++i) {
        if (phiLimits[i]<0) phiLimits[i] += 2.0*Gaudi::Units::pi;
      }
    }
    m_globalPhiLimitsZone[zone].first  = *std::min_element(phiLimits.begin(),phiLimits.end());
    m_globalPhiLimitsZone[zone].second = *std::max_element(phiLimits.begin(),phiLimits.end());
    // map back to [-pi,pi]
    if (isRight()) {
      if (m_globalPhiLimitsZone[zone].first  > Gaudi::Units::pi) m_globalPhiLimitsZone[zone].first  -= 2.0*Gaudi::Units::pi;   
      if (m_globalPhiLimitsZone[zone].second > Gaudi::Units::pi) m_globalPhiLimitsZone[zone].second -= 2.0*Gaudi::Units::pi;   
    } 
    
    // determine the phi ranges of the zones in VELO half box frame
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> halfBoxLimitsMin
      (globalToVeloHalfBox(globalLimitsMin.first),globalToVeloHalfBox(globalLimitsMin.second));
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> halfBoxLimitsMax
      (globalToVeloHalfBox(globalLimitsMax.first),globalToVeloHalfBox(globalLimitsMax.second));
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> halfBoxLimitsMid
      (globalToVeloHalfBox(globalLimitsMid.first),globalToVeloHalfBox(globalLimitsMid.second));
    phiLimits.clear();
    phiLimits.push_back(halfBoxLimitsMin.first.phi()); phiLimits.push_back(halfBoxLimitsMin.second.phi());
    phiLimits.push_back(halfBoxLimitsMax.first.phi()); phiLimits.push_back(halfBoxLimitsMax.second.phi());
    phiLimits.push_back(halfBoxLimitsMid.first.phi()); phiLimits.push_back(halfBoxLimitsMid.second.phi());
    // map to [0,2pi] for right hand side sensors
    if (isRight()) {
      for (unsigned int i=0; i<phiLimits.size(); ++i) {
        if (phiLimits[i]<0) phiLimits[i] += 2.0*Gaudi::Units::pi;
      }
    }
    m_halfboxPhiLimitsZone[zone].first  = *std::min_element(phiLimits.begin(),phiLimits.end());
    m_halfboxPhiLimitsZone[zone].second = *std::max_element(phiLimits.begin(),phiLimits.end());
    // map back to [-pi,pi]
    if (isRight()) {
      if (m_halfboxPhiLimitsZone[zone].first  > Gaudi::Units::pi) m_halfboxPhiLimitsZone[zone].first  -= 2.0*Gaudi::Units::pi;   
      if (m_halfboxPhiLimitsZone[zone].second > Gaudi::Units::pi) m_halfboxPhiLimitsZone[zone].second -= 2.0*Gaudi::Units::pi;   
    } 

    // r limits are the radii of the outer strip + local pitch/2 and the innder strip - local pitch/2
    m_globalRLimitsZone [zone].first  = globalROfStrip(minStrip)  - rPitch(minStrip)/2.0;
    m_globalRLimitsZone [zone].second = globalROfStrip(maxStrip)  + rPitch(maxStrip)/2.0;
    m_halfboxRLimitsZone[zone].first  = halfboxROfStrip(minStrip) - rPitch(minStrip)/2.0;
    m_halfboxRLimitsZone[zone].second = halfboxROfStrip(maxStrip) + rPitch(maxStrip)/2.0;
  }

  return StatusCode::SUCCESS;
}

StatusCode DeVeloRType::updateGeometryCache()
{
  MsgStream msg(msgSvc(), "DeVeloRType");
  
  StatusCode sc = updateStripRCache();
  if(!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to update strip r cache." << endreq;
    return sc;
  }

  sc = updateZoneLimits();
  if(!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to update zone limit cache." << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}
