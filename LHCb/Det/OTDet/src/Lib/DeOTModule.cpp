// GaudiKernel
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "DetDesc/Condition.h"

// LHCbKernel
#include "Kernel/LineTraj.h"

// LHCbMath
#include "LHCbMath/Line.h"
#include "LHCbMath/LineTypes.h"
#include "LHCbMath/GeomFun.h"

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

// GSL
#include "gsl/gsl_math.h"
#if !(defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L)
#include <boost/assign/list_of.hpp>
#endif

// local
#include "OTDet/DeOTModule.h"

/** @file DeOTModule.cpp
 *
 *  Implementation of class :  DeOTModule
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 */

using namespace LHCb;

DeOTModule::DeOTModule(const std::string& name) :
  DetectorElement(name),
  m_stationID(0u),
  m_layerID(0u),
  m_quarterID(0u),
  m_moduleID(0u),
  m_elementID(0u),
  m_uniqueModuleID(0u),
  m_nStraws(0u),
  m_angle(0.0),
  m_sinAngle(0.0),
  m_cosAngle(0.0),
  m_xPitch(0.0),
  m_zPitch(0.0),
  m_cellRadius(0.0),
  m_inefficientRegion(0.0),
  m_sensThickness(0.0),
  m_nModules(0),
  m_ySizeModule(0.0),
  m_xMinLocal(0.0),
  m_xMaxLocal(0.0),
  m_yMinLocal(0.0),
  m_yMaxLocal(0.0),
  m_xInverted(false),
  m_yInverted(false),
  m_dxdy{0.0,0.0},
  m_dzdy{0.0,0.0},
  m_propagationVelocity(0.0),
  m_propagationVelocityY(0.0),
  m_halfXPitch(0.0),
  m_monoXZero{0.0,0.0},
  m_calibrationName( "Calibration" ),
  m_calibration(),
  m_statusName( "Status" ),
  m_status(),
  m_monoDx{0.0,0.0},
  m_monoDdxdy{0.0,0.0}
{
  /// Constructor
}


DeOTModule::~DeOTModule() { /// Destructor
}

const CLID& DeOTModule::clID() const {
  return classID();
}

