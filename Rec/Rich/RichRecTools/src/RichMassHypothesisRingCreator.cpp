
//-----------------------------------------------------------------------------
/** @file RichMassHypothesisRingCreator.cpp
 *
 *  Implementation file for tool : Rich::Rec::MassHypothesisRingCreator
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

// Standard constructor
MassHypothesisRingCreator::
MassHypothesisRingCreator( const std::string& type,
                           const std::string& name,
                           const IInterface* parent )
  : ToolBase        ( type, name, parent ),
    m_traceModeRad  ( Rich::NRadiatorTypes ),
    m_nPointScale   ( Rich::NRadiatorTypes, -1 )
{
  // tool interface
  declareInterface<IMassHypothesisRingCreator>(this);

  // Job Options                                   Aero R1Gas R2Gas
  declareProperty( "MaxRingPoints", m_maxPoint = { 100,  100,  100 } );
  declareProperty( "MinRingPoints", m_minPoint = { 100,  100,  100 } );
  declareProperty( "CheckBeamPipe", m_checkBeamPipe = true );
  declareProperty( "UseDetailedHPDsInRayTracing", m_useDetailedHPDsForRayT = false );
  declareProperty( "RingsLocation", m_ringLocation =
                   contextSpecificTES(LHCb::RichRecRingLocation::SegmentHypoRings) );

  //setProperty( "OutputLevel", 1 );

}

StatusCode MassHypothesisRingCreator::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle",     m_ckAngle      );
  acquireTool( "RichRayTraceCKCone",     m_coneTrace    );
  acquireTool( "RichParticleProperties", m_richPartProp );

  m_pidTypes = m_richPartProp->particleTypes();
  _ri_debug << "Particle types considered = " << m_pidTypes << endmsg;

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // the ray-tracing mode
  LHCb::RichTraceMode tmpMode ( LHCb::RichTraceMode::RespectHPDTubes,
                                ( m_useDetailedHPDsForRayT ?
                                  LHCb::RichTraceMode::FullHPDs :
                                  LHCb::RichTraceMode::SimpleHPDs ) );
  if ( m_checkBeamPipe ) { tmpMode.setBeamPipeIntersects(true); }
  m_traceModeRad[Rich::Aerogel]  = tmpMode;
  m_traceModeRad[Rich::Aerogel].setAeroRefraction(true);
  m_traceModeRad[Rich::Rich1Gas] = tmpMode;
  m_traceModeRad[Rich::Rich2Gas] = tmpMode;
  _ri_debug << "Aerogel  Track " << m_traceModeRad[Rich::Aerogel]  << endmsg;
  _ri_debug << "Rich1Gas Track " << m_traceModeRad[Rich::Rich1Gas] << endmsg;
  _ri_debug << "Rich2Gas Track " << m_traceModeRad[Rich::Rich2Gas] << endmsg;

  // ring info
  _ri_debug << "Maximum # points [Aero,R1Gas,R2Gas] = " << m_maxPoint << endmsg;
  _ri_debug << "Minimum # points [Aero,R1Gas,R2Gas] = " << m_minPoint << endmsg;

  // Make sure we are ready for a new event
  InitNewEvent();

  return sc;
}

// Method that handles various Gaudi "software events"
void MassHypothesisRingCreator::handle ( const Incident& /* incident */ )
{
  // We only subscribe to one sort of incident, so no need to check type
  //if ( IncidentType::BeginEvent == incident.type() )
  InitNewEvent();
}

void MassHypothesisRingCreator::massHypoRings( LHCb::RichRecSegment * segment ) const
{
  for ( const auto& hypo : m_pidTypes ) { massHypoRing( segment, hypo ); }
}

// Forms a new RichRecRing object from a RichRecSegment
LHCb::RichRecRing *
MassHypothesisRingCreator::massHypoRing( LHCb::RichRecSegment * segment,
                                         const Rich::ParticleIDType id ) const
{
  // does the ring already exist ?
  return ( segment ? ( segment->hypothesisRings().dataIsValid(id) ?
                       segment->hypothesisRings()[id] : buildRing(segment,id) ) : nullptr );
}

LHCb::RichRecRing *
MassHypothesisRingCreator::buildRing( LHCb::RichRecSegment * segment,
                                      const Rich::ParticleIDType id ) const
{
  LHCb::RichRecRing * newRing = nullptr;

  // Cherenkov theta for this segment/hypothesis combination
  const double ckTheta = m_ckAngle->avgCherenkovTheta( segment, id );
  if ( ckTheta > 0 )
  {
    _ri_verbo << "Creating " << id
              << " hypothesis ring for RichRecSegment " << segment->key()
              << endmsg;

    // Get a new ring and save it
    newRing = new LHCb::RichRecRing( segment,
                                     (LHCb::RichRecRing::FloatType)(ckTheta),
                                     id );

    // set ring type info
    newRing->setType ( LHCb::RichRecRing::RayTracedCK );

    // ray tracing
    const auto rad = segment->trackSegment().radiator();
    const auto nPoints = m_minPoint[rad] + (unsigned int)( nPointScale(rad) * ckTheta );
    //if      ( nPoints < m_minPoint[rad] ) { nPoints = m_minPoint[rad]; }
    //else if ( nPoints > m_maxPoint[rad] ) { nPoints = m_maxPoint[rad]; }
    const StatusCode sc = m_coneTrace->rayTrace( newRing, nPoints, m_traceModeRad[rad] );
    if ( sc.isSuccess() )
    {
      // save to container
      massHypoRings()->insert( newRing );
    }
    else
    {
      Warning( "Some problem occured during CK cone ray-tracing" ).ignore();
      delete newRing;
      newRing = nullptr;
    }

  }

  // set data in segment ( even if null )
  segment->hypothesisRings().setData( id, newRing );

  // return final pointer
  return newRing;
}

LHCb::RichRecRings * MassHypothesisRingCreator::massHypoRings() const
{
  if ( !m_rings )
  {
    // Try and load the rings
    m_rings = getIfExists<LHCb::RichRecRings>(m_ringLocation);
    if ( !m_rings )
    {
      // Reinitialise the Ring Container
      m_rings = new LHCb::RichRecRings();
      // Register new RichRecRing container to Gaudi data store
      put( m_rings, m_ringLocation );
    }
    else
    {
      _ri_debug << "Found " << m_rings->size() << " pre-existing RichRecRings in TES at "
                << m_ringLocation << endmsg;
    }
  }

  return m_rings;
}

// CRJ : should look to getting rid of the need for these

LHCb::RichRecRing * MassHypothesisRingCreator::newMassHypoRing() const
{
  return new LHCb::RichRecRing();
}

void MassHypothesisRingCreator::saveMassHypoRing( LHCb::RichRecRing * ring ) const
{
  massHypoRings()->insert( ring );
}

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( MassHypothesisRingCreator )

//-----------------------------------------------------------------------------
