// $Id: $
// ============================================================================
#include "BBDTVarHandler.h"
#include "LoKi/IHybridFactory.h"
// ============================================================================
double docaMax(const LHCb::Particle *p,const IDistanceCalculator* dist){
  LHCb::Particle::ConstVector daughters = p->daughtersVector();
  int size = daughters.size();
  double max = 0;
  for(int i = 0; i < size; i++){
    if(daughters[i]->particleID().pid() == 22) continue;
    for(int j = i+1; j < size; j++){
      if(daughters[j]->particleID().pid() == 22) continue;
      double doca;
      dist->distance(daughters[i],daughters[j],doca);
      if(doca > max) max = doca;
    }
  }
  return max;
}
// ============================================================================
void heavyVChi2Dof(const LHCb::Particle *p, double &chi2, int &dof){
  LHCb::Particle::ConstVector daughters = p->daughtersVector();
  int size = daughters.size();
  if(!p->particleID().hasCharm() && !p->particleID().hasBottom()) return;
  if(size == 0 || p->endVertex() == 0) return;
  chi2 += p->endVertex()->chi2();
  dof += p->endVertex()->nDoF();
  for(int i = 0; i < size; i++) heavyVChi2Dof(daughters[i],chi2,dof);
}
double totHeavyVChi2Dof(const LHCb::Particle *p){
  double chi2 = 0; int dof = 0;
  heavyVChi2Dof(p,chi2,dof);
  if(dof > 0) return chi2/(double)dof;
  else return 0;
}
// ============================================================================
BBDTVarHandler::BBDTVarHandler(const IDVAlgorithm* dva, const IDistanceCalculator* dist):
  m_dist(dist), m_ipTool(dist), m_dva(dva), m_values(9),m_use(9,false),
  m_SUMPT(LoKi::Cuts::SUMTREE(LoKi::Cuts::PT,LoKi::Cuts::ISBASIC,0.0)),
  m_MINPT(LoKi::Cuts::MINTREE(LoKi::Cuts::ISBASIC,LoKi::Cuts::PT)),
  m_CHI2IP(m_ipTool, 0), m_BPVCORRM(0), m_BPVVDCHI2(0) {
  m_indices["PTSUM"] = 0;
  m_indices["M"] = 1;
  m_indices["DOCA"] = 2;
  m_indices["CANDIPCHI2"] = 3;
  m_indices["MCOR"] = 4;
  m_indices["PTMIN"] = 5;
  m_indices["FDCHI2"] = 6;
  m_indices["PT"] = 7;
  m_indices["HVCHI2DOFTOT"] = 8;
}
// ============================================================================
void BBDTVarHandler::setPIDs(const LoKi::PhysTypes::Cut &cut){
  m_SUMPT = LoKi::Cuts::SUMTREE(LoKi::Cuts::PT,cut,0.0);
  m_MINPT = LoKi::Cuts::MINTREE(cut,LoKi::Cuts::PT);
}
// ============================================================================
StatusCode BBDTVarHandler::initialize(const std::vector<std::string> &vars){
  int size = vars.size();
  m_map.resize(size);
  for(int i = 0; i < size; i++){
    std::map<std::string,int>::const_iterator it = m_indices.find(vars[i]);
    if(it == m_indices.end()) return StatusCode::FAILURE;
    int index = it->second;
    m_use[index] = true;
    m_map[i] = index;
  }
  return StatusCode::SUCCESS;
}
// ============================================================================
StatusCode BBDTVarHandler::initialize(const std::vector<bool> &use){
  int size = use.size();
  m_map.resize(size);
  int index = 0;
  for(int i=0; i < size; i++){
    m_use[i] = use[i];
    if(use[i]){
      m_map[i] = index;
      index++;
    }
  }
  return StatusCode::SUCCESS;
}
// ============================================================================
bool BBDTVarHandler::set(const LHCb::Particle* p) {

  if(0 == p) return false;

  // get variables we need
  const LHCb::VertexBase* bpv = m_dva->bestVertex(p);
  if(m_use[0]) m_values[0] = m_SUMPT(p)/Gaudi::Units::MeV; 
  if(m_use[1]) m_values[1] = p->measuredMass()/Gaudi::Units::MeV; 
  if(m_use[2]) m_values[2] = docaMax(p,m_dist)/Gaudi::Units::mm;
  if(m_use[3]){
    m_CHI2IP.setVertex(bpv);
    m_values[3] = m_CHI2IP(p); 
  }
  if(m_use[4]){
    m_BPVCORRM.setVertex(bpv);
    m_values[4] = m_BPVCORRM(p)/Gaudi::Units::MeV; 
  }
  if(m_use[5]) m_values[5] = m_MINPT(p)/Gaudi::Units::MeV; 
  if(m_use[6]){
    m_BPVVDCHI2.setVertex(bpv);
    m_values[6] = m_BPVVDCHI2(p); 
  }
  if(m_use[7]) m_values[7] = p->pt();
  if(m_use[8]) m_values[8] = totHeavyVChi2Dof(p);

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
