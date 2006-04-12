// $Id: DeOTModule.cpp,v 1.12 2006-04-12 14:11:56 janos Exp $
/// Kernel
#include "Kernel/Point3DTypes.h"
#include "Kernel/SystemOfUnits.h"
#include "Kernel/LineTraj.h"

/// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

/// GSL
#include "gsl/gsl_math.h"

/// local
#include "OTDet/DeOTModule.h"

/** @file DeOTModule.cpp
 *
 *  Implementation of class :  DeOTModule
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 */

using namespace LHCb;

DeOTModule::DeOTModule( const std::string& name ) :
  DetectorElement( name ),
  m_stationID(0u),
  m_layerID(0u),
  m_quarterID(0u),
  m_moduleID(0u),
  m_elementID(0u),
  m_uniqueModuleID(0u),
  m_nStraws(0u),
  m_stereoAngle(0.0),
  m_sinAngle(0.0),
  m_cosAngle(0.0),
  m_xPitch(0.0),
  m_zPitch(0.0),
  m_cellRadius(0.0),
  m_inefficientRegion(0.0),
  m_nModules(0),
  m_ySizeModule(0.0),
  m_yHalfModule(0.0) { 
  /// Constructor 
}

DeOTModule::~DeOTModule() { /// Destructor 
}

const CLID& DeOTModule::clID() const { 
  return classID();
}


StatusCode DeOTModule::initialize() 
{
 
  IDetectorElement* quarter = this->parentIDetectorElement();
  IDetectorElement* layer = quarter->parentIDetectorElement();
  IDetectorElement* station = layer->parentIDetectorElement();
  IDetectorElement* ot = station->parentIDetectorElement();

  // Get specific parameters from the module
  m_moduleID = (unsigned int) param<int>("moduleID");
  m_quarterID = (unsigned int) quarter->params()->param<int>("quarterID");
  m_layerID = (unsigned int) layer->params()->param<int>("layerID");
  m_stationID = (unsigned int) station->params()->param<int>("stationID");
  m_nStraws = (unsigned int) param<int>("nStraws");
  m_stereoAngle = layer->params()->param<double>("stereoAngle");
  m_sinAngle = sin(m_stereoAngle);
  m_cosAngle = cos(m_stereoAngle);  
  OTChannelID aChan = OTChannelID(m_stationID, m_layerID, 
                                  m_quarterID, m_moduleID, 0u);
  setElementID(aChan);
  m_uniqueModuleID = aChan.uniqueModule();

  // Get some general parameters for the OT
  m_xPitch = ot->params()->param<double>("strawPitchX");
  m_zPitch = ot->params()->param<double>("strawPitchZ");
  m_cellRadius = ot->params()->param<double>("cellRadius");
  m_inefficientRegion = ot->params()->param<double>("inefficientRegion");
  m_nModules = (unsigned int) ot->params()->param<int>("nModules");

  // Get the lenght of the module
  const ILVolume* lv = (this->geometry())->lvolume();
  const ISolid* solid = lv->solid();
  const SolidBox* mainBox = dynamic_cast<const SolidBox*>(solid);
  m_ySizeModule = mainBox->ysize();
  m_yHalfModule = mainBox->yHalfLength();

  return StatusCode::SUCCESS;
}

void DeOTModule::findStraws(const Gaudi::XYZPoint& entryPoint, 
                            const Gaudi::XYZPoint& exitPoint, 
                            std::vector<unsigned int>& straws) const {
  /// This is in local cooridinates of a module
  double xOffset = -(m_nStraws/2. + 0.25) * m_xPitch;
  double lo = (entryPoint.x()-xOffset)/m_xPitch; 
  double hi = (exitPoint.x()-xOffset)/m_xPitch;
  
  if (lo > hi) std::swap(lo , hi);

  int exStraw = 1; ///< Add extra straws to the the left and right
  unsigned int strawLo = GSL_MAX_INT(0, int(std::floor(lo)) - exStraw);
  unsigned int strawHi = GSL_MIN_INT(int(m_nStraws)-1, int(std::ceil(hi)) + exStraw);

  /// Make sure straws vector is empty 
  straws.clear(); ///< This should erase all elements, if any.
  straws.reserve(strawHi-strawLo);
  
  /// Now let's fill the vector. Remember straw numbering starts at 1, i.e. i+1
  for (unsigned int i = strawLo; i <= strawHi; ++i) straws.push_back(i+1);
}

