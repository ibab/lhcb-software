// $Id: LAssembly.cpp,v 1.1 2001-11-18 15:32:44 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/StreamBuffer.h"
// DetDesc
#include "DetDesc/LAssembly.h"

// ============================================================================
/** @file LAssembly.cpp
 *
 *  Implementation file for class : LAssembly
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 18/11/2001 
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
  const std::string& sensitivity ,
  const std::string& magnetic    )
  : LogVolBase( name        , 
                sensitivity , 
                magnetic    )
{};

// ============================================================================
/** constructor 
 *  @exception LVolumeException wrong parameter value
 *  @param name         name of logical volume 
 *  @param validity     validity object 
 *  @param sensitivity  name of sensitive detector object (for simulation)
 *  @param magnetic     name of magnetic field object (for simulation)
 */
// ============================================================================
LAssembly::LAssembly
( const std::string& name        ,
  const IValidity  & validity    ,
  const std::string& sensitivity ,
  const std::string& magnetic    )
  : LogVolBase( name        ,
                validity    , 
                sensitivity , 
                magnetic    )
{};

// ============================================================================
/** constructor
 *  @exception LVolumeException wrong paramaters value
 *  @param name name of logical volume 
 *  @param validSince  begin of validity range 
 *  @param validTill  end of validity range 
 *  @param sensitivity  name of sensitive detector object (for simulation)
 *  @param magnetic  nam eof magnetic field object (for simulation)
 */
// ============================================================================
LAssembly::LAssembly
( const std::string& name        , 
  const ITime&       validSince  , 
  const ITime&       validTill   , 
  const std::string& sensitivity ,
  const std::string& magnetic    )
  : LogVolBase ( name        , 
                 validSince  , 
                 validTill   , 
                 sensitivity , 
                 magnetic    )
{};

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
LAssembly::~LAssembly() { reset(); }

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
( const HepPoint3D&        LocalPoint  ,
  const int                Level       , 
  ILVolume::PVolumePath&   pVolumePath ) const 
{    
  /// check the depth 
  if( 0 == Level ) { return StatusCode::SUCCESS; } 
  /// check the point 
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
( const HepPoint3D&        LocalPoint  ,
  const int                Level       , 
  ILVolume::ReplicaPath&   replicaPath ) const  
{    
  /// check the depth 
  if( 0 == Level     ) { return StatusCode::SUCCESS; } 
  /// check the point 
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
( const HepPoint3D        & Point         , 
  const HepVector3D       & Vector        , 
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
( const HepPoint3D         & Point         , 
  const HepVector3D        & Vector        , 
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
  /** look for the intersections of the given 
   *  line with daughter elements construct the 
   *  intersections container for daughter volumes
   */
  intersectDaughters
    ( Point , Vector , intersections , tickMin , tickMax , Threshold  );
  return intersections.size();    ///< RETURN!!!
};


// ============================================================================
/** object serialization for writing
 *  @param   sb reference to the stream
 *  @return     reference to the stream 
 */
// ============================================================================
StreamBuffer& LAssembly::serialize ( StreamBuffer& sb ) const 
{ return LogVolBase::serialize( sb ); }

  
// ============================================================================
/** object serialization for reading
 *  @param   sb reference to the stream
 *  @return     reference to the stream 
 */
// ============================================================================
StreamBuffer& LAssembly::serialize ( StreamBuffer& sb )       
{ return LogVolBase::serialize( sb ); }

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


// ============================================================================
// The End 
// ============================================================================
