// $Id: CdfRayTracing.cpp,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "CdfRayTracing.h"

// CLHEP
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/PhysicalConstants.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CdfRayTracing
//
// 14/01/2002 : Artur Barczyk
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  ToolFactory<CdfRayTracing>          Factory ;
const        IToolFactory& CdfRayTracingFactory = Factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CdfRayTracing::CdfRayTracing( const std::string& type,
                              const std::string& name,
                              const IInterface* parent)
  : RichToolBase ( type, name, parent ) {

  declareInterface<IRichRayTracing>(this);

  // tolerances on z positions
  m_zTolerance.push_back( 800*mm );
  m_zTolerance.push_back( 800*mm );
  m_zTolerance.push_back( 2000*mm );
  declareProperty( "StateZTolerance", m_zTolerance );

  // Nominal z positions of states at RICHes
  m_nomZstates.push_back( 99.0*cm   );
  m_nomZstates.push_back( 216.5*cm  );
  m_nomZstates.push_back( 945.0*cm  );
  m_nomZstates.push_back( 1190.0*cm );

}

//=============================================================================
// Destructor
//=============================================================================
CdfRayTracing::~CdfRayTracing() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode CdfRayTracing::initialize() {

  // Sets up various tools and services
  if ( !RichToolBase::initialize() ) return StatusCode::FAILURE;

  RichX* rich1 = new Rich1(detSvc());
  if ( rich1 && rich1->statusOK() ) {
    m_rich.push_back( rich1 );
  } else {
    return Error( "Rich1 initialization failed!" );
  }

  RichX* rich2 = new Rich2(detSvc());
  if ( rich2 && rich2->statusOK() ) {
    m_rich.push_back( rich2 );
  } else {
    return Error( "Rich2 initialization failed!" );
  }

  // copy the pointers to the subelements into local stores:
  // note that any missing element is filled in as NULL!
  // m_radiator
  m_radiator.push_back( rich1->radiator(Rich::Aerogel) );
  m_radiator.push_back( rich1->radiator(Rich::C4F10) );
  m_radiator.push_back( rich2->radiator(Rich::CF4) );

  // m_reflector
  m_reflector[Rich::Rich1].push_back( rich1->reflector(Rich::top) );
  m_reflector[Rich::Rich1].push_back( rich1->reflector(Rich::bottom) );
  m_reflector[Rich::Rich2].push_back( rich2->reflector(Rich::left) );
  m_reflector[Rich::Rich2].push_back( rich2->reflector(Rich::right) );

  // m_detector
  m_detector[Rich::Rich1].push_back( rich1->detector(Rich::top) );
  m_detector[Rich::Rich1].push_back( rich1->detector(Rich::bottom) );
  m_detector[Rich::Rich2].push_back( rich2->detector(Rich::left) );
  m_detector[Rich::Rich2].push_back( rich2->detector(Rich::right) );

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CdfRayTracing::finalize() {

  debug() << "Finalize" << endreq;

  // clean up Rich instances
  for ( RichX::PointerIterator rich = m_rich.begin();
        rich != m_rich.end();
        ++rich ) { if (*rich) delete *rich; }

  return RichToolBase::finalize();
}


//=============================================================================
// reflect the trajectory on the mirror, and determine the position where
// it hits the detector plane,
// take into account the geometrical boundaries of mirrors and detector
//=============================================================================
StatusCode CdfRayTracing::traceToDetector ( Rich::DetectorType detectorID,
                                            const HepPoint3D& point,
                                            const HepVector3D& direction,
                                            RichGeomPhoton& photon,
                                            DeRichHPDPanel::traceMode mode)const
{

  HepPoint3D tmpPosition = point;
  HepVector3D tmpDirection = direction;

  int side = m_rich[detectorID]->side(point);

  // protection agains not (yet) implemented reflectors...
  if (m_reflector[detectorID][side] == NULL) return 0;

  // Spherical mirror reflection
  if ( !m_reflector[detectorID][side]->reflectSph( tmpPosition,
                                                   tmpDirection) ) return 0;
  photon.setSphMirReflectionPoint( tmpPosition );

  // flat mirror reflection
  if ( !m_reflector[detectorID][side]->reflectFlat( tmpPosition,
                                                    tmpDirection) ) return 0;
  photon.setFlatMirReflectionPoint( tmpPosition );

  HepPoint3D hitPosition;
  RichSmartID smartID;
  StatusCode sc =
    (m_detector[detectorID])[side]->PDWindowPoint(tmpPosition, tmpDirection,
                                                  hitPosition, smartID, mode);

  // Set detector and side info in RichSmartID
  smartID.setPanel( side );
  smartID.setRich( (int)detectorID );

  // Set RichGeomPhoton
  photon.setSmartID( smartID );
  photon.setEmissionPoint( point );
  photon.setDetectionPoint( hitPosition );

  return ( sc == StatusCode::SUCCESS ? 1 : 0 );
}

//============================================================================
StatusCode CdfRayTracing::intersectPDPanel ( Rich::DetectorType detectorID,
                                             const HepPoint3D& point,
                                             const HepVector3D& direction,
                                             RichGeomPhoton& photon ) const {

  int side = m_rich[detectorID]->side(point);

  HepPoint3D hitPosition;
  RichSmartID smartID;
  StatusCode sc = m_detector[detectorID][side]->PDWindowPoint(point,
                                                              direction,
                                                              hitPosition,
                                                              smartID);
  // Set detector and side info in RichSmartID
  smartID.setPanel( side );
  smartID.setRich( (int)detectorID );

  // Set RichGeomPhoton
  photon.setSmartID( smartID );
  photon.setEmissionPoint( point );
  photon.setDetectionPoint( hitPosition );

  return (sc == StatusCode::SUCCESS) ? 1 : 0;
}

//=============================================================================
// reflect the trajectory on the mirror, and determine the position where
// it hits the detector plane,
// geometrical boundaries checking depends on the traceMode variable
//=============================================================================
StatusCode
CdfRayTracing::traceToDetectorWithoutEff(Rich::DetectorType detectorID,
                                         const HepPoint3D& point,
                                         const HepVector3D& direction,
                                         HepPoint3D& hitPosition,
                                         DeRichHPDPanel::traceMode mode)const {

  HepPoint3D tmpPosition = point;
  HepVector3D tmpDirection = direction;

  int side = m_rich[detectorID]->side(point);

  if ( !m_reflector[detectorID][side]->reflect(tmpPosition, tmpDirection) )
    return 0;


  if ( !m_detector[detectorID][side]->detPlanePoint(tmpPosition, tmpDirection,
                                                    hitPosition, mode ) )
    return 0;

  return 1;
}



StatusCode CdfRayTracing::intersectPlane ( const HepPoint3D&,
                                           const HepVector3D&,
                                           const HepPlane3D&,
                                           HepPoint3D&  ) const 
{
  return Error("**** This function is for compatibiity reasons and it does not perform any task ****");
}

StatusCode CdfRayTracing::reflectSpherical ( HepPoint3D&,
                                             HepVector3D&,
                                             const HepPoint3D&,
                                             double ) const 
{
  return Error( "**** This function is for compatibiity reasons and it does not perform any task ****");
}
