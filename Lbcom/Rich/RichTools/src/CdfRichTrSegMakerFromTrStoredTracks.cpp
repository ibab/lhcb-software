// $Id: CdfRichTrSegMakerFromTrStoredTracks.cpp,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "CdfRichTrSegMakerFromTrStoredTracks.h"

// CLHEP
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/PhysicalConstants.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CdfRichTrSegMakerFromTrStoredTracks
//
// 14/01/2002 : Artur Barczyk
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  ToolFactory<CdfRichTrSegMakerFromTrStoredTracks>          Factory ;
const        IToolFactory& CdfRichTrSegMakerFromTrStoredTracksFactory = Factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CdfRichTrSegMakerFromTrStoredTracks::CdfRichTrSegMakerFromTrStoredTracks( const std::string& type,
                                                                          const std::string& name,
                                                                          const IInterface* parent)
  : RichToolBase ( type, name, parent ) {

  declareInterface<IRichTrSegMaker>(this);

  // tolerances on z positions
  m_zTolerance.push_back( 800*mm );
  m_zTolerance.push_back( 800*mm );
  m_zTolerance.push_back( 2000*mm );
  declareProperty( "StateZTolerance", m_zTolerance );

  // Nominal z positions of states at RICHes
  // Should get from XML instead of hardcode ?
  m_nomZstates.push_back( 99.0*cm   );
  m_nomZstates.push_back( 216.5*cm  );
  m_nomZstates.push_back( 945.0*cm  );
  m_nomZstates.push_back( 1190.0*cm );

}

