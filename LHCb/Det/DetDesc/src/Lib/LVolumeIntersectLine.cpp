// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================

#include "DetDesc/LVolume.h"
#include "DetDesc/VolumeIntersectionIntervals.h"

/** @file LVolumeIntersectLine.cpp 
 * 
 *  implementation of LVolume::intersectLine methods 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

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
 *  @param Point initial point at the line
 *  @param Vector direction vector of the line
 *  @param intersections output container 
 *  @param threshold threshold value 
 *  @return number of intersections  
 */
unsigned int 
LVolume::intersectLine( const HepPoint3D         & Point         , 
                        const HepVector3D        & Vector        , 
                        ILVolume::Intersections  & intersections , 
                        const double               Threshold     ) 
{
  /// useful type definition  
  typedef std::vector<ILVolume::Interval>    Intervals; 
  /// clear the container 
  intersections.clear();
  /// line with null direction vector is not able to intersect any volume 
  if( Vector.mag2() <= 0 ) { return 0 ; }       // RETURN !!! 
  
  /// intersections with "volume body"
  ILVolume::Intersections own;
  if( !isAssembly() ) { intersectBody( Point, Vector , own , Threshold ); }
  /// interasections with childrens 
  ILVolume::Intersections childrens; 
  if( pvBegin() != pvEnd() ) 
    { intersectDaughters( Point , Vector , childrens, Threshold ); }
  
  /// here we'd like to fill the output container:
  if( own.empty() )    // the parent container is empty
    {
      /// we have only child container just copy it to the output 
      std::copy( childrens.begin()          , 
                 childrens.end  ()          , 
                 std::back_inserter( intersections ) ) ;
      return intersections.size();  ///< RETURN!!!
    }
  else /// own container is NOT empty!
    {
      /// perform quite non trivial merging 
      StatusCode sc = 
        VolumeIntersectionIntervals::
        MergeOwnAndChildContainers( own                                 , 
                                    childrens                           ,
                                    std::back_inserter( intersections ) );
      /// check the result!!!
      Assert( sc.isSuccess() , 
              "LVolumeIntersectLine(1) fatal error!LVolume = " + 
              fullpath() , sc );
    }
  ///  
  return intersections.size(); 
};

/** Auxillary method  to calculate own intersections
 *  @exception LVolumeException wrong parameters or geometry error
 *  @param Point initial point at the line
 *  @param Vector direction vector of the line
 *  @param intersections output container
 *  @param threshold threshold value 
 *  @return number of intersections  
 */
