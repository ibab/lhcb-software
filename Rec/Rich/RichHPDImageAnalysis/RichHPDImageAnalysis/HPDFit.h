// $Id: $
#ifndef RICHHPDIMAGEANALYSIS_HPDFIT_H
#define RICHHPDIMAGEANALYSIS_HPDFIT_H 1

#include "TH2D.h"

namespace Rich
{
  namespace HPDImage
  {

    /** @class HPDFit RichHPDImageSummary.h
     *
     *  Simple class to do a fit to the image in an HPD
     *
     *  @author Chris Jones
     *  @date   02/03/0211
     */
    class HPDFit
    {

    public:

      /** @class Params RichHPDImageSummary.h
       *
       *  Fit parameters for HPDFit
       *
       *  @author Chris Jones
       *  @date   02/03/0211
       */
      class Params
      {
      public:
        Params() : cutFraction(0.1), minBoundary(3) { }
      public:
        double cutFraction;
        unsigned int minBoundary;
      };

      /** @class Params RichHPDImageSummary.h
       *
       *  Fit result for HPDFit
       *
       *  @author Chris Jones
       *  @date   02/03/0211
       */
      class Result
      {
      public:
        Result() : OK(false),
                   row(0),rowErr(0),
                   col(0),colErr(0),
                   rad(0),radErr(0)  { }
      public:
        bool OK;
        double row,rowErr;
        double col,colErr;
        double rad,radErr;
      };

    public:

      /// Default contructor
      HPDFit() { }

    public:

      /// Run a fit of the given 2D HPD image histogram
      const Rich::HPDImage::HPDFit::Result fit ( const TH2D& hist,
                                                 const Rich::HPDImage::HPDFit::Params& params ) const;

    };

  }
}

#endif // RICHHPDIMAGEANALYSIS_HPDFIT_H