//=============================================================================
// Destructor
//=============================================================================
CdfRichTrSegMakerFromTrStoredTracks::~CdfRichTrSegMakerFromTrStoredTracks() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode CdfRichTrSegMakerFromTrStoredTracks::initialize() {

  // Sets up various tools and services
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  RichX* rich1 = new Rich1(detSvc());
  if ( rich1 && rich1->statusOK() ) {
    m_rich.push_back( rich1 );
  } else { return Error( "Rich1 initialization failed!" ); }

  RichX* rich2 = new Rich2(detSvc());
  if ( rich2 && rich2->statusOK() ) {
    m_rich.push_back( rich2 );
  } else { return Error( "Rich2 initialization failed!" ); }

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

  // Informational Printout
  debug() << " XML DataBase               = " << getenv("XMLDDDBROOT") << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CdfRichTrSegMakerFromTrStoredTracks::finalize() {

  debug() << "Finalize" << endreq;

  // clean up Rich instances
  for ( RichX::PointerIterator rich = m_rich.begin();
        rich != m_rich.end();
        ++rich ) { if (*rich) delete *rich; }

  // Execute base class method
  return RichToolBase::finalize();
}


//=============================================================================
// Constructs the track segments for a given TrStoredTrack
//=============================================================================
int CdfRichTrSegMakerFromTrStoredTracks::constructSegments( const ContainedObject * obj,
                                                            std::vector<RichTrackSegment>& segments )
  const {

  // Try to cast input data to required type for this implementation
  const TrStoredTrack * track = dynamic_cast<const TrStoredTrack *>(obj);
  if ( !track ) {
    warning() << "Input data object is not of type TrStoredTrack" << endreq;
    return 0;
  }
  if ( msgLevel(MSG::VERBOSE) ) {
    verbose() << "Analysing TrStoredTrack " << track->key() << endreq;
  }

  // loop over radiators
  for ( RichRadiator::ConstPointerIterator radiator = m_radiator.begin();
        radiator != m_radiator.end();
        ++radiator ) {
    const Rich::RadiatorType rad = (*radiator)->id();

    if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << " Considering radiator " << rad << endreq;
    }

    // choose appropriate z positions for initial track states for this radiator
    const double zStart = ( Rich::CF4 == rad ? m_nomZstates[2] : m_nomZstates[0] );
    const double zEnd   = ( Rich::CF4 == rad ? m_nomZstates[3] : m_nomZstates[1] );

    // state1 and state2 give us a first approximation of the trajectory
    SmartRef<TrState> state1 = track->closestState( zStart );
    TrStateP* pState1 = dynamic_cast<TrStateP*>( (TrState*)state1 );
    if ( !pState1 || fabs(zStart - pState1->z()) > m_zTolerance[rad] ) {
      continue;
    }
    SmartRef<TrState> state2 = track->closestState( zEnd );
    TrStateP* pState2 = dynamic_cast<TrStateP*>( (TrState*)state2 );
    if ( !pState2 || fabs(zEnd - pState2->z()) > m_zTolerance[rad] ) {
      continue;
    }

    if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << "  Found appropriate initial start/end TrStates" << endreq
                << "   Entry : " << HepPoint3D(pState1->x(),pState1->y(),pState1->z()) << endreq
                << "   Exit  : " << HepPoint3D(pState2->x(),pState2->y(),pState2->z()) << endreq;
    }

    // use state closest to the entry point in radiator
    HepPoint3D entryPoint1, exitPoint1, entryStatePoint;
    HepVector3D entryStateMomentum( 0, 0, 1 );
    TrStateP* entryPState = NULL;
    bool entryStateOK = false;
    HepPoint3D firstPoint ( pState1->x(), pState1->y(), pState1->z() );
    HepVector3D firstDir  ( pState1->tx(), pState1->ty(), 1 );
    if ( (*radiator)->nextIntersectionPoint( firstPoint,
                                             firstDir,
                                             entryPoint1 ) ) {

      SmartRef<TrState> eState = track->closestState(entryPoint1.z());
      entryPState = dynamic_cast<TrStateP*>( (TrState*)eState );
      if ( entryPState &&
           fabs(entryPState->z() - entryPoint1.z()) < m_zTolerance[rad] ) {
        entryStatePoint.setX( entryPState->x() );
        entryStatePoint.setY( entryPState->y() );
        entryStatePoint.setZ( entryPState->z() );
        entryStateMomentum.setX( entryPState->tx() );
        entryStateMomentum.setY( entryPState->ty() );
        if ( (*radiator)->intersectionPoints( entryStatePoint,
                                              entryStateMomentum,
                                              entryPoint1,
                                              exitPoint1) ) {
          entryStateOK = true;
        }
      }

    }

    // If gas radiator try and use exit state to get exit point more precisely
    bool exitStateOK = false;
    HepPoint3D entryPoint2, exitPoint2, exitStatePoint;
    HepVector3D exitStateMomentum( 0, 0, 1 );
    TrStateP* exitPState = NULL;
    if ( rad != Rich::Aerogel ) {

      HepPoint3D lastPoint  ( pState2->x(), pState2->y(), pState2->z() );
      HepVector3D lastDir   ( -pState2->tx(), -pState2->ty(), -1 );
      if ( (*radiator)->nextIntersectionPoint(lastPoint,
                                              lastDir,
                                              entryPoint2) ) {

        SmartRef<TrState> eState = track->closestState(entryPoint2.z());
        exitPState = dynamic_cast<TrStateP*>( (TrState*)eState );
        if ( exitPState &&
             fabs(exitPState->z() - entryPoint2.z()) < m_zTolerance[rad] ) {
          exitStatePoint.setX( exitPState->x() );
          exitStatePoint.setY( exitPState->y() );
          exitStatePoint.setZ( exitPState->z() );
          exitStateMomentum.setX( exitPState->tx() );
          exitStateMomentum.setY( exitPState->ty() );
          if ( (*radiator)->intersectionPoints(exitStatePoint,
                                               exitStateMomentum,
                                               entryPoint2,
                                               exitPoint2) ) {
            exitStateOK = true;
            // make sure state point is inside spherical mirror envelope
            exitStatePoint = entryPoint2;
          }

        }

      }
    }

    // select best information
    HepPoint3D entryPoint, exitPoint;
    if ( entryStateOK && exitStateOK ) {
      verbose() << "  Both states OK" << endreq;
      entryPoint = entryPoint1;
      exitPoint = exitPoint2;
    } else if ( entryStateOK ) {
      verbose() << "  Entry state only OK" << endreq;
      entryPoint = entryPoint1;
      exitPoint = exitPoint1;
      // set exit data to entry data
      exitStatePoint = entryStatePoint;
      exitPState = entryPState;
      exitStateMomentum = entryStateMomentum;
    } else if ( exitStateOK ) {
      verbose() << "  Exit state only OK" << endreq;
      entryPoint = entryPoint2;
      exitPoint = exitPoint2;
      // set entry data to exit data
      entryStatePoint = exitStatePoint;
      entryPState = exitPState;
      entryStateMomentum = exitStateMomentum;
    } else {
      verbose() << "  Both states failed" << endreq;
      continue;
    }

    // a special hack for the C4F10 - since the aerogel volume
    // is placed INSIDE the C4F10, the default entry point is wrong.
    if ( Rich::C4F10 == rad ) fixC4F10EntryPoint( entryPoint,
                                                  entryStatePoint,
                                                  entryStateMomentum );

    // check for intersection with spherical mirror for gas radiators
    // and if need be correct exit point accordingly
    if ( rad != Rich::Aerogel ) correctRadExitMirror( *radiator,
                                                      exitPoint,
                                                      exitStatePoint,
                                                      exitStateMomentum );

    // Set magnitude of momentum vectors + make error descriptors
    RichTrackSegment::StateErrors * entryErrs = 0;
    RichTrackSegment::StateErrors * exitErrs  = 0;
    if ( entryStateOK && exitStateOK ) {
      entryStateMomentum.setMag( entryPState->p() );
      exitStateMomentum.setMag ( exitPState->p() );
      entryErrs = new RichTrackSegment::StateErrors( entryPState );
      exitErrs  = new RichTrackSegment::StateErrors( exitPState  );
    } else if ( entryStateOK ) {
      entryStateMomentum.setMag( entryPState->p() );
      exitStateMomentum.setMag ( entryPState->p() );
      entryErrs = new RichTrackSegment::StateErrors( entryPState );
      exitErrs  = new RichTrackSegment::StateErrors( entryPState );
    } else if ( exitStateOK ) {
      entryStateMomentum.setMag( exitPState->p() );
      exitStateMomentum.setMag ( exitPState->p() );
      entryErrs = new RichTrackSegment::StateErrors( exitPState );
      exitErrs  = new RichTrackSegment::StateErrors( exitPState );
    }

    // print out final points
    if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << "  Found final points :-" << endreq
                << "   Entry : Pnt=" << entryPoint << " Mom=" << entryStateMomentum
                << " Ptot=" << entryStateMomentum.mag() << endreq
                << "   Exit  : Pnt=" << exitPoint  << " Mom=" << exitStateMomentum
                << " Ptot=" << exitStateMomentum.mag() << endreq;
    }

    // Using this information, make radiator segment
    // assuming straight line between entry and exit
    segments.push_back( RichTrackSegment( RichTrackSegment::UseAllStateVectors(),
                                          entryPoint, entryStateMomentum,
                                          exitPoint, exitStateMomentum,
                                          rad, (*radiator)->rich()->detectorID(),
                                          *entryErrs, *exitErrs ) );

    // cleanup
    delete exitErrs;
    delete entryErrs;

  } // end loop over radiators

  // return value is number of segments formed
  return segments.size();
}

