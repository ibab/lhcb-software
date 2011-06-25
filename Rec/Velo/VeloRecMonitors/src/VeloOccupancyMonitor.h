// $Id: VeloOccupancyMonitor.h,v 1.9 2010-04-04 14:15:44 keaveney Exp $
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
#include "Hist1D.h"

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
    LHCb::VeloClusters* m_clusters;
    
    TH1D* m_histOccSpectAll;
    TH1D* m_histOccSpectLow;
    Velo::Hist1D* m_fastHistOccSpectAll;
    Velo::Hist1D* m_fastHistOccSpectLow;
    TH1D* m_histAvrgSensor;
    TH1D* m_histAvrgSensorPO1;
    TH1D* m_histAvrgSensorPO11;
    TH1D* m_histBCIDSpec;
    
    
    std::vector< TH1D* > m_stripOccupancyHistPerSensor;
    std::vector< TH1D* > m_channelOccupancyHistPerSensor;
    std::vector< IProfile1D* > m_veloOccVsBunchId;
   
    std::vector<Velo::Hist1D*> m_occupancies; 
    std::vector<Velo::Hist1D*> m_occupanciesCh; 

    unsigned int m_occupancyDenom;
    unsigned int m_nstrips;
   
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
