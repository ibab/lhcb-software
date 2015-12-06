// $Id: LVolume.cpp,v 1.36 2009-04-17 08:54:24 cattanem Exp $ 
// ============================================================================
/// STD & STL includes 
// ============================================================================
#include <stdio.h> 
#include <functional> 
#include <algorithm> 
// ============================================================================
/// Gaudi Kernel includes
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SmartDataPtr.h" 
// ============================================================================
/// DetDesc includes 
// ============================================================================
#include "DetDesc/ISolid.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/LVolume.h"
#include "DetDesc/SolidTicks.h"
#include "DetDesc/Material.h"
#include "DetDesc/Solid.h"
#include "DetDesc/Surface.h"
#include "DetDesc/DetDesc.h"
#include "DetDesc/IntersectionErrors.h"
#include "DetDesc/VolumeIntersectionIntervals.h"
// ===========================================================================
/** @file LVolume.cpp 
 *  
 *  implementation of class LVolume 
 *
 *  @author: Vanya Belyaev  Ivan.Belyaev@itep.ru 
 *  @author: Sebastien Ponce
 */
// ===========================================================================
/*  constructor, pointer to ISolid* must be valid!, 
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
}
// =========================================================================== 
/* (default) constructor
 *  @param name         name of logical volume 
 */
// =========================================================================== 
LVolume::LVolume( const std::string& name )
  : LogVolBase     ( name       )
  , m_solid        ( 0          )
  , m_materialName ( ""         )
  , m_material     ( 0          )
{}
// ===========================================================================
// destructor 
// ===========================================================================
LVolume::~LVolume()
{
  /// reset 
  if( 0 != m_solid ) { delete m_solid ; m_solid = 0 ; }
}
// ============================================================================
const CLID& LVolume::clID   () const { return classID()    ; } 
// ============================================================================
const CLID& LVolume::classID()       { return CLID_LVolume ; }
// ============================================================================
/*  calculate the daughter path containing the Point in Local frame , 
 *  can be VERY slow for complex geometry, 
 *  therefore use the appropriate Level for usage 
 *  @param  LocalPoint point in local reference system of logical volume 
 *  @param  level depth level
 *  @param  volumePath  vector of physical volumes
 *  @return status code 
 */
// ============================================================================
StatusCode LVolume::belongsTo
( const Gaudi::XYZPoint&        LocalPoint  ,
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
}
// ============================================================================
/*  calculate the daughter path containing the Point in Local frame , 
 *  can be VERY slow for complex geometry, 
 *  therefore use the appropriate Level for usage 
 *  @param  LocalPoint point in local reference system of logical volume 
 *  @param  level depth level
 *  @param  volumePath  vector of physical volumes
 *  @return status code 
 */
// ============================================================================
StatusCode LVolume::belongsTo
( const Gaudi::XYZPoint&        LocalPoint ,
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
}
// ============================================================================
/*  locate material
 *  @exception LVolumeException
 *  @return pointer to material
 */
// ============================================================================
const Material* LVolume::findMaterial() const 
{
  SmartDataPtr<const Material> material( dataSvc() , materialName() );
  if( !material ) {
    throw LogVolumeException( "Could not locate material " + materialName(),
                              this, StatusCode::FAILURE );
  }
  m_material = material;
  return m_material; 
}
// ============================================================================
/*  printout to STD/STL stream
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
}
// ============================================================================
/*  printout to Gaudi MsgStream stream
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
     << endmsg;
  ///
  return os << "Material name='" << materialName() << "' " << endmsg;
}
// ============================================================================
/*  Auxillary method  to calculate own intersections
 *  @exception LogVolumeException for wrong parameters or geometry errors
 *  @param  Point         initial point at the line
 *  @param  Vector        direction vector of the line
 *  @param  intersections output container
 *  @param  threshold     threshold value 
 *  @return true if line intersects with body
 */
