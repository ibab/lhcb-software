// $Id: $
#ifndef RICHHPDIMAGEANALYSIS_BOUNDARYOUTLIERREJECTION_H
#define RICHHPDIMAGEANALYSIS_BOUNDARYOUTLIERREJECTION_H 1

#include "RichHPDImageAnalysis/HPDPixel.h"
#include "TH2D.h"

namespace Rich
{
  namespace HPDImage
  {

    /** @class BoundaryOutlierRejection RichHPDImageAnalysis/BoundaryOutlierRejection.h
     *
     *  Rejects outliers from boundary
     *
     *  @author Chris Jones
     *  @date   2011-03-07
     */
    class BoundaryOutlierRejection
    {

    public:

      /** @class FitResult RichHPDImageAnalysis/BoundaryOutlierRejection.h
       *
       *  Stores the fit result for BoundaryOutlierRejection
       *
       *  @author Chris Jones
       *  @date   2011-03-07
       */
      class FitResult
      {
      public:
        /// Default Constructor
        FitResult() : row(0), col(0), radius(0) { }
      public:
        double row,col,radius;
      };
      /** @class Params RichHPDImageAnalysis/BoundaryOutlierRejection.h
       *
       *  Parameters for BoundaryOutlierRejection
       *
       *  @author Chris Jones
       *  @date   2011-03-07
       */
      class Params
      {
      public:
        /// Default Constructor
        Params() : maxDistance(1.5) { }
      public:
        double maxDistance;
      };

    public:

      /// Standard constructor
      BoundaryOutlierRejection( const TH2* hist,
                                const Pixel::List& list,
                                const FitResult& result,
                                const Params& params = Params() );

      ~BoundaryOutlierRejection( ) { } ///< Destructor

    public:

      /// Run the outlier rejection
      void reject( Pixel::List & outliersRemoved ) const;

    private:

      Pixel::List m_boundary; ///< List of pixels used in the fit
      FitResult m_result;     ///< The fit result
      Params m_params;        ///< Parameters for outlier rejection
      double m_sf ;           ///< Cached scale factor

    };

  }
}

#endif // RICHHPDIMAGEANALYSIS_BOUNDARYOUTLIERREJECTION_H
