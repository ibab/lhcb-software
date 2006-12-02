
//-----------------------------------------------------------------------------
/** @file RichRayTracing.cpp
 *
 * Implementation file for class : RichRayTracing
 *
 * CVS Log :-
 * $Id: RichRayTracing.cpp,v 1.30 2006-12-02 22:27:12 jonrob Exp $
 *
 * @author Antonis Papanestis
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2003-11-14
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"

// local
#include "RichRayTracing.h"

DECLARE_TOOL_FACTORY( RichRayTracing );

namespace
{
  /// A z point that separates Rich1 from Rich2 (anything between 3000-9000mm)
  static const double s_RichDetSeparationPointZ = 8000.0;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRayTracing::RichRayTracing( const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : RichHistoToolBase         ( type, name, parent  ),
    m_rich                    ( Rich::NRiches       ),
    m_sphMirrorSegRows        ( Rich::NRiches, 0    ),
    m_sphMirrorSegCols        ( Rich::NRiches, 0    ),
    m_secMirrorSegRows        ( Rich::NRiches, 0    ),
    m_secMirrorSegCols        ( Rich::NRiches, 0    ),
    m_deBeam                  ( Rich::NRiches       )
{
  // interface
  declareInterface<IRichRayTracing>(this);
  // job options
  declareProperty( "IgnoreSecondaryMirrors", m_ignoreSecMirrs = false );
}

//=============================================================================
// Destructor
//=============================================================================
RichRayTracing::~RichRayTracing() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode RichRayTracing::initialize()
{

  // intialise base class
  const StatusCode sc = RichHistoToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  acquireTool( "RichMirrorSegFinder", m_mirrorSegFinder );

  // HPD panel names
  const std::string pdPanelName[2][2] = { { DeRichHPDPanelLocation::Rich1Panel0,
                                            DeRichHPDPanelLocation::Rich1Panel1 },
                                          { DeRichHPDPanelLocation::Rich2Panel0,
                                            DeRichHPDPanelLocation::Rich2Panel1 } };

  // RICH detector elements
  const DeRich* rich1 = getDet<DeRich>( DeRichLocation::Rich1 );
  const DeRich* rich2 = getDet<DeRich>( DeRichLocation::Rich2 );
  m_rich[Rich::Rich1] = rich1;
  m_rich[Rich::Rich2] = rich2;

  // loop over riches and photo detector panels
  unsigned int rich, panel;
  for ( rich=0; rich<m_photoDetPanels.size(); ++rich )
  {
    for ( panel=0; panel<m_photoDetPanels[rich].size(); ++panel )
    {
      m_photoDetPanels[rich][panel] = getDet<DeRichHPDPanel>( pdPanelName[rich][panel] );
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Stored photodetector panel "
                << m_photoDetPanels[rich][panel]->name() << endreq;
    }
  }

  // Rich1 mirrors
  if ( rich1->exists("SphMirrorSegRows") )
  {
    m_sphMirrorSegRows[Rich::Rich1] = rich1->param<int>( "SphMirrorSegRows"    );
    m_sphMirrorSegCols[Rich::Rich1] = rich1->param<int>( "SphMirrorSegColumns" );
  }
  else
  {
    return Error ( "No primary mirrors for RICH1 found !" );
  }
  if ( rich1->exists("SecMirrorSegRows") )
  {
    m_secMirrorSegRows[Rich::Rich1] = rich1->param<int>( "SecMirrorSegRows"    );
    m_secMirrorSegCols[Rich::Rich1] = rich1->param<int>( "SecMirrorSegColumns" );
  }
  else
  {
    return Error ( "No secondary mirrors for RICH1 found !" );
  }

  // Rich2 mirrors
  if ( rich2->exists("SphMirrorSegRows") )
  {
    m_sphMirrorSegRows[Rich::Rich2] = rich2->param<int>( "SphMirrorSegRows" );
    m_sphMirrorSegCols[Rich::Rich2] = rich2->param<int>( "SphMirrorSegColumns" );
  }
  else
  {
    return Error ( "No primary mirrors for RICH2 found !" );
  }
  if ( rich2->exists("SecMirrorSegRows") )
  {
    m_secMirrorSegRows[Rich::Rich2] = rich2->param<int>( "SecMirrorSegRows" );
    m_secMirrorSegCols[Rich::Rich2] = rich2->param<int>( "SecMirrorSegColumns" );
  }
  else
  {
    return Error ( "No secondary mirrors for RICH2 found !" );
  }

  // beam pipe objects
  m_deBeam[Rich::Rich1] = getDet<DeRichBeamPipe>( DeRichBeamPipeLocation::Rich1BeamPipe );
  m_deBeam[Rich::Rich2] = getDet<DeRichBeamPipe>( DeRichBeamPipeLocation::Rich2BeamPipe );

  if ( m_ignoreSecMirrs )
  {
    Warning( "Will ignore secondary mirrors", StatusCode::SUCCESS );
  }

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichRayTracing::finalize()
{
  // finalize base class
  return RichHistoToolBase::finalize();
}

//=============================================================================
// reflect the trajectory on the mirror, and determine the position where
// it hits the detector plane,
// take into account the geometrical boundaries of mirrors and detector
//=============================================================================
StatusCode RichRayTracing::traceToDetector ( const Rich::DetectorType rich,
                                             const Gaudi::XYZPoint& startPoint,
                                             const Gaudi::XYZVector& startDir,
                                             Gaudi::XYZPoint& hitPosition,
                                             const LHCb::RichTraceMode mode,
                                             const Rich::Side forcedSide ) const
{
  // need to think if this can be done without creating a temp RichGeomPhoton ?
  LHCb::RichGeomPhoton photon;
  const StatusCode sc =
    traceToDetector ( rich, startPoint, startDir, photon, mode, forcedSide );
  hitPosition = photon.detectionPoint();
  return sc;
}

//=============================================================================
// reflect the trajectory on the mirror, and determine the position where
// it hits the detector plane,
// take into account the geometrical boundaries of mirrors and detector
//=============================================================================
StatusCode RichRayTracing::traceToDetector ( const Rich::DetectorType rich,
                                             const Gaudi::XYZPoint& startPoint,
                                             const Gaudi::XYZVector& startDir,
                                             LHCb::RichGeomPhoton& photon,
                                             const LHCb::RichTraceMode mode,
                                             const Rich::Side forcedSide ) const
{
  Gaudi::XYZPoint tmpPosition( startPoint );
  Gaudi::XYZVector tmpDirection( startDir );

  // reflect off both mirrors
  if ( reflectBothMirrors( rich, tmpPosition, tmpDirection, photon,
                           mode, forcedSide ).isFailure() )
  { return StatusCode::FAILURE; }

  // for hit point use photon data directly
  Gaudi::XYZPoint & hitPosition = photon.detectionPoint();

  // the detector side
  const Rich::Side side = m_rich[rich]->side(tmpPosition);

  // smart ID for RICH and panel (to be filled further when possible in following methods)
  LHCb::RichSmartID smartID ( rich, side );

  // do ray tracing, depending on mode
  StatusCode sc = StatusCode::FAILURE;

  // are we configured to test individual HPD acceptance
  if ( mode.detPlaneBound() == LHCb::RichTraceMode::RespectHPDTubes )
  {
    // ... yes, then use method to test HPD acceptance (using mode)
    sc = m_photoDetPanels[rich][side]->PDWindowPoint( tmpDirection, tmpPosition,
                                                      hitPosition, smartID, mode );
  }
  else
  {
    // ... no, so just trace to HPD panel
    // NOTE : smartID is not updated any more so will only contain RICH and panel data
    sc = m_photoDetPanels[rich][side]->detPlanePoint( tmpPosition, tmpDirection,
                                                      hitPosition, mode );
  }

  // Set remaining RichGeomPhoton data
  photon.setSmartID        ( smartID     );
  photon.setEmissionPoint  ( startPoint  );

  // test for beam pipe intersections ?
  if ( sc.isSuccess() && mode.beamPipeIntersects() )
  {
    Gaudi::XYZPoint inter1, inter2;
    // test for intersections between emission point and spherical reflection point
    const DeRichBeamPipe::BeamPipeIntersectionType intType
      = deBeamPipe(rich)->intersectionPoints( startPoint,
                                              photon.sphMirReflectionPoint()-startPoint,
                                              inter1, inter2 );
    if ( intType != DeRichBeamPipe::NoIntersection )
    {
      sc = StatusCode::FAILURE;
    }
    // Probably not needed to check for other intersections ?
  }

  // return status code
  return sc;
}

//=========================================================================
//  reflect a photon on both mirrors and return the position and direction
//  on the secondary mirror.
//=========================================================================
StatusCode RichRayTracing::reflectBothMirrors( const Rich::DetectorType rich,
                                               Gaudi::XYZPoint& position,
                                               Gaudi::XYZVector& direction,
                                               LHCb::RichGeomPhoton& photon,
                                               const LHCb::RichTraceMode mode,
                                               const Rich::Side forcedSide ) const
{

  Gaudi::XYZPoint tmpPosition( position );
  Gaudi::XYZVector tmpDirection( direction );

  // which side wre we on ?
  Rich::Side side = ( mode.forcedSide() ? forcedSide : m_rich[rich]->side(tmpPosition) );

  // Spherical mirror reflection with nominal parameters
  if ( !reflectSpherical( tmpPosition, tmpDirection,
                          m_rich[rich]->nominalCentreOfCurvature(side),
                          m_rich[rich]->sphMirrorRadius() ) )
    return StatusCode::FAILURE;

  // if not forced, check if still same side, if not change sides
  if ( !mode.forcedSide() )
  {
    const Rich::Side tmpSide = m_rich[rich]->side(tmpPosition);
    if ( side != tmpSide )
    {
      side = tmpSide;
      tmpPosition = position;
      tmpDirection = direction;
      if ( !reflectSpherical( tmpPosition, tmpDirection,
                              m_rich[rich]->nominalCentreOfCurvature(side),
                              m_rich[rich]->sphMirrorRadius() ) )
      { return StatusCode::FAILURE; }
    }
  }

  // find segment
  const DeRichSphMirror* sphSegment = m_mirrorSegFinder->findSphMirror( rich, side, tmpPosition);

  // depending on the tracing flag
  if ( mode.mirrorSegBoundary() )
  {
    // if reflection from a mirror segment is required
    if ( !sphSegment->intersects( position, direction ) )
    {
      if ( produceHistos() )
        plot2D( tmpPosition.x(), tmpPosition.y(),
                "Spherical Mirror missed gap "+Rich::text(rich),
                -18000, 18000, -1500, 15000, 100, 100 );
      return StatusCode::FAILURE;
    }

  }
  else if ( mode.outMirrorBoundary() )
  {
    // check the outside boundaries of the (whole) mirror
    if ( !sphSegment->intersects( position, direction ) )
    {
      const RichMirrorSegPosition pos = m_rich[rich]->sphMirrorSegPos( sphSegment->mirrorNumber() );
      const Gaudi::XYZPoint & mirCentre = sphSegment->mirrorCentre();
      bool fail( false );
      if ( pos.row() == 0 ) 
      { // bottom segment
        if ( tmpPosition.y() < mirCentre.y() ) fail = true;
      }
      if ( pos.row() == m_sphMirrorSegRows[rich]-1 ) 
      { // top segment
        if ( tmpPosition.y() > mirCentre.y() ) fail = true;
      }
      if ( pos.column() == 0 ) 
      { // right side
        if ( tmpPosition.x() < mirCentre.x() ) fail = true;
      }
      if ( pos.column() == m_sphMirrorSegCols[rich]-1 ) 
      { // left side
        if ( tmpPosition.x() > mirCentre.x() ) fail = true;
      }
      if (fail)
      {
        if ( produceHistos() )
          plot2D( tmpPosition.x(), tmpPosition.y(),
                  "Spherical Mirror missed out "+Rich::text(rich),
                  -1800, 1800, -1500, 1500, 100, 100 );
        return StatusCode::FAILURE;
      }
    }
  }

  // reset position, direction before trying again
  tmpPosition  = position;
  tmpDirection = direction;

  // Spherical mirror reflection with exact parameters
  if ( !reflectSpherical( tmpPosition, tmpDirection,
                          sphSegment->centreOfCurvature(),
                          sphSegment->radius() ) )
    return StatusCode::FAILURE;

  // set primary mirror data
  photon.setSphMirReflectionPoint( tmpPosition );
  photon.setSphMirrorNum(sphSegment->mirrorNumber());

  // Are we ignoring the secondary mirrors ?
  if ( !m_ignoreSecMirrs )
  {

    Gaudi::XYZPoint planeIntersection;
    // sec mirror reflection with nominal parameters
    if ( !intersectPlane( tmpPosition,
                          tmpDirection,
                          m_rich[rich]->nominalPlane(side),
                          planeIntersection) )
      return StatusCode::FAILURE;

    // find secondary mirror segment
    const DeRichSphMirror* secSegment = m_mirrorSegFinder->findSecMirror(rich,side,planeIntersection);

    // depending on the tracing flag:
    if ( mode.mirrorSegBoundary() )
    {
      // if reflection from a mirror segment is required
      if ( !secSegment->intersects( tmpPosition, tmpDirection ) )
      {
        if ( produceHistos() )
          plot2D( planeIntersection.x(), planeIntersection.y(),
                  "Sec Mirror missed gap "+Rich::text(rich),
                  -3000, 3000, -1000, 1000, 100, 100 );
        return StatusCode::FAILURE;
      }

    }
    else if ( mode.outMirrorBoundary() )
    {

      // check the outside boundaries of the (whole) mirror
      if ( !secSegment->intersects( tmpPosition, tmpDirection ) ) {
        const RichMirrorSegPosition pos = m_rich[rich]->secMirrorSegPos( secSegment->mirrorNumber() );
        const Gaudi::XYZPoint& mirCentre = secSegment->mirrorCentre();
        bool fail( false );
        if ( pos.row() == 0 ) {                 // bottom segment
          if ( planeIntersection.y() < mirCentre.y() )
            fail = true;
        }
        if ( pos.row() == m_secMirrorSegRows[rich]-1 ) { // top segment
          if ( planeIntersection.y() > mirCentre.y() )
            fail = true;
        }
        if ( pos.column() == 0 ) {                 // right side
          if ( planeIntersection.x() < mirCentre.x() )
            fail = true;
        }
        if ( pos.column() == m_secMirrorSegCols[rich]-1 ) {   // left side
          if ( planeIntersection.x() > mirCentre.x() )
            fail = true;
        }
        if (fail)
        {
          if ( produceHistos() )
            plot2D( planeIntersection.x(), planeIntersection.y(),
                    "Sec Mirror missed out "+Rich::text(rich),
                    -3000, 3000, -1000, 1000, 100, 100 );
          return StatusCode::FAILURE;
        }
      }

    }

    // Secondary mirror reflection with actual parameters
    if ( !reflectSpherical( tmpPosition, tmpDirection,
                            secSegment->centreOfCurvature(),
                            secSegment->radius() ) )
    { return StatusCode::FAILURE; }

    // set secondary ("flat") mirror data
    photon.setFlatMirReflectionPoint( tmpPosition );
    photon.setFlatMirrorNum(secSegment->mirrorNumber());

  } // ignore secondary mirrors

  // Set final direction and position data
  position  = tmpPosition;
  direction = tmpDirection;

  return StatusCode::SUCCESS;
}


//=========================================================================
// Raytraces from a point in the detector panel back to the spherical mirror
// returning the mirror intersection point and the direction a track would
// have in order to hit that point in the detector panel.
//=========================================================================
StatusCode
RichRayTracing::traceBackFromDetector ( const Gaudi::XYZPoint& startPoint,
                                        const Gaudi::XYZVector& startDir,
                                        Gaudi::XYZPoint& endPoint,
                                        Gaudi::XYZVector& endDir ) const
{

  Gaudi::XYZPoint tmpStartPoint( startPoint );
  Gaudi::XYZVector tmpStartDir( startDir );

  // which RICH ?
  const Rich::DetectorType rich
    = ( startPoint.z()/Gaudi::Units::mm < s_RichDetSeparationPointZ ?
        Rich::Rich1 : Rich::Rich2 );
  // which side ?
  const Rich::Side side = m_rich[rich]->side(startPoint);

  // are we using the secondary mirrors ?
  if ( !m_ignoreSecMirrs )
  {

    Gaudi::XYZPoint planeIntersection;
    // sec mirror reflection with nominal parameters
    if ( !intersectPlane( tmpStartPoint,
                          tmpStartDir,
                          m_rich[rich]->nominalPlane(side),
                          planeIntersection ) )
    { return StatusCode::FAILURE; }

    // find secondary mirror segment
    const DeRichSphMirror* secSegment = m_mirrorSegFinder->
      findSecMirror(rich,side,planeIntersection);

    if ( !reflectSpherical( tmpStartPoint, tmpStartDir,
                            secSegment->centreOfCurvature(),
                            secSegment->radius() ) )
    { return StatusCode::FAILURE; }

  }

  // save points after first mirror reflection
  Gaudi::XYZPoint storePoint( tmpStartPoint );
  Gaudi::XYZVector storeDir( tmpStartDir );

  // Spherical mirror reflection with nominal parameters
  if ( !reflectSpherical( tmpStartPoint, tmpStartDir,
                          m_rich[rich]->nominalCentreOfCurvature(side),
                          m_rich[rich]->sphMirrorRadius() ) )
  { return StatusCode::FAILURE; }

  // find primary mirror segment
  const DeRichSphMirror* sphSegment = m_mirrorSegFinder->
    findSphMirror( rich, side,tmpStartPoint );

  // Spherical mirror reflection with exact parameters
  if ( !reflectSpherical( storePoint, storeDir,
                          sphSegment->centreOfCurvature(),
                          sphSegment->radius() ) )
  { return StatusCode::FAILURE; }

  endPoint  = storePoint;
  endDir    = storeDir;

  return StatusCode::SUCCESS;

}

//=========================================================================
//  reflect from a spherical mirror
//=========================================================================
StatusCode RichRayTracing::reflectSpherical ( Gaudi::XYZPoint& position,
                                              Gaudi::XYZVector& direction,
                                              const Gaudi::XYZPoint& CoC,
                                              const double radius ) const
{
  // find intersection point
  // for line sphere intersection look at http://www.realtimerendering.com/int/
  const Gaudi::XYZVector delta( position - CoC );

  const double a = direction.Mag2();
  const double b = 2. * direction.Dot( delta );
  const double c = delta.Mag2() - radius*radius;
  const double discr = b*b - 4.*a*c;
  if ( discr < 0 ) return StatusCode::FAILURE;

  const double distance1 = 0.5 * ( sqrt(discr) - b ) / a;
  // change position to the intersection point
  position += distance1 * direction;

  const Gaudi::XYZVector normal( delta.Unit() );
  // reflect the vector
  // r = u - 2(u.n)n, r=reflction, u=insident, n=normal
  direction -= 2.0 * (normal.Dot(direction)) * normal;

  return StatusCode::SUCCESS;
}


//=========================================================================
//  reflect from a flat mirror
//=========================================================================
StatusCode RichRayTracing::reflectFlatPlane ( Gaudi::XYZPoint& position,
                                              Gaudi::XYZVector& direction,
                                              const Gaudi::Plane3D& plane ) const
{
  // temp intersection point
  Gaudi::XYZPoint intersection;

  // refect of the plane
  const StatusCode sc = intersectPlane( position, direction, plane, intersection );
  if ( sc.isFailure() ) { return sc; }

  // plane normal
  const Gaudi::XYZVector normal( plane.Normal() );

  // update position to intersection point
  position = intersection;

  // reflect the vector and update direction
  // r = u - 2(u.n)n, r=reflction, u=insident, n=normal
  direction -= 2.0 * (normal.Dot(direction)) * normal;

  // return status code
  return sc;
}

//=========================================================================
//  intersect a plane
//=========================================================================
StatusCode RichRayTracing::intersectPlane ( const Gaudi::XYZPoint& position,
                                            const Gaudi::XYZVector& direction,
                                            const Gaudi::Plane3D& plane,
                                            Gaudi::XYZPoint& intersection ) const
{
  const double scalar = direction.Dot( plane.Normal() );
  if ( fabs(scalar) < 1e-99 ) return StatusCode::FAILURE;

  const double distance = -(plane.Distance(position)) / scalar;
  intersection = position + distance*direction;

  return StatusCode::SUCCESS;
}

//=========================================================================
