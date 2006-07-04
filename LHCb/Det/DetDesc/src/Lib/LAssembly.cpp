// $Id: LAssembly.cpp,v 1.12 2006-07-04 09:27:57 jpalac Exp $

// Include files
// DetDesc
#include "DetDesc/LAssembly.h"
#include "DetDesc/SolidBase.h"
#include "DetDesc/SolidBox.h"
// ============================================================================
/** @file LAssembly.cpp
 *
 *  Implementation file for class : LAssembly
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author Sebastien Ponce
 */
// ============================================================================

// ============================================================================
/** constructor
 *  @exception LVolumeException wrong paramaters value
 *  @param name name of logical volume 
 *  @param sensitivity  name of sensitive detector object (for simulation)
 *  @param magnetic  name of magnetic field object (for simulation)
 */
// ============================================================================
LAssembly::LAssembly
( const std::string& name        , 
  ISolid*            Solid       ,
  const std::string& sensitivity ,
  const std::string& magnetic    )
  : LogVolBase( name        , 
                sensitivity , 
                magnetic    ),
    m_solid(Solid),
    m_coverComputed( false )
{
  makeCoverBox();
};

// ============================================================================
/** default constructor
 */
// ============================================================================
LAssembly::LAssembly()
  : LogVolBase()
{};

// ============================================================================
/** destructor 
 */
// ============================================================================
LAssembly::~LAssembly() {
  /// reset 
  if( 0 != m_solid ) { delete m_solid ; m_solid = 0 ; }
};

// ============================================================================
/** class/object identifier (static method)
 *  @return unique class identifier 
 */
// ============================================================================
const CLID& LAssembly::classID ()           { return CLID_LAssembly        ; }

// ============================================================================
/** class/object identification (virtual method)
 *  @return unique class identifier 
 */      
// ============================================================================
const CLID& LAssembly::clID    ()     const { return LAssembly::classID()  ; }


// ============================================================================
/** calculate the daughter path containing the Point in Local frame , 
 *  can be VERY slow for complex geometry, 
 *  therefore use the appropriate Level for usage 
 *  @see ILVolume 
 *  @param  localPoint point in local reference system of logical volume 
 *  @param  level depth level
 *  @param  volumePath  vector of physical volumes
 *  @return status code 
 */
// ============================================================================
StatusCode LAssembly::belongsTo
( const Gaudi::XYZPoint&        LocalPoint  ,
  const int                Level       , 
  ILVolume::PVolumePath&   pVolumePath ) const 
{    
  /// check the depth 
  if( 0 == Level ) { return StatusCode::SUCCESS; } 
  /// check the point 
  if( !isInside( LocalPoint ) )         
    { pVolumePath.clear() ; return StatusCode::FAILURE; } 
  ILVolume::PVolumes::const_iterator ppi = 
    insideDaughter( LocalPoint ) ;
  if( pvEnd() == ppi ) { return StatusCode::FAILURE; } 
  /// look for daughters 
  const IPVolume* pv = *ppi;
  if( 0 == pv || 0 == pv->lvolume() ) 
    { pVolumePath.clear() ; return StatusCode::FAILURE; } 
  /// add volume to the path 
  pVolumePath.push_back( pv ) ;
  /// recursion 
  return pv->lvolume()->belongsTo( pv->matrix() * LocalPoint , 
                                   Level - 1                 , 
                                   pVolumePath               );
};

// ============================================================================
/** calculate the daughter path containing the Point in Local frame , 
 *  can be VERY slow for complex geometry, 
 *  therefore use the appropriate Level for usage 
 *  @see ILVolume 
 *  @param  localPoint point in local reference system of logical volume 
 *  @param  level depth level
 *  @param  volumePath  vector of physical volumes
 *  @return status code 
 */
