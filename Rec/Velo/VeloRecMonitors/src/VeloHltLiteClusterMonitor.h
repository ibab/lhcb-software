// $Id: VeloHltLiteClusterMonitor.h,v 1.3 2009-04-14 11:25:51 mjohn Exp $
#ifndef VELORECMONITORS_VELOHLTLITECLUSTERMONITOR_H 
#define VELORECMONITORS_VELOHLTLITECLUSTERMONITOR_H 1

// from Gaudi
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram1D.h"

// from LHCb
#include "Event/ODIN.h"

// from DigiEvent
#include "Event/VeloLiteCluster.h"

// local
#include "VeloMonitorBase.h"

/** @class VeloHltLiteClusterMonitor
 *  
 *  Reduced-memory cluster monitor for running in the HLT
 *
 */

namespace Velo
{
  
  class VeloHltLiteClusterMonitor : public VeloMonitorBase {
    
  public:
    /// Standard constructor
    VeloHltLiteClusterMonitor( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloHltLiteClusterMonitor( );     ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:
    
  private:

    // Retrieve the VeloLiteClusters
    StatusCode veloLiteClusters();

    // Monitor the VeloLiteClusters
    void monitorLiteClusters();

    // Data members
    LHCb::ODIN* m_odin;
    LHCb::VeloLiteCluster::FastContainer* m_liteClusters;
    std::vector< std::vector<int> > m_nRClusters;
    std::string m_liteClusterLocation;
    unsigned int m_nRSensorsOn;
    bool m_histogramByZone;

    // Histogram pointers
    std::vector<IHistogram1D*> h_nClustersSmallRange;
    std::vector<IHistogram1D*> h_nClustersLargeRange;
    std::vector< IProfile1D* > h_nClustersVsZ;
    std::vector< IProfile1D* > h_nClustersVsBunchId;
    std::vector< std::vector<IHistogram1D*> > h_nClustersByZone;
    std::vector< std::vector< IProfile1D* > > h_nClustersVsZByZone;
   };
}

#endif // VELORECMONITORS_VELOHLTLITECLUSTERMONITOR_H
