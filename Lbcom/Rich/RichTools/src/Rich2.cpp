// $Id: Rich2.cpp,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
// Include files

#include <string>

// Gaudi
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/MsgStream.h"

#include "RichDet/DeRich2.h"

// local
#include "Rich2.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Rich2
//
// 29/05/2002 :
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

// Standard constructor
Rich2::Rich2() : RichX() { }

// destructor
Rich2::~Rich2( ) { }

// Constructor using detector service
Rich2::Rich2(IDataProviderSvc* detSvc) : RichX( ) {

  const std::string & richName      =   DeRich2Location::Default;;
  const std::string & radiatorName  =   DeRichRadiatorLocation::CF4;
  const std::string pdPanelName[2]  = { DeRichHPDPanelLocation::Rich2Panel0,
                                        DeRichHPDPanelLocation::Rich2Panel1 };
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
  MsgStream msg(msgSvc, "DetRich2");

  m_statusOK = true;

  // Check Detector data service pointer
  if ( !detSvc ) {
    msg << MSG::ERROR << "Null Detector Data Service !" << endreq;
  }

  SmartDataPtr<DeRich2> rich2(detSvc, richName);
  if ( !rich2 ) {
    msg << MSG::ERROR << "Failed to locate DeRich2 at " << richName << endreq;
    m_statusOK = false;
  }

  // get the CF4 radiator
  std::vector<DeRichRadiator*> gasRadiator;
  SmartDataPtr<DeRichRadiator> deRadiator(detSvc, radiatorName);
  if (deRadiator) gasRadiator.push_back( (DeRichRadiator*) deRadiator );
  m_radiator[Rich::CF4] = new RichRadiator(gasRadiator, this, Rich::CF4);

  // get the reflectors:
  if ( rich2 ) {
    RichReflector::FlatMirrorPtrVector flatMirrors[10];
    RichReflector::SphMirrorPtrVector  sphMirrors[10];

    IDetectorElement::IDEContainer detelems = rich2->childIDetectorElements();
    std::string detName;
    std::string::size_type isFlat;
    std::string::size_type isSph;

    IDetectorElement::IDEContainer::iterator det_it;
    for (det_it = detelems.begin(); det_it != detelems.end(); det_it++) {
      detName = (*det_it)->name();

      isFlat = detName.find("Rich2FlatMirror");
      isSph  = detName.find("Rich2SphMirror");
      if (isFlat != std::string::npos) {

        SmartDataPtr<DeRichFlatMirror> fm (detSvc, detName);
        if (!fm) {
          msg << MSG::ERROR << "Failed to locate DeRichFlatMirror at " << detName << endreq;
          m_statusOK = false;
        } else {
          // decide on which side the segment belongs to
          if( fm->mirrorCentre().x() > 0.0 )
            flatMirrors[Rich::left].push_back( fm );
          else
            flatMirrors[Rich::right].push_back( fm );
        }

      } else if (isSph != std::string::npos) {

        SmartDataPtr<DeRichSphMirror> sm (detSvc, detName);
        if (!sm) {
          msg << MSG::ERROR << "Failed to locate DeRichSphMirror at " << detName << endreq;
          m_statusOK = false;
        }
        else {
          // decide on which side the segment belongs to
          if( sm->centreOfCurvature().x() > 0.0 )
            sphMirrors[Rich::left].push_back( sm );
          else
            sphMirrors[Rich::right].push_back( sm );
        }
      }
    }

    msg << MSG::DEBUG << "Found " << sphMirrors[Rich::left].size()
        << " left and " << sphMirrors[Rich::right].size()
        << " right spherical mirrors, and " << flatMirrors[Rich::left].size()
        << " left and " << flatMirrors[Rich::right].size()
        << " right flat mirrors" << endmsg;

    RichReflector ref =
      RichReflector(flatMirrors[Rich::left],sphMirrors[Rich::left],
                    rich2->nominalPlane(Rich::left),
                    rich2->nominalCentreOfCurvature(Rich::left),
                    rich2->sphMirrorRadius() );

    m_reflector.push_back( ref );

    m_reflector.
      push_back(RichReflector(flatMirrors[Rich::right],
                              sphMirrors[Rich::right],
                              rich2->nominalPlane(Rich::right),
                              rich2->nominalCentreOfCurvature(Rich::right),
                              rich2->sphMirrorRadius() ) );
  }

  // get the detectors aka PD panels:
  for ( int i=0; i<2; ++i ) {
    SmartDataPtr<DeRichHPDPanel> dePDPanel(detSvc, pdPanelName[i]);
    if (dePDPanel) {
      m_detector.push_back( RichDetector(dePDPanel) );
    } else {
      msg << MSG::ERROR << "Failed to locate DeRichHPDPanel at " << pdPanelName[i] << endreq;
      m_statusOK = false;
    }
  }

  m_detectorID = Rich::Rich2;
  msgSvc->release();
}


int Rich2::reconstructPhoton(const RichTrackSegment& segment,
                             const HepPoint3D& hitPosition,
                             RichGeomPhoton& photon) {

  if ( segment.radiator() != Rich::CF4 ) return 0;
  if ( hitPosition.z() < 8000.0) return 0;

  double fraction = 1;

  // Working objects
  HepPoint3D reflectionPointFlat;
  HepPoint3D emissionPoint;
  HepPoint3D reflectionPoint;
  RichSmartID smartID;

  HepPoint3D emission1 = segment.bestPoint( 0.01 );
  HepPoint3D emission2 = segment.bestPoint( 0.99 );
  HepPoint3D reflection1, reflection2;

  int ok1 =m_reflector[side(hitPosition)].reflectionPoint(emission1,
                                                          hitPosition,
                                                          reflection1,
                                                          reflectionPointFlat);

  int ok2 =m_reflector[side(hitPosition)].reflectionPoint(emission2,
                                                          hitPosition,
                                                          reflection2,
                                                          reflectionPointFlat);

  if ( ok1 && ok2 ) {
    // First reflection and hit point on same y side ?
    bool sameSide1 = ( reflection1.x() * hitPosition.x() > 0 );
    bool sameSide2 = ( reflection2.x() * hitPosition.x() > 0 );
    if ( sameSide1 && sameSide2 ) {
      emissionPoint = segment.bestPoint();
    } else if ( sameSide1 ) {
      fraction = fabs( reflection1.x() / (reflection1.x()-reflection2.x()) );
      emissionPoint = segment.bestPoint( fraction/2 );
    } else if ( sameSide2 ) {
      fraction = fabs( reflection2.x() / (reflection1.x()-reflection2.x()) );
      emissionPoint = segment.bestPoint( 1-fraction/2 );
    } else {
      //std::cout << "Rich2:Both hits opposite side to track hit" << std::endl;
      return 0;
    }
  } else {
    std::cout << "Rich2::reconstructPhoton Reflection Point Error" <<std::endl;
    return 0;
  }

  if ( !m_reflector[side(hitPosition)].reflectionPoint( emissionPoint,
                                                        hitPosition,
                                                        reflectionPoint,
                                                        reflectionPointFlat ) )
    return 0;

  // check that reflection point is on the same side
  if ( reflectionPoint.x() * hitPosition.x() < 0.0 ) return 0;

  // calculate angles
  HepVector3D direction = reflectionPoint - emissionPoint;
  double thetaCerenkov, phiCerenkov;
  segment.angleToDirection( direction, thetaCerenkov, phiCerenkov );

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

int Rich2::side(const HepPoint3D& position) const
{
  return ( position.x() > 0.0 ? 0 : 1 );
}
