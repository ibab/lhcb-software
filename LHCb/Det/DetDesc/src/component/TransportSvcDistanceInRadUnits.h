// $Id: TransportSvcDistanceInRadUnits.h,v 1.7 2005-12-05 16:18:44 jpalac Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2005/12/02 18:36:56  jpalac
//
// ! 2005-02-12 - Juan Palacios
//  - Add Kernel/LHCbDefinitions to requirements
//  - Change CLHEP geometry classes to LHCbDefinitions typedefs:
//             * These typedefs point to MathCore classes with the
//               exception of Gaudi::Plane3D, which pointe to HepPoint3D.
//               Awaiting implementation of MathCore Plane3D class.
//  - Make changes to all code to adapt to MathCore syntax
//  - TO-DO: Not compiling due to Plane3D operaitons with MathCore classes
//
// Revision 1.5  2002/07/11 07:15:05  ibelyaev
//  fix the problems with virtual calls inside constructors
//
// ============================================================================
#ifndef    __DETDESC_TRANSPORTSVC_TRASNPORTSVCDISTANCEINRADUNITS_H__
#define    __DETDESC_TRANSPORTSVC_TRASNPORTSVCDISTANCEINRADUNITS_H__ 1

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
 *  @param Point1 first point
 *  @param Point2 second point 
 *  @param Threshold threshold value 
 *  @param AlternativeGeometry  source ot alternative geomtry 
 *  @param GeometryGuess guess for geometry
 *  @return distance in rad length units 
 */
// ============================================================================
inline double TransportSvc::distanceInRadUnits
( const Gaudi::XYZPoint& point1              ,  
  const Gaudi::XYZPoint& point2              ,  
  double            threshold           ,  
  IGeometryInfo*    alternativeGeometry ,  
  IGeometryInfo*    geometryGuess       )  
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
};

// ============================================================================
// The End 
// ============================================================================
#endif  // __DETDESC_TRANSPORTSVC_TRASNPORTSVCDISTANCEINRADUNITS_H__
// ============================================================================
