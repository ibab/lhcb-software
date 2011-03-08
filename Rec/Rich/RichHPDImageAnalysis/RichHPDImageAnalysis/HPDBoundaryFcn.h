// $Id: $
#ifndef LIB_HPDBOUNDARYFCN_H
#define LIB_HPDBOUNDARYFCN_H 1

#include "RichHPDImageAnalysis/HPDPixel.h"
#include "TH2D.h"

namespace Rich
{
  namespace HPDImage
  {

    /** @class HPDBoundaryFcn HPDBoundaryFcn.h
     *
     *  Boundary function for HPD fit
     *
     *  @author Thomas BLAKE
     *  @date   2010-03-16
     */
    class HPDBoundaryFcn
    {

    public:

      /** @class Params HPDBoundaryFcn.h
       *
       *  Fit parameters for HPDBoundaryFcn
       *
       *  @author Chris Jones
       *  @date   02/03/0211
       */
      class Params
      {
      public:
        Params()
          : cutFraction ( 0.1 ),
            minBoundary ( 5   ) { }
      public:
        double       cutFraction;
        unsigned int minBoundary;
      };

    public:

      /// Standard constructor
      HPDBoundaryFcn( const TH2* hist      = NULL,
                      const Params& params = Params() );

      ~HPDBoundaryFcn( ) {} ///< Destructor

    public:
      
      /// Finds the HPD boundary pixels
      void findBoundary( Pixel::List & boundary ) const;

    private:

      /// Checks if the given pixel has a neighbour
      bool hasNeighbour( const int COL, 
                         const int ROW, 
                         const double thr,
                         const int area = 1 ) const ;

      /// Add a pixel to the bounary list
      void addPixel( const int col, 
                     const int row,
                     Pixel::List & boundary ) const;

    private:

      const TH2* m_hist ;
      Params m_params;

    };

  }
}

#endif // LIB_HPDBOUNDARYFCN_H
