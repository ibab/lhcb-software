
#include <cmath>

// local
#include "RichHPDImageAnalysis/GraphSmoother.h"

using namespace Rich::HPDImage;

//-----------------------------------------------------------------------------
GraphSmoother::GraphSmoother( const double* _x,
                              const double* _y,
                              const unsigned int nPoints )
{
  m_data.reserve(nPoints);
  for ( unsigned int i = 0; i < nPoints; ++i )
  {
    m_data.push_back( Data::value_type(_x[i],_y[i]) );
  }
}

double GraphSmoother::Eval( const double x,
                            const double sigma ) const
{
  double y(0), sum(0);

  // Loop over all the data and weight by distance from requested point
  for ( Data::const_iterator iD = m_data.begin();
        iD != m_data.end(); ++iD )
  {
    //const double weight = 1.0 / ( 1.0 + std::pow(std::fabs(x-iD->first),1) );
    const double weight = std::exp( -0.5 * std::pow(x-iD->first,2) / (sigma*sigma) );
    y   += weight * iD->second;
    sum += weight;
  }
  
  return ( sum > 0 ? y/sum : 0 );
}
