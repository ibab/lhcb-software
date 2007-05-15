// $Id: GhostTrackInfo.cpp,v 1.1 2007-05-15 13:52:43 mneedham Exp $

// local
#include "Event/GhostTrackInfo.h"

//MCParticle* LHCb::GhostTrackInfo::bestMatch() const{

//}


// fillstream method
std::ostream& LHCb::GhostTrackInfo::fillStream(std::ostream& s) const
{
  s << "{ " << "classed as :";
  s << m_classification; 
  s << std::endl << " }";
  return s;
}

LHCb::GhostTrackInfo::LinkPair LHCb::GhostTrackInfo::bestLink() const{

  LHCb::GhostTrackInfo::LinkMap::const_iterator iterMap = m_linkMap.begin();
  double tot = 0.0;
  std::pair<LHCb::MCParticle*,unsigned int> best(0,0);
  for (; iterMap != m_linkMap.end(); ++iterMap){
    if ( iterMap->second > best.second ){
      best = *iterMap;
    }
    tot+= iterMap->second;
  } // iterMap
  
  const double purity = double(best.second)/tot;
  return std::make_pair(best.first,purity); 
}
