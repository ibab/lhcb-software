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

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

// local
#include "VeloMonitorBase.h"

/** @class VeloTrackMonitor VeloTrackMonitor.h
 *  
 *
 *  @author Zhou Xing
 *  @date   2012-02-03
 *  Low ADC Monitoring histogram added
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
    AIDA::IProfile1D* m_prof_unsensors;
    AIDA::IProfile1D* m_prof_pos_mom_unres;
    AIDA::IProfile1D* m_prof_neg_mom_unres;
    AIDA::IProfile1D* m_prof_sensors;
    AIDA::IProfile1D* m_prof_pos_mom_res;
    AIDA::IProfile1D* m_prof_neg_mom_res;
    AIDA::IProfile1D* m_prof_thetaR;
    AIDA::IProfile1D* m_prof_thetaTot;
    AIDA::IProfile1D* m_prof_pseudoEffsens;
    AIDA::IHistogram1D* m_track_theta;
    AIDA::IHistogram1D* m_track_phi;
    AIDA::IHistogram1D* m_track_eta;
    AIDA::IHistogram1D* m_track_adcsum;
    AIDA::IHistogram1D* m_track_radc;
    AIDA::IHistogram1D* m_track_phiadc;
    AIDA::IHistogram1D* m_track_pseudoeff;
    AIDA::IHistogram1D* m_track_pseudoeff_int;
    AIDA::IHistogram1D* m_used_sensors;
    AIDA::IHistogram1D* m_phi_diff;
    AIDA::IHistogram1D* m_track_rcoord;
    AIDA::IHistogram1D* m_half_phi_a;
    AIDA::IHistogram1D* m_half_phi_c;
    AIDA::IHistogram1D* m_nmeasure;
    AIDA::IHistogram1D* m_nposmeas;
    AIDA::IHistogram1D* m_nnegmeas;
    AIDA::IHistogram1D* m_mod_mismatch;
    AIDA::IHistogram1D* m_nrclus;
    AIDA::IHistogram1D* m_nrphiclus;
    AIDA::IHistogram2D* m_xy_hitmap;
    AIDA::IHistogram2D* m_zx_hitmap;
    AIDA::IProfile1D* m_Rate_DistToM2;
    AIDA::IProfile1D* m_Rate_DistToOutStrip;
    
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
