// $Id: $
#ifndef LIB_HPDBOUNDARYFCN_H
#define LIB_HPDBOUNDARYFCN_H 1

#include "RichHPDImageAnalysis/HPDPixel.h"
#include "TH2D.h"

namespace Rich
{
  namespace HPDImage
  {

    /** @class RichHPDImageSummary RichHPDImageSummary.h
     *
     *  Boundary function for HPD fit
     *
     *  @author Thomas BLAKE
     *  @date   2010-03-16
     */
    class HPDBoundaryFcn
    {

    public:

      /// Standard constructor
      HPDBoundaryFcn( const TH2* hist = NULL,
                      const double thr = 0.0 );

      virtual ~HPDBoundaryFcn( ); ///< Destructor

    public:

      /// Type for list of boundary pixels
      typedef Rich::HPDImage::Pixel::List Boundary;

    public:

      const Boundary& boundary() const;

    private:

      /// Finds the HPD boundary pixels
      void findBoundary() const;

      /// Checks if the given pixel has a neighbour
      bool hasNeighbour( const int COL, const int ROW, const double thr ) const ;

      /// Add a pixel to the bounary list
      inline void addPixel( const int col, const int row ) const
      {
        m_boundary.push_back( Pixel(col,row,m_hist->GetBinContent(col+1,row+1)) );
      }
      
    private:

      double m_threshold ;
      const TH2* m_hist ;
      mutable Boundary m_boundary ;
      mutable bool m_boundaryFound;

    };

  }
}

#endif // LIB_HPDBOUNDARYFCN_H
