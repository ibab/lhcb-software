
//============================================================================
/** @file Rich1DTabFunc.h
 *
 *  Header file for utility class : Rich1DTabFunc
 *
 *  CVS Log :-
 *  $Id: Rich1DTabFunc.h,v 1.3 2006-03-14 14:42:19 jonrob Exp $ 
 *  
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-08-13
 */
//============================================================================

#ifndef RICHKERNEL_RICH1DTABFUNC_H
#define RICHKERNEL_RICH1DTABFUNC_H 1

#include <vector>
#include <map>
#include <iostream>
#include <math.h>

// GSL interpolation
#include "gsl/gsl_errno.h"
#include "gsl/gsl_spline.h"

//============================================================================
/** @class Rich1DTabFunc Rich1DTabFunc.h RichKernel/Rich1DTabFunc.h
 *
 *  A class describing a function based on the interpolation of data points
 *  The interpolation is based on the GSL library, with the default interpolation
 *  type based on a simple linear algorithm 'gsl_interp_linear'. 
 *  
 *  For other possibilities see
 *  http://www.gnu.org/software/gsl/manual/gsl-ref_26.html#SEC389
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-08-13
 */
//============================================================================

class Rich1DTabFunc 
{

public:

  /** Default Constructor with optional interpolator type argument
   *
   *  @param interType   GSL Interpolator type. See
   *                     http://www.gnu.org/software/gsl/manual/gsl-ref_26.html#SEC389
   */
  Rich1DTabFunc::Rich1DTabFunc( const gsl_interp_type * interType = gsl_interp_linear );

  /** Constructor from arrays containing x and y values
   *  Arrays must be of the same size, and ordered such that entry i in each
   *  correspond to each other.
   *
   *  @param x         Array of x values
   *  @param y         Array of y values
   *  @param size      Number of data points
   *  @param interType GSL Interpolator type
   */
  Rich1DTabFunc::Rich1DTabFunc( const double x[], 
                                const double y[],
                                const int size, 
                                const gsl_interp_type * interType = gsl_interp_linear );

  /** Constructor from std::vectors containing x and y values
   *  Vectors must be of the same size, and ordered such that entry i in each
   *  correspond to each other.
   *
   *  @param x         Vector of x values
   *  @param y         Vector of y values
   *  @param interType GSL Interpolator type
   */
  Rich1DTabFunc::Rich1DTabFunc( const std::vector<double> & x, 
                                const std::vector<double> & y, 
                                const gsl_interp_type * interType = gsl_interp_linear );

  /** Constructor from map of x,y values
   *
   *  @param data     map contain x(key) and y(data) values
   *  @param interType GSL Interpolator type
   */
  Rich1DTabFunc::Rich1DTabFunc( const std::map<double,double> & data, 
                                const gsl_interp_type * interType = gsl_interp_linear );

  /// Destructor
  virtual ~Rich1DTabFunc( );

  /** Computes the function value (y) for the given parameter (x) value
   *
   *  @param x The parameter value
   *
   *  @return The value of the function at the given parameter value
   */
  double value( const double x ) const;

  /**  Returns the function value (y) for the given parameter (x) value
   *
   *  @param x The parameter value
   *
   *   @return The value of the function at the given parameter value
   */
  double operator [] ( const double x ) const;

  /** Computes the mean function value between the given parameter limits
   *
   *  @param from  The lower parameter limit
   *  @param to    The upper parameter limit
   *
   *  @return the mean function value
   */
  double meanX ( const double from, const double to ) const;

  /** Computes the definite integral of the function between limits
   *
   *  @return the definite function integral
   */
  double integral ( const double from,   ///< The lower parameter limit
                    const double to      ///< The upper parameter limit
                    ) const;

  /** Computes the first derivative of the function at the given parameter point
   *
   *  @param x The parameter value
   *
   *  @return the first derivative
   */
  double firstDerivative( const double x ) const;

  /** Computes the second derivative of the function at the given parameter point
   *
   *  @param x The parameter value
   *
   *  @return the second derivative
   */
  double secondDerivative( const double x ) const;

  /** Computes the R.M.S. value between the given parameter limits
   *
   *  @param from    The lower parameter limit
   *  @param to      The upper parameter limit
   *  @param samples Number of sample points to use in calculating the RMS
   *
   *  @return the r.m.s. value
   */
  double rms( const double from, 
              const double to,
              const unsigned int samples = 100 ) const;

  /** The minimum parameter value for which the function is defined
   *
   *  @return The minimum valid paramter value
   */
  double minX() const;

  /** The function value for the minimum valid parameter
   *
   *  @return The function value at the minimum valid parameter
   */
  double minY() const;

  /** The maximum parameter value for which the function is defined
   *
   *  @return The minimum valid paramter value
   */
  double maxX() const;

  /** The function value for the minimum valid parameter
   *
   *  @return The function value at the minimum valid parameter
   */
  double maxY() const;

  /** The status of the interpolator.
   *  
   *  @return boolean indicating whether the interpolator was correctly initialised
   */
  bool valid() const;

  /** Check whether a given x parameter is within the valid input data range
   *
   *  @param x The parameter value
   *
   *  @return boolean indicating if the parameter is in range
   */
  bool withinInputRange( const double x ) const;

  /** initialise the GSL interpolator
   *  
   *  @param interType GSL Interpolator type
   *
   *  @return the status of the initialisation
   *  @retval true  The interpolator initialised correctly and is ready for use
   *  @retval false The interpolator failed to initialise correctly
   */
  bool initInterpolator( const gsl_interp_type * interType );

  /** Access the number of data points defining the interpolator
   *
   *  @return The number of data (x,y) points
   */
  unsigned int nDataPoints() const;

  /// clear the interpolator
  void clearInterpolator();

  /// Return the interpolator name
  std::string interpName() const;

  /// Return the interpolator type
  const gsl_interp_type * interType() const;

protected: // data

  /// Internal data container type
  typedef std::map < double, double > Data;

  /// the data points
  Data m_data;

  /// Status flag
  bool m_OK;

private: // data

  // GSL interpolator objects
  gsl_interp_accel * m_mainDistAcc;        ///< The accelerator for the main y(x) distribution
  gsl_spline       * m_mainDistSpline;     ///< The spline for the main y(x) distribution
  gsl_interp_accel * m_weightedDistAcc;    ///< The accelerator for the weighted x.y(x) distribution
  gsl_spline       * m_weightedDistSpline; ///< The spline for the weighted x.y(x) distribution
  const gsl_interp_type * m_interType;     ///< The interpolator type

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

//============================================================================

inline unsigned int Rich1DTabFunc::nDataPoints() const
{
  return m_data.size();
}

//============================================================================

inline std::string Rich1DTabFunc::interpName() const
{
  return ( m_mainDistSpline ? 
           std::string(gsl_interp_name(m_mainDistSpline->interp)) : "Undefined" );
}

//============================================================================

inline const gsl_interp_type * Rich1DTabFunc::interType() const
{
  return m_interType;
}

//============================================================================

#endif // RICHKERNEL_RICH1DTABFUNC_H
