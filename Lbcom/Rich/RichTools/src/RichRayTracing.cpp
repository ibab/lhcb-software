
//-----------------------------------------------------------------------------
/** @file RichRayTracing.cpp
 *
 * Implementation file for class : RichRayTracing
 *
 * CVS Log :-
 * $Id: RichRayTracing.cpp,v 1.10 2004-07-26 18:03:05 jonrob Exp $
 * $Log: not supported by cvs2svn $
 *
 * @author Antonis Papanestis
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
#include "RichRayTracing.h"

// Declaration of the Algorithm Factory
static const  ToolFactory<RichRayTracing>          Factory ;
const        IToolFactory& RichRayTracingFactory = Factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRayTracing::RichRayTracing( const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : RichToolBase ( type, name, parent ), m_HDS( 0 )
{
  declareInterface<IRichRayTracing>(this);

  declareProperty( "Monitor", m_moni = false );
  declareProperty( "HistoPath", m_histPth = "RICH/TRACE_TEST/" );
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

  if ( !RichToolBase::initialize() ) return StatusCode::FAILURE;

  const std::string pdPanelName[2][2]  = { { DeRichHPDPanelLocation::Rich1Panel0,
                                             DeRichHPDPanelLocation::Rich1Panel1 },
                                           { DeRichHPDPanelLocation::Rich2Panel0,
                                             DeRichHPDPanelLocation::Rich2Panel1 } };

  DeRich* rich1 = getDet<DeRich>( DeRichLocation::Rich1 );
  DeRich* rich2 = getDet<DeRich>( DeRichLocation::Rich2 );

  //loop over riches and photo detector panels
  unsigned int rich, panel;
  for( rich=0; rich<m_photoDetPanels.size(); ++rich){
    for( panel=0; panel<m_photoDetPanels[rich].size(); ++panel) {
      m_photoDetPanels[rich][panel] = getDet<DeRichHPDPanel>( pdPanelName[rich][panel] );
      debug() << "Stored photodetector panel "
              << m_photoDetPanels[rich][panel]->name() << endreq;
    }
  }

  // load the nominal centre of curvature and flat mirror plane
  m_nominalCoC[Rich::Rich1][Rich::top] = rich1->
    nominalCentreOfCurvature(Rich::top);
  m_nominalCoC[Rich::Rich1][Rich::bottom] = rich1->
    nominalCentreOfCurvature(Rich::bottom);

  m_nominalCoC[Rich::Rich2][Rich::left] = rich2->
    nominalCentreOfCurvature(Rich::left);
  m_nominalCoC[Rich::Rich2][Rich::right] = rich2->
    nominalCentreOfCurvature(Rich::right);

  m_nominalFlatMirrorPlane[Rich::Rich1][Rich::top] = rich1->
    nominalPlane(Rich::top);
  m_nominalFlatMirrorPlane[Rich::Rich1][Rich::bottom]= rich1->
    nominalPlane(Rich::bottom);

  m_nominalFlatMirrorPlane[Rich::Rich2][Rich::left] = rich2->
    nominalPlane(Rich::left);
  m_nominalFlatMirrorPlane[Rich::Rich2][Rich::right] = rich2->
    nominalPlane(Rich::right);

  m_nomSphMirrorRadius[Rich::Rich1] = rich1->sphMirrorRadius();
  m_nomSphMirrorRadius[Rich::Rich2] = rich2->sphMirrorRadius();

  m_rich[Rich::Rich1] = rich1;
  m_rich[Rich::Rich2] = rich2;

  if ( rich1->hasParam("SphMirrorSegRows") ) {
    m_sphMirrorSegRows[Rich::Rich1] = rich1->paramAsInt( "SphMirrorSegRows" );
    m_sphMirrorSegCols[Rich::Rich1] = rich1->paramAsInt( "SphMirrorSegColumns" );
    m_flatMirrorSegRows[Rich::Rich1] = rich1->paramAsInt( "FlatMirrorSegRows" );
    m_flatMirrorSegCols[Rich::Rich1] = rich1->paramAsInt( "FlatMirrorSegColumns" );
  }
  
  if ( rich2->hasParam("SphMirrorSegRows") ) {
    m_sphMirrorSegRows[Rich::Rich2] = rich2->paramAsInt( "SphMirrorSegRows" );
    m_sphMirrorSegCols[Rich::Rich2] = rich2->paramAsInt( "SphMirrorSegColumns" );
    m_flatMirrorSegRows[Rich::Rich2] = rich2->paramAsInt( "FlatMirrorSegRows" );
    m_flatMirrorSegCols[Rich::Rich2] = rich2->paramAsInt( "FlatMirrorSegColumns" );
  }
  

  acquireTool( "RichMirrorSegFinder", m_mirrorSegFinder );

  if ( m_moni ) bookHistos();

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichRayTracing::finalize()
{
  debug() << "Finalize" << endreq;
  if ( 0 != m_HDS ) { m_HDS->release(); m_HDS=0; }

  // finalize base class
  return RichToolBase::finalize();
}


//=============================================================================
// reflect the trajectory on the mirror, and determine the position where
// it hits the detector plane,
// take into account the geometrical boundaries of mirrors and detector
//=============================================================================
StatusCode RichRayTracing::traceToDetector ( const Rich::DetectorType rich,
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
StatusCode RichRayTracing::intersectPDPanel ( const Rich::DetectorType,
                                              const HepPoint3D&,
                                              const HepVector3D&,
                                              RichGeomPhoton& ) const {
  return StatusCode::SUCCESS;
}

//=============================================================================
// reflect the trajectory on the mirror, and determine the position where
// it hits the detector plane,
// geometrical boundaries checking depends on the traceMode variable
//=============================================================================
StatusCode
RichRayTracing::traceToDetectorWithoutEff( const Rich::DetectorType rich,
                                           const HepPoint3D& position,
                                           const HepVector3D& direction,
                                           HepPoint3D& hitPosition,
                                           const RichTraceMode mode,
                                           const Rich::Side forcedSide ) const {

  HepPoint3D tmpPosition( position );
  HepVector3D tmpDirection( direction );

  RichGeomPhoton photon;

  if ( reflectBothMirrors( rich, tmpPosition, tmpDirection, photon,
                           mode, forcedSide ).isFailure() )
    return StatusCode::FAILURE;

  const Rich::Side side = m_rich[rich]->side(tmpPosition);
  const StatusCode sc =
    m_photoDetPanels[rich][side]->detPlanePoint(tmpPosition, tmpDirection,
                                                hitPosition, mode );
  return sc;
}


//=========================================================================
//  reflect a photon on both mirrors and return the position and direction
//  on the flat mirror.
//=========================================================================
StatusCode RichRayTracing::reflectBothMirrors( const Rich::DetectorType rich,
                                               HepPoint3D& position,
                                               HepVector3D& direction,
                                               RichGeomPhoton& photon,
                                               const RichTraceMode mode,
                                               const Rich::Side forcedSide ) const {

  HepPoint3D tmpPosition( position );
  HepVector3D tmpDirection( direction );

  Rich::Side side;

  if ( !mode.forcedSide() )
    side = m_rich[rich]->side(tmpPosition);
  else
    side = forcedSide;

  // Spherical mirror reflection with nominal parameters
  if ( !reflectSpherical( tmpPosition, tmpDirection,
                          m_nominalCoC[rich][side],
                          m_nomSphMirrorRadius[rich]) )
    return StatusCode::FAILURE;

  // if not forced, check if still same side, if not change sides
  if ( !mode.forcedSide() ) {
    const Rich::Side tmpSide = m_rich[rich]->side(tmpPosition);
    if ( side != tmpSide ) {
      side = tmpSide;
      tmpPosition = position;
      tmpDirection = direction;
      if ( !reflectSpherical( tmpPosition, tmpDirection,
                              m_nominalCoC[rich][side],
                              m_nomSphMirrorRadius[rich]) )
        return StatusCode::FAILURE;
    }
  }

  // find segment
  DeRichSphMirror* sphSegment = m_mirrorSegFinder->
    findSphMirror( rich, side, tmpPosition);

  // depending on the tracing flag:
  if ( mode.mirrorSegBoundary() ) {
    // if reflection from a mirror segment is required
    if ( !sphSegment->intersects( position, direction ) ) {
      if (m_moni) m_sphMirMissedGap[rich]->fill(tmpPosition.x(), tmpPosition.y());
      return StatusCode::FAILURE;
    }
    
  } else if (  mode.outMirrorBoundary() ) {
    // check the outside boundaries of the (whole) mirror
    if ( !sphSegment->intersects( position, direction ) ) {
      RichMirrorSegPosition pos = m_rich[rich]->sphMirrorSegPos( sphSegment->mirrorNumber() );
      HepPoint3D mirCentre( sphSegment->mirrorCentre() );
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
  HepPoint3D storePosition( tmpPosition );
  HepVector3D storeDirection ( tmpDirection );

  // flat mirror reflection with nominal parameters
  if ( !reflectFlat( tmpPosition, tmpDirection,
                     m_nominalFlatMirrorPlane[rich][side]) )
    return StatusCode::FAILURE;

  // find segment
  DeRichFlatMirror* flatSegment = m_mirrorSegFinder->
    findFlatMirror(rich,side,tmpPosition);

  // depending on the tracing flag:
  if ( mode.mirrorSegBoundary() ) {
    // if reflection from a mirror segment is required
    if ( !flatSegment->intersects( storePosition, storeDirection ) ) {
      if (m_moni) m_flatMirMissedGap[rich]->fill(tmpPosition.x(), tmpPosition.y());
      return StatusCode::FAILURE;
    }
    
  } else if (  mode.outMirrorBoundary() ) {
    // check the outside boundaries of the (whole) mirror
    if ( !flatSegment->intersects( storePosition, storeDirection ) ) {
      RichMirrorSegPosition pos = m_rich[rich]->flatMirrorSegPos( flatSegment->mirrorNumber() );
      HepPoint3D mirCentre( flatSegment->mirrorCentre() );
      bool fail( false );
      if ( pos.row() == 0 ) {                 // bottom segment
        if ( tmpPosition.y() < mirCentre.y() )
          fail = true;
      }
      if ( pos.row() == m_flatMirrorSegRows[rich]-1 ) { // top segment
        if ( tmpPosition.y() > mirCentre.y() )
          fail = true;
      }
      if ( pos.column() == 0 ) {                 // right side
        if ( tmpPosition.x() < mirCentre.x() ) 
          fail = true;
      }
      if ( pos.column() == m_flatMirrorSegCols[rich]-1 ) {   // left side
        if ( tmpPosition.x() > mirCentre.x() ) 
          fail = true;
      }
      if (fail) {
        if (m_moni) m_flatMirMissedOut[rich]->fill(tmpPosition.x(), tmpPosition.y());
        return StatusCode::FAILURE;
      }  
    }
    
  }
  

  tmpPosition = storePosition;
  tmpDirection = storeDirection;
  // flat mirror reflection with actual parameters
  if ( !reflectFlat( tmpPosition, tmpDirection,
                     flatSegment->mirrorPlane()) )
    return StatusCode::FAILURE;

  photon.setFlatMirReflectionPoint( tmpPosition );
  photon.setFlatMirrorNum(flatSegment->mirrorNumber());
  position = tmpPosition;
  direction = tmpDirection;

  return StatusCode::SUCCESS;

}


//=========================================================================
//  reflect from a spherical mirror
//=========================================================================
StatusCode RichRayTracing::reflectSpherical ( HepPoint3D& position,
                                              HepVector3D& direction,
                                              const HepPoint3D& CoC,
                                              const double radius ) const
{
  // find intersection point
  // for line sphere intersection look at:http://www.realtimerendering.com/int/
  HepVector3D delta( position - CoC );

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
StatusCode RichRayTracing::reflectFlat ( HepPoint3D& position,
                                         HepVector3D& direction,
                                         const HepPlane3D& plane ) const
{
  HepPoint3D intersection;
  HepVector3D normal( plane.normal() );

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
StatusCode RichRayTracing::intersectPlane (const HepPoint3D& position,
                                           const HepVector3D& direction,
                                           const HepPlane3D& plane,
                                           HepPoint3D& intersection ) const
{
  const HepPoint3D normal( plane.normal() );

  const double scalar = direction*normal;
  if ( scalar == 0.0 ) return StatusCode::FAILURE;

  const double distance = -(plane.distance(position)) / scalar;
  intersection = position + distance*direction;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Book Histograms
//=========================================================================

StatusCode RichRayTracing::bookHistos() {

  debug() << "Booking Histos" << endmsg;
    
  m_sphMirMissedOut[0] = histoSvc()->
    book(m_histPth, 101, "Spherical Mirror missed out R1", 100, -700,
         700, 100, -800, 800);
  m_sphMirMissedOut[1] = histoSvc()->
    book(m_histPth, 201, "Spherical Mirror missed out R2", 100, -1800,
         1800, 100, -1500, 1500);

  m_flatMirMissedOut[0] = histoSvc()->
    book(m_histPth, 102, "Flat Mirror missed out R1", 100, -700,
         700, 100, -1000, 1000);
  m_flatMirMissedOut[1] = histoSvc()->
    book(m_histPth, 202, "Flat Mirror missed out R2", 100, -3000,
         3000, 100, -1000, 1000);

  m_sphMirMissedGap[0] = histoSvc()->
    book(m_histPth, 103, "Spherical Mirror missed gap R1", 100, -700,
         700, 100, -800, 800);
  m_sphMirMissedGap[1] = histoSvc()->
    book(m_histPth, 203, "Spherical Mirror missed gap R2", 100, -1800,
         1800, 100, -1500, 1500);

  m_flatMirMissedGap[0] = histoSvc()->
    book(m_histPth, 104, "Flat Mirror missed gap R1", 100, -700,
         700, 100, -1000, 1000);
  m_flatMirMissedGap[1] = histoSvc()->
    book(m_histPth, 204, "Flat Mirror missed gap R2", 100, -3000,
         3000, 100, -1000, 1000);
  
  return StatusCode::FAILURE;
}


//=========================================================================
//  get histogram service
//=========================================================================
IHistogramSvc* RichRayTracing::histoSvc() const {
  if ( 0 == m_HDS ) {
    StatusCode sc = service( "HistogramDataSvc", m_HDS, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [HistogramDataSvc] not found", name(),sc);
    }
  }
  return m_HDS;
}
//=========================================================================
