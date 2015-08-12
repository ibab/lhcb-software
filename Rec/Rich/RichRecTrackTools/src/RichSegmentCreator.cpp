//-----------------------------------------------------------------------------
/** @file RichSegmentCreator.cpp
 *
 *  Implementation file for tool : Rich::Rec::SegmentCreator
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichSegmentCreator.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( SegmentCreator )

// Standard constructor
SegmentCreator::SegmentCreator ( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  : ToolBase        ( type, name, parent      ),
    m_signal        ( NULL                    ),
    m_detParams     ( NULL                    ),
    m_segments      ( 0                       ),
    m_richRecSegmentLocation ( LHCb::RichRecSegmentLocation::Default ),
    m_binsEn        ( Rich::NRadiatorTypes, 5 ),
    m_segCount      ( Rich::NRadiatorTypes, 0 ),
    m_segCountLast  ( Rich::NRadiatorTypes, 0 ),
    m_Nevts         ( 0                       ),
    m_hasBeenCalled ( false                   )
{

  // tool interface
  declareInterface<ISegmentCreator>(this);

  // Define job option parameters
  declareProperty( "EnergyBins", m_binsEn );
  declareProperty( "RichRecSegmentLocation", 
                   m_richRecSegmentLocation = contextSpecificTES(LHCb::RichRecSegmentLocation::Default) );

}

StatusCode SegmentCreator::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "RichRecSegment location : " << m_richRecSegmentLocation << endmsg;
  }

  // tools
  acquireTool( "RichExpectedTrackSignal", m_signal );
  acquireTool( "RichDetParameters",    m_detParams, NULL, true );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  if (msgLevel(MSG::DEBUG)) incSvc()->addListener( this, IncidentType::EndEvent );

  return sc;
}

StatusCode SegmentCreator::finalize()
{

  if ( msgLevel(MSG::DEBUG) )
  {
    // statistical tool
    const StatDivFunctor occ("%7.2f +-%6.2f");

    // Print out final stats
    debug() << "-------------------------------------------------------------------------------" << endmsg
            << " Created on average " << occ(m_segCount[Rich::Aerogel],m_Nevts)  << "  Aerogel  segments/event" << endmsg
            << " Created on average " << occ(m_segCount[Rich::Rich1Gas],m_Nevts) << "  Rich1Gas segments/event" << endmsg
            << " Created on average " << occ(m_segCount[Rich::Rich2Gas],m_Nevts) << "  Rich2Gas segments/event" << endmsg
            << "-------------------------------------------------------------------------------" << endmsg;
  }

  // Execute base class method
  return ToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void SegmentCreator::handle ( const Incident& incident )
{
  // Update prior to start of event. Used to re-initialise data containers
  if ( IncidentType::BeginEvent == incident.type() )
  {
    InitEvent();
  }
  // end of event
  else if ( IncidentType::EndEvent == incident.type() )
  {
    FinishEvent();
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Saved " << richSegments()->size()
              << " RichRecSegments : Aerogel="
              << m_segCount[Rich::Aerogel]-m_segCountLast[Rich::Aerogel]
              << " Rich1Gas=" << m_segCount[Rich::Rich1Gas]-m_segCountLast[Rich::Rich1Gas]
              << " Rich2Gas=" << m_segCount[Rich::Rich2Gas]-m_segCountLast[Rich::Rich2Gas] << endmsg;
    }
  }
}

// Create a new RichRecSegment
LHCb::RichRecSegment *
SegmentCreator::newSegment( LHCb::RichTrackSegment* segment,
                            LHCb::RichRecTrack* pTrk ) const
{
  LHCb::RichRecSegment * seg(NULL);
  if ( segment )
  {
    seg = new LHCb::RichRecSegment 
      ( segment,
        pTrk,
        m_binsEn[segment->radiator()],
        (LHCb::RichRecSegment::FloatType)(m_detParams->minPhotonEnergy(segment->radiator())),
        (LHCb::RichRecSegment::FloatType)(m_detParams->maxPhotonEnergy(segment->radiator())) 
        );
    // Set the average photon energy
    segment->setAvPhotonEnergy( m_signal->avgSignalPhotEnergy(seg) );
  }
  return seg;
}

// Forms a new RichRecSegment object
void SegmentCreator::saveSegment ( LHCb::RichRecSegment * segment ) const
{

  // debug counting
  if ( msgLevel(MSG::DEBUG) )
  {
    // Count segments
    ++m_segCount[segment->trackSegment().radiator()];
    // flag the tool as having been used this event
    m_hasBeenCalled = true;
  }

  // save segment
  richSegments()->insert( segment );
}

LHCb::RichRecSegments * SegmentCreator::richSegments() const
{
  if ( !m_segments )
  {

    if ( !exist<LHCb::RichRecSegments>(m_richRecSegmentLocation) )
    {

      // Reinitialise the Photon Container
      m_segments = new LHCb::RichRecSegments();

      // Register new RichRecPhoton container to Gaudi data store
      put( m_segments, m_richRecSegmentLocation );

    }
    else
    {

      // get segments from TES
      m_segments = get<LHCb::RichRecSegments>(m_richRecSegmentLocation);
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Found " << m_segments->size() << " pre-existing RichRecSegments in TES at "
                << m_richRecSegmentLocation << endmsg;
      }

    }
  }
  return m_segments;
}
