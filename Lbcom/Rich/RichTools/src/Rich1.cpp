// $Id: Rich1.cpp,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
// Include files

#include <string>

// Gaudi
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/MsgStream.h"

#include "RichDet/DeRich1.h"

// local
#include "Rich1.h"

#include "RichKernel/IRichRefractiveIndex.h"
#include "RichKernel/IRichToolRegistry.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Rich1
//
// 29/05/2002 :
//-----------------------------------------------------------------------------

// Standard constructor
Rich1::Rich1( ) 
  : RichX(),
    m_refIndex (0),
    m_toolSvc  (0) { }

// destructor
Rich1::~Rich1( ) 
{
  if ( m_toolSvc && m_refIndex ) { m_toolSvc->releaseTool( m_refIndex ); m_refIndex=0; }
  if ( m_toolSvc ) { m_toolSvc->release(); m_toolSvc=0; }
}

// Constructor using detector service
Rich1::Rich1( IDataProviderSvc* detSvc ) 
  : RichX( ),
    m_refIndex (0),
    m_toolSvc  (0)
{

  const std::string & richName      =   DeRich1Location::Default;
  const std::string aerogelName[]   = { DeRichRadiatorLocation::AerogelQ0,
                                        DeRichRadiatorLocation::AerogelQ1,
                                        DeRichRadiatorLocation::AerogelQ2,
                                        DeRichRadiatorLocation::AerogelQ3 };
  const std::string & gasName       =   DeRichRadiatorLocation::C4F10;
  const std::string pdPanelName[2]  = { DeRichHPDPanelLocation::Rich1Panel0,
                                        DeRichHPDPanelLocation::Rich1Panel1 };

  m_statusOK = true;

  //  locate the message service
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  if( 0 == svcLoc )
    { throw GaudiException("DetRich1::ISvcLocator* points to NULL!",
                           "*RichException*" ,
                           StatusCode::FAILURE  ); }
  IMessageSvc* msgSvc = 0;
  StatusCode sc = svcLoc->service( "MessageSvc" , msgSvc, true);
  if( sc.isFailure() || 0 == msgSvc )
    { throw GaudiException("DetRich1::Could not locate MessageSvc",
                           "*RichException*" ,
                           StatusCode::FAILURE); }
  MsgStream msg(msgSvc, "DetRich1");

  // Get tool service
  if ( !svcLoc->service( "ToolSvc", m_toolSvc, true ) ) {
    throw GaudiException("DetRich1::Could not locate ToolSvc",
                         "*RichException*", StatusCode::FAILURE);
  }

  // Get the refractive index tool
  IRichToolRegistry * toolReg;
  m_toolSvc->retrieveTool( "RichToolRegistry", toolReg );
  const std::string nickname = "RichRefractiveIndex";
  m_toolSvc->retrieveTool(toolReg->toolType(nickname),nickname,m_refIndex);

  // Check Detector data service pointer
  if ( !detSvc ) {
    msg << MSG::ERROR << "Null Detector Data Service !" << endreq;
  }

  // Get the XML RICH object
  SmartDataPtr<DeRich1> rich1 (detSvc, richName);
  if ( !rich1 ) {
    msg << MSG::ERROR << "Failed to locate DeRich1 at " << richName << endreq;
    m_statusOK = false;
  }

  // Get the Rich1 aerogel radiators
  std::vector<DeRichRadiator*> aerogelQuadrants;
  {for ( int i = 0; i<4; ++i ) {
    SmartDataPtr<DeRichRadiator> deRadiator(detSvc, aerogelName[i]);
    if (deRadiator) aerogelQuadrants.push_back( (DeRichRadiator*) deRadiator );
  }} // Fix scope for w2k
  m_radiator[Rich::Aerogel] = new RichRadiator(aerogelQuadrants,
                                               this, Rich::Aerogel);

  // get the Rich1 gas radiators
  std::vector<DeRichRadiator*> gasRadiator;
  SmartDataPtr<DeRichRadiator> deGasRadiator(detSvc, gasName);
  if (deGasRadiator) gasRadiator.push_back( (DeRichRadiator*) deGasRadiator );
  m_radiator[Rich::C4F10] = new RichRadiator(gasRadiator, this, Rich::C4F10);

  // get the reflectors:
  if ( rich1 ) {

    RichReflector::FlatMirrorPtrVector flatMirrors[2];
    RichReflector::SphMirrorPtrVector  sphMirrors[2];

    IDetectorElement::IDEContainer detelems = rich1->childIDetectorElements();
    std::string detName;
    std::string::size_type isFlat;
    std::string::size_type isSph;

    IDetectorElement::IDEContainer::iterator det_it;
    for (det_it = detelems.begin(); det_it != detelems.end(); det_it++) {
      detName = (*det_it)->name();

      isFlat = detName.find("Rich1Mirror2");
      isSph  = detName.find("Rich1Mirror1");
      if (isFlat != std::string::npos) {

        SmartDataPtr<DeRichFlatMirror> fm (detSvc, detName);
        if ( !fm ) {
          msg << MSG::ERROR << "Failed to locate DeRichFlatMirror at " << detName << endreq;
          m_statusOK = false;
        } else {
          // decide on which side the segment belongs to
          if( fm->mirrorCentre().y() > 0.0 )
            flatMirrors[Rich::top].push_back( fm );
          else
            flatMirrors[Rich::bottom].push_back( fm );
        }

      } else if (isSph != std::string::npos) {

        SmartDataPtr<DeRichSphMirror> sm (detSvc, detName);
        if (!sm) {
          msg << MSG::ERROR << "Failed to locate DeRichSphMirror at " << detName << endreq;
          m_statusOK = false;
        }
        else {
          // decide on which side the segment belongs to
          if ( sm->centreOfCurvature().y() > 0.0 )
            sphMirrors[Rich::top].push_back( sm );
          else
            sphMirrors[Rich::bottom].push_back( sm );

        }

      }
    }

    msg << MSG::DEBUG << "Found " << sphMirrors[Rich::top].size()
        << " top and " << sphMirrors[Rich::bottom].size()
        << " bottom spherical mirrors, and " << flatMirrors[Rich::top].size()
        << " top and " << flatMirrors[Rich::bottom].size()
        << " bottom flat mirrors" << endmsg;

    RichReflector ref =
      RichReflector(flatMirrors[Rich::top],sphMirrors[Rich::top],
                    rich1->nominalPlane(Rich::top),
                    rich1->nominalCentreOfCurvature(Rich::top),
                    rich1->sphMirrorRadius() );

    m_reflector.push_back( ref );

    m_reflector.push_back(RichReflector
                          (flatMirrors[Rich::bottom],
                           sphMirrors[Rich::bottom],
                           rich1->nominalPlane(Rich::bottom),
                           rich1->nominalCentreOfCurvature(Rich::bottom),
                           rich1->sphMirrorRadius() ) );
  }

  // get the detectors
  for (int i=0; i<2; ++i) {
    SmartDataPtr<DeRichHPDPanel> dePDPanel(detSvc, pdPanelName[i]);
    if (dePDPanel) {
      m_detector.push_back( RichDetector(dePDPanel) );
    } else {
      msg << MSG::ERROR << "Failed to locate DeRichHPDPanel at " << pdPanelName[i] << endreq;
      m_statusOK = false;
    }
  }

  m_detectorID = Rich::Rich1;

  // release tools and services
  m_toolSvc->releaseTool( toolReg );
  msgSvc->release();

}