// ============================================================================
bool LVolume::intersectBody
( const Gaudi::XYZPoint         & Point         , 
  const Gaudi::XYZVector        & Vector        , 
  ILVolume::Intersections  & intersections , 
  const double               Threshold     ) const 
{
  /// avoid long names 
  typedef std::vector<ILVolume::Interval>    Intervals; 
  using namespace VolumeIntersectionIntervals;
  /// (1) clear the output container 
  intersections.clear();
  /// length of tick "unit"
  const ISolid::Tick TickLength = std::sqrt(Vector.mag2()) ;  
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
  
  // calculate intersections with cover volume
  if( pSolid->intersectionTicks( Point , Vector , ticks ) < 2) { return false ; }

  if( Threshold>0 || m_solid == pSolid ) {
    // construct intervals
    intervals.clear();
    intervals.reserve(ticks.size());
    TicksToIntervals( ticks , std::back_inserter( intervals ) );
    // check the total thickness
    if( Threshold > 0 ) {
      /// the total length of intervals (in tick units)
      double Length = 
        std::accumulate( intervals.begin() , intervals.end       () , 
                         0.0               , AccumulateIntervals () );
      /*  this volume is not to be used for estimations 
       *  of radiation thickness 
       */
      if( Length * TickLength <= Threshold * m_material->radiationLength() ) {
        useThisVolume = false ;   
        ticks.clear     ();
        intervals.clear (); 
      }
    }
  }
  /*  if this volume is to be used,  
   *  find intersections with REAL solid (not cover) 
   */
  if ( useThisVolume && m_solid != pSolid ) 
  {
    /*  no intersections with own solid 
     *  ( at least 2 points are required! )
     *  for solid() == solid->coverTop() we have 
     *  already calculated ticks!
     */
    if (  m_solid->intersectionTicks( Point , Vector , ticks ) < 2) { return false ; }
    
    /** estimate more carefuly its contribution to the total 
     *  radiation thickness and construct intervals!
     */
    intervals.clear();
    intervals.reserve(ticks.size());
    TicksToIntervals( ticks , std::back_inserter( intervals ) ); 
  }
  
  double minInterval =  Threshold * m_material->radiationLength() / TickLength;
  
  /// transform container of its own intervals into own intersection container 
  for ( Intervals::const_iterator it = intervals.begin() ; 
        intervals.end() != it ; ++it ) { 
    if ( minInterval < fabs( (*it).second - (*it).first ) ) {
      intersections.push_back( ILVolume::Intersection( *it , m_material ) ) ; 
    }
  }
  ///
  /// V.B.:try to correct the promlems (if any) 
  VolumeIntersectionIntervals::correct 
    ( this , intersections , TickLength ) ;  
  ///
  return true; 
}
// ============================================================================
/*  Auxillary method  to calculate own intersections
 *  @exception LogVolumeException for wrong parameters or geometry errors
 *  @param  Point         initial point at the line
 *  @param  Vector        direction vector of the line
 *  @param  intersections output container
 *  @param  threshold     threshold value 
 *  @return true if line intersects with body 
 */
