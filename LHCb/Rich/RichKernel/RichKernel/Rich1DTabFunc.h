// $Id: Rich1DTabFunc.h,v 1.2 2004-06-29 19:27:30 jonrob Exp $
#ifndef RICHUTILS_RICH1DTABFUNC_H
#define RICHUTILS_RICH1DTABFUNC_H 1

#include <vector>
#include <map>
#include <iostream>

// GSL interpolation
#include "gsl/gsl_errno.h"
#include "gsl/gsl_spline.h"

/** @class Rich1DTabFunc Rich1DTabFunc.h RichUtils/Rich1DTabFunc.h
 *
 *  A class describing a function based on the interpolation of data points
 *  The interpolation is based on the GSL library, with the default interpolation
 *  type based on a cubic spline algorithm 'gsl_interp_cspline'. For other
 *  possibilities see
 *  http://www.gnu.org/software/gsl/manual/gsl-ref_26.html#SEC389
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-08-13
 */

class Rich1DTabFunc {

public:

  /// Default Constructor
  Rich1DTabFunc::Rich1DTabFunc();

  /// Constructor from array
  Rich1DTabFunc::Rich1DTabFunc( const double x[],
                                const double y[],
                                const int size,
                                const gsl_interp_type * interType = gsl_interp_linear );

  /// Constructor from std::vector
  Rich1DTabFunc::Rich1DTabFunc( const std::vector<double> & x,
                                const std::vector<double> & y,
                                const gsl_interp_type * interType = gsl_interp_linear );

  /// Constructor from std::map
  Rich1DTabFunc::Rich1DTabFunc( const std::map<double,double> & data,
                                const gsl_interp_type * interType = gsl_interp_linear );

  /// Destructor
  virtual ~Rich1DTabFunc( ) { clearInterpolator(); }

  /// Returns the property value for a given parameter value
  double value( const double x ) const;

  /// Returns the property value for a given parameter value
  double operator [] ( const double x ) const;

  /// Returns the mean property value between limits
  double meanX ( const double from, const double to) const;

  /// Returns the intergral of the property between limits
  double integral ( const double from, const double to) const;

  /// Returns the first derivative at the given x point
  double firstDerivative( const double x ) const;

  /// Returns the second derivative at the given x point
  double secondDerivative( const double x ) const;

  /// Returns the minimum parameter value for which the property is defined
  double minX() const;

  /// Returns the property value at the minimum point
  double minY() const;

  /// Returns the minimum parameter value for which the property is defined
  double maxX() const;

  /// Returns the property value at the maximum point
  double maxY() const;

  /// Returns the status of the interpolator
  bool valid() const;

  /// Check whether a given x point is within the input data range
  bool withinInputRange( const double x ) const;

  /// initialise the interpolator
  bool initInterpolator( const gsl_interp_type * interType );

  /// clear the interpolator
  void clearInterpolator();

protected: // data

  // the data points
  std::map<double,double> m_data;

  // Status flag
  bool m_OK;

private: // data

  // GSL data
  gsl_interp_accel * m_mainDistAcc;
  gsl_spline       * m_mainDistSpline;
  gsl_interp_accel * m_weightedDistAcc;
  gsl_spline       * m_weightedDistSpline;

};

//============================================================================

inline double Rich1DTabFunc::value ( const double x ) const
{
  return gsl_spline_eval( m_mainDistSpline, x, m_mainDistAcc );
}

//============================================================================

inline double Rich1DTabFunc::operator [] ( const double x ) const
{
  return value( x );
}

//============================================================================

inline double Rich1DTabFunc::integral ( const double from,
                                        const double to ) const
{
  return gsl_spline_eval_integ( m_mainDistSpline, from, to, m_mainDistAcc );
}

//============================================================================

inline double Rich1DTabFunc::meanX ( const double from,
                                     const double to ) const
{
  const double bot = integral( from, to );
  return ( fabs(bot) > 0 ?
           gsl_spline_eval_integ( m_weightedDistSpline,
                                  from, to, m_weightedDistAcc )/bot : 0 );
}

//============================================================================

inline double Rich1DTabFunc::minX () const
{
  return (*m_data.begin()).first;
}

//============================================================================

inline double Rich1DTabFunc::minY() const
{
  return (*m_data.begin()).second;
}

//============================================================================

inline double Rich1DTabFunc::maxX () const
{
  return (*(--m_data.end())).first;
}

//============================================================================

inline double Rich1DTabFunc::maxY() const
{
  return (*(--m_data.end())).second;
}

//============================================================================

inline double Rich1DTabFunc::firstDerivative( const double x ) const
{
  return gsl_spline_eval_deriv( m_mainDistSpline, x, m_mainDistAcc );
}

//============================================================================

inline double Rich1DTabFunc::secondDerivative( const double x ) const
{
  return gsl_spline_eval_deriv2( m_mainDistSpline, x, m_mainDistAcc );
}

//============================================================================

inline bool Rich1DTabFunc::valid() const
{
  return m_OK;
}

//============================================================================

inline bool Rich1DTabFunc::withinInputRange( const double x ) const
{
  return ( x <= maxX() && minX() <= x );
}

#endif // RICHUTILS_RICH1DTABFUNC_H
