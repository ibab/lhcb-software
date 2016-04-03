
//============================================================================
/** @file Rich1DTabFunc.h
 *
 *  Header file for utility class : Rich::TabulatedFunction1D
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-08-13
 */
//============================================================================

#ifndef RICHDET_RICH1DTABFUNC_H
#define RICHDET_RICH1DTABFUNC_H 1

#include <vector>
#include <map>
#include <iostream>
#include <math.h>
#include <memory>

// GSL interpolation
#include "gsl/gsl_errno.h"
#include "gsl/gsl_spline.h"

namespace Rich
{

  //============================================================================
  /** @class Rich::TabulatedFunction1D RichDet/Rich1DTabFunc.h
   *
   *  A class describing a function based on the interpolation of data points.
   *
   *  The interpolation is based on the GSL library, with the default interpolation
   *  type based on a simple linear algorithm 'gsl_interp_linear'.
   *
   *  For other possibilities see
   *  http://www.gnu.org/software/gsl/manual/html_node/Interpolation.html
   *
   *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
   *  @date   2003-08-13
   */
  //============================================================================

  class TabulatedFunction1D
  {

  public:

    /// Typedef for a vector of pointers to TabulatedFunction1D
    using ConstVector = std::vector<const TabulatedFunction1D*>;

  protected: // definitions

    /// Internal data container type
    using Data = std::map < double, double >;

  public:

    /** Default Constructor with optional interpolator type argument
     *
     *  @param interType   GSL Interpolator type.
     */
    TabulatedFunction1D( const gsl_interp_type * interType = gsl_interp_linear )
      : m_interType ( interType ) 
    {
      initInterpolator(); 
    }

    /** Constructor from arrays containing x and y values
     *
     *  Arrays must be of the same size and ordered such that entry i in each
     *  correspond to each other.
     *
     *  @param x         Array of x values
     *  @param y         Array of y values
     *  @param size      Number of data points
     *  @param interType GSL Interpolator type
     */
    TabulatedFunction1D( const double x[],
                         const double y[],
                         const int size,
                         const gsl_interp_type * interType = gsl_interp_linear )
      : m_interType ( interType )
    {
      initInterpolator ( x, y, size, interType );
    }
    
    /** Constructor from std::vectors containing x and y values
     *
     *  Vectors must be of the same size and ordered such that entry i in each
     *  correspond to each other.
     *
     *  @param x         Vector of x values
     *  @param y         Vector of y values
     *  @param interType GSL Interpolator type
     */
    TabulatedFunction1D( const std::vector<double> & x,
                         const std::vector<double> & y,
                         const gsl_interp_type * interType = gsl_interp_linear )
      : m_interType ( interType )
    {
      initInterpolator ( x, y, interType );
    }

    /** Constructor from map of x,y values
     *
     *  @param data      map containing x(key) and y(data) values
     *  @param interType GSL Interpolator type
     */
    TabulatedFunction1D( const std::map<double,double> & data,
                         const gsl_interp_type * interType = gsl_interp_linear )
      : m_interType ( interType )
    {
      initInterpolator( data, interType );
    }

    /** Constructor from vector of a pair of x,y values
     *
     *  @param data      std::vector containing and pair of x(first) and y(second) values
     *  @param interType GSL Interpolator type
     */
    TabulatedFunction1D( const std::vector< std::pair<double,double> > & data,
                         const gsl_interp_type * interType = gsl_interp_linear )
      :
      m_interType ( interType )
    {
      initInterpolator( data, interType );
    }
    
    /// Destructor
    virtual ~TabulatedFunction1D( );

  public:

    /** Create an interpolator that is the product of a list of interpolators
     *
     *  @param funcs   A vector containing pointers to the functions to merge
     *  @param samples Number of sample points to use
     *
     *  @return Pointer to an interpolator that represents the productof all functions
     *  @retval Non-nullptr Interpolator was successfully created
     *  @retval nullptr     Interpolator could not be created
     */
    static std::unique_ptr<TabulatedFunction1D> 
    combine( const ConstVector & funcs,
             const unsigned int samples = 100,
             const gsl_interp_type * interType = gsl_interp_linear ); 

