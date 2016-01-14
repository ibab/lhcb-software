
//-----------------------------------------------------------------------------
/** @file RichRayTracingEigen.cpp
 *
 * Implementation file for class : RichRayTracingEigen
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date  2015-01-28
 */
//-----------------------------------------------------------------------------

// local
#include "RichRayTracingEigen.h"

/// Factory stuff
DECLARE_NAMESPACE_TOOL_FACTORY( Rich, RayTracingEigen )

namespace
{
  /// A z point that separates Rich1 from Rich2 (anything between 3000-9000mm)
  static const double s_RichDetSeparationPointZ = 8000.0;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::RayTracingEigen::RayTracingEigen( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent)
  : Rich::HistoToolBase       ( type, name, parent  ),
    m_rich                    ( Rich::NRiches, nullptr ),
    m_photoDetPanels          ( Rich::NRiches,
                                PDPanelsPerRich(Rich::NPDPanelsPerRICH,nullptr) ),
    m_sphMirrorSegRows        ( Rich::NRiches, 0    ),
    m_sphMirrorSegCols        ( Rich::NRiches, 0    ),
    m_secMirrorSegRows        ( Rich::NRiches, 0    ),
    m_secMirrorSegCols        ( Rich::NRiches, 0    ),
    m_deBeam                  ( Rich::NRiches, nullptr )
{
  // interface
  declareInterface<IRayTracing>(this);

  // default to having histograms disabled
  setProduceHistos ( false );

  // job options
  declareProperty( "IgnoreSecondaryMirrors", m_ignoreSecMirrs = false );

  // Debug messages
  // setProperty( "OutputLevel", 1 );
}

//=============================================================================
// Destructor
//=============================================================================
Rich::RayTracingEigen::~RayTracingEigen() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode Rich::RayTracingEigen::initialize()
{
  // intialise base class
  const StatusCode sc = Rich::HistoToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  acquireTool( "RichMirrorSegFinder", m_mirrorSegFinder, nullptr, true );

  // RICH detector elements
  m_rich[Rich::Rich1] = getDet<DeRich>( DeRichLocations::Rich1 );
  m_rich[Rich::Rich2] = getDet<DeRich>( DeRichLocations::Rich2 );

  // photo detector panels
  for ( unsigned int rich=0; rich<m_photoDetPanels.size(); ++rich )
  {
    for ( unsigned int panel=0; panel<m_photoDetPanels[rich].size(); ++panel )
    {
      const Rich::Side         side = (Rich::Side)panel;
      const Rich::DetectorType RICH = (Rich::DetectorType)rich;
      m_photoDetPanels[rich][panel] = m_rich[rich]->pdPanel(side);
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Stored for " << Rich::text(RICH,side)
                << " PD Panel " << m_photoDetPanels[rich][side]->name() << endmsg;
    }
  }

  // Rich1 mirrors
  if ( m_rich[Rich::Rich1]->exists("SphMirrorSegRows") )
  {
    m_sphMirrorSegRows[Rich::Rich1] = m_rich[Rich::Rich1]->param<int>( "SphMirrorSegRows"    );
    m_sphMirrorSegCols[Rich::Rich1] = m_rich[Rich::Rich1]->param<int>( "SphMirrorSegColumns" );
  }
  else
  {
    return Error ( "No primary mirrors for RICH1 found !" );
  }
  if ( m_rich[Rich::Rich1]->exists("SecMirrorSegRows") )
  {
    m_secMirrorSegRows[Rich::Rich1] = m_rich[Rich::Rich1]->param<int>( "SecMirrorSegRows"    );
    m_secMirrorSegCols[Rich::Rich1] = m_rich[Rich::Rich1]->param<int>( "SecMirrorSegColumns" );
  }
  else
  {
    return Error ( "No secondary mirrors for RICH1 found !" );
  }

  // Rich2 mirrors
  if ( m_rich[Rich::Rich2]->exists("SphMirrorSegRows") )
  {
    m_sphMirrorSegRows[Rich::Rich2] = m_rich[Rich::Rich2]->param<int>( "SphMirrorSegRows"    );
    m_sphMirrorSegCols[Rich::Rich2] = m_rich[Rich::Rich2]->param<int>( "SphMirrorSegColumns" );
  }
  else
  {
    return Error ( "No primary mirrors for RICH2 found !" );
  }
  if ( m_rich[Rich::Rich2]->exists("SecMirrorSegRows") )
  {
    m_secMirrorSegRows[Rich::Rich2] = m_rich[Rich::Rich2]->param<int>( "SecMirrorSegRows"    );
    m_secMirrorSegCols[Rich::Rich2] = m_rich[Rich::Rich2]->param<int>( "SecMirrorSegColumns" );
  }
  else
  {
    return Error ( "No secondary mirrors for RICH2 found !" );
  }

  if ( m_ignoreSecMirrs )
  {
    Warning( "Will ignore secondary mirrors", StatusCode::SUCCESS );
  }

  _ri_debug << "Using Eigen based Ray Tracing" << endmsg;

  return sc;
}

//=============================================================================
// reflect the trajectory on the mirror, and determine the position where
// it hits the detector plane,
// take into account the geometrical boundaries of mirrors and detector
//=============================================================================
LHCb::RichTraceMode::RayTraceResult
Rich::RayTracingEigen::traceToDetector ( const Rich::DetectorType rich,
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
Rich::RayTracingEigen::traceToDetector ( const Rich::DetectorType rich,
                                         const Gaudi::XYZPoint& startPoint,
                                         const Gaudi::XYZVector& startDir,
                                         Gaudi::XYZPoint& hitPosition,
                                         const LHCb::RichTrackSegment& trSeg,
                                         const LHCb::RichTraceMode mode,
                                         const Rich::Side forcedSide ) const
{
  // need to think if this can be done without creating a temp RichGeomPhoton ?
  LHCb::RichGeomPhoton photon;
  const LHCb::RichTraceMode::RayTraceResult sc =
    traceToDetector ( rich, startPoint, startDir, photon, trSeg, mode, forcedSide );
  hitPosition = photon.detectionPoint();
  return sc;
}

//=============================================================================
// reflect the trajectory on the mirror, and determine the position where
// it hits the detector plane,
// take into account the geometrical boundaries of mirrors and detector
//=============================================================================
LHCb::RichTraceMode::RayTraceResult
Rich::RayTracingEigen::traceToDetector ( const Rich::DetectorType rich,
                                         const Gaudi::XYZPoint& startPoint,
                                         const Gaudi::XYZVector& startDir,
                                         LHCb::RichGeomPhoton& photon,
                                         const LHCb::RichTraceMode mode,
                                         const Rich::Side forcedSide,
                                         const double photonEnergy ) const
{
  // temporary working objects
  Gaudi::XYZPoint  tmpPos ( startPoint );
  Gaudi::XYZVector tmpDir ( startDir   );

  // Correct start point/direction for aerogel refraction, if appropriate
  if ( mode.aeroRefraction() && Rich::Rich1 == rich )
  {
    snellsLaw()->aerogelToGas(tmpPos,tmpDir,photonEnergy);
  }

  // Do the ray tracing
  return _traceToDetector( rich, startPoint, tmpPos, tmpDir, photon, mode, forcedSide );
}

//=============================================================================
// reflect the trajectory on the mirror, and determine the position where
// it hits the detector plane,
// take into account the geometrical boundaries of mirrors and detector
//=============================================================================
LHCb::RichTraceMode::RayTraceResult
Rich::RayTracingEigen::traceToDetector ( const Rich::DetectorType rich,
                                         const Gaudi::XYZPoint& startPoint,
                                         const Gaudi::XYZVector& startDir,
                                         LHCb::RichGeomPhoton& photon,
                                         const LHCb::RichTrackSegment& trSeg,
                                         const LHCb::RichTraceMode mode,
                                         const Rich::Side forcedSide ) const
{
  // temporary working objects
  Gaudi::XYZPoint  tmpPos ( startPoint );
  Gaudi::XYZVector tmpDir ( startDir   );

  // Correct start point/direction for aerogel refraction, if appropriate
  if ( mode.aeroRefraction()            &&
       rich             == Rich::Rich1  &&
       trSeg.radiator() == Rich::Aerogel )
  {
    snellsLaw()->aerogelToGas(tmpPos,tmpDir,trSeg);
  }

  // Do the ray tracing
  return _traceToDetector( rich, startPoint, tmpPos, tmpDir, photon, mode, forcedSide );
}

LHCb::RichTraceMode::RayTraceResult
Rich::RayTracingEigen::_traceToDetector ( const Rich::DetectorType rich,
                                          const Gaudi::XYZPoint& startPoint,
                                          Gaudi::XYZPoint& tmpPos,
                                          Gaudi::XYZVector& tmpDir,
                                          LHCb::RichGeomPhoton& photon,
                                          const LHCb::RichTraceMode mode,
                                          const Rich::Side forcedSide ) const
{
  // default result is failure
  LHCb::RichTraceMode::RayTraceResult result = LHCb::RichTraceMode::RayTraceFailed;

  //if ( msgLevel(MSG::VERBOSE) )
  //  verbose() << "Ray Tracing : " << rich << " Ptn=" << startPoint
  //            << " Dir=" << tmpDir << endmsg;

  // first, try and reflect of both mirrors
  const bool sc = reflectBothMirrors( rich, tmpPos, tmpDir, photon,
                                      mode, forcedSide );

  //if ( msgLevel(MSG::VERBOSE) )
  //  verbose() << "  -> After reflectBothMirrors OK=" << sc << " : Ptn=" << tmpPos
  //            << " Dir=" << tmpDir << endmsg;

  if ( sc )
  {

    // for hit point use photon data directly
    Gaudi::XYZPoint & hitPosition = photon.detectionPoint();

    // the detector side
    const Rich::Side side = m_rich[rich]->side(tmpPos);

    // smart ID for RICH and panel (to be filled further when possible in following methods)
    LHCb::RichSmartID smartID ( rich, side, m_photoDetPanels[rich][side]->pdType() );

    // do ray tracing, depending on mode

    // are we configured to test individual HPD acceptance
    if ( mode.detPlaneBound() == LHCb::RichTraceMode::RespectHPDTubes )
    {
      // ... yes, then use method to test HPD acceptance (using mode)
      result = m_photoDetPanels[rich][side]->PDWindowPoint( tmpDir, tmpPos,
                                                            hitPosition, smartID, mode );
      //if ( msgLevel(MSG::VERBOSE) )
      //  verbose() << "  -> After PDWindowPoint " << hitPosition
      //            << " " << smartID << endmsg;

    }
    else
    {
      // ... no, so just trace to HPD panel
      // NOTE : smartID is not updated any more so will only contain RICH and panel data
      result = m_photoDetPanels[rich][side]->detPlanePoint( tmpPos, tmpDir,
                                                            hitPosition, smartID, mode );
      //if ( msgLevel(MSG::VERBOSE) )
      //  verbose() << "  -> After detPlanePoint " << hitPosition
      //            << " " << smartID << endmsg;
    }

    // Set remaining RichGeomPhoton data
    photon.setSmartID       ( smartID    );
    photon.setEmissionPoint ( startPoint );

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
bool Rich::RayTracingEigen::reflectBothMirrors( const Rich::DetectorType rich,
                                                Gaudi::XYZPoint& position,
                                                Gaudi::XYZVector& direction,
                                                LHCb::RichGeomPhoton& photon,
                                                const LHCb::RichTraceMode mode,
                                                const Rich::Side forcedSide ) const
{
  // which side are we on ?
  Rich::Side side = ( mode.forcedSide() ? forcedSide : m_rich[rich]->side(position) );

  EigenXYZPoint  tmpPos ( position  );
  EigenXYZVector tmpDir ( direction );

  // Spherical mirror reflection with nominal parameters
  if ( !reflectSpherical( tmpPos, tmpDir,
                          m_rich[rich]->nominalCentreOfCurvature(side),
                          m_rich[rich]->sphMirrorRadius() ) )
    return false;

  // if not forced, check if still same side, if not change sides
  if ( !mode.forcedSide() )
  {
    const Rich::Side tmpSide = m_rich[rich]->side( tmpPos );
    if ( side != tmpSide )
    {
      side   = tmpSide;
      tmpPos = position;
      tmpDir = direction;
      if ( !reflectSpherical( tmpPos, tmpDir,
                              m_rich[rich]->nominalCentreOfCurvature(side),
                              m_rich[rich]->sphMirrorRadius() ) ) { return false; }
    }
  }

  // find segment
  const DeRichSphMirror* sphSegment =
    m_mirrorSegFinder->findSphMirror( rich, side, tmpPos );

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
                          sphSegment->radius() ) ) { return false; }

  // set primary mirror data
  photon.setSphMirReflectionPoint( tmpPos );
  photon.setPrimaryMirror(sphSegment);

  // Are we ignoring the secondary mirrors ?
  if ( !m_ignoreSecMirrs )
  {

    EigenXYZPoint planeIntersection;
    // sec mirror reflection with nominal parameters
    if ( !intersectPlane( tmpPos,
                          tmpDir,
                          m_rich[rich]->nominalPlane(side),
                          planeIntersection) )
      return false;

    // find secondary mirror segment
    const DeRichSphMirror* secSegment = 
      m_mirrorSegFinder->findSecMirror(rich,side,planeIntersection);

    // depending on the tracing flag:
    if ( mode.mirrorSegBoundary() )
    {
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
      if ( !secSegment->intersects( tmpPos, tmpDir ) )
      {
        const RichMirrorSegPosition pos = m_rich[rich]->secMirrorSegPos( secSegment->mirrorNumber() );
        const Gaudi::XYZPoint& mirCentre = secSegment->mirrorCentre();
        bool fail( false );
        if ( pos.row() == 0 ) 
        { // bottom segment
          if ( planeIntersection.y() < mirCentre.y() ) fail = true;
        }
        if ( pos.row() == m_secMirrorSegRows[rich]-1 ) 
        { // top segment
          if ( planeIntersection.y() > mirCentre.y() ) fail = true;
        }
        if ( pos.column() == 0 ) 
        { // right side
          if ( planeIntersection.x() < mirCentre.x() ) fail = true;
        }
        if ( pos.column() == m_secMirrorSegCols[rich]-1 ) 
        { // left side
          if ( planeIntersection.x() > mirCentre.x() ) fail = true;
        }
        if ( fail )
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
    photon.setSecondaryMirror(secSegment);

  } // ignore secondary mirrors

  // Set final direction and position data
  position  = tmpPos;
  direction = tmpDir;

  return true;
}

//==========================================================================
// Raytraces from a point in the detector panel back to the spherical mirror
// returning the mirror intersection point and the direction a track would
// have in order to hit that point in the detector panel.
//==========================================================================
bool
Rich::RayTracingEigen::traceBackFromDetector ( const Gaudi::XYZPoint& startPoint,
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
    { return false; }

    // find secondary mirror segment
    const DeRichSphMirror* secSegment =
      m_mirrorSegFinder->findSecMirror( rich, side, planeIntersection );

    if ( !reflectSpherical( tmpStartPoint, tmpStartDir,
                            secSegment->centreOfCurvature(),
                            secSegment->radius() ) )
    { return false; }

  }

  // save points after first mirror reflection
  Gaudi::XYZPoint storePoint( tmpStartPoint );
  Gaudi::XYZVector storeDir( tmpStartDir );

  // Spherical mirror reflection with nominal parameters
  if ( !reflectSpherical( tmpStartPoint, tmpStartDir,
                          m_rich[rich]->nominalCentreOfCurvature(side),
                          m_rich[rich]->sphMirrorRadius() ) )
  { return false; }

  // find primary mirror segment
  const DeRichSphMirror* sphSegment = 
    m_mirrorSegFinder->findSphMirror( rich, side, tmpStartPoint );

  // Spherical mirror reflection with exact parameters
  if ( !reflectSpherical( storePoint, storeDir,
                          sphSegment->centreOfCurvature(),
                          sphSegment->radius() ) )
  { return false; }

  endPoint  = storePoint;
  endDir    = storeDir;

  return true;
}

//=========================================================================
//  reflect from a spherical mirror
//=========================================================================

bool
Rich::RayTracingEigen::reflectSpherical ( Gaudi::XYZPoint& position,
                                          Gaudi::XYZVector& direction,
                                          const Gaudi::XYZPoint& CoC,
                                          const double radius ) const
{
  // find intersection point
  // for line sphere intersection look at http://www.realtimerendering.com/int/

  const double a = direction.Mag2();
  if ( 0 == a )  
  {
    Warning( "reflectSpherical: Direction vector has zero length..." ).ignore();
    return false;
  }
  const Gaudi::XYZVector delta( position - CoC );
  const double b = 2.0 * direction.Dot( delta );
  const double c = delta.Mag2() - radius*radius;
  const double discr = b*b - 4.0*a*c;
  if ( discr < 0 ) return false;

  const double distance1 = 0.5 * ( std::sqrt(discr) - b ) / a;
  // change position to the intersection point
  position += distance1 * direction;

  // reflect the vector
  // r = u - 2(u.n)n, r=reflction, u=insident, n=normal

  const Gaudi::XYZVector normal( position - CoC );
  direction -= ( 2.0 * normal.Dot(direction) / normal.Mag2() ) * normal;

  return true;
}

//=========================================================================
//  reflect from a flat mirror
//=========================================================================
bool
Rich::RayTracingEigen::reflectFlatPlane ( Gaudi::XYZPoint& position,
                                          Gaudi::XYZVector& direction,
                                          const Gaudi::Plane3D& plane ) const
{
  // temp intersection point
  Gaudi::XYZPoint intersection;

  // refect of the plane
  const bool sc = intersectPlane( position, direction, plane, intersection );
  if ( sc ) 
  {
    // plane normal
    const Gaudi::XYZVector normal( plane.Normal() );
    
    // update position to intersection point
    position = intersection;
    
    // reflect the vector and update direction
    // r = u - 2(u.n)n, r=reflction, u=insident, n=normal
    direction -= 2.0 * normal.Dot(direction) * normal;
  }

  // return status code
  return sc;
}

//=========================================================================
//  intersect a plane
//=========================================================================

bool
Rich::RayTracingEigen::intersectPlane ( const Gaudi::XYZPoint& position,
                                        const Gaudi::XYZVector& direction,
                                        const Gaudi::Plane3D& plane,
                                        Gaudi::XYZPoint& intersection ) const
{
  bool OK = false;
  const double scalar = direction.Dot( plane.Normal() );
  if ( fabs(scalar) > 1e-99 )
  {
    const double distance = -(plane.Distance(position)) / scalar;
    intersection = position + (distance*direction);
    OK = true;
  }
  return OK;
}

//=========================================================================
