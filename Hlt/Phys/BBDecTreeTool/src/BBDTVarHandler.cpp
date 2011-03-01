// $Id: $
// ============================================================================
#include "BBDTVarHandler.h"
// ============================================================================
BBDTVarHandler::BBDTVarHandler(): m_values(7),
    m_SUMPT(LoKi::Cuts::SUMTREE(LoKi::Cuts::PT,"K+" == LoKi::Cuts::ABSID,0.0)),
    m_MINPT(LoKi::Cuts::MINTREE("K+" == LoKi::Cuts::ABSID,LoKi::Cuts::PT)),
    m_BPVIPCHI2(""){
  m_indices["SUMTREE(PT,'K+'==ABSID,0.0)"] = 0;
  m_indices["M"] = 1;
  m_indices["AMAXDOCA('LoKi::DistanceCalculator')"] = 2;
  m_indices["BPVIPCHI2()"] = 3;
  m_indices["BPVCORRM"] = 4;
  m_indices["MINTREE('K+'==ABSID,PT)"] = 5;
  m_indices["BPVVDCHI2"] = 6;
}
// ============================================================================
bool BBDTVarHandler::set(const LHCb::Particle* p,const DVAlgorithm *dvalg, 
			 const IDistanceCalculator* dist) {

  if(0 == p) return false;

  // get variables we need
  const LHCb::VertexBase* bpv = dvalg->bestPV(p);
  // SUMTREE(PT,'K+'==ABSID,0.0)
  m_values[0] = m_SUMPT(p)/Gaudi::Units::MeV; 
  // M
  m_values[1] = p->measuredMass()/Gaudi::Units::MeV; 
  // AMAXDOCA('LoKi::DistanceCalculator')
  const LoKi::ATypes::AFun AMAXDOCA = LoKi::Cuts::AMAXDOCA(dist);
  m_values[2] = AMAXDOCA(p->daughtersVector())/Gaudi::Units::mm; 
  // BPVIPCHI2()
  m_values[3] = m_BPVIPCHI2(p); 
  // BPVCORRM
  const LoKi::Types::Fun BPVCORRM = LoKi::Cuts::CORRM(bpv);
  m_values[4] = BPVCORRM(p)/Gaudi::Units::MeV; 
  // MINTREE('K+'==ABSID,PT)
  m_values[5] = m_MINPT(p)/Gaudi::Units::MeV; 
  // BPVVDCHI2
  const LoKi::Types::Fun BPVVDCHI2 = LoKi::Cuts::VDCHI2(bpv);
  m_values[6] = BPVVDCHI2(p); 

  return true;
}
// ============================================================================
bool BBDTVarHandler::cut(const std::map<std::string,std::pair<double,double> > 
			 &cuts) const {
  std::map<std::string,std::pair<double,double> >::const_iterator iter
    = cuts.begin();
  while(iter != cuts.end()){
    double min_cut = iter->second.first;
    double max_cut = iter->second.second;
    std::map<std::string,int>::const_iterator it = m_indices.find(iter->first);
    if(it == m_indices.end()){
      //Error(iter->first + " is not a known variable!");
      return false;
    }
    double value = m_values[it->second];
    if(min_cut > 0 && value < min_cut) return false;
    if(max_cut > 0 && value > max_cut) return false;
    iter++;
  }
  return true;
}
// ============================================================================
