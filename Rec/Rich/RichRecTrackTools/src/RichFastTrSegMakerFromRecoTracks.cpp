
//-----------------------------------------------------------------------------
/** @file RichFastTrSegMakerFromRecoTracks.cpp
 *
 * Implementation file for class : Rich::Rec::FastTrSegMakerFromRecoTracks
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 23/08/2004
 */
//-----------------------------------------------------------------------------

// local
#include "RichFastTrSegMakerFromRecoTracks.h"

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

DECLARE_TOOL_FACTORY( FastTrSegMakerFromRecoTracks )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FastTrSegMakerFromRecoTracks::
FastTrSegMakerFromRecoTracks( const std::string& type,
                              const std::string& name,
                              const IInterface* parent)
  : BaseTrSegMakerFromRecoTracks ( type, name, parent ),
    m_rayTracing   ( 0                          ),
    m_nomZstates   ( Rich::NRadiatorTypes, 0    ),
    m_zTolerance   ( Rich::NRadiatorTypes, 0    ),
    m_minStateDiff ( Rich::NRadiatorTypes, 0    ),
    m_entryPlanes  ( Rich::NRadiatorTypes       ),
    m_exitPlanes   ( Rich::NRadiatorTypes       ),
    m_entryZ       ( Rich::NRadiatorTypes       ),
    m_exitZ        ( Rich::NRadiatorTypes       ),
    m_maxX         ( Rich::NRadiatorTypes, 0    ),
    m_maxY         ( Rich::NRadiatorTypes, 0    ),
    m_minR2        ( Rich::NRadiatorTypes, 0    ),
    m_deRads       ( Rich::NRadiatorTypes       )
{
  // job options

  // Should get from XML instead of hardcode ?

  // Nominal z positions of states at RICHes (only needs to be rough)
  m_nomZstates[Rich::Aerogel]  =     0*Gaudi::Units::cm;
  m_nomZstates[Rich::Rich1Gas] = 249.0*Gaudi::Units::cm;
  m_nomZstates[Rich::Rich2Gas] = 951.0*Gaudi::Units::cm;
  declareProperty( "NominalZPositions", m_nomZstates );

  // tolerances on z positions
  m_zTolerance[Rich::Aerogel]  =  800*Gaudi::Units::mm;
  m_zTolerance[Rich::Rich1Gas] =  800*Gaudi::Units::mm;
  m_zTolerance[Rich::Rich2Gas] = 2000*Gaudi::Units::mm;
  declareProperty( "ZTolerances", m_zTolerance );

  // sanity checks on state information
  m_minStateDiff[Rich::Aerogel]  =   5*Gaudi::Units::mm;
  m_minStateDiff[Rich::Rich1Gas] =  50*Gaudi::Units::mm;
  m_minStateDiff[Rich::Rich2Gas] = 100*Gaudi::Units::mm;
  declareProperty( "MinStateDiff", m_minStateDiff );

  m_maxX[Rich::Aerogel]        = 375;
  m_maxX[Rich::Rich1Gas]       = 500;
  m_maxX[Rich::Rich2Gas]       = 3000;
  declareProperty( "MaxX", m_maxX );

  m_maxY[Rich::Aerogel]        = 375;
  m_maxY[Rich::Rich1Gas]       = 500;
  m_maxY[Rich::Rich2Gas]       = 2500;
  declareProperty( "MaxY", m_maxY );

  m_minR2[Rich::Aerogel]       = 50*50;
  m_minR2[Rich::Rich1Gas]      = 50*50;
  m_minR2[Rich::Rich2Gas]      = 100*100;
  declareProperty( "MinRSq", m_minR2 );

  m_entryZ[Rich::Aerogel]      = 1110;
  m_entryZ[Rich::Rich1Gas]     = 1160;
  m_entryZ[Rich::Rich2Gas]     = 9500;
  declareProperty( "RadEntryZ", m_entryZ );

  m_exitZ[Rich::Aerogel]       = 1160;
  m_exitZ[Rich::Rich1Gas]      = 1930;
  m_exitZ[Rich::Rich2Gas]      = 11340;
  declareProperty( "RadExitZ", m_exitZ );

  // Type of track segments to create
  declareProperty( "SegmentType", m_trSegTypeJO = "AllStateVectors" );

}

