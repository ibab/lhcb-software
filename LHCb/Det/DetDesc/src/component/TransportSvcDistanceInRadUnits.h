#ifndef    __DETDESC_TRANSPORTSVC_TRASNPORTSVCDISTANCEINRADUNITS_H__
#define    __DETDESC_TRANSPORTSVC_TRASNPORTSVCDISTANCEINRADUNITS_H__ 1 


#include "DetDesc/TransportSvc.h" 

/// 
///
/// simple implementation of  TrnasportSvc::distanceInRadUnits method 
///
/// Author: Vanya Belyaev
///
/// 

///
///
///     Estimate the distance between 2 points in units of radiation length units 
///
double TransportSvc::distanceInRadUnits( const HepPoint3D& point1              ,  /// first  point 
                                         const HepPoint3D& point2              ,  /// second point 
                                         double            threshold           ,  /// threshold value 
                                         IGeometryInfo*    alternativeGeometry ,  /// source of alternative geometry information 
                                         IGeometryInfo*    geometryGuess       )  /// a guess for navigation
{
  ///
  ///
  /// check for the  distance 
  if( point1 == point2 ) { return 0 ;} 
  ///

  ///
  /// retrieve the history 
  ///
  HepVector3D Vector( point2 - point1 ) ; 
  //unsigned long nIntrsct = 
  intersections( point1              ,   /// initial point on the line 
                       Vector              ,   /// direction vector of the line 
                 0.0                 ,   /// minimal value of the parameter of the line  
                 1.0                 ,   /// maximal value of the parameter of the line 
                 m_local_intersects  ,   /// (output) container of intersections 
                 threshold           ,   /// threshold value 
                 alternativeGeometry ,   /// source of the alternative geometry information 
                 geometryGuess           /// a guess for navigation
               ); 
  
  /// 
  ///  radiation length in tick units 
  ///
  
  double RadLength = 
    std::accumulate(  m_local_intersects.begin()                              ,  
                      m_local_intersects.end  ()                              , 
                      0.0                                                     ,   /// initial value
                      VolumeIntersectionIntervals::AccumulateIntersections()  );  /// "accumulator"
  ///
  const ISolid::Tick TickLength = Vector.mag(); 
  ///
  return RadLength * TickLength ;  
  ///  
};

///
///
///



#endif  // __DETDESC_TRANSPORTSVC_TRASNPORTSVCDISTANCEINRADUNITS_H__
