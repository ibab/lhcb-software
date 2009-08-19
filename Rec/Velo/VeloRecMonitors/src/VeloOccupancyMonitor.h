// $Id: VeloOccupancyMonitor.h,v 1.2 2009-08-19 09:04:38 keaveney Exp $
#ifndef VELORECMONITORS_VELOOCCUPANCYMONITOR_H 
#define VELORECMONITORS_VELOOCCUPANCYMONITOR_H 1

// Include files
// -------------
#include "TH1D.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

#include "Event/VeloCluster.h"

#include "DetDesc/Condition.h"

#include "VeloMonitorBase.h"

// from LHCb
#include "Event/ODIN.h"

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
    LHCb::ODIN* m_odin;
    std::vector< int > m_nClusters;
    LHCb::VeloClusters* m_clusters;
    
    TH1D* m_histOccSpectAll;
    TH1D* m_histOccSpectLow;
    TH1D* m_histAvrgSensor;
    std::vector< TH1D* > m_stripOccupancyHistPerSensor;
    std::vector< TH1D* > m_channelOccupancyHistPerSensor;
    std::vector< IProfile1D* > h_veloOccVsBunchId;
    

    unsigned int m_occupancyDenom;
    double m_nstrips;
    double m_percOcc; 
   
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
