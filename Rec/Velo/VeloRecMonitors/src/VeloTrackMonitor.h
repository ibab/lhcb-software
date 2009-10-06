// $Id: VeloTrackMonitor.h,v 1.9 2009-10-06 19:55:53 wouter Exp $
#ifndef VELORECMONITORS_VELOTRACKMONITOR_H 
#define VELORECMONITORS_VELOTRACKMONITOR_H 1

// Include files
//--------------

//from DigiEvent
#include "Event/VeloCluster.h"

//from TrackEvent
#include "Event/Track.h"
#include "Event/State.h"

//from TrackFitEvent
#include "Event/FitNode.h"

//from TrackInterfaces
#include "TrackInterfaces/IVeloClusterPosition.h"
#include "TrackInterfaces/IVeloExpectation.h"

//from Gaudi
#include "GaudiAlg/IHistoTool.h"
#include "GaudiAlg/GaudiTupleAlg.h"

// local
#include "VeloMonitorBase.h"

/** @class VeloTrackMonitor VeloTrackMonitor.h
 *  
 *
 *  @author Sadia Khalil
 *  @date   2008-09-15
 *  Based on original version by
 *  @author Aras Papadelis, Thijs Versloot
 *  @date   2006-04-30
 */

namespace Velo
{
  class VeloTrackMonitor : public VeloMonitorBase{
  public: 
    /// Standard constructor
    VeloTrackMonitor( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloTrackMonitor( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

  private:

  protected:

    StatusCode getVeloClusters();
    StatusCode getVeloTracks();
    StatusCode MCInfo();
    StatusCode monitorTracks();
    StatusCode unbiasedResiduals(LHCb::Track *track);
    Gaudi::XYZPoint extrapolateToZ(LHCb::Track *track, double toZ);
    DeVelo* m_veloDet;
    
    // Data members
    LHCb::Tracks* m_tracks;
    LHCb::VeloClusters* m_clusters;
    LHCb::VeloClusters*m_rawClusters;
   
    IVeloClusterPosition* m_clusterTool;
    IVeloExpectation* m_expectTool;

    AIDA::IHistogram1D *m_h_aliMon_Mean_R_A;
    AIDA::IHistogram1D *m_h_aliMon_Mean_R_C;
    AIDA::IHistogram1D *m_h_aliMon_Mean_P_A;
    AIDA::IHistogram1D *m_h_aliMon_Mean_P_C;
    AIDA::IHistogram1D *m_h_aliMon_Sigma_R_A;
    AIDA::IHistogram1D *m_h_aliMon_Sigma_R_C;
    AIDA::IHistogram1D *m_h_aliMon_Sigma_P_A;
    AIDA::IHistogram1D *m_h_aliMon_Sigma_P_C;
    AIDA::IProfile1D* m_prof_res[ 84 ]; 
    
    // Job options
    unsigned long m_event;
    unsigned long m_resetProfile;
    std::string m_clusterCont;
    std::string m_trackCont;
    bool m_hitmapHistos;   
    bool m_biasedResidualProfile;
    bool m_unbiasedResidualProfile;
    bool m_ACDC;
    bool m_xPlots;
    bool m_EventClusterInfo;
    bool m_alignMoniBasic;
    unsigned int m_alignNMeasMin;
    double m_binary;
  };
}

#endif // VELORECMONITORS_VELOTRACKMONITOR_H
