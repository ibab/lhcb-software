// $Id: LVolume.cpp,v 1.21 2002-05-15 14:56:29 cattanem Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.20  2002/05/15 14:51:56  cattanem
// fix for windows
//
// Revision 1.19  2002/05/15 14:25:25  ibelyaev
//  bug fix
//
// Revision 1.18  2002/05/11 18:25:47  ibelyaev
//  see $DETDESCROOT/doc/release.notes 11 May 2002
//
// Revision 1.17  2002/05/04 13:13:24  ibelyaev
//  see $DETDESCROOT/doc/release.notes ( 4 May 2002 )
//
// ===========================================================================
/// STD & STL includes 
#include <iostream> 
#include <string> 
#include <functional> 
#include <algorithm> 
/// CLHEP includes 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Transform3D.h"
/// Gaudi Kernel includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/ISvcLocator.h" 
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/StreamBuffer.h" 
#include "GaudiKernel/SmartRefVector.h" 
/// DetDesc includes 
#include "DetDesc/ISolid.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/LVolume.h"
#include "DetDesc/VolumeIntersectionIntervals.h"
#include "DetDesc/SolidTicks.h"
#include "DetDesc/Material.h"
#include "DetDesc/Solid.h"
#include "DetDesc/Surface.h"
#include "DetDesc/ClhepToStream.h"
#include "DetDesc/DetDesc.h"

// ===========================================================================
/** @file LVolume.cpp 
 *  
 *  implementation of class LVolume 
 *
 *  @author: Vanya Belyaev  Ivan.Belyaev@itep.ru 
 */
// ===========================================================================

// ===========================================================================
/** constructor, pointer to ISolid* must be valid!, 
 *  overvise constructor throws LVolumeException!  
 *  @exception LVolumeException wrong paramaters value
 *  @param name         name of logical volume 
 *  @param Solid        pointer to ISolid object 
 *  @param material     name of the material 
 *  @param validity     validity object
 *  @param sensitivity  name of sensitive detector object (for simulation)
 *  @param magnetic     name of magnetic field object (for simulation)
 */
// ===========================================================================
LVolume::LVolume
( const std::string& name        , 
  ISolid*            Solid       ,
  const std::string& material    ,
  const IValidity&   validity    , 
  const std::string& sensitivity ,
  const std::string& magnetic    )
  : LogVolBase     ( name        , 
                     validity    , 
                     sensitivity , 
                     magnetic    )
  , m_solid        ( Solid       )
  , m_materialName ( material    )
  , m_material     (    0        )
{
  if( 0 == m_solid ) 
    { throw LogVolumeException("LVolume: ISolid* points to NULL ") ; }
}; 

// ===========================================================================
/** constructor, pointer to ISolid* must be valid!, 
 *  overvise constructor throws LVolumeException!  
 *  @exception LVolumeException wrong paramaters value
 *  @param name         name of  logical volume 
 *  @param Solid        pointer to ISolid object 
 *  @param material     name of the material 
 *  @param validSince   begin of validity range 
 *  @param validTill    end of validity range 
 *  @param sensitivity  name of sensitive detector object (for simulation)
 *  @param magnetic     name of magnetic field object (for simulation)
 */
// =========================================================================== 
LVolume::LVolume
( const std::string& name         , 
  ISolid*            Solid        ,
  const std::string& material     ,
  const ITime&       validSince   , 
  const ITime&       validTill    , 
  const std::string& sensitivity  ,
  const std::string& magnetic     )
  : LogVolBase     ( name        , 
                     validSince  , 
                     validTill   , 
                     sensitivity ,      
                     magnetic    )
  , m_solid        ( Solid       )
  , m_materialName ( material    )
  , m_material     (    0        )
{
  if( 0 == m_solid ) 
    { throw LogVolumeException("LVolume: ISolid* points to NULL ") ; }
}; 

// =========================================================================== 
/** constructor, pointer to ISolid* must be valid!, 
 *  overvise constructor throws LVolumeException!  
 *  @exception LVolumeException wrong paramaters value
 *  @param name         name of logical volume 
 *  @param Solid        pointer to ISolid object 
 *  @param material     name of the material 
 *  @param sensitivity  name of sensitive detector object (for simulation)
 *  @param magnetic     name of magnetic field object (for simulation)
 */
