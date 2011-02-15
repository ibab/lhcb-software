// $Id: $
// ============================================================================
#include "BBDTSimpleTool.h"
// ============================================================================
BBDTSimpleTool::BBDTSimpleTool(const std::string& type, 
			       const std::string& name, 
			       const IInterface* parent) 
  : base_class(type,name,parent), m_nbody(-1), m_dist(0), m_dva(0), m_vars(){
  // declare configurable properties
  declareProperty("NBody", m_nbody, "2, 3 or 4-body?");
}
// ===========================================================================
StatusCode BBDTSimpleTool::initialize() {

  // initialize the base class  (the first action)
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure()) return sc; 

  // get tools and algs
  m_dist = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);
  m_dva = Gaudi::Utils::getDVAlgorithm(contextSvc());
  if (0 == m_dva) 
    return Error("Couldn't get parent DVAlgorithm", StatusCode::FAILURE);  

  return StatusCode::SUCCESS;
}
// ============================================================================
bool BBDTSimpleTool::operator()(const LHCb::Particle* p) const {

  if(0 == p) {
    Error("LHCb::Particle* points to NULL, return false");
    return false ;
  }

  if(m_nbody < 2 || m_nbody > 4){
    Error("Not configured for 2, 3 or 4-body, return false");
    return false;
  }
  if(!m_vars.set(p,m_dva,m_dist)) return false;

  double ptSum = m_vars.ptSum(),m = m_vars.mass(),doca = m_vars.doca(),
    ipChi2 = m_vars.ipChi2(), mCor = m_vars.massCor(),ptMin = m_vars.ptMin(),
    fdChi2 = m_vars.fdChi2();

  if(m > 7000 || doca > 0.2 || fdChi2 < 100 || ptSum < 3000) return false;  
  if(m_nbody == 2){
    if(m > 4900 && m < 5900 && ipChi2 < 20 && doca < 0.1){
      if((ptSum > 4000 && fdChi2 > 300) || (ptSum > 6000)) return true;
    }
  }
  if(mCor < 4000 || mCor > 7000) return false;

  if(m_nbody == 2){
    if(ptMin > 3000) return true;
    if(ptSum > 10000) return true;
    if(m < 2000) return false;
    if(fdChi2 > 1000){
      if(ptSum > 5000 && ptMin > 750) return true;
    }
    else if(fdChi2 > 300){
      if(ptSum > 5500 && ptMin > 1000) return true;
    }
    return false;
  }
  else if(m_nbody == 3){
    if(ptSum < 4000 || doca > 0.1) return false;
    if(ptMin > 3000) return true;
    if(ptSum > 15000) return true;
    if(m < 3000) return false;
    if(mCor < 4500 || mCor > 6500) return false;
    if(fdChi2 > 300){
      if(ptMin > 2000) return true;
      if(ptSum > 6000 && ptMin > 1000) return true;
      if(ptSum > 7500 && ptMin > 750) return true;
    }
    if(fdChi2 > 1000){    
      if(ptMin > 1500) return true;
      if(ptSum > 7000 && ptMin > 600) return true;
    }
    return false;
  }
  else {
    if(ptSum < 4000 || doca > 0.1) return false;
    if(ptMin > 2000) return true;
    if(ptSum > 15000) return true;
    if(m < 3500) return false;
    if(fdChi2 > 300){
      if(ptMin > 1750) return true;
      if(ptSum > 8000 && ptMin > 600) return true;
    }
    if(fdChi2 > 1000){    
      if(ptMin > 1250) return true;
      if(ptSum > 7500 && ptMin > 550) return true;
    }
    return false;
  }
}
// ============================================================================
/// declare & implement the factory 
DECLARE_TOOL_FACTORY(BBDTSimpleTool);
// ============================================================================
