// $Id: STClusterMonitor.h,v 1.14 2010/04/12 13:13:24 mtobin Exp $
#ifndef STCLUSTERMONITOR_H 
#define STCLUSTERMONITOR_H 1

// Include files
// from Gaudi
#include "Kernel/STHistoAlgBase.h"

namespace LHCb{
  class STCluster;
}
class DeSTSector;


// Boost accumulator classes to store MPVs of sectors
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/mean.hpp>

// Use root histos to get MPV for sectors (in absence of something better)
class TH1D;

/** @class STClusterMonitor STClusterMonitor.h
 *  
 *
 *  @author Mark Tobin
 *  @date   2009-03-10
 */

/// Define some typedefs for boost accumulators
namespace ST {

  // Median of distribution
  typedef boost::accumulators::accumulator_set
  <double,boost::accumulators::stats<boost::accumulators::tag::median(boost::accumulators::with_p_square_quantile) > > 
  MedianAccumulator;

  // Mean of distribution
  typedef boost::accumulators::accumulator_set
  <double,boost::accumulators::stats<boost::accumulators::tag::mean(boost::accumulators::immediate) > > 
  MeanAccumulator;

}

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
    void fillClusterMaps(const LHCb::STCluster* cluster);

    /// Fill the cluster mpv map per sector
    void fillMPVMap(const DeSTSector* sector, double charge);

    std::string m_clusterLocation; ///< Input location for clusters



    /// Toggles to turn plots off/on
    bool m_plot2D; ///< Toggle 2D plots on/off
    bool m_plotBySvcBox; ///< Plot by Service Box
    bool m_plotByDetRegion; ///< Plot by unique detector region
    bool m_plotByPort; ///< Plot number of clusters/tell1 by vs port
    bool m_hitMaps; ///< True if cluster maps are to be shown
    bool m_debug; ///< true if message service level is debug
    bool m_verbose; ///< true if message service level is verbose

    /// Cuts on the data quality
    unsigned int m_minNClusters; ///< Cut on minimum number of clusters in the event
    unsigned int m_maxNClusters; ///< Cut on maximum number of clusters in the event
    double m_chargeCut;///< Cut on charge
    double m_minMPVCharge;///< Minimum charge for calculation on MPV
    
    unsigned int m_nTELL1s; ///< Number of TELL1 boards expected.

    /// Store number of clusters/TELL1 (48 (42) Tell1s, 1->48 (42) for TT (IT))
    std::vector<unsigned int> m_nClustersPerTELL1;

  private:

    /// Book histograms
    void bookHistograms();

    // filled in monitor clusters
    AIDA::IHistogram1D* m_1d_nClusters;///< Number of clusters
    AIDA::IHistogram1D* m_1d_nClusters_gt_100;///< Number of clusters (N > 100)
    AIDA::IHistogram1D* m_1d_nClustersVsTELL1;///< Number of clusters per TELL1
    AIDA::IProfile1D* m_prof_nClustersVsTELL1;///< Number of clusters per TELL1
    AIDA::IHistogram2D* m_2d_nClustersVsTELL1;///< Number of clusters per TELL1

    AIDA::IHistogram1D* m_1d_nClustersVsTELL1Links;///< Number of clusters per TELL1 (split into links)
    AIDA::IHistogram1D* m_1d_nClustersVsTELL1Sectors;///< Number of clusters per TELL1 (split into sectors)

    unsigned int m_overFlowLimit;
    AIDA::IHistogram1D* m_1d_nClusters_overflow;///< Number of clusters where last bin contains overflow info

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
    AIDA::IProfile2D* m_2dp_clSize_map;///< Cluster size map
    AIDA::IProfile1D* m_prof_sectorMPVs;///< Sector MPV vs arbitrary sector number 
    AIDA::IProfile1D* m_prof_sectorTruncMean1;///< Sector MPV calculated using truncated mean losing first and last 15%
    AIDA::IProfile1D* m_prof_sectorTruncMean2;///< Sector MPV calculated using truncated mean 1st 70% of ADC distribution
    AIDA::IProfile1D* m_prof_sectorBinnedMPV;///< Sector MPV vs arbitrary sector number 
    AIDA::IHistogram2D* m_2d_sectorMPVs;///< Sector MPV maps
    AIDA::IHistogram2D* m_2d_sectorMPVsNorm;///< Use to normalize the MPV distribution in the history mode of the presenter

    unsigned int m_nSectors;///< Number of sectors (ie hybrids)
    unsigned int m_nBeetlePortsPerSector; ///< Number of beetle ports per sector
    static const unsigned int m_nBeetlePortsPerTTSector=16u;///< Number of bins per TT sector in the hitmap (beetle ports)
    static const unsigned int m_nBeetlePortsPerUTSector=16u;///< Number of bins per UT sector in the hitmap (beetle ports)
    static const unsigned int m_nBeetlePortsPerITSector=12u;///< Number of bins in each IT sector (beetle ports)
    unsigned int m_nSectorsPerTELL1;
    /// Accumulation of statistics for the MPV per sector
    std::map<const unsigned int,TH1D*> m_1ds_chargeBySector;
    std::map<const unsigned int, std::vector<double> > m_chargeBySector;

    /// Contains the bin corresponding to a given sectors in the 1d representation of the hitmap
    std::map<const unsigned int, unsigned int> m_sectorBins1D;
    AIDA::IHistogram1D* m_1d_nClustersVsSector; ///< Number of clusters in each sector
    AIDA::IHistogram1D* m_1d_nClustersVsBeetlePort; ///< Number of clusters in each beetle port

    /// Reset rate for MPV calculation
    unsigned int m_resetRate;

  };
} // End of ST namespace
#endif // STCLUSTERMONITOR_H
