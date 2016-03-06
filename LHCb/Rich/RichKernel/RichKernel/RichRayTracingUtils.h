
#ifndef RICHKERNEL_RICHRAYTRACING_H
#define RICHKERNEL_RICHRAYTRACING_H 1

// MathCore
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"

namespace Rich
{

  /** @namespace RayTracingUtils RichKernel/RayTracingUtils.h
   *
   *  Utility namespace providing basic ray tracing methods for
   *  intersecting and reflecting photons (line) off a sphere and plane.
   *
   *  @author Chris Jones
   *  @date   2016-03-02
   */
  namespace RayTracingUtils
  {

    /** Intersect a given direction, from a given point, with a given plane.
     *
     *  @param[in]  position      The start point to use for the ray tracing
     *  @param[in]  direction     The direction to ray trace from the start point
     *  @param[in]  plane         The plane to intersect
     *  @param[out] intersection  The intersection point of the direction with the plane
     *
     *  @return Boolean indicating the status of the ray tracing
     *  @retval true  Ray tracing was successful
     *  @retval false Ray tracing was unsuccessful
     */
    inline bool
    intersectPlane ( const Gaudi::XYZPoint& position,
                     const Gaudi::XYZVector& direction,
                     const Gaudi::Plane3D& plane,
                     Gaudi::XYZPoint& intersection )
    {
      bool OK = true;
      const auto scalar = direction.Dot( plane.Normal() );
      if ( UNLIKELY( fabs(scalar) < 1e-99 ) ) { OK = false; }
      else
      {
        const auto distance = -(plane.Distance(position)) / scalar;
        intersection = position + (distance*direction);
      }
      return OK;
    }

    /** Intersect a given direction, from a given point, with a given spherical shell.
     *
     *  @attention This method is specifically optimised for a spherical shell from 
     *             the inside out, as is required for the RICH mirrors. It will *not*
     *             give the correct results if used in the other direction...
     *
     *  @param[in]  position      The start point to use for the ray tracing
     *  @param[in]  direction     The direction to ray trace from the start point
     *  @param[in] CoC            The centre of curvature of the spherical mirror
     *  @param[in] radius         The radius of curvature of the spherical mirror
     *  @param[out] intersection  The intersection point of the direction with the plane
     *
     *  @return Boolean indicating the status of the ray tracing
     *  @retval true  Ray tracing was successful
     *  @retval false Ray tracing was unsuccessful
     */
    inline bool
    intersectSpherical ( const Gaudi::XYZPoint& position,
                         const Gaudi::XYZVector& direction,
                         const Gaudi::XYZPoint& CoC,
                         const double radius,
                         Gaudi::XYZPoint& intersection )
    {
      bool OK = true;
      // for line sphere intersection look at http://www.realtimerendering.com/int/
      const auto a = direction.Mag2();
      if ( UNLIKELY( 0 == a ) ) { OK = false; }
      else
      {
        const auto delta = position - CoC;
        const auto     b = 2.0 * direction.Dot( delta );
        const auto     c = delta.Mag2() - radius*radius;
        const auto discr = b*b - 4.0*a*c;
        if ( UNLIKELY( discr < 0 ) ) { OK = false; }
        else
        {
          const auto dist = 0.5 * ( std::sqrt(discr) - b ) / a;
          // set intersection point
          intersection = position + ( dist * direction );
        }
      }
      return OK;
    }

    /** Reflect a given direction off a spherical mirror. Can be used for intersection.
     *
     *  @attention This method is specifically optimised for a spherical shell from 
     *             the inside out, as is required for the RICH mirrors. It will *not*
     *             give the correct results if used in the other direction...
     *
     *  @param[in,out] position   The start point to use for the ray tracing.
     *                            Afterwards gives the reflection point on the spherical mirror.
     *  @param[in,out] direction  The direction to ray trace from the start point.
     *                            Afterwards represents the reflection direction from the spherical mirror.
     *  @param[in] CoC        The centre of curvature of the spherical mirror
     *  @param[in] radius     The radius of curvature of the spherical mirror
     *
     *  @return Boolean indicating if the ray tracing was succesful
     *  @retval true  Ray tracing was successful
     *  @retval false Ray tracing was unsuccessful
     */
    inline bool
    reflectSpherical ( Gaudi::XYZPoint& position,
                       Gaudi::XYZVector& direction,
                       const Gaudi::XYZPoint& CoC,
                       const double radius )
    {
      bool OK = true;
      // for line sphere intersection look at http://www.realtimerendering.com/int/
      const auto a = direction.Mag2();
      if ( UNLIKELY( 0 == a ) ) { OK = false; }
      else
      {
        const auto delta = position - CoC;
        const auto     b = 2.0 * direction.Dot( delta );
        const auto     c = delta.Mag2() - radius*radius;
        const auto discr = b*b - 4.0*a*c;
        if ( UNLIKELY( discr < 0 ) ) { OK = false; }
        else
        {
          const auto dist = 0.5 * ( std::sqrt(discr) - b ) / a;
          // change position to the intersection point
          position += dist * direction;
          // reflect the vector
          // r = u - 2(u.n)n, r=reflction, u=insident, n=normal
          const auto normal = position - CoC;
          direction -= ( 2.0 * normal.Dot(direction) / normal.Mag2() ) * normal;
        }
      }
      return OK;
    }

    /** Ray trace from given position in given direction off flat mirrors
     *
     *  @param[in,out] position  On input the start point. On output the reflection point
     *  @param[in,out] direction On input the starting direction. On output the reflected direction.
     *  @param[in]     plane     The plane to refect off
     *
     *  @return Boolean indicating if the ray tracing was succesful
     *  @retval true  Ray tracing was successful
     *  @retval false Ray tracing was unsuccessful
     */
    inline bool
    reflectPlane ( Gaudi::XYZPoint& position,
                   Gaudi::XYZVector& direction,
                   const Gaudi::Plane3D& plane )
    {
      bool OK = true;
      // Plane normal
      const auto normal = plane.Normal();
      // compute distance to the plane
      const auto scalar = direction.Dot(normal);
      if ( UNLIKELY( fabs(scalar) < 1e-99 ) ) { OK = false; }
      else
      {
        const auto distance = -(plane.Distance(position)) / scalar;
        // change position to reflection point and direction
        position  += distance * direction;
        direction -= 2.0 * (normal.Dot(direction)) * normal;
      }
      return OK;
    }

  }

}

#endif // RICHKERNEL_RICHRAYTRACING_H
