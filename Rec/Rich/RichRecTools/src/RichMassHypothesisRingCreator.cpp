
//-----------------------------------------------------------------------------
/** @file RichMassHypothesisRingCreator.cpp
 *
 *  Implementation file for tool : Rich::Rec::MassHypothesisRingCreator
 *
 *  CVS Log :-
 *  $Id: RichMassHypothesisRingCreator.cpp,v 1.17 2007-05-08 12:17:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichMassHypothesisRingCreator.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( MassHypothesisRingCreator );

// Standard constructor
MassHypothesisRingCreator::
MassHypothesisRingCreator( const std::string& type,
                           const std::string& name,
                           const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_ckAngle       ( NULL ),
    m_rings         ( NULL ),
    m_coneTrace     ( NULL ),
    m_traceMode     ( LHCb::RichTraceMode::RespectHPDTubes,
                      LHCb::RichTraceMode::SimpleHPDs )
{

  // tool interface
  declareInterface<IMassHypothesisRingCreator>(this);

  // Define job option parameters
  declareProperty( "RingsLocation",
                   m_ringLocation = LHCb::RichRecRingLocation::SegmentHypoRings );

}

StatusCode MassHypothesisRingCreator::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle   );
  acquireTool( "RichRayTraceCKCone", m_coneTrace );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // the ray-tracing mode
  info() << "Track " << m_traceMode  << endreq;

  // Make sure we are ready for a new event
  InitNewEvent();

  return sc;
}

StatusCode MassHypothesisRingCreator::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void MassHypothesisRingCreator::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) InitNewEvent();
}

void MassHypothesisRingCreator::newMassHypoRings( LHCb::RichRecSegment * segment ) const
{
  for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
    newMassHypoRing( segment, static_cast<const Rich::ParticleIDType>(iHypo) );
  }
}

// Forms a new RichRecRing object from a RichRecSegment
LHCb::RichRecRing *
MassHypothesisRingCreator::newMassHypoRing( LHCb::RichRecSegment * segment,
                                            const Rich::ParticleIDType id ) const
{
  if ( !segment ) return NULL;

  // does the ring already exist ?
  return ( segment->hypothesisRings().dataIsValid(id) ?
           segment->hypothesisRings()[id] : buildRing(segment, id) );
}

LHCb::RichRecRing *
MassHypothesisRingCreator::buildRing( LHCb::RichRecSegment * segment,
                                      const Rich::ParticleIDType id ) const
{

  LHCb::RichRecRing * newRing = 0;

  // Cherenkov theta for this segment/hypothesis combination
  const double ckTheta = m_ckAngle->avgCherenkovTheta( segment, id );
  if ( ckTheta > 0 )
  {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Creating " << id
                << " hypothesis ring for RichRecSegment " << segment->key()
                << endreq;
    }

    // Get a new ring and save it
    newRing = newMassHypoRing();

    // set ring type info
    newRing->setType ( LHCb::RichRecRing::RayTracedCK );

    // set the segment information
    newRing->setRichRecSegment( segment );

    // set centre point information
    newRing->setCentrePointGlobal( segment->pdPanelHitPoint()      );
    newRing->setCentrePointLocal ( segment->pdPanelHitPointLocal() );

    // Set cherenkov information
    newRing->setRadius( ckTheta );

    // set the PID for this segment
    newRing->setMassHypo( id );

    // detector information
    newRing->setRich( segment->trackSegment().rich() );

    // ray tracing
    int nPoints = static_cast<int>( 1500 * ckTheta );
    if ( nPoints<30 ) nPoints = 30;
    m_coneTrace->rayTrace( newRing, nPoints, m_traceMode );

    // save to container
    saveMassHypoRing(newRing);

  }

  // set data in segment
  segment->hypothesisRings().setData( id, newRing );

  // return final pointer
  return newRing;
}

LHCb::RichRecRing * MassHypothesisRingCreator::newMassHypoRing() const
{
  // Make a new ring
  LHCb::RichRecRing * newRing = new LHCb::RichRecRing();
  return newRing;
}

void MassHypothesisRingCreator::saveMassHypoRing( LHCb::RichRecRing * ring ) const
{
  massHypoRings()->insert( ring );
}

LHCb::RichRecRings * MassHypothesisRingCreator::massHypoRings() const
{
  if ( !m_rings )
  {

    if ( !exist<LHCb::RichRecRings>(m_ringLocation) )
    {

      // Reinitialise the Ring Container
      m_rings = new LHCb::RichRecRings();

      // Register new RichRecRing container to Gaudi data store
      put( m_rings, m_ringLocation );

    }
    else
    {

      // Set smartref to TES Ring container
      m_rings = get<LHCb::RichRecRings>(m_ringLocation);

      debug() << "Found " << m_rings->size() << " pre-existing RichRecRings in TES at "
              << m_ringLocation << endreq;

    }
  }

  return m_rings;
}
