
//-----------------------------------------------------------------------------
/** @file RichRayTracing.cpp
 *
 * Implementation file for class : RichRayTracing
 *
 * CVS Log :-
 * $Id: RichRayTracing.cpp,v 1.39 2007-08-10 09:23:12 jonrob Exp $
 *
 * @author Antonis Papanestis
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2003-11-14
 */
//-----------------------------------------------------------------------------

// local
#include "RichRayTracing.h"

/// Factory stuff
DECLARE_NAMESPACE_TOOL_FACTORY( Rich, RayTracing );

namespace Rich
{
  /// A z point that separates Rich1 from Rich2 (anything between 3000-9000mm)
  static const double s_RichDetSeparationPointZ = 8000.0;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::RayTracing::RayTracing( const std::string& type,
                              const std::string& name,
                              const IInterface* parent)
  : RichHistoToolBase         ( type, name, parent  ),
    m_refIndex                ( NULL                ),
    m_rich                    ( Rich::NRiches       ),
    m_sphMirrorSegRows        ( Rich::NRiches, 0    ),
    m_sphMirrorSegCols        ( Rich::NRiches, 0    ),
    m_secMirrorSegRows        ( Rich::NRiches, 0    ),
    m_secMirrorSegCols        ( Rich::NRiches, 0    ),
    m_deBeam                  ( Rich::NRiches       )
{
  // interface
  declareInterface<IRayTracing>(this);

  // default to having histograms disabled
  setProduceHistos ( false );

  // initialise
  m_deBeam[Rich::Rich1] = NULL;
  m_deBeam[Rich::Rich2] = NULL;

  // job options
  declareProperty( "IgnoreSecondaryMirrors", m_ignoreSecMirrs = false );
}

//=============================================================================
// Destructor
//=============================================================================
Rich::RayTracing::~RayTracing() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode Rich::RayTracing::initialize()
{

  // intialise base class
  const StatusCode sc = RichHistoToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  acquireTool( "RichMirrorSegFinder", m_mirrorSegFinder );
  acquireTool( "RichRefractiveIndex", m_refIndex        );

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

  if ( m_ignoreSecMirrs )
  {
    Warning( "Will ignore secondary mirrors", StatusCode::SUCCESS );
  }

  // initialise aerogel refraction correction
  {
    // radiator tool
    const IRadiatorTool * radTool;
    acquireTool( "RichRadiatorTool", radTool );

    // get three points in exit plane
    RichRadIntersection::Vector intersections;
    Gaudi::XYZPoint p1(100,100,0), p2(100,-100,0), p3(-100,-100,0);
    const Gaudi::XYZVector v(0,0,1);
    radTool->intersections( p1, v, Rich::Aerogel, intersections );
    p1 = intersections.back().exitPoint();
    m_minZaero = p1.z();
    radTool->intersections( p2, v, Rich::Aerogel, intersections );
    p2 = intersections.back().exitPoint();
    if ( p2.z() < m_minZaero ) m_minZaero = p2.z();
    radTool->intersections( p3, v, Rich::Aerogel, intersections );
    p3 = intersections.back().exitPoint();
    if ( p3.z() < m_minZaero ) m_minZaero = p3.z();
    debug() << "Points " << p1 << " " << p2 << " " << p3 << endreq;

    // make a plane using these points
    m_aeroExitPlane = Gaudi::Plane3D(p1,p2,p3);

    // get the normal to the plane
    m_aeroNormVect = m_aeroExitPlane.Normal();

    // release tools no longer needed
    releaseTool(radTool);
  }

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Rich::RayTracing::finalize()
{
  // finalize base class
  return RichHistoToolBase::finalize();
}

//=============================================================================
// reflect the trajectory on the mirror, and determine the position where
// it hits the detector plane,
// take into account the geometrical boundaries of mirrors and detector
//=============================================================================
LHCb::RichTraceMode::RayTraceResult
Rich::RayTracing::traceToDetector ( const Rich::DetectorType rich,
                                    const Gaudi::XYZPoint& startPoint,
                                    const Gaudi::XYZVector& startDir,
                                    Gaudi::XYZPoint& hitPosition,
                                    const LHCb::RichTraceMode mode,
                                    const Rich::Side forcedSide,
                                    const double photonEnergy ) const
{
  // need to think if this can be done without creating a temp RichGeomPhoton ?
  LHCb::RichGeomPhoton photon;
  const LHCb::RichTraceMode::RayTraceResult sc =
    traceToDetector ( rich, startPoint, startDir, photon, mode, forcedSide, photonEnergy );
  hitPosition = photon.detectionPoint();
  return sc;
}

//=============================================================================
// reflect the trajectory on the mirror, and determine the position where
// it hits the detector plane,
// take into account the geometrical boundaries of mirrors and detector
//=============================================================================
LHCb::RichTraceMode::RayTraceResult
Rich::RayTracing::traceToDetector ( const Rich::DetectorType rich,
                                    const Gaudi::XYZPoint& startPoint,
                                    const Gaudi::XYZVector& startDir,
                                    LHCb::RichGeomPhoton& photon,
                                    const LHCb::RichTraceMode mode,
                                    const Rich::Side forcedSide,
                                    const double photonEnergy ) const
{
  Gaudi::XYZPoint  tmpPos ( startPoint );
  Gaudi::XYZVector tmpDir ( startDir   );

  // Correct start point/direction for aerogel refraction, if appropriate
  // see http://en.wikipedia.org/wiki/Snell's_law for details
  if ( mode.aeroRefraction() && photonEnergy > 0 && startPoint.z() < m_minZaero )
  {
    // get refractive indices for aerogel and rich1Gas
    const double refAero     = m_refIndex->refractiveIndex( Rich::Aerogel,  photonEnergy );
    const double refrich1Gas = m_refIndex->refractiveIndex( Rich::Rich1Gas, photonEnergy );
    const double Rratio = refAero/refrich1Gas;
    // normalise the direction vector
    tmpDir = tmpDir.Unit();
    // the dot product between the plane normal and the direction
    const double cosT1 = tmpDir.Dot(m_aeroNormVect);
    // update the point extrapolating the photon dir inside the aerogel to exit plane
    const double distance = -(m_aeroExitPlane.Distance(tmpPos))/cosT1;
    tmpPos += distance*tmpDir;
    // Update the direction due to refraction on exit
    const double cosT2 = sqrt( 1.0 - Rratio*Rratio*( 1.0 - cosT1*cosT1 ) );
    if ( cosT1<0 ) { tmpDir = tmpDir*Rratio - m_aeroNormVect*(cosT2+(Rratio*cosT1)); }
    else           { tmpDir = tmpDir*Rratio + m_aeroNormVect*(cosT2-(Rratio*cosT1)); }
  }

  // default result is failure
  LHCb::RichTraceMode::RayTraceResult result = LHCb::RichTraceMode::RayTraceFailed;

  // first, try and reflect of both mirrors
  bool sc = reflectBothMirrors( rich, tmpPos, tmpDir, photon,
                                mode, forcedSide );
  if ( sc )
  {

    // for hit point use photon data directly
    Gaudi::XYZPoint & hitPosition = photon.detectionPoint();

    // the detector side
    const Rich::Side side = m_rich[rich]->side(tmpPos);

    // smart ID for RICH and panel (to be filled further when possible in following methods)
    LHCb::RichSmartID smartID ( rich, side );

    // do ray tracing, depending on mode

    // are we configured to test individual HPD acceptance
    if ( mode.detPlaneBound() == LHCb::RichTraceMode::RespectHPDTubes )
    {
      // ... yes, then use method to test HPD acceptance (using mode)
      result = m_photoDetPanels[rich][side]->PDWindowPoint( tmpDir,tmpPos,
                                                            hitPosition, smartID, mode );
    }
    else
    {
      // ... no, so just trace to HPD panel
      // NOTE : smartID is not updated any more so will only contain RICH and panel data
      result = m_photoDetPanels[rich][side]->detPlanePoint( tmpPos, tmpDir,
                                                            hitPosition, mode );
    }

    // Set remaining RichGeomPhoton data
    photon.setPixelCluster   ( Rich::HPDPixelCluster(smartID) );
    photon.setEmissionPoint  ( startPoint                     );

    // test for beam pipe intersections ?
    if ( mode.traceWasOK(result) && mode.beamPipeIntersects() )
    {
      // test for intersections between emission point and spherical reflection point
      if ( deBeam(rich)->testForIntersection( startPoint,
                                              photon.sphMirReflectionPoint()-startPoint ) )
      {
        result = LHCb::RichTraceMode::OutsideHPDPanel; // CRJ : Do we need a special result flag ?
      }
      // Probably not needed to check for other intersections ?
    }

  } // mirrors reflection OK

  // return the result
  return result;
}

//=========================================================================
//  reflect a photon on both mirrors and return the position and direction
//  on the secondary mirror.
//=========================================================================
bool Rich::RayTracing::reflectBothMirrors( const Rich::DetectorType rich,
                                           Gaudi::XYZPoint& position,
                                           Gaudi::XYZVector& direction,
                                           LHCb::RichGeomPhoton& photon,
                                           const LHCb::RichTraceMode mode,
                                           const Rich::Side forcedSide ) const
{

  Gaudi::XYZPoint  tmpPos ( position  );
  Gaudi::XYZVector tmpDir ( direction );

  // which side are we on ?
  Rich::Side side = ( mode.forcedSide() ? forcedSide : m_rich[rich]->side(tmpPos) );

  // Spherical mirror reflection with nominal parameters
  if ( !reflectSpherical( tmpPos, tmpDir,
                          m_rich[rich]->nominalCentreOfCurvature(side),
                          m_rich[rich]->sphMirrorRadius() ) )
    return false;

  // if not forced, check if still same side, if not change sides
  if ( !mode.forcedSide() )
  {
    const Rich::Side tmpSide = m_rich[rich]->side(tmpPos);
    if ( side != tmpSide )
    {
      side   = tmpSide;
      tmpPos = position;
      tmpDir = direction;
      if ( !reflectSpherical( tmpPos, tmpDir,
                              m_rich[rich]->nominalCentreOfCurvature(side),
                              m_rich[rich]->sphMirrorRadius() ) )
        return false;
    }
  }

  // find segment
  const DeRichSphMirror* sphSegment = m_mirrorSegFinder->findSphMirror( rich, side, tmpPos);

  // depending on the tracing flag
  if ( mode.mirrorSegBoundary() )
  {
    // if reflection from a mirror segment is required
    if ( !sphSegment->intersects( position, direction ) )
    {
      // CRJ : Comment out for speed in HLT
      //if ( produceHistos() )
      //  plot2D( tmpPos.x(), tmpPos.y(),
      //          "Spherical Mirror missed gap "+Rich::text(rich),
      //          -18000, 18000, -1500, 15000, 100, 100 );
      return false;
    }

  }
  else if (  mode.outMirrorBoundary() )
  {
    // check the outside boundaries of the (whole) mirror
    if ( !sphSegment->intersects( position, direction ) )
    {
      const RichMirrorSegPosition pos = m_rich[rich]->sphMirrorSegPos( sphSegment->mirrorNumber() );
      const Gaudi::XYZPoint & mirCentre = sphSegment->mirrorCentre();
      bool fail( false );
      if ( pos.row() == 0 )
      { // bottom segment
        if ( tmpPos.y() < mirCentre.y() ) fail = true;
      }
      if ( pos.row() == m_sphMirrorSegRows[rich]-1 )
      { // top segment
        if ( tmpPos.y() > mirCentre.y() ) fail = true;
      }
      if ( pos.column() == 0 )
      { // right side
        if ( tmpPos.x() < mirCentre.x() ) fail = true;
      }
      if ( pos.column() == m_sphMirrorSegCols[rich]-1 )
      { // left side
        if ( tmpPos.x() > mirCentre.x() ) fail = true;
      }
      if (fail)
      {
        // CRJ : Comment out for speed in HLT
        //if ( produceHistos() )
        //  plot2D( tmpPos.x(), tmpPos.y(),
        //          "Spherical Mirror missed out "+Rich::text(rich),
        //          -1800, 1800, -1500, 1500, 100, 100 );
        return false;
      }
    }
  }

  // reset position, direction before trying again
  tmpPos = position;
  tmpDir = direction;

  // Spherical mirror reflection with exact parameters
  if ( !reflectSpherical( tmpPos, tmpDir,
                          sphSegment->centreOfCurvature(),
                          sphSegment->radius() ) )
    return false;

  // set primary mirror data
  photon.setSphMirReflectionPoint( tmpPos );
  photon.setSphMirrorNum(sphSegment->mirrorNumber());

  // Are we ignoring the secondary mirrors ?
  if ( !m_ignoreSecMirrs )
  {

    Gaudi::XYZPoint planeIntersection;
    // sec mirror reflection with nominal parameters
    if ( !intersectPlane( tmpPos,
                          tmpDir,
                          m_rich[rich]->nominalPlane(side),
                          planeIntersection) )
      return false;

    // find secondary mirror segment
    const DeRichSphMirror* secSegment = m_mirrorSegFinder->findSecMirror(rich,side,planeIntersection);

    // depending on the tracing flag:
    if ( mode.mirrorSegBoundary() ) {
      // if reflection from a mirror segment is required
      if ( !secSegment->intersects( tmpPos, tmpDir ) )
      {
        //if ( produceHistos() )
        //  plot2D( planeIntersection.x(), planeIntersection.y(),
        //          "Sec Mirror missed gap "+Rich::text(rich),
        //          -3000, 3000, -1000, 1000, 100, 100 );
        return false;
      }

    }
    else if ( mode.outMirrorBoundary() )
    {

      // check the outside boundaries of the (whole) mirror
      if ( !secSegment->intersects( tmpPos, tmpDir ) ) {
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
          //if ( produceHistos() )
          //  plot2D( planeIntersection.x(), planeIntersection.y(),
          //          "Sec Mirror missed out "+Rich::text(rich),
          //          -3000, 3000, -1000, 1000, 100, 100 );
          return false;
        }
      }

    }

    // Secondary mirror reflection with actual parameters
    if ( !reflectSpherical( tmpPos, tmpDir,
                            secSegment->centreOfCurvature(),
                            secSegment->radius() ) )
      return false;

    // set secondary ("flat") mirror data
    photon.setFlatMirReflectionPoint( tmpPos );
    photon.setFlatMirrorNum(secSegment->mirrorNumber());

  } // ignore secondary mirrors

  // Set final direction and position data
  position  = tmpPos;
  direction = tmpDir;

  return true;
}


//=========================================================================
// Raytraces from a point in the detector panel back to the spherical mirror
// returning the mirror intersection point and the direction a track would
// have in order to hit that point in the detector panel.
//=========================================================================
StatusCode
Rich::RayTracing::traceBackFromDetector ( const Gaudi::XYZPoint& startPoint,
                                          const Gaudi::XYZVector& startDir,
                                          Gaudi::XYZPoint& endPoint,
                                          Gaudi::XYZVector& endDir ) const
{
  Gaudi::XYZPoint  tmpStartPoint ( startPoint );
  Gaudi::XYZVector tmpStartDir   ( startDir   );

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
StatusCode
Rich::RayTracing::reflectSpherical ( Gaudi::XYZPoint& position,
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

  const Gaudi::XYZVector normal( (position - CoC).Unit() );
  // reflect the vector
  // r = u - 2(u.n)n, r=reflction, u=insident, n=normal
  direction -= 2.0 * (normal.Dot(direction)) * normal;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  reflect from a flat mirror
//=========================================================================
StatusCode
Rich::RayTracing::reflectFlatPlane ( Gaudi::XYZPoint& position,
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
StatusCode
Rich::RayTracing::intersectPlane ( const Gaudi::XYZPoint& position,
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
