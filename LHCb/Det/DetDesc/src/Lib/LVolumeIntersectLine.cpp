#ifndef      __DETDESC_VOLUMES_LVOLUMEINTERSECTLINE_H__ 
#define      __DETDESC_VOLUMES_LVOLUMEINTERSECTLINE_H__  1


#include "DetDesc/LVolume.h"
#include "DetDesc/VolumeIntersectionIntervals.h"

///  inmplementation of LVolume::intersectLine methods 



///
/// intersection of the logical volume with the line.
/// line is parametrized in the local reference system of the logical volume 
/// by initial Point and direction Vector.
/// Method returns the number of intersection points("ticks") and 
/// the container of pairs - ticks and pointer to the corresponding 
/// material. The simplification is determined by value of threshold
/// (in units of radiation length)   
///

unsigned int LVolume::intersectLine( const HepPoint3D         & Point         , // initial point at the line 
                                     const HepVector3D        & Vector        , // direction vectot of the line 
                                     ILVolume::Intersections  & intersections , // output container  
                                     const double               Threshold     ) // threshold value 
{
  /// useful type definition  
  typedef std::vector<ILVolume::Interval>    Intervals; 
  /// clear the container 
  intersections.clear();
  /// line with null direction vector is not able to intersect any volume 
  if( Vector.mag2() <= 0 )                                       { return 0 ; }       // RETURN !!! 
  /// length of tick "unit"
  const ISolid::Tick TickLength = Vector.mag() ; 
  /// assertion for material and solid 
  Assert( 0 != material() , "LVolume::intersectLine, material() points to NULL for LVolume="+fullpath() ); 
  Assert( 0 != solid()    , "LVolume::intersectLine, solid   () points to NULL for LVolume="+fullpath() ); 
  ///
  const ISolid* pSolid = ( "solidUnion" == solid()->typeName() ) ? solid() : solid()->coverTop() ; 
  ///
  ISolid::Ticks           ticks               ;   
  Intervals               intervals           ; 
  bool                    useThisVolume = true;       
  /// If threshold is given, before performing the accurate calculations, perform the rought estimation 
  /// using the coverTop() method!  
  /// Line MUST intersect the coverTop() solid at least in 2 points!
  if( Threshold > 0 || solid() == pSolid ) 
    { 
      /// number of intersections 
      if( 2 >  pSolid->intersectionTicks( Point , Vector , ticks ) )      { return 0; }   /// RETURN !!!        
      /// estimate the radiation thickness of the coverTop volume and  construct intervals 
      intervals.clear();
      VolumeIntersectionIntervals::TicksToIntervals( ticks , std::back_inserter( intervals ) );
      /// the total length of intervals (in tick units)
      double Length    = std::accumulate( intervals.begin() , intervals.end() ,                 // what to accumulate?
                                          0.0               ,                                   // initial value 
                                          VolumeIntersectionIntervals::AccumulateIntervals() );  // "accumulator"
      /// this volume is not to be used for estimations of radiation thickness 
      if( Length * TickLength <= Threshold * material()->radiationLength() ) 
        {
          useThisVolume = false ;   
          ticks.clear(); 
          intervals.clear(); 
        }
    }
  /// if this volume is to be used,  find intersections with REAL solid (not cover) 
  if( useThisVolume && solid() != pSolid ) 
    {
      /// no intersections with own solid ( at least 2 points are required! )
      /// for solid() == solid->coverTop() we have already calculated ticks!
      if( 2 > solid()->intersectionTicks( Point , Vector , ticks ) )      { return 0; }      /// RETURN !!! 
      /// estimate more carefuly its contribution to the total radiation thickness 
      /// construct intervals!
      intervals.clear();
      VolumeIntersectionIntervals::TicksToIntervals( ticks , std::back_inserter( intervals ) );  
      /// the total length of intervals in Tick units
      double Length    = std::accumulate( intervals.begin() , intervals.end() ,                 // what to accumulate?
                                          0.0               ,                                   // initial value 
                                          VolumeIntersectionIntervals::AccumulateIntervals() ); // "accumulator"
      /// this volume is not to be used for estimations of radiation thickness 
      if( Threshold > 0 && Length * TickLength < Threshold * material()->radiationLength() ) 
        { 
          ticks.clear()         ; 
          intervals.clear()     ;
          useThisVolume = false ; 
        }      
    }
  /// look for the intersections of the given line with daughter elements 
  /// construct the intersections container for daughter volumes 
  ILVolume::Intersections childIntersections; 
  if( pvBegin() != pvEnd() ) {
    //unsigned int nIntDaug = 
    intersectDaughters( Point , Vector , childIntersections , Threshold );
  }
  /// transform container of its own intervals into own intersection container 
  ILVolume::Intersections ownIntersections;
  for( Intervals::const_iterator it = intervals.begin() ; intervals.end() != it ; ++it ) 
    { ownIntersections.push_back( ILVolume::Intersection( *it , material() ) ) ; }    
  /// here we'd like to fill the output container:
  if( ownIntersections.empty() )    // the parent container is empty
    {
      /// we have only child container just copy it to the output 
      std::copy( childIntersections.begin()          , 
                 childIntersections.end  ()          , 
                 std::back_inserter( intersections ) ) ;
      return intersections.size();                                                  /// RETURN!!!
      ///
    }
  else   /// own container is NOT empty!
    {
      /// perform quite non trivial merging 
      StatusCode sc = 
        VolumeIntersectionIntervals::MergeOwnAndChildContainers( ownIntersections                    , 
                                                                 childIntersections                  ,
                                                                 std::back_inserter( intersections ) );
      /// check the result!!!
      Assert( sc.isSuccess() , 
              "LVolume::intersectLine::(1) fatal error in Geometry for lvolume="+fullpath() , sc );
    }
  ///  
  return intersections.size(); 
  ///
};
///
/// intersection of the logical volume with the line.
/// line is parametrized in the local reference system of the logical volume 
/// by initial Point and direction Vector.
/// Method returns the number of intersection points("ticks") and 
/// the container of pairs - ticks and pointer to the corresponding 
/// material. The simplification is determined by value of threshold
/// (in units of radiation length)   
///

