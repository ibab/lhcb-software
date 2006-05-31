// $Id: SolidIntersections.h,v 1.1 2006-05-31 13:38:31 jpalac Exp $
#ifndef KERNEL_SOLIDINTERSECTIONS_H 
#define KERNEL_SOLIDINTERSECTIONS_H 1

// Include files

/** @file SolidIntersections.h Kernel/SolidIntersections.h
 *  
 *
 *  @author Juan PALACIOS
 *  @todo   Decide whether to keep in this package. If needed migrate implementations.
 *  @date   2006-05-02
 */
namespace Gaudi {

  namespace Math 
  {
    
    /** find intersection ticks for the line parametrized as 
     *  beginPoint + direction() * Tick with sphere of radius sqrt(radius)
     *  @author         Vanya Belyaev   Ivan.Belyaev@itep.ru 
     *  @date           10.02.2000 
     *  @param line     line satisfying line.beginPoint() + scalar*line.direction() 
     *  @param radius2  sphere radius squared 
     *  @param out      output iterator 
     *  @return         number of intersections (==1)
     */
    template <typename ITERATOR, typename aLine>
    unsigned int intersectionLineSphere( const aLine& line,
                                         const double radius2,
                                         ITERATOR out );
    
    /** find intersection ticks for the line parametrized as 
     *  beginPoint + direction() * Tick with cylinder of radius sqrt(radius)
     *  @author         Vanya Belyaev   Ivan.Belyaev@itep.ru 
     *  @date           10.02.2000 
     *  @param line     line satisfying line.beginPoint() + scalar*line.direction() 
     *  @param radius2  cylinder radius squared 
     *  @param out      output iterator 
     *  @return         number of intersections (==1)
     */
    template <typename ITERATOR, typename aLine>
    unsigned int intersectionLineCylinder( const aLine& line,
                                           const double radius2,
                                           ITERATOR out );


    /** find intersection ticks for the line parametrized as 
     *  beginPoint + direction() * Tick with plane x=X
     *  @author       Vanya Belyaev   Ivan.Belyaev@itep.ru 
     *  @date         10.02.2000 
     *  @param line     line satisfying line.beginPoint() + scalar*line.direction() 
     *  @param X      x-parameter
     *  @param out    output iterator 
     *  @return       number of intersections (==1)
     */
    template <typename ITERATOR, typename aLine>
    unsigned int intersectionLinePlaneX( const aLine& line,
                                         const double X,
                                         ITERATOR out );

    /** find intersection ticks for the line parametrized as 
     *  beginPoint + direction() * Tick with plane y=Y
     *  @author       Vanya Belyaev   Ivan.Belyaev@itep.ru 
     *  @date         10.02.2000 
     *  @param line     line satisfying line.beginPoint() + scalar*line.direction() 
     *  @param Y      y-parameter
     *  @param out    output iterator 
     *  @return       number of intersections (==1)
     */
    template <typename ITERATOR, typename aLine>
    unsigned int intersectionLinePlaneY( const aLine& line,
                                         const double Y,
                                         ITERATOR out );

    /** find intersection ticks for the line parametrized as 
     *  beginPoint + direction() * Tick with plane z=Z
     *  @author       Vanya Belyaev   Ivan.Belyaev@itep.ru 
     *  @date         10.02.2000 
     *  @param  line  line satisfying line.beginPoint() + scalar*line.direction() 
     *  @param  Z     z-parameter
     *  @param  out   output iterator 
     *  @return       number of intersections (==1)
     */
    template <typename ITERATOR, typename aLine>
    unsigned int intersectionLinePlaneZ( const aLine& line,
                                         const double Z,
                                         ITERATOR out );
    

    /** find intersection ticks for the line parametrized 
     *  beginPoint + direction() * Tick with half-plane phi=Phi
     *
     *  @author       Vanya Belyaev   Ivan.Belyaev@itep.ru 
     *  @date         10.02.2000 
     *  @param  line  line satisfying line.beginPoint() + scalar*line.direction() 
     *  @param  Phi   phi-parameter
     *  @param  out   output iterator 
     *  @return       number of intersections (==1)
     */
    template <typename ITERATOR, typename aLine>
    unsigned int intersectionLinePlanePhi( const aLine& line,
                                           const double Phi,
                                           ITERATOR out );
    

    /** find intersection ticks for the line parametrized 
     *  beginPoint + direction() * Tick with cone theta=Theta
     *  @author        Vanya Belyaev   Ivan.Belyaev@itep.ru 
     *  @date          10.02.2000 
     *  @param  line   line satisfying line.beginPoint() + scalar*line.direction() 
     *  @param  Theta  Theta-parameter
     *  @param  out    output iterator 
     *  @return        number of intersections (==1)
     */
    template <typename ITERATOR, typename aLine>
    unsigned int intersectionLineCone( const aLine& line,
                                       const double Theta,
                                       ITERATOR out );



    
  }; // Math namespace

}; // Gaudi namespace

#endif // KERNEL_SOLIDINTERSECTIONS_H
