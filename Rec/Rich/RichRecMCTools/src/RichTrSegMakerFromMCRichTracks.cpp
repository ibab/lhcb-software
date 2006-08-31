
//-----------------------------------------------------------------------------
/** @file RichTrSegMakerFromMCRichTracks.cpp
 *
 * Implementation file for class : RichTrSegMakerFromMCRichTracks
 *
 * CVS Log :-
 * $Id: RichTrSegMakerFromMCRichTracks.cpp,v 1.7 2006-08-31 12:36:10 cattanem Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichTrSegMakerFromMCRichTracks.h"

// From Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// namespaces
using namespace LHCb;

DECLARE_TOOL_FACTORY( RichTrSegMakerFromMCRichTracks );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichTrSegMakerFromMCRichTracks::
RichTrSegMakerFromMCRichTracks( const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : RichToolBase ( type, name, parent ),
    m_truth      ( 0 ),
    m_rectruth   ( 0 ),
    m_usedRads   ( Rich::NRadiatorTypes, true ),
    m_minPathL   ( Rich::NRadiatorTypes, 0 ),
    m_minPhots   ( Rich::NRadiatorTypes, 1 )
{
  // interface
  declareInterface<IRichTrSegMaker>(this);

  // job options

  declareProperty( "UseRadiators", m_usedRads );

  m_minPathL[Rich::Aerogel]  = 10 *Gaudi::Units::mm;
  m_minPathL[Rich::Rich1Gas] = 500*Gaudi::Units::mm;
  m_minPathL[Rich::Rich2Gas] = 500*Gaudi::Units::mm;
  declareProperty( "MinPathLengths", m_minPathL );

  m_minPhots[Rich::Aerogel] = 3;
  m_minPhots[Rich::Rich1Gas]   = 5;
  m_minPhots[Rich::Rich2Gas]     = 5;
  declareProperty( "MinNumPhotons", m_minPhots );

}

//=============================================================================
// Destructor
//=============================================================================
RichTrSegMakerFromMCRichTracks::~RichTrSegMakerFromMCRichTracks() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode RichTrSegMakerFromMCRichTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // acquire tools
  acquireTool( "RichMCTruthTool",    m_truth    );
  acquireTool( "RichRecMCTruthTool", m_rectruth );

  // get the radiators
  m_radiators[Rich::Aerogel] = getDet<DeRichRadiator>( DeRichRadiatorLocation::Aerogel );
  m_radiators[Rich::Rich1Gas]   = getDet<DeRichRadiator>( DeRichRadiatorLocation::Rich1Gas   );
  m_radiators[Rich::Rich2Gas]     = getDet<DeRichRadiator>( DeRichRadiatorLocation::Rich2Gas     );

  info() << "Min path lengths for aero/Rich1Gas/Rich2Gas segments = " << m_minPathL << endreq
         << "Min # photons for aero/Rich1Gas/Rich2Gas segments    = " << m_minPhots << endreq;

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichTrSegMakerFromMCRichTracks::finalize()
{
  // Execute base class method
  return RichToolBase::finalize();
}

//=============================================================================
// Constructs the track segments for a given input object
//=============================================================================
int
RichTrSegMakerFromMCRichTracks::constructSegments( const ContainedObject * obj,
                                                   std::vector<RichTrackSegment*>& segments )
  const {

  // make sure vector is empty
  segments.clear();

  // get MCRichTrack
  const MCRichTrack * track = mcRichTrack(obj);
  if ( !track )
  {
    verbose() << "MCRichTrack not available for input data object" << endreq;
    return 0;
  }

  verbose() << "Trying MCRichTrack " << track->key() << " with "
            << track-> mcSegments().size() << " MCRichSegments" << endreq;

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
    const MCRichSegment * segment = track->segmentInRad(rad);
    if ( !segment ) continue;

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " -> Found " << rad << " segment : pathL=" << segment->pathLength()
                << " nPhots=" << segment->mcRichOpticalPhotons().size() << endreq;
    }

    // Apply selection cuts
    if ( segment->pathLength() < m_minPathL[rad] ) continue;
    if ( segment->mcRichOpticalPhotons().size() < m_minPhots[rad] ) continue;

    verbose() << "  -> Segment selected" << endreq;

    // Get entry information
    const Gaudi::XYZPoint & entryPoint          = segment->entryPoint();
    const Gaudi::XYZVector & entryStateMomentum = segment->entryMomentum();

    // Get exit information
    const Gaudi::XYZPoint & exitPoint          = segment->exitPoint();
    const Gaudi::XYZVector & exitStateMomentum = segment->exitMomentum();

    if ( Rich::Aerogel == rad )
    {

      // Using this information, make radiator segment and add to vector
      // assuming straight line between entry and exit
      segments.push_back( new RichTrackSegment( RichTrackSegment::UseAllStateVectors(),
                                                entryPoint, entryStateMomentum,
                                                exitPoint, exitStateMomentum,
                                                rad, (*radiator)->rich() ) );

      // printout
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << " -> Found MCRichSegment for " << rad << endreq
                  << "    Entry Point : " << entryPoint << endreq
                  << "    Entry Mom.  : " << entryStateMomentum << endreq
                  << "    Exit Point  : " << exitPoint << endreq
                  << "    Exit Mom.   : " << exitStateMomentum << endreq;
      }

    }
    else
    {

      // Get middle point information
      const Gaudi::XYZPoint midPoint          = segment->bestPoint(0.5);
      const Gaudi::XYZVector midStateMomentum = segment->bestMomentum(0.5);

      // Using this information, make radiator segment and add to vector
      // Use a middle state as well as entry and exit ones
      segments.push_back( new RichTrackSegment( RichTrackSegment::UseAllStateVectors(),
                                                entryPoint, entryStateMomentum,
                                                midPoint, midStateMomentum,
                                                exitPoint, exitStateMomentum,
                                                rad, (*radiator)->rich() ) );

      // printout
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << " -> Found MCRichSegment for " << rad << endreq
                  << "    Entry Point : " << entryPoint << endreq
                  << "    Entry Mom.  : " << entryStateMomentum << endreq
                  << "    Mid Point   : " << midPoint << endreq
                  << "    Mid Mom.    : " << midStateMomentum << endreq
                  << "    Exit Point  : " << exitPoint << endreq
                  << "    Exit Mom.   : " << exitStateMomentum << endreq;
      }

    }

  }

  // return value is number of segments formed
  verbose() << "Created " << segments.size() << " RichTrackSegments" << endreq;
  return segments.size();
}

//============================================================================
const MCRichTrack *
RichTrSegMakerFromMCRichTracks::mcRichTrack( const ContainedObject * obj ) const
{
  // Work out what we have been given
  const MCRichTrack * track = dynamic_cast<const MCRichTrack *>(obj);
  if ( track )
  {
    verbose() << "Input data is of type MCRichTrack" << endreq;
  }
  else
  {
    const Track * trTrack = dynamic_cast<const Track *>(obj);
    if ( trTrack )
    {
      verbose() << "Input data is of type Track" << endreq;
      track = m_truth->mcRichTrack( m_rectruth->mcParticle(trTrack) );
    }
  }

  // return final object
  return track;
}