// ============================================================================
StatusCode LAssembly::belongsTo
( const Gaudi::XYZPoint&        LocalPoint  ,
  const int                Level       , 
  ILVolume::ReplicaPath&   replicaPath ) const  
{    
  /// check the depth 
  if( 0 == Level     ) { return StatusCode::SUCCESS; } 
  /// check if point is inside cover or a daughter
  if( !isInside( LocalPoint ) )
    { replicaPath.clear() ; return StatusCode::FAILURE; } 
  /// find daughter in which point is
  ILVolume::PVolumes::const_iterator ppi = 
    insideDaughter( LocalPoint ) ;
  if( pvEnd() == ppi ) { return StatusCode::FAILURE; } 
  /// look for daughters 
  const IPVolume* pv = *ppi;
  if( 0 == pv || 0 == pv->lvolume() ) 
    { replicaPath.clear() ; return StatusCode::FAILURE; } 
  /// get replica number 
  ILVolume::ReplicaType replica = ppi - pvBegin();
  /// add volume to the path 
  replicaPath.push_back( replica ) ;
  /// recursion 
  return pv->lvolume()->belongsTo( pv->matrix() * LocalPoint , 
                                   Level - 1                 , 
                                   replicaPath               );
};

// ============================================================================
/** intersection of the logical volume with with the line \n
 *  The line is parametrized in the local reference system 
 *  of the logical volume by initial Point and direction Vector \n 
 *  @f$ \vec{x}(t) = \vec{p} + t\times \vec{v} @f$  \n 
 * 
 *   Method returns the number of intersection points("ticks") and 
 *   the container of pairs - ticks and pointer to the corresponding 
 *   material. The simplification is determined by value of threshold
 *   (in units of radiation length) 
 *   Method throws LVolumeException in the case, then 
 *   solid is not defined or material is not accessible.
 *
 *  @see ILVolume 
 *  @exception LogVolumeException solid or/and matherial problems 
 *  @param Point initial point at the line
 *  @param Vector direction vector of the line
 *  @param intersections output container 
 *  @param threshold threshold value 
 *  @return number of intersections  
 */
// ============================================================================
unsigned int LAssembly::intersectLine
( const Gaudi::XYZPoint        & Point         , 
  const Gaudi::XYZVector       & Vector        , 
  ILVolume::Intersections & intersections , 
  const double              threshold     ) const 
{
  /// clear the container 
  intersections.clear();
  /// line with null direction vector is not able to intersect any volume 
  if( Vector.mag2() <= 0 ) { return 0 ; }       // RETURN !!!
  /// intersections with childrens 
  intersectDaughters
    ( Point     , Vector    , intersections  , threshold );
  ///
  return intersections.size();  ///< RETURN!!!
};

// ============================================================================
/** intersection of the logical volume with with the line \n
 *  Theine is parametrized in the local reference system 
 *  of the logical volume by initial Point and direction Vector \n 
 *  @f$ \vec{x}(t) = \vec{p} + t\times \vec{v} @f$  \n 
 * 
 *   Method returns the number of intersection points("ticks") and 
 *   the container of pairs - ticks and pointer to the corresponding 
 *   material. The simplification is determined by value of threshold
 *   (in units of radiation length) 
 *   Method throws LVolumeException in the case, then 
 *   solid is not defined or material is not accessible.
 *
 *  @see ILVolume 
 *  @exception LogVolumeException solid or/and matherial problems 
 *  @param Point initial point at the line
 *  @param Vector direction vector of the line
 *  @param intersections output container
 *  @param tickMin minimum value of possible Tick
 *  @param tickMax maximum value of possible Tick
 *  @param threshold threshold value 
 *  @return number of intersections  
 */
