// $Id: RichSegmentCreator.cpp,v 1.6 2004-03-16 13:45:05 jonesc Exp $

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
    m_binsEn        ( Rich::NRadiatorTypes ),
    m_maxPhotEn     ( Rich::NRadiatorTypes ),
    m_minPhotEn     ( Rich::NRadiatorTypes ) {

  declareInterface<IRichSegmentCreator>(this);

  // Define job option parameters

  declareProperty( "RichRecSegmentLocation",
                   m_richRecSegmentLocation = RichRecSegmentLocation::Default );

  m_binsEn[Rich::Aerogel] = 5;
  m_binsEn[Rich::C4F10]   = 5;
  m_binsEn[Rich::CF4]     = 5;
  declareProperty( "EnergyBins", m_binsEn );

  m_maxPhotEn[Rich::Aerogel] = 3.8;
  m_maxPhotEn[Rich::C4F10]   = 7.0;
  m_maxPhotEn[Rich::CF4]     = 7.0;
  declareProperty( "MaxPhotonEnergy", m_maxPhotEn );

  m_minPhotEn[Rich::Aerogel] = 1.75;
  m_minPhotEn[Rich::C4F10]   = 1.75;
  m_minPhotEn[Rich::CF4]     = 1.75;
  declareProperty( "MinPhotonEnergy", m_minPhotEn );

}

StatusCode RichSegmentCreator::initialize() {

  debug() << "Initialize" << endreq;

  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Setup incident services
  IIncidentSvc * incSvc = svc<IIncidentSvc>( "IncidentSvc", true );
  incSvc->addListener( this, "BeginEvent" ); // Informed of a new event

  // Make sure we are ready for a new event
  InitNewEvent();

  // Informational Printout
  debug() << " Number of Energy bins      = " << m_binsEn << endreq
          << " Max Photon Energy          = " << m_maxPhotEn << endreq
          << " Min Photon Energy          = " << m_minPhotEn << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichSegmentCreator::finalize() {

  debug() << "Finalize" << endreq;

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichSegmentCreator::handle ( const Incident& incident )
{
  if ( "BeginEvent" == incident.type() ) InitNewEvent();
}

// Create a new RichRecSegment
RichRecSegment * RichSegmentCreator::newSegment( RichTrackSegment& segment,
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

      // Set smartref to TES segment container
      m_segments = tdsSegments;

    }
  }
  return m_segments;
}
