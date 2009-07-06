// $Id: STTAEClusterMonitor.h,v 1.2 2009-07-06 17:30:58 mtobin Exp $
#ifndef STTAECLUSTERMONITOR_H 
#define STTAECLUSTERMONITOR_H 1

// Include files
// from Gaudi
#include "Kernel/STHistoAlgBase.h"

namespace LHCb{
  class STCluster;
}

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
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:

  private:
    /// Book histograms

    /// This is the algorithm which plots cluster ADCs vs Sampling time
    void monitorClusters();

    std::vector<std::string> m_clusterLocations; ///< Input location for clusters
    unsigned int m_nSamples; ///< Number of input locations
    double m_maxSample; ///< define the max bin of samples sample
    std::vector<std::string> m_spills;///< Vector of all possible spill location for TAE events
    unsigned int m_maxClusterSize; ///< Cut on number of strips in clusters
    bool m_plotBySvcBox; ///< Plot by Service Box
    bool m_plotByDetRegion; ///< Plot by unique detector region
    bool m_debug; ///< true if message service level is debug
    bool m_verbose; ///< true if message service level is verbose


  private: // Booking histograms
    void bookHistograms();
    AIDA::IProfile1D* m_prof_clustersVsSample;
    AIDA::IHistogram2D* m_2d_ADCsVsSample;
    std::map<std::string, AIDA::IHistogram2D*> m_2ds_ADCsVsSampleByServiceBox;
    std::map<std::string, AIDA::IProfile1D*> m_profs_ADCsVsSampleByServiceBox;
    std::map<std::string, AIDA::IHistogram2D*> m_2ds_ADCsVsSampleByDetRegion;
    std::map<std::string, AIDA::IProfile1D*> m_profs_ADCsVsSampleByDetRegion;
  };
} // End of ST namespace
#endif // STTAECLUSTERMONITOR_H
