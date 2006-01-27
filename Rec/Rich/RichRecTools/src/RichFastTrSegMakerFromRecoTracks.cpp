
//-----------------------------------------------------------------------------
/** @file RichFastTrSegMakerFromRecoTracks.cpp
 *
 * Implementation file for class : RichFastTrSegMakerFromRecoTracks
 *
 * CVS Log :-
 * $Id: RichFastTrSegMakerFromRecoTracks.cpp,v 1.1 2006-01-27 09:14:17 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 23/08/2004
 */
//-----------------------------------------------------------------------------

// local
#include "RichFastTrSegMakerFromRecoTracks.h"

// namespaces
using namespace LHCb;

// Declaration of the Algorithm Factory
static const  ToolFactory<RichFastTrSegMakerFromRecoTracks>          Factory ;
const        IToolFactory& RichFastTrSegMakerFromRecoTracksFactory = Factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichFastTrSegMakerFromRecoTracks::
RichFastTrSegMakerFromRecoTracks( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent)
  : RichToolBase ( type, name, parent         ),
    m_rayTracing ( 0                          ),
    m_usedRads   ( Rich::NRadiatorTypes, true )
{

  // Interface
  declareInterface<IRichTrSegMaker>(this);

  // job options
  declareProperty( "UseRadiators", m_usedRads );

}

//=============================================================================
// Destructor
//=============================================================================
RichFastTrSegMakerFromRecoTracks::~RichFastTrSegMakerFromRecoTracks() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode RichFastTrSegMakerFromRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Get the ray tracing tool
  acquireTool( "RichRayTracing", m_rayTracing );

  // Should get from XML instead of hardcode ?

  // Nominal z positions of states at RICHes (only needs to be rough)
  m_nomZstates[Rich::Aerogel] = 0*cm;
  m_nomZstates[Rich::C4F10]   = 249.0*cm;
  m_nomZstates[Rich::CF4]     = 951.0*cm;

  // tolerances on z positions
  m_zTolerance[Rich::Aerogel] = 800*mm;
  m_zTolerance[Rich::C4F10]   = 800*mm;
  m_zTolerance[Rich::CF4]     = 2000*mm;

  // sanity checks on state information
  m_minStateDiff[Rich::Aerogel] = 5*mm;
  m_minStateDiff[Rich::C4F10]   = 50*mm;
  m_minStateDiff[Rich::CF4]     = 100*mm;

  // The following forms a simple radiator description
  // The values do not need to be very precise, but should still be eventually
  // obtained from the Detector elements

  const Gaudi::XYZVector tmpNorm(0,0,1);

  // Make temporary Aerogel description
  if ( m_usedRads[Rich::Aerogel] )
  {
    m_rads.push_back(Rich::Aerogel);
    m_entryPlanes[Rich::Aerogel] = Gaudi::Plane3D( tmpNorm, Gaudi::XYZPoint(0,0,1110) );
    m_exitPlanes[Rich::Aerogel]  = Gaudi::Plane3D( tmpNorm, Gaudi::XYZPoint(0,0,1160) );
    m_maxX[Rich::Aerogel]        = 375;
    m_maxY[Rich::Aerogel]        = 375;
    m_minR2[Rich::Aerogel]       = 50*50;
  }
  else
  {
    Warning("Track segments for Aerogel are disabled",StatusCode::SUCCESS);
  }
  // Make temporary C4F10 description
  if ( m_usedRads[Rich::C4F10] )
  {
    m_rads.push_back(Rich::C4F10);
    m_entryPlanes[Rich::C4F10]   = Gaudi::Plane3D( tmpNorm, Gaudi::XYZPoint(0,0,1160) );
    m_exitPlanes[Rich::C4F10]    = Gaudi::Plane3D( tmpNorm, Gaudi::XYZPoint(0,0,1930) );
    m_maxX[Rich::C4F10]          = 500;
    m_maxY[Rich::C4F10]          = 500;
    m_minR2[Rich::C4F10]         = 50*50;
  }
  else
  {
    Warning("Track segments for C4F10 are disabled",StatusCode::SUCCESS);
  }
  // Make temporary CF4 description
  if ( m_usedRads[Rich::CF4] )
  {
    m_rads.push_back(Rich::CF4);
    m_entryPlanes[Rich::CF4]     = Gaudi::Plane3D( tmpNorm, Gaudi::XYZPoint(0,0,9500)  );
    m_exitPlanes[Rich::CF4]      = Gaudi::Plane3D( tmpNorm, Gaudi::XYZPoint(0,0,11340) );
    m_maxX[Rich::CF4]            = 3000;
    m_maxY[Rich::CF4]            = 2500;
    m_minR2[Rich::CF4]           = 100*100;
  }
  else
  {
    Warning("Track segments for CF4 are disabled",StatusCode::SUCCESS);
  }

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichFastTrSegMakerFromRecoTracks::finalize()
{
  // Execute base class method
  return RichToolBase::finalize();
}

