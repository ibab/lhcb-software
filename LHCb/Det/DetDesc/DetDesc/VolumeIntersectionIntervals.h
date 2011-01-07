// $Id: VolumeIntersectionIntervals.h,v 1.15 2009-04-17 08:54:24 cattanem Exp $ 
// ==============================================================================
#ifndef       DETDESC_VOLUMEINTERSECTIONIINTERVALS_H
#define       DETDESC_VOLUMEINTERSECTIONIINTERVALS_H 
// ==============================================================================
// STD & STL  
// ==============================================================================
#include <algorithm>
#include <functional>
#include <numeric>
// ==============================================================================
// GaudiKernel
// ==============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
// ==============================================================================
// DetDesc 
// ==============================================================================
#include "DetDesc/DetDesc.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/Compare.h"
#include "DetDesc/IntersectionErrors.h"
// ==============================================================================
/** @file 
 * 
 *  a collection of useful technical methods 
 *  for manipulation of 'Intersection' and 'Intervals' 
 *  @see ILVolume
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author Sebastien Ponce
 */
// ==============================================================================
#ifdef __INTEL_COMPILER         // Disable ICC remark
  #pragma warning(disable:1572) // Floating-point equality and inequality comparisons are unreliable
#endif
/// boolean operation for intervals  
inline bool operator< ( const ILVolume::Interval& Int , double Length ) 
{ return ( Int.second - Int.first ) <  Length ; }  
/// boolean operation for intervals  
inline bool operator==( const ILVolume::Interval& Int , double Length ) 
{ return ( Int.second - Int.first ) == Length ; }
/// boolean operation for intervals  
inline bool operator> ( const ILVolume::Interval& Int , double Length ) 
{ return ( Int.second - Int.first ) >  Length ; }  
/// boolean operation for intervals 
inline bool operator<=( const ILVolume::Interval& Int , double Length ) 
{ return ( Int.second - Int.first ) <= Length ; }  
/// boolean operation for intervals 
inline bool operator>=( const ILVolume::Interval& Int , double Length ) 
{ return ( Int.second - Int.first ) >= Length ; }  
/// boolean operation for intervals 
inline bool operator!=( const ILVolume::Interval& Int , double Length ) 
{ return ( Int.second - Int.first ) != Length ; }  


/// boolean operations for Intersections  
inline bool operator< ( const ILVolume::Intersection& Int , double RadLength ) 
{ return ( ( 0 == Int.second ) ? true  :  
           Int.first <  RadLength * ( Int.second->radiationLength() ) ) ; } 

/// boolean operations for Intersections  
inline bool operator<=( const ILVolume::Intersection& Int , double RadLength ) 
{ return ( ( 0 == Int.second ) ? true  :  
           Int.first <= RadLength * ( Int.second->radiationLength() ) ) ; } 

/// boolean operations for Intersections  
inline bool operator> ( const ILVolume::Intersection& Int , double RadLength ) 
{ return ( ( 0 == Int.second ) ? false :  
           Int.first >  RadLength * ( Int.second->radiationLength() ) ) ; } 

/// boolean operations for Intersections  
inline bool operator>=( const ILVolume::Intersection& Int , double RadLength ) 
{ return ( ( 0 == Int.second ) ? false :  
           Int.first >= RadLength * ( Int.second->radiationLength() ) ) ; } 

/// boolean operations for Intersections  
inline bool operator==( const ILVolume::Intersection& Int , double RadLength ) 
{ return ( ( 0 == Int.second ) ? ( ( 0 == RadLength ) ? true : false )  :  
           Int.first >= RadLength * ( Int.second->radiationLength() ) ) ; }