StatusCode DeOTModule::findDocaParams(const Gaudi::XYZPoint& entryPoint,
                                      const Gaudi::XYZVector& pUnit,
                                      const Gaudi::XYZPoint& wireBottom,
                                      const Gaudi::XYZVector& wUnit,
                                      double& lambda, double& mu) const { 
  Gaudi::XYZVector PenMinWb = (entryPoint - wireBottom);
  double e_pDote_w = pUnit.Dot(wUnit);
  
  double dnom = 1.0 - gsl_pow_2(e_pDote_w);
  double eps = 0.0001;
  
  if ( dnom > eps ) {
    lambda = (-1/dnom)*(PenMinWb.Dot(pUnit) - PenMinWb.Dot(wUnit)*e_pDote_w);
    mu = (1/dnom)*(PenMinWb.Dot(wUnit) - PenMinWb.Dot(pUnit)*e_pDote_w);
    return StatusCode::SUCCESS;
  } else {
    /// You never know :-)
    MsgStream msg( msgSvc(), name() );
    msg << MSG::WARNING << "Track is parallel to wire" << endreq;
    return StatusCode::FAILURE;
  }
}

double DeOTModule::docaWire(const Gaudi::XYZPoint& entryPoint,
                            const Gaudi::XYZVector& pUnit,
                            const Gaudi::XYZPoint& wireBottom,
                            const Gaudi::XYZVector& wUnit,
                            double lambda,
                            double mu) const {
  
  Gaudi::XYZVector PenMinWb = (entryPoint - wireBottom);
  Gaudi::XYZVector d = PenMinWb + lambda*pUnit - mu*wUnit;
  
  /// Right(Left) of straw -1(1)
  double ambiguity = (d.x() > 0 ? 1 : -1);
  
  return ambiguity*d.r();
}

/// This gives you the x position of the wire
double DeOTModule::localUOfStraw(const int aStraw) const {
  int tmpStraw = aStraw;
  double uLeftStraw =  -(m_nStraws/2. - 0.25) * m_xPitch;
  if (this->monoLayerB(aStraw)) { // monoLayer B
    tmpStraw -= m_nStraws;
    uLeftStraw += 0.5 * m_xPitch;
  }
  tmpStraw -= 1; // straw numbering starts at 1
  
  return uLeftStraw + tmpStraw * m_xPitch;
}

