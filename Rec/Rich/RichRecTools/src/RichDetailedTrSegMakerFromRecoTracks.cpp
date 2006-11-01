
//---------------------------------------------------------------------------------
/** @file RichDetailedTrSegMakerFromRecoTracks.cpp
 *
 * Implementation file for class : RichDetailedTrSegMakerFromRecoTracks
 *
 * CVS Log :-
 * $Id: RichDetailedTrSegMakerFromRecoTracks.cpp,v 1.11 2006-11-01 19:36:06 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//---------------------------------------------------------------------------------

// local
#include "RichDetailedTrSegMakerFromRecoTracks.h"

// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// namespaces
using namespace LHCb;

//-------------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( RichDetailedTrSegMakerFromRecoTracks );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichDetailedTrSegMakerFromRecoTracks::
RichDetailedTrSegMakerFromRecoTracks( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichToolBase         ( type, name, parent           ),
    m_rayTracing         ( NULL                         ),
    m_richPartProp       ( NULL                         ),
    m_radTool            ( NULL                         ),
    m_zTolerance         ( Rich::NRadiatorTypes, 0      ),
    m_nomZstates         ( 2*Rich::NRiches,      0      ),
    m_mirrShift          ( Rich::NRiches,        0      ),
    m_minStateDiff       ( Rich::NRadiatorTypes, 0      ),
    m_trExt1             ( NULL                         ),
    m_trExt2             ( NULL                         ),
    m_trExt1Name         ( "TrackHerabExtrapolator"     ),
    m_trExt2Name         ( "TrackParabolicExtrapolator" ),
    m_usedRads           ( Rich::NRadiatorTypes, true   ),
    m_extrapFromRef      ( true                         ),
    m_minZmove           ( 1 * Gaudi::Units::mm         ),
    m_minEntryRad        ( Rich::NRadiatorTypes, 0      ),
    m_minExitRad         ( Rich::NRadiatorTypes, 0      )
{

  // the interface
  declareInterface<IRichTrSegMaker>(this);

  // job options

  declareProperty( "PrimaryTrackExtrapolator", m_trExt1Name    );
  declareProperty( "BackupTrackExtrapolator",  m_trExt2Name    );
  declareProperty( "UseRadiators",             m_usedRads      );
  declareProperty( "ExtrapolateFromReference", m_extrapFromRef );
  declareProperty( "MinimumZMove",             m_minZmove      );

  // Nominal z positions of states at RICHes
  // do not have to be precisely accurate
  m_nomZstates[0] =   99.0*Gaudi::Units::cm; // Place to look for Rich1 entry state
  m_nomZstates[1] =  216.5*Gaudi::Units::cm; // Place to look for Rich1 exit state
  m_nomZstates[2] =  945.0*Gaudi::Units::cm; // Place to look for Rich2 entry state
  m_nomZstates[3] = 1190.0*Gaudi::Units::cm; // Place to look for Rich2 exit state
  declareProperty( "NominalStateZ", m_nomZstates );

  // tolerances on z positions
  m_zTolerance[Rich::Aerogel]  =  800*Gaudi::Units::mm;
  m_zTolerance[Rich::Rich1Gas] =  800*Gaudi::Units::mm;
  m_zTolerance[Rich::Rich2Gas] = 2000*Gaudi::Units::mm;
  declareProperty( "ZTolerances", m_zTolerance );

  // sanity checks on state information
  m_minStateDiff[Rich::Aerogel]  =  1*Gaudi::Units::mm;
  m_minStateDiff[Rich::Rich1Gas] = 25*Gaudi::Units::mm;
  m_minStateDiff[Rich::Rich2Gas] = 50*Gaudi::Units::mm;
  declareProperty( "ZSanityChecks", m_minStateDiff );

  // shifts for mirror correction
  m_mirrShift[Rich::Rich1] =  35*Gaudi::Units::cm;
  m_mirrShift[Rich::Rich2] = 150*Gaudi::Units::cm;
  declareProperty( "MirrorShiftCorr", m_mirrShift );

  // Type of track segments to create
  declareProperty( "SegmentType", m_trSegTypeJO = "AllStateVectors" );

  // temp hacks

  m_minEntryRad[Rich::Rich1Gas] = 40*Gaudi::Units::mm;
  m_minExitRad[Rich::Rich1Gas]  = 60*Gaudi::Units::mm;
  declareProperty( "MinEntryRadius", m_minEntryRad );
  declareProperty( "MinExitRadius",  m_minExitRad  );

}

//=============================================================================
// Destructor
//=============================================================================
RichDetailedTrSegMakerFromRecoTracks::~RichDetailedTrSegMakerFromRecoTracks() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode RichDetailedTrSegMakerFromRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // load primary track extrapolator ( backup is loaded on-demand )
  m_trExt1 = tool<ITrackExtrapolator>( m_trExt1Name );

  // Get the RICH tools
  acquireTool( "RichRayTracing",          m_rayTracing   );
  acquireTool( "RichParticleProperties",  m_richPartProp );
  acquireTool( "RichRadiatorTool",        m_radTool      );

  // get Detector elements for RICH1 and RICH2
  m_rich[Rich::Rich1] = getDet<DeRich>( DeRichLocation::Rich1 );
  m_rich[Rich::Rich2] = getDet<DeRich>( DeRichLocation::Rich2 );

  // get the radiators
  m_radiators[Rich::Aerogel]  = getDet<DeRichRadiator>( DeRichRadiatorLocation::Aerogel  );
  m_radiators[Rich::Rich1Gas] = getDet<DeRichRadiator>( DeRichRadiatorLocation::Rich1Gas );
  m_radiators[Rich::Rich2Gas] = getDet<DeRichRadiator>( DeRichRadiatorLocation::Rich2Gas );
  if ( !m_usedRads[Rich::Aerogel] )
  {
    Warning("Track segments for Aerogel are disabled",StatusCode::SUCCESS);
  }
  if ( !m_usedRads[Rich::Rich1Gas] )
  {
    Warning("Track segments for Rich1Gas are disabled",StatusCode::SUCCESS);
  }
  if ( !m_usedRads[Rich::Rich2Gas] )
  {
    Warning("Track segments for Rich2Gas are disabled",StatusCode::SUCCESS);
  }

  // warn that we are applying a RICH1 beam hole correction
  Warning( "Applying RICH1 beam hole correction. To be removed.", StatusCode::SUCCESS );
  info() << "Minimum radiator entry radii = " << m_minEntryRad
         << " exit radii = " << m_minExitRad << endreq;

  if ( m_extrapFromRef )
  {
    info() << "Will perform all track extrapolations from reference states" << endreq;
  }

  // Define the segment type
  if      ( "AllStateVectors" == m_trSegTypeJO )
  {
    info() << "Will create track segments using all State information" << endreq;
    m_trSegType = RichTrackSegment::UseAllStateVectors;
  }
  else if ( "Chord" == m_trSegTypeJO )
  {
    info() << "Will create track segments using the 'chord' direction definition" << endreq;
    m_trSegType = RichTrackSegment::UseChordBetweenStates;
  }
  else
  {
    return Error( "Unknown RichTrackSegment type " + m_trSegTypeJO );
  }

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichDetailedTrSegMakerFromRecoTracks::finalize()
{
  // Execute base class method
  return RichToolBase::finalize();
}

//=============================================================================
// Constructs the track segments for a given Track
//=============================================================================
int
RichDetailedTrSegMakerFromRecoTracks::
constructSegments( const ContainedObject * obj,
                   std::vector<RichTrackSegment*>& segments )
  const {

  // Try to cast input data to required type for this implementation
  const Track * track = dynamic_cast<const Track *>(obj);
  if ( !track )
  {
    Warning("Input data object is not of type Track");
    return 0;
  }
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Analysing Track key=" << track->key()
              << " history=" << track->history()
              << " : " << track->states().size() << " States at z =";
    for ( std::vector<State*>::const_iterator iS = track->states().begin();
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
        ++radiator )
  {

    // which radiator
    const Rich::RadiatorType rad = (*radiator)->radiatorID();
    // is this radiator in use ?
    if ( !m_usedRads[rad] ) continue;
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " Considering radiator " << rad << endreq;
    }

    // choose appropriate z start position for initial track states for this radiator
    const double zStart = ( Rich::Rich2Gas == rad ? m_nomZstates[2] : m_nomZstates[0] );
    // Get the track entry state points
    const State * entryPStateRaw = &(track->closestState(zStart));
    if ( !entryPStateRaw || fabs(zStart-entryPStateRaw->z()) > m_zTolerance[rad] ) continue;

    // check above electron threshold
    if ( m_richPartProp->thresholdMomentum(Rich::Electron,rad) > entryPStateRaw->p() )
    {
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "  -> Below electron cherenkov threshold -> reject" << endreq;
      }
      continue;
    }

    // choose appropriate z end position for initial track states for this radiator
    const double zEnd   = ( Rich::Rich2Gas == rad ? m_nomZstates[3] : m_nomZstates[1] );
    // Get the track enrty state points
    const State * exitPStateRaw = &(track->closestState(zEnd));
    if ( !exitPStateRaw || fabs(zEnd-exitPStateRaw->z()) > m_zTolerance[rad] ) continue;

    // Clone entry state
    State * entryPState = entryPStateRaw->clone();
    if ( !entryPState ) { Warning("Failed to clone State"); continue; }

    // Clone exit state (for aero use entrance point)
    State * exitPState = ( Rich::Aerogel == rad ?
                           entryPStateRaw->clone() : exitPStateRaw->clone() );
    if ( !exitPState ) { Warning("Failed to clone State"); delete entryPState; continue; }

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "  Found appropriate initial start/end States" << endreq
                << "   EntryPos : "
                << entryPState->position() << endreq
                << "   EntryDir : "
                << entryPState->slopes() << endreq
                << "   ExitPos  : "
                << exitPState->position() << endreq
                << "   ExitDir  : "
                << exitPState->slopes() << endreq;
    }

    // use state closest to the entry point in radiator
    Gaudi::XYZPoint entryPoint1;
    RichRadIntersection::Vector intersects1;
    bool entryStateOK = false;
    if ( (*radiator)->nextIntersectionPoint( entryPState->position(),
                                             entryPState->slopes(),
                                             entryPoint1 ) )
    {
      // extrapolate state to the correct z
      if ( moveState( entryPState, entryPoint1.z(), entryPStateRaw ) )
      {
        // find radiator entry and exit points
        if ( entryPState &&
             fabs(entryPState->z() - entryPoint1.z()) < m_zTolerance[rad] )
        {
          if ( getRadIntersections( entryPState->position(),
                                    entryPState->slopes(),
                                    *radiator,
                                    intersects1 ) > 0 )
          {
            entryStateOK = true;
            entryPoint1 = intersects1.front().entryPoint();
            if ( msgLevel(MSG::VERBOSE) )
              verbose() << "      Entry state rad intersection points "
                        << intersects1 << endreq;
          }
        }
      }
    }
    else if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Failed to intersect entry state" << endreq;
    }

    // If gas radiator try and use exit state to get exit point more precisely
    bool exitStateOK = false;
    Gaudi::XYZPoint entryPoint2;
    RichRadIntersection::Vector intersects2;
    if ( rad != Rich::Aerogel )
    {
      if ( (*radiator)->nextIntersectionPoint ( exitPState->position(),
                                                -exitPState->slopes(),
                                                entryPoint2 ) )
      {
        // extrapolate state to the correct z
        if ( moveState( exitPState, entryPoint2.z(), exitPStateRaw ) )
        {
          // find radiator entry and exit points
          if ( exitPState &&
               fabs(exitPState->z() - entryPoint2.z()) < m_zTolerance[rad] )
          {
            if ( getRadIntersections( exitPState->position(),
                                      exitPState->slopes(),
                                      *radiator,
                                      intersects2 ) > 0 )
            {
              exitStateOK = true;
              entryPoint2 = intersects2.front().entryPoint();
              if ( msgLevel(MSG::VERBOSE) )
                verbose() << "      Exit state rad intersection points "
                          << intersects2 << endreq;
            }
          }
        }
      }
      else if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "Failed to intersect exit state" << endreq;
      }
    } // end !aerogel if

    // transport entry and exit states to best points
    StatusCode sc = StatusCode::FAILURE;
    if ( entryStateOK && exitStateOK )
    {
      if (msgLevel(MSG::VERBOSE)) verbose() << "  Both states OK : Zentry=" << entryPoint1.z()
                                            << " Zexit=" << intersects2.back().exitPoint().z() << endreq;

      // make sure at current z positions
      if (msgLevel(MSG::VERBOSE))
        verbose() << "  Checking entry point is at final z=" << entryPoint1.z() << endreq;
      const StatusCode sc1 = moveState( entryPState, entryPoint1.z(), entryPStateRaw );
      if (msgLevel(MSG::VERBOSE))
        verbose() << "  Checking exit point is at final z=" << intersects2.back().exitPoint().z() << endreq;
      const StatusCode sc2 = moveState( exitPState,  intersects2.back().exitPoint().z(), exitPStateRaw );
      sc = sc1 && sc2;

    }
    else if ( entryStateOK )
    {
      if (msgLevel(MSG::VERBOSE))
        verbose() << "  Entry state OK : Zentry=" << entryPoint1.z()
                  << " Zexit=" << intersects1.back().exitPoint().z() << endreq;

      if ( Rich::Aerogel != rad )
      {
        // delete current exit state and replace with clone of raw entrance state
        delete exitPState;
        exitPState = entryPStateRaw->clone();
        if ( !exitPState ) { Warning("Failed to clone State"); delete entryPState; continue; }
      }

      // make sure at current z positions
      if (msgLevel(MSG::VERBOSE))
        verbose() << "  Checking entry point is at final z= " << entryPoint1.z() << endreq;
      const StatusCode sc1 = moveState( entryPState, entryPoint1.z(), entryPStateRaw );
      if (msgLevel(MSG::VERBOSE))
        verbose() << "  Checking exit point is at final z= " << intersects1.back().exitPoint().z() << endreq;
      const StatusCode sc2 = moveState( exitPState, intersects1.back().exitPoint().z(), exitPStateRaw );
      sc = sc1 && sc2;

    }
    else if ( exitStateOK )
    {
      if (msgLevel(MSG::VERBOSE))
        verbose() << "  Exit state OK  : Zentry=" << entryPoint2.z()
                  << " Zexit=" << intersects2.back().exitPoint().z() << endreq;

      // delete current entry state and replace with clone of raw entrance state
      delete entryPState;
      entryPState = exitPStateRaw->clone();
      if ( !entryPState ) { Warning("Failed to clone State"); delete exitPState; continue; }

      // make sure at current z positions
      if (msgLevel(MSG::VERBOSE))
        verbose() << "  Checking entry point is at final z= " << entryPoint2.z() << endreq;
      const StatusCode sc1 = moveState( entryPState, entryPoint2.z(), entryPStateRaw );
      if (msgLevel(MSG::VERBOSE))
        verbose() << "  Checking exit point is at final z= " << intersects2.back().exitPoint().z() << endreq;
      const StatusCode sc2 = moveState( exitPState,  intersects2.back().exitPoint().z(), exitPStateRaw );
      sc = sc1 && sc2;

    }
    else
    {
      // no valid extrapolations, so quit skip this track/radiator
      delete entryPState;
      delete exitPState;
      if (msgLevel(MSG::VERBOSE)) verbose() << "  Both states failed" << endreq;
      continue;
    }

    // Test final status code
    if ( sc.isFailure() )
    {
      delete entryPState;
      delete exitPState;
      if (msgLevel(MSG::VERBOSE))
        verbose() << "    --> Failed to use state information. Quitting." << endreq;
      continue;
    }

    // temp hack. Apply radiator hole checks
    // work around for lack of hole in RICH1 gas
    // to be fixed in RichDet and/or XML properly
    const bool entry_out = ( gsl_pow_2(m_minEntryRad[rad]) >
                             gsl_pow_2(entryPState->x()) + gsl_pow_2(entryPState->y()) );
    const bool exit_out  = ( gsl_pow_2(m_minExitRad[rad]) >
                             gsl_pow_2(exitPState->x()) + gsl_pow_2(exitPState->y()) );
    sc = StatusCode::SUCCESS;
    if ( entry_out && exit_out )
    {
      if (msgLevel(MSG::VERBOSE))
        verbose() << "    --> Failed beampipe hole test" << endreq;
      delete entryPState;
      delete exitPState;
      continue;
    }
    else if ( entry_out )
    {
      const Gaudi::XYZVector vect = exitPState->position() - entryPState->position();
      const unsigned int nSamples = 100;
      for ( unsigned int i = 1; i<nSamples; ++i )
      {
        const Gaudi::XYZPoint tmpPoint = ( entryPState->position() +
                                            ((double)i/(double)nSamples)*vect );
        if ( gsl_pow_2(m_minEntryRad[rad]) < gsl_pow_2(tmpPoint.x()) + gsl_pow_2(tmpPoint.y()) )
        {
          if (msgLevel(MSG::VERBOSE))
            verbose() << "    --> Fudging entry point to : " << tmpPoint << endreq;
          sc = moveState( entryPState, tmpPoint.z(), entryPStateRaw );
          break;
        }
      }
    } 
    else if ( exit_out )
    {
      const Gaudi::XYZVector vect = entryPState->position() - exitPState->position();
      const unsigned int nSamples = 100;
      for ( unsigned int i = 1; i<nSamples; ++i )
      {
        const Gaudi::XYZPoint tmpPoint = ( exitPState->position() +
                                           ((double)i/(double)nSamples)*vect );
        if ( gsl_pow_2(m_minExitRad[rad]) < gsl_pow_2(tmpPoint.x()) + gsl_pow_2(tmpPoint.y()) )
        {
          if (msgLevel(MSG::VERBOSE))
            verbose() << "    --> Fudging exit point to : " << tmpPoint << endreq;
          sc = moveState( exitPState, tmpPoint.z(), exitPStateRaw );
          break;
        }
      }
    }
    if ( sc.isFailure() )
    {
      delete entryPState;
      delete exitPState;
      if (msgLevel(MSG::VERBOSE))
        verbose() << "    --> Error fudging radiator entry/exit points. Quitting." << endreq;
      continue;
    }

    // a special hack for the Rich1Gas - since the aerogel volume
    // is placed INSIDE the Rich1Gas, the default entry point is wrong.
    if ( Rich::Rich1Gas == rad ) fixRich1GasEntryPoint( entryPState, entryPStateRaw );

    // check for intersection with spherical mirror for gas radiators
    // and if need be correct exit point accordingly
    if ( rad != Rich::Aerogel ) correctRadExitMirror( *radiator, exitPState, exitPStateRaw );

    // Final check that info is reaonable
    if ( (entryPState->z() > exitPState->z()) )
    {
      delete entryPState;
      delete exitPState;
      Warning( "Entry state after exit state for " + Rich::text(rad) + " -> rejecting segment",
               StatusCode::SUCCESS, 5 );
      continue;
    }
    if ( (exitPState->z()-entryPState->z()) < m_minStateDiff[rad] )
    {
      delete entryPState;
      delete exitPState;
      Warning( "Track states for " + Rich::text(rad) + " too close in z -> rejecting segment",
               StatusCode::SUCCESS, 5 );
      continue;
    }

    // Create final entry and exit state points and momentum vectors
    const Gaudi::XYZPoint  entryPoint( entryPState->position() );
    Gaudi::XYZVector entryStateMomentum( entryPState->slopes() );
    entryStateMomentum *= entryPState->p()/sqrt(entryStateMomentum.Mag2());
    const Gaudi::XYZPoint  exitPoint(exitPState->position());
    Gaudi::XYZVector exitStateMomentum( exitPState->slopes() );
    exitStateMomentum *= exitPState->p()/sqrt(exitStateMomentum.Mag2());

    // Update final intersections
    RichRadIntersection::Vector & final_intersects = ( entryStateOK ? intersects1 : intersects2 );
    final_intersects.front().setEntryPoint(entryPoint);
    final_intersects.front().setEntryMomentum(entryStateMomentum);
    final_intersects.back().setExitPoint(exitPoint);
    final_intersects.back().setExitMomentum(exitStateMomentum);

    // Errors for entry and exit states
    
    const RichTrackSegment::StateErrors entryErrs ( entryPState->errX2(),
                                                    entryPState->errY2(),
                                                    entryPState->errTx2(),
                                                    entryPState->errTy2(),
                                                    entryPState->errP2() );
    const RichTrackSegment::StateErrors exitErrs  ( exitPState->errX2(),
                                                    exitPState->errY2(),
                                                    exitPState->errTx2(),
                                                    exitPState->errTy2(),
                                                    exitPState->errP2() );

    // print out final points
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "  Found final points :-" << endreq
                << "   Entry : Pnt=" << entryPoint << " Mom=" << entryStateMomentum
                << " Ptot=" << sqrt(entryStateMomentum.Mag2()) << endreq
                << "   Exit  : Pnt=" << exitPoint  << " Mom=" << exitStateMomentum
                << " Ptot=" << sqrt(exitStateMomentum.Mag2()) << endreq;
    }

    try
    {

      // For gas radiators transport entry state to mid point to create middle point
      // information for three point RichTrackSegment constructor
      if ( rad != Rich::Aerogel )
      {

        // data for middle state
        RichTrackSegment::StateErrors midErrs;
        Gaudi::XYZPoint midPoint;
        Gaudi::XYZVector midMomentum;
        const bool OK = createMiddleInfo( rad,
                                          entryPState, entryPStateRaw,
                                          exitPState,  exitPStateRaw,
                                          midPoint, midMomentum, midErrs ).isSuccess();

        if ( OK )
        {
          // Using this information, make radiator segment
          // this version uses 3 states and thus incorporates some concept of track curvature
          segments.push_back( new RichTrackSegment( m_trSegType,
                                                    final_intersects,
                                                    midPoint,  midMomentum,
                                                    rad, (*radiator)->rich(),
                                                    entryErrs, midErrs, exitErrs ) );
        }
        else
        {
          // Using this information, make radiator segment
          // this version uses 2 states and thus forces a straight line approximation
          segments.push_back( new RichTrackSegment( m_trSegType,
                                                    final_intersects,
                                                    rad, (*radiator)->rich(),
                                                    entryErrs, exitErrs ) );
        }

      }
      else
      {

        // Using this information, make radiator segment
        // this version uses 2 states and thus forces a straight line approximation
        segments.push_back( new RichTrackSegment( m_trSegType,
                                                  final_intersects,
                                                  rad, (*radiator)->rich(),
                                                  entryErrs, exitErrs ) );

      }

    }
    catch ( const std::exception & excpt )
    {
      Warning( "Exception whilst creating RichTrackSegment '"+std::string(excpt.what())+"'" );
    }

    // Clean up cloned states
    delete entryPState;
    delete exitPState;

  } // end loop over radiators

  // Final printout of states, to see if anything has changed ...
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Finished with Track key=" << track->key()
              << " history=" << track->history()
              << " : " << track->states().size() << " States at z =";
    for ( std::vector<State*>::const_iterator iS = track->states().begin();
          iS != track->states().end(); ++iS )
    {
      if (*iS) verbose() << " " << (*iS)->z();
    }
    verbose() << endreq;
  }

  // return value is number of segments formed
  return segments.size();
}
//====================================================================================================

//====================================================================================================
// creates middle point info
StatusCode
RichDetailedTrSegMakerFromRecoTracks::createMiddleInfo( const Rich::RadiatorType rad,
                                                        LHCb::State *& fState,
                                                        const LHCb::State * fStateRef,
                                                        LHCb::State *& lState,
                                                        const LHCb::State * lStateRef,
                                                        Gaudi::XYZPoint & midPoint,
                                                        Gaudi::XYZVector & midMomentum,
                                                        RichTrackSegment::StateErrors & errors ) const
{

  // middle point z position
  const double midZ = (fState->position().z()+lState->position().z())/2;

  // move start state to this z
  const StatusCode moveFirst = moveState( fState, midZ, fStateRef );

  // move end state to this z
  const StatusCode moveLast = ( Rich::Rich1Gas == rad ?
                                moveState( lState, midZ, lStateRef ) : StatusCode::FAILURE );

  if ( moveFirst && moveLast )
  {
    midPoint     = fState->position() + (lState->position()-fState->position())/2;
    midMomentum  = (fState->slopes()+lState->slopes())/2;
    midMomentum *= (fState->p()+lState->p()) / (2.0*sqrt(midMomentum.Mag2()));
    errors = RichTrackSegment::StateErrors( (fState->errX2()+lState->errX2())/2,
                                            (fState->errY2()+lState->errY2())/2,
                                            (fState->errTx2()+lState->errTx2())/2,
                                            (fState->errTy2()+lState->errTy2())/2,
                                            (fState->errP2()+lState->errP2())/2 );
  }
  else if ( moveFirst )
  {
    midPoint     = fState->position();
    midMomentum  = fState->slopes();
    midMomentum *= fState->p() / sqrt(midMomentum.Mag2());
    errors = RichTrackSegment::StateErrors( fState->errX2(),
                                            fState->errY2(),
                                            fState->errTx2(),
                                            fState->errTy2(),
                                            fState->errP2() );
  }
  else if ( moveLast )
  {
    midPoint     = lState->position();
    midMomentum  = lState->slopes();
    midMomentum *= lState->p() / sqrt(midMomentum.Mag2());
    errors = RichTrackSegment::StateErrors( lState->errX2(),
                                            lState->errY2(),
                                            lState->errTx2(),
                                            lState->errTy2(),
                                            lState->errP2() );
  }

  return ( moveFirst || moveLast );
}
//====================================================================================================

//====================================================================================================
// Get radiator intersections
unsigned int
RichDetailedTrSegMakerFromRecoTracks::
getRadIntersections( const Gaudi::XYZPoint  & point,
                     const Gaudi::XYZVector & direction,
                     const DeRichRadiator * rad,
                     RichRadIntersection::Vector & intersections ) const
{

  // clear the intersections
  intersections.clear();

  // get the intersections
  m_radTool->intersections( point, direction, rad->radiatorID(), intersections );

  // return the nuber of intersections found
  return intersections.size();
}
//====================================================================================================


//====================================================================================================
// fixup Rich1Gas entry point
void RichDetailedTrSegMakerFromRecoTracks::fixRich1GasEntryPoint( State *& state,
                                                                  const State * refState ) const
{

  Gaudi::XYZPoint dummyPoint, aerogelExitPoint;
  if ( m_radiators[Rich::Aerogel]->intersectionPoints( state->position(),
                                                       state->slopes(),
                                                       dummyPoint,
                                                       aerogelExitPoint ) )
  {
    if ( aerogelExitPoint.z() > state->z() )
    {
      if (msgLevel(MSG::VERBOSE)) verbose() << "   Correcting Rich1Gas entry point" << endreq;
      moveState( state, aerogelExitPoint.z(), refState );
    }
  }

}
//====================================================================================================


//====================================================================================================
void RichDetailedTrSegMakerFromRecoTracks::correctRadExitMirror( const DeRichRadiator* radiator,
                                                                 State *& state,
                                                                 const State * refState ) const
{

  // get rich information
  const Rich::DetectorType rich = radiator->rich();

  // move state to be on the inside of the mirror
  moveState( state, state->z() - m_mirrShift[rich], refState );
  bool correct = false;

  // find mirror intersection using the reflect method
  Gaudi::XYZPoint intersection( state->position() );
  Gaudi::XYZVector tempDir(state->slopes());

  if ( m_rayTracing->reflectSpherical( intersection,
                                       tempDir,
                                       m_rich[rich]->nominalCentreOfCurvature(m_rich[rich]->side(intersection)),
                                       m_rich[rich]->sphMirrorRadius() ) )
  {
    if ( radiator->geometry()->isInside(intersection) ) { correct = true; }
  }

  // finally, update state
  if ( correct )
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "   Correcting exit point to spherical mirror" << endreq;
    moveState( state, intersection.z(), refState );
  }
  else
  {
    if (msgLevel(MSG::VERBOSE))
      verbose() << "   Failed to correct exit state to spherical mirror" << endreq;
    moveState( state, state->z() + m_mirrShift[rich], refState );
  }

}
//====================================================================================================


//====================================================================================================
StatusCode RichDetailedTrSegMakerFromRecoTracks::moveState( State *& stateToMove,
                                                            const double z,
                                                            const State * refState ) const
{
  // Check if requested move is big enough to bother with
  if ( fabs(stateToMove->z() - z) > m_minZmove )
  {

    // debug printout
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "    --> Extrapolating state from "
                << stateToMove->position() << endreq;
    }

    if ( m_extrapFromRef && refState )
    {
      // Delete current working state and start fresh from reference state
      delete stateToMove;
      stateToMove = refState->clone();
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "      --> Using reference state  "
                  << stateToMove->position() << endreq;
      }
    }

    // try first with the primary extrapolator
    if ( !primaryExtrapolator()->propagate(*stateToMove,z) )
    {
      // if that fails, try the backup one
      if ( backupExtrapolator()->propagate(*stateToMove,z) )
      {
        Warning("'"+m_trExt1Name+"' failed -> successfully reverted to '"+
                m_trExt2Name+"'",StatusCode::SUCCESS);
      } else
      {
        // Both failed ...
        Warning("Failed to extrapolate state using either '"+
                m_trExt1Name+"' or '"+m_trExt2Name+"'");
        return StatusCode::FAILURE;
      }
    }

    // debug printout
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "                            to   "
                << stateToMove->position() << endreq;
    }

  }

  return StatusCode::SUCCESS;
}
//====================================================================================================
