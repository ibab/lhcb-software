// $Id: VeloOccupancyMonitor.h,v 1.6 2009-09-15 11:39:50 krinnert Exp $
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

#include "Tell1Kernel/ITELL1SensorMap.h"
#include "Tell1Kernel/ITimeStampProvider.h"
#include "Tell1Kernel/IPvssTell1Names.h"

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
    void veloClusters();
    // Retrieve the ODIN bank
    void getOdinBank();

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
    
    // tools
    Velo::ITELL1SensorMap* m_tell1Map;
    Velo::ITimeStampProvider* m_timeStamps;
    Velo::IPvssTell1Names* m_pvssTell1Names;

    // Job options
    bool m_writeXML;
    std::string m_xmlDir;
    std::string m_paramName;
    double m_highOccCut;
    
    // Job options
    std::string m_clusterCont;
    unsigned int m_occupancyResetFreq;	
    bool m_useOdin;
    
  };
}

#endif // VELORECMONITORS_VELOOCCUPANCYMONITOR_H
