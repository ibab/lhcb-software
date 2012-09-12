// $Id: $
#ifndef LIB_SobelPeakPixelFinder_H
#define LIB_SobelPeakPixelFinder_H 1

#include "RichHPDImageAnalysis/HPDPixel.h"
#include "TH2D.h"

namespace Rich
{
  namespace HPDImage
  {

    /** @class SobelPeakPixelFinder SobelPeakPixelFinder.h
     *
     *  Finds 'peak' pixels after Sobel Filter, for subsequent fit
     *
     *  @author Chris Jones
     *  @date   2011-03-07
     */
    class SobelPeakPixelFinder
    {

    public:

      /** @class Params SobelPeakPixelFinder.
       *
       *  Fit parameters for SobelPeakPixelFinder
       *
       *  @author Chris Jones
       *  @date   02/03/0211
       */
      class Params
      {
      public:
        /// Default Constructor
        Params()
          : cutFractor    ( 2.00 ), // was 2.25
            minBoundary   ( 5    ),
            neighbourFrac ( 0.50 )  { }
      public:
        double       cutFractor;
        unsigned int minBoundary;   ///< Minimum number of pixels for the boundary
        double       neighbourFrac;
      };

    public:

      /// Standard constructor
      SobelPeakPixelFinder( const TH2* hist = NULL,
                            const Params& params = Params() );

      ~SobelPeakPixelFinder( ) { } ///< Destructor

    public:

      /// Finds the HPD boundary pixels
      void findBoundary( Pixel::List & boundary ) const;

    private:

      /// Checks if the given pixel has a neighbour
      bool isPeak( const int COL, const int ROW, const double thr ) const ;

      /// Computes a local threshold around a pixel
      double localThreshold( const int COL, 
                             const int ROW,
                             const int area = 3 ) const;

      /// select neighbouring pixels to include in the boundary
      void selectNeighbours( const int COL, 
                             const int ROW,
                             Pixel::List & boundary,
                             const int area = 1 ) const;

    private:

      const TH2* m_hist ;
      Params m_params;

    };

  }
}

#endif // LIB_HPDBOUNDARYFCN_H