//============================================================================
int Rich1::reconstructPhoton( const RichTrackSegment& segment,
                              const HepPoint3D& hitPosition,
                              RichGeomPhoton& photon ) {

  if ( hitPosition.z() > 3000.0 ) return 0;

  double fraction = 1;

  // Working objects
  HepPoint3D reflectionPointFlat;
  HepPoint3D emissionPoint;
  HepPoint3D reflectionPoint;
  RichSmartID smartID;

  if ( segment.radiator() == Rich::Aerogel ) {

    // thin radiator, assume full track segment is seen by pixel
    emissionPoint = segment.bestPoint();

  } else if ( segment.radiator() == Rich::C4F10 ) {
    // rich1 gas radiator

    HepPoint3D emission1 = segment.bestPoint( 0.01 );
    HepPoint3D emission2 = segment.bestPoint( 0.99 );
    HepPoint3D reflection1, reflection2;
    HepPoint3D reflectionPointFlat1, reflectionPointFlat2;

    const int ok1 = m_reflector[side(hitPosition)].
      reflectionPoint(emission1,hitPosition,reflection1,reflectionPointFlat1);

    const int ok2 = m_reflector[side(hitPosition)].
      reflectionPoint(emission2,hitPosition,reflection2,reflectionPointFlat2);

    if ( ok1 && ok2 ) {
      // First reflection and hit point on same y side ?
      bool sameSide1 = ( reflection1.y() * hitPosition.y() > 0 );
      bool sameSide2 = ( reflection2.y() * hitPosition.y() > 0 );
      if ( sameSide1 && sameSide2 ) {
        emissionPoint = segment.bestPoint();
      } else if ( sameSide1 ) {
        fraction = fabs( reflection1.y() / (reflection1.y()-reflection2.y()) );
        emissionPoint = segment.bestPoint( fraction/2 );
      } else if ( sameSide2 ) {
        fraction = fabs( reflection2.y() / (reflection1.y()-reflection2.y()) );
        emissionPoint = segment.bestPoint( 1-fraction/2 );
      } else {
        //std::cout << "Rich1:Both hits opposite side to track hit"<<std::endl;
        return 0;
      }
    } else {
      std::cout <<"Rich1::reconstructPhoton Reflection Point Error"<<std::endl;
      return 0;
    }

  } else {
    // invalid radiator
    return 0;
  }

  if ( !m_reflector[side(hitPosition)].reflectionPoint( emissionPoint,
                                                        hitPosition,
                                                        reflectionPoint,
                                                        reflectionPointFlat) )
    return 0;

  // check that reflection point is on the same side
  if ( reflectionPoint.y() * hitPosition.y() < 0.0 ) return 0;

  // calculates angles
  HepVector3D photonDir = reflectionPoint - emissionPoint;
  double thetaCerenkov, phiCerenkov;
  segment.angleToDirection( photonDir, thetaCerenkov, phiCerenkov );

  // Correct theta Cerenkov for refraction at exit of aerogel
  if ( Rich::Aerogel == segment.radiator() ) {
    HepVector3D newDir;
    photonDir.setMag(1);
    const double refAero  = m_refIndex->refractiveIndex( Rich::Aerogel, segment.avPhotonEnergy() );
    const double refc4f10 = m_refIndex->refractiveIndex( Rich::C4F10,   segment.avPhotonEnergy() );
    const double RratioSq = (refAero*refAero)/(refc4f10*refc4f10);
    newDir.setZ( sqrt(1. - (1. - photonDir.z()*photonDir.z())/RratioSq) );
    const double R = photonDir.y()/photonDir.x();
    newDir.setX( sqrt( (1.-newDir.z()*newDir.z())/(1. + R*R) ) );
    if ( photonDir.x() < 0 ) newDir.setX( -newDir.x() );
    newDir.setY( R*newDir.x() );
    const double ctc = newDir.dot( segment.bestMomentum().unit() );
    thetaCerenkov = ( ctc>1 ? 0 : acos(ctc) );
  }

  // Set photon parameters
  photon.setCherenkovTheta( thetaCerenkov );
  photon.setCherenkovPhi( phiCerenkov );
  photon.setActiveSegmentFraction( fraction );
  photon.setDetectionPoint( hitPosition );
  photon.setFlatMirReflectionPoint( reflectionPointFlat );
  photon.setEmissionPoint( emissionPoint );
  photon.setSphMirReflectionPoint(reflectionPoint);

  // for later:
  //  photon.setRawMirrorNumber(int mirrorNumber);
  //  photon.setSphMirrorNum( int num );
  //  photon.setFlatMirrorNum( int num );
  //  photon.setMirrorNumValid (bool);

  return 1;
}


