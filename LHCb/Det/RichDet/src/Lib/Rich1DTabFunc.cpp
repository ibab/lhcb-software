
//============================================================================
/** @file Rich1DTabFunc.cpp
 *
 *  Implementation file for class : Rich::TabulatedFunction1D
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-08-13
 */
//============================================================================

// STL
#include <sstream>
#include <cmath>

// GaudiKernel
// Suppress "debug information truncated" warnings on Windows
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "RichDet/Rich1DTabFunc.h"

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

using namespace Rich;

//============================================================================

// Destructor
TabulatedFunction1D::~TabulatedFunction1D( ) { clearInterpolator(); }

//============================================================================

bool TabulatedFunction1D::initInterpolator( const double x[],
                                            const double y[],
                                            const int size,
                                            const gsl_interp_type * interType )
{
  // clear the current data map
  m_data.clear();

  // copy data to internal container
  for ( int i = 0; i < size; ++i ) { m_data[ x[i] ] = y[i]; }

  // initialise interpolation
  return ( m_OK = initInterpolator( interType ) );
}

//============================================================================

bool TabulatedFunction1D::initInterpolator( const std::vector<double> & x,
                                            const std::vector<double> & y,
                                            const gsl_interp_type * interType )
{
  // clear the current data map
  m_data.clear();

  // Check on size of containers
  if ( x.size() != y.size() )
  {
    m_OK = false;
    clearInterpolator();
    throw GaudiException( "Size of x vector != size of y vector",
                          "*Rich::TabulatedFunction1D*", StatusCode::FAILURE );
  }
  else
  {
    // copy data to internal container
    std::vector<double>::const_iterator ix,iy;
    for ( ix = x.begin(), iy = y.begin();
          ix != x.end(); ++ix, ++iy ) { m_data[*ix] = *iy; }

    // initialise interpolation
    m_OK = initInterpolator( interType );
  }

  return m_OK;
}

//============================================================================

bool TabulatedFunction1D::initInterpolator( const std::map<double,double> & data,
                                            const gsl_interp_type * interType )
{
  // clear the current data map
  m_data.clear();

  // update the data map
  m_data = data;

  // initialise interpolation
  return ( m_OK = initInterpolator( interType ) );
}

//============================================================================

bool
TabulatedFunction1D::initInterpolator( const std::vector< std::pair<double,double> > & data,
                                       const gsl_interp_type * interType )
{
  // clear the current data map
  m_data.clear();

  // copy data to internal container
  std::vector< std::pair<double,double> >::const_iterator i;
  for ( i = data.begin(); i != data.end(); ++i ) { m_data[i->first] = i->second; }

  // initialise interpolation
  return ( m_OK = initInterpolator( interType ) );
}

//============================================================================

// initialise the interpolator
bool TabulatedFunction1D::initInterpolator( const gsl_interp_type * interType )
{

  // clean up first
  clearInterpolator();

  // set interpolator type
  if ( NULL != interType ) m_interType = interType;

  // Create the GSL interpolators
  m_mainDistAcc        = gsl_interp_accel_alloc();
  m_weightedDistAcc    = gsl_interp_accel_alloc();
  m_mainDistSpline     = gsl_spline_alloc ( m_interType, m_data.size() );
  m_weightedDistSpline = gsl_spline_alloc ( m_interType, m_data.size() );

  // Check number of points needed to work ...
  const unsigned int min_points = gsl_interp_min_size(m_mainDistSpline->interp);
  if ( m_data.size() < min_points )
  {
    std::ostringstream mess;
    mess << "Error whilst initialising GSL interpolator : Type '" << interpName()
         << "' requires a minimum of " << min_points << " data points. Only given "
         << m_data.size();
    clearInterpolator();
    throw GaudiException( mess.str(), "*Rich::TabulatedFunction1D*", StatusCode::FAILURE );
    return false;
  }

  // Copy data to temporary initialisation arrays
  double * x  = new double[m_data.size()];
  double * y  = new double[m_data.size()];
  double * xy = new double[m_data.size()];
  unsigned int i = 0;
  for ( Data::const_iterator iD = m_data.begin();
        iD != m_data.end(); ++iD, ++i )
  {
    x[i]  = (*iD).first;
    y[i]  = (*iD).second;
    xy[i] = x[i]*y[i];
  }

  // Initialise the interpolators
  const int err1 = gsl_spline_init ( m_mainDistSpline,     x, y,  m_data.size() );
  const int err2 = gsl_spline_init ( m_weightedDistSpline, x, xy, m_data.size() );

  // delete temporary arrays
  delete[] x;
  delete[] y;
  delete[] xy;

  if ( err1 || err2 )
  {
    clearInterpolator();
    throw GaudiException( "Error whilst initialising GSL interpolators",
                          "*TabulatedFunction1D*", StatusCode::FAILURE );
    return false;
  }

  return true;
}

