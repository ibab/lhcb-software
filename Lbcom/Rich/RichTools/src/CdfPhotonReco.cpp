// $Id: CdfPhotonReco.cpp,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "CdfPhotonReco.h"

// CLHEP
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/PhysicalConstants.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CdfPhotonReco
//
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  ToolFactory<CdfPhotonReco>          Factory ;
const        IToolFactory& CdfPhotonRecoFactory = Factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CdfPhotonReco::CdfPhotonReco( const std::string& type,
                              const std::string& name,
                              const IInterface* parent)
  : RichToolBase ( type, name, parent ) {

  declareInterface<IRichPhotonReconstruction>(this);

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
CdfPhotonReco::~CdfPhotonReco() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode CdfPhotonReco::initialize() {

  // Sets up various tools and services
  if ( !RichToolBase::initialize() ) return StatusCode::FAILURE;

  IDataProviderSvc * detSvc = svc<IDataProviderSvc>("DetectorDataSvc", true ); 

  RichX* rich1 = new Rich1(detSvc);
  if ( rich1 && rich1->statusOK() ) {
    m_rich.push_back( rich1 );
  } else {
    return Error( "Rich1 initialization failed!" );
  }

  RichX* rich2 = new Rich2(detSvc);
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
StatusCode CdfPhotonReco::finalize() {

  debug() << "Finalize" << endreq;

  // clean up Rich instances
  for ( RichX::PointerIterator rich = m_rich.begin();
        rich != m_rich.end();
        ++rich ) { if (*rich) delete *rich; }

  // release tools
  //if ( m_trueStates ) { toolSvc()->releaseTool( m_trueStates );
  //m_trueStates=0; }

  return RichToolBase::finalize();
}

//=============================================================================
// converters RichSmartID <-> HepPoint3D,
// return 1 on success, 0 if no position/id association could be found
//=============================================================================
StatusCode CdfPhotonReco::globalPosition ( const RichSmartID& pixelID,
                                           HepPoint3D& position ) const {

  unsigned int rich;
  StatusCode sc;
  if ( (rich = pixelID.rich()) < 2 ) {
    // Rich::left and Rich::top are equivalent, same for right and bottom
    if ( (sc=m_detector[rich][pixelID.panel()]->
          detectionPoint(pixelID,position)).isSuccess() ) return sc;
  }

  return StatusCode::FAILURE;
}


//=============================================================================
// Geometrical photon reconstruction. Solve the quartic equation for reflection
// of a straight line on a spherical mirror, using pixel global position
// return value: 1 on success, 0 on failure
//=============================================================================
StatusCode CdfPhotonReco::reconstructPhoton( const RichTrackSegment&
                                             segment,
                                             const HepPoint3D&
                                             pixelPosition,
                                             RichGeomPhoton&
                                             photon ) const {

  RichX* rich = m_rich[segment.rich()];
  if (rich) return rich->reconstructPhoton(segment, pixelPosition, photon);

  return 0;
}

//=============================================================================
// Geometrical photon reconstruction. Solve the quartic equation for reflection
// of a straight line on a spherical mirror, using pixel SmartID
// return value: 1 on success, 0 on failure
//=============================================================================
StatusCode CdfPhotonReco::reconstructPhoton(const RichTrackSegment&
                                            segment,
                                            const RichSmartID& pixelID,
                                            RichGeomPhoton& photon )
  const {

  HepPoint3D pixelPosition;
  if ( globalPosition(pixelID, pixelPosition).isFailure() ) return 0;

  return reconstructPhoton( segment, pixelPosition, photon);
}

