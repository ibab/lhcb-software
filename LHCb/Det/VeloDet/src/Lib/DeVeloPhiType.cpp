//==============================================================================
#define VELODET_DEVELOPHITYPE_CPP 1
//==============================================================================
// Include files
#include <cmath>
#include "vdt/atan2.h"
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
#include "Kernel/LineTraj.h"

// From Velo
#include "VeloDet/DeVeloPhiType.h"
#include "Kernel/VeloChannelID.h"

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
  ,  m_corner1X1(0)
  ,  m_corner1Y1(0)
  ,  m_corner1X2(0)
  ,  m_corner1Y2(0)
  ,  m_corner2X1(0)
  ,  m_corner2Y1(0)
  ,  m_corner2X2(0)
  ,  m_corner2Y2(0)
  ,  m_innerCoverage(0)
  ,  m_outerCoverage(0)
  , m_globalPhi(m_numberOfStrips,0.0)
  , m_halfboxPhi(m_numberOfStrips,0.0)
  , m_idealPhi(m_numberOfStrips,0.0)
  , m_associatedRSensor(0)
  , m_otherSidePhiSensor(0)
  , m_otherSideRSensor(0)
  , m_stripLengths(VeloDet::deVeloPhiTypeStaticStripLengths())
  , m_msgStream(NULL)
{
}
//==============================================================================
/// Destructor
//==============================================================================
DeVeloPhiType::~DeVeloPhiType() { delete m_msgStream; }
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
  if ( 0 < outputLevel ) {
    msgSvc()->setOutputLevel("DeVeloPhiType", outputLevel);
  }
  delete pmgr;
  if( !sc ) return sc;

  sc = DeVeloSensor::initialize();
  if(!sc.isSuccess()) {
    msg() << MSG::ERROR << "Failed to initialise DeVeloSensor" << endmsg;
    return sc;
  }
  m_debug   = (msgSvc()->outputLevel("DeVeloPhiType") == MSG::DEBUG  ) ;
  m_verbose = (msgSvc()->outputLevel("DeVeloPhiType") == MSG::VERBOSE) ;
  if(m_verbose) m_debug = true;

  m_numberOfZones=2;
  m_nbInner = param<int>("NbPhiInner");
  m_stripsInZone.clear();
  m_stripsInZone.push_back(m_nbInner);
  m_stripsInZone.push_back(numberOfStrips()-m_nbInner);
  m_middleRadius = param<double>("PhiBoundRadius"); // PhiBound
  // Point where strips of inner/outer regions cross
  m_phiOrigin = param<double>("PhiOrigin");
  m_phiOrigin -= Gaudi::Units::halfpi;
  /* Inner strips (dist. to origin defined by angle between
     extrapolated strip and phi)*/
  m_innerDistToOrigin = param<double>("InnerDistToOrigin");
  m_innerTilt = vdt::fast_asin(m_innerDistToOrigin/innerRadius());
  m_innerTilt += m_phiOrigin;
  // Outer strips
  m_outerDistToOrigin = param<double>("OuterDistToOrigin");
  m_outerTilt = vdt::fast_asin(m_outerDistToOrigin/m_middleRadius);
  double phiAtBoundary   = m_innerTilt -
    vdt::fast_asin( m_innerDistToOrigin / m_middleRadius );
  m_outerTilt += phiAtBoundary;
  double phi = m_outerTilt - vdt::fast_asin( m_outerDistToOrigin/outerRadius() );
  if(m_debug) msg() << MSG::DEBUG << "Phi (degree) inner "    
		    << m_phiOrigin/Gaudi::Units::degree
		    << " at boundary " << phiAtBoundary/Gaudi::Units::degree
		    << " and outside " << phi/Gaudi::Units::degree
		    << endmsg;

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

  // register geometry conditions, update global r of strip cache
  updMgrSvc()->
    registerCondition(this,this->m_geometry,&DeVeloPhiType::updateGeometryCache);

  // first update
  sc = updMgrSvc()->update(this);
  if(!sc.isSuccess()) {
    msg() << MSG::ERROR << "Failed to update geometry cache." << endmsg;
    return sc;
  }

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
    } else {
      rInner=m_middleRadius+m_rGap/2;
      rOuter=outerRadius();
    }
    double sin(0), cos(0);
    vdt::fast_sincos(phiOfStrip(strip,0.,rInner), sin, cos);
    x1 = rInner * cos;
    y1 = rInner * sin;
    vdt::fast_sincos(phiOfStrip(strip,0.,rOuter), sin, cos);
    x2 = rOuter * cos;
    y2 = rOuter * sin;

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
      msg() << MSG::VERBOSE << "Sensor " << sensorNumber() << " " << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
    }
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
      const auto dx = iStLi->first.x() - iStLi->second.x();
      const auto dy = iStLi->first.y() - iStLi->second.y();
      const auto dz = iStLi->first.z() - iStLi->second.z();
      m_stripLengths.push_back(std::sqrt(dx * dx + dy * dy + dz * dz));
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
  double phi=vdt::fast_atan2(localPoint.y(), localPoint.x());

  // Calculate nearest channel....
  unsigned int closestStrip(0);
  double strip=0;
  phi -= phiOffset(radius);
  if(m_middleRadius > radius) {
    strip = phi / m_innerPitch;
  } else {
    strip = phi / m_outerPitch;
    strip += m_nbInner;
  }
  closestStrip = LHCb::Math::round(strip);
  fraction = strip - closestStrip;

  pitch = phiPitch(radius);
  unsigned int sensor=sensorNumber();

  // set VeloChannelID....
  channel.setSensor(sensor);
  channel.setStrip(closestStrip);
  channel.setType(LHCb::VeloChannelID::PhiType);

  if (UNLIKELY(m_verbose)) {
    msg() << MSG::VERBOSE << "pointToChannel; local phi " << localPoint.phi()/Gaudi::Units::degree
          << " radius " << localPoint.Rho()
          << " phiOffset " << phiOffset(radius)/Gaudi::Units::degree
          << " phi corrected " << phi/Gaudi::Units::degree << endmsg;
    msg() << MSG::VERBOSE << " strip " << strip << " closest strip " << closestStrip
          << " fraction " << fraction <<endmsg;
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
  if(m_verbose) msg() << MSG::VERBOSE << "isInActiveArea: x=" << point.x() 
		      << ",y=" << point.y() << endmsg;
  //  check boundaries....
  double radius=point.Rho();
  if(innerRadius() >= radius || outerRadius() <= radius) {
    return StatusCode::FAILURE;
  }
  bool isInner=true;
  if(m_middleRadius < radius) {
    isInner=false;
  }
  // Dead region
  if(m_middleRadius+(m_rGap/2.) > radius && 
     m_middleRadius-(m_rGap/2.) < radius){
    return StatusCode::FAILURE;
  }

  // Corner cut-offs
  //  if(m_isDownstream) y = -y;
  double x=point.x();
  double y=point.y();
  bool cutOff=isCutOff(x,y);
  if(cutOff) {
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
  double phi=vdt::fast_atan2(y,x);
  double phiStrip=phiOfStrip(endStrip,0.0,radius);
  //  double pitch=phiPitch(radius);
  if(0 > y) {
    if(phiStrip > phi) return StatusCode::FAILURE;
  } else {
    if(phiStrip < phi) return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Is the local point in the corner cut-off?
//==============================================================================
bool DeVeloPhiType::isCutOff(double x, double y) const
{
  double radius=std::sqrt(x*x+y*y);
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
  xNear /= (1+gradient * gradient);

  double yNear = gradient*xNear + intercept;

  residual = std::sqrt((xNear - x) * (xNear - x) + (yNear - y) * (yNear - y));

  // Work out how to calculate the sign!
  Gaudi::XYZPoint localNear(xNear,yNear,0.0);
  Gaudi::XYZPoint globalNear = DeVeloSensor::localToGlobal(localNear);
  {
    const auto irl = 1 / std::hypot(xNear, yNear);
    const auto irg = 1 / std::hypot(globalNear.x(), globalNear.y());
    const auto cdphi = irl * irg * (xNear * globalNear.x() + yNear * globalNear.y());
    const auto sdphi = irl * irg * (yNear * globalNear.x() - xNear * globalNear.y());
    if (vdt::fast_atan2(sdphi, cdphi) < 0) residual = -residual;
  }

  double radius = localPoint.Rho();
  double sigma = m_resolution.first*phiPitch(radius) - m_resolution.second;
  chi2 = (residual / sigma) * (residual / sigma);

  if (UNLIKELY(m_verbose)) {
    msg() << MSG::VERBOSE << "Residual; sensor " << channel.sensor()
	  << " strip " << strip
	  << " x " << x << " y " << y << endmsg;
    msg() << MSG::VERBOSE << " xNear " << xNear << " yNear " << yNear
	  << " residual " << residual << " sigma = " << sigma
	  << " chi2 = " << chi2 << endmsg;
  }
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
  if(m_debug){
    msg() << MSG::DEBUG << "Building routing line map for sensor "
	  << (this->sensorNumber()) << endmsg;
  }
  for(unsigned int routLine=m_minRoutingLine;routLine<=m_maxRoutingLine;++routLine,++count){
    if(0 == count%6){
      if(m_verbose) msg() << MSG::VERBOSE << "Pattern of six ---------------------------------------\n";
    }
    strip=this->strip(routLine);

    m_mapStripToRoutingLine[strip]=routLine;
    m_mapRoutingLineToStrip[routLine]=strip;

    if(m_verbose) msg() << MSG::VERBOSE << "Routing line " << routLine
		      << " Patttern element " << (patternElement(routLine))
		      << " number " << (patternNumber(routLine))
		      << " strip " << strip
		      << endmsg;
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
  for (unsigned int zone=0; zone<m_numberOfZones; ++zone) {

    unsigned int firstStrip = m_nbInner*zone;
    std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint>  limits = localStripLimits(firstStrip);
    double r0 = (limits.first.rho()+limits.second.rho())/2.0;

    double d0 = zone ? m_outerDistToOrigin : m_innerDistToOrigin;
    d0 = isDownstream() ? d0 : -d0;
    m_idealDistToOrigin[zone] = d0;
    double offset = vdt::fast_asin(d0/r0);
    m_idealOffsetAtR0[zone] = offset;

    Gaudi::XYZPoint begin = localToGlobal(limits.first);
    Gaudi::XYZPoint end   = localToGlobal(limits.second);
    r0 = (begin.rho()+end.rho())/2.0;
    Gaudi::XYZVector dx = end-begin;
    Gaudi::XYZPoint center = begin + 0.5*dx;
    d0 = r0*vdt::fast_sin(center.phi()-dx.phi());
    m_globalDistToOrigin[zone] = d0;
    m_globalOffsetAtR0[zone] = vdt::fast_asin(d0/r0);

    begin = localToVeloHalfBox(limits.first);
    end   = localToVeloHalfBox(limits.second);
    r0 = (begin.rho()+end.rho())/2.0;
    dx = end-begin;
    center = begin + 0.5*dx;
    d0 = r0*vdt::fast_sin(center.phi()-dx.phi());
    m_halfboxDistToOrigin[zone] = d0;
    m_halfboxOffsetAtR0[zone] = vdt::fast_asin(d0/r0);

    for (unsigned int s=0; s<m_stripsInZone[zone]; ++s) {

      unsigned int strip = s + m_nbInner*zone;
      double sin(0), cos(0);
      vdt::fast_sincos(phiOfStrip(strip,0.0,r0), sin, cos);
      double x = r0*cos;
      double y = r0*sin;

      Gaudi::XYZPoint lp(x,y,0.0);
      m_idealPhi[strip] = localPhiToGlobal(lp.phi());

      Gaudi::XYZPoint gp = localToGlobal(lp);
      m_globalPhi[strip] = gp.phi();

      Gaudi::XYZPoint hbp = localToVeloHalfBox(lp);
      m_halfboxPhi[strip] = hbp.phi();
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode DeVeloPhiType::updateZoneLimits()
{
  for (unsigned int zone=0; zone<2; ++zone) {
    unsigned int minStrip = (zone ? m_nbInner : 0 );
    unsigned int maxStrip = (zone ? m_numberOfStrips-1 : m_nbInner-1 );
    unsigned int midStrip = (minStrip+maxStrip)/2;

    // determine the r ranges of the zones in global frame
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMin = globalStripLimits(minStrip);
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMax = globalStripLimits(maxStrip);
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMid = globalStripLimits(midStrip);
    std::vector<double> rLimits;
    rLimits.push_back(globalLimitsMin.first.rho()); rLimits.push_back(globalLimitsMin.second.rho());
    rLimits.push_back(globalLimitsMax.first.rho()); rLimits.push_back(globalLimitsMax.second.rho());
    rLimits.push_back(globalLimitsMid.first.rho()); rLimits.push_back(globalLimitsMid.second.rho());
    m_globalRLimitsZone[zone].first  = *std::min_element(rLimits.begin(),rLimits.end());
    m_globalRLimitsZone[zone].second = *std::max_element(rLimits.begin(),rLimits.end());

    std::vector<double> phiLimits;
    phiLimits.push_back(globalLimitsMin.first.phi()); phiLimits.push_back(globalLimitsMin.second.phi());
    phiLimits.push_back(globalLimitsMax.first.phi()); phiLimits.push_back(globalLimitsMax.second.phi());
    // map to [0,2pi] for righ hand side sensors
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

    // determine the r ranges of the zones in VELO half box frame
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> halfBoxLimitsMin
      (globalToVeloHalfBox(globalLimitsMin.first),globalToVeloHalfBox(globalLimitsMin.second));
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> halfBoxLimitsMax
      (globalToVeloHalfBox(globalLimitsMax.first),globalToVeloHalfBox(globalLimitsMax.second));
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> halfBoxLimitsMid
      (globalToVeloHalfBox(globalLimitsMid.first),globalToVeloHalfBox(globalLimitsMid.second));
    rLimits.clear();
    rLimits.push_back(halfBoxLimitsMin.first.rho()); rLimits.push_back(halfBoxLimitsMin.second.rho());
    rLimits.push_back(halfBoxLimitsMax.first.rho()); rLimits.push_back(halfBoxLimitsMax.second.rho());
    rLimits.push_back(halfBoxLimitsMid.first.rho()); rLimits.push_back(halfBoxLimitsMid.second.rho());
    m_halfboxRLimitsZone[zone].first  = *std::min_element(rLimits.begin(),rLimits.end());
    m_halfboxRLimitsZone[zone].second = *std::max_element(rLimits.begin(),rLimits.end());

    phiLimits.clear();
    phiLimits.push_back(halfBoxLimitsMin.first.phi()); phiLimits.push_back(halfBoxLimitsMin.second.phi());
    phiLimits.push_back(halfBoxLimitsMax.first.phi()); phiLimits.push_back(halfBoxLimitsMax.second.phi());
    // map to [0,2pi] for righ hand side sensors
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

    // extend the phi ranges by half a strip pitch
    double pitch = std::abs(phiPitch(minStrip));
    m_globalPhiLimitsZone [zone].first  -= pitch/2.0;
    m_globalPhiLimitsZone [zone].second += pitch/2.0;
    m_halfboxPhiLimitsZone[zone].first  -= pitch/2.0;
    m_halfboxPhiLimitsZone[zone].second += pitch/2.0;
  }

  return StatusCode::SUCCESS;
}

StatusCode DeVeloPhiType::updateGeometryCache()
{

  StatusCode sc = updatePhiCache();
  if(!sc.isSuccess()) {
    msg() << MSG::ERROR << "Failed to update phi cache." << endmsg;
    return sc;
  }

  sc = updateZoneLimits();
  if(!sc.isSuccess()) {
    msg() << MSG::ERROR << "Failed to update zone limit cache." << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}