// ============================================================================
/** @namespace VolumeIntersectionIntervals 
 *  collection of useful methods to deal with intersection and intervals
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
namespace  VolumeIntersectionIntervals
{
  // ==========================================================================
  /// check the intersection 
  inline int intersect 
  ( const ILVolume::Interval& i1 , 
    const ILVolume::Interval& i2 ) 
  {
    const int res1 = DetDesc::compare ( i1.second , i2.first ) ;
    if ( ! ( 0 < res1 ) ) { return -1 ; }                         // RETURN   
    const int res2 = DetDesc::compare ( i2.second , i1.first ) ;    
    if ( ! ( 0 < res2 ) ) { return  1 ; }                         // RETURN 
    return 0 ;                                                    // RETURN 
  }
  // ==========================================================================
  /// check the intersection 
  inline int intersect 
  ( const ILVolume::Intersection& i1 , 
    const ILVolume::Intersection& i2 ) 
  {
    return intersect ( i1.first , i2.first ) ;
  }
  // ==========================================================================
  /** helpful method to decode the ticks sequence 
   *  into sequence of intervals 
   *  return the number of intervals 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @param ticks input container of Ticks 
   *  @param out   output iterator for container of "Intervals"
   */
  template < class OUTPUTTYPE >
  inline  unsigned int TicksToIntervals 
  ( const ISolid::Ticks & ticks ,  // input "ticks", assumed to be sorted!
    OUTPUTTYPE            out   ) 
  {
    // interval can be constructed from at least 2 ticks!
    if( ticks.size() < 2 ) { return 0 ; } // RETURN!
    
    unsigned int res = 0 ;
    
    ISolid::Ticks::const_iterator it = ticks.begin();
    ISolid::Tick  tickPrevious = *it++;   // size tested. this is safe
    while( ticks.end() != it ) {
      ISolid::Tick tickCurrent = *it++;
      if( tickCurrent > tickPrevious ) {
        ++res;
        *out++ = ILVolume::Interval( tickPrevious , tickCurrent ) ;
      }
      if ( ticks.end() != it ) { tickPrevious = *it++; }
    }
    ///
    return res; 
  }
  // ==========================================================================  
  /** @class AccumulateIntervals 
   *  accumulation utility to accumulate the total length of intervals 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   */
  class AccumulateIntervals:  
    public std::binary_function<double,const ILVolume::Interval,double>
  {
  public: 
    inline double  operator() 
      ( double& Length  , const ILVolume::Interval& interval ) const  
    { return Length += (interval.second-interval.first); }  
  };
  // ==========================================================================
  /** @class AccumulateIntersections
   *  accumulation utility to accumulate the total length of intersections 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   */
  class AccumulateIntersections:  
    public std::binary_function<double,const ILVolume::Intersection,double>
  {
  public: 
    inline double  operator() 
      ( double& Length  , const ILVolume::Intersection& intersection ) const  
    {
      const  Material*  mat          = intersection.second;     
      const  ILVolume::Interval& Int = intersection.first;   
      return 
        ( Length += ( ( 0 == mat ) ? 0 : 
                      (Int.second-Int.first) / mat->radiationLength() ) ) ; 
    }
  };
  // ==========================================================================
  /** helpful method of merging 2 containers 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @param  own container of 'own' intersections
   *  @param  child container of 'child' intersections 
   *  @param  out  ??
   *  @return status code 
   */
  template < class OUTPUTTYPE > 
  inline  StatusCode MergeOwnAndChildContainers
  ( const ILVolume::Intersections&   own       , 
    const ILVolume::Intersections&   child     ,
    OUTPUTTYPE                       out       , 
    const ILVolume*               /* volum */  , 
    const double                  /* length */ )    
  {
    /** here we have both containers - own and child containers
     *  try to merge the containers
     */
    typedef std::vector<ILVolume::Intersections::size_type> IndexCont ;
    typedef ILVolume::Intersections::const_iterator         Iter      ; 
    typedef ISolid::Tick                                    Tick      ;
    typedef ILVolume::Interval                              Interval  ; 
    const double tol = 1.e-6;  // Needed to test equalities...
    ///
    ///
    for( Iter iterTop = own.begin(); own.end() != iterTop ; ++iterTop ) {
      const Interval& intervalTop = iterTop->first  ;
      const Material* matTop      = iterTop->second ;
      // temporary container of indexes of related intervals 
      IndexCont tmpIndex; 
      for( Iter iter = child.begin();  child.end() != iter ; ++iter ) {
        const Interval& intervalLoc = iter->first;
        
        if( intervalLoc.second <= (intervalTop.first+tol)  ) {
          ; // the second interval is lower  then the first one! it's OK
        } else if( intervalTop.first  <= (intervalLoc.first+tol) && 
                   intervalLoc.second <= (intervalTop.second+tol) ) {
          // the second interval is inside      the first one! it's OK
          tmpIndex.push_back( iter - child.begin() );
        } else if( intervalTop.second <= (intervalLoc.first+tol)  ) {
          ; // the second interval in higher then the first one! it's OK 
        } else {
          /** this situation means that something wrong
           *  with geometry description!!!
           */
          DetDesc::Services* services = DetDesc::services();
          MsgStream log (services->msgSvc() , "TransportSvc");
          log << MSG::ERROR
              << "VolumeIntersection:Merge error 15 : interval " 
              << intervalLoc.first << " "
              << intervalLoc.second << " master "
              << intervalTop.first << " " 
              << intervalTop.second << endmsg;
          services->release();
          return StatusCode(15) ;
        }
        // ? RETURN !!!
      }  // end of loop over the child container 
      /// 
      // try to merge intervals 
      Tick leftTick       = intervalTop.first  ; 
      Tick mostRightTick  = intervalTop.second ;
      const  Material*  prevMatLocal = 0;
      for( IndexCont::const_iterator it = tmpIndex.begin(); 
           tmpIndex.end() != it ; ++it ) { 
        Iter              iterLocal     = child.begin() + (*it) ; 
        const  Interval&  intervalLocal = iterLocal->first  ;
        const  Material*  matLocal      = iterLocal->second ;
        /// 
        if( leftTick <= (intervalLocal.first + tol) && 
            intervalLocal.first < mostRightTick ) {
          if( leftTick <= (intervalLocal.first - tol) ) { 
            *out++ =  ILVolume::Intersection( Interval( leftTick , 
                                                        intervalLocal.first ), 
                                              matTop  ) ; 
          }
          leftTick = intervalLocal.first;
                
          if( intervalLocal.second <= (mostRightTick+tol) ) { 
            *out++ = ILVolume::Intersection( Interval( leftTick , 
                                                       intervalLocal.second), 
                                             matLocal ) ; 
          } else { // geometry error!!!
            return StatusCode(16) ; 
          }      // RETURN !!!

          //=== This is a hack, to accept overlaping volumes with same
          //=== material without complaining...
        } else if ( (matLocal == prevMatLocal) && 
                    (intervalLocal.second <= (mostRightTick+tol) ) ) {
          if ( intervalLocal.second > leftTick  ) {
            *out++ = ILVolume::Intersection( Interval( leftTick , 
                                                       intervalLocal.second), 
                                             matLocal ) ; 
          }
        } else {                                 // geometry error!!!
          DetDesc::Services* services = DetDesc::services();
          MsgStream log (services->msgSvc() , "TransportSvc");
          log << MSG::ERROR
              << "VolumeIntersection:Merge error 17 : interval " 
              << intervalLocal.first << " "
              << intervalLocal.second << " master "
              << leftTick << " " 
              << mostRightTick << endmsg;
          services->release();
          return StatusCode(17) ; 
        }         // RETURN !!!
        leftTick     = intervalLocal.second;
        prevMatLocal = matLocal;
      }  // end of loop over temporary index container 

      if( leftTick <= (mostRightTick - tol) ) { 
        *out++ = ILVolume::Intersection( Interval( leftTick , mostRightTick ) ,
                                         matTop  ) ; 
      } 
      leftTick = mostRightTick;    
    }  // end of loop over own intervals
    
    /// return status 
    return StatusCode::SUCCESS;
  }
  // ==========================================================================
  /** helpful method of merging 2 containers 
   *  @author Vanya Belyaev ibelyaev@physics.syr.edu
   *  @param  own container of 'own' intersections
   *  @param  child container of 'child' intersections 
   *  @param  out  ??
   *  @return status code 
   */
  template < class OUTPUTTYPE > 
  inline  StatusCode 
  MergeOwnAndChildContainers2
  ( const ILVolume::Intersections& own    , 
    const ILVolume::Intersections& child  ,
    OUTPUTTYPE                     out    , 
    const ILVolume*                volume , 
    const double                   length ) 
  {
    /*  here we have both containers - own and child containers
     *  try to merge the containers
     */
    typedef std::vector<ILVolume::Intersections::size_type> IndexCont ;
    typedef ILVolume::Intersections::const_iterator         Iter      ; 
    typedef ISolid::Tick                                    Tick      ;
    typedef ILVolume::Interval                              Interval  ; 
    // loop over all "own" intervals 
    for ( Iter iterTop = own.begin(); own.end() != iterTop ; ++iterTop ) 
    {
      const Interval& intervalTop = iterTop->first  ;
      const Material* matTop      = iterTop->second ;
      // temporary container of indexes of related intervals 
      IndexCont tmpIndex; 
      for ( Iter iter = child.begin();  child.end() != iter ; ++iter ) 
      {
        const Interval& intervalLoc = iter->first ;
        
        const int result = VolumeIntersectionIntervals::intersect 
          ( intervalTop , intervalLoc ) ;
        
        if     ( 0 != result ) { /* it is OK */ }
        else 
        {
          // here we have either GOOD case or geometry error , 
          // keep them togather for a moment 
          tmpIndex.push_back ( iter - child.begin() ); 
        }
      }  
      // end of loop over the child container 
      //  
      // try to merge intervals 
      Tick leftTick       = intervalTop.first  ; 
      Tick mostRightTick  = intervalTop.second ;
      
      for ( IndexCont::const_iterator it = tmpIndex.begin(); 
            tmpIndex.end() != it ; ++it ) 
      { 
        Iter              iterLocal     = child.begin() + (*it) ; 
        const  Interval&  intervalLocal = iterLocal->first  ;
        const  Material*  matLocal      = iterLocal->second ;
        /// ignore the child with the same material : ?
        if ( matLocal == matTop ) { continue ; }
        //
        const int rL = DetDesc::compare ( leftTick , intervalLocal.first       ) ;
        const int rR = DetDesc::compare ( intervalLocal.second , mostRightTick ) ;
        
        if ( rL <= 0 && rR <=0 )
        {
          // 
          if      ( rL < 0 ) 
          {
            *out++ = ILVolume::Intersection 
              ( ILVolume::Interval ( leftTick , intervalLocal.first )  , matTop    ) ;
            leftTick = intervalLocal.first ;
          }
          //
          if      ( rR < 0 ) 
          {
            *out++ = ILVolume::Intersection 
              ( ILVolume::Interval ( leftTick , intervalLocal.second ) , matLocal ) ;
            leftTick = intervalLocal.second ;
          }
          else if ( rR ==0 ) 
          {
            *out++ = ILVolume::Intersection 
              ( ILVolume::Interval ( leftTick , mostRightTick )        , matLocal ) ;
            leftTick = mostRightTick ;
          }
        }
        else 
        { 
          /* here we have real problems */
          
          /* the problem is serious, but we have some guess how to solve it */
          if ( DetDesc::IntersectionErrors::recovery() ) 
          {
            // try to recover (1) 
            if ( 0 < rL && rR < 0 ) 
            {
              *out++ = ILVolume::Intersection 
                ( ILVolume::Interval ( leftTick , intervalLocal.second ) , matLocal ) ;
              // report the problem 
              DetDesc::IntersectionErrors::recovered 
                ( volume , matTop , matLocal , 
                  length * ( leftTick - intervalLocal.first  ) ) ;
              //
              leftTick = intervalLocal.second ;
            }
            // try to recover (2) 
            else if ( 0 < rL && rR == 0 ) 
            {
              *out++ = ILVolume::Intersection 
                ( ILVolume::Interval ( leftTick , mostRightTick  ) , matLocal ) ;
              //
              // report the problem 
              DetDesc::IntersectionErrors::recovered 
                ( volume , matTop , matLocal , 
                  length * ( leftTick - intervalLocal.first ) ) ;
              //
              leftTick = mostRightTick  ;
            }
            // try to recover (3) 
            else if ( rL <= 0 && 0 < rR  ) 
            {
              if ( rL < 0 ) 
              {
                *out++ = ILVolume::Intersection 
                  ( ILVolume::Interval ( leftTick , intervalLocal.first ) , matTop ) ;
                leftTick = intervalLocal.first ;
              }
              // 
              *out++ = ILVolume::Intersection 
                ( ILVolume::Interval ( leftTick , mostRightTick  ) , matLocal ) ;
              //
              // report the problem 
              DetDesc::IntersectionErrors::recovered 
                ( volume , matTop , matLocal , 
                  length * ( intervalLocal.second - mostRightTick ) )  ;
              //
              leftTick = mostRightTick ;
            }
            else if ( rL > 0 && rR > 0 ) 
            {
              *out++ = ILVolume::Intersection 
                ( ILVolume::Interval ( leftTick , mostRightTick) , matLocal ) ;
              //
              // report the problem 
              DetDesc::IntersectionErrors::recovered 
                ( volume , matTop ,  matLocal ,
                  length * ( intervalLocal.second - mostRightTick       +
                             leftTick             - intervalLocal.first ) ) ;
              //
              leftTick = mostRightTick ;
            }
          } // recover allowed? 
          else 
          {
            DetDesc::IntersectionErrors::setCode ( 17 , volume ) ;
            return StatusCode ( 17 ) ;
          }
        } // geometry problmes
      }// end of loop over the temporary index container
      // the last intersection 
      const int rF = DetDesc::compare ( leftTick , mostRightTick ) ;
      if ( rF < 0  ) 
      {
        *out++ = ILVolume::Intersection
          ( ILVolume::Interval ( leftTick , mostRightTick ) , matTop  ) ; 
      }
      // adjuts the left tick 
      leftTick = mostRightTick;    
    }  // end of loop over own intervals
    
    /// return status 
    return StatusCode::SUCCESS;
  }
  // ======================================================================-===
  /** @class CompareIntersections
   * "very specific" comparison for intersections!!!
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   */
  class CompareIntersections : 
    public std::binary_function<const ILVolume::Intersection&,
    const ILVolume::Intersection&,bool>                               
  {
  public:
    inline bool operator() 
      ( const ILVolume::Intersection& i1 , 
        const ILVolume::Intersection& i2 ) const 
    { return i1.first.first < i2.first.first; }
  };
  // ==========================================================================
  /** try to validate&correct the intersections 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-12-13 
   */
  inline int 
  correct  
  ( const ILVolume*          volume , 
    ILVolume::Intersections& cnt    ,
    const double             tick   , 
    unsigned int index =     0      ) 
  {
    // nothing to correct? 
    if ( 2 + index > cnt.size()  ) { return 0 ; } // RETURN
    ILVolume::Intersections::iterator i1 = cnt.begin() + index ;
    // loop over all the intersections    
    for ( ILVolume::Intersections::iterator i2 = i1 + 1 ;
          i2 != cnt.end() ; ++i1 , ++i2 , ++index ) 
    {
      // OPTIONAL CHECK (could be removed) 
      // the intersection of the zero length 
      // check the length of the intersecion:
      const int length = DetDesc::compare ( i1->first.first , i1->first.second ) ;
      // invalid or empty intersection!!! 
      if        ( 0 <  length ) 
      { 
        if (  DetDesc::IntersectionErrors::recovery () ) 
        {        
          DetDesc::IntersectionErrors::skip 
            ( volume , i1->second , 
              tick *  ( i1->first.second - i1->first.first ) ) ;
          cnt.erase( i1 ) ;
          return correct ( volume , cnt , tick , index ) + 1 ;              // RETURN 
        }
        DetDesc::IntersectionErrors::setCode ( 26 , volume ) ;
        return 0 ;
      }
      else if        ( 0 ==  length ) 
      { 
        DetDesc::IntersectionErrors::skip 
          ( volume , i1->second , 0 ) ;
        cnt.erase ( i1 ) ;
        return correct ( volume , cnt , tick , index ) + 1 ;              // RETURN 
      }
      // ======================================================================
      /* check the end point of the first intersection 
       * and the begin point of the second intersection
       */
      const int result = DetDesc::compare ( i1->first.second , i2->first.first ) ;
      // invalid position of the points:
      if ( 0 < result  && i1->second != i2->second ) 
      {
        if ( DetDesc::IntersectionErrors::recovery() ) 
        {
          DetDesc::IntersectionErrors::recovered
            ( volume       , 
              i1 -> second , 
              i2 -> second , 
              tick *  ( i1->first.second - i2->first.first ) ) ;
          const double t1 = 0.5 * ( i1 -> first.second + i1 -> first.first) ;
          i1 -> first.second = t1 ;
          i2 -> first.first  = t1 ;
          return correct ( volume , cnt , tick , index ) + 1 ;              // RETURN 
        }
        DetDesc::IntersectionErrors::setCode ( 27 , volume ) ;
        return 0 ;
      }
      // try to recover this pathological case 
      if ( 0 < result  && i1->second == i2->second  ) 
      {
        if ( DetDesc::IntersectionErrors::recovery() ) 
        {
          DetDesc::IntersectionErrors::recovered
            ( volume       , 
              i1 -> second , 
              i2 -> second , 
              tick *  ( i1->first.second - i2->first.first ) ) ;
          i1->first.second = i2->first.second ;
          cnt.erase ( i2 ) ;
          return correct ( volume , cnt , tick , index ) + 1 ;              // RETURN
        }
        DetDesc::IntersectionErrors::setCode ( 28 , volume ) ;
        return 0 ;
      }
      // the points are "comparable" 
      if ( 0 == result ) 
      {
        // redefine the intersection for the exact match:
        i1 -> first.second = i2->first.first ;
        // but if we have the same material, 
        // we can "merge" both intersection togather:
        if ( i1->second == i2->second ) // the same material!
        {
          i1->first.second = i2->first.second ;
          cnt.erase ( i2 ) ;
          return correct ( volume , cnt , tick , index ) + 1 ;     // RETURN 
        } // the same material 
      } // the points are comparable
    } // loop over intersections
    return 0 ;                                                     // RETURN                                                  
  } // "correct"
  // ==========================================================================  
} // end of namespace
// ============================================================================
// The End 
// ============================================================================
#endif   //   __DETDESC_VOLUMES_VOLUMEINTERSECTIONIINTERVALS_H__
// ============================================================================
