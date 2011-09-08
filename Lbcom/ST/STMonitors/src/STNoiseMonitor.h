// $Id: STNoiseMonitor.h,v 1.9 2009-12-03 18:46:37 mtobin Exp $
#ifndef STNoiseMonitor_H
#define STNoiseMonitor_H 1

// from STKernel
#include "Kernel/STHistoAlgBase.h"

/** @class STNoiseMonitor STNoiseMonitor.h
 *
 *  Class for monitoring the noise of the Tell1's. For each Tell1 the noise
 *  versus the strip number is stored in a histogram. The histograms can be
 *  stored using sourceID or tell1 name with the option \b UseSourceID. The
 *  noise is calculated using a ISTNoiseCalculationTool.  The algorithm can be
 *  configured using the following options:
 *  - \b UpdateRate: Rate at which the histograms are updated (in number of
 *    events). Useful in online mode. Set to -1 to update only at the end.
 *  - \b NoiseToolType: Sets the type of the noise calculation tool. (default is ST::STNoiseCalculationTool)
 *  - \b NoiseToolName: Sets the name of the noise calculation tool. (default is TTNoiseCalculationTool)
 *
 *  Code produces histograms of Noise, Common Mode Subtracted noise and pedestals.
 *
 *  Based on STNZSMonitor from 
 *  @author J. van Tilburg, N. Chiapolini
 *  @date   10/02/2009
 *
 */
class TH2D;

namespace ST {
  class ISTNoiseCalculationTool;

  class STNoiseMonitor : public ST::HistoAlgBase {

  public:
 
    /// Standard constructer
    STNoiseMonitor( const std::string& name, ISvcLocator *svcloc );

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

  private:

    /// Book histograms
    void bookHistograms();

    /// Fill the noise histograms (only called every N events and at finalize)
    void updateNoiseHistogram(unsigned int tell1ID, bool updateTitle=false);

    /// Make some summary plots of things like average noise/link etc...
    void updateSummaryHistograms();

    //const std::string   m_basenameNoiseHisto; 
    int                 m_evtNumber;

    unsigned int m_nTELL1s; ///< Number of TELL1 boards expected.

    // jobOptions:

    /// Location in the Event Data Store with the ADC values
    std::string m_dataLocation;

    /// When set to true: use the sourceID in the histogram name. Otherwise use
    /// the tell1 name.
    bool m_useSourceID;

    /// Rate at which the histograms are updated (in number of events).
    /// Set to -1 to update only at the end.
    int m_updateRate;
  
    /// Dumps noise calculation variables to histograms for debugging
    bool m_checkCalculation;
    /// Plot noise calculation variables (mean, mean squared and rms)
    void dumpNoiseCalculation(unsigned int sourceID);
  
    std::vector< unsigned int > m_limitToTell;/// List of TELL1s to look at
    bool   m_selectedTells;///< Use only selected TELL1s
  
    bool m_useODINTime;///< ODIN time of first event is added to the histogram title in the finalize method
    const LHCb::ODIN* m_ODIN;///< Time of the first event in the run
    std::string m_odinEvent;///< String of the time of the first run

    /// Map of RAW noise histograms booked in initialize
    std::map<int, AIDA::IProfile1D*> m_raw_noiseHistos;

    /// Map of pedestal histograms booked in initialize
    std::map<int, AIDA::IProfile1D*> m_raw_pedestalHistos;

    /// Map of CMS noise histograms booked in initialize
    std::map<int, AIDA::IProfile1D*> m_cms_noiseHistos;

    /// Map of CMS pedestal histograms booked in initialize
    std::map<int, AIDA::IProfile1D*> m_cms_pedestalHistos;

    /// Map of CMS noise histograms booked in initialize
    std::map<int, AIDA::IProfile1D*> m_cm_noiseHistos;

    /// Map of average RAW noise per port histograms booked in initialize
    std::map<int, AIDA::IProfile1D*> m_average_raw_noiseHistos;

