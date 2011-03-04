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

      /// A data point, containing (x,y) data points, and the errors on x and y
      class Point
      {
      public:
        Point( const double _x  =  0,
               const double _y  =  0,
               const double _ex = -1,
               const double _ey = -1 ) : x(_x),y(_y),ex(_ex),ey(_ey) { }
      public:
        double x,y,ex,ey;
      };

      /// Type for data container
      typedef std::vector<Point> Data;

    public:

      /// Standard constructor
      GraphSmoother( ) { }

      /// Constructor from (x+-ex,y+-ey) data values 
      GraphSmoother( const unsigned int nPoints,
                     const double* _x,
                     const double* _y,
                     const double* _ex,
                     const double* _ey );

      /// Constructor from (x,y) data values 
      GraphSmoother( const unsigned int nPoints,
                     const double* _x,
                     const double* _y );

      ~GraphSmoother( ) { } ///< Destructor

    public:

      /** Get the 'smoothed' value at the given x
       *  @param x     The x value to evaulate the smoothed function at
       *  @param sigma The sigma to use in the smoothing function
       *  @return The smoothed value of the function at x
       */
      double Eval( const double x,
                   const double sigma = 25 ) const;

    private:

      Data m_data; ///< The data ( list of (x,y) values )
 
    };

  }
}

#endif // RICHHPDIMAGEANALYSIS_GRAPHSMOOTHER_H