unsigned int 
LVolume::intersectBody( const HepPoint3D         & Point         , 
                        const HepVector3D        & Vector        , 
                        ILVolume::Intersections  & intersections , 
                        const double               Threshold     ) 
{
  /// useful type definition  
  typedef std::vector<ILVolume::Interval>    Intervals; 
  /// clear the container 
  intersections.clear();
  /// length of tick "unit"
  const ISolid::Tick TickLength = Vector.mag() ;  
  /// assertion for material and solid 
  Assert( 0 != material() , 
          "LVolume::intersectBody, material() points to NULL for LVolume=" 
          + fullpath() ); 
  Assert( 0 != solid()    , 
          "LVolume::intersectBody, solid   () points to NULL for LVolume=" 
          + fullpath() ); 
  ///
  const ISolid* pSolid = 
    ( "SolidUnion" == solid()->typeName() ) ? solid() : solid()->coverTop() ; 
  ///
  ISolid::Ticks           ticks               ;   
  Intervals               intervals           ; 
  bool                    useThisVolume = true;  
  
  /**  If threshold is given, before performing the accurate calculations,
   *   perform the rought estimation using the coverTop() method!  
   *   Line MUST intersect the coverTop() solid at least in 2 points!
   */ 
  if( Threshold > 0 || solid() == pSolid ) 
    { 
      /// number of intersections 
      if( 2 >  pSolid->intersectionTicks( Point , Vector , ticks ) ) 
        { return 0; }   /// RETURN !!!        
      /** estimate the radiation thickness of the coverTop volume 
       *  and  construct intervals
       */
      intervals.clear();
      VolumeIntersectionIntervals::
        TicksToIntervals( ticks , std::back_inserter( intervals ) );
      /// the total length of intervals (in tick units)
      double Length = 
        std::accumulate( intervals.begin() , 
                         intervals.end  () , 
                         0.0               , 
                         VolumeIntersectionIntervals::AccumulateIntervals() );
      /// this volume is not to be used for estimations of radiation thickness 
      if( Length * TickLength <= Threshold * material()->radiationLength() ) 
        {
          useThisVolume = false ;   
          ticks.clear(); 
          intervals.clear(); 
        }
    }
  /** if this volume is to be used,  
   *  find intersections with REAL solid (not cover) 
   */
  if( useThisVolume && solid() != pSolid ) 
    {
      /** no intersections with own solid 
       *  ( at least 2 points are required! )
       *  for solid() == solid->coverTop() we have 
       *  already calculated ticks!
       */
      if( 2 > solid()->intersectionTicks( Point , Vector , ticks ) ) 
        { return 0; }      /// RETURN !!! 
      /** estimate more carefuly its contribution to the total 
       *  radiation thickness and construct intervals!
       */
      intervals.clear();
      VolumeIntersectionIntervals::
        TicksToIntervals( ticks , std::back_inserter( intervals ) );  
      /// the total length of intervals in Tick units
      double Length = 
        std::accumulate( intervals.begin() , 
                         intervals.end()   ,
                         0.0               ,
                         VolumeIntersectionIntervals::AccumulateIntervals() ); 
      /** this volume is not to be used for estimations 
       *  of radiation thickness
       */
      if( Threshold > 0 && 
          Length * TickLength < Threshold * material()->radiationLength() ) 
        { 
          ticks.clear()         ; 
          intervals.clear()     ;
          useThisVolume = false ; 
        }      
    }
  /// transform container of its own intervals into own intersection container 
  for( Intervals::const_iterator it = intervals.begin() ; 
       intervals.end() != it ; ++it ) 
    { intersections.push_back( ILVolume::Intersection( *it , material() ) ) ; }
  ///  
  return intersections.size(); 
};

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
unsigned int 
LVolume::intersectLine( const HepPoint3D         & Point         ,
                        const HepVector3D        & Vector        ,
                        ILVolume::Intersections  & intersections ,
                        ISolid::Tick               tickMin       ,
                        ISolid::Tick               tickMax       ,
                        const double               Threshold     )
{
  /// useful type definition 
  typedef std::vector<ILVolume::Interval>    Intervals; 
  /// clear the output container 
  intersections.clear();
  /// check the valid tick values 
  if( tickMin >= tickMax ) { return 0 ;} 
  /** line with null direction vector 
   * is not able to intersect any volume
   */ 
  if( Vector.mag2() <= 0 ) { return 0 ; }       // RETURN !!! 
  
  /** own interasections 
   */
  ILVolume::Intersections own;
  if( !isAssembly() ) 
    { 
      intersectBody( Point     , 
                     Vector    , 
                     own       , 
                     tickMin   , 
                     tickMax   ,
                     Threshold ); 
    }
  /** look for the intersections of the given 
   *  line with daughter elements construct the 
   *  intersections container for daughter volumes
   */
  ILVolume::Intersections childrens; 
  if( pvBegin() != pvEnd() ) 
    {
      intersectDaughters( Point      , 
                          Vector     , 
                          childrens  , 
                          tickMin    , 
                          tickMax    , 
                          Threshold  );
    }  
  /// here we'd like to fill the output container:
  if( own.empty() )    // the parent container is empty
    {
      /// we have only child container - just copy it to the output 
      std::copy( childrens.begin()          , 
                 childrens.end  ()          , 
                 std::back_inserter( intersections ) ) ;
      return intersections.size();    ///< RETURN!!!
    }
  else   /// own container is NOT empty!
    {
      StatusCode sc = 
        VolumeIntersectionIntervals::
        MergeOwnAndChildContainers( own                                 , 
                                    childrens                           ,
                                    std::back_inserter( intersections ) );
      Assert( sc.isSuccess() , 
              "LVolume::intersectLine::(2) fatal error!for LVolume="
              +fullpath() , sc );
    }
  ///  
  return intersections.size(); 
  ///
};


