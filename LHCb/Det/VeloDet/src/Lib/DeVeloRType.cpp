//==============================================================================
#define VELODET_DEVELORTYPE_CPP 1
//==============================================================================
// Include files
#include <cmath>

#include "vdt/asin.h"
#include "vdt/cos.h"
#include "vdt/sin.h"
#include "vdt/sincos.h"

// From Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// From LHCb
#include "LHCbMath/LHCbMath.h"
#include "Kernel/CircleTraj.h"

// From Velo
#include "VeloDet/DeVeloRType.h"
#include "Kernel/VeloChannelID.h"

namespace {

    // simplified implementation of Library TS2's std::make_array... @FIXME: remove once std::make_array is available
    template <typename... Args>
    auto make_array( Args&&... a ) -> std::array<typename std::common_type<Args...>::type, sizeof...(a)>
    {
            return { std::forward<Args>(a)... };
    }


}

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
  static std::vector<std::pair<double, double>>& deVeloRTypeStaticStripPhiLimits()
  {
    static std::vector<std::pair<double, double>> s_stripPhiLimits;
    return s_stripPhiLimits;
  }

  /** This function simply provides access to a local static
   *  data which is used to initialize references in each instance
   *  of DeVeloRType.
   *  @see DeVeloRType
   */
  static std::vector<DeVeloRType::PolyLine>& deVeloRTypeStaticM2RoutingLines()
  {
    static std::vector<DeVeloRType::PolyLine> s_M2RoutingLines;
    return s_M2RoutingLines;
  }

  /** This function simply provides access to a local static
   *  data which is used to initialize references in each instance
   *  of DeVeloRType.
   *  @see DeVeloRType
   */
  static std::vector<std::pair<double,unsigned int>>& deVeloRTypeStaticM2RLMinPhi()
  {
    static std::vector<std::pair<double,unsigned int>> s_M2RLMinPhi;
    return s_M2RLMinPhi;
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
  m_stripPhiLimits(VeloDet::deVeloRTypeStaticStripPhiLimits()),
  m_M2RoutingLines(VeloDet::deVeloRTypeStaticM2RoutingLines()),
  m_M2RLMinPhi(VeloDet::deVeloRTypeStaticM2RLMinPhi()),
  m_msgStream(NULL)
{
}
//==============================================================================
/// Destructor
//==============================================================================
DeVeloRType::~DeVeloRType()
{
  delete m_msgStream;
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
  { // Trick from old DeVelo to set the output level
    std::unique_ptr<PropertyMgr> pmgr{  new PropertyMgr() };
    int outputLevel=0;
    pmgr->declareProperty("OutputLevel", outputLevel);
    auto jobSvc = Gaudi::svcLocator()->service<IJobOptionsSvc>("JobOptionsSvc");
    if( jobSvc ) jobSvc->setMyProperties("DeVeloRType", pmgr.get()).ignore();
    if ( 0 < outputLevel ) {
      msgSvc()->setOutputLevel("DeVeloRType", outputLevel);
    }
  }

  auto sc = DeVeloSensor::initialize();
  if(!sc.isSuccess()) {
    msg() << MSG::ERROR << "Failed to initialise DeVeloSensor" << endmsg;
    return sc;
  }
  m_debug   = (msgSvc()->outputLevel("DeVeloRType") == MSG::DEBUG  ) ;
  m_verbose = (msgSvc()->outputLevel("DeVeloRType") == MSG::VERBOSE) ;
  if(m_verbose) m_debug = true;

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
    msg() << MSG::ERROR << "Failed to update geometry cache." << endmsg;
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
  Gaudi::XYZPoint localPoint = globalToLocal(point);

  // Check boundaries...
  StatusCode sc = isInActiveArea(localPoint);
  if(!sc.isSuccess()) return sc;

  // work out closet channel....
  double radius=localPoint.Rho();
  double logarithm = (m_pitchSlope*(radius - m_innerR)+m_innerPitch) /
    m_innerPitch;
  double strip = vdt::fast_log(logarithm)/m_pitchSlope;
  unsigned int closestStrip = LHCb::Math::round(strip);
  fraction = strip - closestStrip;

  // Which zone is the strip in?
  double phi=vdt::fast_atan2(localPoint.y(), localPoint.x());
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
  // check boundaries....
  double radius=point.Rho();
  if(innerRadius() >= radius || outerRadius() <= radius) {
    return StatusCode::FAILURE;
  }
  // Dead region from bias line
  double y=point.y();
  if (m_phiGap > y && -m_phiGap < y) {
    return StatusCode::FAILURE;
  }
  // corner cut-offs
  bool cutOff=isCutOff(point.x(),point.y());
  if(cutOff) return StatusCode::FAILURE;
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
    double yMax=m_gradCutOff*x+m_intCutOff;
    if(yMax > std::abs(y)) return true;
  }
  return false;
}
//==============================================================================
/// Returns the minimum phi in a zone at given radius
//==============================================================================
double DeVeloRType::phiMinZone(unsigned int zone, double radius) const {
  double phiMin(0);
  if(0 == zone){
    phiMin = -vdt::fast_acos(m_overlapInX/radius);
    double y=radius*vdt::fast_sin(phiMin);
    if(isCutOff(m_overlapInX,y)) {
      double x,yy;
      intersectCutOff(radius,x,yy);
      yy = -yy;
      phiMin=vdt::fast_atan2(yy,x);
    }
  } else if(2 == zone){
    phiMin = vdt::fast_asin(m_phiGap/radius);
  } else {
    phiMin = phiMinZone(zone);
  }
  return phiMin;
}
//==============================================================================
/// Returns the maximum phi in a zone at given radius
//==============================================================================
double DeVeloRType::phiMaxZone(unsigned int zone, double radius) const {
  double phiMax(0);
  if(1 == zone){
    phiMax = vdt::fast_asin(-m_phiGap/radius);
  } else if(3 == zone){
    phiMax = vdt::fast_acos(m_overlapInX/radius);
    double y=radius*vdt::fast_sin(phiMax);
    if(isCutOff(m_overlapInX,y)) {
      double x,yy;
      intersectCutOff(radius,x,yy);
      phiMax=vdt::fast_atan2(yy,x);
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
  return 2*(phiMax-phiMin)*radius;
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
  if(startZone != endZone) return StatusCode::FAILURE;
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
  chi2 = (residual / sigma) * (residual / sigma);

  if(m_verbose) {
    msg() << MSG::VERBOSE << "Residual; rPoint = " << rPoint << " strip " << strip
          << " rStrip = " << rStrip << " offset: " << offset <<  " residual " << residual
          << " sigma = " << sigma
          << " chi2 = " << chi2 << endmsg;
  }
  return StatusCode::SUCCESS;
}

//==============================================================================
/// Store the local radius and phi limits for each strip in the sensor
//==============================================================================
void DeVeloRType::calcStripLimits()
{

  if(m_verbose) msg() << MSG::VERBOSE << "calcStripLimits" << endmsg;
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
        radius = (vdt::fast_exp(m_pitchSlope*istrip)*m_innerPitch -
                  (m_innerPitch-m_pitchSlope*m_innerR)) /
          m_pitchSlope;
        pitch = vdt::fast_exp(m_pitchSlope*istrip)*m_innerPitch;
        double phiMin=phiMinZone(zone,radius);
        double phiMax=phiMaxZone(zone,radius);
	double sin(0), cos(0);
	vdt::fast_sincos(phiMin, sin, cos);
        double x1 = radius*cos;
        double y1 = radius*sin;
	vdt::fast_sincos(phiMax, sin, cos);
        double x2 = radius*cos;
        double y2 = radius*sin;

        m_rStrips.push_back(radius);
        m_rPitch.push_back(pitch);
        m_stripPhiLimits.emplace_back(phiMin,phiMax);

        // intialize base class members
        Gaudi::XYZPoint begin(x1,y1,0.);
        Gaudi::XYZPoint end(x2,y2,0.);
        m_stripLimits.emplace_back(begin,end);
      }
    }
    // load Metal 2 routing line map for intra-sensor cross talk simulation
    loadM2RoutingLines();

    m_staticDataInvalid = false;  // these are valid now for all instances
  } else { // statics are valid, initialize base class member only

    auto ri = m_rStrips.begin();
    for ( auto spli = m_stripPhiLimits.begin();
          spli !=  m_stripPhiLimits.end() && ri != m_rStrips.end();
          ++spli, ++ri) {
      double phiMin = spli->first;
      double phiMax = spli->second;
      double radius = *ri;
      double sin(0), cos(0);
      vdt::fast_sincos(phiMin, sin, cos);
      double x1 = radius*cos;
      double y1 = radius*sin;
      vdt::fast_sincos(phiMax, sin, cos);
      double x2 = radius*cos;
      double y2 = radius*sin;
      Gaudi::XYZPoint begin(x1,y1,0.);
      Gaudi::XYZPoint end(x2,y2,0.);
      m_stripLimits.emplace_back(begin,end);
    }
  }

  if(m_debug) {
    for(unsigned int i=0; i < m_phiMin.size(); ++i){
      msg() << MSG::DEBUG << "Zone limits; zone " << i << " min " << m_phiMin[i]
            << " max " << m_phiMax[i] << " phiMin "
            << phiMinZone(i,innerRadius())
            << " max " << phiMaxZone(i,innerRadius()) << endmsg;
    }
    msg() << MSG::DEBUG << "Radius of first strip is " << m_rStrips[0]
          << " last strip " << m_rStrips[m_rStrips.size()-1] << endmsg;
    msg() << MSG::DEBUG << "Pitch; inner " << m_rPitch[0] << " outer "
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
    msg() << MSG::DEBUG << "Cut off starts at x=" << m_cornerX1 << ",y=" << m_cornerY1
          << " and ends at x=" << m_cornerX2 << ",y=" << m_cornerY2
          << " gradient=" << m_gradCutOff << " intercept=" << m_intCutOff << endmsg;
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
    double x1 = ( -QuadB+std::sqrt(B2Minus4AC) ) / (2*QuadA);
    double y1 = m*x1+c;
    double x2 =  ( -QuadB-std::sqrt(B2Minus4AC) ) / (2*QuadA);
    double y2 = m*x2+c;
    if(x1 < x2) {
      x=x1;
      y=y1;
    } else {
      x=x2;
      y=y2;
    }
    if(m_verbose) {
      msg() << MSG::VERBOSE << "a=" << QuadA << ",b=" << QuadB << ",c=" << QuadC
            << " Solution 1: x=" << x1 << " y=" << y1
            << " Solution 2: x=" << x2 << " y=" << y2
            << " Chose: x=" << x << " y=" << y
            << endmsg;
    }
  } else {
    msg() << MSG::ERROR << "Unable to calculate corner intersect at r = " << r << endmsg;
  }
}
//==============================================================================
/// Store the angular limits of zones sensor at +ve x
//==============================================================================
void DeVeloRType::phiZoneLimits()
{
  auto phi = vdt::fast_acos(m_overlapInX/outerRadius());
  m_phiMin = { -phi, -m_quarterAngle, 0, m_quarterAngle };
  m_phiMax = { -m_quarterAngle, 0, m_quarterAngle, phi };
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
      msg() << MSG::VERBOSE << "Routing line " << routLine
            << " area " << routArea
            << " strip " << m_mapRoutingLineToStrip[routLine]
            << " +1024 line " << routLine+1024
            << " +1024 strip " << m_mapRoutingLineToStrip[routLine+1024]
            << endmsg;
      msg() << MSG::VERBOSE << "Routing line " << routLine
            << " strip " << RoutingLineToStrip(routLine)
            << " chip channel " << RoutingLineToChipChannel(routLine)
            << " and back " << ChipChannelToRoutingLine(RoutingLineToChipChannel(routLine))
            << " from strip " << endmsg;
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
  if(routingLine <= m_nChan0) return 0;
  if(routingLine <= m_nChan0+m_nChan1) return 1;
  if(routingLine <= m_nChan0+m_nChan1+m_nChan2) return 2;
  if(routingLine <= m_nChan0+m_nChan1+m_nChan2+m_nChan3) return 3;
  return 999;
}
//=============================================================================
// Return strip from routing line area (first 1024 strips only)
//=============================================================================
unsigned int DeVeloRType::RoutLineToStrip(unsigned int routLine, unsigned int routArea){
  unsigned int strip(0);
  if(0 == routArea){
    strip = (m_nChan1+routLine-1);
  } else if(1 == routArea) {
    strip = m_nChan0+m_nChan1-routLine;
  } else if(2 == routArea){
    strip = routLine-1;
  } else if(3 == routArea){
    strip = (m_maxRoutingLine-m_nChan3-routLine);
  } else strip=9999;
  return ScrambleStrip(strip);
}
//==============================================================================
// Return a trajectory (for track fit) from strip + offset
//==============================================================================
std::unique_ptr<LHCb::Trajectory> DeVeloRType::trajectory(const LHCb::VeloChannelID& id,
                                                          double offset) const {
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
  double sin(0), cos(0);
  vdt::fast_sincos(phiMin, sin, cos);
  Gaudi::XYZPoint lBegin(radius*cos,radius*sin,z);
  vdt::fast_sincos(phiMax, sin, cos);
  Gaudi::XYZPoint lEnd(radius*cos,radius*sin,z);

  // move to global frame
  Gaudi::XYZPoint gOrigin = localToGlobal(lOrigin);
  Gaudi::XYZPoint gBegin = localToGlobal(lBegin);
  Gaudi::XYZPoint gEnd = localToGlobal(lEnd);
  // Convert phi range to [0,2pi] on the right (C) side only 
  // to make sure trajectories run in right direction
  // and protect against crossing the -pi/pi boundary 
  double phiBeginTmp=vdt::fast_atan2(gBegin.y(), gBegin.x());
  if( isRight() && phiBeginTmp < 0) phiBeginTmp += 2*Gaudi::Units::pi;
  double phiEndTmp=vdt::fast_atan2(gEnd.y(), gEnd.x());
  if( isRight() && phiEndTmp < 0) phiEndTmp += 2*Gaudi::Units::pi;
  if(phiBeginTmp > phiEndTmp){
    Gaudi::XYZPoint gTmp=gBegin;
    gBegin=gEnd;
    gEnd=gTmp;
  }

  // put into trajectory
  return std::unique_ptr<LHCb::Trajectory>{ new LHCb::CircleTraj(gOrigin,gBegin-gOrigin,gEnd-gOrigin,radius) };
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
      double sin(0), cos(0);
      vdt::fast_sincos(phiLocal, sin, cos);
      Gaudi::XYZPoint lp(rLocal*cos,rLocal*sin,0.0);

      num += dphi;
      hbden += dphi/localToVeloHalfBox(lp).rho();
      gden += dphi/localToGlobal(lp).rho();
    }

    // deal with the last interval, it might be shorter than the original dphi
    dphi = phiMax - phiLocal + dphi;

    double sin(0), cos(0);
    vdt::fast_sincos(phiMax, sin, cos);
    Gaudi::XYZPoint lp(rLocal*cos,rLocal*sin,0.0);

    num += dphi;
    hbden += dphi/localToVeloHalfBox(lp).rho();
    gden += dphi/localToGlobal(lp).rho();

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
    auto globalLimitsMin = globalStripLimits(minStrip);
    auto globalLimitsMax = globalStripLimits(maxStrip);
    auto globalLimitsMid = globalStripLimits(midStrip);
    auto phiLimits = make_array( globalLimitsMin.first.phi(), globalLimitsMin.second.phi(),
                                 globalLimitsMax.first.phi(), globalLimitsMax.second.phi(),
                                 globalLimitsMid.first.phi(), globalLimitsMid.second.phi()  );
    // map to [0,2pi] for right hand side sensors
    if (isRight()) {
      for (auto& phiLimit : phiLimits) {
        if (phiLimit<0) phiLimit += 2.0*Gaudi::Units::pi;
      }
    }
    auto minmax = std::minmax_element( phiLimits.begin(), phiLimits.end() );
    m_globalPhiLimitsZone[zone].first  = *minmax.first;
    m_globalPhiLimitsZone[zone].second = *minmax.second;
    // map back to [-pi,pi]
    if (isRight()) {
      if (m_globalPhiLimitsZone[zone].first  > Gaudi::Units::pi) m_globalPhiLimitsZone[zone].first  -= 2.0*Gaudi::Units::pi;
      if (m_globalPhiLimitsZone[zone].second > Gaudi::Units::pi) m_globalPhiLimitsZone[zone].second -= 2.0*Gaudi::Units::pi;
    }

    // determine the phi ranges of the zones in VELO half box frame
    auto halfBoxLimitsMin = std::make_pair(globalToVeloHalfBox(globalLimitsMin.first),
                                           globalToVeloHalfBox(globalLimitsMin.second));
    auto halfBoxLimitsMax = std::make_pair(globalToVeloHalfBox(globalLimitsMax.first),
                                           globalToVeloHalfBox(globalLimitsMax.second));
    auto halfBoxLimitsMid = std::make_pair(globalToVeloHalfBox(globalLimitsMid.first),
                                           globalToVeloHalfBox(globalLimitsMid.second));
    phiLimits = make_array( halfBoxLimitsMin.first.phi(),halfBoxLimitsMin.second.phi(),
                            halfBoxLimitsMax.first.phi(),halfBoxLimitsMax.second.phi(),
                            halfBoxLimitsMid.first.phi(),halfBoxLimitsMid.second.phi() );
    // map to [0,2pi] for right hand side sensors
    if (isRight()) {
      for (auto& phiLimit : phiLimits ) {
        if (phiLimit<0) phiLimit += 2.0*Gaudi::Units::pi;
      }
    }
    minmax = std::minmax_element( phiLimits.begin(), phiLimits.end() );
    m_halfboxPhiLimitsZone[zone].first  = *minmax.first;
    m_halfboxPhiLimitsZone[zone].second = *minmax.second;
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

  StatusCode sc = updateStripRCache();
  if(!sc.isSuccess()) {
    msg() << MSG::ERROR << "Failed to update strip r cache." << endmsg;
    return sc;
  }

  sc = updateZoneLimits();
  if(!sc.isSuccess()) {
    msg() << MSG::ERROR << "Failed to update zone limit cache." << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode DeVeloRType::distToM2Line(const Gaudi::XYZPoint& point, 
                                     LHCb::VeloChannelID &vID, 
                                     double & distToM2,
                                     double & distToStrip) const{
  if( m_M2RoutingLines.size() != m_numberOfStrips ){
    // routing lines not loaded: have to fail
    return StatusCode::FAILURE;
  }
    
  Gaudi::XYZPoint lPoint = globalToLocal(point);
  // Check boundaries...
  StatusCode sc = isInActiveArea(lPoint);
  if(!sc.isSuccess()) return sc; 

  // work out closet channel....
  double radius=lPoint.Rho();
  double logarithm = (m_pitchSlope*(radius - m_innerR)+m_innerPitch) /
    m_innerPitch;
  double strip = vdt::fast_log(logarithm)/m_pitchSlope;
  // no routing lines below strip 0 (+ rounding error fix)
  if( strip < 0. ) return StatusCode::FAILURE; 
  unsigned int closestStrip = LHCb::Math::round(strip);
  // sanity check in case of rounding error
  if( closestStrip > 2047 ) closestStrip = 2047; 
  distToStrip = std::abs(strip - closestStrip)*rPitch(closestStrip);

  bool OKM2 = distToM2Line(lPoint.x(), lPoint.y(), vID, distToM2);
  return OKM2 ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

bool DeVeloRType::distToM2Line(double const & x, double const & y,
			       LHCb::VeloChannelID &vID, double & dist) const{
  double dist2 = 1.e9; 
  unsigned int strip = 1e9;

  // find range of strips covering this phi point
  // a crude optimisation by skipping paths not covering this phi
  double phi = vdt::fast_atan2(y,x);
  unsigned int zone = zoneOfPhi(phi); // no overlap between zones
  // start at begining of zone
  auto iLineMin = m_M2RLMinPhi.begin() + zone*m_stripsInZone;
  // skip all strips with a minPhi > this phi
  auto iLineMax = 
    lower_bound(iLineMin, iLineMin+m_stripsInZone,
                std::make_pair(phi,0u));
  for ( ; iLineMin != iLineMax; ++iLineMin ){
    unsigned int iL = iLineMin->second;
    // as ranges are complicated in phi need to keep going even if 
    // this strip can be ignored
    if( m_M2RoutingLines[iL].m_maxPhi < phi ) continue;
    std::vector<double> const & xP = m_M2RoutingLines[iL].m_x;
    std::vector<double> const & yP = m_M2RoutingLines[iL].m_y;
    for ( unsigned int i = 0 ; i < (xP.size()-1) ; ++i ){
      // solve point of closest approach on line (x1,y1) to (x2,y2) to (x,y)
      // defined as (xp,yp) below
      double x1 = xP[i];
      double x2 = xP[i+1];
      double y1 = yP[i];
      double y2 = yP[i+1];
      double dx = x1-x2;
      double dy = y1-y2;
      double xp = ( x*dx*dx + (x2*(y1-y) + x1*(y-y2))*dy ) / (dx*dx + dy*dy);
      double yp = y1 + (xp-x1)*dy/dx;
      // order of x1<x2 or x2>x1 and for y is not defined so try both
      bool checkLimits = ( ( (x1<xp && xp<x2) || (x2<xp && xp<x1) ) && 
			   ( (y1<yp && yp<y2) || (y2<yp && yp<y1) ) );
      // check (xp,yp) is in the extent of the original line
      if( !checkLimits )continue;
      // use distance^2 to avoid too many sqrt calls
      double lDist2 = (xp-x)*(xp-x) + (yp-y)*(yp-y);
      // cap distance to consider at 200 microns 
      if( lDist2 < dist2 && lDist2 < 0.04 ) { 
        dist2 = lDist2;
        strip = iL; // lines in strip order
      }
    }
  }

  if(strip < 3000) { // check we found a valid routing line match
    // set VeloChannelID....
    vID.setSensor(this->sensorNumber());
    vID.setStrip(strip);
    if(isR()) {
      vID.setType(LHCb::VeloChannelID::RType);
    } else if( isPileUp() ) {
      vID.setType(LHCb::VeloChannelID::PileUpType);
    }
    dist = std::sqrt(dist2);
    return true;
  }
  return false;
}        

void DeVeloRType::loadM2RoutingLines(){
  m_M2RoutingLines.clear();  m_M2RoutingLines.reserve(2048);
  for( unsigned int strip = 0 ; strip < 2048 ; ++strip ){
    std::string param_X = format("Routing-R-X-Strip%04i",strip);
    std::string param_Y = format("Routing-R-Y-Strip%04i",strip);
    if( !exists(param_X) || !exists(param_Y) ){
      msg() << MSG::WARNING 
	    << "VELO R sensor M2 routing lines not in conditions DB cond: " 
            << param_X << " and " << param_Y
            <<endmsg;
      return;
    }
    m_M2RoutingLines.emplace_back(param<std::vector<double>>(param_X),
                                  param<std::vector<double>>(param_Y));
  }
  m_M2RLMinPhi.clear(); m_M2RLMinPhi.reserve(m_M2RoutingLines.size());
  for(auto iLine = m_M2RoutingLines.begin(); iLine != m_M2RoutingLines.end() ; ++iLine ){
    m_M2RLMinPhi.emplace_back(iLine->minPhi(),iLine-m_M2RoutingLines.begin());
  }
  std::sort(m_M2RLMinPhi.begin(),m_M2RLMinPhi.end());
}
