// $Id: RichHPDImageSummary.h,v 1.2 2009/11/25 18:35:55 tblake Exp $
#ifndef RICHHPDIMAGESUMMARY_H
#define RICHHPDIMAGESUMMARY_H 1

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// RICH
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "RichDet/DeRichSystem.h"

// Histogramming
#include "AIDA/IHistogram2D.h"
#include "TH2D.h"
#include "GaudiUtils/Aida2ROOT.h"

// Event Model
#include "Event/ODIN.h"

// Det Desc
#include "DetDesc/IGeometryInfo.h"

// STL
#include <map>
#include <vector>
#include <cmath>

// Minuit2
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/FCNBase.h"

// Boost
#include "boost/lexical_cast.hpp"

namespace Rich
{
  namespace Mon
  {

    /** @class RichHPDImageSummary RichHPDImageSummary.h
     *
     *  Monitors the HPD image movements within an HPD
     *
     *  @author Thomas BLAKE
     *  @date   2010-03-16
     */
    class RichHPDImageSummary : public Rich::Rec::HistoAlgBase
    {

    private:

      /** @class RichHPDImageSummary RichHPDImageSummary.h
       *
       *  Boundary function for HPD fit
       *
       *  @author Thomas BLAKE
       *  @date   2010-03-16
       */
      class HPDBoundaryFcn : public ROOT::Minuit2::FCNBase
      {

      public:
        /// Standard constructor
        HPDBoundaryFcn( const TH2* hist = NULL, 
                        const double thr = 0.0 );
        virtual double operator()(const std::vector<double>& par ) const ;
        virtual double Up() const{  return m_errDef; }
        void setErrDef( const double def ){ m_errDef = def; }

        int findBoundary() ;
        double nPixels() const ;

        virtual ~HPDBoundaryFcn( ); ///< Destructor

      protected:

        bool hasNeighbour( const TH2* hist, const int COL, const int ROW, const double thr ) const ;

      private:

        double m_errDef ;
        double m_threshold ;
        const TH2* m_hist ;
        double m_sf ;
        std::vector< std::pair< int, int > > m_boundary ;

      };

    public:

      /// Standard constructor
      RichHPDImageSummary( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~RichHPDImageSummary( ); ///< Destructor

      virtual StatusCode initialize();    ///< Algorithm initialization

      virtual StatusCode execute   ();    ///< Algorithm execution

      virtual StatusCode finalize  ();    ///< Algorithm finalization

    private:

      /// Perform fit and publish HPD summary information
      void summaryINFO( const unsigned int ID, const TH2D* hist ) const;

      //  Conditional booking of 2D histograms
      TH2D* create2D( const std::string& name )  ;

      /// Calculate the local x position from a col nr in pixels
      double localXFromPixels( const double col ) const ;

      /// Calculate the local y position from a row nr in pixels
      double localYFromPixels( const double row ) const ;

      /// Convert error in pixels to error in local positions
      double localErrorFromPixels( const double pixerr ) const;

      /// Distance from image centre to existing CondDB value
      double distanceToCondDBValue( const unsigned int ID,
                                    const double x0,
                                    const double y0 ) const ;

    private:

      /// Event Counter
      int m_nEvt ;

      /// Fraction of average pixel value used to define image boundary
      double m_cutFraction ;

      /// Display Smart ID warnings
      bool m_displayWarnings ;

      /// Minimum number of hits required in HPD
      int m_minOccupancy ;

      /// Minimum number of pixels needed to define HPD edge
      int m_minBoundary ;

      /// Pointer to Rich Sys Detector Element
      const DeRichSystem * m_RichSys;

      /// Raw Buffer Decoding tool
      const Rich::DAQ::IRawBufferToSmartIDsTool * m_SmartIDDecoder;

      /// Map between HPD CopyNr and Histogram pointer
      std::map< unsigned int , TH2D* > m_histo;

      /// Iterator over map of HPD CopyNr and Histogram pointers
      std::map< unsigned int , TH2D* >::iterator  m_iter;

      /// Pixel size in mm
      double m_pixelsize ;

      /// Silicon size in mm
      double m_siliconx ;
      double m_silicony ;

      /// Flag for comparison against existing CondDB value
      bool m_compareCondDB;

      /// Maximum allowed movement before triggering the update
      double m_maxMovement;

      /// Flag to turn on the saving of 2D histograms
      bool m_keep2Dhistos;

    };

  } // namespae Mon

} // namespace Rich

//=============================================================================

inline double
Rich::Mon::RichHPDImageSummary::localXFromPixels( const double col ) const
{
  return col*m_pixelsize + 0.5*m_pixelsize - 0.5*m_siliconx ;
}

inline double
Rich::Mon::RichHPDImageSummary::localYFromPixels( const double row ) const
{
  return 0.5*m_silicony - row*m_pixelsize - 0.5*m_pixelsize ;
}

inline double
Rich::Mon::RichHPDImageSummary::localErrorFromPixels( const double pixerr ) const
{
  return pixerr * m_pixelsize;
}

#endif // RICHHPDIMAGEMOVEMENT_H