    /// Map of average raw pedestal per port histograms booked in initialize
    std::map<int, AIDA::IProfile1D*> m_average_raw_pedestalHistos;

    /// Map of average CMS noise per port histograms booked in initialize
    std::map<int, AIDA::IProfile1D*> m_average_cms_noiseHistos;

    /// Map of average CMS pedestal per port histograms booked in initialize
    std::map<int, AIDA::IProfile1D*> m_average_cms_pedestalHistos;

    /// Map of average CMS per port noise histograms booked in initialize
    std::map<int, AIDA::IProfile1D*> m_average_cm_noiseHistos;

    /// Histograms of all strips combined
    AIDA::IHistogram1D* m_1d_raw_noise;///< Raw noise of each strip
    AIDA::IHistogram1D* m_1d_cms_noise;///< CMS noise of each strip
    AIDA::IHistogram1D* m_1d_cm_noise;///< CM noise of each strip
    AIDA::IHistogram1D* m_1d_raw_pedestal;///< Raw pedestal of each strip
    AIDA::IHistogram1D* m_1d_cms_pedestal;///< CMS pedestal of each strip

    // Maps of histograms by sector type
    std::map<std::string, AIDA::IHistogram1D*> m_1d_raw_noiseByType;///< Raw noise of each strip
    std::map<std::string, AIDA::IHistogram1D*> m_1d_cms_noiseByType;///< CMS noise of each strip
    std::map<std::string, AIDA::IHistogram1D*> m_1d_cm_noiseByType;///< CM noise of each strip
    std::map<std::string, AIDA::IHistogram1D*> m_1d_raw_pedestalByType;///< Raw pedestal of each strip
    std::map<std::string, AIDA::IHistogram1D*> m_1d_cms_pedestalByType;///< CMS pedestal of each strip

    AIDA::IProfile1D* m_1dp_AvRAWNoisePerTell1;///< Average raw noise in each tell1
    AIDA::IProfile1D* m_1dp_AvCMSNoisePerTell1;///< Average cms noise in each tell1
    AIDA::IProfile1D* m_1dp_AvCMNoisePerTell1;///< Average cm noise in each tell1

    TH2D* m_2d_PedestalPerLinkVsTell1;///< 2d map of pedestal vs link
    AIDA::IProfile1D* m_1dp_AvRAWPedestalPerTell1;///< Average raw pedestal in each tell1
    AIDA::IProfile1D* m_1dp_AvCMSPedestalPerTell1;///< Average cms pedestal in each tell1

    TH2D* m_2d_RawNoisePerLinkVsTell1;///< 2d map of raw noise vs link
    TH2D* m_2d_CMSNoisePerLinkVsTell1;///< 2d map of raw noise vs link
    TH2D* m_2d_CMNoisePerLinkVsTell1;///< 2d map of raw noise vs link

    /// 2d map used for normalisation of noise, pedestal plots as 2d profile histograms are not supported by online monitoring
    TH2D* m_2d_NormalisationPerLinkVsTell1;

    ST::ISTNoiseCalculationTool* m_noiseTool; ///< Tool to calculate noise
    std::string m_noiseToolType; ///< Tool type (default is STNoiseCalculationTool)
    std::string m_noiseToolName; ///< Tool name (default is TTNoiseCalculationTool)

    /// Period of the an exponential moving average.(read from the noise tool)
    int m_followingPeriod;

    /// Rate at which the counters for histograms are reset (read from the noise tool)
    int m_resetRate;
  
    /// Number of events to be skipped (read from noise tool)
    int m_skipEvents;

    /// Make summary plots
    bool m_summaryPlots;

    unsigned int m_nStripsInSector;///< number of strips in sector (from STDAQ::nStripsInXTSector where X== T or I)

    std::map<int, std::vector<DeSTSector*> > m_sectorMap;///< Map of DeSTSectors (for use filling / type histograms)
    std::map<std::string, std::string> m_types;///< keep map of sector types to book / type histograms

  };
}
#endif // STNoiseMonitor_H
