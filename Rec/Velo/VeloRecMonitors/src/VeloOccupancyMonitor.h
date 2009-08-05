// $Id: VeloOccupancyMonitor.h,v 1.1 2009-08-05 15:00:44 krinnert Exp $
#ifndef VELORECMONITORS_VELOOCCUPANCYMONITOR_H 
#define VELORECMONITORS_VELOOCCUPANCYMONITOR_H 1

// Include files
// -------------
#include "TH1D.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "Event/VeloCluster.h"

#include "DetDesc/Condition.h"

#include "VeloMonitorBase.h"

/** @class VeloOccupancyMonitor VeloOccupancyMonitor.h
 *  
 *  @author Kurt Rinnert
 *  @date   2009-08-04
 *
 */

namespace Velo
{
  
  class VeloOccupancyMonitor : public VeloMonitorBase {
    
  public:
    /// Standard constructor
    VeloOccupancyMonitor( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloOccupancyMonitor( );     ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:
    
  private:

    // Retrieve the VeloClusters
    StatusCode veloClusters();

    // Monitor the VeloClusters
    void monitorOccupancy();

    // Data members
    std::string m_tae;

    LHCb::VeloClusters* m_clusters;
    
    TH1D* m_histOccSpectAll;
    TH1D* m_histOccSpectLow;
    TH1D* m_histAvrgSensor;
    std::vector< TH1D* > m_stripOccupancyHistPerSensor;
    std::vector< TH1D* > m_channelOccupancyHistPerSensor;
    unsigned int m_occupancyDenom;
   
    std::map<unsigned int, Condition> m_conditions; 
    
    // Job options
    std::string m_clusterCont;
    unsigned int m_occupancyResetFreq;	
    bool m_writeXML;
    std::string m_xmlDir;
    std::string m_paramName;
    double m_highOccCut;
    
  };
}

#endif // VELORECMONITORS_VELOOCCUPANCYMONITOR_H
