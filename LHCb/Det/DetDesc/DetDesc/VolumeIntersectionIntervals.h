#ifndef       __DETDESC_VOLUMES_VOLUMEINTERSECTIONIINTERVALS_H__
#define       __DETDESC_VOLUMES_VOLUMEINTERSECTIONIINTERVALS_H__ 1 

#include <algorithm>
#include <functional>
#include <numeric>

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"

#include "GaudiKernel/ISolid.h"
#include "GaudiKernel/ILVolume.h"
#include "GaudiKernel/IPVolume.h"

class Material;

#include "DetDesc/Material.h"

///
///   VolumeIntersectionIntervals.h : a collection of useful technical methods 
///                                   for manipulation of Intersection Intervals 
///
///

///
///
/// boolean operation for intervals 
inline bool operator< ( const ILVolume::Interval& Int , double Length ) { return ( Int.second - Int.first ) <  Length ; }  
inline bool operator> ( const ILVolume::Interval& Int , double Length ) { return ( Int.second - Int.first ) >  Length ; }  
inline bool operator<=( const ILVolume::Interval& Int , double Length ) { return ( Int.second - Int.first ) <= Length ; }  
inline bool operator>=( const ILVolume::Interval& Int , double Length ) { return ( Int.second - Int.first ) >= Length ; }  
inline bool operator==( const ILVolume::Interval& Int , double Length ) { return ( Int.second - Int.first ) == Length ; }  
inline bool operator!=( const ILVolume::Interval& Int , double Length ) { return ( Int.second - Int.first ) != Length ; }  


///
///
/// boolean operations for Intersections  
inline bool operator< ( const ILVolume::Intersection& Int , double RadLength ) 
{ return ( ( 0 == Int.second ) ? true  :  Int.first <  RadLength * ( Int.second->radiationLength() ) ) ; } 

inline bool operator<=( const ILVolume::Intersection& Int , double RadLength ) 
{ return ( ( 0 == Int.second ) ? true  :  Int.first <= RadLength * ( Int.second->radiationLength() ) ) ; } 

inline bool operator> ( const ILVolume::Intersection& Int , double RadLength ) 
{ return ( ( 0 == Int.second ) ? false :  Int.first >  RadLength * ( Int.second->radiationLength() ) ) ; } 

inline bool operator>=( const ILVolume::Intersection& Int , double RadLength ) 
{ return ( ( 0 == Int.second ) ? false :  Int.first >= RadLength * ( Int.second->radiationLength() ) ) ; } 

inline bool operator==( const ILVolume::Intersection& Int , double RadLength ) 
{ return ( ( 0 == Int.second ) ? ( ( 0 == RadLength ) ? true : false )  :  Int.first >= RadLength * ( Int.second->radiationLength() ) ) ; } 

 
namespace  VolumeIntersectionIntervals
{
  
  ///
  ///
  /// helpful method to decode the ticks sequence into sequence of intervals   
  /// return the number of intervals (normally it is just a number of ticks - 1 )
  template < class OUTPUTTYPE >
    inline  unsigned int TicksToIntervals ( const ISolid::Ticks & ticks ,  // input "ticks", assumed to be sorted!!!! NB!!!
                                            OUTPUTTYPE            out   ) 
    {
      // interval can be constructed from at least 2 ticks!
      if( ticks.size() < 2 ) { return 0 ; }                             // RETURN!
      
      unsigned int res = 0 ; 
      ISolid::Ticks::const_iterator it = ticks.begin(); 
      ISolid::Tick  tickPrevious = *it; 
      while( ticks.end() != it ) 
        {
          ISolid::Tick tickCurrent = *it++; 
          if( tickCurrent > tickPrevious ) { ++res; *out++ = ILVolume::Interval( tickPrevious , tickCurrent ) ; } 
          tickPrevious = tickCurrent; 
        }
      ///
      return res; 
    }
  
  
  ///
  ///
  ///  
  class AccumulateIntervals:  public std::unary_function< const ILVolume::Interval& , double >
    {
    public: 
      inline double  operator() ( double& Length  , const ILVolume::Interval& interval ) const  
        { return Length += (interval.second-interval.first); }  
    };
  
  ///
  ///
  ///
  class AccumulateIntersections:  public std::unary_function< const ILVolume::Intersection& , double >
    {
    public: 
      inline double  operator() ( double& Length  , const ILVolume::Intersection& intersection ) const  
        {
          const  Material*  mat          = intersection.second;     
          const  ILVolume::Interval& Int = intersection.first;   
          return ( Length += ( ( 0 == mat ) ? 0 : (Int.second-Int.first) / mat->radiationLength() ) ) ; 
        }
    };
  
