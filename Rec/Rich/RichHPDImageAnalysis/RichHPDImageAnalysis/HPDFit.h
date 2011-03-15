// $Id: $
#ifndef RICHHPDIMAGEANALYSIS_HPDFIT_H
#define RICHHPDIMAGEANALYSIS_HPDFIT_H 1

#include <iostream>
#include "RichHPDImageAnalysis/HPDPixel.h"
#include "TH2D.h"

namespace Rich
{
  namespace HPDImage
  {

    /** @class HPDFit RichHPDImageAnalysis/RichHPDFit.h
     *
     *  Simple class to do a fit to the image in an HPD
     *
     *  @author Chris Jones
     *  @date   02/03/0211
     */
    class HPDFit
    {

    public:

      /** @class Params RichHPDImageAnalysis/RichHPDFit.h
       *
       *  Fit parameters for HPDFit
       *
       *  @author Chris Jones
       *  @date   02/03/0211
       */
      class Params
      {
      public:
        /// Default Constructor
        Params() : type           ( "Sobel" ),
                   cleanHistogram (  true   ),
                   maxImageShift  (  3.0    ) // in mm
        { }
      public:
        std::string type;
        bool cleanHistogram;
        double maxImageShift;
      public:
        /// Overload output to ostream
        friend inline std::ostream& operator << ( std::ostream& os,
                                                  const Params & params )
        { return params.fillStream(os); }
      private:
        /// Print this object
        std::ostream& fillStream( std::ostream& os ) const;
      };

      /** @class Params RichHPDImageAnalysis/RichHPDFit.h
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
        Result() : m_OK ( false ),
                   m_row ( 0 ), m_rowErr ( 0 ),
                   m_col ( 0 ), m_colErr ( 0 ),
                   m_rad ( 0 ), m_radErr ( 0 )   { }
      public:
        void setOK( const bool OK ) { m_OK = OK; }
      public:
        bool OK() const { return m_OK; }
      public:
        void setRowAndErr( const double val, const double err ) { m_row = val; m_rowErr = err; }
        void setColAndErr( const double val, const double err ) { m_col = val; m_colErr = err; }
        void setRadAndErr( const double val, const double err ) { m_rad = val; m_radErr = err; }
      public:
        double row()         const { return m_row;    }
        double rowErr()      const { return m_rowErr; }
        double col()         const { return m_col;    }
        double colErr()      const { return m_colErr; }
        double radInPix()    const { return m_rad;    }
        double radErrInPix() const { return m_radErr; }
      public:
        double x()    const { return -1.0*(m_col*m_pixelsize + 0.5*m_pixelsize - 0.5*m_siliconx); }
        double xErr() const { return m_colErr * m_pixelsize; }
        double y()    const { return -1.0*(0.5*m_silicony - m_row*m_pixelsize - 0.5*m_pixelsize); }
        double yErr() const { return m_rowErr * m_pixelsize; }
        double radInMM()    const { return m_pixelsize * m_rad;    }
        double radErrInMM() const { return m_pixelsize * m_radErr; }
      public:
        /// Overload output to ostream
        friend inline std::ostream& operator << ( std::ostream& os,
                                                  const Result & result )
        { return result.fillStream(os); }
      private:
        /// Print this object
        std::ostream& fillStream( std::ostream& os ) const;
      private:
        bool m_OK;
        double m_row,m_rowErr;
        double m_col,m_colErr;
        double m_rad,m_radErr;
      };

    public:

      /// Default contructor
      HPDFit() { m_boundaryPixels.reserve(1024); }

      /// Destructor
      ~HPDFit() { }

    public:

      /// Run a fit of the given 2D HPD image histogram
      HPDFit::Result fit ( const TH2D& hist,
                           const Params& params ) const;

      /// Get the boundary pixels (read only access)
      const Pixel::List & boundaryPixels() const { return m_boundaryPixels; }

    private:

      /// List of boundary pixels
      mutable Pixel::List m_boundaryPixels;

    };

  }
}

#endif // RICHHPDIMAGEANALYSIS_HPDFIT_H
