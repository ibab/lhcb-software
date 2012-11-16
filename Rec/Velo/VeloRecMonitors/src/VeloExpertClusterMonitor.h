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

/** @class VeloExpertClusterMonitor VeloExpertClusterMonitor.h
 *  
 *
 *  @author Abdirizack NOOR
 *  @date   2008-12-04
 */
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
    LHCb::Tracks*       m_tracks;
    LHCb::VeloClusters* m_clusters;

   
      
  private:
    //    DeVelo* m_veloDet;
    StatusCode loopTracks();
    StatusCode loopClusters();
    StatusCode plotCluster(LHCb::VeloCluster* cluster,std::string ClusterType, double theta=-400.,double prap = -400.);
    StatusCode plotSensorsADC(double& adc,std::string corr,std::string& ClusterType,int sensor_num = -400);
    StatusCode plotSensorsSize(double& clsize,std::string corr,std::string& ClusterType,int sensor_num = -400);
    StatusCode plotAngles(double& adc,std::string& range,std::string& ClusterType,
                          //double theta=-400.,
                          int sensor_num = -400);
    StatusCode plotRPhiRange( LHCb::VeloCluster* cluster,LHCb::VeloChannelID idtemp, double adc,
                              std::string path,double clsize,std::string ClusterType, double theta);
    bool m_Expert;   
    bool m_Shift;  
    bool m_PhiZones;
    bool m_BothRange;
    bool m_RZones;
    bool m_RRange;
    bool m_PhiRange;
    bool m_RapidRange;
    bool m_Matching;
    bool m_ThetaRange;
    int m_i_max;
    bool m_plotClSize;
    AIDA::IHistogram1D *m_adc_hist[84][3];

    std::string m_clusterCont;
    std::string m_trackCont; 


  };
}

#endif // VELOEXPERTCLUSTERMONITOR_H