  ///
  ///
  ///
  /// a helpful method of merging 2 containers 
  ///
  template < class OUTPUTTYPE > 
    inline  StatusCode MergeOwnAndChildContainers( const ILVolume::Intersections& own   , 
                                                   const ILVolume::Intersections& child ,
                                                   OUTPUTTYPE                     out   ) 
    {
      /// here we have both containers - own and child containers
      /// try to merge the containers 
      ///
      typedef std::vector<ILVolume::Intersections::size_type>      IndexCont     ;
      typedef ILVolume::Intersections::const_iterator              Iter          ; 
      typedef ISolid::Tick                                         Tick          ;
      typedef ILVolume::Interval                                   Interval      ; 
      ///
      ///
      for( Iter iterTop = own.begin(); own.end() != iterTop ; ++iterTop ) 
        {
          const Interval& intervalTop = iterTop->first  ;
          const Material* matTop      = iterTop->second ;
          ///
          IndexCont tmpIndex; // temporary container of indexes of related intervals 
    for( Iter iter = child.begin();  child.end() != iter ; ++iter )
    {
      const Interval& intervalLoc = iter->first;

      if( intervalLoc.second <= intervalTop.first  ) {
        ; // the second interval is lower  then the first one! it's OK
      } 
      else if( intervalTop.first  <= intervalLoc.first && 
               intervalLoc.second <= intervalTop.second ) {
        // the second interval is inside      the first one! it's OK
        tmpIndex.push_back( iter - child.begin() );
      }
      else if( intervalTop.second <= intervalLoc.first  ) {
        ; // the second interval in higher then the first one! it's OK 
      } 
      else {
        // this situation means that something wrong with geometry description!!!
        return StatusCode(15) ;
      }
      // RETURN !!!
      //                { return StatusCode::FAILURE; }                                                       // RETURN !!!
    }  // end of loop over the child container 
          /// 
          /// try to merge intervals 
          Tick leftTick       = intervalTop.first  ; 
          Tick mostRightTick  = intervalTop.second ;
          for( IndexCont::const_iterator it = tmpIndex.begin(); tmpIndex.end() != it ; ++it ) 
            { 
              Iter              iterLocal     = child.begin() + (*it) ; 
              const  Interval&  intervalLocal = iterLocal->first  ;
              const  Material*  matLocal      = iterLocal->second ;
              /// 
              if( leftTick <= intervalLocal.first && intervalLocal.first < mostRightTick )
                {
                  if( intervalLocal.first != leftTick ) 
                    { *out++ =  ILVolume::Intersection( Interval( leftTick , intervalLocal.first )  , matTop  ) ; }
                  leftTick = intervalLocal.first;

                  if( intervalLocal.second <= mostRightTick ) 
                    { *out++ =  ILVolume::Intersection( Interval( leftTick , intervalLocal.second ) , matLocal ) ; }
                  else                                                                                    // geometry error!!!
                    { return StatusCode(16) ; }                                                       // RETURN !!!
                    //                    { return StatusCode::FAILURE; }                                                       // RETURN !!!
                  leftTick = intervalLocal.second;
                }
              else                                                                                    // geometry error!!!
                { return StatusCode(17) ; }                                                       // RETURN !!!
              //                { return StatusCode::FAILURE; }                                                       // RETURN !!!
            }  // end of loop over temporary index container 
          if( leftTick != mostRightTick ) 
            { *out++ = ILVolume::Intersection( Interval( leftTick , mostRightTick ) , matTop  ) ; } 
          leftTick = mostRightTick;    
        }  // end of loop over own intervals
      
      /// return status 
      return StatusCode::SUCCESS;
    }


  
  ///
  ///
  /// "very specific" comparison for intersections!!!
  ///
  class CompareIntersections : public std::binary_function<const ILVolume::Intersection&,const ILVolume::Intersection&,bool>                               
    {
    public:
      inline bool operator() ( const ILVolume::Intersection& i1 , const ILVolume::Intersection& i2 ) const 
        { return i1.first.first < i2.first.first; };
    };

}; // end of namespace 


///
///
///



#endif   //   __DETDESC_VOLUMES_VOLUMEINTERSECTIONIINTERVALS_H__