StatusCode DeOTModule::initialize() {
  IDetectorElement* quarter = this->parentIDetectorElement();
  IDetectorElement* layer = quarter->parentIDetectorElement();
  IDetectorElement* station = layer->parentIDetectorElement();
  IDetectorElement* ot = station->parentIDetectorElement();

  // Get specific parameters from the module
  m_moduleID = (unsigned int)param<int>("moduleID");
  m_quarterID = (unsigned int)quarter->params()->param<int>("quarterID");
  m_layerID = (unsigned int)layer->params()->param<int>("layerID");
  m_stationID = (unsigned int)station->params()->param<int>("stationID");
  m_nStraws = (unsigned int)param<int>("nStraws");
  m_angle = layer->params()->param<double>("stereoAngle");
  m_sinAngle = sin(m_angle);
  m_cosAngle = cos(m_angle);
  OTChannelID aChan = OTChannelID(m_stationID, m_layerID, m_quarterID, m_moduleID, 0u);
  setElementID(aChan);
  m_uniqueModuleID = aChan.uniqueModule();

  // Get some general parameters for the OT
  m_xPitch     = ot->params()->param<double>("strawPitchX");
  m_halfXPitch = 0.5*m_xPitch;
  m_zPitch = ot->params()->param<double>("strawPitchZ");
  m_monoXZero[0] = ot->params()->param<double>("firstMonoOffset");
  m_monoXZero[1] = ot->params()->param<double>("secondMonoOffset");
  m_cellRadius = ot->params()->param<double>("cellRadius");
  m_inefficientRegion = ot->params()->param<double>("inefficientRegion");
  m_sensThickness = ot->params()->param<double>("sensThickness");
  m_nModules = (unsigned int)ot->params()->param<int>("nModules");

  // Added for the A-team. This is the calibration for the
  // simulation. In the end, we need to read this from a database.
  m_propagationVelocity = 1/(4.0*Gaudi::Units::ns/Gaudi::Units::m) ;

  // double resolution     = 0.200*Gaudi::Units::mm ;
  // Coefficients of polynomial t(r/rmax): for MC this is just t = 0 + 42/2.5 * r
  //std::vector<double> tcoeff    = boost::assign::list_of(0.0)(42*Gaudi::Units::ns ) ;
  // Coefficients of polynomial sigma_t(r/rmax): for MC this is just sigma_t = 0.200 * 42/2.5
  //std::vector<double> terrcoeff = boost::assign::list_of(resolution * 42*Gaudi::Units::ns / m_cellRadius) ;
  // Since everything is so simple, we need just two bins in the table
  //m_rtrelation = OTDet::RtRelation(m_cellRadius,tcoeff,terrcoeff,2) ;

  // Get the lenght of the module
  //const ISolid* solid = this->geometry()->lvolume()->solid();
  //const SolidBox* mainBox = dynamic_cast<const SolidBox*>(solid);
  const SolidBox* mainBox = dynamic_cast<const SolidBox*>( this->geometry()->lvolume()->solid() );
  m_ySizeModule = mainBox->ysize();
  m_xMaxLocal = (0.5*m_nStraws+0.25)*m_xPitch;
  m_xMinLocal = -m_xMaxLocal;
  m_yMaxLocal = mainBox->yHalfLength();
  m_yMinLocal = -m_yMaxLocal;

  /// Register conditions with update manager svc
  MsgStream msg( msgSvc(), name() );
  try {
    if( msg.level() <= MSG::VERBOSE )
      msg << MSG::VERBOSE << "Registering conditions" << endmsg;
    updMgrSvc()->registerCondition( this, this->geometry()    , &DeOTModule::cacheInfo           );
    if ( hasCondition( m_statusName ) ) { ///< Only do this if condtion is in LHCBCOND
      m_status = condition( m_statusName );
      updMgrSvc()->registerCondition( this, m_status.path()     , &DeOTModule::statusCallback      );
    }
    if ( hasCondition( m_calibrationName ) ) { ///< Only do this if condtion is in LHCBCOND
      m_calibration = condition( m_calibrationName );
      updMgrSvc()->registerCondition( this, m_calibration.path(), &DeOTModule::calibrationCallback );
    } else {
      if( msg.level() <= MSG::VERBOSE )
        msg << MSG::VERBOSE << "Going to use DC06 defaults for RT-relation and T0s" << endmsg;
    }
    if( msg.level() <= MSG::VERBOSE )
      msg << MSG::VERBOSE << "Start first update of conditions" << endmsg;
    StatusCode sc = updMgrSvc()->update( this );
    if ( !sc.isSuccess() ) {
      msg << MSG::WARNING << "Failed to update detector element " << this->name()
          << "!" << endmsg;
      return sc;

    }
  } catch (DetectorElementException &e) {
    msg << MSG::ERROR    << "Failed to update detector element " << this->name()
        << "! " << "See exeption (next)." << endmsg;
    msg << MSG::ERROR << e << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

void DeOTModule::findStraws(const Gaudi::XYZPoint& entryPoint,
                            const Gaudi::XYZPoint& exitPoint,
                            Straws& straws) const {
  /// This is in local cooridinates of a module
  const double xOffset = m_xMinLocal; //-(0.5*m_nStraws + 0.25)*m_xPitch;
  double lo = (entryPoint.x()-xOffset)/m_xPitch;
  double hi = (exitPoint.x()-xOffset)/m_xPitch;

  if (lo > hi) std::swap(lo , hi);

  const int exStraw = 1; ///< Add extra straws to the the left and right
  unsigned int strawLo = GSL_MAX_INT(0, int(std::floor(lo)) - exStraw);
  unsigned int strawHi = GSL_MIN_INT(int(m_nStraws)-1, GSL_MAX_INT(0, int(std::ceil(hi)) + exStraw));

  /// Now let's fill the vector. Remember straw numbering starts at 1, i.e. i+1
  straws.clear();
  straws.reserve(strawHi-strawLo + 1u);
  for (unsigned int i = strawLo; i <= strawHi; ++i) straws.push_back(i+1);
}

void DeOTModule::calculateHits(const Gaudi::XYZPoint& entryPoint,
                               const Gaudi::XYZPoint& exitPoint,
                               std::vector<std::pair<OTChannelID, double> >& chanAndDist) const {
  /// check that entry and exit points are inside module
  if (isInside(entryPoint) && isInside(exitPoint)) {
    /// Make sure channels and driftdistances vectors are empty
    chanAndDist.clear(); ///< This should erase all elements, if any.

    /// Go from global to local.
    Gaudi::XYZPoint enP = toLocal(entryPoint);
    Gaudi::XYZPoint exP = toLocal(exitPoint);

    /// Need this to check that enZ and exZ aren't sort of in the same plane,
    /// i.e. not a curly track. These are typically low momentum (50 MeV)
    /// electrons.
    bool samePlane = std::abs(enP.z()-exP.z()) < m_cellRadius;
    if (!samePlane) { // Track in cell
      /// Track
      Gaudi::Math::XYZLine track(enP, (exP-enP).Unit());
      /// Now let's get a list of possible hit straws
      Straws straws;
      findStraws(enP, exP, straws);
      /// Wire
      const double z = 0.5*m_zPitch;
      Gaudi::XYZPoint wB(0.0, m_yMinLocal, -z);
      Gaudi::XYZPoint wT(0.0, m_yMaxLocal, -z);
      Gaudi::Math::XYZLine wire;
      /// Are the wire and track parallel
      bool notParallel = true;
      Gaudi::XYZPoint mu;
      Gaudi::XYZPoint lambda;
      /// is in efficient region of (F-modules)
      bool efficientY = true;
      unsigned int straw = 0u;
      double x = 0.0;
      double dist = 0.0; /// lambda - mu
      OTChannelID aChannel; /// channelID
      /// loop over straws
      /// First monolayer
      Straws::const_iterator iS;
      for (iS= straws.begin(); iS != straws.end(); ++iS) {
        straw = (*iS);
        x = localUOfStraw(straw);
        wB.SetX(x);
        wT.SetX(x);
        wire = Gaudi::Math::XYZLine(wB, (wT-wB).Unit());
        notParallel = Gaudi::Math::closestPoints(wire, track, mu, lambda);
        if (notParallel) {
          dist = driftDistance(lambda-mu);
          efficientY = isEfficientA(mu.y());
          if (efficientY && std::abs(dist) < m_cellRadius) {
            aChannel = OTChannelID(m_stationID, m_layerID, m_quarterID, m_moduleID, straw);
            chanAndDist.push_back(std::make_pair(aChannel, dist));
          }
        }
      }
      /// Second monolayer
      wB.SetZ(z);
      wT.SetZ(z);
      for (iS= straws.begin(); iS != straws.end(); ++iS) {
        straw = (*iS) + m_nStraws;
        x = localUOfStraw(straw);
        wB.SetX(x);
        wT.SetX(x);
        wire = Gaudi::Math::XYZLine(wB, (wT-wB).Unit());
        notParallel = Gaudi::Math::closestPoints(wire, track, mu, lambda);
        if (notParallel) {
          dist = driftDistance(lambda-mu);
          efficientY = isEfficientB(mu.y());
          if (efficientY && std::abs(dist) < m_cellRadius) {
            aChannel = OTChannelID(m_stationID, m_layerID, m_quarterID, m_moduleID, straw);
            chanAndDist.push_back(std::make_pair(aChannel, dist));
          }
        }
      }
    } else {/// Need this to estimate occupancies
      const double x1 = enP.x();
      const double z1 = enP.z();
      const double x2 = exP.x();
      const double z2 = exP.z();

      double uLow = x1;
      double uHigh = x2;
      if ( uLow > uHigh ) std::swap(uLow, uHigh);

      // zfrac is between 0 and 1. 2.7839542167 means nothing.
      // This seems to acts as a random number generator.
      // if distance xy entry-exit is small generate z3 close
      // to the z1 and z2 ( z1 is close to z2)
      /// Circle in global
      const double u1 = (entryPoint.z() > exitPoint.z()) ? exitPoint.x() : entryPoint.x();
      const double v1 = (entryPoint.z() > exitPoint.z()) ? exitPoint.y() : entryPoint.y();
      double zint;
      const double zfrac = std::modf((std::abs(u1)+std::abs(v1))/2.7839542167, &zint);
      const double distXY = std::sqrt(( exP - enP ).perp2());
      double z3Circ = ((distXY > 2.0*m_xPitch) ? 2.0 * (zfrac-0.5) :(z1<0?-zfrac:zfrac))*m_zPitch;
      double zCirc, uCirc, rCirc;
      sCircle(z1, x1, z2, x2, z3Circ, zCirc, uCirc, rCirc);


      double uStep = uLow;
      double distCirc = 0.0;
      int amb = 0;
      double dist = 0.0;
      OTChannelID aChannel;

      // monolayer A
      unsigned int strawA = hitStrawA(uLow);
      const double zStrawA = -0.5*m_zPitch;//localZOfStraw(strawA);
      while ( (uStep < uHigh) && strawA != 0 ) {
        uStep = localUOfStraw(strawA);
        distCirc = gsl_hypot((zCirc-zStrawA), (uCirc-uStep));
        amb = ((-(uStep-(x1+x2)/2.0)*(distCirc-rCirc)) < 0.0) ? -1 : 1;
        dist = amb*std::abs(distCirc-rCirc);
        const unsigned int straw = strawA;
        if ( std::abs(dist) < m_cellRadius ) {
          aChannel = OTChannelID(m_stationID, m_layerID, m_quarterID, m_moduleID, straw);
          chanAndDist.push_back(std::make_pair(aChannel, dist));
        }
        strawA = nextRightStraw(straw);
      }

      // monolayer B
      unsigned int strawB = hitStrawB(uLow);
      const double zStrawB = 0.5*m_zPitch;//localZOfStraw(strawB);
      uStep = uLow;
      while ( (uStep < uHigh) && strawB != 0 ) {
        uStep = localUOfStraw(strawB);
        distCirc = gsl_hypot((zCirc-zStrawB), (uCirc-uStep));
        amb = ((-(uStep-(x1+x2)/2.0)*(distCirc-rCirc))< 0.0) ?  -1 : 1;
        dist = amb*std::abs(distCirc-rCirc);
        const unsigned int straw = strawB;
        if ( std::abs(dist) < m_cellRadius ) {
          aChannel = OTChannelID(m_stationID, m_layerID, m_quarterID, m_moduleID, straw);
          chanAndDist.push_back(std::make_pair(aChannel, dist));
        }
        strawB = nextRightStraw(straw);
      }
    } //curling tracks
  } else {
    MsgStream msg(msgSvc(), name());
    if( msg.level() <= MSG::VERBOSE )
      msg << MSG::DEBUG << "Entry and exit points are not inside module. Failed to generate hits!" << endmsg;
  }
}

void DeOTModule::sCircle(const double z1, const double u1, const double z2,
                         const double u2, const double z3c,
                         double& zc, double& uc, double& rc) const {
  const double zw=(z1+z2)/2.0;
  double uw=(u2+u1)/2.0;

  zc=0.5*(z3c*z3c-zw*zw-(u1-uw)*(u1-uw))/(z3c-zw);
  uc=uw;
  rc=std::abs(zc-z3c);
}

double DeOTModule::distanceToWire(const unsigned int aStraw,
                                  const Gaudi::XYZPoint& aPoint,
                                  const double tx, const double ty) const {
  // go to the local coordinate system
  Gaudi::XYZVector vec(tx, ty, 1.);
  Gaudi::XYZPoint localPoint = toLocal(aPoint);
  Gaudi::XYZVector localVec = toLocal(aPoint+vec) - localPoint;

  // calculate distance to the straw
  double u = localPoint.x()+localVec.x()*(localZOfStraw(aStraw)-localPoint.z());
  double cosU = 1.0/gsl_hypot(1.0, (localVec.x()/localVec.z()));

  // return distance to straw
  return (u-localUOfStraw(aStraw))*cosU;
}

void DeOTModule::monoLayerIntersection(int monolayer,
				       const Gaudi::XYZPoint& aPoint,
				       const double tx, const double ty,
				       double& straw, double& yfrac) const {

  const Gaudi::XYZVector& dp = m_dp0di ;
  Gaudi::XYZVector dX = aPoint - m_p0[monolayer] ;

  // the is the most efficient I could come up with
  double trackCrossWireX =   (ty*m_dzdy[monolayer] -      1) ;
  double trackCrossWireY = - (tx*m_dzdy[monolayer] - m_dxdy[monolayer]) ;

  double a = trackCrossWireX * ( dX.x() - tx * dX.z() ) ;
  double b = trackCrossWireY * ( dX.y() - ty * dX.z() ) ;
  double c = trackCrossWireY * ( ty * dp.z() - dp.y() ) ;
  double d = trackCrossWireX * ( tx * dp.z() - dp.x() ) ;

  double u = - (a + b) / (c + d ) ;

  // here we could still optimize something
  double eta = ( ( tx * dp.z() - dp.x() ) * u + dX.x() - tx * dX.z() ) / trackCrossWireY ;

  // cross check with this alternative computation
  //   {
  //     Gaudi::XYZVector q( m_dxdy, 1, m_dzdy) ;
  //     Gaudi::XYZVector n = q.Cross( dp ) ;
  //     Gaudi::XYZVector t(tx,ty,1) ;
  //     double dz = - dX.Dot(n) / t.Dot(n) ;
  //     Gaudi::XYZVector inplanevector = dX + dz * t ;
  //     double thisu   = inplanevector.Dot( dp )/dp.Mag2() ;
  //     double thiseta = inplanevector.Dot( q )/q.Mag2() ;
  //     eta = thiseta ;
  //     u   = thisu ;
  //   }

  yfrac = eta / m_dy[monolayer] ;
  straw = u + 1 ;


}

void DeOTModule::clear() {
  m_midTraj[0].reset();
  m_midTraj[1].reset();
}

StatusCode DeOTModule::cacheInfo() {
  clear();

  double xUpper = m_xMaxLocal;
  double xLower = m_xMinLocal;
  double yUpper = m_yMaxLocal;
  double yLower = m_yMinLocal;

  /// Direction; points to readout
  if (bottomModule()) std::swap(yUpper, yLower);
  Gaudi::XYZPoint g1 = globalPoint(0.0, yLower, 0.0);
  Gaudi::XYZPoint g2 = globalPoint(0.0, yUpper, 0.0);
  Gaudi::XYZVector dir = (g2 - g1).Unit() ;
  
  /// Fix directions for monolayer shifts. Best to do this also in the
  /// local frame, but we'll assume deviations are small.
  for( unsigned char mono=0; mono<2; ++mono) {
    double y0 = (yUpper + yLower)/2 ;
    Gaudi::XYZPoint monog1 = globalPoint((yLower-y0)*m_monoDdxdy[mono], yLower, 0.0);
    Gaudi::XYZPoint monog2 = globalPoint((yUpper-y0)*m_monoDdxdy[mono], yUpper, 0.0);
    m_dir[mono] = Gaudi::XYZVector(monog2-monog1).Unit();
  }

  /// trajs of middle of monolayers
  Gaudi::XYZPoint g3[2];
  /// 0 -> first monolayer
  g3[0] = globalPoint(xLower, 0.0, -0.5*m_zPitch);
  /// 1 -> second monolayer
  g3[1] = globalPoint(xLower, 0.0, +0.5*m_zPitch);
  Gaudi::XYZPoint g4[2];
  /// first monolayer
  g4[0] = globalPoint(xUpper, 0.0, -0.5*m_zPitch);
  /// second monolayer
  g4[1] = globalPoint(xUpper, 0.0, +0.5*m_zPitch);
  /// first monolayer
  m_midTraj[0].reset(new LineTraj(g3[0], g4[0]));
  /// second monolayer
  m_midTraj[1].reset(new LineTraj(g3[1], g4[1]));

  /// range -> wire length
  /// wire length = module length
  /// first mono layer
  m_range[0] = std::make_pair(m_yMinLocal, m_yMaxLocal);
  /// second mono layer
  m_range[1] = std::make_pair(m_yMinLocal, m_yMaxLocal);
  // correct for inefficient regions in long modules
  //if (longModule() && topModule()) m_range[0].first=m_yMinLocal+m_inefficientRegion;
  //if (longModule() && bottomModule()) m_range[1].first=m_yMinLocal+m_inefficientRegion;
  // correct for inefficient regions in long modules
  // m_dir "corrects" for sign of min and max
  if (longModule() && topModule()    )     m_range[m_layerID%2].first = m_yMinLocal + m_inefficientRegion;
  if (longModule() && bottomModule() ) m_range[1 - m_layerID%2].first = m_yMinLocal + m_inefficientRegion;

  /// plane
  m_plane = Gaudi::Plane3D(g1, g2, g4[0] + 0.5*(g4[1]-g4[0]));

  m_entryPlane = Gaudi::Plane3D(m_plane.Normal(), globalPoint(0.,0.,-0.5*m_sensThickness));
  m_exitPlane = Gaudi::Plane3D(m_plane.Normal(), globalPoint(0.,0., 0.5*m_sensThickness));
  m_centerModule = globalPoint(0.,0.,0.);

  // These are the trajectories as used by the pattern reco. We
  // extract these from the track fit trajectories: That may seem
  // nonsense, but it is most fool proof. Do this such that it also
  // works for piecewise trajectories.
  m_dp0di = (g4[0]-g3[0]).unit() * m_xPitch ;
  for( int imono=0; imono<2; ++imono) {
    std::auto_ptr<Trajectory> traj = trajectoryFirstWire(imono) ;
    Gaudi::XYZPoint p0 = traj->position(traj->beginRange()) ;
    Gaudi::XYZPoint p1 = traj->position(traj->endRange()) ;
    Gaudi::XYZVector dp = p1 - p0 ;
    m_dy[imono] = dp.y() ;
    m_p0[imono] = p0 ;
    m_dxdy[imono] = dp.x()/dp.y() ;
    m_dzdy[imono] = dp.z()/dp.y() ;
  }
  
  // propagation velocity along y-direction (includes correction for readout side)
  m_propagationVelocityY = m_propagationVelocity * dir.y() ;

  // Update the stereo angle. We correct by 'pi' if necessary.
  if(dir.y()<0) dir *= -1 ;
  m_angle = atan2(-dir.x(),dir.y()) ;
  m_cosAngle    = cos( m_angle ) ;
  m_sinAngle    = sin( m_angle ) ;

  // the t0 will be defined such that
  //
  //  tdc = drifttime + propagationtime + delta-tof + t0
  //
  // the delta-tof is the tof compared to a straight line to the
  // midpoint of the straw. does that make sense, actually?

  // The following just makes sense for MC, of course.
  std::fill( m_strawdefaulttof, m_strawdefaulttof + MAXNUMCHAN, 0 ) ;
  for(unsigned int istraw=1; istraw<=2*m_nStraws; ++istraw) {
    OTChannelID id(stationID(),layerID(),quarterID(),moduleID(),istraw,0) ;
    std::auto_ptr<Trajectory> traj = trajectory(id) ;
    Gaudi::XYZPoint p0 = traj->position(0.5*(traj->beginRange()+traj->endRange())) ;
    // to get same results as with old OTTimeCreator, use x-z plane only
    //double defaulttof = p0.r() / Gaudi::Units::c_light;
    double defaulttof = sqrt(p0.x()*p0.x() + p0.z()*p0.z()) / Gaudi::Units::c_light;
    m_strawdefaulttof[istraw - 1] = defaulttof ;
  }

  /// Only call this one if the calibration condition doesn't exist
  /// Call it after all the trajectory stuff and after we've set some default tofs
  if ( !hasCondition( m_calibrationName ) ) fallbackDefaults();

  return StatusCode::SUCCESS;
}


StatusCode DeOTModule::calibrationCallback() {
  /// Don't need to check if condition exists
  /// Callback will only be registered with ums if condition exists
  MsgStream msg( msgSvc(), name() );
  if( msg.level() <= MSG::DEBUG )
    msg << MSG::DEBUG << "Updating Calibration parameters" << endmsg;
  try {
    const std::vector<double>& trParameters =
      m_calibration->param< std::vector<double> >( "TRParameters" ); // in ns
    const std::vector<double>& sigmaTParameters =
      m_calibration->param< std::vector<double> >( "STParameters" ); // in ns
    const std::vector<double>& t0Parameters =
      m_calibration->param< std::vector<double> >( "TZero" );

    // Here we assume the cell radius is the same for all straws. Should be  ;)
    // Maybe add the number of bins to the conditions
    m_rtrelation = OTDet::RtRelation( m_cellRadius, trParameters, sigmaTParameters ) ;

//    double propagationVelocity = 1.0 / (4.0 * Gaudi::Units::ns/Gaudi::Units::m);
//    if(m_calibration->exists("PropagationVelocity")) propagationVelocity = m_calibration->param<double>("PropagationVelocity");
//    m_propagationVelocity = propagationVelocity;
//    m_propagationVelocityY = m_propagationVelocity * m_dir.y();

    if(m_calibration->exists("WalkParameters"))
    {
      const std::vector<double>& walkParameters = m_calibration->param< std::vector<double> >("WalkParameters");
      if(walkParameters.size() == 4)
      {
        m_walkrelation = OTDet::WalkRelation(walkParameters[0], walkParameters[1], walkParameters[2], walkParameters[3]);
      }
      else
      {
        m_walkrelation = OTDet::WalkRelation();
        msg << MSG::ERROR << "There should be 4 walk parameters: " << walkParameters.size() << " provided" << endmsg;
      }
    }
    else m_walkrelation = OTDet::WalkRelation();

    m_monoDx[0] = m_monoDx[1] = 0;
    m_monoDdxdy[0] = m_monoDdxdy[1] = 0;
    if(m_calibration->exists("MonoAlignment")) {
      const std::vector<double>& pars = m_calibration->param< std::vector<double> >("MonoAlignment");
      if(pars.size() == 2) {
        m_monoDx[0]    =  pars[0]/2 ;
        m_monoDx[1]    = -pars[0]/2;
        m_monoDdxdy[0] = pars[1]/2 ;
        m_monoDdxdy[1] = -pars[1]/2;
      } else {
        msg << MSG::ERROR << "There should be 2 mono alignment parameters: " << pars.size() << " provided" << endmsg;
      }
    } 

    // how we set the straw t0 depends on the size of the vector.  we
    // allow that the calibration sets either every connected channel,
    // or for all channels
    if( t0Parameters.size() == nChannels() || t0Parameters.size() == MAXNUMCHAN ) // 1 t0 per channel
      std::copy( t0Parameters.begin(), t0Parameters.end(), m_strawt0 ) ;
    else if( t0Parameters.size()== nChannels()/32 || t0Parameters.size()== 4 ) // 1 t0 per otis
      for( size_t ichan=0; ichan<nChannels(); ++ichan)
	m_strawt0[ichan] = t0Parameters[ichan/32] ;
    else if( t0Parameters.size() == 1 )      // 1 t0 per module
      std::fill( m_strawt0, m_strawt0 + MAXNUMCHAN, t0Parameters.front() ) ;
    else {
      msg << MSG::ERROR << "Bad length of t0 vector in conditions: " << t0Parameters.size() << endmsg ;
      std::fill( m_strawt0, m_strawt0 + MAXNUMCHAN, 0 ) ;
    }
  }
  catch (...) {
    msg << MSG::ERROR << "Failed to update calibration conditions for " << this->name() << "!" << endmsg;
    return StatusCode::FAILURE;
  }

  // we need to propagate the info from the change in wire direction
  return cacheInfo() ;
}

StatusCode DeOTModule::statusCallback() {
  /// Don't need to check if condition exists
  /// Callback will only be registered with ums if condition exists
  MsgStream msg( msgSvc(), name() );
  if( msg.level() <= MSG::DEBUG )
    msg << MSG::DEBUG << "Updating Status parameters" << endmsg;
  try {
    const std::vector<int>& statusflags = m_status->param< std::vector<int> >( "ChannelStatus" );
    if( statusflags.size() == nChannels() || statusflags.size() == MAXNUMCHAN ) // 1 t0 per channel
      std::copy( statusflags.begin(), statusflags.end(), m_strawStatus ) ;
    else if( statusflags.size()== nChannels()/32 || statusflags.size()== 4 ) // 1 t0 per otis
      for( size_t ichan=0; ichan<nChannels(); ++ichan)
	m_strawStatus[ichan] = statusflags[ichan/32] ;
    else if( statusflags.size() == 1 )      // 1 t0 per module
      std::fill( m_strawStatus, m_strawStatus + MAXNUMCHAN, statusflags.front() ) ;
    else {
      msg << MSG::ERROR << "Bad length of straw status vector in conditions: " << statusflags.size() << endmsg ;
      std::fill( m_strawStatus, m_strawStatus + MAXNUMCHAN, 0 ) ;
    }
  }
  catch (...) {
    msg << MSG::ERROR << "Failed to update status conditions for " << this->name() << "!" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

std::auto_ptr<LHCb::Trajectory> DeOTModule::trajectoryFirstWire(int monolayer) const {
  /// Default is 0 -> straw 1
  double lUwire = (monolayer == 1 ? localUOfStraw(m_nStraws+1) : localUOfStraw(1) );
  Gaudi::XYZPoint firstWire = m_midTraj[monolayer]->position(lUwire);
  return std::auto_ptr<LHCb::Trajectory>(new LineTraj(firstWire, m_dir[monolayer], m_range[monolayer], true));
}

std::auto_ptr<LHCb::Trajectory> DeOTModule::trajectoryLastWire(int monolayer) const {
  /// Default is 1 -> straw 64(s3)/128
  double lUwire = (monolayer == 0 ? localUOfStraw(m_nStraws) : localUOfStraw(2*m_nStraws) );
  Gaudi::XYZPoint lastWire = m_midTraj[monolayer]->position(lUwire);
  return std::auto_ptr<LHCb::Trajectory>(new LineTraj(lastWire, m_dir[monolayer], m_range[monolayer], true)) ;
}

/// Returns a Trajectory representing the wire identified by the LHCbID
/// The offset is zero for all OT Trajectories
std::auto_ptr<LHCb::Trajectory> DeOTModule::trajectory(const OTChannelID& aChan,
                                                       const double /*offset*/) const {
  if (!contains(aChan)) {
    throw GaudiException("Failed to make trajectory!", "DeOTModule.cpp",
			 StatusCode::FAILURE);
  }

  unsigned int aStraw = aChan.straw();
  unsigned int mono = this->mono(aStraw) ;
  Gaudi::XYZPoint posWire = m_midTraj[mono]->position( localUOfStraw(aStraw) );
  return std::auto_ptr<Trajectory>(new LineTraj(posWire, m_dir[mono], m_range[mono],true));
}

StatusCode DeOTModule::setStrawStatus( const std::vector< int >& flags )
{
  if ( hasCondition( m_statusName ) ) {
    // Modify condition in TES
    m_status->param< std::vector<int> >( "ChannelStatus" ) = flags ;
    // m_calibration->neverUpdateMode() /// Always valid for any and all IOV
    // Now we need to inform the ums that the condition has changed
    updMgrSvc()->invalidate( m_status.target() );
    // Trigger an update
    StatusCode sc = updMgrSvc()->update( this );
    if ( !sc.isSuccess() ) {
      MsgStream msg( msgSvc(), name() );
      msg << MSG::ERROR << "Failed to update straw status flags for " << this->name() << "!" << endmsg;
      return StatusCode::FAILURE;
    }
  } else {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Condition " << m_statusName << " doesn't exist for " << this->name() << "!" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

const std::vector<int>&  DeOTModule::strawStatus() const
{
  const std::vector<int> *rc(0) ;
  if ( hasCondition( m_statusName ) ) {
    rc =&( m_status->param< std::vector<int> >( "ChannelStatus" )) ;
  } else {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Condition " << m_statusName << " doesn't exist for " << this->name() << "!" << endmsg;
    static std::vector<int> dummy ;
    rc = &dummy ;
  }
  return *rc ;
}

StatusCode DeOTModule::setStrawT0s( const std::vector< double >& tzeros ) {
  if ( hasCondition( m_calibrationName ) ) {
    /// Modify condition in TES
    m_calibration->param< std::vector<double> >( "TZero" ) = tzeros;
    /// m_calibration->neverUpdateMode() /// Always valid for any and all IOV
    /// Now we need to inform the ums that the condition has changed
    updMgrSvc()->invalidate( m_calibration.target() );
    /// Trigger an update
    StatusCode sc = updMgrSvc()->update( this );
    if ( !sc.isSuccess() ) {
      MsgStream msg( msgSvc(), name() );
      msg << MSG::ERROR << "Failed to update straw T0 conditions for " << this->name() << "!" << endmsg;
      return StatusCode::FAILURE;
    }
  } else {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Condition " << m_calibrationName << " doesn't exist for " << this->name() << "!" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

const std::vector<double>&  DeOTModule::getStrawT0s() const {
  const std::vector<double> *rc(0) ;
  if ( hasCondition( m_calibrationName ) ) {
    rc =&( m_calibration->param< std::vector<double> >( "TZero" )) ;
  } else {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Condition " << m_calibrationName << " doesn't exist for " << this->name() << "!" << endmsg;
    static std::vector<double> dummy ;
    rc = &dummy ;
  }
  return *rc ;
}

StatusCode DeOTModule::setRtRelation(const OTDet::RtRelation& rtr) {
  if ( hasCondition( m_calibrationName ) ) {
    /// First we need to get the tr and st parameters
    std::vector< double > trParameters     = rtr.tcoeff();
    std::vector< double > sigmaTParameters = rtr.terrcoeff();
    /// Ok now we modify the conditions in the tes
    m_calibration->param< std::vector< double > >( "TRParameters" ) = trParameters;
    m_calibration->param< std::vector< double > >( "STParameters" ) = sigmaTParameters;
    /// Now we need to inform the ums that the condition has changed
    updMgrSvc()->invalidate( m_calibration.target() );
    /// Trigger an update
    StatusCode sc = updMgrSvc()->update( this );
    if ( !sc.isSuccess() ) {
      MsgStream msg( msgSvc(), name() );
      msg << MSG::ERROR << "Failed to update RT conditions for " << this->name() << "!" << endmsg;
      return StatusCode::FAILURE;
    }
  } else {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Condition " << m_calibrationName << " doesn't exist for " << this->name() << "!" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

void DeOTModule::fallbackDefaults() {
  /// Need some default t0s and rt-relation
  /// Only to ensure backwards compatibility with DC06
  /// Frist the rt-relation
  double resolution     = 0.200*Gaudi::Units::mm ;
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
  // Coefficients of polynomial t(r/rmax): for MC this is just t = 0 + 42/2.5 * r
  std::vector<double> tcoeff    = {0.0, 42*Gaudi::Units::ns};
  // Coefficients of polynomial sigma_t(r/rmax): for MC this is just sigma_t = 0.200 * 42/2.5
  std::vector<double> terrcoeff = {resolution * 42*Gaudi::Units::ns / m_cellRadius};
#else
  // Coefficients of polynomial t(r/rmax): for MC this is just t = 0 + 42/2.5 * r
  std::vector<double> tcoeff    = boost::assign::list_of(0.0)(42*Gaudi::Units::ns ) ;
  // Coefficients of polynomial sigma_t(r/rmax): for MC this is just sigma_t = 0.200 * 42/2.5
  std::vector<double> terrcoeff = boost::assign::list_of(resolution * 42*Gaudi::Units::ns / m_cellRadius) ;
#endif
  // Since everything is so simple, we need just two bins in the table
  m_rtrelation = OTDet::RtRelation(m_cellRadius,tcoeff,terrcoeff,2) ;
  m_walkrelation = OTDet::WalkRelation();
  // Now the T0s
  const double startReadOutGate[]   = { 28.0*Gaudi::Units::ns, 30.0*Gaudi::Units::ns, 32.0*Gaudi::Units::ns } ;
  double thisModuleStartReadOutGate = startReadOutGate[m_stationID-1] ;

  // the t0 will be defined such that
  //
  //  tdc = drifttime + propagationtime + delta-tof + t0
  //
  // the delta-tof is the tof compared to a straight line to the
  // midpoint of the straw. does that make sense, actually?

  // The following just makes sense for MC, of course.
  std::fill( m_strawt0, m_strawt0 + MAXNUMCHAN, 0 ) ;
  for( unsigned int istraw = 1; istraw <= 2*m_nStraws; ++istraw )
    m_strawt0[istraw - 1] = m_strawdefaulttof[istraw - 1] - thisModuleStartReadOutGate ;

}

StatusCode DeOTModule::setWalkRelation(const OTDet::WalkRelation& walkRelation) {
  if(!hasCondition(m_calibrationName))
  {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Condition " << m_calibrationName << " doesn't exist for " << this->name() << "!" << endmsg;
    return StatusCode::FAILURE;
  }

  std::vector<double> walkParameters;
  walkParameters.push_back(walkRelation.getOff());
  walkParameters.push_back(walkRelation.getAmp());
  walkParameters.push_back(walkRelation.getTau());
  walkParameters.push_back(walkRelation.getDpt());
  if(m_calibration->exists("WalkParameters")) m_calibration->param< std::vector<double> >("WalkParameters") = walkParameters;
  else m_calibration->addParam("WalkParameters", walkParameters, "Walk parameters");

  updMgrSvc()->invalidate( m_calibration.target() );
  StatusCode sc = updMgrSvc()->update( this );

  if(!sc.isSuccess())
  {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Failed to update walk conditions for " << this->name() << "!" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode DeOTModule::setMonoAlignment(double dx, double ddxdy)
{
  // Do this via the update service such that the condition is created.
  std::vector<double> pars{ dx, ddxdy } ;
  if(m_calibration->exists("MonoAlignment")) m_calibration->param< std::vector<double> >("MonoAlignment") = pars;
  else m_calibration->addParam("MonoAlignment",pars, "Monolayer alignment parameters");

  updMgrSvc()->invalidate( m_calibration.target() );
  StatusCode sc = updMgrSvc()->update( this );

  return sc ;
}

double DeOTModule::propagationTime(const LHCb::OTChannelID& channel, double arclen) const
{
  double dist2readout = trajectory(channel)->endRange() - arclen;
  double dist2strawend = wireLength(channel) - dist2readout;
  return dist2readout / propagationVelocity() + walkRelation().walk(dist2strawend);
}

double DeOTModule::propagationTimeFromY(const LHCb::OTChannelID& channel, double globalY) const
{
  //FIXME: isn't there a quicker way to do this, without the trajectory?
  double dist2readout = (trajectory(channel)->endPoint().y() - globalY) / m_dir[mono(channel)].y() ;
  double dist2strawend = wireLength(channel) - dist2readout;
  return dist2readout / propagationVelocity() + walkRelation().walk(dist2strawend);
}
