// $Id: RichSegmentCreator.cpp,v 1.9 2004-06-29 19:53:39 jonesc Exp $

// local
#include "RichSegmentCreator.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichSegmentCreator
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichSegmentCreator>          s_factory ;
const        IToolFactory& RichSegmentCreatorFactory = s_factory ;

// Standard constructor
RichSegmentCreator::RichSegmentCreator ( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
  : RichRecToolBase ( type, name, parent   ),
    m_segments      ( 0 ),
    m_binsEn        ( Rich::NRadiatorTypes )
{

  declareInterface<IRichSegmentCreator>(this);

  // Define job option parameters

  declareProperty( "RichRecSegmentLocation",
                   m_richRecSegmentLocation = RichRecSegmentLocation::Default );

  m_binsEn[Rich::Aerogel] = 5;
  m_binsEn[Rich::C4F10]   = 5;
  m_binsEn[Rich::CF4]     = 5;
  declareProperty( "EnergyBins", m_binsEn );

}

StatusCode RichSegmentCreator::initialize() 
{

  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Setup incident services
  IIncidentSvc * incSvc = svc<IIncidentSvc>( "IncidentSvc", true );
  incSvc->addListener( this, IncidentType::BeginEvent );
  if (msgLevel(MSG::DEBUG)) incSvc->addListener( this, IncidentType::EndEvent );

  // Get the max/min photon energies
  IRichDetParameters * detParams;
  acquireTool( "RichDetParameters", detParams );
  m_maxPhotEn[Rich::Aerogel] = detParams->maxPhotonEnergy( Rich::Aerogel );
  m_maxPhotEn[Rich::C4F10]   = detParams->maxPhotonEnergy( Rich::C4F10   );
  m_maxPhotEn[Rich::CF4]     = detParams->maxPhotonEnergy( Rich::CF4     );
  m_minPhotEn[Rich::Aerogel] = detParams->minPhotonEnergy( Rich::Aerogel );
  m_minPhotEn[Rich::C4F10]   = detParams->minPhotonEnergy( Rich::C4F10   );
  m_minPhotEn[Rich::CF4]     = detParams->minPhotonEnergy( Rich::CF4     );
  releaseTool(detParams);

  // Make sure we are ready for a new event
  InitNewEvent();

  return StatusCode::SUCCESS;
}

StatusCode RichSegmentCreator::finalize() 
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichSegmentCreator::handle ( const Incident& incident )
{
  // Update prior to start of event. Used to re-initialise data containers
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
  // Debug printout at the end of each event
  else if ( msgLevel(MSG::DEBUG) && IncidentType::EndEvent == incident.type() ) 
  {
    debug() << "Saved " << richSegments()->size() 
            << " RichRecSegments : Aerogel=" << m_segCount[Rich::Aerogel] 
            << " C4F10=" << m_segCount[Rich::C4F10] 
            << " CF4=" << m_segCount[Rich::CF4] << endreq;
  }
}

// Create a new RichRecSegment
RichRecSegment * RichSegmentCreator::newSegment( const RichTrackSegment& segment,
                                                 RichRecTrack* pTrk ) const
{
  return new RichRecSegment ( segment,
                              pTrk,
                              m_binsEn[segment.radiator()],
                              m_minPhotEn[segment.radiator()],
                              m_maxPhotEn[segment.radiator()] );
}

// Forms a new RichRecSegment object
void RichSegmentCreator::saveSegment ( RichRecSegment * segment ) const
{
  // count
  if ( msgLevel(MSG::DEBUG) ) ++m_segCount[segment->trackSegment().radiator()];
  // save segment
  richSegments()->insert( segment );
}

RichRecSegments * RichSegmentCreator::richSegments() const
{
  if ( !m_segments ) {
    SmartDataPtr<RichRecSegments> tdsSegments( evtSvc(),
                                               m_richRecSegmentLocation );
    if ( !tdsSegments ) {

      // Reinitialise the Photon Container
      m_segments = new RichRecSegments();

      // Register new RichRecPhoton container to Gaudi data store
      put( m_segments, m_richRecSegmentLocation );

    } else {

      debug() << "Found " << tdsSegments->size() << " pre-existing RichRecSegments in TES at "
              << m_richRecSegmentLocation << endreq;

      // Set smartref to TES segment container
      m_segments = tdsSegments;

    }
  }
  return m_segments;
}