// ============================================================================
unsigned int LAssembly::intersectLine
( const Gaudi::XYZPoint         & Point         , 
  const Gaudi::XYZVector        & Vector        , 
  ILVolume::Intersections  & intersections , 
  const ISolid::Tick         tickMin       , 
  const ISolid::Tick         tickMax       , 
  const double               Threshold     ) const
{
  /// clear the output container 
  intersections.clear();
  /// check the valid tick values 
  if( tickMin >= tickMax ) { return 0 ;} 
  /** line with null direction vector 
   * is not able to intersect any volume
   */
  if( Vector.mag2() <= 0 ) { return 0 ; }       // RETURN !!!

  
  //== Check the 'cover'

// FOR SOME REASON THIS SLOWS EVERYTHING DOWN! CHECK WHAT'S GOING ON!
//   if (m_solid) { // this should now always be true!
//     ISolid::Ticks ticks;
//     m_solid->intersectionTicks( Point , Vector , tickMin , tickMax , ticks ) ;
//     if ( 2 > ticks.size()         ) { return 0 ; }  // RETURN !!!
//   } else {

    Gaudi::XYZPoint p1 = Point + tickMin * Vector;
    Gaudi::XYZPoint p2 = Point + tickMax * Vector;

    if ( (zMin() > p1.z()) && (zMin() > p2.z()) ) return 0 ;
    if ( (zMax() < p1.z()) && (zMax() < p2.z()) ) return 0 ;
    if ( (xMin() > p1.x()) && (xMin() > p2.x()) ) return 0 ;
    if ( (xMax() < p1.x()) && (xMax() < p2.x()) ) return 0 ;
    if ( (yMin() > p1.y()) && (yMin() > p2.y()) ) return 0 ;
    if ( (yMax() < p1.y()) && (yMax() < p2.y()) ) return 0 ;
//   }
  
  /** look for the intersections of the given 
   *  line with daughter elements construct the 
   *  intersections container for daughter volumes
   */
  intersectDaughters
    ( Point , Vector , intersections , tickMin , tickMax , Threshold  );

  return intersections.size();    ///< RETURN!!!
};

// ============================================================================
/** printout to STD/STL stream
 *  @see ILVolume 
 *  @param os STD/STL stream
 *  @return reference to the stream
 */
// ============================================================================
std::ostream& LAssembly::printOut 
( std::ostream & os             ) const 
{ return LogVolBase::printOut( os ) ; }


// ============================================================================
/** printout to Gaudi MsgStream stream
 *  @see ILVolume 
 *  @param os Gaudi MsgStream  stream
 *  @return reference to the stream
 */
