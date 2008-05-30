
#include "Map.h"

const TrackMonitorMaps::TypeMap& TrackMonitorMaps::typeDescription()
{
  static TrackMonitorMaps::TypeMap s_map ;
   if ( s_map.empty() ) { 
    s_map = boost::assign::map_list_of("Velo",LHCb::Track::Velo)
                                           ("VeloR",LHCb::Track::VeloR)
                                           ("Long",LHCb::Track::Long)
                                           ("Upstream",LHCb::Track::Upstream)
                                           ("Downstream",LHCb::Track::Downstream)
                                           ("Ttrack",LHCb::Track::Ttrack)
                                           ("Muon",LHCb::Track::Muon);
   }
   return s_map ;
}
            
const TrackMonitorMaps::InfoMap& TrackMonitorMaps::infoDescription()
{
   static TrackMonitorMaps::InfoMap s_map ;
   if ( s_map.empty() ) { 
     s_map = boost::assign::map_list_of("Likelihood",LHCb::Track::Likelihood)
                                       ("PatQuality",LHCb::Track::PatQuality)
                                       ("MatchChi2",LHCb::Track::MatchChi2);
   };
   return s_map ;
}


const TrackMonitorMaps::InfoHistMap& TrackMonitorMaps::infoHistDescription()
{
   static InfoHistMap s_map ;
   if ( s_map.empty() ) { 
     s_map = boost::assign::map_list_of(LHCb::Track::Likelihood,HistoRange("1",-100., 10.))
       (LHCb::Track::PatQuality,HistoRange("2",0., 20.))
       (LHCb::Track::MatchChi2,HistoRange("3", 0., 1000.))    
       (LHCb::Track::nExpectedVelo,HistoRange("4", 0., 50.))
       (LHCb::Track::nExpectedTT,HistoRange("5", 0., 50.))
       (LHCb::Track::nExpectedOT,HistoRange("6", 0., 50.))
       (LHCb::Track::nExpectedIT,HistoRange("7", 0., 50.))
       (LHCb::Track::FitTChi2,HistoRange("8", 0., 100.))
       (LHCb::Track::FitTNDoF,HistoRange("9", 0., 50.))
       (LHCb::Track::FitVeloChi2,HistoRange("10", 0., 100.))
       (LHCb::Track::FitVeloNDoF,HistoRange("11", 0., 50.))
       (LHCb::Track::FitMatchChi2,HistoRange("12", 0., 100.));
   }
   return s_map ;
}
