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

      /// Type for data container
      typedef std::vector< std::pair<double,double> > Data;

    public:

      /// Standard constructor
      GraphSmoother( ) { }

      /// Constructor from data values (python friendly...)
      GraphSmoother( const double* _x,
                     const double* _y,
                     const unsigned int nPoints );

      ~GraphSmoother( ) { } ///< Destructor

    public:

      /// Get the 'smoothed' value at the given x
      double Eval( const double x,
                   const double sigma = 25 ) const;

    private:

      Data m_data; ///< The data ( list of (x,y) values )
 
    };

  }
}

#endif // RICHHPDIMAGEANALYSIS_GRAPHSMOOTHER_H
