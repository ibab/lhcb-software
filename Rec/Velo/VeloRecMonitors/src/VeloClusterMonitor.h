#ifndef VELORECMONITORS_VELOCLUSTERMONITOR_H 
#define VELORECMONITORS_VELOCLUSTERMONITOR_H 1

// Include files
// -------------
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "Event/VeloCluster.h"

#include "VeloMonitorBase.h"

/** @class VeloClusterMonitor VeloClusterMonitor.h
 *  
 *  @author Eduardo Rodrigues
 *  @date   2008-08-18
 *
 *  @author Mark Tobin, Kazu Akiba
 *  @date   2008-06-28
 *
 *  Based on original version by
 *  @author Aras Papadelis, Thijs Versloot
 *  @date   2006-04-30
 *
 */

namespace Velo
{
  
  class VeloClusterMonitor : public VeloMonitorBase {
    
  public:
    /// Standard constructor
    VeloClusterMonitor( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloClusterMonitor( );     ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    
  protected:
    
  private:

    // Retrieve the VeloClusters
    StatusCode veloClusters();

    // Monitor the VeloClusters
    void monitorClusters();

    // Fill the high-multiplicity histogram
    void fillHighMultiplicity(unsigned int nClustersDecoded);

    // Get number of clusters from raw event
    unsigned int getNClustersFromRaw();

  private:
    // Data members
    std::string m_tae;

    LHCb::VeloClusters* m_clusters;
    std::vector<unsigned int> m_nClustersPerSensor;
    
    // Histograms
    AIDA::IHistogram1D* m_hNCluEvt;
    AIDA::IHistogram1D* m_hNCluEvtZoom;
    AIDA::IHistogram1D* m_hNClustersHM;
    AIDA::IHistogram1D* m_hCluSize;
    AIDA::IHistogram2D* m_hCluSizeSens;
    AIDA::IHistogram1D* m_hCluADC;
    AIDA::IHistogram2D* m_hCluADCSens;
    AIDA::IHistogram1D* m_hCluADCR;
    AIDA::IHistogram1D* m_hCluADCPhi;
    AIDA::IHistogram1D* m_hSeedADC;
    AIDA::IHistogram1D* m_hSeedADCR;
    AIDA::IHistogram1D* m_hSeedADCPhi;
    AIDA::IHistogram1D* m_hIncADC;
    AIDA::IHistogram1D* m_hIncADCR;
    AIDA::IHistogram1D* m_hIncADCPhi;
    AIDA::IHistogram2D* m_hActiveLinkSens;
    
    std::vector<AIDA::IHistogram1D*> m_hNCluSens;
    
    // Job options
    std::string m_clusterCont;
    bool        m_perSensorPlots;
    bool        m_highMultiplicityPlot;
  };
}

#endif // VELORECMONITORS_VELOCLUSTERMONITOR_H