//============================================================================

// clean out the GSL components
void TabulatedFunction1D::clearInterpolator()
{

  // Free GSL components
  if ( m_mainDistSpline )
  {
    gsl_spline_free( m_mainDistSpline );
    m_mainDistSpline = NULL;
  }
  if ( m_mainDistAcc )
  {
    gsl_interp_accel_free( m_mainDistAcc );
    m_mainDistAcc = NULL;
  }
  if ( m_weightedDistSpline )
  {
    gsl_spline_free( m_weightedDistSpline );
    m_weightedDistSpline = NULL;
  }
  if ( m_weightedDistAcc )
  {
    gsl_interp_accel_free( m_weightedDistAcc );
    m_weightedDistAcc = NULL;
  }

}

//============================================================================

double
TabulatedFunction1D::rms( const double from,
                          const double to,
                          const unsigned int samples,
                          const TabulatedFunction1D * weightFunc ) const
{
  if ( samples < 2 )
  {
    throw GaudiException( "rms() : samples must be > 1",
                          "*Rich::TabulatedFunction1D*", StatusCode::FAILURE );
  }

  // x increment
  const double xInc = (to-from)/(double)(samples-1);

  double rms(0), X(from);
  for ( unsigned int i = 0; i < samples; ++i, X += xInc )
  {
    const double Y = value(X) * ( weightFunc ? weightFunc->value(X) : 1.0 );
    if ( Y>0 )
    {
      rms += Y * Y;
    }
  }
  rms /= (double)samples;

  return std::sqrt(rms);
}

//============================================================================

double
TabulatedFunction1D::standardDeviation( const double from,
                                        const double to,
                                        const unsigned int samples,
                                        const TabulatedFunction1D * weightFunc ) const
{
  if ( samples < 2 )
  {
    throw GaudiException( "standardDeviation() : samples must be > 1",
                          "*Rich::TabulatedFunction1D*", StatusCode::FAILURE );
  }

  // mean value
  const double avgX = meanX(from,to);

  // x increment
  const double xInc = (to-from)/(double)(samples-1);

  double sd(0), sum(0), X(from);
  for ( unsigned int i = 0; i < samples; ++i, X += xInc )
  {
    const double Y = value(X) * ( weightFunc ? weightFunc->value(X) : 1.0 );
    if ( Y>0 )
    {
      sd  += Y * std::pow(X-avgX,2);
      sum += Y;
    }
  }
  sd /= sum;

  return std::sqrt(sd);
}

//============================================================================

TabulatedFunction1D * TabulatedFunction1D::combine( const ConstVector & funcs,
                                                    const unsigned int samples,
                                                    const gsl_interp_type * interType )
{
  if ( samples < 2 )
  {
    throw GaudiException( "combine() : samples must be > 1",
                          "*Rich::TabulatedFunction1D*", StatusCode::FAILURE );
  }
  
  // Default top a NULL pointer. Filled later on.
  TabulatedFunction1D * combFunc = NULL;

  // Get global min and max range of function
  double maxX(boost::numeric::bounds<double>::highest());
  double minX(boost::numeric::bounds<double>::lowest());
  for ( ConstVector::const_iterator iF = funcs.begin();
        iF != funcs.end(); ++iF )
  {
    if ( (*iF)->minX() > minX ) { minX = (*iF)->minX(); }
    if ( (*iF)->maxX() < maxX ) { maxX = (*iF)->maxX(); }
  }

  // Check all is OK
  if ( minX < maxX )
  {

    // x increment
    const double xInc = (maxX-minX)/(double)(samples-1);

    // Create the data points
    Data mergedData;
    double X(minX);
    for ( unsigned int i = 0; i < samples; ++i, X += xInc )
    {
      double Y = 1.0;
      for ( ConstVector::const_iterator iF = funcs.begin();
            iF != funcs.end(); ++iF )
      {
        Y *= (*iF)->value(X);
      }
      mergedData[X] = Y;
    }

    // Create the new interpolated function
    combFunc = new TabulatedFunction1D(mergedData,interType);

  }

  // return
  return combFunc;
}
