
#include "Map.h"

const TrackMaps::TypeMap& TrackMaps::typeDescription()
{
  static TrackMaps::TypeMap s_map = boost::assign::map_list_of("Velo",LHCb::Track::Velo)
                                           ("VeloR",LHCb::Track::VeloR)
                                           ("Long",LHCb::Track::Long)
                                           ("Upstream",LHCb::Track::Upstream)
                                           ("Downstream",LHCb::Track::Downstream)
                                           ("Ttrack",LHCb::Track::Ttrack)
                                           ("Muon",LHCb::Track::Muon);
  return s_map;
}
            
//const TrackMaps::InfoMap& TrackMaps::infoDescription()
//{
//   static TrackMaps::InfoMap s_map ;
//   if ( s_map.empty() ) { 
//     s_map = boost::assign::map_list_of("Likelihood",LHCb::Track::Likelihood)
//                                       ("PatQuality",LHCb::Track::PatQuality)
//                                       ("MatchChi2",LHCb::Track::MatchChi2);
//   };
//   return s_map ;
//}


const TrackMaps::RecMap& TrackMaps::recDescription()
{
   static TrackMaps::RecMap s_map = boost::assign::map_list_of
	     ("ChargedLong",IMCReconstructible::ChargedLong)
             ("ChargedDownstream",IMCReconstructible::ChargedDownstream )
             ("ChargedUpstream", IMCReconstructible::ChargedUpstream)
             ("ChargedTtrack",IMCReconstructible::ChargedTtrack)
             ("ChargedVelo", IMCReconstructible::ChargedVelo);
   return s_map;
}



const TrackMaps::InfoHistMap& TrackMaps::infoHistDescription()
{
   static InfoHistMap s_map = boost::assign::map_list_of(LHCb::Track::FitTChi2,HistoRange("FitTChi2", 0., 100.))
       (LHCb::Track::FitTNDoF,HistoRange("FitTNDof", 0., 50.))
       (LHCb::Track::FitVeloChi2,HistoRange("FitVeloChi2", 0., 100.))
       (LHCb::Track::FitVeloNDoF,HistoRange("FitVeloNDoF", 0., 50.))
       (LHCb::Track::FitMatchChi2,HistoRange("FitMatchChi2", 0., 100.));
   return s_map;
}