  private:

    /** Issue an out of range warning
     *  @param x    The requested x value
     *  @param retx The x value to use (corrected to be in range)
     *  @return x value to use
     */
    virtual double rangeWarning( const double x, const double retx ) const;

    /** x value range check
     *  @param x The x value to check
     *  @return The x value to use
     */
    double checkRange( const double x ) const
    {
      return ( withinInputRange(x) ? x :
               x < minX() ? rangeWarning(x,minX()) : rangeWarning(x,maxX()) );
    }

  public:

    /** Computes the function value (y) for the given parameter (x) value
     *
     *  @param x The parameter value
     *
     *  @return The value of the function at the given parameter value
     */
    inline double value( const double x ) const
    {
      return gsl_spline_eval( m_mainDistSpline, checkRange(x), m_mainDistAcc );
    }

    /**  Returns the function value (y) for the given parameter (x) value
     *
     *  @param x The parameter value
     *
     *   @return The value of the function at the given parameter value
     */
    inline double operator [] ( const double x ) const
    {
      return value( x );
    }

    /** Computes the mean function value between the given parameter limits
     *
     *  @param from       The lower parameter limit
     *  @param to         The upper parameter limit
     *
     *  @return the mean function value
     */
    inline double meanX ( const double from, 
                          const double to ) const
    {
      const auto _from = checkRange(from);
      const auto _to   = checkRange(to);
      const auto bot = integral( _from, _to );
      return ( fabs(bot) > 0 ?
               gsl_spline_eval_integ( m_weightedDistSpline,
                                      _from, _to, m_weightedDistAcc )/bot : 0 );
    }

    /** Computes the definite integral of the function between limits
     *
     *  @param from       The lower parameter limit
     *  @param to         The upper parameter limit
     *  
     *  @return the definite function integral
     */
    inline double integral ( const double from,  
                             const double to ) const
    {
      return gsl_spline_eval_integ( m_mainDistSpline, 
                                    checkRange(from), 
                                    checkRange(to), 
                                    m_mainDistAcc );
    }

    /** Computes the first derivative of the function at the given parameter point
     *
     *  @param x The parameter value
     *
     *  @return the first derivative
     */
    inline double firstDerivative( const double x ) const
    {
      return gsl_spline_eval_deriv( m_mainDistSpline, checkRange(x), m_mainDistAcc );
    }

    /** Computes the second derivative of the function at the given parameter point
     *
     *  @param x The parameter value
     *
     *  @return the second derivative
     */
    inline double secondDerivative( const double x ) const
    {
      return gsl_spline_eval_deriv2( m_mainDistSpline, checkRange(x), m_mainDistAcc );
    }
    
    /** Computes the R.M.S. value between the given parameter limits.
     *
     *  Optionally, an additional weight function can be given.
     *
     *  @param from       The lower parameter limit
     *  @param to         The upper parameter limit
     *  @param samples    Number of sample points to use in calculating the RMS
     *  @param weightFunc Optional weight function
     *
     *  @return the r.m.s. value
     */
    double rms( const double from,
                const double to,
                const unsigned int samples = 100,
                const TabulatedFunction1D * weightFunc = nullptr ) const;

    /** Computes the standard deviation between the given parameter limits
     *
     *  Optionally, an additional weight function can be given.
     *
     *  @param from    The lower parameter limit
     *  @param to      The upper parameter limit
     *  @param samples Number of sample points to use in calculating the S.D.
     *  @param weightFunc Optional weight function
     *
     *  @return the standard deviation
     */
    double standardDeviation( const double from,
                              const double to,
                              const unsigned int samples = 100,
                              const TabulatedFunction1D * weightFunc = nullptr ) const;

    /** The minimum parameter value for which the function is defined
     *
     *  @return The minimum valid paramter value
     */
    inline double minX() const noexcept 
    {
      return m_mainDistSpline->x[0];
    }

