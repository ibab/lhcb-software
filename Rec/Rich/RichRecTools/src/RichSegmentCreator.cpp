
//-----------------------------------------------------------------------------
/** @file RichSegmentCreator.cpp
 *
 *  Implementation file for tool : RichSegmentCreator
 *
 *  CVS Log :-
 *  $Id: RichSegmentCreator.cpp,v 1.23 2006-05-05 11:01:40 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichSegmentCreator.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichSegmentCreator>          s_factory ;
const        IToolFactory& RichSegmentCreatorFactory = s_factory ;

// Standard constructor
RichSegmentCreator::RichSegmentCreator ( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
  : RichRecToolBase ( type, name, parent      ),
    m_segments      ( 0                       ),
    m_richRecSegmentLocation ( RichRecSegmentLocation::Default ),
    m_binsEn        ( Rich::NRadiatorTypes, 5 ),
    m_segCount      ( Rich::NRadiatorTypes, 0 ),
    m_segCountLast  ( Rich::NRadiatorTypes, 0 ),
    m_Nevts         ( 0                       ),
    m_hasBeenCalled ( false                   )
{

  // tool interface
  declareInterface<IRichSegmentCreator>(this);

  // Define job option parameters

  declareProperty( "EnergyBins", m_binsEn );

  if      ( context() == "Offline" )
  {
    m_richRecSegmentLocation = RichRecSegmentLocation::Offline;
  }
  else if ( context() == "HLT" )
  {
    m_richRecSegmentLocation = RichRecSegmentLocation::HLT;
  }
  declareProperty( "RichRecSegmentLocation", m_richRecSegmentLocation );

}

StatusCode RichSegmentCreator::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "RichRecSegment location : " << m_richRecSegmentLocation << endreq;
  }

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  if (msgLevel(MSG::DEBUG)) incSvc()->addListener( this, IncidentType::EndEvent );

  // Get the max/min photon energies
  const IRichDetParameters * detParams;
  acquireTool( "RichDetParameters", detParams );
  m_maxPhotEn[Rich::Aerogel] = detParams->maxPhotonEnergy( Rich::Aerogel );
  m_maxPhotEn[Rich::Rich1Gas]   = detParams->maxPhotonEnergy( Rich::Rich1Gas   );
  m_maxPhotEn[Rich::Rich2Gas]     = detParams->maxPhotonEnergy( Rich::Rich2Gas     );
  m_minPhotEn[Rich::Aerogel] = detParams->minPhotonEnergy( Rich::Aerogel );
  m_minPhotEn[Rich::Rich1Gas]   = detParams->minPhotonEnergy( Rich::Rich1Gas   );
  m_minPhotEn[Rich::Rich2Gas]     = detParams->minPhotonEnergy( Rich::Rich2Gas     );
  releaseTool(detParams);

  return sc;
}

StatusCode RichSegmentCreator::finalize()
{

  if ( msgLevel(MSG::DEBUG) )
  {
    // statistical tool
    RichStatDivFunctor occ("%7.2f +-%6.2f");

    // Print out final stats
    debug() << "-------------------------------------------------------------------------------" << endreq
            << " Created on average " << occ(m_segCount[Rich::Aerogel],m_Nevts) << "  Aerogel segments/event" << endreq
            << " Created on average " << occ(m_segCount[Rich::Rich1Gas],m_Nevts)   << "  Rich1Gas   segments/event" << endreq
            << " Created on average " << occ(m_segCount[Rich::Rich2Gas],m_Nevts)     << "  Rich2Gas     segments/event" << endreq
            << "-------------------------------------------------------------------------------" << endreq;
  }

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichSegmentCreator::handle ( const Incident& incident )
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
              << " Rich2Gas=" << m_segCount[Rich::Rich2Gas]-m_segCountLast[Rich::Rich2Gas] << endreq;
    }
  }
}

// Create a new RichRecSegment
RichRecSegment * RichSegmentCreator::newSegment( RichTrackSegment* segment,
                                                 RichRecTrack* pTrk ) const
{
  return ( !segment ? 0 :
           new RichRecSegment ( segment,
                                pTrk,
                                m_binsEn[segment->radiator()],
                                m_minPhotEn[segment->radiator()],
                                m_maxPhotEn[segment->radiator()] ) );
}

// Forms a new RichRecSegment object
void RichSegmentCreator::saveSegment ( RichRecSegment * segment ) const
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

RichRecSegments * RichSegmentCreator::richSegments() const
{
  if ( !m_segments )
  {

    if ( !exist<RichRecSegments>(m_richRecSegmentLocation) )
    {

      // Reinitialise the Photon Container
      m_segments = new RichRecSegments();

      // Register new RichRecPhoton container to Gaudi data store
      put( m_segments, m_richRecSegmentLocation );

    }
    else
    {

      // get segments from TES
      m_segments = get<RichRecSegments>(m_richRecSegmentLocation);
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Found " << m_segments->size() << " pre-existing RichRecSegments in TES at "
                << m_richRecSegmentLocation << endreq;
      }

    }
  }
  return m_segments;
}
