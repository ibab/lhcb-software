// $Id: $
#ifndef RICHHPDIMAGEANALYSIS_HPDFIT_H
#define RICHHPDIMAGEANALYSIS_HPDFIT_H 1

#include "RichHPDImageAnalysis/HPDPixel.h"
#include "TH2D.h"
#include "RichKernel/RichDAQDefinitions.h"

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
        Params() : type             ( "Sobel" ),
                   cleanHistogram   (  true   ),
                   maxImageShift    (  3.0    ), // in mm
                   maxXYErrDevation ( 99999   ),
                   retryWithLogzImage ( true  )
        { }
      public:
        std::string type;        ///< Fit type
        bool cleanHistogram;     ///< Flag to turn on image cleaning prior to the fit
        double maxImageShift;    ///< Max allowed image shift for a good fit
        double maxXYErrDevation; ///< HPD image fit max errors max deviation
        bool retryWithLogzImage; ///< Retry the fit as needed with a log-z image
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
      public:
        /// Default Constructor
        Result() : m_OK ( false ),
                   m_row ( 0 ), m_rowErr ( 0 ),
                   m_col ( 0 ), m_colErr ( 0 ),
                   m_rad ( 0 ), m_radErr ( 0 ),
                   m_usedLogZ( false ) { }
      public:
        /// Set the fit status
        void setOK( const bool OK ) { m_OK = OK; }
        /// Access the fit status
        bool OK() const { return m_OK; }
      public:
        /// Set the 'Used log-z' flag
        void setUsedLogZ( const bool flag ) { m_usedLogZ = flag; }
        /// Access the 'Used log-z' flag
        bool usedLogZ() const { return m_usedLogZ; }
      public:
        /// Set the row number for the centre point and error
        void setRowAndErr( const double val, const double err ) { m_row = val; m_rowErr = err; }
        /// Set the column number for the centre point and error
        void setColAndErr( const double val, const double err ) { m_col = val; m_colErr = err; }
        /// Set the image radius and error
        void setRadAndErr( const double val, const double err ) { m_rad = val; m_radErr = err; }
      public:
        /// Access the row number for the centre point
        double row()         const { return m_row;    }
        /// Access the row number error for the centre point
        double rowErr()      const { return m_rowErr; }
        /// Access the column number for the centre point
        double col()         const { return m_col;    }
        /// Access the column number error for the centre point
        double colErr()      const { return m_colErr; }
        /// Access the image radius (in pixel units)
        double radInPix()    const { return m_rad;    }
        /// Access the image radius error (in pixel units)
        double radErrInPix() const { return m_radErr; }
      public:
        /// Access the x shift in the image centre position from the centre of the silicon sensor
        double x()    const { return -1.0 * ( m_col*Rich::DAQ::PixelSize + 
                                              0.5*Rich::DAQ::PixelSize - 
                                              0.25*Rich::DAQ::NumPixelColumns ); }
        /// Access the error in the x shift in the image centre position from the centre of the silicon sensor
        double xErr() const { return m_colErr * Rich::DAQ::PixelSize; }
        /// Access the y shift in the image centre position from the centre of the silicon sensor
        double y()    const { return -1.0 * ( 0.25*Rich::DAQ::NumPixelColumns - 
                                              m_row*Rich::DAQ::PixelSize - 
                                              0.5*Rich::DAQ::PixelSize ); }
        /// Access the error in the y shift in the image centre position from the centre of the silicon sensor
        double yErr() const { return m_rowErr * Rich::DAQ::PixelSize; }
        /// Access the radius of the image in mm 
        double radInMM()    const { return m_rad    * Rich::DAQ::PixelSize; }
        ///Access the error on the radius of the image in mm 
        double radErrInMM() const { return m_radErr * Rich::DAQ::PixelSize; }
      public:
        /// Overload output to ostream
        friend inline std::ostream& operator << ( std::ostream& os,
                                                  const Result & result )
        { return result.fillStream(os); }
      private:
        /// Print this object
        std::ostream& fillStream( std::ostream& os ) const;
      private:
        bool m_OK;        ///< Fit status
        double m_row;     ///< Row number of centre point
        double m_rowErr;  ///< Error on the row number of centre point
        double m_col;     ///< Column number of centre point
        double m_colErr;  ///< Error on the column number of centre point
        double m_rad;     ///< Image radius in pixel units
        double m_radErr;  ///< Error on the image radius in pixel units
        bool m_usedLogZ;  ///< Used Log-z image
      };

    public:

      /// Default contructor
      HPDFit();
      
      /// Destructor
      ~HPDFit();
      
    public:

      /// Run a fit of the given 2D HPD image histogram
      HPDFit::Result fit ( const TH2D& hist,
                           const Params& params,
                           const unsigned int nEvents = 0 ) const;

      /// Get the boundary pixels (read only access)
      const Pixel::List & boundaryPixels() const { return m_boundaryPixels; }

      /** Get a copy of the fit histogram (cleaned and filtered as required).
       *  @attention Caller takes ownership of the returned histogram object */
      const TH2D * getFitHistogram( const TH2D& hist,
                                    const Params& params,
                                    const unsigned int nEvents = 0 ) const;

    public:

      /// Check the fit errors for strange deviations
      inline bool errorsOK( const HPDFit::Result& result,
                            const Params& params ) const
      {
        const double avErr = ( result.xErr() + result.yErr() ) / 2.0;
        return ( fabs( result.xErr() - avErr ) < avErr*params.maxXYErrDevation &&
                 fabs( result.yErr() - avErr ) < avErr*params.maxXYErrDevation );
      }

    public:

      /// Create a 'log-z' version of the image histogram
      TH2D * createLogzImage( const TH2D& image ) const;

    private:

      /// List of boundary pixels
      mutable Pixel::List m_boundaryPixels;

    };

  }
}

#endif // RICHHPDIMAGEANALYSIS_HPDFIT_H
