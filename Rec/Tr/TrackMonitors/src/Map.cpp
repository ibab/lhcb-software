#include "Map.h"

const TrackMonitorMaps::TypeMap& TrackMonitorMaps::typeDescription()
{
  static const TrackMonitorMaps::TypeMap s_map = { {"Velo",LHCb::Track::Velo},
                                                   {"VeloR",LHCb::Track::VeloR},
                                                   {"Long",LHCb::Track::Long},
                                                   {"Upstream",LHCb::Track::Upstream},
                                                   {"Downstream",LHCb::Track::Downstream},
                                                   {"Ttrack",LHCb::Track::Ttrack},
                                                   {"Muon",LHCb::Track::Muon} };
  return s_map;
}
            
const TrackMonitorMaps::InfoHistMap& TrackMonitorMaps::infoHistDescription()
{
   static const  InfoHistMap s_map = { {LHCb::Track::FitTChi2,HistoRange("8", 0., 100.)},
                                       {LHCb::Track::FitTNDoF,HistoRange("9", 0., 50.)},
                                       {LHCb::Track::FitVeloChi2,HistoRange("10", 0., 100.)},
                                       {LHCb::Track::FitVeloNDoF,HistoRange("11", 0., 50.)},
                                       {LHCb::Track::FitMatchChi2,HistoRange("12", 0., 100.)} };
   return s_map;
}
