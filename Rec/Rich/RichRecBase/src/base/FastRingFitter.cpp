
//-----------------------------------------------------------------------------
/** @file FastRingFitter.cpp
 *
 *  Implementation file for class : Rich::Rec::FastRingFitter
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-01
 */
//-----------------------------------------------------------------------------

// stl
#include <cmath>

// local
#include "RichRecBase/FastRingFitter.h"

// event
#include "Event/RichRecRing.h"

using namespace Rich::Rec;

//-----------------------------------------------------------------------------

void FastRingFitter::clear()
{
  // clear input data and output results
  m_X.clear();
  m_Y.clear();
  m_result = Result();
}

const FastRingFitter::Result& FastRingFitter::fit()
{

  double xx(0), yy(0), xx2(0), yy2(0);
  double f(0), g(0), h(0), p(0), q(0), t(0), g0(0), g02(0);
  double  a(0), b(0), c(0), d(0);
  double xroot(0), ff(0), fp(0), xd(0), yd(0), g1(0);
  double dx(0), dy(0), dradius2(0), xnom(0);

  // initialise fit
  double xgravity = 0.0;
  double ygravity = 0.0;
  double x2       = 0.0;
  double y2       = 0.0;
  double xy       = 0.0;
  double xx2y2    = 0.0;
  double yx2y2    = 0.0;
  double x2y22    = 0.0;
  double radius2  = 0.0;

  m_result.Status = 0; // default status set to OK

  // check number of fit points > 4
  const unsigned int npoints = numberOfPoints();
  if ( npoints <= 3 ) { m_result.Status = 1; return result(); }

  // average point
  for ( unsigned int i = 0; i<npoints; ++i )
  {
    xgravity += m_X[i];
    ygravity += m_Y[i];
  }
  xgravity /= npoints;
  ygravity /= npoints;

  {for ( unsigned int i = 0; i < npoints; ++i )
  {
    xx  = m_X[i]-xgravity;
    yy  = m_Y[i]-ygravity;
    xx2 = xx*xx;
    yy2 = yy*yy;
    x2  += xx2;
    y2  += yy2;
    xy  += xx*yy;
    xx2y2 += xx*(xx2+yy2);
    yx2y2 += yy*(xx2+yy2);
    x2y22 += (xx2+yy2)*(xx2+yy2);
  }}
  if ( 0 == xy ) { m_result.Status = 2; return result(); }

  f = (3.*x2+y2)/npoints;
  g = (x2+3.*y2)/npoints;
  h = 2*xy/npoints;
  p = xx2y2/npoints;
  q = yx2y2/npoints;
  t = x2y22/npoints;
  g0 = (x2+y2)/npoints;
  g02 = g0*g0;
  a = -4.0;
  b = (f*g-t-h*h)/g02;
  c = (t*(f+g)-2.*(p*p+q*q))/(g02*g0);
  d = (t*(h*h-f*g)+2.*(p*p*g+q*q*f)-4.*p*q*h)/(g02*g02);
  xroot = 1.0;
  {for ( int i = 0; i<5; ++i )
  {
    ff = (((xroot+a)*xroot+b)*xroot+c)*xroot+d;
    fp = ((4.*xroot+3.*a)*xroot+2.*b)*xroot+c;
    xroot -= ff/fp;
  }}
  g1 = xroot*g0;
  xnom = (g-g1)*(f-g1)-h*h;
  if ( 0 == xnom ) { m_result.Status = 3; return result(); }

  yd = (q*(f-g1)-h*p)/xnom;
  xnom = f-g1;
  if ( 0 == xnom ) { m_result.Status = 4; return result(); }
  xd = (p-h*yd )/xnom;

  radius2 = xd*xd+yd*yd+g1;
  m_result.XCenter = xd+xgravity;
  m_result.YCenter = yd+ygravity;

  {for ( unsigned int i = 0; i < npoints; ++i )
  {
    dx = m_X[i]-(m_result.XCenter);
    dy = m_Y[i]-(m_result.YCenter);
    dradius2 = dx*dx+dy*dy;
    m_result.Variance += dradius2+radius2-2.*std::sqrt(dradius2*radius2);
  }}
  m_result.Variance /= npoints-3.0;

  m_result.Radius = std::sqrt(radius2);
  m_result.Status = 0;

  return result();
}

void FastRingFitter::addPoints( const LHCb::RichRecRing & ring )
{
  // clean current data list
  clear();

  // Radiator info
  const Rich::RadiatorType rad = ring.radiator();

  // loop over hits on the ring
  m_X.reserve( ring.richRecPixels().size() );
  m_Y.reserve( ring.richRecPixels().size() );
  for ( const auto & P : ring.richRecPixels() )
  {
    // get pixel from pixelOnRing
    const LHCb::RichRecPixel * pixel = P.pixel();
    // pixel hit position
    const Gaudi::XYZPoint & pos = ( rad != Rich::InvalidRadiator ?
                                    pixel->radCorrLocalPositions().position(rad) :
                                    pixel->localPosition() );
    // add (x,y) point to the fitter
    addPoint( pos.x(), pos.y() );
  }

}
