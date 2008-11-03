// $Id: VeloTrackMonitor.h,v 1.2 2008-11-03 11:01:55 erodrigu Exp $
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
#include "TrackInterfaces/IMeasurementProvider.h"

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
   
    IMeasurementProvider* m_measurementprovider;
    IVeloClusterPosition* m_clusterTool;
    IVeloExpectation* m_expectTool;
    
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
  };
}

#endif // VELORECMONITORS_VELOTRACKMONITOR_H
