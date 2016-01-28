
//---------------------------------------------------------------------------------
/** @file RichDetailedTrSegMakerFromRecoTracks.cpp
 *
 * Implementation file for class : Rich::Rec::DetailedTrSegMakerFromRecoTracks
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//---------------------------------------------------------------------------------

// local
#include "RichDetailedTrSegMakerFromRecoTracks.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//===============================================================================
// Standard constructor, initializes variables
//===============================================================================
DetailedTrSegMakerFromRecoTracks::
DetailedTrSegMakerFromRecoTracks( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : BaseTrSegMakerFromRecoTracks ( type, name, parent ),
    m_nomZstates         ( 2*Rich::NRiches,    0      )
{
  using namespace Gaudi::Units;

  // job options

  declareProperty( "PrimaryTrackExtrapolator",
                   m_trExt1Name = "TrackRungeKuttaExtrapolator" );
  declareProperty( "BackupTrackExtrapolator",
                   m_trExt2Name = "TrackParabolicExtrapolator"  );

  // Always start state extrapolations from reference state.
  declareProperty( "ExtrapolateFromReference", m_extrapFromRef = false );

  // Minimum move in z to bother with
  declareProperty( "MinimumZMove", m_minZmove = 1*mm );

  // Nominal z positions of states at RICHes
  m_nomZstates[0] =   990*mm; // Place to look for Rich1 entry state
  m_nomZstates[1] =  2165*mm; // Place to look for Rich1 exit state
  m_nomZstates[2] =  9450*mm; // Place to look for Rich2 entry state
  m_nomZstates[3] = 11900*mm; // Place to look for Rich2 exit state
  declareProperty( "NominalStateZ", m_nomZstates );

  // tolerances on z positions
  // Was { 800*mm  , 2000*mm , 3000*mm }
  declareProperty( "ZTolerances", m_zTolerance = { 10*mm, 10*mm, 10*mm  } );

  // sanity checks on state information
  declareProperty( "ZSanityChecks", m_minStateDiff = { 1*mm, 25*mm, 50*mm } );

  // shifts for mirror correction
  declareProperty( "MirrorShiftCorr", m_mirrShift = { 35*cm, 150*cm } );

  // Type of track segments to create
  declareProperty( "SegmentType", m_trSegTypeJO = "AllStateVectors" );

  // min path length
  declareProperty( "MinRadiatorPathLength",
                   m_minRadLength = { 0*mm, 500*mm, 1500*mm } );

  // Recreate missing states using the state provider tool
  declareProperty( "CreateMissingStates", m_createMissingStates = true );

  // Flag to use TrackTraj to move states, instead of the extrapolator
  declareProperty( "UseTrackTraj", m_useTrackTraj = false );

  // Use the State provider instead of the extrapolator to move states
  declareProperty( "UseStateProvider", m_useStateProvider = false );

  // State checks ?
  declareProperty( "StateSanityCheck", m_checkStates = false );

  // Radiators to skip by track type, when using the track provider tool
  m_radsToSkip[LHCb::Track::Ttrack]   = { Rich::Aerogel, Rich::Rich1Gas };
  m_radsToSkip[LHCb::Track::Upstream] = { Rich::Rich2Gas };

  // Debug messages
  //setProperty( "OutputLevel", 1 );

}

//=============================================================================
// Destructor
//=============================================================================
DetailedTrSegMakerFromRecoTracks::~DetailedTrSegMakerFromRecoTracks() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode DetailedTrSegMakerFromRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = BaseTrSegMakerFromRecoTracks::initialize();
  if ( sc.isFailure() ) return sc;

  // load primary track extrapolator now if it is going to be used to perform
  // the state extrapolations   ( backup is loaded on-demand )
  if ( !m_useTrackTraj && !m_useStateProvider ) { primaryExtrapolator(); }

  // Load the state provider if it is to be used
  if ( m_createMissingStates || m_useStateProvider ) { stateProvider(); }

  // Get the RICH tools
  acquireTool( "RichRayTracing",          m_rayTracing, nullptr, true );
  acquireTool( "RichParticleProperties",  m_richPartProp );
  acquireTool( "RichRadiatorTool",        m_radTool, nullptr, true );

  // get Detector elements for RICH1 and RICH2
  m_rich[Rich::Rich1] = getDet<DeRich>( DeRichLocations::Rich1 );
  m_rich[Rich::Rich2] = getDet<DeRich>( DeRichLocations::Rich2 );

  // Radiators
  m_radiators.push_back( usedRads(Rich::Aerogel)  ?
                         getDet<DeRichRadiator>(DeRichLocations::Aerogel)  : nullptr );
  m_radiators.push_back( usedRads(Rich::Rich1Gas) ?
                         getDet<DeRichRadiator>(DeRichLocations::Rich1Gas) : nullptr );
  m_radiators.push_back( usedRads(Rich::Rich2Gas) ?
                         getDet<DeRichRadiator>(DeRichLocations::Rich2Gas) : nullptr );

  if ( m_useStateProvider )
  {
    _ri_debug << "Will use StateProvider instead of extrapolator to move states" << endmsg;
  }

  if ( m_useTrackTraj )
  {
    _ri_debug << "Will use TrackTraj instead of extrapolator to move states" << endmsg;
    if ( m_useStateProvider )
    {
      return Error( "Cannot use both TrackTraj and StateProvider at the same time !" );
    }
  }

  if ( m_extrapFromRef )
  {
    _ri_debug << "Will perform all track extrapolations from reference states" << endmsg;
    if ( m_useTrackTraj )
    {
      return Error( "TrackTraj and 'extrapolations from reference states' are incompatible options" );
    }
  }

  // Define the segment type
  if      ( "AllStateVectors" == m_trSegTypeJO )
  {
    _ri_debug << "Will create track segments using all State information" << endmsg;
    m_trSegType = LHCb::RichTrackSegment::UseAllStateVectors;
  }
  else if ( "Chord" == m_trSegTypeJO )
  {
    _ri_debug << "Will create track segments using the 'chord' direction definition" << endmsg;
    m_trSegType = LHCb::RichTrackSegment::UseChordBetweenStates;
  }
  else
  {
    return Error( "Unknown RichTrackSegment type " + m_trSegTypeJO );
  }

  _ri_debug << "Min radiator path lengths (aero/R1Gas/R2Gas) : "
            << m_minRadLength << " mm " << endmsg;

  return sc;
}

//=============================================================================
// Constructs the track segments for a given Track
//=============================================================================
int
DetailedTrSegMakerFromRecoTracks::
constructSegments( const ContainedObject * obj,
                   std::vector<LHCb::RichTrackSegment*>& segments ) const
{

  // Try to cast input data to required type for this implementation
  const LHCb::Track * track = dynamic_cast<const LHCb::Track *>(obj);
  if ( !track )
  {
    Warning("Input data object is not of type Track").ignore();
    return 0;
  }
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Analysing Track key=" << track->key()
            << " history=" << track->history()
            << " : " << track->states().size() << " States at z =";
    for ( const auto * S : track->states() )
    {
      if (S) debug() << " " << S->z();
    }
    debug() << endmsg;
  }

  // make sure vector is empty
  segments.clear();

  // Histogramming
  //RADIATOR_GLOBAL_POSITIONS_X;
  //RADIATOR_GLOBAL_POSITIONS_Y;

  // Loop over all radiators
  for ( const auto* radiator : m_radiators )
  {

    // is this radiator active ?
    if ( !radiator ) continue; // Means not in use

    // which radiator
    const Rich::RadiatorType rad = radiator->radiatorID();
    _ri_verbo << " Considering radiator " << rad << endmsg;

    // choose appropriate z start position for initial track states for this radiator
    const double zStart = ( Rich::Rich2Gas == rad ? m_nomZstates[2] : m_nomZstates[0] );

    // Get the track entry state points
    const LHCb::State * entryPStateRaw = &(track->closestState(zStart));
    if ( !entryPStateRaw ) { Error( "Problem getting track state" ).ignore(); continue; }
    //     richHisto2D( HID("rawEntry",rad), "Raw State Entry",
    //                  -xRadEntGlo[rad], xRadEntGlo[rad], 200,
    //                  -yRadEntGlo[rad], yRadEntGlo[rad], 200 )->fill( entryPStateRaw->x(),
    //                                                                  entryPStateRaw->y() );

    // check tolerance
    _ri_verbo << " -> Closest Entry State at z=" << entryPStateRaw->z() << "mm" << endmsg;
    const double entryTol = zStart - entryPStateRaw->z();
    //    richHisto1D( HID("rawEntryTol",rad), "Entry Tolerance", -3000, 3000, nBins1D() )->fill(entryTol);
    if ( fabs(entryTol) > m_zTolerance[rad] )
    {
      _ri_verbo << "  -> Entry State : Requested z=" << zStart << " found z="
                << entryPStateRaw->z() << " failed tolerance check dz="
                << m_zTolerance[rad] << endmsg;

      //       richHisto1D(  HID("entryZ",rad), "Entry Z Failed Tolerance",
      //                     -2000, 10000, nBins1D() )->fill(entryPStateRaw->z());
      //       richHisto2D( HID("rawEntryFailedTolCheck",rad), "Raw State Entry failed tolerence check",
      //                    -xRadEntGlo[rad], xRadEntGlo[rad], 200,
      //                    -yRadEntGlo[rad], yRadEntGlo[rad], 200 )->fill( entryPStateRaw->x(),
      //                                                                    entryPStateRaw->y() );
      entryPStateRaw = nullptr;
    }

    // Failed to find the state, so try with the state provider....
    if ( !entryPStateRaw && m_createMissingStates && !skipByType(track,rad) )
    {
      const StatusCode sc = stateProvider()->state( m_states[0], *track, zStart );
      if ( sc.isSuccess() )
      {
        entryPStateRaw = &m_states[0];
        if ( msgLevel(MSG::DEBUG) )
        { debug() << "   -> Found entry state at z=" << zStart << "mm via StateProvider" << endmsg; }
      }
      else
      {
        _ri_verbo << "   -> Failed to get entry State at z=" << zStart
                  << "mm via StateProvider" << endmsg;
      }
    }
    // if still no state, skip this track
    if ( !entryPStateRaw ) continue;

    // check above electron threshold
    if ( m_richPartProp->thresholdMomentum(Rich::Electron,rad) > entryPStateRaw->p() )
    {
      _ri_verbo << "  -> Below electron cherenkov threshold -> reject" << endmsg;
      continue;
    }

    // choose appropriate z end position for initial track states for this radiator
    const double zEnd = ( Rich::Rich2Gas == rad ? m_nomZstates[3] :
                          Rich::Aerogel  == rad ? m_nomZstates[0] :
                          m_nomZstates[1] );

    // Get the track exit state points
    const LHCb::State * exitPStateRaw = &(track->closestState(zEnd));
    if ( !exitPStateRaw ) { Error( "Problem getting track state" ).ignore(); continue; }
    //     richHisto2D( HID("rawExit",rad), "Raw State Exit",
    //                  -xRadExitGlo[rad], xRadExitGlo[rad], 200,
    //                  -yRadExitGlo[rad], yRadExitGlo[rad], 200 )->fill( exitPStateRaw->x(),
    //                                                                    exitPStateRaw->y() );

    // check tolerance
    _ri_verbo << " -> Closest Exit State at  z=" << exitPStateRaw->z() << "mm" << endmsg;
    const double exitTol = zEnd - exitPStateRaw->z();
    //    richHisto1D( HID("rawExitTol",rad), "Exit Tolerance", -3000, 3000, nBins1D() )->fill(exitTol);
    if ( fabs(exitTol) > m_zTolerance[rad] )
    {
      _ri_verbo << "  -> Exit State  : Requested z=" << zEnd << " found z="
                << exitPStateRaw->z() << " failed tolerance check dz="
                << m_zTolerance[rad] << endmsg;

      //       richHisto1D(  HID("exitZ",rad), "Exit Z Failed Tolerance",
      //                     -2000, 10000, nBins1D() )->fill(exitPStateRaw->z());
      //       richHisto2D( HID("rawExit",rad), "Raw State Exit failed tolerance check",
      //                    -xRadExitGlo[rad], xRadExitGlo[rad], 200,
      //                    -yRadExitGlo[rad], yRadExitGlo[rad], 200 )->fill( exitPStateRaw->x(),
      //                                                                      exitPStateRaw->y() );
      exitPStateRaw = nullptr;
    }

    // Failed to find the state, so try with the state provider....
    if ( !exitPStateRaw && m_createMissingStates && !skipByType(track,rad) )
    {
      const StatusCode sc = stateProvider()->state( m_states[1], *track, zEnd );
      if ( sc.isSuccess() )
      {
        exitPStateRaw = &m_states[1];
        if ( msgLevel(MSG::DEBUG) )
        { debug() << "    -> Found exit state at z=" << zEnd << "mm via StateProvider" <<  endmsg; }
      }
      else
      {
        _ri_verbo << "    -> Failed to get exit State at z=" << zEnd
                  << "mm via StateProvider" << endmsg;
      }
    }
    // if still no state, skip this track
    if ( !exitPStateRaw ) continue;

    // Check for strange states
    if ( UNLIKELY(m_checkStates) )
    {
      checkState( entryPStateRaw, rad );
      checkState( exitPStateRaw,  rad );
    }

    // Construct TrackTraj if it is to be used
    std::unique_ptr<LHCb::TrackTraj> trackTraj( !m_useTrackTraj ? nullptr :
                                                new LHCb::TrackTraj(*track,magFieldSvc()) );
    m_trackTraj = trackTraj.get();

    // Clone entry state
    std::unique_ptr<LHCb::State> entryPState( entryPStateRaw->clone() );
    if ( !entryPState ) { Warning("Failed to clone entry State").ignore(); continue; }

    // Clone exit state (for aero use entrance point)
    std::unique_ptr<LHCb::State> exitPState ( Rich::Aerogel == rad ?
                                              entryPStateRaw->clone() : exitPStateRaw->clone() );
    if ( !exitPState ) { Warning("Failed to clone exit State").ignore(); continue; }

    _ri_verbo << "  Found appropriate initial start/end States" << endmsg
              << "   EntryPos : "
              << entryPState->position() << endmsg
              << "   EntryDir : "
              << entryPState->slopes() << endmsg
              << "   ExitPos  : "
              << exitPState->position() << endmsg
              << "   ExitDir  : "
              << exitPState->slopes() << endmsg;

    //     richHisto2D( HID("initialEntry",rad), "Initial Entry",
    //                  -xRadEntGlo[rad], xRadEntGlo[rad], 200,
    //                  -yRadEntGlo[rad], yRadEntGlo[rad], 200 )->fill( entryPState->x(),
    //                                                                  entryPState->y() );
    //     richHisto2D( HID("initialExit",rad), "Initial Exit",
    //                  -xRadExitGlo[rad], xRadExitGlo[rad], 200,
    //                  -yRadExitGlo[rad], yRadExitGlo[rad], 200 )->fill( exitPState->x(),
    //                                                                    exitPState->y() );

    // use state closest to the entry point in radiator
    Gaudi::XYZPoint entryPoint1;
    Rich::RadIntersection::Vector intersects1;
    bool entryStateOK = false;
    if ( getNextInterPoint( entryPState->position(),
                            entryPState->slopes(),
                            radiator, entryPoint1 ) )
    {
      // extrapolate state to the correct z
      if ( moveState( *entryPState, *track, entryPoint1.z(), entryPStateRaw ) )
      {
        // find radiator entry and exit points
        if ( entryPState &&
             fabs(entryPState->z() - entryPoint1.z()) < m_zTolerance[rad] )
        {
          if ( getRadIntersections( entryPState->position(),
                                    entryPState->slopes(),
                                    radiator,
                                    intersects1 ) > 0 )
          {
            entryStateOK = true;
            entryPoint1 = intersects1.front().entryPoint();
            _ri_verbo << "      Entry state rad intersection points "
                      << intersects1 << endmsg;
          }
        }
      }
    }
    else
    {
      _ri_verbo << "Failed to intersect entry state" << endmsg;
    }

    // If gas radiator try and use exit state to get exit point more precisely
    bool exitStateOK = false;
    Gaudi::XYZPoint entryPoint2;
    Rich::RadIntersection::Vector intersects2;
    if ( rad != Rich::Aerogel )
    {
      if ( getNextInterPoint(  exitPState->position(),
                               -exitPState->slopes(),
                               radiator, entryPoint2 ) )
      {
        // extrapolate state to the correct z
        if ( moveState( *exitPState, *track, entryPoint2.z(), exitPStateRaw ) )
        {
          // find radiator entry and exit points
          if ( exitPState &&
               fabs(exitPState->z() - entryPoint2.z()) < m_zTolerance[rad] )
          {
            if ( getRadIntersections( exitPState->position(),
                                      exitPState->slopes(),
                                      radiator,
                                      intersects2 ) > 0 )
            {
              exitStateOK = true;
              entryPoint2 = intersects2.front().entryPoint();
              _ri_verbo << "      Exit state rad intersection points "
                        << intersects2 << endmsg;
            }
          }
        }
      }
      else
      {
        _ri_verbo << "Failed to intersect exit state" << endmsg;
      }
    } // end !aerogel if

    //     if ( !entryStateOK )
    //     {
    //       richHisto2D( HID("entryStateFailedRadInt",rad), "Entry State Failed Radiator Intersections",
    //                    -xRadEntGlo[rad], xRadEntGlo[rad], 200,
    //                    -yRadEntGlo[rad], yRadEntGlo[rad], 200 )->fill( entryPState->x(),
    //                                                                    entryPState->y() );
    //     }
    //     if ( !exitStateOK )
    //     {
    //       richHisto2D( HID("exitStateFailedRadInt",rad), "Exit State Failed Radiator Intersections",
    //                    -xRadExitGlo[rad], xRadExitGlo[rad], 200,
    //                    -yRadExitGlo[rad], yRadExitGlo[rad], 200 )->fill( exitPState->x(),
    //                                                                      exitPState->y() );
    //     }

    // transport entry and exit states to best points
    bool sc = false;
    if ( entryStateOK && exitStateOK )
    {
      _ri_verbo << "  Both states OK : Zentry=" << entryPoint1.z()
                << " Zexit=" << intersects2.back().exitPoint().z()
                << endmsg;

      // make sure at current z positions
      _ri_verbo << "  Checking entry point is at final z=" << entryPoint1.z() << endmsg;
      const bool sc1 = moveState( *entryPState, *track,
                                  entryPoint1.z(), entryPStateRaw );
      _ri_verbo << "  Checking exit point is at final z=" << intersects2.back().exitPoint().z()
                << endmsg;
      const bool sc2 = moveState( *exitPState, *track,
                                  intersects2.back().exitPoint().z(), exitPStateRaw );
      sc = sc1 && sc2;

    }
    else if ( entryStateOK )
    {
      _ri_verbo << "  Entry state OK : Zentry=" << entryPoint1.z()
                << " Zexit=" << intersects1.back().exitPoint().z() << endmsg;

      if ( Rich::Aerogel != rad )
      {
        // delete current exit state and replace with clone of raw entrance state
        exitPState.reset( entryPStateRaw->clone() );
        if ( !exitPState ) { Warning("Failed to clone State").ignore(); continue; }
      }

      // make sure at current z positions
      _ri_verbo << "  Checking entry point is at final z= " << entryPoint1.z() << endmsg;
      const bool sc1 = moveState( *entryPState, *track, entryPoint1.z(), entryPStateRaw );
      _ri_verbo << "  Checking exit point is at final z= " << intersects1.back().exitPoint().z()
                << endmsg;
      const bool sc2 = moveState( *exitPState, *track,
                                  intersects1.back().exitPoint().z(), exitPStateRaw );
      sc = sc1 && sc2;

    }
    else if ( exitStateOK )
    {
      _ri_verbo << "  Exit state OK  : Zentry=" << entryPoint2.z()
                << " Zexit=" << intersects2.back().exitPoint().z() << endmsg;

      // delete current entry state and replace with clone of raw entrance state
      entryPState.reset( exitPStateRaw->clone() );
      if ( !entryPState ) { Warning("Failed to clone State").ignore(); continue; }

      // make sure at current z positions
      _ri_verbo << "  Checking entry point is at final z= " << entryPoint2.z() << endmsg;
      const bool sc1 = moveState( *entryPState, *track, entryPoint2.z(), entryPStateRaw );
      _ri_verbo << "  Checking exit point is at final z= " << intersects2.back().exitPoint().z()
                << endmsg;
      const bool sc2 = moveState( *exitPState, *track,
                                  intersects2.back().exitPoint().z(), exitPStateRaw );
      sc = sc1 && sc2;

    }
    else
    {
      // no valid extrapolations, so quit skip this track/radiator
      _ri_verbo << "  Both states failed" << endmsg;
      continue;
    }

    // Test final status code
    if ( !sc )
    {
      _ri_verbo << "    --> Failed to use state information. Quitting." << endmsg;
      //       richHisto2D( HID("entryStateFailedToUseState",rad), "Entry State Failed to use state information",
      //                    -xRadEntGlo[rad], xRadEntGlo[rad], 200,
      //                    -yRadEntGlo[rad], yRadEntGlo[rad], 200 )->fill( entryPState->x(),
      //                                                                    entryPState->y() );
      //       richHisto2D( HID("exitStateFailedToUseState",rad), "Exit State Failed to use state information",
      //                    -xRadExitGlo[rad], xRadExitGlo[rad], 200,
      //                    -yRadExitGlo[rad], yRadExitGlo[rad], 200 )->fill( exitPState->x(),
      //                                                                      exitPState->y() );
      continue;
    }

    //     richHisto2D( HID("postEntry",rad), "Post Entry",
    //                  -xRadEntGlo[rad], xRadEntGlo[rad], 200,
    //                  -yRadEntGlo[rad], yRadEntGlo[rad], 200 )->fill( entryPState->x(),
    //                                                                  entryPState->y() );
    //     richHisto2D( HID("postExit",rad), "Post Exit",
    //                  -xRadExitGlo[rad], xRadExitGlo[rad], 200,
    //                  -yRadExitGlo[rad], yRadExitGlo[rad], 200 )->fill( exitPState->x(),
    //                                                                    exitPState->y() );

    //---------------------------------------------------------------------------------------------
    // Correction for beam pipe intersections
    if ( checkBeamPipe(rad) )
    {

      // Get intersections with beam pipe using DeRich object
      const Gaudi::XYZVector vect = exitPState->position() - entryPState->position();
      Gaudi::XYZPoint inter1, inter2;
      const DeRichBeamPipe::BeamPipeIntersectionType intType
        = deBeam(rad)->intersectionPoints( entryPState->position(), vect, inter1, inter2 );

      _ri_verbo << "  --> Beam Intersects : " << intType << " : "
                << inter1 << " " << inter2 << endmsg;

      sc = true;
      if ( intType == DeRichBeamPipe::NoIntersection )
      {
        _ri_verbo << "   --> No beam intersections -> No corrections needed" << endmsg;
      }
      else if ( intType == DeRichBeamPipe::FrontAndBackFace )
      {
        _ri_verbo << "   --> Inside beam pipe -> Reject segment" << endmsg;
        //         richHisto2D( HID("entryStateInsideBeamPipe",rad), "Entry State Inside beampipe",
        //                      -xRadEntGlo[rad], xRadEntGlo[rad], 200,
        //                      -yRadEntGlo[rad], yRadEntGlo[rad], 200 )->fill( entryPState->x(),
        //                                                                      entryPState->y() );
        //         richHisto2D( HID("exitStateInsideBeamPipe",rad), "Exit State Inside beampipe",
        //                      -xRadExitGlo[rad], xRadExitGlo[rad], 200,
        //                      -yRadExitGlo[rad], yRadExitGlo[rad], 200 )->fill( exitPState->x(),
        //                                                                        exitPState->y() );
        continue;
      }
      else if ( intType == DeRichBeamPipe::FrontFaceAndCone )
      {
        // Update entry point to exit point on cone
        _ri_verbo << "   --> Correcting entry point to point on cone" << endmsg;
        sc = moveState( *entryPState, *track, inter2.z(), entryPStateRaw );
      }
      else if ( intType == DeRichBeamPipe::BackFaceAndCone )
      {
        // Update exit point to entry point on cone
        _ri_verbo << "   --> Correcting exit point to point on cone" << endmsg;
        sc = moveState( *exitPState, *track, inter1.z(), exitPStateRaw );
      }
      if ( !sc )
      {
        _ri_verbo << "    --> Error fixing radiator entry/exit points for beam-pipe. Quitting."
                  << endmsg;
        //         richHisto2D( HID("entryStateErrFixRadForBeamP",rad),
        //                      "Entry State Error fixing radiator entry/exit points for beam-pipe",
        //                      -xRadEntGlo[rad], xRadEntGlo[rad], 200,
        //                      -yRadEntGlo[rad], yRadEntGlo[rad], 200 )->fill( entryPState->x(),
        //                                                                      entryPState->y() );
        //         richHisto2D( HID("exitStateErrFixRadForBeamP",rad),
        //                      "Exit State Error fixing radiator entry/exit points for beam-pipe",
        //                      -xRadExitGlo[rad], xRadExitGlo[rad], 200,
        //                      -yRadExitGlo[rad], yRadExitGlo[rad], 200 )->fill( exitPState->x(),
        //                                                                        exitPState->y() );
        continue;
      }

    }
    //---------------------------------------------------------------------------------------------

    //     richHisto2D( HID("entryPostBeampipe",rad), "Entry Post Beampipe",
    //                  -xRadEntGlo[rad], xRadEntGlo[rad], 200,
    //                  -yRadEntGlo[rad], yRadEntGlo[rad], 200 )->fill( entryPState->x(),
    //                                                                  entryPState->y() );
    //     richHisto2D( HID("exitPostBeampipe",rad), "Exit Post Beampipe",
    //                  -xRadExitGlo[rad], xRadExitGlo[rad], 200,
    //                  -yRadExitGlo[rad], yRadExitGlo[rad], 200 )->fill( exitPState->x(),
    //                                                                    exitPState->y() );

    //---------------------------------------------------------------------------------------------
    // a special hack for the Rich1Gas - since the aerogel volume
    // is placed INSIDE the Rich1Gas, the default entry point is wrong.
    //---------------------------------------------------------------------------------------------
    if ( Rich::Rich1Gas == rad ) fixRich1GasEntryPoint( *track, *entryPState, entryPStateRaw );
    //---------------------------------------------------------------------------------------------

    //     richHisto2D( HID("entryPostR1GasFix",rad), "Entry Post R1 Gas Fix",
    //                  -xRadEntGlo[rad], xRadEntGlo[rad], 200,
    //                  -yRadEntGlo[rad], yRadEntGlo[rad], 200 )->fill( entryPState->x(),
    //                                                                  entryPState->y() );

    //---------------------------------------------------------------------------------------------
    // check for intersection with spherical mirror for gas radiators
    // and if need be correct exit point accordingly
    //---------------------------------------------------------------------------------------------
    if ( Rich::Aerogel != rad  ) correctRadExitMirror( radiator, *track,
                                                       *exitPState, exitPStateRaw );
    //---------------------------------------------------------------------------------------------

    //     richHisto2D( HID("exitPostMirrCorr",rad), "Exit Post Mirror Exit Correction",
    //                  -xRadExitGlo[rad], xRadExitGlo[rad], 200,
    //                  -yRadExitGlo[rad], yRadExitGlo[rad], 200 )->fill( exitPState->x(),
    //                                                                    exitPState->y() );

    //---------------------------------------------------------------------------------------------
    // Final check that info is reasonable
    //---------------------------------------------------------------------------------------------
    const bool Zcheck     = entryPState->z() > exitPState->z();
    const bool ZdiffCheck = (exitPState->z()-entryPState->z()) < m_minStateDiff[rad];
    //counter( "Entry state after exit state for " + Rich::text(rad)     ) += (int)Zcheck;
    //counter( "Track states for " + Rich::text(rad) + " too close in z" ) += (int)ZdiffCheck;
    if ( Zcheck || ZdiffCheck )
    {
      //       richHisto2D( HID("entryFailedFinalCheck",rad), "Entry Final Check",
      //                    -xRadEntGlo[rad], xRadEntGlo[rad], 200,
      //                    -yRadEntGlo[rad], yRadEntGlo[rad], 200 )->fill( entryPState->x(),
      //                                                                    entryPState->y() );
      //       richHisto2D( HID("exitFailedFinalCheck",rad), "Exit Final Check",
      //                    -xRadExitGlo[rad], xRadExitGlo[rad], 200,
      //                    -yRadExitGlo[rad], yRadExitGlo[rad], 200 )->fill( exitPState->x(),
      //                                                                      exitPState->y() );
      continue;
    }
    //---------------------------------------------------------------------------------------------

    //---------------------------------------------------------------------------------------------
    // Radiator path length cut
    //---------------------------------------------------------------------------------------------
    if ( (exitPState->position()-entryPState->position()).Mag2() < m_minRadLength[rad]*m_minRadLength[rad] )
    {
      _ri_verbo << "    --> Path length too short -> rejecting segment" << endmsg;
      //       richHisto2D( HID("entryTooShortZ",rad), "Entry Too SHort In Z",
      //                    -xRadEntGlo[rad], xRadEntGlo[rad], 200,
      //                    -yRadEntGlo[rad], yRadEntGlo[rad], 200 )->fill( entryPState->x(),
      //                                                                    entryPState->y() );
      //       richHisto2D( HID("exitTooShortZ",rad), "Exit Too SHort In Z",
      //                    -xRadExitGlo[rad], xRadExitGlo[rad], 200,
      //                    -yRadExitGlo[rad], yRadExitGlo[rad], 200 )->fill( exitPState->x(),
      //                                                                      exitPState->y() );
      continue;
    }
    //---------------------------------------------------------------------------------------------

    //     richHisto2D( HID("entryBeforeFinal",rad), "Entry Before Final",
    //                  -xRadEntGlo[rad], xRadEntGlo[rad], 200,
    //                  -yRadEntGlo[rad], yRadEntGlo[rad], 200 )->fill( entryPState->x(),
    //                                                                  entryPState->y() );
    //     richHisto2D( HID("exitBeforeFinal",rad), "Exit Before Final",
    //                  -xRadExitGlo[rad], xRadExitGlo[rad], 200,
    //                  -yRadExitGlo[rad], yRadExitGlo[rad], 200 )->fill( exitPState->x(),
    //                                                                    exitPState->y() );

    // Create final entry and exit state points and momentum vectors
    auto entryPoint( entryPState->position() );
    auto entryStateMomentum( entryPState->slopes() );
    entryStateMomentum *= entryPState->p()/std::sqrt(entryStateMomentum.Mag2());
    auto exitPoint(exitPState->position());
    auto exitStateMomentum( exitPState->slopes() );
    exitStateMomentum *= exitPState->p()/std::sqrt(exitStateMomentum.Mag2());

    // ================================== NOTE ==========================================
    //
    // From now on we might use move semantics, as we no longer care about the
    // various data objects once they have been used to create a RichTrackSegment object.
    // So must be careful not 'move' something before it is finished with....
    //
    /// Moving is a test here...
    //
    // ==================================================================================

    // Update final intersections
    Rich::RadIntersection::Vector & final_intersects = ( entryStateOK ? intersects1 : intersects2 );
    // final_intersects.front().setEntryPoint    ( entryPoint );
    // final_intersects.front().setEntryMomentum ( entryStateMomentum );
    // final_intersects.back().setExitPoint      ( exitPoint );
    // final_intersects.back().setExitMomentum   ( exitStateMomentum );
    final_intersects.front().setEntryPoint    ( std::move(entryPoint) );
    final_intersects.front().setEntryMomentum ( std::move(entryStateMomentum) );
    final_intersects.back().setExitPoint      ( std::move(exitPoint) );
    final_intersects.back().setExitMomentum   ( std::move(exitStateMomentum) );

    // Errors for entry and exit states
    LHCb::RichTrackSegment::StateErrors entryErrs ( entryPState->errX2(),
                                                    entryPState->errY2(),
                                                    entryPState->errTx2(),
                                                    entryPState->errTy2(),
                                                    entryPState->errP2() );
    LHCb::RichTrackSegment::StateErrors exitErrs  ( exitPState->errX2(),
                                                    exitPState->errY2(),
                                                    exitPState->errTx2(),
                                                    exitPState->errTy2(),
                                                    exitPState->errP2() );
    // Check for strange states
    if ( UNLIKELY(m_checkStates) )
    {
      checkState( entryPState.get(), rad );
      checkState( exitPState.get(),  rad );
    }

    // print out final points
    _ri_verbo << "  Found final points :-" << endmsg
              << "   Entry : Pnt=" << final_intersects.front().entryPoint()
              << " Mom=" << final_intersects.front().entryMomentum()
              << " Ptot=" << std::sqrt(final_intersects.front().entryMomentum().Mag2()) << endmsg;
    _ri_verbo << "   Exit  : Pnt=" << final_intersects.back().exitPoint()
              << " Mom=" << final_intersects.back().exitMomentum()
              << " Ptot=" << std::sqrt(final_intersects.back().exitMomentum().Mag2()) << endmsg;

    //     richHisto2D( HID("selectedEntry",rad), "Selected Entry",
    //                  -xRadEntGlo[rad], xRadEntGlo[rad], 200,
    //                  -yRadEntGlo[rad], yRadEntGlo[rad], 200 )->fill( entryPState->x(),
    //                                                                  entryPState->y() );
    //     richHisto2D( HID("selectedExit",rad), "Selected Exit",
    //                  -xRadExitGlo[rad], xRadExitGlo[rad], 200,
    //                  -yRadExitGlo[rad], yRadExitGlo[rad], 200 )->fill( exitPState->x(),
    //                                                                    exitPState->y() );


    try
    {

      // For gas radiators transport entry state to mid point to create middle point
      // information for three point RichTrackSegment constructor
      if ( rad != Rich::Aerogel )
      {

        // data for middle state
        LHCb::RichTrackSegment::StateErrors midErrs;
        Gaudi::XYZPoint midPoint;
        Gaudi::XYZVector midMomentum;
        const bool OK = createMiddleInfo( *track, rad,
                                          *entryPState, entryPStateRaw,
                                          *exitPState,  exitPStateRaw,
                                          midPoint, midMomentum, midErrs );

        if ( OK )
        {
          // Using this information, make radiator segment
          // this version uses 3 states and thus incorporates some concept of track curvature
          // segments.push_back( new LHCb::RichTrackSegment( m_trSegType,
          //                                                 final_intersects,
          //                                                 midPoint, 
          //                                                 midMomentum,
          //                                                 rad, radiator->rich(),
          //                                                 entryErrs,
          //                                                 midErrs,
          //                                                 exitErrs ) );
          segments.push_back( new LHCb::RichTrackSegment( m_trSegType,
                                                          std::move(final_intersects),
                                                          std::move(midPoint), 
                                                          std::move(midMomentum),
                                                          rad, radiator->rich(),
                                                          std::move(entryErrs),
                                                          std::move(midErrs),
                                                          std::move(exitErrs) ) );
        }
        else
        {
          // Using this information, make radiator segment
          // this version uses 2 states and thus forces a straight line approximation
          // segments.push_back( new LHCb::RichTrackSegment( m_trSegType,
          //                                                 final_intersects,
          //                                                 rad, radiator->rich(),
          //                                                 entryErrs,
          //                                                 exitErrs ) );
          segments.push_back( new LHCb::RichTrackSegment( m_trSegType,
                                                          std::move(final_intersects),
                                                          rad, radiator->rich(),
                                                          std::move(entryErrs),
                                                          std::move(exitErrs) ) );
        }

      }
      else
      {

        // Using this information, make radiator segment
        // this version uses 2 states and thus forces a straight line approximation
        // segments.push_back( new LHCb::RichTrackSegment( m_trSegType,
        //                                                 final_intersects,
        //                                                 rad, radiator->rich(),
        //                                                 entryErrs,
        //                                                 exitErrs ) );
        segments.push_back( new LHCb::RichTrackSegment( m_trSegType,
                                                        std::move(final_intersects),
                                                        rad, radiator->rich(),
                                                        std::move(entryErrs),
                                                        std::move(exitErrs) ) );

      }

    }
    catch ( const std::exception & excpt )
    {
      Warning( "Exception whilst creating RichTrackSegment '"+std::string(excpt.what())+"'" ).ignore();
    }

  } // end loop over radiators

  // Final printout of states, to see if anything has changed ...
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Finished with Track key=" << track->key()
              << " history=" << track->history()
              << " : " << track->states().size() << " States at z =";
    for ( std::vector<LHCb::State*>::const_iterator iS = track->states().begin();
          iS != track->states().end(); ++iS )
    {
      if (*iS) verbose() << " " << (*iS)->z();
    }
    verbose() << endmsg;
  }

  // return value is number of segments formed
  return segments.size();
}
//====================================================================================================

//====================================================================================================
// creates middle point info
bool
DetailedTrSegMakerFromRecoTracks::
createMiddleInfo( const LHCb::Track & track,
                  const Rich::RadiatorType rad,
                  LHCb::State & fState,
                  const LHCb::State * fStateRef,
                  LHCb::State & lState,
                  const LHCb::State * lStateRef,
                  Gaudi::XYZPoint & midPoint,
                  Gaudi::XYZVector & midMomentum,
                  LHCb::RichTrackSegment::StateErrors & errors ) const
{
  _ri_verbo << "   --> Creating middle point information" << endmsg;

  // middle point z position
  const double midZ = (fState.position().z()+lState.position().z())/2;

  // move start state to this z
  const bool moveFirst = moveState( fState, track, midZ, fStateRef );

  // move end state to this z
  const bool moveLast = ( Rich::Rich1Gas == rad ?
                          moveState( lState, track, midZ, lStateRef ) : false );

  if ( moveFirst && moveLast )
  {
    midPoint     = fState.position() + (lState.position()-fState.position())/2;
    midMomentum  = (fState.slopes()+lState.slopes())/2;
    midMomentum *= (fState.p()+lState.p()) / (2.0*std::sqrt(midMomentum.Mag2()));
    errors = LHCb::RichTrackSegment::StateErrors( (fState.errX2()+lState.errX2())/2,
                                                  (fState.errY2()+lState.errY2())/2,
                                                  (fState.errTx2()+lState.errTx2())/2,
                                                  (fState.errTy2()+lState.errTy2())/2,
                                                  (fState.errP2()+lState.errP2())/2 );
  }
  else if ( moveFirst )
  {
    midPoint     = fState.position();
    midMomentum  = fState.slopes();
    midMomentum *= fState.p() / std::sqrt(midMomentum.Mag2());
    errors = LHCb::RichTrackSegment::StateErrors( fState.errX2(),
                                                  fState.errY2(),
                                                  fState.errTx2(),
                                                  fState.errTy2(),
                                                  fState.errP2() );
  }
  else if ( moveLast )
  {
    midPoint     = lState.position();
    midMomentum  = lState.slopes();
    midMomentum *= lState.p() / std::sqrt(midMomentum.Mag2());
    errors = LHCb::RichTrackSegment::StateErrors( lState.errX2(),
                                                  lState.errY2(),
                                                  lState.errTx2(),
                                                  lState.errTy2(),
                                                  lState.errP2() );
  }

  return ( moveFirst || moveLast );
}
//====================================================================================================

//====================================================================================================
// Get radiator intersections
unsigned int
DetailedTrSegMakerFromRecoTracks::
getRadIntersections( const Gaudi::XYZPoint  & point,
                     const Gaudi::XYZVector & direction,
                     const DeRichRadiator * rad,
                     Rich::RadIntersection::Vector & intersections ) const
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
// Short cut method to get just first intersection point
bool
DetailedTrSegMakerFromRecoTracks::
getNextInterPoint( const Gaudi::XYZPoint&   point,
                   const Gaudi::XYZVector&  direction,
                   const DeRichRadiator * rad,
                   Gaudi::XYZPoint& interP ) const
{
  Rich::RadIntersection::Vector intersections;
  bool OK = false;
  if ( 0 < getRadIntersections(point,direction,rad,intersections) )
  {
    OK = true;
    interP = intersections.front().entryPoint();
  }
  return OK;
}
//====================================================================================================

//====================================================================================================
// fixup Rich1Gas entry point
void
DetailedTrSegMakerFromRecoTracks::fixRich1GasEntryPoint( const LHCb::Track & track,
                                                         LHCb::State & state,
                                                         const LHCb::State * refState ) const
{
  if ( m_radiators[Rich::Aerogel] )
  {
    Rich::RadIntersection::Vector intersections;
    if ( 0 < getRadIntersections ( state.position(),
                                   state.slopes(),
                                   m_radiators[Rich::Aerogel],
                                   intersections ) )
    {
      const Gaudi::XYZPoint & aerogelExitPoint = intersections.back().exitPoint();
      if ( aerogelExitPoint.z() > state.z() )
      {
        _ri_verbo << "   Correcting Rich1Gas entry point" << endmsg;
        const bool sc = moveState( state, track, aerogelExitPoint.z(), refState );
        if ( !sc ) Warning( "Problem correcting RICH1Gas entry point for aerogel" ).ignore();
      }
    }
  }
}
//====================================================================================================

//====================================================================================================
void
DetailedTrSegMakerFromRecoTracks::correctRadExitMirror( const DeRichRadiator* radiator,
                                                        const LHCb::Track & track,
                                                        LHCb::State & state,
                                                        const LHCb::State * refState ) const
{
  _ri_verbo << "   --> Attempting Correction to exit point for spherical mirror" << endmsg;

  bool sc = true;

  // get rich information
  const Rich::DetectorType rich = radiator->rich();

  // initial z position of state
  const double initialZ = state.z();

  // move state to be on the inside of the mirror
  _ri_verbo << "    --> Moving state first to be inside mirror" << endmsg;
  sc = sc && moveState( state, track, state.z() - m_mirrShift[rich], refState );
  bool correct = false;

  // find mirror intersection using the reflect method
  Gaudi::XYZPoint intersection ( state.position() );
  Gaudi::XYZVector tempDir     ( state.slopes()   );

  if ( m_rayTracing->reflectSpherical( intersection,
                                       tempDir,
                                       m_rich[rich]->nominalCentreOfCurvature(m_rich[rich]->side(intersection)),
                                       m_rich[rich]->sphMirrorRadius() ) )
  {
    if ( intersection.z() < initialZ && radiator->geometry()->isInside(intersection) )
    {
      correct = true;
    }
  }

  // finally, update state
  if ( correct )
  {
    _ri_verbo << "    --> Found correction is needed" << endmsg;
    sc = sc && moveState( state, track, intersection.z(), refState );
  }
  else
  {
    _ri_verbo << "    --> Found correction not needed. Moving back to original position" << endmsg;
    sc = sc && moveState( state, track, initialZ, refState );
  }

  if ( !sc ) Warning( "Problem correcting segment exit to mirror intersection" ).ignore();
}
//====================================================================================================

//====================================================================================================
bool
DetailedTrSegMakerFromRecoTracks::moveState( LHCb::State & stateToMove,
                                             const LHCb::Track& track,
                                             const double z,
                                             const LHCb::State * refState ) const
{
  bool OK = true;

  // Check if requested move is big enough to bother with
  if ( fabs( stateToMove.z() - z ) > m_minZmove )
  {

    // verbose printout
    _ri_verbo << "    --> Extrapolating state from "
              << stateToMove.position() << endmsg;

    if ( UNLIKELY( m_extrapFromRef && refState ) )
    {
      // Delete current working state and start fresh from reference state
      stateToMove = *refState;
      _ri_verbo << "      --> Using reference state  "
                << stateToMove.position() << endmsg;
    }

    // Use State provider to move the state
    if ( m_useStateProvider )
    {
      //if ( !stateProvider()->state(stateToMove,track,z) )
      if ( !stateProvider()->stateFromTrajectory(stateToMove,track,z) )
      {
        Warning( "Failed to move state using StateProvider" ).ignore();
        OK = false;
      }
    }
    // Use Track Traj ?
    else if ( UNLIKELY(m_useTrackTraj) )
    {
      stateToMove = m_trackTraj->state(z);
    }
    // Use original extrapolator
    else
    {
      // try first with the primary extrapolator
      if ( !primaryExtrapolator()->propagate(stateToMove,z) )
      {
        // if that fails, try the backup one
        if ( backupExtrapolator()->propagate(stateToMove,z) )
        {
          Warning( "'"+m_trExt1Name+"' failed -> successfully reverted to '"+
                   m_trExt2Name+"'",StatusCode::SUCCESS ).ignore();
        }
        else
        {
          // Both failed ...
          Warning( "Failed to extrapolate state using either '"+
                   m_trExt1Name+"' or '"+m_trExt2Name+"'" ).ignore();
          OK =  false;
        }
      }
    }

    // verbose printout
    _ri_verbo << "                            to   "
              << stateToMove.position() << endmsg;

  }

  return OK;
}
//====================================================================================================

//====================================================================================================
void DetailedTrSegMakerFromRecoTracks::checkState( const LHCb::State * state,
                                                   const Rich::RadiatorType rad ) const
{
  if ( state )
  {
    if ( state->errX2() < 0 )
    {
      Warning( Rich::text(rad) + " State has negative errX^2", StatusCode::SUCCESS ).ignore();
    }
    if ( state->errY2() < 0 )
    {
      Warning( Rich::text(rad) + " State has negative errY^2", StatusCode::SUCCESS ).ignore();
    }
    if ( state->errTx2() < 0 )
    {
      Warning( Rich::text(rad) + " State has negative errTx^2", StatusCode::SUCCESS ).ignore();
    }
    if ( state->errTy2() < 0 )
    {
      Warning( Rich::text(rad) + " State has negative errTy^2", StatusCode::SUCCESS ).ignore();
    }
    if ( state->errP2() < 0 )
    {
      Warning( Rich::text(rad) + " State has negative errP^2", StatusCode::SUCCESS ).ignore();
    }
  }
}
//====================================================================================================

DECLARE_TOOL_FACTORY( DetailedTrSegMakerFromRecoTracks )
