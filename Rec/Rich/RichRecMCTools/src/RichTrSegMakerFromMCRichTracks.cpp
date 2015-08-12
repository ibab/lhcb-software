
//-----------------------------------------------------------------------------
/** @file RichTrSegMakerFromMCRichTracks.cpp
 *
 * Implementation file for class : RichTrSegMakerFromMCRichTracks
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichTrSegMakerFromMCRichTracks.h"

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

DECLARE_TOOL_FACTORY( TrSegMakerFromMCRichTracks )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrSegMakerFromMCRichTracks::
TrSegMakerFromMCRichTracks( const std::string& type,
                            const std::string& name,
                            const IInterface* parent)
  : Rich::Rec::ToolBase ( type, name, parent ),
    m_truth      ( NULL ),
    m_rectruth   ( NULL ),
    m_usedRads   ( Rich::NRadiatorTypes, true ),
    m_minPathL   ( Rich::NRadiatorTypes, 0 ),
    m_minPhots   ( Rich::NRadiatorTypes, 1 )
{
  // interface
  declareInterface<ITrSegMaker>(this);

  // job options

  declareProperty( "UseRadiators", m_usedRads );

  m_minPathL[Rich::Aerogel]  = 10 *Gaudi::Units::mm;
  m_minPathL[Rich::Rich1Gas] = 500*Gaudi::Units::mm;
  m_minPathL[Rich::Rich2Gas] = 1500*Gaudi::Units::mm;
  declareProperty( "MinPathLengths", m_minPathL );

  m_minPhots[Rich::Aerogel]   = 3;
  m_minPhots[Rich::Rich1Gas]  = 5;
  m_minPhots[Rich::Rich2Gas]  = 5;
  declareProperty( "MinNumPhotons", m_minPhots );

}

//=============================================================================
// Destructor
//=============================================================================
TrSegMakerFromMCRichTracks::~TrSegMakerFromMCRichTracks() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode TrSegMakerFromMCRichTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // acquire tools
  acquireTool( "RichMCTruthTool",    m_truth, NULL, true );
  acquireTool( "RichRecMCTruthTool", m_rectruth );

  // get the radiators
  m_radiators[Rich::Aerogel]  = getDet<DeRichRadiator>( DeRichLocations::Aerogel  );
  m_radiators[Rich::Rich1Gas] = getDet<DeRichRadiator>( DeRichLocations::Rich1Gas );
  m_radiators[Rich::Rich2Gas] = getDet<DeRichRadiator>( DeRichLocations::Rich2Gas );

  info() << "Min path lengths for aero/Rich1Gas/Rich2Gas segments = " << m_minPathL << endmsg;
  info() << "Min # photons for aero/Rich1Gas/Rich2Gas segments    = " << m_minPhots << endmsg;

  return sc;
}

//=============================================================================
// Constructs the track segments for a given input object
//=============================================================================
int
TrSegMakerFromMCRichTracks::constructSegments( const ContainedObject * obj,
                                               std::vector<LHCb::RichTrackSegment*>& segments )
  const {

  // make sure vector is empty
  segments.clear();

  // get MCRichTrack
  const LHCb::MCRichTrack * track = mcRichTrack(obj);
  if ( !track )
  {
    verbose() << "MCRichTrack not available for input data object" << endmsg;
    return 0;
  }

  verbose() << "Trying MCRichTrack " << track->key() << " with "
            << track-> mcSegments().size() << " MCRichSegments" << endmsg;

  // loop over radiators
  for ( Radiators::const_iterator radiator = m_radiators.begin();
        radiator != m_radiators.end();
        ++radiator )
  {

    // which radiator
    const Rich::RadiatorType rad = (*radiator)->radiatorID();

    // is this radiator in use ?
    if ( !m_usedRads[rad] ) continue;

    // See if there is an MCRichSegment for this radiator
    const LHCb::MCRichSegment * segment = track->segmentInRad(rad);
    if ( !segment ) continue;

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " -> Found " << rad << " segment : pathL=" << segment->pathLength()
                << " nPhots=" << segment->mcRichOpticalPhotons().size() << endmsg;
    }

    // Apply selection cuts
    if ( segment->pathLength()                  < m_minPathL[rad] ) continue;
    if ( segment->mcRichOpticalPhotons().size() < m_minPhots[rad] ) continue;

    verbose() << "  -> Segment selected" << endmsg;

    // Get entry information
    const Gaudi::XYZPoint & entryPoint          = segment->entryPoint();
    const Gaudi::XYZVector & entryStateMomentum = segment->entryMomentum();

    // Get exit information
    const Gaudi::XYZPoint & exitPoint          = segment->exitPoint();
    const Gaudi::XYZVector & exitStateMomentum = segment->exitMomentum();

    // Create intersection info
    Rich::RadIntersection::Vector intersects;
    intersects.push_back( Rich::RadIntersection( entryPoint, entryStateMomentum,
                                                 exitPoint, exitStateMomentum,
                                                 *radiator ) );

    if ( Rich::Aerogel == rad )
    {

      // Using this information, make radiator segment and add to vector
      // assuming straight line between entry and exit
      segments.push_back( new LHCb::RichTrackSegment( LHCb::RichTrackSegment::UseAllStateVectors,
                                                      intersects,
                                                      rad, (*radiator)->rich() ) );

      // printout
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << " -> Found MCRichSegment for " << rad << endmsg
                  << "    Entry Point : " << entryPoint << endmsg
                  << "    Entry Mom.  : " << entryStateMomentum << endmsg
                  << "    Exit Point  : " << exitPoint << endmsg
                  << "    Exit Mom.   : " << exitStateMomentum << endmsg;
      }

    }
    else
    {

      // Get middle point information
      const Gaudi::XYZPoint midPoint          = segment->bestPoint(0.5);
      const Gaudi::XYZVector midStateMomentum = segment->bestMomentum(0.5);

      // Using this information, make radiator segment and add to vector
      // Use a middle state as well as entry and exit ones
      segments.push_back( new LHCb::RichTrackSegment( LHCb::RichTrackSegment::UseAllStateVectors,
                                                      intersects,
                                                      midPoint, midStateMomentum,
                                                      rad, (*radiator)->rich() ) );

      // printout
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << " -> Found MCRichSegment for " << rad << endmsg
                  << "    Entry Point : " << entryPoint << endmsg
                  << "    Entry Mom.  : " << entryStateMomentum << endmsg
                  << "    Mid Point   : " << midPoint << endmsg
                  << "    Mid Mom.    : " << midStateMomentum << endmsg
                  << "    Exit Point  : " << exitPoint << endmsg
                  << "    Exit Mom.   : " << exitStateMomentum << endmsg;
      }

    }

  }

  // return value is number of segments formed
  verbose() << "Created " << segments.size() << " RichTrackSegments" << endmsg;
  return segments.size();
}

//============================================================================
const LHCb::MCRichTrack *
TrSegMakerFromMCRichTracks::mcRichTrack( const ContainedObject * obj ) const
{
  // Work out what we have been given
  const LHCb::MCRichTrack * track = dynamic_cast<const LHCb::MCRichTrack *>(obj);
  if ( track )
  {
    verbose() << "Input data is of type MCRichTrack" << endmsg;
  }
  else
  {
    const LHCb::Track * trTrack = dynamic_cast<const LHCb::Track *>(obj);
    if ( trTrack )
    {
      verbose() << "Input data is of type Track" << endmsg;
      track = m_truth->mcRichTrack( m_rectruth->mcParticle(trTrack) );
    }
  }

  // return final object
  return track;
}
