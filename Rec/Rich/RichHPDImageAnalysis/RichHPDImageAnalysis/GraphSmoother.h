// $Id: $
#ifndef RICHHPDIMAGEANALYSIS_GRAPHSMOOTHER_H
#define RICHHPDIMAGEANALYSIS_GRAPHSMOOTHER_H 1

#include <vector>

namespace Rich
{
  namespace HPDImage
  {

    /** @class GraphSmoother RichHPDImageAnalysis/GraphSmoother.h
     *
     *  'Smoothes' (x,y) data points for HPD fit
     *
     *  @author Chris Jones
     *  @date   2011-03-03
     */

    class GraphSmoother
    {

    private:

      typedef std::vector< std::pair<double,double> > Data;

    public:

      /// Standard constructor
      GraphSmoother( ) { }

      /// Constructor from data values
      GraphSmoother( const double* _x,
                     const double* _y,
                     const unsigned int nPoints );

      ~GraphSmoother( ) { } ///< Destructor

    public:

      /// Get the 'smoothed' value at the given X
      double Eval( const double x,
                   const double sigma = 100 ) const;

    private:

      Data m_data;
 
    };

  }
}

#endif // RICHHPDIMAGEANALYSIS_GRAPHSMOOTHER_H
