
//---------------------------------------------------------------------------------
/** @file RichTrSegMakerFromTrStoredTracks.cpp
 *
 * Implementation file for class : RichTrSegMakerFromTrStoredTracks
 *
 * CVS Log :-
 * $Id: RichTrSegMakerFromTrStoredTracks.cpp,v 1.11 2005-02-24 11:56:01 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//---------------------------------------------------------------------------------

// local
#include "RichTrSegMakerFromTrStoredTracks.h"

// Declaration of the Algorithm Factory
static const  ToolFactory<RichTrSegMakerFromTrStoredTracks>          Factory ;
const        IToolFactory& RichTrSegMakerFromTrStoredTracksFactory = Factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichTrSegMakerFromTrStoredTracks::
RichTrSegMakerFromTrStoredTracks( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent)
  : RichToolBase         ( type, name, parent ),
    m_rayTracing         ( 0 ),
    m_nomSphMirrorRadius ( Rich::NRiches,        0    ),
    m_zTolerance         ( Rich::NRadiatorTypes, 0    ),
    m_nomZstates         ( 2*Rich::NRiches,      0    ),
    m_mirrShift          ( Rich::NRiches,        0    ),
    m_minStateDiff       ( Rich::NRadiatorTypes, 0    ),
    m_trExt1             ( 0 ),
    m_trExt2             ( 0 ),
    m_Ext1               ( "TrHerabExtrapolator"      ),
    m_Ext2               ( "TrParabolicExtrapolator"  ),
    m_usedRads           ( Rich::NRadiatorTypes, true ),
    m_extrapFromRef      ( false                      )
{

  // the interface
  declareInterface<IRichTrSegMaker>(this);

  // job options

  declareProperty( "PrimaryTrackExtrapolator", m_Ext1     );
  declareProperty( "BackupTrackExtrapolator",  m_Ext2     );
  declareProperty( "UseRadiators",             m_usedRads );
  declareProperty( "ExtrapolateFromReference", m_extrapFromRef );

  // Nominal z positions of states at RICHes
  m_nomZstates[0] = 99.0*cm;    // Place to look for Rich1 entry state
  m_nomZstates[1] = 216.5*cm;   // Place to look for Rich1 exit state
  m_nomZstates[2] = 945.0*cm;   // Place to look for Rich2 entry state
  m_nomZstates[3] = 1190.0*cm;  // Place to look for Rich2 exit state
  declareProperty( "NominalStateZ", m_nomZstates );

  // tolerances on z positions
  m_zTolerance[Rich::Aerogel] = 800*mm;
  m_zTolerance[Rich::C4F10]   = 800*mm;
  m_zTolerance[Rich::CF4]     = 2000*mm;
  declareProperty( "ZTolerances", m_zTolerance );

  // sanity checks on state information
  m_minStateDiff[Rich::Aerogel] = 1*mm;
  m_minStateDiff[Rich::C4F10]   = 25*mm;
  m_minStateDiff[Rich::CF4]     = 50*mm;
  declareProperty( "ZSanityChecks", m_minStateDiff );

  // shifts for mirror correction
  m_mirrShift[Rich::Rich1] = 35*cm;
  m_mirrShift[Rich::Rich2] = 150*cm;
  declareProperty( "MirrorShiftCorr", m_mirrShift );

}

//=============================================================================
// Destructor
//=============================================================================
RichTrSegMakerFromTrStoredTracks::~RichTrSegMakerFromTrStoredTracks() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode RichTrSegMakerFromTrStoredTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Get the track state extrapolator for the RICH
  //acquireTool( "RichTrStateExtrapolator", m_trExt );
  m_trExt1 = tool<ITrExtrapolator>( m_Ext1 );
  m_trExt2 = tool<ITrExtrapolator>( m_Ext2 );

  // Get the ray tracing tool
  acquireTool( "RichRayTracing", m_rayTracing );
  
  // get Detector elements for RICH1 and RICH2
  m_rich[Rich::Rich1] = getDet<DeRich>( DeRichLocation::Rich1 );
  m_rich[Rich::Rich2] = getDet<DeRich>( DeRichLocation::Rich2 );

  // load the nominal centre of curvature and radius
  m_nominalCoC[Rich::Rich1][Rich::top] = m_rich[Rich::Rich1]->
    nominalCentreOfCurvature(Rich::top);
  m_nominalCoC[Rich::Rich1][Rich::bottom] = m_rich[Rich::Rich1]->
    nominalCentreOfCurvature(Rich::bottom);
  m_nominalCoC[Rich::Rich2][Rich::left] = m_rich[Rich::Rich2]->
    nominalCentreOfCurvature(Rich::left);
  m_nominalCoC[Rich::Rich2][Rich::right] = m_rich[Rich::Rich2]->
    nominalCentreOfCurvature(Rich::right);
  m_nomSphMirrorRadius[Rich::Rich1] = m_rich[Rich::Rich1]->sphMirrorRadius();
  m_nomSphMirrorRadius[Rich::Rich2] = m_rich[Rich::Rich2]->sphMirrorRadius();

  // get the radiators
  if ( !m_usedRads[Rich::Aerogel] ) 
  { 
    Warning("Track segments for Aerogel are disabled",StatusCode::SUCCESS); 
  } else
  {
    m_radiators.push_back( getDet<DeRichRadiator>( DeRichRadiatorLocation::Aerogel ) );
  }
  if ( !m_usedRads[Rich::C4F10] ) 
  {
    Warning("Track segments for C4F10 are disabled",StatusCode::SUCCESS);
  } else
  {
    m_radiators.push_back( getDet<DeRichRadiator>( DeRichRadiatorLocation::C4F10 ) );
  } 
  if ( !m_usedRads[Rich::CF4] ) 
  {
    Warning("Track segments for CF4 are disabled",StatusCode::SUCCESS);
  } else
  {
    m_radiators.push_back( getDet<DeRichRadiator>( DeRichRadiatorLocation::CF4 ) );
  }

  return sc;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichTrSegMakerFromTrStoredTracks::finalize()
{
  // Execute base class method
  return RichToolBase::finalize();
}

//=============================================================================
// Constructs the track segments for a given TrStoredTrack
//=============================================================================
int RichTrSegMakerFromTrStoredTracks::constructSegments( const ContainedObject * obj,
                                                         std::vector<RichTrackSegment>& segments )
  const {

  // Try to cast input data to required type for this implementation
  const TrStoredTrack * track = dynamic_cast<const TrStoredTrack *>(obj);
  if ( !track ) 
  {
    Warning("::constructSegments : Input data object is not of type TrStoredTrack");
    return 0;
  }
  if ( msgLevel(MSG::VERBOSE) ) 
  {
    verbose() << "Analysing TrStoredTrack key=" << track->key();
    if      ( track->unique()  ) verbose() << " unique";
    if      ( track->forward() ) verbose() << " forward";
    else if ( track->match()   ) verbose() << " match";
    else if ( track->seed()    ) verbose() << " seed";
    else if ( track->veloTT()  ) verbose() << " veloTT";
    else if ( track->ksTrack() ) verbose() << " KsTrack";
    else if ( track->velo()    ) verbose() << " velo";
    else                         verbose() << " UNKNOWN";
    verbose() << " (history " << track->history() 
              << ") : " << track->states().size() << " TrStates at z =";
    for ( SmartRefVector<TrState>::const_iterator iS = track->states().begin();
          iS != track->states().end(); ++iS ) 
    {
      if (*iS) verbose() << " " << (*iS)->z();
    }
    verbose() << endreq;
  }

  // make sure vector is empty
  segments.clear();

  // Loop over all radiators
  for ( Radiators::const_iterator radiator = m_radiators.begin();
        radiator != m_radiators.end();
        ++radiator ) {    

    // which radiator
    const Rich::RadiatorType rad = (*radiator)->radiatorID();
    if ( msgLevel(MSG::VERBOSE) ) 
    {
      verbose() << " Considering radiator " << rad << endreq;
    }

    // choose appropriate z positions for initial track states for this radiator
    const double zStart = ( Rich::CF4 == rad ? m_nomZstates[2] : m_nomZstates[0] );
    const double zEnd   = ( Rich::CF4 == rad ? m_nomZstates[3] : m_nomZstates[1] );

    // Get the track state points
    TrStateP * entryPStateRaw = dynamic_cast<TrStateP*>( (TrState*)track->closestState(zStart) );
    if ( !entryPStateRaw || fabs(zStart-entryPStateRaw->z()) > m_zTolerance[rad] ) continue;
    TrStateP * exitPStateRaw = dynamic_cast<TrStateP*>( (TrState*)track->closestState(zEnd) );
    if ( !exitPStateRaw || fabs(zEnd-exitPStateRaw->z()) > m_zTolerance[rad] ) continue;

    // Clone entry state
    TrStateP * entryPState = dynamic_cast<TrStateP*>( entryPStateRaw->clone() );
    if ( !entryPState ) { Warning("Failed to clone TrStateP"); continue; }

    // Clone exit state (for aero use entrance point)
    TrStateP * exitPState = dynamic_cast<TrStateP*>( Rich::Aerogel == rad ?
                                                     entryPStateRaw->clone() : exitPStateRaw->clone() );
    if ( !exitPState ) { Warning("Failed to clone TrStateP"); continue; }

    if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << "  Found appropriate initial start/end TrStates" << endreq
                << "   EntryPos : " << HepPoint3D(entryPState->x(),entryPState->y(),entryPState->z()) << endreq
                << "   EntryDir : " << HepVector3D(entryPState->tx(),entryPState->ty(),1) << endreq
                << "   ExitPos  : " << HepPoint3D(exitPState->x(),exitPState->y(),exitPState->z()) << endreq
                << "   ExitDir  : " << HepVector3D(exitPState->tx(),exitPState->ty(),1) << endreq;
    }

    // use state closest to the entry point in radiator
    HepPoint3D entryPoint1, exitPoint1;
    bool entryStateOK = false;
    HepPoint3D firstPoint ( entryPState->x(), entryPState->y(), entryPState->z() );
    HepVector3D firstDir  ( entryPState->tx(), entryPState->ty(), 1 );
    if ( (*radiator)->nextIntersectionPoint( firstPoint,
                                             firstDir,
                                             entryPoint1 ) ) {
      
      // extrapolate state to the correct z
      if ( moveState( entryPState, entryPoint1.z(), entryPStateRaw ) ) {
        // find radiator entry and exit points
        if ( entryPState &&
             fabs(entryPState->z() - entryPoint1.z()) < m_zTolerance[rad] ) {
          if ( (*radiator)->intersectionPoints( HepPoint3D(entryPState->x(),entryPState->y(),entryPState->z()),
                                                HepVector3D(entryPState->tx(),entryPState->ty(),1),
                                                entryPoint1,
                                                exitPoint1) ) { 
            entryStateOK = true;
            if ( msgLevel(MSG::VERBOSE) ) 
              verbose() << "      Entry state rad intersection points " << entryPoint1 << " " << exitPoint1 << endreq;
          }          
        }
      }
    } else if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << "Failed to intersect entry state" << endreq;
    }

    // If gas radiator try and use exit state to get exit point more precisely
    bool exitStateOK = false;
    HepPoint3D entryPoint2, exitPoint2;
    if ( rad != Rich::Aerogel ) {
      HepPoint3D lastPoint  ( exitPState->x(), exitPState->y(), exitPState->z() );
      HepVector3D lastDir   ( -exitPState->tx(), -exitPState->ty(), -1 );
      if ( (*radiator)->nextIntersectionPoint(lastPoint,
                                              lastDir,
                                              entryPoint2) ) {
        // extrapolate state to the correct z
        if ( moveState( exitPState, entryPoint2.z(), exitPStateRaw ) ) {
          // find radiator entry and exit points
          if ( exitPState &&
               fabs(exitPState->z() - entryPoint2.z()) < m_zTolerance[rad] ) {
            if ( (*radiator)->intersectionPoints(HepPoint3D(exitPState->x(),exitPState->y(),exitPState->z()),
                                                 HepVector3D(exitPState->tx(),exitPState->ty(),1),
                                                 entryPoint2,
                                                 exitPoint2) ) { 
              exitStateOK = true; 
              if ( msgLevel(MSG::VERBOSE) ) 
                verbose() << "      Exit state rad intersection points " << entryPoint2 << " " << exitPoint2 << endreq;
            }
          }
        }
      } else if ( msgLevel(MSG::VERBOSE) ) {
        verbose() << "Failed to intersect exit state" << endreq;
      }
    } // end aerogel if

    // transport entry and exit states to best points
    StatusCode sc;
    if ( entryStateOK && exitStateOK ) {
      if (msgLevel(MSG::VERBOSE)) verbose() << "  Both states OK : Zentry=" << entryPoint1.z()
                                            << " Zexit=" << exitPoint2.z() << endreq;

      // make sure at current z positions
      if (msgLevel(MSG::VERBOSE)) verbose() << "  Checking entry point is at final z=" << entryPoint1.z() << endreq;
      const StatusCode sc1 = moveState( entryPState, entryPoint1.z(), entryPStateRaw );
      if (msgLevel(MSG::VERBOSE)) verbose() << "  Checking exit point is at final z=" << exitPoint2.z() << endreq;
      const StatusCode sc2 = moveState( exitPState,  exitPoint2.z(), exitPStateRaw );
      sc = sc1 && sc2;

    } else if ( entryStateOK ) {
      if (msgLevel(MSG::VERBOSE)) verbose() << "  Entry state OK : Zentry=" << entryPoint1.z()
                                            << " Zexit=" << exitPoint1.z() << endreq;

      if ( Rich::Aerogel != rad ) {
        // delete current exit state and replace with clone of raw entrance state
        delete exitPState;
        exitPState = dynamic_cast<TrStateP*>( entryPStateRaw->clone() );
        if ( !exitPState ) { Warning("Failed to clone TrStateP"); continue; }
      }

      // make sure at current z positions
      if (msgLevel(MSG::VERBOSE)) verbose() << "  Checking entry point is at final z= " << entryPoint1.z() << endreq;
      const StatusCode sc1 = moveState( entryPState, entryPoint1.z(), entryPStateRaw );
      if (msgLevel(MSG::VERBOSE)) verbose() << "  Checking exit point is at final z= " << exitPoint1.z() << endreq;
      const StatusCode sc2 = moveState( exitPState, exitPoint1.z(), exitPStateRaw );
      sc = sc1 && sc2;

    } else if ( exitStateOK ) {
      if (msgLevel(MSG::VERBOSE)) 
        verbose() << "  Exit state OK  : Zentry=" << entryPoint2.z()
                  << " Zexit=" << exitPoint2.z() << endreq;

      // delete current exit state and replace with clone of raw entrance state
      delete entryPState;
      entryPState = dynamic_cast<TrStateP*>( exitPStateRaw->clone() );
      if ( !entryPState ) { Warning("Failed to clone TrStateP"); continue; }

      // make sure at current z positions
      if (msgLevel(MSG::VERBOSE)) verbose() << "  Checking entry point is at final z= " << entryPoint2.z() << endreq;
      const StatusCode sc1 = moveState( entryPState, entryPoint2.z(), entryPStateRaw );
      if (msgLevel(MSG::VERBOSE)) verbose() << "  Checking exit point is at final z= " << exitPoint2.z() << endreq;
      const StatusCode sc2 = moveState( exitPState,  exitPoint2.z(), exitPStateRaw );
      sc = sc1 && sc2;

    } else {
      if (msgLevel(MSG::VERBOSE)) verbose() << "  Both states failed" << endreq;
      continue;
    }
    if ( sc.isFailure() ) {
      if (msgLevel(MSG::VERBOSE)) verbose() << "    --> Failed to use state information. Quitting." << endreq;
      continue;
    }

    // a special hack for the C4F10 - since the aerogel volume
    // is placed INSIDE the C4F10, the default entry point is wrong.
    if ( Rich::C4F10 == rad ) fixC4F10EntryPoint( entryPState, entryPStateRaw );

    // check for intersection with spherical mirror for gas radiators
    // and if need be correct exit point accordingly
    if ( rad != Rich::Aerogel ) correctRadExitMirror( *radiator, exitPState, exitPStateRaw );

    // Final check that info is reaonable
    if ( (entryPState->z() > exitPState->z()) ) {
      Warning( "Entry state after exit state for " + Rich::text(rad) + "  -> rejecting segment",
               StatusCode::SUCCESS, 5 );
      continue;
    }
    if ( (exitPState->z()-entryPState->z()) < m_minStateDiff[rad] ) {
      Warning( "Track states for " + Rich::text(rad) + " too close in z -> rejecting segment",
               StatusCode::SUCCESS, 5 );
      continue;
    }

    // Create final entry and exit state points and momentum vectors
    const HepPoint3D  entryPoint(entryPState->x(),entryPState->y(),entryPState->z());
    HepVector3D entryStateMomentum( entryPState->tx(),entryPState->ty(), 1 );
    entryStateMomentum.setMag( entryPState->p() );
    const HepPoint3D  exitPoint(exitPState->x(),exitPState->y(),exitPState->z());
    HepVector3D exitStateMomentum( exitPState->tx(),exitPState->ty(), 1 );
    exitStateMomentum.setMag( exitPState->p() );

    // Errors for entry and exit states
    const RichTrackSegment::StateErrors entryErrs( entryPState );
    const RichTrackSegment::StateErrors exitErrs( exitPState );

    // print out final points
    if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << "  Found final points :-" << endreq
                << "   Entry : Pnt=" << entryPoint << " Mom=" << entryStateMomentum
                << " Ptot=" << entryStateMomentum.mag() << endreq
                << "   Exit  : Pnt=" << exitPoint  << " Mom=" << exitStateMomentum
                << " Ptot=" << exitStateMomentum.mag() << endreq;
    }

    try {

      // For gas radiators transport entry state to mid point to create middle point
      // information for three point RichTrackSegment constructor
      if ( rad != Rich::Aerogel && moveState( entryPState,
                                              (entryPoint.z()+exitPoint.z())/2, 
                                              entryPStateRaw ) ) {

        // middle point information
        const HepPoint3D midPoint(entryPState->x(),entryPState->y(),entryPState->z());
        HepVector3D midStateMomentum( entryPState->tx(),entryPState->ty(), 1 );
        midStateMomentum.setMag( entryPState->p() );

        // errors for middle state
        const RichTrackSegment::StateErrors midErrs( entryPState );

        // Using this information, make radiator segment
        // this version uses 3 states and thus incorporates some concept of track curvature
        //segments.push_back( RichTrackSegment( RichTrackSegment::UseChordBetweenStates(),
        segments.push_back( RichTrackSegment( RichTrackSegment::UseAllStateVectors(),
                                              entryPoint,entryStateMomentum,
                                              midPoint,  midStateMomentum,
                                              exitPoint, exitStateMomentum,
                                              rad, (*radiator)->rich(),
                                              entryErrs, midErrs, exitErrs ) );

      } else {

        // Using this information, make radiator segment
        // this version uses 2 states and thus forces a straight line approximation
        //segments.push_back( RichTrackSegment( RichTrackSegment::UseChordBetweenStates(),
        segments.push_back( RichTrackSegment( RichTrackSegment::UseAllStateVectors(),
                                              entryPoint,entryStateMomentum,
                                              exitPoint, exitStateMomentum,
                                              rad, (*radiator)->rich(),
                                              entryErrs, exitErrs ) );

      }

    }
    catch ( const std::exception & excpt ) {
      Warning( "Exception whilst creating RichTrackSegment '"+std::string(excpt.what())+"'" );
    }
    
    // Clean up cloned states
    delete entryPState;
    delete exitPState;

  } // end loop over radiators

  // return value is number of segments formed
  return segments.size();
}

//============================================================================
// fixup C4F10 entry point
void RichTrSegMakerFromTrStoredTracks::fixC4F10EntryPoint( TrStateP *& state,
                                                           TrStateP * refState ) const
{

  HepPoint3D dummyPoint, aerogelExitPoint;
  if ( m_radiators[Rich::Aerogel]->intersectionPoints(HepPoint3D(state->x(),state->y(),state->z()),
                                                     HepVector3D(state->tx(),state->ty(),1),
                                                     dummyPoint,
                                                     aerogelExitPoint) ) {
    if ( aerogelExitPoint.z() > state->z() ) 
    {
      if (msgLevel(MSG::VERBOSE)) verbose() << "   Correcting C4F10 entry point" << endreq;
      moveState( state, aerogelExitPoint.z(), refState );
    }
  }

}

//============================================================================
void RichTrSegMakerFromTrStoredTracks::correctRadExitMirror( DeRichRadiator* radiator,
                                                             TrStateP *& state,
                                                             TrStateP * refState ) const
{

  // get rich information
  const Rich::DetectorType rich = radiator->rich();

  // move state to be on the inside of the mirror
  moveState( state, state->z() - m_mirrShift[rich], refState );
  bool correct = false;

  // find mirror intersection using the reflect method
  HepPoint3D intersection(state->x(),state->y(),state->z());
  HepVector3D tempDir(state->tx(),state->ty(),1);

  if ( m_rayTracing->reflectSpherical(intersection,
                                      tempDir,
                                      m_nominalCoC[rich][m_rich[rich]->side(intersection)],
                                      m_nomSphMirrorRadius[rich]) ) 
  {    
    if ( radiator->geometry()->isInside(intersection) ) { correct = true; }
  }                   

  // finally, update state
  if ( correct ) {
    if (msgLevel(MSG::VERBOSE)) verbose() << "   Correcting exit point to spherical mirror" << endreq;
    moveState( state, intersection.z(), refState );
  } else {
    if (msgLevel(MSG::VERBOSE)) verbose() << "   Failed to correct exit state to spherical mirror" << endreq;
    moveState( state, state->z() + m_mirrShift[rich], refState );
  }

}

//============================================================================
StatusCode RichTrSegMakerFromTrStoredTracks::moveState( TrStateP *& stateToMove,
                                                        const double z,
                                                        TrStateP * refState ) const
{
  // Check if requested move is big enough to bother with
  if ( fabs(stateToMove->z() - z) > 1*mm ) {

    // debug printout
    if ( msgLevel(MSG::VERBOSE) ) 
    {
      verbose() << "    --> Extrapolating state from " 
                << HepPoint3D(stateToMove->x(),stateToMove->y(),stateToMove->z()) << endreq;
    }

    if ( m_extrapFromRef && refState )
    {
      // Delete current working state and start fresh from reference state
      delete stateToMove;
      stateToMove = dynamic_cast<TrStateP*>( refState->clone() );
      if ( msgLevel(MSG::VERBOSE) ) 
      {
        verbose() << "      --> Using reference state  "
                  << HepPoint3D(stateToMove->x(),stateToMove->y(),stateToMove->z()) << endreq;
      }
    }

    // try first with the primary extrapolator
    if ( !m_trExt1->propagate(stateToMove,z) ) 
    {
      // if that fails, try the backup one
      if ( m_trExt2->propagate(stateToMove,z) ) 
      {
        Warning(m_Ext1+" failed -> reverted to "+m_Ext2,StatusCode::SUCCESS);
      } else 
      {
        // Both failed ...
        Warning("Failed to transport state using "+m_Ext1+" or "+m_Ext2);
        return StatusCode::FAILURE;
      }
    }

    // debug printout
    if ( msgLevel(MSG::VERBOSE) ) 
    {
      verbose() << "                            to   " 
                << HepPoint3D(stateToMove->x(),stateToMove->y(),stateToMove->z()) << endreq;
    }

  }

  return StatusCode::SUCCESS;
}