StatusCode DeOTModule::calculateHits(const Gaudi::XYZPoint& entryPoint,
                                     const Gaudi::XYZPoint& exitPoint,
                                     std::vector<OTChannelID>& channels,
                                     std::vector<double>& driftDistances) const {

  /// Before we do anything crazy let's first check if the 
  /// entry and exit points are inside the module.
  if ( !isInside(entryPoint) || !isInside(exitPoint) ) return StatusCode::FAILURE;
  
  /// Make sure channels and driftdistances vectors are empty 
  channels.clear(); ///< This should erase all elements, if any.
  driftDistances.clear(); ///< This should erase all elements, if any.
  
  /// OK, so apparently the points are inside, so let's get the
  /// the range of straws that might contain hits. First we need to
  /// convert the points to the local coordinate system of the module.
  const IGeometryInfo* gi = this->geometry();
  Gaudi::XYZPoint enP = gi->toLocal(entryPoint);
  Gaudi::XYZPoint exP = gi->toLocal(exitPoint);

  /// Now let's get the range of straws
  std::vector<unsigned int> rStraws;
  findStraws(enP, exP, rStraws);

  /// Unit vector parallel to entry and exit points
  Gaudi::XYZVector e_p = (exP - enP).unit();

  /// Now, let's loop over the straws and check if they do contain hits
  /// and asign OTChannelID's to those that do.
  /// This going to be the same for monolayer A as well as monolayer B, except
  /// for the channel numbering. So it makes sense to define a method
  /// findHitStraws(...)

  /// Need this to check if hits are in the inefficient regions
  double enY = enP.y();
  double exY = exP.y();
  if (enY > exY) std::swap(enY,exY);
  double yMiddle = (enY + exY)/2;
  
  /// Need this to check that enZ and exZ aren't sort of in the same plane,
  /// i.e. not a curly track. These are typically low momentum (50 MeV) 
  /// electrons.
  double enZ = enP.z();
  double exZ = exP.z();
  bool samePlane = std::abs(enZ-exZ) < 1.*m_cellRadius;
  
  Gaudi::XYZPoint wB;
  Gaudi::XYZPoint wT;
  Gaudi::XYZVector e_w; ///< Unit vector parallel to wire
  
  /// See LHCb note: 2003-019
  // If top long module wire length in monolayer A is 2.45m - 0.036m
  isEfficientA(yMiddle)?wB.SetY(-m_yHalfModule+m_inefficientRegion):wB.SetY(-m_yHalfModule);
  // If bottom long module wirelength in monolayer B is 2.45m - 0.036m
  isEfficientB(yMiddle)?wT.SetY(m_yHalfModule-m_inefficientRegion):wT.SetY(m_yHalfModule);
    
  if (!samePlane) { // Track in cell
  
    OTChannelID tmpChannel;
    
    /// first layer
    wB.SetZ(-0.5*m_zPitch);
    wT.SetZ(-0.5*m_zPitch);
    std::vector<unsigned int>::iterator iStraw = rStraws.begin();
    for ( ; iStraw != rStraws.end() ; ++iStraw) {
      double mu;
      double lambda;
      /// Wire bottom
      wB.SetX(localUOfStraw(*iStraw));
      /// Wire top
      wT.SetX(localUOfStraw(*iStraw)); 
            
      e_w = (wT - wB).unit();

      findDocaParams(enP, e_p, wB, e_w, lambda, mu);
      double dist = docaWire(enP, e_p, wB, e_w, lambda, mu);
      /// Do we have a hit?
      if (fabs(dist) < m_cellRadius && isEfficientA(yMiddle)) {
        tmpChannel = OTChannelID(m_stationID, m_layerID,
                                 m_quarterID, m_moduleID, *iStraw);
        channels.push_back(tmpChannel);
        driftDistances.push_back(dist);
      }
    }

    /// second layer
    wB.SetZ(0.5*m_zPitch);
    wT.SetZ(0.5*m_zPitch);
    iStraw = rStraws.begin();
    for ( ; iStraw != rStraws.end() ; ++iStraw) {
      double mu;
      double lambda;
      /// Wire bottom
      wB.SetX(localUOfStraw(*iStraw+m_nStraws));
      /// Wire top
      wT.SetX(localUOfStraw(*iStraw+m_nStraws));
    
      e_w = (wT - wB).unit();
      
      findDocaParams(enP, e_p, wB, e_w, lambda, mu);
      double dist = docaWire(enP, e_p, wB, e_w, lambda, mu);
      /// Do we have a hit?
      if (fabs(dist) < m_cellRadius && isEfficientB(yMiddle)) {
        tmpChannel = OTChannelID(m_stationID, m_layerID,
                                 m_quarterID, m_moduleID, 
                                 m_nStraws+*iStraw);
        channels.push_back(tmpChannel);
        driftDistances.push_back(dist);
      }
    }
  } else { // curly track
    /// This is the old curly track code. I still need to figure out
    /// how to treat this better and make it more readable.
    //msg << MSG::DEBUG << "==> We have a curly track" << endreq;
    //msg << MSG::DEBUG << "Pen_x = " << enP.x() << " Pen_y = " << enP.y() 
    //<< " Pen_z = " << enP.z() << endreq;
    //msg << MSG::DEBUG << "Pex_x = " << exP.x() << " Pex_y = " << exP.y() 
    //<< " Pex_z = " << exP.z() << endreq;
    
    double x1 = enP.x();
    double z1 = enP.z();
    double x2 = exP.x();
    double z2 = exP.z();

    double uLow = x1;
    double uHigh = x2;      
    if ( uLow > uHigh ) std::swap(uLow, uHigh);
        
    double distXY = ( exP - enP ).Rho();
    double z3Circ,zCirc,uCirc,rCirc;
    double zfrac,zint;

    // zfrac is between 0 and 1. 2.7839542167 means nothing.
    // This seems to acts as a random number generator.
    // if distance xy entry-exit is small generate z3 close
    // to the z1 and z2 ( z1 is close to z2)
    double u1 = (entryPoint.z() > exitPoint.z()) ? exitPoint.x() : entryPoint.x();
    double v1 = (entryPoint.z() > exitPoint.z()) ? exitPoint.y() : entryPoint.y();
    zfrac = modf( (fabs(u1)+fabs(v1))/2.7839542167, &zint);
    if (distXY > 2.0 * m_xPitch ) {
      z3Circ = 2.0 * (zfrac-0.5)*m_zPitch;
    } else {
      if ( z1 < 0 ) {
        z3Circ = - zfrac * m_zPitch; 
      } else {
        z3Circ = zfrac * m_zPitch; 
      }
    }

    sCircle(z1, x1, z2, x2, z3Circ, zCirc, uCirc, rCirc);
    
    // monolayer A
    int strawA = hitStrawA(uLow);
    double zStrawA = localZOfStraw(strawA);
    double uStep = uLow;
    while ( (uStep < uHigh) && strawA != 0 ) {
      uStep = localUOfStraw(strawA);
      double distCirc = 
        sqrt((zCirc-zStrawA)*(zCirc-zStrawA) + (uCirc-uStep)*(uCirc-uStep));
      double distTmp = fabs(distCirc-rCirc);
      int straw = strawA;
      double ambTmp = - (uStep-(x1+x2)/2.) * (distCirc-rCirc);

      if ( distTmp < m_cellRadius ) {
        if (ambTmp < 0.0 ) distTmp *= -1.0;
        OTChannelID tmpChan = OTChannelID(m_stationID, m_layerID, 
                                          m_quarterID, m_moduleID, straw);
        channels.push_back(tmpChan);
        driftDistances.push_back(distTmp);
      }
      strawA = nextRightStraw(straw);
    }

    // monolayer B
    int strawB = hitStrawB(uLow);
    double zStrawB = localZOfStraw(strawB);
    uStep = uLow;
    while ( (uStep < uHigh) && strawB != 0 ) {
      uStep = localUOfStraw(strawB);
      double distCirc = 
        sqrt((zCirc-zStrawB)*(zCirc-zStrawB) + (uCirc-uStep)*(uCirc-uStep));
      double distTmp = fabs(distCirc-rCirc);
      int straw = strawB;

      double ambTmp = - (uStep-(x1+x2)/2.) * (distCirc-rCirc);
      if ( distTmp < m_cellRadius ) {
        if (ambTmp < 0.0) distTmp *= -1.0;
        OTChannelID tmpChan = OTChannelID( m_stationID, m_layerID, 
                                           m_quarterID, m_moduleID, straw);
        channels.push_back(tmpChan);
        driftDistances.push_back(distTmp);
      }
      strawB = nextRightStraw(straw);
    }
  } // curling tracks
  
  return StatusCode::SUCCESS;
}

