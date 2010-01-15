// $Id: HPDIonFeedbackMoni.h,v 1.3 2010-01-15 22:50:08 ryoung Exp $
#ifndef RICHIFBANALYSIS_HPDIonFeedbackMoni_H
#define RICHIFBANALYSIS_HPDIonFeedbackMoni_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichPixelClusteringTool.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "RichKernel/RichHistoAlgBase.h"

namespace Rich
{
  namespace Mon
  {

    //-----------------------------------------------------------------------------
    /** @class HPDIonFeedbackMoni HPDIonFeedbackMoni.h
     *
     *  Monitors the Ion Feedback in the RICH HPDs
     *
     *  @author Ross Young
     *  @author Young Min Kim
     *  @author Nicholas Styles
     */
    //-----------------------------------------------------------------------------

    class HPDIonFeedbackMoni : public Rich::HistoAlgBase
    {

    public:

      /// Standard constructor
      HPDIonFeedbackMoni( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~HPDIonFeedbackMoni( ); ///< Destructor

      virtual StatusCode initialize();    ///< Algorithm initialization
      virtual StatusCode execute   ();    ///< Algorithm execution
      virtual StatusCode finalize  ();    ///< Algorithm finalisation 
      
    private:

      //std::vector<IFBrate> m_IFBvect;
      const Rich::DAQ::IRawBufferToSmartIDsTool * m_SmartIDDecoder;
      const Rich::ISmartIDTool * m_idTool;
      const Rich::DAQ::IPixelClusteringTool * m_clusterTool;

      void ExecuteIFB(const Rich::DetectorType RichNum, const Rich::Side PanNum, const unsigned int HPDCol,
                      const unsigned int HPDRow, unsigned int ionfeedbackCut,
                      const LHCb::RichSmartID::Vector  &smartIDHits );
      void ExecuteQuickHitmap(const Rich::DetectorType RichNum, const LHCb::RichSmartID::Vector &smartIDHits);
      
      unsigned int m_ionFeedbackCutLHCB;      // Minimum cluster-size to be defined as IFB (LHCb/PHYSICS mode)
      unsigned int m_ionFeedbackCutALICE; // Minimum cluster-size to be defined as IFB (ALICE mode)
      unsigned int m_MonitorRate;       // Defines how often events are monitored (every Xth events)
      unsigned long m_interval;          // Defines event sizes for hitmaps and IFB calculations
      unsigned long m_nEvts;               // Total number of events 
      unsigned long m_nMonitoredEvents;    // Monitored number of events
      unsigned int dataset;                // Index of dataset 
      unsigned long r1clustersThisEvent;    // Number of non-zero RICH1 clusters in event
      unsigned long r2clustersThisEvent;    // Number of non-zero RICH2 clusters in event
      unsigned int minX, maxX, minY, maxY, binsX, binsY;   // Hitmap limits

      bool m_wantIFB;           // To execute clustering. 
      bool m_wantHitmaps;       // To execute plotting of "All-cluster" hitmap and "IFB-cluster" hitmap. CPU-time consuming. 
      bool m_wantQuickHitmap;   // To execute quick plotting of hitmap. If "true" sets m_wantIFB to "false". 
      bool m_suppresscornpix;   // Suppress corner pixels of an HPD. Used with test pattern trigger on calibration farm
    };
  }
}

#endif // RICHIFBANALYSIS_HPDIonFeedbackMoni_H