//=============================================================================
// Destructor
//=============================================================================
FastTrSegMakerFromRecoTracks::~FastTrSegMakerFromRecoTracks() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode FastTrSegMakerFromRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = BaseTrSegMakerFromRecoTracks::initialize();
  if ( sc.isFailure() ) return sc;

  // Get the ray tracing tool
  acquireTool( "RichRayTracing", m_rayTracing, NULL, true );

  // The following forms a simple radiator description
  // The values do not need to be very precise, but should still be eventually
  // obtained from the Detector elements

  const Gaudi::XYZVector tmpNorm(0,0,1);

  // Make temporary Aerogel description
  if ( usedRads(Rich::Aerogel) )
  {
    const Rich::RadiatorType rad = Rich::Aerogel;
    info() << rad << " : Entry/Exit z position " << m_entryZ[rad] << " " << m_exitZ[rad] << endmsg;
    m_rads.push_back(rad);
    m_entryPlanes[rad] = Gaudi::Plane3D( tmpNorm, Gaudi::XYZPoint(0,0,m_entryZ[rad]) );
    m_exitPlanes[rad]  = Gaudi::Plane3D( tmpNorm, Gaudi::XYZPoint(0,0,m_exitZ[rad]) );
    m_deRads[rad]      = getDet<DeRichRadiator>( DeRichLocations::Aerogel  );
  }

  // Make temporary Rich1Gas description
  if ( usedRads(Rich::Rich1Gas) )
  {
    const Rich::RadiatorType rad = Rich::Rich1Gas;
    info() << rad << " : Entry/Exit z position " << m_entryZ[rad] << " " << m_exitZ[rad] << endmsg;
    m_rads.push_back(rad);
    m_entryPlanes[rad] = Gaudi::Plane3D( tmpNorm, Gaudi::XYZPoint(0,0,m_entryZ[rad]) );
    m_exitPlanes[rad]  = Gaudi::Plane3D( tmpNorm, Gaudi::XYZPoint(0,0,m_exitZ[rad]) );
    m_deRads[rad]      = getDet<DeRichRadiator>( DeRichLocations::Rich1Gas );
  }

  // Make temporary Rich2Gas description
  if ( usedRads(Rich::Rich2Gas) )
  {
    const Rich::RadiatorType rad = Rich::Rich2Gas;
    info() << rad << " : Entry/Exit z position " << m_entryZ[rad] << " " << m_exitZ[rad] << endmsg;
    m_rads.push_back(rad);
    m_entryPlanes[rad] = Gaudi::Plane3D( tmpNorm, Gaudi::XYZPoint(0,0,m_entryZ[rad]) );
    m_exitPlanes[rad]  = Gaudi::Plane3D( tmpNorm, Gaudi::XYZPoint(0,0,m_exitZ[rad]) );
    m_deRads[rad]      = getDet<DeRichRadiator>( DeRichLocations::Rich2Gas );
  }

  // Define the segment type
  if      ( "AllStateVectors" == m_trSegTypeJO )
  {
    info() << "Will create track segments using all State information" << endmsg;
    m_trSegType = LHCb::RichTrackSegment::UseAllStateVectors;
  }
  else if ( "Choord" == m_trSegTypeJO )
  {
    info() << "Will create track segments using the 'choord' direction definition" << endmsg;
    m_trSegType = LHCb::RichTrackSegment::UseChordBetweenStates;
  }
  else
  {
    return Error( "Unknown RichTrackSegment type " + m_trSegTypeJO );
  }

  return sc;
}

