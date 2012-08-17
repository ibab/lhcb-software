// $Id: VeloOccupancyMonitor.h,v 1.9 2010-04-04 14:15:44 keaveney Exp $
#ifndef VELORECMONITORS_VELOOCCUPANCYMONITOR_H
#define VELORECMONITORS_VELOOCCUPANCYMONITOR_H 1

// Include files
// -------------
#include "TH1D.h"
#include "TH2D.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

#include "Event/VeloCluster.h"

#include "DetDesc/Condition.h"

#include "Tell1Kernel/ITELL1SensorMap.h"
#include "Tell1Kernel/ITimeStampProvider.h"
#include "Tell1Kernel/IPvssTell1Names.h"

#include "VeloMonitorBase.h"

// from LHCb
#include "Event/ODIN.h"
#include "Hist1D.h"

#include <set>

/** @class VeloOccupancyMonitor VeloOccupancyMonitor.h
 *
 *  @author Kurt Rinnert
 *  @date   2009-08-04
 *
 */

namespace Velo
{

  class VeloOccupancyMonitor : public VeloMonitorBase
  {

    public:
      /// Standard constructor
      VeloOccupancyMonitor(const std::string& name, ISvcLocator* pSvcLocator);

      virtual ~VeloOccupancyMonitor();      ///< Destructor

      virtual StatusCode initialize();    ///< Algorithm initialization
      virtual StatusCode execute();       ///< Algorithm execution
      virtual StatusCode finalize();      ///< Algorithm finalization

    protected:

    private:

      /// Retrieve the VeloClusters
      LHCb::VeloClusters* veloClusters();
      /// Retrieve the ODIN bank
      LHCb::ODIN* getOdinBank();

      /// Monitor the VeloClusters
      void monitorOccupancy();
      /// latch accumulator values and update histograms with them
      void latchOccupancyFromAccu(bool force = false);
      /// update deay/noisy channel histos
      void updateDeadAndNoisy();

      // Data members
      unsigned int m_nstrips;

      std::string m_tae;

      std::map<unsigned int, Condition> m_conditions;
      std::set<unsigned> m_runsseen;

      // tools
      Velo::ITELL1SensorMap* m_tell1Map;
      Velo::ITimeStampProvider* m_timeStamps;
      Velo::IPvssTell1Names* m_pvssTell1Names;

      // Job options
      bool m_writeXML;
      std::string m_xmlDir;
      std::string m_paramName;
      double m_highOccCut; // in percent
      double m_lowOccCut; // in percent
      std::string m_clusterCont;
      unsigned m_occupancyResetFreq;
      bool m_useOdin;
      bool m_xmlDirDate;
      bool m_writeDeadToXML;

      // histograms
      unsigned m_occupancyDenom;
      TH1D* m_histOccSpectAll;
      TH1D* m_histOccSpectLow;
      Velo::Hist1D* m_fastHistOccSpectAll;
      Velo::Hist1D* m_fastHistOccSpectLow;
      TH1D* m_histAvrgSensor;
      TH1D* m_histAvrgSensorPU;
      TH1D* m_histBCIDSpec;


      std::vector< TH1D* > m_stripOccupancyHistPerSensor;
      std::vector< TH1D* > m_channelOccupancyHistPerSensor;
      std::vector< IProfile1D* > m_veloOccVsBunchId;

      std::vector<Velo::Hist1D*> m_occupancies;
      std::vector<Velo::Hist1D*> m_occupanciesCh;
      // we keep a tiny integer based accumulator "histogram" around to
      // keep track of the last few events and update the main
      // histograms periodically
      unsigned char m_inOccAccu; ///< number of events in accu.
      std::vector<std::vector<unsigned char> > m_occAccu; ///< per strip accu
      /// mask which says which groups of 64 strips have been touched
      std::vector<unsigned> m_occAccuTouched;

      TH1D *m_deadPerSensor, *m_noisyPerSensor, *m_deadOrNoisyPerSensor;
      TH2D *m_deadPerSensorAndLink, *m_noisyPerSensorAndLink, *m_deadOrNoisyPerSensorAndLink;
  };
}

#endif // VELORECMONITORS_VELOOCCUPANCYMONITOR_H
