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
        Params() : cutFraction(0.1), minBoundary(3), type("Fit1") { }
      public:
        double cutFraction;
        unsigned int minBoundary;
        std::string type;
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
      private:
        static const double m_pixelsize =  0.5; ///< Pixel size in mm
        static const double m_siliconx  = 16.0; ///< Silicon x size in mm
        static const double m_silicony  = 16.0; ///< Silicon y size in mm
      public:
        Result() : m_OK(false),
                   m_row(0),m_rowErr(0),
                   m_col(0),m_colErr(0),
                   m_rad(0),m_radErr(0)  { }
      public:
        void setOK( const bool OK ) { m_OK = OK; }
      public:
        bool OK() const { return m_OK; }
      public:
        void setRowAndErr( const double val, const double err ) { m_row = val; m_rowErr = err; }
        void setColAndErr( const double val, const double err ) { m_col = val; m_colErr = err; }
        void setRadAndErr( const double val, const double err ) { m_rad = val; m_radErr = err; }
      public:
        double row()    const { return m_row;    }
        double rowErr() const { return m_rowErr; }
        double col()    const { return m_col;    }
        double colErr() const { return m_colErr; }
        double radInPix()    const { return m_col;    }
        double radErrInPix() const { return m_colErr; }
      public:
        double x()    const { return -1.0*(m_col*m_pixelsize + 0.5*m_pixelsize - 0.5*m_siliconx); }
        double y()    const { return -1.0*(0.5*m_silicony - m_row*m_pixelsize - 0.5*m_pixelsize); }
        double xErr() const { return m_colErr * m_pixelsize; }
        double yErr() const { return m_rowErr * m_pixelsize; }
        double radInMM()    const { return m_pixelsize * m_col;    }
        double radErrInMM() const { return m_pixelsize * m_colErr; }
      private:
        bool m_OK;
        double m_row,m_rowErr;
        double m_col,m_colErr;
        double m_rad,m_radErr;
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
