#include "Map.h"

const TrackIdealPR::CatMap& TrackIdealPR::catToType()
{
   static TrackIdealPR::CatMap s_map = boost::assign::map_list_of(IMCReconstructible::ChargedVelo,LHCb::Track::Velo)
                                           (IMCReconstructible::ChargedLong,LHCb::Track::Long)
                                           (IMCReconstructible::ChargedUpstream,LHCb::Track::Upstream)
                                           (IMCReconstructible::ChargedDownstream,LHCb::Track::Downstream)
                                           (IMCReconstructible::ChargedTtrack,LHCb::Track::Ttrack);
   return s_map;
}