//=============================================================================
// Constructs the track segments for a given Track
//=============================================================================
int
FastTrSegMakerFromRecoTracks::constructSegments( const ContainedObject * obj,
                                                 std::vector<LHCb::RichTrackSegment*>& segments )
  const {

  // Try to cast input data to required type for this implementation
  const LHCb::Track * track = dynamic_cast<const LHCb::Track *>(obj);
  if ( !track )
  {
    Warning("Input data object is not of type Track").ignore();
    return 0;
  }
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Analysing Track key=" << track->key()
              << " history=" << track->history()
              << " : " << track->states().size() << " States at z =";
    for ( std::vector<LHCb::State*>::const_iterator iS = track->states().begin();
          iS != track->states().end(); ++iS )
    {
      if (*iS) verbose() << " " << (*iS)->z();
    }
    verbose() << endmsg;
  }

  // make sure vector is empty
  segments.clear();

  // Loop over all radiators
  for ( const auto& rad : m_rads )
  {
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " Considering radiator " << rad << endmsg;

    // Get the best state information for this track and radiator
    std::vector<const LHCb::State*> states(2);
    if ( !stateInfo( track, rad, states ) ) continue;

    // Create entry momentum vector
    Gaudi::XYZVector entryVect( states[0]->slopes() );
    entryVect *= fabs(states[0]->p()) / sqrt(entryVect.Mag2());

    // Find radiator entry point
    Gaudi::XYZPoint entryPoint;
    if ( !m_rayTracing->intersectPlane( states[0]->position(),
                                        entryVect,
                                        m_entryPlanes[rad],
                                        entryPoint ) )
    {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "   Failed to intersect entry plane" << endmsg;
      continue;
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "     Entry point intersection :- " << endmsg
                  << "      Pos " << entryPoint << endmsg
                  << "      Dir " << entryVect << endmsg;
      if ( !checkBoundaries(entryPoint,rad) )
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "     Entry point failed boundary checks for " << rad << endmsg;
        continue;
      }
    }

    // Create exit momentum vector
    Gaudi::XYZVector exitVect ( states[1]->slopes() );
    exitVect *= fabs(states[1]->p()) / sqrt(exitVect.Mag2());

    // Find radiator exit point
    Gaudi::XYZPoint exitPoint;
    if ( !m_rayTracing->intersectPlane( states[1]->position(),
                                        exitVect,
                                        m_exitPlanes[rad],
                                        exitPoint ) )
    {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "   Failed to intersect exit plane" << endmsg;
      continue;
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "     Exit point intersection :- " << endmsg
                  << "      Pos " << exitPoint << endmsg
                  << "      Dir " << exitVect << endmsg;
      if ( !checkBoundaries(exitPoint,rad) )
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "     Exit point failed boundary checks for " << rad << endmsg;
        continue;
      }
    }

    //---------------------------------------------------------------------------------------------
    // Correction for beam pipe intersections
    if ( checkBeamPipe(rad) )
    {
      // Get intersections with beam pipe using DeRich object
      const Gaudi::XYZVector vect = exitPoint - entryPoint;
      Gaudi::XYZPoint inter1, inter2;
      const DeRichBeamPipe::BeamPipeIntersectionType intType
        = deBeam(rad)->intersectionPoints( entryPoint, vect, inter1, inter2 );

      if (msgLevel(MSG::VERBOSE))
        verbose() << "  --> Beam Intersects : " << intType << " : " << inter1 << " " << inter2 << endmsg;

      if ( intType == DeRichBeamPipe::NoIntersection )
      {
        if (msgLevel(MSG::VERBOSE))
          verbose() << "   --> No beam intersections -> No corrections needed" << endmsg;
      }
      else if ( intType == DeRichBeamPipe::FrontAndBackFace )
      {
        if (msgLevel(MSG::VERBOSE))
          verbose() << "   --> Inside beam pipe -> Reject segment" << endmsg;
        continue;
      }
      else if ( intType == DeRichBeamPipe::FrontFaceAndCone )
      {
        // Update entry point to exit point on cone
        if (msgLevel(MSG::VERBOSE))
          verbose() << "   --> Correcting entry point to point on cone" << endmsg;
        entryPoint = inter2;
      }
      else if ( intType == DeRichBeamPipe::BackFaceAndCone )
      {
        // Update exit point to entry point on cone
        if (msgLevel(MSG::VERBOSE))
          verbose() << "   --> Correcting exit point to point on cone" << endmsg;
        exitPoint = inter1;
      }
    }
    //---------------------------------------------------------------------------------------------

    // Printout the found entry/exit points
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "  Final States for " << rad << " are :-" << endmsg
                << "   Entry Point    : " << entryPoint << endmsg
                << "   Exit Point     : " << exitPoint << endmsg
                << "   Entry Momentum : " << entryVect << endmsg
                << "   Exit Momentum  : " << exitVect << endmsg;
    }

    // Final sanity check that all is OK with the states
    if ( (entryPoint.z() > exitPoint.z()) )
    {
      Warning( "Entry state after exist state for " + Rich::text(rad) + "  -> rejecting segment",
               StatusCode::SUCCESS, 5 ).ignore();
      continue;
    }
    if ( (exitPoint.z()-entryPoint.z()) < m_minStateDiff[rad] )
    {
      Warning( "Track states for " + Rich::text(rad) + " too close in z -> rejecting segment",
               StatusCode::SUCCESS, 5 ).ignore();
      continue;
    }

    try
    {

      // Which RICH
      const Rich::DetectorType rich = ( Rich::Rich2Gas == rad ? Rich::Rich2 : Rich::Rich1 );

      // input state errors
      const LHCb::RichTrackSegment::StateErrors inErrs( states[0]->errX2(),
                                                        states[0]->errY2(),
                                                        states[0]->errTx2(),
                                                        states[0]->errTy2(),
                                                        states[0]->errP2() );
      // output state errors
      const LHCb::RichTrackSegment::StateErrors outErrs( states[1]->errX2(),
                                                         states[1]->errY2(),
                                                         states[1]->errTx2(),
                                                         states[1]->errTy2(),
                                                         states[1]->errP2() );

      // Create intersection info
      Rich::RadIntersection::Vector intersects;
      intersects.push_back( Rich::RadIntersection( entryPoint, entryVect,
                                                   exitPoint, entryVect, // seems best to always use entry momentum !!
                                                   m_deRads[rad] ) );

      // Using this information, make radiator segment
      // this version uses 2 states and thus forces a straight line approximation
      segments.push_back( new LHCb::RichTrackSegment( m_trSegType,
                                                      intersects,
                                                      rad, rich,
                                                      inErrs, outErrs ) );

    }
    catch ( const std::exception & excpt )
    {
      Warning( "Exception whilst creating RichTrackSegment '"+std::string(excpt.what())+"'" ).ignore();
    }

  } // loop over radiators

  // return value is number of segments formed
  return segments.size();
}

