
//============================================================================
/** @file Rich1DTabFunc.cpp
 *
 *  Implementation file for class : Rich1DTabFunc
 *
 *  CVS Log :-
 *  $Id: Rich1DTabFunc.cpp,v 1.3 2006-03-14 14:42:19 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-08-13
 */
//============================================================================

// GaudiKernel
// Suppress "debug information truncated" warnings on Windows
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "RichDet/Rich1DTabFunc.h"

//============================================================================

// Default Constructor
Rich1DTabFunc::Rich1DTabFunc( const gsl_interp_type * interType ) :
  m_OK                 ( false ),
  m_mainDistAcc        ( 0 ),
  m_mainDistSpline     ( 0 ),
  m_weightedDistAcc    ( 0 ),
  m_weightedDistSpline ( 0 ),
  m_interType          ( interType ) { }


// Constructor from arrays
Rich1DTabFunc::Rich1DTabFunc( const double x[],
                              const double y[],
                              const int size,
                              const gsl_interp_type * interType ) :
  m_OK                 ( false ),
  m_mainDistAcc        ( 0 ),
  m_mainDistSpline     ( 0 ),
  m_weightedDistAcc    ( 0 ),
  m_weightedDistSpline ( 0 ),
  m_interType          ( interType )
{

  // copy data to internal container
  for ( int i = 0; i < size; ++i ) { m_data[ x[i] ] = y[i]; }

  // initialise interpolation
  m_OK = initInterpolator( interType );
}

// Constructor from std::vector
Rich1DTabFunc::Rich1DTabFunc( const std::vector<double> & x,
                              const std::vector<double> & y,
                              const gsl_interp_type * interType ) :
  m_OK                 ( false ),
  m_mainDistAcc        ( 0 ),
  m_mainDistSpline     ( 0 ),
  m_weightedDistAcc    ( 0 ),
  m_weightedDistSpline ( 0 ),
  m_interType          ( interType )
{

  // Check on size of containers
  if ( x.size() != y.size() ) 
  {
    m_OK = false;
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

}

//============================================================================

// Constructor from map
Rich1DTabFunc::Rich1DTabFunc( const std::map<double,double> & data,
                              const gsl_interp_type * interType ) :
  m_data               ( data  ),
  m_OK                 ( false ),
  m_mainDistAcc        ( 0 ),
  m_mainDistSpline     ( 0 ),
  m_weightedDistAcc    ( 0 ),
  m_weightedDistSpline ( 0 ),
  m_interType          ( interType )
{
  // initialise interpolation
  m_OK = initInterpolator( interType );
}

//============================================================================
// Destructor
Rich1DTabFunc::~Rich1DTabFunc( ) { clearInterpolator(); }
//============================================================================

// initialise the interpolator
bool Rich1DTabFunc::initInterpolator( const gsl_interp_type * interType ) 
{

  // clean up first
  clearInterpolator();

  // Needs at least 2 points to work...
  const int size = m_data.size();
  if ( size < 2 ) return false;

  // Copy data to temporary initialisation arrays
  double * x  = new double[size];
  double * y  = new double[size];
  double * xy = new double[size];
  unsigned int i = 0;
  for ( Data::const_iterator iD = m_data.begin();
        iD != m_data.end(); ++iD, ++i ) 
  {
    x[i]  = (*iD).first;
    y[i]  = (*iD).second;
    xy[i] = x[i]*y[i];
  }

  // Initialise the GSL interpolators
  m_mainDistAcc        = gsl_interp_accel_alloc();
  m_weightedDistAcc    = gsl_interp_accel_alloc();
  m_mainDistSpline     = gsl_spline_alloc ( interType, size );
  m_weightedDistSpline = gsl_spline_alloc ( interType, size );
  const int err1 = gsl_spline_init ( m_mainDistSpline,     x, y,  size );
  const int err2 = gsl_spline_init ( m_weightedDistSpline, x, xy, size );

  // delete temporary arrays
  delete[] x;
  delete[] y;
  delete[] xy;

  if ( err1 || err2 ) 
  {
    throw GaudiException( "Error whilst initialising GSL interpolators",
                          "*Rich1DTabFunc*", StatusCode::FAILURE );
    return false;
  }

  return true;
}

//============================================================================

void Rich1DTabFunc::clearInterpolator()
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

double Rich1DTabFunc::rms( const double from, 
                           const double to,
                           const unsigned int samples ) const
{
  if ( samples < 2 ) 
  {
    throw GaudiException( "rms() : samples must be > 1",
                          "*Rich1DTabFunc*", StatusCode::FAILURE );
  }

  // mean value
  const double avgX = meanX(from,to);

  // x increment
  const double xInc = (to-from)/(samples-1);

  double rms(0), sum(0);
  for ( unsigned int i = 0; i < samples; ++i )
  {
    const double X = from + i*xInc;
    const double Y = value(X);
    if ( Y>0 )
    {
      rms += Y*(X-avgX)*(X-avgX);
      sum += Y;
    }
  }
  rms /= sum;

  return sqrt(rms);
}
