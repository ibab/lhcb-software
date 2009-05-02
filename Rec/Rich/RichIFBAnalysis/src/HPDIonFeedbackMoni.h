// $Id: HPDIonFeedbackMoni.h,v 1.2 2009-05-02 12:19:01 ryoung Exp $
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

      int m_ionFeedbackCut;      // Define what IFB means: cluster of size this or larger = IFB
      int m_ionFeedbackCutALICE; // Define what IFB means: cluster of size this or larger = IFB (ALICE MODE)
      int m_MonitorRate;       // Defines how often events are monitored (every Xth events)
      int m_dayref;              // Reference day with which dates of savesets are defined (Def. 14365 for 01/05/2009)
      int m_nEvts;               // Total number of events 
      int m_nMonitoredEvents;    // Monitored number of events
      
      bool m_rich1flag;         // RICH1 flag for histo filling in "finalise" method 
      bool m_rich2flag;         // RICH2 flag for histo filling in "finalise" method 
       
    };
  }
}

#endif // RICHIFBANALYSIS_HPDIonFeedbackMoni_H
