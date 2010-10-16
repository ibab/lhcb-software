
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
    typedef std::vector<const TabulatedFunction1D*> ConstVector;

  public:

    /** Default Constructor with optional interpolator type argument
     *
     *  @param interType   GSL Interpolator type.
     */
    TabulatedFunction1D( const gsl_interp_type * interType = gsl_interp_linear );

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
                         const gsl_interp_type * interType = gsl_interp_linear );

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
                         const gsl_interp_type * interType = gsl_interp_linear );

    /** Constructor from map of x,y values
     *
     *  @param data      map containing x(key) and y(data) values
     *  @param interType GSL Interpolator type
     */
    TabulatedFunction1D( const std::map<double,double> & data,
                         const gsl_interp_type * interType = gsl_interp_linear );

    /** Constructor from vector of a pair of x,y values
     *
     *  @param data      std::vector containing and pair of x(first) and y(second) values
     *  @param interType GSL Interpolator type
     */
    TabulatedFunction1D( const std::vector< std::pair<double,double> > & data,
                         const gsl_interp_type * interType = gsl_interp_linear );

    /// Destructor
    virtual ~TabulatedFunction1D( );

  public:

    /** Create an interpolator that is the product of a list of interpolators
     *
     *  @param funcs   A vector containing pointers to the functions to merge
     *  @param samples Number of sample points to use
     *
     *  @return Pointer to an interpolator that represents the productof all functions
     *  @retval Non-NULL Interpolator was successfully created
     *  @retval NULL     Interpolator could not be created
     */
    static TabulatedFunction1D * combine( const ConstVector & funcs,
                                          const unsigned int samples = 100,
                                          const gsl_interp_type * interType = gsl_interp_linear ); 

  public:

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
     *  @param from       The lower parameter limit
     *  @param to         The upper parameter limit
     *
     *  @return the mean function value
     */
    double meanX ( const double from, 
                   const double to ) const;

    /** Computes the definite integral of the function between limits
     *
     *  @param from       The lower parameter limit
     *  @param to         The upper parameter limit
     *  
     *  @return the definite function integral
     */
    double integral ( const double from,  
                      const double to ) const;

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
                const TabulatedFunction1D * weightFunc = NULL ) const;

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
                              const TabulatedFunction1D * weightFunc = NULL ) const;

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

    /** Access the number of data points defining the interpolator
     *
     *  @return The number of data (x,y) points
     */
    unsigned int nDataPoints() const;

    /// Return the interpolator name
    std::string interpName() const;

    /// Return the interpolator type
    const gsl_interp_type * interType() const;

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
                           const gsl_interp_type * interType = NULL );

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
                           const gsl_interp_type * interType = NULL );

    /** Initialisation from a map of x,y values
     *
     *  @param data      map containing x(key) and y(data) values
     *  @param interType GSL Interpolator type (If not given, currently configured type is used)
     */
    bool initInterpolator( const std::map<double,double> & data,
                           const gsl_interp_type * interType = NULL );

    /** Initialisation from a vector of a pair of x,y values
     *
     *  @param data      std::vector containing and pair of x(first) and y(second) values
     *  @param interType GSL Interpolator type (If not given, currently configured type is used)
     */
    bool initInterpolator( const std::vector< std::pair<double,double> > & data,
                           const gsl_interp_type * interType = NULL );

    /** initialise the GSL interpolator using the given type
     *
     *  Initialisation will (re)use whatever data values the interpolator has been given
     *
     *  @param interType GSL Interpolator type
     *
     *  @return the status of the initialisation
     *  @retval true  The interpolator initialised correctly and is ready for use
     *  @retval false The interpolator failed to initialise correctly
     */
    bool initInterpolator( const gsl_interp_type * interType );

  protected: // methods

    /// clear the interpolator
    void clearInterpolator();

  protected: // definitions

    /// Internal data container type
    typedef std::map < double, double > Data;

  protected: // data

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


  protected:

    /// The interpolator type
    const gsl_interp_type * m_interType;

  };

  //============================================================================

  // Default Constructor
  inline TabulatedFunction1D::TabulatedFunction1D( const gsl_interp_type * interType ) :
    m_OK                 ( false     ),
    m_mainDistAcc        ( NULL      ),
    m_mainDistSpline     ( NULL      ),
    m_weightedDistAcc    ( NULL      ),
    m_weightedDistSpline ( NULL      ),
    m_interType          ( interType )
  { }

  //============================================================================

  // Constructor from arrays
  inline TabulatedFunction1D::TabulatedFunction1D( const double x[],
                                                   const double y[],
                                                   const int size,
                                                   const gsl_interp_type * interType ) :
    m_OK                 ( false     ),
    m_mainDistAcc        ( NULL      ),
    m_mainDistSpline     ( NULL      ),
    m_weightedDistAcc    ( NULL      ),
    m_weightedDistSpline ( NULL      ),
    m_interType          ( interType )
  {
    initInterpolator ( x, y, size, interType );
  }

  //============================================================================

  // Constructor from std::vector
  inline TabulatedFunction1D::TabulatedFunction1D( const std::vector<double> & x,
                                                   const std::vector<double> & y,
                                                   const gsl_interp_type * interType ) :
    m_OK                 ( false     ),
    m_mainDistAcc        ( NULL      ),
    m_mainDistSpline     ( NULL      ),
    m_weightedDistAcc    ( NULL      ),
    m_weightedDistSpline ( NULL      ),
    m_interType          ( interType )
  {
    initInterpolator ( x, y, interType );
  }

  //============================================================================

  // Constructor from map
  inline TabulatedFunction1D::TabulatedFunction1D( const std::map<double,double> & data,
                                                   const gsl_interp_type * interType ) :
    m_OK                 ( false     ),
    m_mainDistAcc        ( NULL      ),
    m_mainDistSpline     ( NULL      ),
    m_weightedDistAcc    ( NULL      ),
    m_weightedDistSpline ( NULL      ),
    m_interType          ( interType )
  {
    initInterpolator( data, interType );
  }

  //============================================================================

  // Constructor from vector of pairs
  inline 
  TabulatedFunction1D::TabulatedFunction1D( const std::vector< std::pair<double,double> > & data,
                                            const gsl_interp_type * interType ) :
    m_OK                 ( false     ),
    m_mainDistAcc        ( NULL      ),
    m_mainDistSpline     ( NULL      ),
    m_weightedDistAcc    ( NULL      ),
    m_weightedDistSpline ( NULL      ),
    m_interType          ( interType )
  {
    initInterpolator( data, interType );
  }

  //============================================================================

  inline double TabulatedFunction1D::value ( const double x ) const
  {
    return gsl_spline_eval( m_mainDistSpline, x, m_mainDistAcc );
  }

  //============================================================================

  inline double TabulatedFunction1D::operator [] ( const double x ) const
  {
    return value( x );
  }

  //============================================================================

  inline double TabulatedFunction1D::integral ( const double from,
                                                const double to ) const
  {
    return gsl_spline_eval_integ( m_mainDistSpline, from, to, m_mainDistAcc );
  }

  //============================================================================

  inline double TabulatedFunction1D::meanX ( const double from,
                                             const double to ) const
  {
    const double bot = integral( from, to );
    return ( fabs(bot) > 0 ?
             gsl_spline_eval_integ( m_weightedDistSpline,
                                    from, to, m_weightedDistAcc )/bot : 0 );
  }

  //============================================================================

  inline double TabulatedFunction1D::minX () const
  {
    return (*m_data.begin()).first;
  }

  //============================================================================

  inline double TabulatedFunction1D::minY() const
  {
    return (*m_data.begin()).second;
  }

  //============================================================================

  inline double TabulatedFunction1D::maxX () const
  {
    return (*(--m_data.end())).first;
  }

  //============================================================================

  inline double TabulatedFunction1D::maxY() const
  {
    return (*(--m_data.end())).second;
  }

  //============================================================================

  inline double TabulatedFunction1D::firstDerivative( const double x ) const
  {
    return gsl_spline_eval_deriv( m_mainDistSpline, x, m_mainDistAcc );
  }

  //============================================================================

  inline double TabulatedFunction1D::secondDerivative( const double x ) const
  {
    return gsl_spline_eval_deriv2( m_mainDistSpline, x, m_mainDistAcc );
  }

  //============================================================================

  inline bool TabulatedFunction1D::valid() const
  {
    return m_OK;
  }

  //============================================================================

  inline bool TabulatedFunction1D::withinInputRange( const double x ) const
  {
    return ( x <= maxX() && minX() <= x );
  }

  //============================================================================

  inline unsigned int TabulatedFunction1D::nDataPoints() const
  {
    return m_data.size();
  }

  //============================================================================

  inline std::string TabulatedFunction1D::interpName() const
  {
    return ( m_mainDistSpline ?
             std::string(gsl_interp_name(m_mainDistSpline->interp)) : "Undefined" );
  }

  //============================================================================

  inline const gsl_interp_type * TabulatedFunction1D::interType() const
  {
    return m_interType;
  }

  //============================================================================

}

/** backwards compatibility
 *  @todo remove this typedef */
typedef Rich::TabulatedFunction1D Rich1DTabFunc;

#endif // RICHDET_RICH1DTABFUNC_H
