// $Id: RichRecRingCreator.cpp,v 1.1 2004-05-30 16:48:13 jonrob Exp $

// local
#include "RichRecRingCreator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecRingCreator
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRecRingCreator>          s_factory ;
const        IToolFactory& RichRecRingCreatorFactory = s_factory ;

// Standard constructor
RichRecRingCreator::RichRecRingCreator( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_rayTrace     ( 0 ),
    m_ckAngle      ( 0 ),
    m_rings        ( 0 ),
    m_nRayTrace    ( 0 )
{

  declareInterface<IRichRecRingCreator>(this);

  // Define job option parameters
  declareProperty( "RichRecRingLocation",
                   m_richRecRingLocation = RichRecRingLocation::SegmentHypoRings );

  // Define job option parameters
  declareProperty( "NPhotonsRayTrace", m_nRayTrace = 100 );

}

StatusCode RichRecRingCreator::initialize() {

  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",     m_rayTrace );
  acquireTool( "RichCherenkovAngle", m_ckAngle  );

  // Setup incident services
  IIncidentSvc * incSvc = svc<IIncidentSvc>( "IncidentSvc", true );
  incSvc->addListener( this, IncidentType::BeginEvent );

  // Set up cached parameters for photon tracing
  m_incPhi = M_2PI/( static_cast<double>(m_nRayTrace) );
  double ckPhi = 0.0;
  m_sinCkPhi.clear();
  m_cosCkPhi.clear();
  for ( int iPhot = 0; iPhot < m_nRayTrace; ++iPhot, ckPhi += m_incPhi ) {
    m_sinCkPhi.push_back( sin(ckPhi) );
    m_cosCkPhi.push_back( cos(ckPhi) );
  }

  // Make sure we are ready for a new event
  InitNewEvent();

  return StatusCode::SUCCESS;
}

StatusCode RichRecRingCreator::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichRecRingCreator::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) InitNewEvent();
}

void RichRecRingCreator::newRings( RichRecSegment * segment,
                                   const RichRecRing::RingType type ) const
{
  for ( int iHypo = 0; iHypo < Rich::NParticleTypes ; ++iHypo ) {
    const Rich::ParticleIDType id = static_cast<const Rich::ParticleIDType>(iHypo);
    newRing( segment, id, type );
  }

}

// Forms a new RichRecRing object from a RichDigit
RichRecRing *
RichRecRingCreator::newRing( RichRecSegment * segment,
                             const Rich::ParticleIDType id,
                             const RichRecRing::RingType type ) const
{
  if ( !segment ) return 0;

  if ( RichRecRing::RayTracedPhotons == type ) {

    // does the ring already exist ?
    RichRecRing * ring = segment->hypothesisRings()[id];
    return ( ring ? ring : buildRayTracedRing( segment, id ) );

  } else { Exception("Unknown Ring Type !"); }

  return NULL;
}

RichRecRing * RichRecRingCreator::buildRayTracedRing( RichRecSegment * segment,
                                                      const Rich::ParticleIDType id ) const
{

  RichRecRing * newRing = 0;

  // Cherenkov theta for this segment/hypothesis combination
  const double ckTheta = m_ckAngle->avgCherenkovTheta( segment, id );
  if ( ckTheta > 0 ) {

    // Make a new ring and give to container
    newRing = new RichRecRing();
    richRings()->insert( newRing );

    const RichTrackSegment & trackSeg = segment->trackSegment();

    // set centre point
    newRing->setCentrePointGlobal( segment->pdPanelHitPoint()      );
    newRing->setCentrePointLocal ( segment->pdPanelHitPointLocal() );

    // Define rotation matrix
    const HepVector3D z = trackSeg.bestMomentum().unit();
    HepVector3D y = ( fabs( z * HepVector3D(1.,0.,0.) ) < 1. ?
                      z.cross( HepVector3D(0.,1.,0.) ) :
                      z.cross( HepVector3D(1.,0.,0.) ) );
    y.setMag(1);
    const HepVector3D x = y.cross(z);
    HepRotation rotation = HepRotation();
    rotation.rotateAxes(x,y,z);

    // loop around the ring
    const double sinCkTheta = sin(ckTheta);
    const double cosCkTheta = cos(ckTheta);
    AngleVector::const_iterator iCos = m_cosCkPhi.begin();
    AngleVector::const_iterator iSin = m_sinCkPhi.begin();
    for ( int iPhot = 0; iPhot < m_nRayTrace; ++iPhot, ++iCos, ++iSin ) {

      // Photon emission point is random between segment start and end points
      //const HepPoint3D emissionPt = trackSeg.bestPoint( m_uniDist() );
      // Photon emission point is half-way between segment start and end points
      const HepPoint3D & emissionPt = trackSeg.bestPoint();

      // Photon direction around loop
      const HepVector3D photDir = rotation * HepVector3D( sinCkTheta*(*iCos),
                                                          sinCkTheta*(*iSin),
                                                          cosCkTheta );

      // Ray trace through detector, using fast circle modelling of HPDs
      HepPoint3D hitPoint;
      if ( m_rayTrace->traceToDetectorWithoutEff( trackSeg.rich(),
                                                  emissionPt,
                                                  photDir,
                                                  hitPoint,
                                                  DeRichHPDPanel::loose ) ) {
        newRing->ringPoints().push_back( hitPoint );
      }

      // if no points found, delete ring, otherwise given to segment
      if ( newRing->ringPoints().empty() ) {
        delete newRing;
        newRing = 0;
      } else {
        segment->hypothesisRings().setData( id, newRing );
      }

    }

  }

  return newRing;
}

RichRecRings * RichRecRingCreator::richRings() const
{
  if ( !m_rings ) {

    SmartDataPtr<RichRecRings> tdsRings( evtSvc(), m_richRecRingLocation );
    if ( !tdsRings ) {

      // Reinitialise the Ring Container
      m_rings = new RichRecRings();

      // Register new RichRecRing container to Gaudi data store
      put( m_rings, m_richRecRingLocation );

    } else {

      // Set smartref to TES Ring container
      m_rings = tdsRings;

    }
  }

  return m_rings;
}
