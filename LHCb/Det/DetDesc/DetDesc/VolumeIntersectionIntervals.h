// $Id: VolumeIntersectionIntervals.h,v 1.13 2004-07-09 14:19:42 cattanem Exp $ 
#ifndef       DETDESC_VOLUMEINTERSECTIONIINTERVALS_H
#define       DETDESC_VOLUMEINTERSECTIONIINTERVALS_H 
// STD & STL  
#include <algorithm>
#include <functional>
#include <numeric>
// GaudiKernel
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
// DetDesc 
#include "DetDesc/DetDesc.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"


/** @file 
 * 
 *  a collection of useful technical methods 
 *  for manipulation of 'Intersection' and 'Intervals' 
 *  @see ILVolume
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author Sebastien Ponce
 */

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

/** @namespace VolumeIntersectionIntervals 
 *  collection of useful methods to deal with intersection and intervals
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
namespace  VolumeIntersectionIntervals
{
  
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
  };
  
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

  /** helpful method of merging 2 containers 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @param  own container of 'own' intersections
   *  @param  child container of 'child' intersections 
   *  @param  out  ??
   *  @return status code 
   */
  template < class OUTPUTTYPE > 
  inline  StatusCode MergeOwnAndChildContainers
  ( const ILVolume::Intersections& own   , 
    const ILVolume::Intersections& child ,
    OUTPUTTYPE                     out   ) 
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
              << intervalTop.second << endreq;
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
              << mostRightTick << endreq;
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
    { return i1.first.first < i2.first.first; };
  };
  
}; // end of namespace 


// ============================================================================
// The End 
// ============================================================================
#endif   //   __DETDESC_VOLUMES_VOLUMEINTERSECTIONIINTERVALS_H__
// ============================================================================