//============================================================================
// fixup C4F10 entry point
void CdfRichTrSegMakerFromTrStoredTracks::fixC4F10EntryPoint( HepPoint3D & entryPoint,
                                                              const HepPoint3D & entryStatePoint,
                                                              const HepVector3D & entryStateMomentum
                                                              ) const {

  RichRadiator * aerogel = m_radiator[Rich::Aerogel];

  HepPoint3D dummyPoint, aerogelExitPoint;
  if ( aerogel->intersectionPoints(entryStatePoint,
                                   entryStateMomentum,
                                   dummyPoint,
                                   aerogelExitPoint) ) {
    if ( aerogelExitPoint.z() > entryPoint.z() ) {
      entryPoint = aerogelExitPoint;
      verbose() << "   Correcting C4F10 entry point" << endreq
                << "      --> Moved point to " << entryPoint << endreq;
    }
  }

}

//============================================================================
void CdfRichTrSegMakerFromTrStoredTracks::correctRadExitMirror( RichRadiator * radiator,
                                                                HepPoint3D & exitPoint,
                                                                const HepPoint3D & exitStatePoint,
                                                                const HepVector3D & exitStateMomentum
                                                                ) const {

  RichX * thisRich = radiator->rich();
  int thisSide = thisRich->side(exitPoint);
  RichReflector* reflector = thisRich->reflector((Rich::Side)thisSide);

  HepPoint3D intersection;
  if ( reflector->intersectNominalSphMirror(exitStatePoint,
                                            exitStateMomentum,
                                            intersection) ) {
    if ( radiator->isInside(intersection) ) {
      exitPoint = intersection;
      verbose() << "   Correcting exit point to spherical mirror" << endreq
                << "      --> Moved point to " << exitPoint << endreq;
      return;
    }
  }

  verbose() << "   Failed to correct exit state to spherical mirror" << endreq;
}

