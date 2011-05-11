
#include <cmath>
#include <sstream>

// local
#include "RichHPDImageAnalysis/GraphSmoother.h"

using namespace Rich::HPDImage;

//-----------------------------------------------------------------------------

GraphSmoother::GraphSmoother( const unsigned int nPoints,
                              const double* _x,
                              const double* _y )
{
  m_data.reserve(nPoints);
  for ( unsigned int i = 0; i < nPoints; ++i )
  {
    m_data.push_back( Point(_x[i],_y[i]) );
  }
}

GraphSmoother::GraphSmoother( const unsigned int nPoints,
                              const double* _x,
                              const double* _y,
                              const double* _ex,
                              const double* _ey )
{
  m_data.reserve(nPoints);
  for ( unsigned int i = 0; i < nPoints; ++i )
  {
    m_data.push_back( Point(_x[i],_y[i],_ex[i],_ey[i]) );
  }
}

double GraphSmoother::Eval( const double x,
                            const double sigma ) const
{
  long double y(0), sum(0);

  // Loop over all the data and weight by distance from requested point
  for ( Data::const_iterator iD = m_data.begin();
        iD != m_data.end(); ++iD )
  {
    const long double _sigma = ( iD->ex > 0 ? iD->ex + sigma : sigma );
    long double weight = std::exp( -0.5 * std::pow(x-iD->x,2) / std::pow(_sigma,2) );
    if ( iD->ey > 0 ) { weight /= iD->ey; }
    y   += weight * iD->y;
    sum += weight;
  }
  
  return (double) ( sum > 0 ? y/sum : 0 );
}

std::string GraphSmoother::text() const
{
  std::ostringstream m;
  m << *this;
  return m.str();
}