//=============================================================================
// Constructs the track segments for a given TrStoredTrack
//=============================================================================
int
RichFastTrSegMakerFromRecoTracks::constructSegments( const ContainedObject * obj,
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
  for ( Radiators::const_iterator rad = m_rads.begin(); rad != m_rads.end(); ++rad )
  {
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " Considering radiator " << *rad << endreq;

    // Get the best state information for this track and radiator
    std::vector<const LHCb::State*> states(2);
    if ( !stateInfo( track, *rad, states ) ) continue;

    // Create entry momentum vector
    Gaudi::XYZVector entryVect( states[0]->slopes() );
    entryVect *= fabs(states[0]->p()) / sqrt(entryVect.Mag2());

    // Find radiator entry point
    Gaudi::XYZPoint entryPoint;
    if ( !m_rayTracing->intersectPlane( states[0]->position(),
                                        entryVect,
                                        m_entryPlanes[*rad],
                                        entryPoint ) )
    {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "   Failed to intersect entry plane" << endreq;
      continue;
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "     Entry point intersection :- " << endreq
                  << "      Pos " << entryPoint << endreq
                  << "      Dir " << entryVect << endreq;
      if ( !checkBoundaries(entryPoint,*rad) )
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "     Entry point failed boundary checks for " << *rad << endreq;
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
                                        m_exitPlanes[*rad],
                                        exitPoint ) )
    {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "   Failed to intersect exit plane" << endreq;
      continue;
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "     Exit point intersection :- " << endreq
                  << "      Pos " << exitPoint << endreq
                  << "      Dir " << exitVect << endreq;
      if ( !checkBoundaries(exitPoint,*rad) )
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "     Exit point failed boundary checks for " << *rad << endreq;
        continue;
      }
    }

    // Printout the found entry/exit points
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "  Final States for " << *rad << " are :-" << endreq
                << "   Entry Point    : " << entryPoint << endreq
                << "   Exit Point     : " << exitPoint << endreq
                << "   Entry Momentum : " << entryVect << endreq
                << "   Exit Momentum  : " << exitVect << endreq;
    }

    // Final sanity check that all is OK with the states
    if ( (entryPoint.z() > exitPoint.z()) )
    {
      Warning( "Entry state after exist state for " + Rich::text(*rad) + "  -> rejecting segment",
               StatusCode::SUCCESS, 5 );
      continue;
    }
    if ( (exitPoint.z()-entryPoint.z()) < m_minStateDiff[*rad] )
    {
      Warning( "Track states for " + Rich::text(*rad) + " too close in z -> rejecting segment",
               StatusCode::SUCCESS, 5 );
      continue;
    }

    try
    {

      // Which RICH
      const Rich::DetectorType rich = ( Rich::CF4 == *rad ? Rich::Rich2 : Rich::Rich1 );

      // Using this information, make radiator segment
      // this version uses 2 states and thus forces a straight line approximation
      //segments.push_back( new RichTrackSegment( RichTrackSegment::UseChordBetweenStates(),
      segments.push_back( new RichTrackSegment( RichTrackSegment::UseAllStateVectors(),
                                                entryPoint, entryVect,
                                                exitPoint,  entryVect, // seems best to always use entry momentum !!
                                                *rad, rich,
                                                RichTrackSegment::StateErrors( states[0]->errX2(),
                                                                               states[0]->errY2(),
                                                                               states[0]->errTx2(),
                                                                               states[0]->errTy2(),
                                                                               states[0]->errP2() ),
                                                RichTrackSegment::StateErrors( states[1]->errX2(),
                                                                               states[1]->errY2(),
                                                                               states[1]->errTx2(),
                                                                               states[1]->errTy2(),
                                                                               states[1]->errP2() ) ) );

    }
    catch ( const std::exception & excpt )
    {
      Warning( "Exception whilst creating RichTrackSegment '"+std::string(excpt.what())+"'" );
    }

  }

  // return value is number of segments formed
  return segments.size();
}

//=======================================================================================
// Get the state information for a given radiator
//=======================================================================================
bool
RichFastTrSegMakerFromRecoTracks::stateInfo( const Track * track,
                                             const Rich::RadiatorType rad,
                                             std::vector<const LHCb::State*> & states ) const
{
  if ( Rich::Aerogel == rad )
  {

    // Best entry and exit state is the vertex state
    const State * state = &track->firstState();
    if ( !state ) return false;

    // Set the best states
    states[0] = state; // entry
    states[1] = state; // exit

  }
  else if ( Rich::C4F10 == rad )
  {

    // Entry state is vertex
    const State * entryState = &track->firstState();
    // Exit state is at TT
    const State * exitState  = stateAt( track, rad );

    // Check states
    if ( !entryState || !exitState ) return false;

    // Set the best states
    states[0] = entryState; // entry
    states[1] = exitState ; // exit

  }
  else if ( Rich::CF4 == rad )
  {

    // Both entry and exit states are at T stations
    const State * state = stateAt( track, rad );

    // check state
    if ( !state ) return false;

    // Set the best states
    states[0] = state; // entry
    states[1] = state; // exit

  }
  else
  {

    Error("Unknown radiator type");
    return false;

  }

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "  For " << rad << " chose states at :-" << endreq
              << "    Entry : Pos " << states[0]->position() << " : Dir "
              << states[0]->slopes() << endreq
              << "    Exit  : Pos " << states[1]->position() << " : Dir "
              << states[1]->slopes() << endreq;
  }

  return true;
}
