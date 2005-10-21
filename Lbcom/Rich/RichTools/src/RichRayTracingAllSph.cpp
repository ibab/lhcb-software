
//-----------------------------------------------------------------------------
/** @file RichRayTracingAllSph.cpp
 *
 * Implementation file for class : RichRayTracingAllSph
 *
 * CVS Log :-
 * $Id: RichRayTracingAllSph.cpp,v 1.3 2005-10-21 12:51:46 jonrob Exp $
 *
 * @author Antonis Papanestis
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2003-11-14
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/GaudiException.h"

// Kernel
#include "Kernel/RichSide.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// RichDet
#include "RichDet/DeRich.h"

// local
#include "RichRayTracingAllSph.h"

// Declaration of the Algorithm Factory
static const  ToolFactory<RichRayTracingAllSph>          Factory ;
const        IToolFactory& RichRayTracingAllSphFactory = Factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRayTracingAllSph::RichRayTracingAllSph( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent)
  : RichMoniToolBase     ( type, name, parent  ),
    m_rich               ( Rich::NRiches       ),
    m_sphMirrorSegRows   ( Rich::NRiches, 0    ),
    m_sphMirrorSegCols   ( Rich::NRiches, 0    ),
    m_secMirrorSegRows   ( Rich::NRiches, 0    ),
    m_secMirrorSegCols   ( Rich::NRiches, 0    ),
    m_RichDetSeparationPointZ ( 8000.0 )
{
  declareInterface<IRichRayTracing>(this);

  declareProperty( "Monitor", m_moni = false );
  declareProperty( "HistoPath", m_histPth = "RICH/TRACE_TEST/" );
}

//=============================================================================
// Destructor
//=============================================================================
RichRayTracingAllSph::~RichRayTracingAllSph() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode RichRayTracingAllSph::initialize()
{

  // intialise base class
  const StatusCode sc = RichMoniToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  acquireTool( "RichMirrorSegFinder", m_mirrorSegFinder );

  // HPD panel names
  const std::string pdPanelName[2][2]  = { { DeRichHPDPanelLocation::Rich1Panel0,
                                             DeRichHPDPanelLocation::Rich1Panel1 },
                                           { DeRichHPDPanelLocation::Rich2Panel0,
                                             DeRichHPDPanelLocation::Rich2Panel1 } };
  
  // RICH detector elements
  const DeRich * rich1 = getDet<DeRich>( DeRichLocation::Rich1 );
  const DeRich * rich2 = getDet<DeRich>( DeRichLocation::Rich2 );
  m_rich[Rich::Rich1] = rich1;
  m_rich[Rich::Rich2] = rich2;

  // loop over riches and photo detector panels
  unsigned int rich, panel;
  for ( rich=0; rich<m_photoDetPanels.size(); ++rich)
  {
    for ( panel=0; panel<m_photoDetPanels[rich].size(); ++panel) 
    {
      m_photoDetPanels[rich][panel] = getDet<DeRichHPDPanel>( pdPanelName[rich][panel] );
      debug() << "Stored photodetector panel "
              << m_photoDetPanels[rich][panel]->name() << endreq;
    }
  }

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
    info() << "Found spherical secondary mirrors for RICH1" << endreq;
  }
  else if ( rich1->exists("FlatMirrorSegRows") )
  {
    m_secMirrorSegRows[Rich::Rich1] = rich1->param<int>( "FlatMirrorSegRows"    );
    m_secMirrorSegCols[Rich::Rich1] = rich1->param<int>( "FlatMirrorSegColumns" );
    info() << "Found flat secondary mirrors for RICH1" << endreq;
  }
  else
  {
    return Error ( "No secondary mirrors for RICH1 found !" );
  }

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
    info() << "Found spherical secondary mirrors for RICH2" << endreq;
  }
  else if ( rich2->exists("FlatMirrorSegRows") )
  {
    m_secMirrorSegRows[Rich::Rich2] = rich2->param<int>( "FlatMirrorSegRows" );
    m_secMirrorSegCols[Rich::Rich2] = rich2->param<int>( "FlatMirrorSegColumns" );
    info() << "Found flat secondary mirrors for RICH2" << endreq;
  }
  else
  {
    return Error ( "No secondary mirrors for RICH2 found !" );
  }

  if ( m_moni ) bookHistos();

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichRayTracingAllSph::finalize()
{
  debug() << "Finalize" << endreq;
  //if ( 0 != m_HDS ) { m_HDS->release(); m_HDS=0; }

  // finalize base class
  return RichMoniToolBase::finalize();
}

//=============================================================================
// reflect the trajectory on the mirror, and determine the position where
// it hits the detector plane,
// take into account the geometrical boundaries of mirrors and detector
//=============================================================================
StatusCode RichRayTracingAllSph::traceToDetector ( const Rich::DetectorType rich,
                                                   const HepPoint3D& startPoint,
                                                   const HepVector3D& startDir,
                                                   RichGeomPhoton& photon,
                                                   const RichTraceMode mode,
                                                   const Rich::Side forcedSide )
  const {

  HepPoint3D tmpPosition( startPoint );
  HepVector3D tmpDirection( startDir );

  if ( reflectBothMirrors( rich, tmpPosition, tmpDirection, photon,
                           mode, forcedSide ).isFailure() )
    return StatusCode::FAILURE;

  HepPoint3D hitPosition;
  const Rich::Side side = m_rich[rich]->side(tmpPosition);
  RichSmartID smartID(rich, side, 0, 0, 0, 0);
  const StatusCode sc =
    m_photoDetPanels[rich][side]->PDWindowPoint(tmpDirection,tmpPosition,
                                                hitPosition, smartID, mode);
  // Set RichGeomPhoton
  photon.setSmartID( smartID );
  photon.setEmissionPoint( startPoint );
  photon.setDetectionPoint( hitPosition );

  return sc;
}

//============================================================================
StatusCode RichRayTracingAllSph::intersectPDPanel ( const Rich::DetectorType,
                                                    const HepPoint3D&,
                                                    const HepVector3D&,
                                                    RichGeomPhoton& ) const 
{
  return Warning ( "Unimplemented method", StatusCode::SUCCESS );
}

//=============================================================================
// reflect the trajectory on the mirror, and determine the position where
// it hits the detector plane,
// geometrical boundaries checking depends on the traceMode variable
//=============================================================================
StatusCode
RichRayTracingAllSph::traceToDetectorWithoutEff( const Rich::DetectorType rich,
                                                 const HepPoint3D& position,
                                                 const HepVector3D& direction,
                                                 HepPoint3D& hitPosition,
                                                 const RichTraceMode mode,
                                                 const Rich::Side forcedSide ) const
{

  HepPoint3D tmpPosition( position );
  HepVector3D tmpDirection( direction );

  RichGeomPhoton photon; // need to work out how to avoid creating this object for this case
  if ( reflectBothMirrors( rich, tmpPosition, tmpDirection, photon,
                           mode, forcedSide ).isFailure() )
    return StatusCode::FAILURE;

  const Rich::Side side = m_rich[rich]->side(tmpPosition);
  return m_photoDetPanels[rich][side]->detPlanePoint(tmpPosition, tmpDirection,
                                                     hitPosition, mode );
}


//=========================================================================
//  reflect a photon on both mirrors and return the position and direction
//  on the secondary mirror.
//=========================================================================
StatusCode RichRayTracingAllSph::reflectBothMirrors( const Rich::DetectorType rich,
                                                     HepPoint3D& position,
                                                     HepVector3D& direction,
                                                     RichGeomPhoton& photon,
                                                     const RichTraceMode mode,
                                                     const Rich::Side forcedSide ) const 
{



  HepPoint3D tmpPosition( position );
  HepVector3D tmpDirection( direction );

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
        return StatusCode::FAILURE;
    }
  }

  // find segment
  const DeRichSphMirror* sphSegment = m_mirrorSegFinder->findSphMirror( rich, side, tmpPosition);

  // depending on the tracing flag:
  if ( mode.mirrorSegBoundary() ) 
  {
    // if reflection from a mirror segment is required
    if ( !sphSegment->intersects( position, direction ) ) {
      if (m_moni) m_sphMirMissedGap[rich]->fill(tmpPosition.x(), tmpPosition.y());
      return StatusCode::FAILURE;
    }

  }
  else if (  mode.outMirrorBoundary() ) 
  {
    // check the outside boundaries of the (whole) mirror
    if ( !sphSegment->intersects( position, direction ) ) 
    {
      const RichMirrorSegPosition pos = m_rich[rich]->sphMirrorSegPos( sphSegment->mirrorNumber() );
      const HepPoint3D & mirCentre = sphSegment->mirrorCentre();
      bool fail( false );
      if ( pos.row() == 0 ) {                 // bottom segment
        if ( tmpPosition.y() < mirCentre.y() )
          fail = true;
      }
      if ( pos.row() == m_sphMirrorSegRows[rich]-1 ) { // top segment
        if ( tmpPosition.y() > mirCentre.y() )
          fail = true;
      }
      if ( pos.column() == 0 ) {                 // right side
        if ( tmpPosition.x() < mirCentre.x() )
          fail = true;
      }
      if ( pos.column() == m_sphMirrorSegCols[rich]-1 ) {   // left side
        if ( tmpPosition.x() > mirCentre.x() )
          fail = true;
      }
      if (fail) {
        if (m_moni) m_sphMirMissedOut[rich]->fill(tmpPosition.x(), tmpPosition.y());
        return StatusCode::FAILURE;
      }
    }
  }

  // reset position, direction before trying again
  tmpPosition = position;
  tmpDirection = direction;

  // Spherical mirror reflection with exact parameters
  if ( !reflectSpherical( tmpPosition, tmpDirection,
                          sphSegment->centreOfCurvature(),
                          sphSegment->radius() ) )
    return StatusCode::FAILURE;

  photon.setSphMirReflectionPoint( tmpPosition );
  photon.setSphMirrorNum(sphSegment->mirrorNumber());

  // store the position after spherical mirror reflection
  //  HepPoint3D storePosition( tmpPosition );
  //  HepVector3D storeDirection ( tmpDirection );

  HepPoint3D planeIntersection;
  // sec mirror reflection with nominal parameters
  if ( !intersectPlane( tmpPosition, 
                        tmpDirection,
                        m_rich[rich]->nominalPlane(side),
                        planeIntersection) )
    return StatusCode::FAILURE;

  // find segment
  const DeRichSphMirror* secSegment = m_mirrorSegFinder->findSecMirror(rich,side,planeIntersection);

  // depending on the tracing flag:
  if ( mode.mirrorSegBoundary() ) {
    // if reflection from a mirror segment is required
    if ( !secSegment->intersects( tmpPosition, tmpDirection ) ) {
      if (m_moni) m_secMirMissedGap[rich]->fill(planeIntersection.x(), planeIntersection.y());
      return StatusCode::FAILURE;
    }

  } else if ( mode.outMirrorBoundary() ) {

    // check the outside boundaries of the (whole) mirror
    if ( !secSegment->intersects( tmpPosition, tmpDirection ) ) {
      const RichMirrorSegPosition pos = m_rich[rich]->flatMirrorSegPos( secSegment->mirrorNumber() );
      const HepPoint3D& mirCentre = secSegment->mirrorCentre();
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
      if (fail) {
        if (m_moni) m_secMirMissedOut[rich]->fill(planeIntersection.x(), planeIntersection.y());
        return StatusCode::FAILURE;
      }
    }

  }

  //  tmpPosition = storePosition;
  //  tmpDirection = storeDirection;
  // Secondary mirror reflection with actual parameters
  if ( !reflectSpherical( tmpPosition, tmpDirection,
                          secSegment->centreOfCurvature(),
                          secSegment->radius() ) )
    return StatusCode::FAILURE;

  photon.setFlatMirReflectionPoint( tmpPosition );
  photon.setFlatMirrorNum(secSegment->mirrorNumber());
  position = tmpPosition;
  direction = tmpDirection;

  return StatusCode::SUCCESS;
}


//=========================================================================
// Raytraces from a point in the detector panel back to the spherical mirror
// returning the mirror intersection point and the direction a track would
// have in order to hit that point in the detector panel.
//=========================================================================
StatusCode
RichRayTracingAllSph::traceBackFromDetector ( const HepPoint3D& startPoint,
                                              const HepVector3D& startDir,
                                              HepPoint3D& endPoint,
                                              HepVector3D& endDir ) const
{

  HepPoint3D tmpStartPoint( startPoint );
  HepVector3D tmpStartDir( startDir );

  const Rich::DetectorType rich = ( startPoint.z()/mm < m_RichDetSeparationPointZ ?
                                    Rich::Rich1 : Rich::Rich2 );
  const Rich::Side side = m_rich[rich]->side(startPoint);

  HepPoint3D planeIntersection;
  // sec mirror reflection with nominal parameters
  if ( !intersectPlane( tmpStartPoint,
                        tmpStartDir,
                        m_rich[rich]->nominalPlane(side),
                        planeIntersection ) )
    return StatusCode::FAILURE;

  // find segment
  const DeRichSphMirror* secSegment = m_mirrorSegFinder->
    findSecMirror(rich,side,planeIntersection);

  if ( !reflectSpherical( tmpStartPoint, tmpStartDir,
                          secSegment->centreOfCurvature(),
                          secSegment->radius() ) )
    return StatusCode::FAILURE;

  // save points after first mirror reflection
  HepPoint3D storePoint( tmpStartPoint );
  HepVector3D storeDir( tmpStartDir );

  // Spherical mirror reflection with nominal parameters
  if ( !reflectSpherical( tmpStartPoint, tmpStartDir,
                          m_rich[rich]->nominalCentreOfCurvature(side),
                          m_rich[rich]->sphMirrorRadius() ) )
  {
    return StatusCode::FAILURE;
  }

  // find segment
  const DeRichSphMirror* sphSegment = m_mirrorSegFinder->
    findSphMirror( rich, side,tmpStartPoint );

  // Spherical mirror reflection with exact parameters
  if ( !reflectSpherical( storePoint, storeDir,
                          sphSegment->centreOfCurvature(),
                          sphSegment->radius() ) )
    return StatusCode::FAILURE;

  endPoint = storePoint;
  endDir = storeDir;

  return StatusCode::SUCCESS;

}

//=========================================================================
//  reflect from a spherical mirror
//=========================================================================
StatusCode RichRayTracingAllSph::reflectSpherical ( HepPoint3D& position,
                                                    HepVector3D& direction,
                                                    const HepPoint3D& CoC,
                                                    const double radius ) const
{
  // find intersection point
  // for line sphere intersection look at:http://www.realtimerendering.com/int/
  const HepVector3D delta( position - CoC );

  const double a = direction.mag2();
  const double b = 2. * direction * delta;
  const double c = delta.mag2() - radius*radius;
  const double discr = b*b - 4.*a*c;
  if ( discr < 0 ) return StatusCode::FAILURE;

  const double distance1 = 0.5 * ( sqrt(discr) - b ) / a;
  // change position to the intersection point
  position += distance1 * direction;

  const HepNormal3D normal( (position - CoC).unit() );
  // reflect the vector
  // r = u - 2(u.n)n, r=reflction, u=insident, n=normal
  direction -= 2.0 * (normal*direction) * normal;

  return StatusCode::SUCCESS;
}


//=========================================================================
//  reflect from a flat mirror
//=========================================================================
StatusCode RichRayTracingAllSph::reflectFlatPlane ( HepPoint3D& position,
                                                    HepVector3D& direction,
                                                    const HepPlane3D& plane ) const
{
  HepPoint3D intersection;
  const HepVector3D normal( plane.normal() );

  if ( intersectPlane( position, direction, plane, intersection ).isFailure() )
    return StatusCode::FAILURE;

  position = intersection;
  // reflect the vector
  // r = u - 2(u.n)n, r=reflction, u=insident, n=normal
  direction -= 2.0 * (normal*direction) * normal;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  intersect a plane
//=========================================================================
StatusCode RichRayTracingAllSph::intersectPlane ( const HepPoint3D& position,
                                                  const HepVector3D& direction,
                                                  const HepPlane3D& plane,
                                                  HepPoint3D& intersection ) const
{
  const double scalar = direction * plane.normal();
  if ( scalar == 0.0 ) return StatusCode::FAILURE;

  const double distance = -(plane.distance(position)) / scalar;
  intersection = position + distance*direction;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Book Histograms
//=========================================================================

StatusCode RichRayTracingAllSph::bookHistos() {

  debug() << "Booking Histos" << endmsg;

  m_sphMirMissedOut[0] = histoSvc()->
    book(m_histPth, 101, "Spherical Mirror missed out R1", 100, -700,
         700, 100, -800, 800);
  m_sphMirMissedOut[1] = histoSvc()->
    book(m_histPth, 201, "Spherical Mirror missed out R2", 100, -1800,
         1800, 100, -1500, 1500);

  m_secMirMissedOut[0] = histoSvc()->
    book(m_histPth, 102, "Sec Mirror missed out R1", 100, -700,
         700, 100, -1000, 1000);
  m_secMirMissedOut[1] = histoSvc()->
    book(m_histPth, 202, "Sec Mirror missed out R2", 100, -3000,
         3000, 100, -1000, 1000);

  m_sphMirMissedGap[0] = histoSvc()->
    book(m_histPth, 103, "Spherical Mirror missed gap R1", 100, -700,
         700, 100, -800, 800);
  m_sphMirMissedGap[1] = histoSvc()->
    book(m_histPth, 203, "Spherical Mirror missed gap R2", 100, -1800,
         1800, 100, -1500, 1500);

  m_secMirMissedGap[0] = histoSvc()->
    book(m_histPth, 104, "Sec Mirror missed gap R1", 100, -700,
         700, 100, -1000, 1000);
  m_secMirMissedGap[1] = histoSvc()->
    book(m_histPth, 204, "Sec Mirror missed gap R2", 100, -3000,
         3000, 100, -1000, 1000);

  return StatusCode::FAILURE;
}

//=========================================================================
