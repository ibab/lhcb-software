
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

DECLARE_TOOL_FACTORY( MassHypothesisRingCreator )

// Standard constructor
MassHypothesisRingCreator::
MassHypothesisRingCreator( const std::string& type,
                           const std::string& name,
                           const IInterface* parent )
  : ToolBase        ( type, name, parent ),
    m_ckAngle       ( NULL ),
    m_rings         ( NULL ),
    m_coneTrace     ( NULL ),
    m_richPartProp  ( NULL ),
    m_traceModeRad  ( Rich::NRadiatorTypes ),
    m_nPointScale   ( Rich::NRadiatorTypes ),
    m_maxPoint      ( Rich::NRadiatorTypes, 100 ),
    m_minPoint      ( Rich::NRadiatorTypes, 10  )
{
  using namespace boost::assign;

  // tool interface
  declareInterface<IMassHypothesisRingCreator>(this);

  // Define job option parameters
  // context specific defaults
  if ( contextContains("HLT") )
  {
    //               Aero   R1Gas  R2Gas
    std::vector<unsigned int>
      tmp = list_of  (50)   (50)   (50);
    m_maxPoint = tmp;
    m_minPoint = tmp;
  }
  else // offline
  {
    //               Aero   R1Gas  R2Gas
    std::vector<unsigned int>
      tmp = list_of  (100)  (100)  (100);
    m_maxPoint = tmp;
    m_minPoint = tmp;
  }
  declareProperty( "RingsLocation",
                   m_ringLocation = contextSpecificTES(LHCb::RichRecRingLocation::SegmentHypoRings) );
  declareProperty( "MaxRingPoints", m_maxPoint  );
  declareProperty( "MinRingPoints", m_minPoint  );
  declareProperty( "CheckBeamPipe", m_checkBeamPipe = true );
  declareProperty( "UseDetailedHPDsInRayTracing", m_useDetailedHPDsForRayT = false );

}

StatusCode MassHypothesisRingCreator::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle   );
  acquireTool( "RichRayTraceCKCone", m_coneTrace );
  acquireTool( "RichParticleProperties",  m_richPartProp );

  m_pidTypes = m_richPartProp->particleTypes();
  info() << "Particle types considered = " << m_pidTypes << endmsg;

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
  info() << "Aerogel  Track " << m_traceModeRad[Rich::Aerogel]  << endmsg;
  info() << "Rich1Gas Track " << m_traceModeRad[Rich::Rich1Gas] << endmsg;
  info() << "Rich2Gas Track " << m_traceModeRad[Rich::Rich2Gas] << endmsg;

  // only need to be rough
  m_nPointScale[Rich::Aerogel]  = m_maxPoint[Rich::Aerogel]  / 0.240;
  m_nPointScale[Rich::Rich1Gas] = m_maxPoint[Rich::Rich1Gas] / 0.050;
  m_nPointScale[Rich::Rich2Gas] = m_maxPoint[Rich::Rich2Gas] / 0.028;

  // ring info
  info() << "Maximum # ray trace points = " << m_maxPoint << endmsg;
  info() << "Minimum # ray trace points = " << m_minPoint << endmsg;

  // Make sure we are ready for a new event
  InitNewEvent();

  return sc;
}

// Method that handles various Gaudi "software events"
void MassHypothesisRingCreator::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) InitNewEvent();
}

void MassHypothesisRingCreator::massHypoRings( LHCb::RichRecSegment * segment ) const
{
  for ( Rich::Particles::const_iterator hypo = m_pidTypes.begin();
        hypo != m_pidTypes.end(); ++hypo )
  {
    massHypoRing( segment, *hypo );
  }
}

// Forms a new RichRecRing object from a RichRecSegment
LHCb::RichRecRing *
MassHypothesisRingCreator::massHypoRing( LHCb::RichRecSegment * segment,
                                         const Rich::ParticleIDType id ) const
{
  // does the ring already exist ?
  return ( segment ? ( segment->hypothesisRings().dataIsValid(id) ?
                       segment->hypothesisRings()[id] : buildRing(segment, id) ) : NULL );
}

LHCb::RichRecRing *
MassHypothesisRingCreator::buildRing( LHCb::RichRecSegment * segment,
                                      const Rich::ParticleIDType id ) const
{

  LHCb::RichRecRing * newRing = NULL;

  // Cherenkov theta for this segment/hypothesis combination
  const double ckTheta = m_ckAngle->avgCherenkovTheta( segment, id );
  if ( ckTheta > 0 )
  {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Creating " << id
                << " hypothesis ring for RichRecSegment " << segment->key()
                << endmsg;
    }

    // Get a new ring and save it
    newRing = new LHCb::RichRecRing( segment, 
                                     (LHCb::RichRecRing::FloatType)(ckTheta), 
                                     id );

    // set ring type info
    newRing->setType ( LHCb::RichRecRing::RayTracedCK );

    // ray tracing
    const Rich::RadiatorType rad = segment->trackSegment().radiator();
    unsigned int nPoints = (unsigned int) ( m_nPointScale[rad] * ckTheta );
    if      ( nPoints < m_minPoint[rad] ) { nPoints = m_minPoint[rad]; }
    else if ( nPoints > m_maxPoint[rad] ) { nPoints = m_maxPoint[rad]; }
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
      newRing = NULL;
    }

  }

  // set data in segment
  segment->hypothesisRings().setData( id, newRing );

  // return final pointer
  return newRing;
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
