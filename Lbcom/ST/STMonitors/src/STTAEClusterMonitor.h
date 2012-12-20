#ifndef STTAECLUSTERMONITOR_H 
#define STTAECLUSTERMONITOR_H 1

// Include files
// from Gaudi
#include "Kernel/STHistoAlgBase.h"

namespace LHCb{
  class STCluster;
}
class TH2D;

/** @class STTAEClusterMonitor STTAEClusterMonitor.h
 *  
 *
 *  @author Mark Tobin
 *  @date   2009-03-10
 *
 *  Implementation of monitoring algorithm for TAE events.  
 *  Defaults:
 *  (TT) Plots number of clusters per spill for each readout quadrant
 *  (IT) Plots number of clusters per spill for each unique detector region
 *  
 *  Options: 
 *  (TT) Can plot per service box
 *  (TT+IT) Turn off/on 2D histograms
 */
class ISTReadoutTool;
namespace ST
{
  class STTAEClusterMonitor : public HistoAlgBase {
  public: 
  /// Standard constructor
    STTAEClusterMonitor( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~STTAEClusterMonitor( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    
  protected:

  private:
    /// Book histograms

    /// This is the algorithm which plots cluster ADCs vs Sampling time
    void monitorClusters();

    std::vector<std::string> m_clusterLocations; ///< Input location for clusters
    unsigned int m_nSamples; ///< Number of input locations
    double m_maxSample; ///< define the max bin of samples sample
    std::vector<std::string> m_spills;///< Vector of all possible spill location for TAE events

    /// Cut on the bunch ID (distinguish Beam from cosmics)
    std::vector<unsigned int> m_bunchID;

    unsigned int m_maxClusterSize; ///< Cut on number of strips in clusters
    double m_chargeCut;///< Cut on charge
    bool m_plotBySvcBox; ///< Plot by Service Box
    bool m_plotByDetRegion; ///< Plot by unique detector region
    bool m_useMean;///< Use mean instead of highest bin as MPV
    bool m_debug; ///< true if message service level is debug
    bool m_verbose; ///< true if message service level is verbose

    unsigned int m_updateRateMPV;

  private: // Histogram operations
    void bookHistograms();
    AIDA::IProfile1D* m_prof_clustersVsSample;
    AIDA::IHistogram2D* m_2d_ADCsVsSample;

    typedef std::pair<TH2D*, AIDA::IProfile1D*> HistoPair;
    std::map<std::string, HistoPair> m_histos_ADCsVsSampleByServiceBox;
    std::map<std::string, HistoPair> m_histos_ADCsVsSampleByDetRegion;
    void fillHistogram(HistoPair histoPair, int sample, double charge);
    void updateMPVHistograms();
  };
} // End of ST namespace
#endif // STTAECLUSTERMONITOR_H