void DeOTModule::sCircle(const double z1, const double u1, const double z2, 
                         const double u2, const double z3c,
                         double& zc, double& uc, double& rc) const {
  
  double zw=(z1+z2)/2.;
  double uw=(u2+u1)/2.;
  
  zc=0.5*(z3c*z3c-zw*zw-(u1-uw)*(u1-uw))/(z3c-zw);
  uc=uw;
  rc=fabs(zc-z3c);
}

double DeOTModule::distanceToWire(const unsigned int aStraw, 
                                  const Gaudi::XYZPoint& aPoint, 
                                  const double tx, const double ty) const {
  // go to the local coordinate system
  Gaudi::XYZVector vec(tx, ty, 1.);
  Gaudi::XYZPoint localPoint = (this->geometry())->toLocal(aPoint);
  Gaudi::XYZVector localVec = (this->geometry())->toLocal(aPoint+vec) - localPoint;

  // calculate distance to the straw
  double z = this->localZOfStraw(aStraw);
  double u = localPoint.x() + localVec.x() * (z - localPoint.z());
  double cosU = 1./sqrt(1. + gsl_pow_2(localVec.x()/localVec.z()));
  double dist = (u - localUOfStraw(aStraw)) * cosU;
  
  return dist;
}

double DeOTModule::distanceAlongWire(const double xHit, 
                                     const double yHit) const { 
  Gaudi::XYZPoint globalPoint(xHit, yHit, 0);
  Gaudi::XYZPoint localPoint = (this->geometry())->toLocal(globalPoint);

  // For the upper modules of the station the readout is above.
  return ( this->topModule() ) ? 
    m_yHalfModule - localPoint.y() : m_yHalfModule + localPoint.y();
}

