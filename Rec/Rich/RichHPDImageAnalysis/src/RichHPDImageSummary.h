// $Id: RichHPDImageSummary.h,v 1.2 2009/11/25 18:35:55 tblake Exp $
#ifndef RICHHPDIMAGESUMMARY_H 
#define RICHHPDIMAGESUMMARY_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "RichRecBase/RichRecHistoAlgBase.h"

#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "RichDet/DeRichSystem.h"

#include "AIDA/IHistogram2D.h"
#include "TH2D.h"

#include <map>

/** @class RichHPDImageSummary RichHPDImageSummary.h
 *  
 *
 *  @author Thomas BLAKE
 *  @date   2010-03-16
 */


namespace Rich
{
  namespace Mon 
  {

    class RichHPDImageSummary : public RichRecHistoAlgBase  {

    public: 
      /// Standard constructor
      RichHPDImageSummary( const std::string& name, ISvcLocator* pSvcLocator );
      
      virtual ~RichHPDImageSummary( ); ///< Destructor
      
      virtual StatusCode initialize();    ///< Algorithm initialization
      
      virtual StatusCode execute   ();    ///< Algorithm execution
      
      virtual StatusCode finalize  ();    ///< Algorithm finalization
      
    protected:
      
      /// Perform fit and publish HPD summary information
      void summaryINFO( const unsigned int ID, const TH2D* hist ) const;
      
      //  Conditional booking of 2D histograms
      TH2D* create2D( const std::string& name )  ;
      
      /// Calculate the local x position from a col nr in pixels
      double localXFromPixels( const double col ) const ;

      /// Calculate the local y position from a row nr in pixels
      double localYFromPixels( const double row ) const ;
      
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
      
    };
  } // namespae Mon
} // namespace Rich 

#endif // RICHHPDIMAGEMOVEMENT_H