unsigned int 
LVolume::intersectBody( const HepPoint3D         & Point         ,
                        const HepVector3D        & Vector        ,
                        ILVolume::Intersections  & intersections ,
                        ISolid::Tick             & tickMin       ,
                        ISolid::Tick             & tickMax       ,
                        const double               Threshold     )
{
  /// useful type definition 
  typedef std::vector<ILVolume::Interval>    Intervals; 
  /// clear the output container 
  intersections.clear();
  /// check the valid tick values 
  if( tickMin >= tickMax ) { return 0 ;} 
  /** line with null direction vector 
   * is not able to intersect any volume
   */ 
  if( Vector.mag2() <= 0 ) { return 0 ; }       // RETURN !!! 
  /// length of tick unit 
  const ISolid::Tick TickLength = Vector.mag(); 
  /// assertion for material and solid 
  Assert( 0 != material() , 
          "LVolume::intersectLine, material()==NULL for LVolume=" 
          + fullpath() ); 
  Assert( 0 != solid()    , 
          "LVolume::intersectLine, solid   ()==NULL for LVolume=" 
          + fullpath() ); 
  /// define "top"
  const ISolid* pSolid = 
    ( "SolidUnion"  == solid()->typeName() ) ? solid() : solid()->coverTop() ; 
  ///  contribution of this volume is estimated in 3 steps:
  bool useThisVolume      = true  ; 
  /**  (1) estimate the maximal possible constribution 
   *   of this volume using tickMin and tickMax 
   */
  if( Threshold > 0  &&  
      ( tickMax - tickMin )  * TickLength  
      <= Threshold * material()->radiationLength() ) 
    { useThisVolume = false; }  /// NB!!!! 
  ISolid::Ticks           ticks;   
  /** If threshold is given, before performing the accurate 
   *  calculations, perform the rought estimation 
   *  using the coverTop() method!  
   *  Line MUST intersect the coverTop() solid at 
   *  least in 2 points! common points with cover 
   *  TopSolid within range from tickMin to tickMax 
   */
  if( useThisVolume || solid() == pSolid ) 
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
      || solid() == pSolid ) 
    { 
      /** estimate the radiation thickness of the coverTop 
       *  volume and construct intervals
       */ 
      intervals.clear(); 
      VolumeIntersectionIntervals::
        TicksToIntervals( ticks , std::back_inserter( intervals ) );
      /// the total length of intervals in Tick units 
      double Length = 
        std::accumulate( intervals.begin() , 
                         intervals.end  () ,
                         0.0               ,
                         VolumeIntersectionIntervals::AccumulateIntervals() );
      /// this volume is not to be used for estimations of radiation thickness 
      if( Length * TickLength <= Threshold * material()->radiationLength() ) 
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
  if( useThisVolume && solid() != pSolid ) 
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
      VolumeIntersectionIntervals::
        TicksToIntervals( ticks , std::back_inserter( intervals ) );  
      /// the total length of intervals in Tick units 
      double Length = 
        std::accumulate( intervals.begin() , 
                         intervals.end  () ,
                         0.0               ,
                         VolumeIntersectionIntervals::AccumulateIntervals() );
      /// this volume is not to be used for estimations of radiation thickness 
      if( Length * TickLength < Threshold * material()->radiationLength() ) 
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
      intersections.push_back( ILVolume::Intersection( *it , material() ) );
    }
  ///
  return intersections.size(); 
  ///
};

/// Auxillary method  to calculate intersections with daughter volumes  
unsigned int
LVolume::intersectDaughters( const HepPoint3D&        Point              , 
                             const HepVector3D&       Vector             , 
                             ILVolume::Intersections& childIntersections , 
                             const ISolid::Tick     & tickMin            , 
                             const ISolid::Tick     & tickMax            , 
                             const double             Threshold          )
{
  ///
  if( pvEnd() ==  pvBegin() ) { return 0; } /// RETURN!!!
  ///
  ILVolume::Intersections child; 
  for( ILVolume::PVolumes::iterator it = pvBegin() ; pvEnd() != it ; ++it ) 
    { 
      IPVolume* pv = *it; 
      Assert( 0 != pv , 
              "LVolume::intersect line, IPVolume==NULL for " + 
              fullpath()); 
      /// construct the intersections container for each daughter volumes 
      pv->intersectLine( Point     , 
                         Vector    , 
                         child     , 
                         tickMin   , 
                         tickMax   , 
                         Threshold ) ;  
      /// merge individidual containers and clear child container 
      std::copy( child.begin() , 
                 child.end  () , 
                 std::back_inserter( childIntersections ) ) ; 
      child.clear();
    }
  ///
  std::sort( childIntersections.begin() , 
             childIntersections.end  () , 
             VolumeIntersectionIntervals::CompareIntersections() ); 
  ///
  return childIntersections.size(); 
  ///
};

/// Auxillary method  to calculate intersections with daughter volumes  
unsigned int  
LVolume::intersectDaughters( const HepPoint3D&        Point              , 
                             const HepVector3D&       Vector             , 
                             ILVolume::Intersections& childIntersections , 
                             const double             Threshold          )
{
  ///
  if( pvEnd() ==  pvBegin() ) { return 0; } /// RETURN!!!
  ///
  ILVolume::Intersections child; 
  for( ILVolume::PVolumes::iterator it = pvBegin() ; pvEnd() != it ; ++it ) 
    { 
      IPVolume* pv = *it; 
      Assert( 0 != pv , 
              "LVolume::intersect line, IPVolume==NULL for " +
              fullpath()); 
      /// construct the intersections container for each daughter volumes 
      pv->intersectLine( Point     , 
                         Vector    , 
                         child     , 
                         Threshold ) ;  
      /// merge individidual containers and clear child container 
      std::copy( child.begin() , 
                 child.end  () , 
                 std::back_inserter( childIntersections ) ) ; 
      child.clear();
    }
  ///
  std::sort( childIntersections.begin() , 
             childIntersections.end  () , 
             VolumeIntersectionIntervals::CompareIntersections() ); 
  ///
  return childIntersections.size(); 
  ///
};
/// 
