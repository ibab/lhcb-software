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
      TH2D* create2D( const std::string& name )  ;
      
      /// Retrieve ODIN gps time and update counters
      StatusCode timeInformation() ;      
      
    private:
      
      /// Event Counter 
      int m_nEvt ;
      
      /// Fraction of average pixel value used to define image boundary
      double m_cutFraction ;
      
      /// Flag for histogram production  
      bool m_bookHistos;
      
      /// Display Smart ID warnings
      bool m_displayWarnings ;
      
      
      /// Pointer to Rich Sys Detector Element
      const DeRichSystem * m_RichSys;
  
      /// Raw Buffer Decoding tool
      const Rich::DAQ::IRawBufferToSmartIDsTool * m_SmartIDDecoder;
      
      /// Map between HPD CopyNr and Histogram pointer 
      std::map< unsigned int , TH2D* > m_histo;

      /// Iterator over map of HPD CopyNr and Histogram pointers
      std::map< unsigned int , TH2D* >::iterator  m_iter;

    };
  } // namespae Mon
} // namespace Rich 

#endif // RICHHPDIMAGEMOVEMENT_H