// =========================================================================== 
LVolume::LVolume
( const std::string& name        , 
  ISolid*            Solid       ,
  const std::string& material    ,
  const std::string& sensitivity ,
  const std::string& magnetic    )
  : LogVolBase     ( name        , 
                     sensitivity , 
                     magnetic    )
  , m_solid        ( Solid       )
  , m_materialName ( material    )
  , m_material     (    0        )
{
  if( 0 == m_solid ) 
    { throw LogVolumeException("LVolume: ISolid* points to NULL ") ; }
}; 

// =========================================================================== 
/** (default) constructor
 *  @param name         name of logical volume 
 */
// =========================================================================== 
LVolume::LVolume( const std::string& name )
  : LogVolBase     ( name       )
  , m_solid        ( 0          )
  , m_materialName ( ""         )
  , m_material     ( 0          )
{};

// ===========================================================================
/// destructor 
// ===========================================================================
LVolume::~LVolume()
{
  /// reset 
  reset();
  if( 0 != m_solid ) { delete m_solid ; m_solid = 0 ; }
};

const CLID& LVolume::clID   () const { return classID()    ; } 
const CLID& LVolume::classID()       { return CLID_LVolume ; };

// ============================================================================
/** calculate the daughter path containing the Point in Local frame , 
 *  can be VERY slow for complex geometry, 
 *  therefore use the appropriate Level for usage 
 *  @param  LocalPoint point in local reference system of logical volume 
 *  @param  level depth level
 *  @param  volumePath  vector of physical volumes
 *  @return status code 
 */
