
//-----------------------------------------------------------------------------
/** @file RichHPDFlashMoni.h
 *
 *  Header file for algorithm class : Rich::Rec::HPDFlashMoni
 *
 *  @author Antonis Papanestis   Antonis.Papanestis@cern.ch
 *  @date   11/10/2011
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichHPDFlashMoni_H
#define RICHRECMONITOR_RichHPDFlashMoni_H 1

// STD
#include <sstream>

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// RichKernel
#include "RichKernel/RichPoissonEffFunctor.h"
#include "RichKernel/RichStatDivFunctor.h"
#include "RichKernel/IRichHPDOccupancyTool.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// Interfaces
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "RichKernel/IRichSmartIDTool.h"

namespace Rich
{
  namespace Rec
  {
    //-------------------------------------------------------------------------
    /** @class HPDFlashMoni RichHPDFlashMoni.h
     *
     *  Simple monitor for large events in each HPD
     *
     *  @author Antonis Papanestis   Antonis.Papanestis@cern.ch
     *  @date   11/10/2011
     */
    //-------------------------------------------------------------------------

    class HPDFlashMoni : public Rich::Rec::HistoAlgBase
    {

    public:

      /// Standard constructor
      HPDFlashMoni( const std::string& name,
                    ISvcLocator* pSvcLocator );

      virtual ~HPDFlashMoni( ); ///< Destructor

      virtual StatusCode initialize();    // Algorithm initialization
      virtual StatusCode execute   ();    // Algorithm execution
      virtual StatusCode prebookHistograms();
      /// dump this event in a histogram for the specified detector
      virtual StatusCode dumpEvent(int thisRich);

    private: // data

      /// Raw Buffer Decoding tool
      const Rich::DAQ::IRawBufferToSmartIDsTool * m_decoder;
      const Rich::ISmartIDTool * m_idTool;

      mutable const Rich::IHPDOccupancyTool * m_HpdOccupancyTool;

      /// Pointer to RICH system detector element
      const DeRichSystem * m_richSys;

      std::vector<AIDA::IProfile1D*> m_perCentHistos;
      std::vector<AIDA::IHistogram2D*> m_HpdPanelHistos;
      //std::vector<AIDA::IHistogram2D*> m_hitMaps;
      AIDA::IHistogram1D* m_goodBadEvents;
      AIDA::IProfile1D* m_thresholdHisto;

      int m_operationMode; // 0=absolute, 1=relative

      int m_fixedUpperThreshold;     //< fixed upper threshold
      int m_fixedLowerThreshold;     //< fixed lower threshold
      int m_absThreshold;       //< absolute threshold
      double m_relThreshold;    //< relative threshold
      double m_secondThreshold; //< lower threshold relative to higher

      int m_events;           //< keep track of events
      int m_skipEvents;       //< events to skip

      bool m_dumpEvents;      //< dump events in histograms
      int m_dumpedEvents;     //< dumped events counter
      int m_maxDumpedEvents;  //< max number of dumped events

      bool m_stopSequence;    //< control sequence filter (only bad events pass)

    };
  }
}

#endif // RICHRECMONITOR_RichHPDFlashMoni_H
