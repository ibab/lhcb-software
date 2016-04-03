
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
  // copy data to temporary map
  Data data;
  for ( int i = 0; i < size; ++i ) { data[ x[i] ] = y[i]; }

  // initialise interpolation
  return ( m_OK = initInterpolator( data, interType ) );
}

//============================================================================

bool TabulatedFunction1D::initInterpolator( const std::vector<double> & x,
                                            const std::vector<double> & y,
                                            const gsl_interp_type * interType )
{
  // Check on size of containers
  if ( x.size() != y.size() )
  {
    m_OK = false;
    initInterpolator();
    throw GaudiException( "Size of x vector != size of y vector",
                          "*Rich::TabulatedFunction1D*", StatusCode::FAILURE );
  }
  else
  {
    // copy data to temporary map
    Data data;
    for ( auto ix(x.begin()), iy(y.begin());
          ix != x.end(); ++ix, ++iy ) { data[*ix] = *iy; }

    // initialise interpolation
    m_OK = initInterpolator( data, interType );
  }

  return m_OK;
}

//============================================================================

bool
TabulatedFunction1D::initInterpolator( const std::vector< std::pair<double,double> > & data,
                                       const gsl_interp_type * interType )
{
  // copy data to temporary map
  Data data_map;
  for ( auto i = data.begin(); i != data.end(); ++i ) { data_map[i->first] = i->second; }

  // initialise interpolation
  return ( m_OK = initInterpolator( data_map, interType ) );
}

//============================================================================

bool TabulatedFunction1D::initInterpolator( const std::map<double,double> & data,
                                            const gsl_interp_type * interType )
{

  // clean up first
  clearInterpolator();

  // set interpolator type
  if ( nullptr != interType ) m_interType = interType;

  // Create the GSL interpolators
  m_mainDistAcc        = gsl_interp_accel_alloc();
  m_weightedDistAcc    = gsl_interp_accel_alloc();
  m_mainDistSpline     = gsl_spline_alloc ( m_interType, data.size() );
  m_weightedDistSpline = gsl_spline_alloc ( m_interType, data.size() );

  // Check number of points needed to work ...
  const auto min_points = gsl_interp_min_size(m_mainDistSpline->interp);
  if ( data.size() < min_points )
  {
    std::ostringstream mess;
    mess << "Error whilst initialising GSL interpolator : Type '" << interpName()
         << "' requires a minimum of " << min_points << " data points. Only given "
         << data.size();
    initInterpolator();
    throw GaudiException( mess.str(), "*Rich::TabulatedFunction1D*", StatusCode::FAILURE );
    return false;
  }

  // Copy data to temporary initialisation arrays
  double * x  = new double[data.size()];
  double * y  = new double[data.size()];
  double * xy = new double[data.size()];
  unsigned int i = 0;
  for ( auto iD = data.begin(); iD != data.end(); ++iD, ++i )
  {
    x[i]  = (*iD).first;
    y[i]  = (*iD).second;
    xy[i] = x[i]*y[i];
  }

  // Initialise the interpolators
  const auto err1 = gsl_spline_init ( m_mainDistSpline,     x, y,  data.size() );
  const auto err2 = gsl_spline_init ( m_weightedDistSpline, x, xy, data.size() );

  // delete temporary arrays
  delete[] x;
  delete[] y;
  delete[] xy;

  if ( err1 || err2 )
  {
    initInterpolator();
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
    m_mainDistSpline = nullptr;
  }
  if ( m_mainDistAcc )
  {
    gsl_interp_accel_free( m_mainDistAcc );
    m_mainDistAcc = nullptr;
  }
  if ( m_weightedDistSpline )
  {
    gsl_spline_free( m_weightedDistSpline );
    m_weightedDistSpline = nullptr;
  }
  if ( m_weightedDistAcc )
  {
    gsl_interp_accel_free( m_weightedDistAcc );
    m_weightedDistAcc = nullptr;
  }
}

//============================================================================

void TabulatedFunction1D::initInterpolator()
{
  // remove any existing interpolators
  clearInterpolator();
  // initialise with defaults
  m_mainDistAcc         = gsl_interp_accel_alloc();
  m_weightedDistAcc     = gsl_interp_accel_alloc();
  const auto min_points = 3;
  m_mainDistSpline      = gsl_spline_alloc ( m_interType, min_points );
  m_weightedDistSpline  = gsl_spline_alloc ( m_interType, min_points );
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
  const auto xInc = (to-from)/(double)(samples-1);

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
  const auto avgX = meanX(from,to);

  // x increment
  const auto xInc = (to-from)/(double)(samples-1);

  double sd(0), sum(0), X(from);
  for ( unsigned int i = 0; i < samples; ++i, X += xInc )
  {
    const auto Y = value(X) * ( weightFunc ? weightFunc->value(X) : 1.0 );
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

std::unique_ptr<TabulatedFunction1D> 
TabulatedFunction1D::combine( const ConstVector & funcs,
                              const unsigned int samples,
                              const gsl_interp_type * interType )
{
  if ( samples < 2 )
  {
    throw GaudiException( "combine() : samples must be > 1",
                          "*Rich::TabulatedFunction1D*", StatusCode::FAILURE );
  }
  
  // Default top a nullptr pointer. Filled later on.
  TabulatedFunction1D * combFunc = nullptr;

  // Get global min and max range of function
  auto maxX(boost::numeric::bounds<double>::highest());
  auto minX(boost::numeric::bounds<double>::lowest());
  for ( const auto F : funcs )
  {
    if ( F->minX() > minX ) { minX = F->minX(); }
    if ( F->maxX() < maxX ) { maxX = F->maxX(); }
  }

  // Check all is OK
  if ( minX < maxX )
  {

    // x increment
    const auto xInc = (maxX-minX)/(double)(samples-1);

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
  return std::unique_ptr<TabulatedFunction1D>(combFunc);
}

//============================================================================

double 
TabulatedFunction1D::rangeWarning( const double x, const double retx ) const
{
  std::cerr << "Rich::TabulatedFunction1D : WARNING : Out-Of-Range x = " << x
            << " Valid Range = " << minX() << " to " << maxX() << std::endl;
  return retx;
}

//============================================================================