// ============================================================================
StatusCode LVolume::belongsTo
( const HepPoint3D&        LocalPoint  ,
  const int                Level       , 
  ILVolume::PVolumePath&   pVolumePath ) const 
{    
  /// check the depth 
  if( 0 == Level ) { return StatusCode::SUCCESS; } 
  /// check the point 
  if( !isInside( LocalPoint ) )         
    { pVolumePath.clear() ; return StatusCode::FAILURE; } 
  /// look for daughters 
  ILVolume::PVolumes::const_iterator ppi =
    insideDaughter( LocalPoint );
  if( pvEnd() == ppi ) { return StatusCode::SUCCESS; }
  /// check for the volume 
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
 *  @param  LocalPoint point in local reference system of logical volume 
 *  @param  level depth level
 *  @param  volumePath  vector of physical volumes
 *  @return status code 
 */
// ============================================================================
StatusCode LVolume::belongsTo
( const HepPoint3D&        LocalPoint ,
  const int                Level      , 
  ILVolume::ReplicaPath&   replicaPath ) const 
{
  /// check for level 
  if( 0 == Level ) { return StatusCode::SUCCESS; } 
  /// check for points  
  if( !isInside( LocalPoint ) )
    { replicaPath.clear() ; return StatusCode::FAILURE; } 
  /// look fopr daughters 
  ILVolume::PVolumes::const_iterator ppi = 
    insideDaughter( LocalPoint );
  /// the last level?  
  if( pvEnd() == ppi ) { return StatusCode::SUCCESS; } 
  /// check for the volume 
  const IPVolume* pv = *ppi;
  if( 0 == pv || 0 == pv->lvolume() ) 
    { replicaPath.clear() ; return StatusCode::FAILURE; } 
  /// get the replica number 
  ILVolume::ReplicaType replica = ppi - pvBegin();
  /// add replica to the path 
  replicaPath.push_back( replica ) ;
  /// recursion 
  return pv->lvolume()->belongsTo( pv->matrix() * LocalPoint , 
                                   Level - 1                 , 
                                   replicaPath               );
};  

// ============================================================================
/** locate material
 *  @exception LVolumeException
 *  @return pointer to material
 */
// ============================================================================
const Material* LVolume::findMaterial() const 
{
  const Material* mat = 0; 
  SmartDataPtr<const Material> material( dataSvc() , materialName() );
  if( 0 != material) { mat = material ; }
  Assert( 0 != mat , "Could not locate material=" + materialName() ); 
  m_material = mat;
  return m_material; 
};

// ============================================================================
/** serialization for reading 
 *  - implementation of DataObject method
 *  - implementation of ISerialize interface
 *  @see DataObject
 *  @see ILVolume 
 *  @see ISerialize 
 *  @param s reference to stream buffer 
 *  @return reference to stream buffer 
 */ 
// ============================================================================
StreamBuffer& LVolume::serialize(StreamBuffer& s )
{
  /// reset the logical volume  
  reset();
  /// Serialize the base class  
  LogVolBase::serialize( s ) ;
  /// solid 
  if( 0 != m_solid ) { delete m_solid ; m_solid = 0 ; }
  std::string solidType;
  s >> solidType ;
  m_solid = Solid::createSolid( solidType );
  if( 0 == solid() )
    { throw LogVolumeException( "Could not instantiate SolidType='"
                                + solidType + "'" ); }
  /// serialize the solid 
  m_solid->serialize( s ) ;
  /// material 
  return s >> m_materialName ;
};

// ============================================================================
/** serialization for writing 
 *  - implementation of DataObject method
 *  - implementation of ISerialize interface
 *  @see DataObject
 *  @see ILVolume 
 *  @see ISerialize 
 *  @param s reference to stream buffer 
 *  @return reference to stream buffer 
 */ 
// ============================================================================
StreamBuffer& LVolume::serialize( StreamBuffer& s )  const
{
  /// Serialize the base class  
  LogVolBase::serialize( s ) ;
  /// Serialize the members 
  /// solid 
  s << m_solid->typeName() ;
  m_solid->serialize( s ) ;
  /// material 
  return s << m_materialName ;
};

// ============================================================================
/** printout to STD/STL stream
 *  @see ILVolume 
 *  @param os STD/STL stream
 *  @return reference to the stream
 */
// ============================================================================
std::ostream& LVolume::printOut 
( std::ostream & os             ) const 
{
  ///  printout the base 
  LogVolBase::printOut( os ) ;
  /// 
  os << solid() 
     << std::endl ;
  ///
  return os << "Material name='" << materialName() << "' " << std::endl ;
};

// ============================================================================
/** printout to Gaudi MsgStream stream
 *  @see ILVolume 
 *  @param os Gaudi MsgStream  stream
 *  @return reference to the stream
 */
// ============================================================================
MsgStream&    LVolume::printOut
( MsgStream    & os             ) const
{
  ///  printout the base 
  LogVolBase::printOut( os ) ;
  /// 
  os << solid() 
     << endreq;
  ///
  return os << "Material name='" << materialName() << "' " << endreq;
};

// ============================================================================
/** Auxillary method  to calculate own intersections
 *  @exception LogVolumeException for wrong parameters or geometry errors
 *  @param  Point         initial point at the line
 *  @param  Vector        direction vector of the line
 *  @param  intersections output container
 *  @param  threshold     threshold value 
 *  @return number of intersections  
 */
// ============================================================================
unsigned int LVolume::intersectBody
( const HepPoint3D         & Point         , 
  const HepVector3D        & Vector        , 
  ILVolume::Intersections  & intersections , 
  const double               Threshold     ) const 
{
  /// avoid long names 
  typedef std::vector<ILVolume::Interval>    Intervals; 
  using namespace VolumeIntersectionIntervals;
  /// (1) clear the output container 
  intersections.clear();
  /// length of tick "unit"
  const ISolid::Tick TickLength = Vector.mag() ;  
  /// assertion for material and solid 
  if( 0 == material() || 0 == m_solid ) 
    { Assert( false , "LVolume::intersectBody(1): FATAL for" + name() ); }
  /// get cover solid 
  const ISolid* pSolid = m_solid->coverTop()  ; 
  ///
  ISolid::Ticks           ticks               ;   
  Intervals               intervals           ; 
  bool                    useThisVolume = true;
  /**  If threshold is given, before performing the accurate calculations,
   *   perform the rought estimation using the coverTop() method!  
   *   Line MUST intersect the coverTop() solid at least in 2 points!
   */
  if( Threshold > 0 || m_solid == pSolid ) 
    { 
      /// number of intersections
      const unsigned n1 = 
        pSolid->intersectionTicks( Point , Vector , ticks ) ;
      if( n1 < 2 ) { ticks.clear() ; return 0 ; }           ///< RETURN !!!
      /** estimate the radiation thickness of the coverTop volume 
       *  and  construct intervals
       */
      intervals.clear();
      TicksToIntervals( ticks , std::back_inserter( intervals ) );
      /// the total length of intervals (in tick units)
      double Length = 
        std::accumulate( intervals.begin() , intervals.end       () , 
                         0.0               , AccumulateIntervals () );
      /** this volume is not to be used for estimations 
       *  of radiation thickness 
       */
      if( Length * TickLength <= Threshold * m_material->radiationLength() ) 
        {
          useThisVolume = false ;   
          ticks.clear     (); 
          intervals.clear (); 
        }
    }
  /** if this volume is to be used,  
   *  find intersections with REAL solid (not cover) 
   */
  if( useThisVolume && m_solid != pSolid ) 
    {
      /** no intersections with own solid 
       *  ( at least 2 points are required! )
       *  for solid() == solid->coverTop() we have 
       *  already calculated ticks!
       */
      const unsigned n1 = 
        m_solid->intersectionTicks( Point , Vector , ticks ) ;
      if( n1 < 2 )     { ticks.clear() ; return 0; }         ///< RETURN !!! 
      /** estimate more carefuly its contribution to the total 
       *  radiation thickness and construct intervals!
       */
      intervals.clear();
      TicksToIntervals( ticks , std::back_inserter( intervals ) );  
      /// the total length of intervals in Tick units
      double Length = 
        std::accumulate( intervals.begin() , intervals.end()   ,
                         0.0               , AccumulateIntervals() ); 
      /** this volume is not to be used for estimations 
       *  of radiation thickness
       */
      if( Threshold > 0 && 
          Length * TickLength < Threshold * m_material->radiationLength() ) 
        { 
          ticks.clear()         ; 
          intervals.clear()     ;
          useThisVolume = false ; 
        }      
    }
  /// transform container of its own intervals into own intersection container 
  for( Intervals::const_iterator it = intervals.begin() ; 
       intervals.end() != it ; ++it ) 
    { intersections.push_back( ILVolume::Intersection( *it , m_material ) ) ; }
  ///  
  return intersections.size(); 
};

// ============================================================================
/** Auxillary method  to calculate own intersections
 *  @exception LogVolumeException for wrong parameters or geometry errors
 *  @param  Point         initial point at the line
 *  @param  Vector        direction vector of the line
 *  @param  intersections output container
 *  @param  threshold     threshold value 
 *  @return number of intersections  
 */
// ============================================================================
unsigned int LVolume::intersectBody
( const HepPoint3D         & Point         ,
  const HepVector3D        & Vector        ,
  ILVolume::Intersections  & intersections ,
  ISolid::Tick             & tickMin       ,
  ISolid::Tick             & tickMax       ,
  const double               Threshold     ) const 
{
  // useful type definition 
  typedef std::vector<ILVolume::Interval>    Intervals;
  using namespace VolumeIntersectionIntervals;
  // clear the output container 
  intersections.clear();
  // check the valid tick values 
  if( tickMin >= tickMax ) { return 0 ;} 
  /** line with null direction vector 
   * is not able to intersect any volume
   */ 
  if( Vector.mag2() <= 0 ) { return 0 ; }       // RETURN !!! 
  // length of tick unit 
  const ISolid::Tick TickLength = Vector.mag(); 
  // assertion for material and solid 
  if( 0 == material() || 0 == m_solid ) 
    { Assert( false , "LVolume::intersectBody(1): FATAL for" + name() ); }
  // define "top"
  const ISolid* pSolid = m_solid->coverTop() ; 
  //  contribution of this volume is estimated in 3 steps:
  bool useThisVolume      = true  ; 
  /**  (1) estimate the maximal possible constribution 
   *   of this volume using tickMin and tickMax 
   */
  if( Threshold > 0  &&  
      ( tickMax - tickMin )  * TickLength  
      <= Threshold * m_material->radiationLength() ) 
    { useThisVolume = false; }                                   /// NB!!!! 
  ISolid::Ticks           ticks;   
  /** If threshold is given, before performing the accurate 
   *  calculations, perform the rought estimation 
   *  using the coverTop() method!  
   *  Line MUST intersect the coverTop() solid at 
   *  least in 2 points! common points with cover 
   *  TopSolid within range from tickMin to tickMax 
   */
  if( useThisVolume || m_solid == pSolid ) 
    {
      if( pSolid->intersectionTicks( Point   ,  
                                     Vector  , 
                                     tickMin , 
                                     tickMax , 
                                     ticks   ) < 2 ) { return 0; } 
      /// redefine tickMin and tickMax   
      tickMin = *std::min_element( ticks.begin() , ticks.end() ); 
      tickMax = *std::max_element( ticks.begin() , ticks.end() ); 
    }  
  
  ///
  Intervals               intervals; 
  if( ( useThisVolume && Threshold > 0 ) 
      || m_solid == pSolid ) 
    { 
      /** estimate the radiation thickness of the coverTop 
       *  volume and construct intervals
       */ 
      intervals.clear(); 
      TicksToIntervals( ticks , std::back_inserter( intervals ) );
      /// the total length of intervals in Tick units 
      double Length = 
        std::accumulate( intervals.begin() , intervals.end       () ,
                         0.0               , AccumulateIntervals () );
      /// this volume is not to be used for estimations of radiation thickness 
      if( Length * TickLength 
          <= Threshold * m_material->radiationLength() ) 
        { 
          useThisVolume = false ;   
          intervals.clear(); 
          ticks.clear(); 
        }
    }
  /**  if this volume is to be used,  
   *   find intersections with REAL solid (not cover) 
   *   no intersections with own solid 
   *   ( at least 2 points are required! )
   */
  if( useThisVolume && m_solid != pSolid ) 
    {
      /** no intersections with own solid 
       *   ( at least 2 points are required! )
       *  (sor solidUnion we have already calculated ticks!
       */
      if( 2 > solid()->intersectionTicks( Point   , 
                                          Vector  , 
                                          tickMin , 
                                          tickMax , 
                                          ticks   ) ) { return 0; }
      /**  check for tickMin and tickMax 
       *  remove extra ticks from container
       * refine tickMin and tickMax
       */
      tickMin = *std::min_element( ticks.begin() , ticks.end() ); 
      tickMax = *std::max_element( ticks.begin() , ticks.end() ); 
      /** estimate more carefuly its contribution to the total 
       *  radiation thickness and construct intervals!
       */
      intervals.clear(); 
      TicksToIntervals( ticks , std::back_inserter( intervals ) );  
      /// the total length of intervals in Tick units 
      double Length = 
        std::accumulate( intervals.begin() , intervals.end       () ,
                         0.0               , AccumulateIntervals () );
      /// this volume is not to be used for estimations of radiation thickness 
      if( Length * TickLength 
          <= Threshold * m_material->radiationLength() ) 
        { 
          ticks.clear()         ; 
          intervals.clear()     ;
          useThisVolume = false ; 
        }  
    }
  /** transform container of its own intervals 
   *  into own intersection container
   */ 
  for( Intervals::const_iterator it = intervals.begin() ; 
       intervals.end() != it ; ++it ) 
    { 
      intersections.push_back( ILVolume::Intersection( *it , m_material ) );
    }
  ///
  return intersections.size(); 
  ///
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
 *  @exception LVolumeException solid or/and matherial problems 
 *  @param Point         initial point at the line
 *  @param Vector        direction vector of the line
 *  @param intersections output container 
 *  @param Threshold     threshold value 
 *  @return number of intersections  
 */
// ============================================================================
unsigned int LVolume::intersectLine
( const HepPoint3D        & Point         , 
  const HepVector3D       & Vector        , 
  ILVolume::Intersections & intersections , 
  const double              Threshold     ) const 
{
  // avoid long names 
  typedef std::vector<ILVolume::Interval>    Intervals; 
  using namespace VolumeIntersectionIntervals ;
  // clear the container 
  intersections.clear();
  // line with null direction vector is not able to intersect any volume 
  if( Vector.mag2() <= 0 ) { return 0 ; }       // RETURN !!!

  // intersections with own "volume body"
  ILVolume::Intersections own;
  intersectBody
    ( Point     , Vector    , own       , Threshold ); 
  // intersections with childrens 
  ILVolume::Intersections childrens; 
  intersectDaughters
    ( Point     , Vector    , childrens , Threshold );
  // here we'd like to fill the output container:
  if( own.empty() )    // the parent container is empty
    {
      // we have only child container just copy it to the output 
      std::copy( childrens.begin() , childrens.end  () , 
                 std::back_inserter( intersections ) ) ;
      return intersections.size();  ///< RETURN!!!
    }
  else // own container is NOT empty!
    {
      // perform quite non trivial merging 
      StatusCode sc = 
        MergeOwnAndChildContainers
        ( own , childrens , std::back_inserter( intersections ) );
      // check the result!!!
      if( sc.isFailure() )
        {
          MsgStream log( DetDesc::msgSvc() , "DetectorDataSvc" );
          log << MSG::ERROR
              << "===== Failure to merge " << name()
              << " type " << m_solid->typeName()
              << " x " << Point.x()
              << " y " << Point.y()
              << " z " << Point.z()
              << endreq ;
          
          for ( ILVolume::Intersections::const_iterator itI = own.begin();
                own.end() != itI; itI++ ) {
            double s = (*itI).first.first;
            double t = (*itI).first.second;
            log << MSG::INFO
                << "Own : s " << s
                << " x " << Point.x() + s * Vector.x()
                << " y " << Point.y() + s * Vector.y()
                << " z " << Point.z() + s * Vector.z()
                << " to t " << t
                << " x " << Point.x() + t * Vector.x()
                << " y " << Point.y() + t * Vector.y()
                << " z " << Point.z() + t * Vector.z()
                << " radl " << (*itI).second->radiationLength()
                << endreq ;
          }
          for ( ILVolume::Intersections::const_iterator itJ = childrens.begin();
                childrens.end() != itJ; itJ++ ) {
            double s = (*itJ).first.first;
            double t = (*itJ).first.second;
            log << MSG::INFO 
                << "Child : s " << s
                << " x " << Point.x() + s * Vector.x()
                << " y " << Point.y() + s * Vector.y()
                << " z " << Point.z() + s * Vector.z()
                << " to t " << t
                << " x " << Point.x() + t * Vector.x()
                << " y " << Point.y() + t * Vector.y()
                << " z " << Point.z() + t * Vector.z()
                << " radl " << (*itJ).second->radiationLength()
                << endreq;
          }
          
          intersections.clear();          
          std::copy( childrens.begin () , 
                     childrens.end   () ,
                     std::back_inserter( intersections ) ) ;
          
          return intersections.size();                          // RETURN!!!
          // temporary
          // Assert( false , "LVolumeIntersectLine FATAL! " + name() , sc ); 
        }
    }
  ///  
  return intersections.size(); 
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
 *  @exception LVolumeException solid or/and matherial problems 
 *  @param Point initial point at the line
 *  @param Vector direction vector of the line
 *  @param intersections output container
 *  @param tickMin minimum value of possible Tick
 *  @param tickMax maximum value of possible Tick
 *  @param threshold threshold value 
 *  @return number of intersections  
 */
// ============================================================================
unsigned int LVolume::intersectLine
( const HepPoint3D         & Point         , 
  const HepVector3D        & Vector        , 
  ILVolume::Intersections  & intersections , 
  const ISolid::Tick         tickMin       , 
  const ISolid::Tick         tickMax       , 
  const double               Threshold     ) const 
{
  ISolid::Tick TickMin = tickMin ;
  ISolid::Tick TickMax = tickMax ;  
  // avoid long names 
  typedef std::vector<ILVolume::Interval>    Intervals;
  using namespace VolumeIntersectionIntervals;
  // clear the output container 
  intersections.clear();
  // check the valid tick values 
  if( tickMin >= tickMax ) { return 0 ;}            // RETURN !!! 
  /** line with null direction vector 
   * is not able to intersect any volume
   */ 
  if( Vector.mag2() <= 0 ) { return 0 ; }           // RETURN !!!
  // check the region TickMin tickMax 
  { // the following lines are based on O.Callot's code  
    // Check that the intersections are not all on the same side of the segment
    const ISolid* pSolid = m_solid->coverTop() ;
    ISolid::Ticks ticks;
    pSolid->intersectionTicks( Point , Vector , TickMin , TickMax , ticks ) ;
    // pSolid->intersectionTicks( Point , Vector , ticks ) ;
    if ( 2 > ticks.size()         ) { return 0 ; }  // RETURN !!!
    // redefine TickMin and TicMax
    // NB: container of "ticks" is always sorted!
    if( TickMax < ticks.front () ) { return 0 ; }
    if( TickMin > ticks.back  () ) { return 0 ; }
    TickMin = ticks.front () ;
    TickMax = ticks.back  () ;
    if( TickMin >= TickMax       ) { return 0 ; }  // RETURN !!!
  }
  // own intersections
  ILVolume::Intersections own;
  intersectBody 
    ( Point , Vector , own       , TickMin , TickMax , Threshold ); 
  /** look for the intersections of the given 
   *  line with daughter elements construct the 
   *  intersections container for daughter volumes
   */
  ILVolume::Intersections childrens; 
  intersectDaughters
    ( Point , Vector , childrens , TickMin , TickMax , Threshold  );
  // here we'd like to fill the output container:
  if( own.empty() )    // the parent container is empty
    {
      // we have only child container - just copy it to the output 
      std::copy( childrens.begin () , childrens.end  () , 
                 std::back_inserter( intersections ) ) ;
      return intersections.size();    ///< RETURN!!!
    }
  else   // own container is NOT empty!
    {
      // perform quite non trivial merging 
      StatusCode sc = 
        MergeOwnAndChildContainers
        ( own , childrens , std::back_inserter( intersections ) );
      // check the result !
      if( sc.isFailure() )
        {
          MsgStream log( DetDesc::msgSvc() , "DetectorDataSvc" );
          log << MSG::ERROR
              << "===== Failure to merge " << name()
              << " type " << m_solid->typeName()
              << " x " << Point.x()
              << " y " << Point.y()
              << " z " << Point.z()
              << " #own   " << own.size() 
              << " #child " << childrens.size()            
              << endreq ;
          
          for ( ILVolume::Intersections::const_iterator itI = own.begin();
                own.end() != itI; itI++ ) {
            double s = (*itI).first.first;
            double t = (*itI).first.second;
            log << MSG::INFO 
                << "Own : s " << s
                << " x " << Point.x() + s * Vector.x()
                << " y " << Point.y() + s * Vector.y()
                << " z " << Point.z() + s * Vector.z()
                << " to t " << t
                << " x " << Point.x() + t * Vector.x()
                << " y " << Point.y() + t * Vector.y()
                << " z " << Point.z() + t * Vector.z()
                << " radl " << (*itI).second->radiationLength()
                << endreq ;
          }
          for ( ILVolume::Intersections::const_iterator itJ = childrens.begin();
                childrens.end() != itJ; itJ++ ) {
            double s = (*itJ).first.first;
            double t = (*itJ).first.second;
            log << MSG::INFO
                << "Child : s " << s
                << " x " << Point.x() + s * Vector.x()
                << " y " << Point.y() + s * Vector.y()
                << " z " << Point.z() + s * Vector.z()
                << " to t " << t
                << " x " << Point.x() + t * Vector.x()
                << " y " << Point.y() + t * Vector.y()
                << " z " << Point.z() + t * Vector.z()
                << " radl " << (*itJ).second->radiationLength()
                << endreq ;
          }
          
          intersections.clear();          
          std::copy( childrens.begin () , 
                     childrens.end   () ,
                     std::back_inserter( intersections ) ) ;
          
          return intersections.size();                          // RETURN!!!
          // commented out as temporary solution.
          // Assert( false , "LVolumeIntersectLine FATAL! " + name() , sc ); 
        }
    }
  //  
  return intersections.size(); 
  //
};

// ============================================================================
// The End 
// ============================================================================

