// $Id: TransportSvcDistanceInRadUnits.h,v 1.3 2002-04-03 11:01:45 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef    __DETDESC_TRANSPORTSVC_TRASNPORTSVCDISTANCEINRADUNITS_H__
#define    __DETDESC_TRANSPORTSVC_TRASNPORTSVCDISTANCEINRADUNITS_H__ 1

// local 
#include "TransportSvc.h" 

/** @file TransportSvcDistanceInRaDUnits.h
 * 
 * simple implementation of  TrnasportSvc::distanceInRadUnits method 
 * @see TransportSvc 
 * @see ITransportSvc 
 * 
 * @author: Vanya Belyaev
 */

/** Estimate the distance between 2 points in 
 *  units of radiation length units 
 *  @see ITransportSvc 
 *  @param Point1 first point
 *  @param Point2 second point 
 *  @param Threshold threshold value 
 *  @param AlternativeGeometry  source ot alternative geomtry 
 *  @param GeometryGuess guess foe geometry
 *  @return distance in rad length units 
 */
inline double TransportSvc::distanceInRadUnits
( const HepPoint3D& point1              ,  
  const HepPoint3D& point2              ,  
  double            threshold           ,  
  IGeometryInfo*    alternativeGeometry ,  
  IGeometryInfo*    geometryGuess       )  
{
  /// check for the  distance 
  if( point1 == point2 ) { return 0 ;} 
  ///

  /// retrieve the history 
  HepVector3D Vector( point2 - point1 ) ; 

  /// initial point on the line 
  /// direction vector of the line 
  /// minimal value of the parameter of the line  
  /// maximal value of the parameter of the line 
  /// (output) container of intersections 
  /// threshold value 
  /// source of the alternative geometry information 
  /// a guess for navigation
  intersections( point1              ,   
                 Vector              ,   
                 0.0                 ,   
                 1.0                 ,   
                 m_local_intersects  ,   
                 threshold           ,
                 alternativeGeometry ,
                 geometryGuess           
                 ); 
  

  ///  radiation length in tick units 
  double RadLength = 
    std::accumulate
    (  m_local_intersects.begin()                              ,  
       m_local_intersects.end  ()                              , 
       0.0                                                     ,  
       VolumeIntersectionIntervals::AccumulateIntersections()  ); 
  ///
  const ISolid::Tick TickLength = Vector.mag(); 
  ///
  return RadLength * TickLength ;  
  ///  
};

// ============================================================================
// The End 
// ============================================================================
#endif  // __DETDESC_TRANSPORTSVC_TRASNPORTSVCDISTANCEINRADUNITS_H__
// ============================================================================