unsigned int LVolume::intersectLine( const HepPoint3D         & Point         , // initial point at the line 
                                     const HepVector3D        & Vector        , // direction vectot of the line 
                                     ILVolume::Intersections  & intersections , // output container  
                                     ISolid::Tick               tickMin       , // minimum value of possible Tick
                                     ISolid::Tick               tickMax       , // maximum value of possible Tick
                                     const double               Threshold     ) // threshold value 
{
  /// useful type definition 
  typedef std::vector<ILVolume::Interval>    Intervals; 
  /// clear the output container 
  intersections.clear();
  /// check the valid tick values 
  if( tickMin >= tickMax ) { return 0 ;} 
  /// line with null direction vector is not able to intersect any volume 
  if( Vector.mag2() <= 0 )                                       { return 0 ; }       // RETURN !!! 
  /// length of tick unit 
  const ISolid::Tick TickLength = Vector.mag(); 
  /// assertion for material and solid 
  Assert( 0 != material() , "LVolume::intersectLine, material()==NULL for LVolume="+fullpath() ); 
  Assert( 0 != solid()    , "LVolume::intersectLine, solid   ()==NULL for LVolume="+fullpath() ); 
  /// define "top"
  const ISolid* pSolid = ( "SolidUnion"  == solid()->typeName() ) ? solid() : solid()->coverTop() ; 
  ///  contribution of this volume is estimated in 3 steps:
  bool useThisVolume      = true  ; 
  /// (1) estimate the maximal possible constribution of this volume using tickMin and tickMax 
  if( Threshold > 0  &&  
      ( tickMax - tickMin )  * TickLength  <= Threshold * material()->radiationLength() ) { useThisVolume = false; }  /// NB!!!! 
  ISolid::Ticks           ticks;   
  /// If threshold is given, before performing the accurate calculations, perform the rought estimation 
  /// using the coverTop() method!  
  /// Line MUST intersect the coverTop() solid at least in 2 points!
  /// no common points with cover TopSolid within range from tickMin to tickMax 
  if( useThisVolume || solid() == pSolid ) 
    {
      if( pSolid->intersectionTicks( Point , Vector , tickMin , tickMax , ticks ) < 2 ) { return 0; } // RETURN!!! 
      /// redefine tickMin and tickMax   
      tickMin = *std::min_element( ticks.begin() , ticks.end() ); 
      tickMax = *std::max_element( ticks.begin() , ticks.end() ); 
    }  
  ///
  Intervals               intervals; 
  if( ( useThisVolume && Threshold > 0 ) || solid() == pSolid ) 
    { 
      /// estimate the radiation thickness of the coverTop volume and construct intervals 
      intervals.clear(); 
      VolumeIntersectionIntervals::TicksToIntervals( ticks , std::back_inserter( intervals ) );
      /// the total length of intervals in Tick units 
      double Length = std::accumulate( intervals.begin() , intervals.end() ,                  // what to accumulate?
                                       0.0               ,                                    // initial value 
                                       VolumeIntersectionIntervals::AccumulateIntervals() );  // "accumulator"
      /// this volume is not to be used for estimations of radiation thickness 
      if( Length * TickLength <= Threshold * material()->radiationLength() ) 
        { 
          useThisVolume = false ;   
          intervals.clear(); 
          ticks.clear(); 
        }
    }
  /// if this volume is to be used,  find intersections with REAL solid (not cover) 
  /// no intersections with own solid ( at least 2 points are required! )
  if( useThisVolume && solid() != pSolid ) 
    {
      /// no intersections with own solid ( at least 2 points are required! )
      /// (sor solidUnion we have already calculated ticks!
      if( 2 > solid()->intersectionTicks( Point , Vector , tickMin , tickMax , ticks ) ) { return 0; }      // RETURN !!! 
      /// check for tickMin and tickMax 
      /// remove extra ticks from container
      /// refine tickMin and tickMax
      tickMin = *std::min_element( ticks.begin() , ticks.end() ); 
      tickMax = *std::max_element( ticks.begin() , ticks.end() ); 
      /// estimate more carefuly its contribution to the total radiation thickness and construct intervals!
      intervals.clear(); 
      VolumeIntersectionIntervals::TicksToIntervals( ticks , std::back_inserter( intervals ) );  
      /// the total length of intervals in Tick units 
      double Length = std::accumulate( intervals.begin() , intervals.end() ,                 // what to accumulate?
                                       0.0               ,                                   // initial value 
                                       VolumeIntersectionIntervals::AccumulateIntervals() ); // "accumulator"
      /// this volume is not to be used for estimations of radiation thickness 
      if( Length * TickLength < Threshold * material()->radiationLength() ) 
        { 
          ticks.clear()         ; 
          intervals.clear()     ;
          useThisVolume = false ; 
        }  
    }
  /// look for the intersections of the given line with daughter elements 
  /// construct the intersections container for daughter volumes
  ILVolume::Intersections childIntersections; 
  if( pvBegin() != pvEnd() ) {
    //unsigned int nIntDaug = 
    intersectDaughters( Point , Vector , childIntersections , tickMin , tickMax , Threshold );
  }
  /// transform container of its own intervals into own intersection container 
  ILVolume::Intersections ownIntersections;
  for( Intervals::const_iterator it = intervals.begin() ; intervals.end() != it ; ++it ) 
    { ownIntersections.push_back( ILVolume::Intersection( *it , material() ) ) ; }    
  /// here we'd like to fill the output container:
  if( ownIntersections.empty() )    // the parent container is empty
    {
      /// we have only child container - just copy it to the output 
      std::copy( childIntersections.begin()          , 
                 childIntersections.end  ()          , 
                 std::back_inserter( intersections ) ) ;
      return intersections.size();                                               // RETURN!!!
    }
  else   /// own container is NOT empty!
    {
      StatusCode sc = 
        VolumeIntersectionIntervals::MergeOwnAndChildContainers( ownIntersections                    , 
                                                                 childIntersections                  ,
                                                                 std::back_inserter( intersections ) );
      Assert( sc.isSuccess() , 
              "LVolume::intersectLine::(2) fatal error in Geometry for lvolume="+fullpath() , sc );
    }
  ///  
  return intersections.size(); 
  ///
};
/// Auxillary method  to calculate intersections with daughter volumes  
unsigned int  LVolume::intersectDaughters( const HepPoint3D&        Point              , 
                                           const HepVector3D&       Vector             , 
                                           ILVolume::Intersections& childIntersections , 
                                           const ISolid::Tick       tickMin            , 
                                           const ISolid::Tick       tickMax            , 
                                           const double             Threshold          )
{
  ///
  if( pvEnd() ==  pvBegin() ) { return 0; }                                                      /// RETURN!!!
  ///
  ILVolume::Intersections child; 
  for( ILVolume::PVolumes::iterator it = pvBegin() ; pvEnd() != it ; ++it ) 
    { 
      IPVolume* pv = *it; 
      Assert( 0 != pv , "LVolume::intersect line, IPVolume==NULL for "+fullpath()); 
      /// construct the intersections container for each daughter volumes 
      pv->intersectLine( Point, Vector , child , tickMin , tickMax , Threshold ) ;  
      /// merge individidual containers and clear child container 
      std::copy( child.begin() , child.end() , std::back_inserter( childIntersections ) ) ; child.clear();
    }
  ///
  std::sort( childIntersections.begin() , childIntersections.end() , 
             VolumeIntersectionIntervals::CompareIntersections() ); 
  ///
  return childIntersections.size(); 
  ///
};
/// Auxillary method  to calculate intersections with daughter volumes  
unsigned int  LVolume::intersectDaughters( const HepPoint3D&        Point              , 
                                           const HepVector3D&       Vector             , 
                                           ILVolume::Intersections& childIntersections , 
                                           const double             Threshold          )
{
  ///
  if( pvEnd() ==  pvBegin() ) { return 0; }                                                      /// RETURN!!!
  ///
  ILVolume::Intersections child; 
  for( ILVolume::PVolumes::iterator it = pvBegin() ; pvEnd() != it ; ++it ) 
    { 
      IPVolume* pv = *it; 
      Assert( 0 != pv , "LVolume::intersect line, IPVolume==NULL for "+fullpath()); 
      /// construct the intersections container for each daughter volumes 
      pv->intersectLine( Point, Vector , child , Threshold ) ;  
      /// merge individidual containers and clear child container 
      std::copy( child.begin() , child.end() , std::back_inserter( childIntersections ) ) ; child.clear();
    }
  ///
  std::sort( childIntersections.begin() , childIntersections.end() , 
             VolumeIntersectionIntervals::CompareIntersections() ); 
  ///
  return childIntersections.size(); 
  ///
};
/// 

#endif   //  __DETDESC_VOLUMES_LVOLUMEINTERSECTLINE_H__ 
