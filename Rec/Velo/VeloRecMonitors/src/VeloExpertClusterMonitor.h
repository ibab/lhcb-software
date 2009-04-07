// $Id: VeloExpertClusterMonitor.h,v 1.3 2009-04-07 14:39:08 jmylroie Exp $
#ifndef VELOEXPERTCLUSTERMONITOR_H 
#define VELOEXPERTCLUSTERMONITOR_H 1

// Include files
// from Gaudi
//#include "GaudiAlg/GaudiAlgorithm.h"
#include "VeloMonitorBase.h"
#include "Event/VeloCluster.h"
#include "Event/Track.h"
#include "TrackInterfaces/IVeloClusterPosition.h"
#include "TrackInterfaces/IVeloExpectation.h"
#include "TrackInterfaces/IMeasurementProvider.h"

/** @class ClusterMon ClusterMon.h
 *  
 *
 *  @author Abdirizack NOOR
 *  @date   2008-12-04
 */
//class ClusterMon : public GaudiAlgorithm {
namespace Velo
{
  class VeloExpertClusterMonitor : public VeloMonitorBase {//GaudiHistoAlg
  public: 
    /// Standard constructor
    VeloExpertClusterMonitor( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloExpertClusterMonitor( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:
    LHCb::Tracks* tracks;
    DeVelo* m_veloDet;
     LHCb::VeloClusters* m_clusters;
   
      
  private:
    //    DeVelo* m_veloDet;
    StatusCode loopTracks();
    StatusCode plotCluster(LHCb::VeloCluster* cluster,std::string ClusterType, double theta=-400.,double prap = -400.);
    StatusCode plotSensorsADC(double& adc,std::string corr,std::string& ClusterType,int sensor_num = -400);
    StatusCode plotSensorsSize(double& clsize,std::string corr,std::string& ClusterType,int sensor_num = -400);
    StatusCode plotAngles(double& adc,std::string& range,std::string& ClusterType, double theta=-400.,int sensor_num = -400);
    StatusCode plotRPhiRange( LHCb::VeloCluster* cluster,LHCb::VeloChannelID idtemp, double adc,
                              std::string path,double clsize,std::string ClusterType, double theta);
   
    //    LHCb::VeloClusters m_clusters;
    bool m_useExpert;   
    bool m_useShift;  
    std::string m_clusterCont;
    std::string m_trackCont; 


  };
}

#endif // CLUSTERMON_H
