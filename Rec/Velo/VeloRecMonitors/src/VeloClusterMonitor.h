// $Id: VeloClusterMonitor.h,v 1.2 2008-08-19 17:08:15 erodrigu Exp $
#ifndef VELORECMONITORS_VELOCLUSTERMONITOR_H 
#define VELORECMONITORS_VELOCLUSTERMONITOR_H 1

// Include files
// -------------

// from DigiEvent
#include "Event/VeloCluster.h"

// local
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
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:
    
  private:

    // Retrieve the VeloClusters
    StatusCode veloClusters();

    // Monitor the VeloClusters
    void monitorClusters();

    // Return a /Prev, /Next or alike if present in the VeloClusters location string
    std::string prevsOrNexts();
    
    // Data members
    LHCb::VeloClusters* m_clusters;

    // Job options
    std::string m_clusterCont;
  };
}

#endif // VELORECMONITORS_VELOCLUSTERMONITOR_H