// ============================================================================
bool LVolume::intersectBody
( const Gaudi::XYZPoint         & Point         ,
  const Gaudi::XYZVector        & Vector        ,
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
  if ( tickMin >= tickMax ) { return 0 ;} 
  /* line with null direction vector 
   * is not able to intersect any volume
   */ 
  if ( Vector.mag2() <= 0 ) { return 0 ; }       // RETURN !!! 
  // length of tick unit 
  const ISolid::Tick TickLength = std::sqrt(Vector.mag2()); 
  // assertion for material and solid 
  if ( 0 == material() || 0 == m_solid ) 
  { Assert( false , "LVolume::intersectBody(1): FATAL for" + name() ); }
  // define "top"
  const ISolid* pSolid = m_solid->coverTop() ; 
  //  contribution of this volume is estimated in 3 steps:
  bool useThisVolume      = true  ; 
  /*   (1) estimate the maximal possible constribution 
   *   of this volume using tickMin and tickMax 
   */
  if ( Threshold > 0  &&  
       ( tickMax - tickMin )  * TickLength  
       <= Threshold * m_material->radiationLength() ) 
  { useThisVolume = false; }                                   /// NB!!!! 
  ISolid::Ticks           ticks;   
  /*  If threshold is given, before performing the accurate 
   *  calculations, perform the rought estimation 
   *  using the coverTop() method!  
   *  Line MUST intersect the coverTop() solid at 
   *  least in 2 points! common points with cover 
   *  TopSolid within range from tickMin to tickMax 
   */
  // first intersect with the covertop. return if no intersections.
  if ( pSolid->intersectionTicks
       ( Point   ,  
         Vector  , 
         tickMin , 
         tickMax , 
         ticks   ) < 2 ) { return false ; } 
  // redefine tickMin and tickMax. wdh: why? to hide a real bug?
  tickMin = *std::min_element( ticks.begin() , ticks.end() ); 
  tickMax = *std::max_element( ticks.begin() , ticks.end() );
  
  ///
  Intervals intervals; 
  if ( ( useThisVolume && Threshold > 0 ) || m_solid == pSolid ) {
    // create the intervals for the covertop
    intervals.reserve(ticks.size());
    TicksToIntervals( ticks , std::back_inserter( intervals ) );
    
    // if there is a threshold, check that covertop intersection large enough
    if ( Threshold > 0 ) 
    {
      /// the total length of intervals in Tick units 
      double Length = 
        std::accumulate( intervals.begin() , intervals.end       () ,
                         0.0               , AccumulateIntervals () );
      /// this volume is not to be used for estimations of radiation thickness 
      if( Length * TickLength 
          <= Threshold * m_material->radiationLength() ) {
        useThisVolume = false ;   
        intervals.clear(); 
        ticks.clear(); 
      }
    }
  }
  /*   if this volume is to be used,  
   *   find intersections with REAL solid (not cover) 
   *   no intersections with own solid 
   *   ( at least 2 points are required! )
   */
  if ( useThisVolume && m_solid != pSolid ) 
  {
    /*  no intersections with own solid 
     *   ( at least 2 points are required! )
     *  (sor solidUnion we have already calculated ticks!
     */
    if ( 2 > solid()->intersectionTicks
         ( Point   , 
           Vector  , 
           tickMin , 
           tickMax , 
           ticks   ) ) { return false ; }
    /*  check for tickMin and tickMax 
     *  remove extra ticks from container
     * refine tickMin and tickMax
     */
    tickMin = *std::min_element( ticks.begin() , ticks.end() ); 
    tickMax = *std::max_element( ticks.begin() , ticks.end() ); 
    /* estimate more carefuly its contribution to the total 
     *  radiation thickness and construct intervals!
     */
    intervals.clear(); 
    intervals.reserve(ticks.size());
    TicksToIntervals( ticks , std::back_inserter( intervals ) );
    if(Threshold>0) {
      /// the total length of intervals in Tick units 
      double Length = 
        std::accumulate( intervals.begin() , intervals.end       () ,
                         0.0               , AccumulateIntervals () );
      /// this volume is not to be used for estimations of radiation thickness 
      if( Length * TickLength 
          <= Threshold * m_material->radiationLength() ) {
        ticks.clear()         ; 
        intervals.clear()     ;
        useThisVolume = false ; 
      }  
    }
  }
  /*  transform container of its own intervals 
   *  into own intersection container
   */ 
  double minInterval =  
    Threshold * m_material->radiationLength() / TickLength;
  
  for ( Intervals::const_iterator it = intervals.begin() ; 
        intervals.end() != it ; ++it )    { 
    if ( minInterval < fabs( (*it).second - (*it).first ) ) 
    {
      intersections.push_back( ILVolume::Intersection( *it , m_material ) ) ; 
    }
  }
  /// check the intersections 
  /// V.B.: try to correct 
  VolumeIntersectionIntervals::correct 
    ( this ,  intersections , TickLength ) ;  
  ///
  return true; 
  ///
}
// ============================================================================
/*  intersection of the logical volume with with the line \n
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
( const Gaudi::XYZPoint        & Point         , 
  const Gaudi::XYZVector       & Vector        , 
  ILVolume::Intersections & intersections , 
  const double              Threshold     ) const 
{
  // avoid long names 
  //  typedef std::vector<ILVolume::Interval>    Intervals; 
  using namespace VolumeIntersectionIntervals ;
  // clear the container 
  intersections.clear();
  // line with null direction vector is not able to intersect any volume 
  if ( Vector.mag2() <= 0 ) { return 0 ; }       // RETURN !!!
  
  // intersections with own "volume body"
  ILVolume::Intersections own;
  if (!intersectBody( Point, Vector, own, Threshold ) ) { return 0 ; }
  
  // intersections with childrens 
  ILVolume::Intersections childrens; 
  intersectDaughters
    ( Point     , Vector    , childrens , Threshold );
  // here we'd like to fill the output container:
  if ( own.empty() )    // the parent container is empty
  {
    // we have only child container just copy it to the output 
    std::copy ( childrens.begin() , childrens.end  () , 
                std::back_inserter( intersections ) ) ;
    return intersections.size();  ///< RETURN!!!
  }
  else // own container is NOT empty!
  {
    // perform quite non trivial merging 
    StatusCode sc = 
      // MergeOwnAndChildContainers
      MergeOwnAndChildContainers2
      ( own       , 
        childrens , 
        std::back_inserter ( intersections ) , 
        this , Vector.R() ) ; 
    // check the result!!!
    if ( sc.isFailure() ) 
    { DetDesc::IntersectionErrors::setCode ( sc , this ) ; }
  }
  ///  
  return intersections.size(); 
}
// ============================================================================
/*  intersection of the logical volume with with the line \n
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
( const Gaudi::XYZPoint         & Point         , 
  const Gaudi::XYZVector        & Vector        , 
  ILVolume::Intersections  & intersections , 
  const ISolid::Tick         tickMin       , 
  const ISolid::Tick         tickMax       , 
  const double               Threshold     ) const 
{
  ISolid::Tick TickMin = tickMin ;
  ISolid::Tick TickMax = tickMax ;  
  // avoid long names 
  //  typedef std::vector<ILVolume::Interval>    Intervals;
  using namespace VolumeIntersectionIntervals;
  // clear the output container 
  intersections.clear();
  // check the valid tick values 
  if( tickMin >= tickMax ) { return 0 ;}            // RETURN !!! 
  /* line with null direction vector 
   * is not able to intersect any volume
   */ 
  if( Vector.mag2() <= 0 ) { return 0 ; }           // RETURN !!!

  // own intersections
  ILVolume::Intersections own;
  if ( !intersectBody (Point,Vector,own,TickMin,TickMax,Threshold ) ) { return 0 ; }
  
  /*  look for the intersections of the given 
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
      // MergeOwnAndChildContainers
      MergeOwnAndChildContainers2
      ( own       , 
        childrens , 
        std::back_inserter( intersections ) , 
        this , Vector.R () ) ;
    // check the result !
    if ( sc.isFailure() ) 
    { DetDesc::IntersectionErrors::setCode ( sc , this ) ; } 
  }
  //  
  return intersections.size(); 
  //
}
// ============================================================================
// The End 
// ============================================================================

