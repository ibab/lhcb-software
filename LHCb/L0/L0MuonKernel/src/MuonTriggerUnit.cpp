#include "L0MuonKernel/MuonTriggerUnit.h"

L0Muon::MuonTriggerUnit::MuonTriggerUnit() {}

L0Muon::MuonTriggerUnit::MuonTriggerUnit(DOMNode* pNode):L0MUnit(pNode){
}

L0Muon::MuonTriggerUnit::~MuonTriggerUnit() {}

void L0Muon::MuonTriggerUnit::initialize() {

  m_ignoreM1     =  m_properties["ignoreM1"];
  m_ptparameters =  m_properties["ptParameters"]; 
  m_xfoi         =  m_properties["foiXSize"]; 
  m_yfoi         =  m_properties["foiYSize"];

  L0Muon::Unit::initialize();
}


int L0Muon::MuonTriggerUnit::xFoi(int sta)
{
  int xfoi= m_xfoi[sta];
  return xfoi;  
}

int L0Muon::MuonTriggerUnit::yFoi(int sta)
{
  int yfoi= m_yfoi[sta];
  return yfoi;
}
