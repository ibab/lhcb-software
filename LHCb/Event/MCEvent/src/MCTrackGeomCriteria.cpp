#include "Event/MCTrackGeomCriteria.h"
#include <boost/assign/list_of.hpp> // for 'map_list_of()'

LHCb::MC::MCTrackGeomCriteria::MCTrackGeomCriteria(std::vector<std::string>& criteria)
{
  // contructor
  for (std::vector<std::string>::iterator iterS = criteria.begin(); 
       iterS != criteria.end(); ++iterS){
   Criteria crit = theMap().toType(*iterS);
   m_criteria.push_back(crit);
  } // iterS
}

bool LHCb::MC::MCTrackGeomCriteria::accepted(MCTrackInfo& info, 
                                   const LHCb::MCParticle* aParticle) const{

  // make the acceptance test....
  std::vector<Criteria>::const_iterator iter = m_criteria.begin();
  while (iter != m_criteria.end() && (*iter)(info,aParticle) == true){
    ++iter;
  }
  return (iter != m_criteria.end() ? false : true); 
}

LHCb::MC::MCTrackGeomCriteria::FunctionMap::FunctionMap(){
  // constructer
  m_mapping = 
      boost::assign::map_list_of("hasVeloAndT",&MCTrackInfo::hasVeloAndT)
                         ("hasTT",&MCTrackInfo::hasTT)
                         ("hasT",&MCTrackInfo::hasT)
                         ("hasVelo",&MCTrackInfo::hasVelo)
                         ("hasVeloR",&MCTrackInfo::hasVeloR)
                         ("hasVeloPhi",&MCTrackInfo::hasVeloPhi)
                         ("hasTT1",&MCTrackInfo::hasTT1)
                         ("hasTT2",&MCTrackInfo::hasTT2)
                         ("hasT1X",&MCTrackInfo::hasT1X)
                         ("hasT1S",&MCTrackInfo::hasT1S)
                         ("hasT2X",&MCTrackInfo::hasT2X)
                         ("hasT2S",&MCTrackInfo::hasT2S)
                         ("hasT3X",&MCTrackInfo::hasT3X)
                         ("hasT3S",&MCTrackInfo::hasT3S)
                         ("hasT1",&MCTrackInfo::hasT1)
                         ("hasT2",&MCTrackInfo::hasT2)
                         ("hasT3",&MCTrackInfo::hasT3)
                         ("accVeloAndT",&MCTrackInfo::accVeloAndT)
                         ("accTT",&MCTrackInfo::accTT)
                         ("accT",&MCTrackInfo::accT)
                         ("accVelo",&MCTrackInfo::accVelo)
                         ("accVeloR",&MCTrackInfo::accVeloR)
                         ("accVeloPhi",&MCTrackInfo::accVeloPhi)
                         ("accTT1",&MCTrackInfo::accTT1)
                         ("accTT2",&MCTrackInfo::accTT2)
                         ("accT1X",&MCTrackInfo::accT1X)
                         ("accT1S",&MCTrackInfo::accT1S)
                         ("accT2X",&MCTrackInfo::accT2X)
                         ("accT2S",&MCTrackInfo::accT2S)
                         ("accT3X",&MCTrackInfo::accT3X)
                         ("accT3S",&MCTrackInfo::accT3S)
                         ("accT1",&MCTrackInfo::accT1)
                         ("accT2",&MCTrackInfo::accT2)
                         ("accT3",&MCTrackInfo::accT3);
}
