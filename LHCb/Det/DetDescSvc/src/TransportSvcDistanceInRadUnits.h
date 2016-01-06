#ifndef    __DETDESC_TRANSPORTSVC_TRASNPORTSVCDISTANCEINRADUNITS_H__
#define    __DETDESC_TRANSPORTSVC_TRASNPORTSVCDISTANCEINRADUNITS_H__ 1

// DetDesc
#include "DetDesc/VolumeIntersectionIntervals.h"

// local 
#include "TransportSvc.h" 

// ============================================================================
/** @file 
 * 
 * simple implementation of  TrnasportSvc::distanceInRadUnits method 
 * @see TransportSvc 
 * @see ITransportSvc 
 * 
 * @author: Vanya Belyaev
 */
// ============================================================================

// ============================================================================
/** Estimate the distance between 2 points in 
 *  units of radiation length units 
 *  @see ITransportSvc 
 *  @param point1 first point
 *  @param point2 second point 
 *  @param threshold threshold value 
 *  @param alternativeGeometry  source ot alternative geomtry 
 *  @param geometryGuess guess for geometry
 *  @return distance in rad length units 
 */
// ============================================================================
inline double TransportSvc::distanceInRadUnits
( const Gaudi::XYZPoint& point1         ,  
  const Gaudi::XYZPoint& point2         ,  
  double            threshold           ,  
  IGeometryInfo*    alternativeGeometry ,  
  IGeometryInfo*    geometryGuess       )  const
{
  // check for the  distance 
  if( point1 == point2 ) { return 0 ;} 

  // retrieve the history 
  Gaudi::XYZVector Vector( point2 - point1 ) ;
  // initial point on the line 
  // direction vector of the line 
  // minimal value of the parameter of the line  
  // maximal value of the parameter of the line 
  // (output) container of intersections 
  // threshold value 
  // source of the alternative geometry information 
  // a guess for navigation
  intersections( point1              ,   
                 Vector              ,   
                 0.0                 ,   
                 1.0                 ,   
                 m_local_intersects  ,   
                 threshold           ,
                 alternativeGeometry ,
                 geometryGuess       ); 
  
  //  radiation length in tick units 
  double RadLength = 
    std::accumulate
    (  m_local_intersects.begin()                              ,  
       m_local_intersects.end  ()                              , 
       0.0                                                     ,  
       VolumeIntersectionIntervals::AccumulateIntersections()  ); 

  // scale
  const ISolid::Tick TickLength = std::sqrt( Vector.mag2() ); 
  
  return RadLength * TickLength ;  
  ///  
}

// ============================================================================
// The End 
// ============================================================================
#endif  // __DETDESC_TRANSPORTSVC_TRASNPORTSVCDISTANCEINRADUNITS_H__
// ============================================================================
