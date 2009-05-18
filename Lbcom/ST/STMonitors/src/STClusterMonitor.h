// $Id: STClusterMonitor.h,v 1.2 2009-05-18 11:35:45 mtobin Exp $
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
    bool m_hitMaps; ///< True if cluster maps are to be shown
    bool m_debug; ///< true if message service level is debug
    bool m_verbose; ///< true if message service level is verbose

    unsigned int m_nTELL1s; ///< Number of TELL1 boards expected.
    unsigned int m_maxNClusters; ///< Maximum number of clusters expected

    /// Store number of clusters/TELL1 (48 Tell1s, 1->48)
    std::vector<unsigned int> m_nClustersPerTELL1;

  };
} // End of ST namespace
#endif // STCLUSTERMONITOR_H
