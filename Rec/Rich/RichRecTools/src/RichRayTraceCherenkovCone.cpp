
//-----------------------------------------------------------------------------
/** @file RichRayTraceCherenkovCone.cpp
 *
 *  Implementation file for tool : RichRayTraceCherenkovCone
 *
 *  CVS Log :-
 *  $Id: RichRayTraceCherenkovCone.cpp,v 1.12 2005-10-18 13:03:51 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichRayTraceCherenkovCone.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRayTraceCherenkovCone>          s_factory ;
const        IToolFactory& RichRayTraceCherenkovConeFactory = s_factory ;

// Standard constructor
RichRayTraceCherenkovCone::RichRayTraceCherenkovCone( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_rayTrace      ( 0 ),
    m_ckAngle       ( 0 ),
    m_smartIDTool   ( 0 ),
    m_nRayTrace     ( 0 )
{
  // Define interface for this tool
  declareInterface<IRichRayTraceCherenkovCone>(this);

  // Define job option parameters
  declareProperty( "NPhotonsRayTrace", m_nRayTrace = 100 );
}

StatusCode RichRayTraceCherenkovCone::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",     m_rayTrace );
  acquireTool( "RichCherenkovAngle", m_ckAngle  );
  acquireTool( "RichSmartIDTool", m_smartIDTool, 0, true );

  // Set up cached parameters for photon tracing
  m_incPhi = M_2PI / static_cast<double>(m_nRayTrace) ;

  return sc;
}

StatusCode RichRayTraceCherenkovCone::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

const std::vector<HepPoint3D> &
RichRayTraceCherenkovCone::rayTraceLocal ( RichRecRing * ring,
                                           const RichTraceMode mode ) const
{
  if ( !ring ) Exception( "Null RichRecRing pointer!" );

  if ( ring->ringPointsLocal().empty() ) {

    // Check global points have been computed
    if ( ring->ringPoints().empty() ) rayTrace ( ring, mode );

    // convert global to local points
    for ( std::vector<HepPoint3D>::const_iterator iP = ring->ringPoints().begin();
          iP != ring->ringPoints().end(); ++iP ) {
      ring->ringPointsLocal().push_back( m_smartIDTool->globalToPDPanel(*iP) );
    }

  }

  return ring->ringPointsLocal();
}

const std::vector<HepPoint3D> &
RichRayTraceCherenkovCone::rayTrace ( RichRecRing * ring,
                                      const RichTraceMode mode ) const
{
  if ( !ring ) Exception( "Null RichRecRing pointer!" );

  // cache points in each ring
  if ( ring->ringPoints().empty() ) {
    rayTrace( ring->richRecSegment(), ring->radius(), ring->ringPoints(), mode );
  }

  return ring->ringPoints();
}

StatusCode
RichRayTraceCherenkovCone::rayTrace ( RichRecSegment * segment,
                                      const Rich::ParticleIDType id,
                                      std::vector<HepPoint3D> & points,
                                      const RichTraceMode mode ) const
{
  return rayTrace( segment,
                   m_ckAngle->avgCherenkovTheta(segment, id),
                   points, mode );
}

StatusCode
RichRayTraceCherenkovCone::rayTrace ( RichRecSegment * segment,
                                      const double ckTheta,
                                      std::vector<HepPoint3D> & points,
                                      const RichTraceMode mode ) const
{
  // make sure segment is valid
  if ( !segment ) Exception( "Null RichRecSegment pointer!" );

  // Do the ray-tracing
  return rayTrace ( segment->trackSegment().rich(),
                    segment->trackSegment().bestPoint(),
                    segment->trackSegment().bestMomentum(),
                    ckTheta, points, mode );
}

StatusCode
RichRayTraceCherenkovCone::rayTrace ( const Rich::DetectorType rich,
                                      const HepPoint3D & emissionPoint,
                                      const HepVector3D & direction,
                                      const double ckTheta,
                                      std::vector<HepPoint3D> & points,
                                      const RichTraceMode mode ) const
{

  if ( ckTheta > 0 ) {

    // Define rotation matrix
    const HepVector3D z = direction.unit();
    HepVector3D y = z.cross( HepVector3D(0.,1.,0.) );
    y.setMag(1);
    const HepVector3D x = y.cross(z);
    HepRotation rotation = HepRotation();
    rotation.rotateAxes(x,y,z);

    // loop around the ring
    const double sinCkTheta = sin(ckTheta);
    const double cosCkTheta = cos(ckTheta);   
    double ckPhi = 0.0;
    for ( int iPhot = 0; iPhot < m_nRayTrace; ++iPhot, ckPhi+=m_incPhi ) {

      // Photon direction around loop
      const HepVector3D photDir = rotation * HepVector3D( sinCkTheta*cos(ckPhi),
                                                          sinCkTheta*sin(ckPhi),
                                                          cosCkTheta );

      // Ray trace to detector plane
      HepPoint3D hitPoint;
      if ( m_rayTrace->traceToDetectorWithoutEff( rich,
                                                  emissionPoint,
                                                  photDir,
                                                  hitPoint,
                                                  mode ) ) {
        points.push_back( hitPoint );
      }

    }

    return StatusCode::SUCCESS;
  } else {
    // cannot perform ray-tracing
    return StatusCode::FAILURE;
  }

}
