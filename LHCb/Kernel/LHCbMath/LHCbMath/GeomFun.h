// $Id: GeomFun.h,v 1.4 2006-10-27 15:34:42 jpalac Exp $
#ifndef LHCBMATH_GEOMFUN_H
#define LHCBMATH_GEOMFUN_H 1


namespace Gaudi{

  namespace Math{

    /** calculate intersection point between a aLine and aPlane.
     *
     *  aLine must satisfy point = line.beginPoint() + scalar*line.direction()
     *  and export its vector type as aLine::Vector
     *
     *  aPlane must have methods Normal() and HesseDistance()
     *
     *  aPoint must be constructible from object with x(), y(), z() methods.
     *  
     *  @author  Matthew Needham
     *  @param   line: generic line
     *  @param   plane: generic plane
     *  @param   intersect: Intersection point
     *  @param   mu: scalar defining point on line which intersects plane
     *  @return  bool indicating whether intersection exists
     *
     */
    template<typename aLine, typename aPlane, typename aPoint>
    bool intersection(const aLine& line,
                      const aPlane& plane,
                      aPoint& intersect,
                      double& mu);

    /** Calculate aLine, the intersection of two aPlanes pane0 and
     *  plane1. 
     *
     *  aLine must satisfy point = line.beginPoint() + scalar*line.direction()
     *  and export its vector and point types as aLine::Vector and aLine::Point
     *  respectively
     *
     *  aPlane must have methods Normal() and HesseDistance() and export 
     *  its scalar type as aPlane::Scalar
     *
     *  @author  Juan Palacios Juan.Palacios@cern.ch
     *  @date    01/05/2006
     *  @param   plane0: aPlane to be intersected 
     *  @param   plane1: aPlane to be intersected
     *  @param   intersect: aLine of intersection between plane0 and plane1
     *  @return  false if planes parallel
     *  @todo    test for planes being parallel
     */
    template<typename aLine, typename aPlane>
    bool intersection(const aPlane& plane0,
                      const aPlane& plane1,
                      aLine& intersect);

    /** Calculate aPoint, the intersection of three aPlanes.
     *
     *  aPlane must have methods Normal() and HesseDistance()
     *  and export its scalar type as aPlane::Scalar
     *
     *
     *  aPoint must be constructible from object with x(), y(), z() methods.
     *
     *  @author  Juan Palacios Juan.Palacios@cern.ch
     *  @date    01/05/2006
     *  @param   plane0:    aPlane to be intersected 
     *  @param   plane1:    aPlane to be intersected
     *  @param   plane3:    aPlane to be intersected
     *  @param   intersect: aPoint, point of intersection between three planes
     *  @return  true if intersection exists, false if any two planes parallel
     *  @todo test for any two planes being parallel.
    */
    template<typename aPoint, typename aPlane>
    bool intersection(const aPlane& plane0,
                      const aPlane& plane1,
                      const aPlane& plane2,
                      aPoint& intersect);

    /** Return the distance between point and line
     *
     *  @author  Juan Palacios Juan.Palacios@cern.ch
     *  @date    01/05/2006
     *  @param   point: aPoint who's distance to line is to be calculated
     *  @param   line: aLine who's distance to point is to be calculated
     *  @return  distance between point and line (double)
     */
    template<typename aPoint, typename aLine>
    double impactParameter(const aPoint&  point ,
                           const aLine& line);

    /** Compute the distance between two lines
     *
     *  aLine must satisfy point = line.beginPoint() + scalar*line.direction()
     *  and export its vector and point types as aLine::Vector and Line::Point
     *  respectively
     *
     *  aPoint must be constructible from object with x(), y(), z() methods.
     *
     *  @author  Juan Palacios Juan.Palacios@cern.ch
     *  @date    01/05/2006
     *  @param   line0: first aLine in distance calculation
     *  @param   line1: second aLine in distance calculation
     *  @return  distance between lines (double)
     */
    template <typename aLine>
    double distance(const aLine& line0,
                    const aLine& line1);


    /** Get nearest aPoints p0 and p1 between aLines line0 and line2 such that 
     *  p0 = line0.beginPoint() + mu0 * line0.direction()
     *  p1 = line1.beginPoint() + mu1 * line1.direction()
     *
     *  aLine must satisfy point = line.beginPoint() + scalar*line.direction()
     *
     *  aPoint must be constructible from object with x(), y(), z() methods.
     *  @author  Juan Palacios Juan.Palacios@cern.ch
     *  @date    01/05/2006
     *  @param   line0: first aLine in distance calculation
     *  @param   line0: second aLine in distance calculation
     *  @param   p0: closest point line0.beginPoint() + mu0 * line0.direction()
     *  @param   p1: closest point line1.beginPoint() + mu1 * line1.direction()
     *  @param   mu0: line0 scalar definint p0
     *  @param   mu1: line1 scalar definint p1
     *  @return  false if not numerically calculable
     */
    template <typename aLine, typename aPoint>
    bool closestPoints(const aLine& line0,
                       const aLine& line1,
                       aPoint& p0,
                       aPoint& p1);


    /** Return scalar parameter defining the point on aLine 
     *  which is closest to an aPoint, such that
     *  closest point = line.beginPoint() + scalar * line.direction()
     *
     *  aLine must satisfy point = line.beginPoint() + scalar*line.direction()
     *
     *  aPoint must be constructible from object with x(), y(), z() methods.
     *
     *  @author  Matthew Needham, Juan Palacios
     *  @date    19/04/2006
     *  @param   point:
     *  @param   line:
     *  @return  scalar defining aPoint on line which is closest to point
     */
    template <typename aLine, typename aPoint>
    double closestPointParam(const aPoint&  point ,
                             const aLine& line);

    /** Return aPoint on aLine which is closest to an aPoint.
     *
     *  aLine must satisfy point = line.beginPoint() + scalar*line.direction()
     *  and export its vector and point types as aLine::Vector and aLine::Point
     *  respectively
     *
     *  aPoint must be constructible from object with x(), y(), z() methods.
     *
     *  @author  Matthew Needham, Juan Palacios
     *  @date    19/04/2006
     *  @param   point:
     *  @param   line:
     *  @return  aPoint on line which is closest to point
     */
    template <typename aLine, typename aPoint>
    aPoint closestPoint(const aPoint&  point ,
                        const aLine& line);

    /** Calculate scalar parameters defining points of nearest approach 
     *  between aLines line0, line1. If p0 and p1 are the points of closest
     *  approach, mu0 and mu1 satisfy
     *  p0 = line0.beginPoint() + mu0 * line0.direction()
     *  p0 = line1.beginPoint() + mu1 * line1.direction()
     * 
     *  @author  Matthew Needham
     *  @date    19/04/2006
     *  @param   line0
     *  @param   line1
     *  @param   mu0: scalar parameter defining point on line0 closest to line1
     *  @param   mu1: scalar parameter defining point on line1 closest to line0
     *  @return  true
     */
    template <typename aLine>
    bool closestPointParams(const aLine& line0,
                            const aLine& line1,
                            double& mu0, double& mu1);

    /** Estimate whether two lines are parallel
     *  Use dot product of line0.direction() and line1.direction() = 1
     *
     *  @author  Juan Palacios
     *  @date    10/05/2006
     *  @param   line0
     *  @param   line1
     *  @return  true if lines parallel
    */

    template <typename aLine>
    bool parallel(const aLine& line0,
                  const aLine& line1);




  }; // Math namespace

}; // Gaudi namespace

#include "LHCbMath/GeomFun.icpp"

#endif