//=======================================================================================
// Get the state information for a given radiator
//=======================================================================================
bool
FastTrSegMakerFromRecoTracks::stateInfo( const LHCb::Track * track,
                                         const Rich::RadiatorType rad,
                                         std::vector<const LHCb::State*> & states ) const
{
  if ( Rich::Aerogel == rad )
  {

    // Best entry and exit state is the vertex state
    const LHCb::State * state = &track->firstState();
    if ( !state ) return false;

    // Set the best states
    states[0] = state; // entry
    states[1] = state; // exit

  }
  else if ( Rich::Rich1Gas == rad )
  {

    // Entry state is vertex
    const LHCb::State * entryState = &track->firstState();
    // Exit state is at TT
    const LHCb::State * exitState  = stateAt( track, rad );

    // Check states
    if ( !entryState || !exitState ) return false;

    // Set the best states
    states[0] = entryState; // entry
    states[1] = exitState ; // exit

  }
  else if ( Rich::Rich2Gas == rad )
  {

    // Both entry and exit states are at T stations
    const LHCb::State * state = stateAt( track, rad );

    // check state
    if ( !state ) return false;

    // Set the best states
    states[0] = state; // entry
    states[1] = state; // exit

  }
  else
  {

    Error("Unknown radiator type").ignore();
    return false;

  }

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "  For " << rad << " chose states at :-" << endmsg
              << "    Entry : Pos " << states[0]->position() << " : Dir "
              << states[0]->slopes() << endmsg
              << "    Exit  : Pos " << states[1]->position() << " : Dir "
              << states[1]->slopes() << endmsg;
  }

  return true;
}

const LHCb::State *
FastTrSegMakerFromRecoTracks::stateAt( const LHCb::Track * track,
                                       const Rich::RadiatorType rad ) const
{
  // First, set found state to NULL
  const LHCb::State * pS = NULL;
  // search for best state
  for ( const auto* S : track->states() )
  {
    if ( fabs( S->z() - m_nomZstates[rad]) < m_zTolerance[rad] ) { pS = S; break; }
  }
  return pS;
}