    /** The function value for the minimum valid parameter
     *
     *  @return The function value at the minimum valid parameter
     */
    inline double minY() const noexcept 
    {
      return m_mainDistSpline->y[0];
    }

    /** The maximum parameter value for which the function is defined
     *
     *  @return The minimum valid paramter value
     */
    inline double maxX() const noexcept 
    {
      return m_mainDistSpline->x[ m_mainDistSpline->size - 1 ];
    }

    /** The function value for the minimum valid parameter
     *
     *  @return The function value at the minimum valid parameter
     */
    inline double maxY() const noexcept 
    {
      return m_mainDistSpline->y[ m_mainDistSpline->size - 1 ];
    }

    /** The status of the interpolator.
     *
     *  @return boolean indicating whether the interpolator was correctly initialised
     */
    inline bool valid() const noexcept { return m_OK; }

    /** Check whether a given x parameter is within the valid input data range
     *
     *  @param x The parameter value
     *
     *  @return boolean indicating if the parameter is in range
     */
    inline bool withinInputRange( const double x ) const noexcept
    {
      return ( x <= maxX() && minX() <= x );
    }

    /** Access the number of data points defining the interpolator
     *
     *  @return The number of data (x,y) points
     */
    inline size_t nDataPoints() const noexcept { return m_mainDistSpline->size; }

    /// Return the interpolator name
    inline std::string interpName() const
    {
      return gsl_interp_name(m_mainDistSpline->interp);
    }
    
    /// Return the interpolator type
    inline const gsl_interp_type * interType() const noexcept
    {
      return m_interType;
    }

  public:

    /** Initialisation from arrays containing x and y values
     *
     *  Arrays must be of the same size and ordered such that entry i in each
     *  correspond to each other.
     *
     *  @param x         Array of x values
     *  @param y         Array of y values
     *  @param size      Number of data points
     *  @param interType GSL Interpolator type (If not given, currently configured type is used)
     */
    bool initInterpolator( const double x[],
                           const double y[],
                           const int size,
                           const gsl_interp_type * interType = nullptr );

    /** Initialisation from std::vectors containing x and y values
     *
     *  Vectors must be of the same size and ordered such that entry i in each
     *  correspond to each other.
     *
     *  @param x         Vector of x values
     *  @param y         Vector of y values
     *  @param interType GSL Interpolator type (If not given, currently configured type is used)
     */
    bool initInterpolator( const std::vector<double> & x,
                           const std::vector<double> & y,
                           const gsl_interp_type * interType = nullptr );

    /** Initialisation from a map of x,y values
     *
     *  @param data      map containing x(key) and y(data) values
     *  @param interType GSL Interpolator type (If not given, currently configured type is used)
     */
    bool initInterpolator( const std::map<double,double> & data,
                           const gsl_interp_type * interType = nullptr );

    /** Initialisation from a vector of a pair of x,y values
     *
     *  @param data      std::vector containing and pair of x(first) and y(second) values
     *  @param interType GSL Interpolator type (If not given, currently configured type is used)
     */
    bool initInterpolator( const std::vector< std::pair<double,double> > & data,
                           const gsl_interp_type * interType = nullptr );

  protected: // methods

    /// clear the interpolator
    void clearInterpolator();

    /// Default initialise the interpolator
    void initInterpolator();
    
  private: // data
    
    // GSL interpolator objects
    gsl_interp_accel * m_mainDistAcc        = nullptr; ///< The accelerator for the main y(x) distribution
    gsl_spline       * m_mainDistSpline     = nullptr; ///< The spline for the main y(x) distribution
    gsl_interp_accel * m_weightedDistAcc    = nullptr; ///< The accelerator for the weighted x.y(x) distribution
    gsl_spline       * m_weightedDistSpline = nullptr; ///< The spline for the weighted x.y(x) distribution
    
  protected:
    
    /// Status flag
    bool m_OK = false;
    
    /// The interpolator type
    const gsl_interp_type * m_interType = nullptr;
    
  };

}

#endif // RICHDET_RICH1DTABFUNC_H
