// $Id: RichRayTraceCherenkovCone.cpp,v 1.2 2004-05-31 22:02:06 jonrob Exp $

// local
#include "RichRayTraceCherenkovCone.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRayTraceCherenkovCone
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRayTraceCherenkovCone>          s_factory ;
const        IToolFactory& RichRayTraceCherenkovConeFactory = s_factory ;

// Standard constructor
RichRayTraceCherenkovCone::RichRayTraceCherenkovCone( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_rayTrace     ( 0 ),
    m_ckAngle      ( 0 ),
    m_nRayTrace    ( 0 )
{

  declareInterface<IRichRayTraceCherenkovCone>(this);

  // Define job option parameters
  declareProperty( "NPhotonsRayTrace", m_nRayTrace = 100 );

}

StatusCode RichRayTraceCherenkovCone::initialize() {

  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",     m_rayTrace );
  acquireTool( "RichCherenkovAngle", m_ckAngle  );

  // Set up cached parameters for photon tracing
  m_incPhi = M_2PI/( static_cast<double>(m_nRayTrace) );
  double ckPhi = 0.0;
  m_sinCkPhi.clear();
  m_cosCkPhi.clear();
  for ( int iPhot = 0; iPhot < m_nRayTrace; ++iPhot, ckPhi += m_incPhi ) {
    m_sinCkPhi.push_back( sin(ckPhi) );
    m_cosCkPhi.push_back( cos(ckPhi) );
  }

  return StatusCode::SUCCESS;
}

StatusCode RichRayTraceCherenkovCone::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

std::vector<HepPoint3D> & 
RichRayTraceCherenkovCone::rayTrace ( RichRecRing * ring ) const
{
  if ( !ring ) Exception( "Null RichRecRing pointer!" );

  // cache points in each ring
  if ( ring->ringPoints().empty() ) {
    rayTrace( ring->richRecSegment(), ring->radius(), ring->ringPoints() );
  }

  return ring->ringPoints();
}

StatusCode
RichRayTraceCherenkovCone::rayTrace ( RichRecSegment * segment,
                                      const Rich::ParticleIDType id,
                                      std::vector<HepPoint3D> & points ) const
{
  return rayTrace( segment,
                   m_ckAngle->avgCherenkovTheta(segment, id),
                   points );
}

StatusCode
RichRayTraceCherenkovCone::rayTrace ( RichRecSegment * segment,
                                      const double ckTheta,
                                      std::vector<HepPoint3D> & points ) const
{

  // make sure segment is valid
  if ( !segment ) Exception( "Null RichRecSegment pointer!" );

  if ( ckTheta > 0 ) {

    // track information
    const RichTrackSegment & trackSeg = segment->trackSegment();

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

      // Photon emission point is half-way between segment start and end points
      const HepPoint3D & emissionPt = trackSeg.bestPoint();

      // Photon direction around loop
      const HepVector3D photDir = rotation * HepVector3D( sinCkTheta*(*iCos),
                                                          sinCkTheta*(*iSin),
                                                          cosCkTheta );

      // Ray trace to detector plain
      HepPoint3D hitPoint;
      if ( m_rayTrace->traceToDetectorWithoutEff( trackSeg.rich(),
                                                  emissionPt,
                                                  photDir,
                                                  hitPoint,
                                                  DeRichHPDPanel::loose ) ) {
        points.push_back( hitPoint );
      }

    }

    if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << "Successfully traced " << points.size() 
                << " points to detector plane for segment " << segment->key() << endreq;
    }

    return StatusCode::SUCCESS;
  } else {
    // cannot perform ray-tracing
    return StatusCode::FAILURE;
  }

}
