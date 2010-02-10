// $Id: STClusterMonitor.h,v 1.8 2010-02-10 11:33:55 mtobin Exp $
#ifndef STCLUSTERMONITOR_H 
#define STCLUSTERMONITOR_H 1

// Include files
// from Gaudi
#include "Kernel/STHistoAlgBase.h"

namespace LHCb{
  class STCluster;
}
class ISTSignalToNoiseTool;

/** @class STClusterMonitor STClusterMonitor.h
 *  
 *
 *  @author Mark Tobin
 *  @date   2009-03-10
 */
namespace ST
{
  class STClusterMonitor : public HistoAlgBase {
  public: 
  /// Standard constructor
    STClusterMonitor( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~STClusterMonitor( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:

  private:
    /// This is the algorithm which plots cluster ADCs vs Sampling time
    void monitorClusters();

    /// Fill basic histograms
    void fillHistograms(const LHCb::STCluster* cluster);

    /// Fill detailed histograms
    void fillDetailedHistograms(const LHCb::STCluster* cluster);

    /// Fill the hitmaps when required.
    void fillHitMaps(const LHCb::STCluster* cluster);
    
    std::string m_clusterLocation; ///< Input location for clusters

    ///< Name of signal to noise tool
    std::string m_sigNoiseToolName;
    ISTSignalToNoiseTool* m_sigNoiseTool; 
    bool m_stn;

    /// Toggles to turn plots off/on
    bool m_plot2D; ///< Toggle 2D plots on/off
    bool m_plotBySvcBox; ///< Plot by Service Box
    bool m_plotByDetRegion; ///< Plot by unique detector region
    bool m_plotByPort; ///< Plot number of clusters/tell1 by vs port
    bool m_hitMaps; ///< True if cluster maps are to be shown
    bool m_debug; ///< true if message service level is debug
    bool m_verbose; ///< true if message service level is verbose

    unsigned int m_nTELL1s; ///< Number of TELL1 boards expected.

    /// Store number of clusters/TELL1 (48 (42) Tell1s, 1->48 (42) for TT (IT))
    std::vector<unsigned int> m_nClustersPerTELL1;

  private:

    /// Book histograms
    void bookHistograms();

    // filled in monitor clusters
    AIDA::IHistogram1D* m_1d_nClusters;///< Number of clusters
    AIDA::IHistogram1D* m_1d_nClusters_gt_100;///< Number of clusters (N > 100)
    AIDA::IHistogram2D* m_2d_nClustersVsTELL1;///< Number of clusters per TELL1

    // filled in fillHistograms
    AIDA::IHistogram1D* m_1d_ClusterSize;///< Cluster Size
    AIDA::IHistogram2D* m_2d_ClusterSizeVsTELL1;///< Cluster Size vs TELL1
    AIDA::IHistogram2D* m_2d_STNVsTELL1;///< Signal to Noise vs TELL1
    AIDA::IHistogram2D* m_2d_ChargeVsTELL1;///< Cluster Charge vs TELL1
    AIDA::IHistogram2D* m_2d_ClustersPerPortVsTELL1;///< Clusters per port vs TELL1

    AIDA::IHistogram1D* m_1d_totalCharge;///< Total charge in cluster

    /// Plots by service box
    std::map<std::string, AIDA::IHistogram1D*> m_1ds_chargeByServiceBox;
    /// Plots by detector region
    std::map<std::string, AIDA::IHistogram1D*> m_1ds_chargeByDetRegion;

    /// Hitmaps
    AIDA::IHistogram2D* m_2d_hitmap;///< Cluster hitmap

    /// Cut on the bunch ID (distinguish Beam from cosmics)
    std::vector<unsigned int> m_bunchID;

    static const unsigned int m_nBinsPerTTSector=16u;///< Number of bins per TT sector in the hitmap (beetle ports)
    static const unsigned int m_nBinsPerITSector=12u;///< Number of bins in each IT sector (beetle ports)

  };
} // End of ST namespace
#endif // STCLUSTERMONITOR_H