// ============================================================================
MsgStream&    LAssembly::printOut
( MsgStream    & os             ) const
{ return LogVolBase::printOut( os ) ; }
//=========================================================================
void LAssembly::makeCoverBox() 
{
  if (0==m_solid) {
    computeCoverBoxParams ();
    makeCoverBoxSolid() ;
  } else {
    ///@to-do Check if all solids have xMin(), xMax(), etc implemented.
    SolidBase* pSolid = dynamic_cast<SolidBase*>(m_solid);
    if (pSolid) {
      m_xMin = pSolid->xMin();
      m_xMax = pSolid->xMax();
      m_yMin = pSolid->yMin();
      m_yMax = pSolid->yMax();
      m_zMin = pSolid->zMin();
      m_zMax = pSolid->zMax();
    } else {
      MsgStream log ( msgSvc() , "LAssembly" );
      log << MSG::ERROR << "Could not cast ISolid* to SolidBase*. Calculating rough box parameters" << endreq;
      computeCoverBoxParams ();
    }
    
    
  } 
}
//=========================================================================
void LAssembly::makeCoverBoxSolid()
{
  m_solid = new SolidBox(name()+"-CoverBox", 
                         std::fabs(xMax() - xMin() )/2.0,
                         std::fabs(yMax() - yMin() )/2.0,
                         std::fabs(zMax() - zMin() )/2.0  );
                           
}
//=========================================================================
//  
//=========================================================================
void LAssembly::computeCoverBoxParams() {

  if ( coverBoxComputed() ) return;

  MsgStream log ( msgSvc() , "TransportSvc" );

  m_xMin = 1000000.;
  m_yMin = 1000000.;
  m_zMin = 1000000.;
  m_xMax = -1000000.;
  m_yMax = -1000000.;
  m_zMax = -1000000.;

  double pointX, pointY, pointZ = 0.;
  Gaudi::XYZPoint motherPt( 0., 0., 0. );
  int i, j, k;
  
  for ( ILVolume::PVolumes::const_iterator ipv = pvBegin(); 
        pvEnd() != ipv; ipv++ ) {
    IPVolume* pv = *ipv;
    if ( 0 != pv ) {
      const ISolid* mySolid = pv->lvolume()->solid();
      if ( 0 != mySolid ) {  //== Solid => has a cover
        const ISolid* iCover = mySolid->cover();
        const SolidBase* cover = dynamic_cast<const SolidBase*>( iCover );
        if ( 0 != cover ) {
          //== Compute the 8 corners, transform to mother frame and build the 
          //== envelop as a box (x,y,z Min/Max)
          pointX=cover->xMin();
          for ( i = 0 ; 2 > i ; i++ ) {
            pointY=cover->yMin();
            for ( j = 0 ; 2 > j ; j++ ) {
              pointZ=cover->zMin();
              for ( k = 0 ; 2 > k ; k++ ) {
                motherPt = 
                  pv->toMother( Gaudi::XYZPoint(pointX,pointY,pointZ ) );
                if ( m_xMin > motherPt.x() ) m_xMin = motherPt.x();
                if ( m_xMax < motherPt.x() ) m_xMax = motherPt.x();
                if ( m_yMin > motherPt.y() ) m_yMin = motherPt.y();
                if ( m_yMax < motherPt.y() ) m_yMax = motherPt.y();
                if ( m_zMin > motherPt.z() ) m_zMin = motherPt.z();
                if ( m_zMax < motherPt.z() ) m_zMax = motherPt.z();
                pointZ=cover->zMax();
              }
              pointY = cover->yMax();
            }
            pointX = cover->xMax();
          }
        } else {
          log << MSG::ERROR << " === No cover for assembly " << name() 
              << " pv " << pv->name() << endreq;;
        }
      } else {  //== No solid : This is an assembly
        const LAssembly* assem = dynamic_cast<const LAssembly*>(pv->lvolume());
        if ( 0 == assem ) {
          log << MSG::ERROR << " === No solid for assembly " << name() 
              << " pv " << pv->name() << " not assembly !" << endreq;
        } else {
          //== Cover of the assembly is computed at construction.
          //== Compute the 8 corners, transform to mother frame and build the 
          //== envelop as a box (x,y,z Min/Max)
          pointX = assem->xMin();
          for ( i = 0 ; 2 > i ; i++ ) {
            pointY = assem->yMin();
            for ( j = 0 ; 2 > j ; j++ ) {
              pointZ = assem->zMin();
              for ( k = 0 ; 2 > k ; k++ ) {
                motherPt = 
                  pv->toMother( Gaudi::XYZPoint(pointX,pointY,pointZ ) );
                if ( m_xMin > motherPt.x() ) m_xMin = motherPt.x();
                if ( m_xMax < motherPt.x() ) m_xMax = motherPt.x();
                if ( m_yMin > motherPt.y() ) m_yMin = motherPt.y();
                if ( m_yMax < motherPt.y() ) m_yMax = motherPt.y();
                if ( m_zMin > motherPt.z() ) m_zMin = motherPt.z();
                if ( m_zMax < motherPt.z() ) m_zMax = motherPt.z();
                pointZ =  assem->zMax();
              }
              pointY = assem->yMax();
            }
            pointX = assem->xMax();
          }
        }
      }  
    }
  }
  log << MSG::VERBOSE << "Assembly " << name() 
      << " x [" << m_xMin << "," << m_xMax
      << "], y [" << m_yMin << "," << m_yMax
      << "], z [" << m_zMin << "," << m_zMax
      << "]" << endreq;
  m_coverComputed = true;
}
// ============================================================================
// The End 
// ============================================================================
