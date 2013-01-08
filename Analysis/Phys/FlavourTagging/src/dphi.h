// $Id: $
/**
 * @file dphi.h
 *
 * (correctly) calculate the difference between two angles in the plane
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2013-01-08
 */
#ifndef INCLUDE_DPHI_H
#define INCLUDE_DPHI_H 1

#include <cmath>
#include <limits>

namespace TaggingHelpers {
    /** @brief return the difference between two angles
     *
     * @author Manuel Schiller <manuel.schiller@nikhef.nl>
     * @date 2013-01-08
     *
     * @param phi1	first angle in radians
     * @param phi2	second angle in radians
     *
     * @returns		difference between angles in radians
     */
    inline double dphi(const double phi1, const double phi2)
    {
	const double c1 = std::cos(phi1), s1 = std::sin(phi1);
	const double c2 = std::cos(phi2), s2 = std::sin(phi2);
	return std::atan2(c1 * s2 - c2 * s1, c1 * c2 + s1 * s2);
    }

    /** @brief return the difference in angle between two vectors in the plane
     *
     * @author Manuel Schiller <manuel.schiller@nikhef.nl>
     * @date 2013-01-08
     *
     * @param x1	x component of first vector in the plane
     * @param y1	y component of first vector in the plane
     * @param x2	x component of second vector in the plane
     * @param y2	y component of second vector in the plane
     *
     * @returns		difference in angles in radians, or NaN if at least
     * 			one of the vectors has zero length
     */
    inline double dphi(const double x1, const double y1,
	    const double x2, const double y2)
    {
	const double r1 = std::sqrt(x1 * x1 + y1 * y1);
	const double r2 = std::sqrt(x2 * x2 + y2 * y2);
	// at least one of the vectors has zero length, return NaN
	if (0. == r1 || 0. == r2)
	    return std::numeric_limits<double>::quiet_NaN();
	const double c1 = x1 / r1, s1 = y1 / r1;
	const double c2 = x2 / r2, s2 = y2 / r2;
	return std::atan2(c1 * s2 - c2 * s1, c1 * c2 + s1 * s2);
    }
}

#endif // INCLUDE_DPHI_H

// vim: tw=78:sw=4:ft=cpp
