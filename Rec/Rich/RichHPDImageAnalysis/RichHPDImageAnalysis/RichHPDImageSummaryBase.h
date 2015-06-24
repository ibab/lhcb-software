// $Id: RichHPDImageSummary.h,v 1.2 2009/11/25 18:35:55 tblake Exp $
#ifndef RICHHPDIMAGESUMMARYBASE_H
#define RICHHPDIMAGESUMMARYBASE_H 1

// Base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// RICH
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "RichDet/DeRichSystem.h"

// STL
#include <map>
#include <vector>
#include <cmath>
#include <sstream>

// Local
#include "RichHPDImageAnalysis/HPDFit.h"

// Histogramming
#include "AIDA/IHistogram2D.h"
#include "TH2D.h"
#include "GaudiUtils/Aida2ROOT.h"

namespace Rich
{

  /** @namespace HPDImage
   *
   *  General namespace for RICH HPD image analysis code
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   02/03/0211
   */
  namespace HPDImage
  {

    /** @class SummaryBase RichHPDImageSummaryBase.h
     *
     *  base class for HPD Imagine monitors
     *
     *  @author Thomas BLAKE
     *  @date   2010-03-16
     */
    class SummaryBase : public Rich::Rec::HistoAlgBase
    {

    public:

      /// Standard constructor
      SummaryBase( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~SummaryBase( ); ///< Destructor

      virtual StatusCode initialize();    ///< Algorithm initialization

      virtual StatusCode execute   ();    ///< Algorithm execution

      virtual StatusCode finalize  ();    ///< Algorithm finalization

    protected:

      /// Perform fit and publish HPD summary information
      void summaryINFO( const LHCb::RichSmartID id, const TH2D* hist ) const;

      //  Conditional booking of 2D histograms
      TH2D* create2D( const std::string& name )  ;

      /// Calculate the local x position from a col nr in pixels
      double localXFromPixels( const double col ) const ;

      /// Calculate the local y position from a row nr in pixels
      double localYFromPixels( const double row ) const ;

      /// Convert error in pixels to error in local positions
      double localErrorFromPixels( const double pixerr ) const;

      /// Distance from image centre to existing CondDB value
      double distanceToCondDBValue( const Rich::DAQ::HPDCopyNumber copyNumber,
                                    const double x0,
                                    const double y0 ) const ;

      /// Get 2D image histograms, given a smart ID
      TH2D* getHPDImageHist( const LHCb::RichSmartID& smartID ) ;
      
    protected:
      
      /// Fit paramaters
      HPDFit::Params m_params;
      
    private:

      /// Minimum number of hits required in HPD
      unsigned int m_minOccupancy ;

      /// Pointer to Rich Sys Detector Element
      const DeRichSystem * m_RichSys;

      /// Raw Buffer Decoding tool
      const Rich::DAQ::IRawBufferToSmartIDsTool * m_SmartIDDecoder;

      /// Type for mapping between HPD and histogram
      typedef std::map< LHCb::RichSmartID , TH2D* > PD2Histo;

      /// Map between HPD CopyNr and Histogram pointer
      PD2Histo m_histo;

      /// Flag for comparison against existing CondDB value
      bool m_compareCondDB;

      /// Maximum allowed movement before triggering the update
      double m_maxMovement;

      /// Flag to turn on/off the saving of 2D histograms
      bool m_keep2Dhistos;

      /// Flag to turn on/off the final HPD fitting and summary histograms
      bool m_finalFit;

      /// HPD Fitter
      mutable Rich::HPDImage::HPDFit m_fitter;

    };

  } // namespae Mon

} // namespace Rich

//=============================================================================

#endif // RICHHPDIMAGEMOVEMENTBASE_H