Gaudi::XYZPoint DeOTModule::centerOfStraw(const unsigned int straw) const {
  /// get the global coordinate of the middle of the channel
  Gaudi::XYZPoint localPoint(this->localUOfStraw(straw), 0., 
                             this->localZOfStraw(straw));
  return (this->geometry())->toGlobal(localPoint);
}

Gaudi::XYZPoint DeOTModule::centerOfModule() const {
  /// get the global coordinate of the middle of the module
  Gaudi::XYZPoint localPoint(0., 0., 0.);
  return (this->geometry())->toGlobal(localPoint);
}

double DeOTModule::z() const {
  /// Get the global z-coordinate of the module
  return centerOfModule().z();
}

/// Returns a Trajectory representing the wire identified by the LHCbID
/// The offset is zero for all OT Trajectories
/// FIXME: This needs to be corrected for the inefficient region
std::auto_ptr<LHCb::Trajectory> DeOTModule::trajectory( const OTChannelID& aChan,
                                                        const double /*offset*/ ) const {
  LineTraj* traj = 0;
  if( contains( aChan) == true ) {
    double halfSizeModule = m_yHalfModule;
    // Trajectory points from beamline towards readout.
    if( bottomModule() ) halfSizeModule *= -1.;
    const unsigned int straw = aChan.straw();
    Gaudi::XYZPoint lowerLocal( this->localUOfStraw(straw),
                                -halfSizeModule,
                                this->localZOfStraw(straw) );
    Gaudi::XYZPoint upperLocal( this->localUOfStraw(straw),
                                halfSizeModule,
                                this->localZOfStraw(straw) );
    Gaudi::XYZPoint lowerGlobal = (this->geometry())->toGlobal(lowerLocal);
    Gaudi::XYZPoint upperGlobal = (this->geometry())->toGlobal(upperLocal);
    traj = new LineTraj( lowerGlobal, upperGlobal );
  }
  else { throw GaudiException( "Failed to make trajectory!", "DeOTModule.cpp",
                               StatusCode::FAILURE ); 
  }

  std::auto_ptr<Trajectory> autoTraj(traj);
  
  return autoTraj;
}
