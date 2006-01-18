#include "L0MuonKernel/BoardUnit.h"
#include "L0MuonKernel/CrateUnit.h"

L0Muon::BoardUnit::BoardUnit(){

}

L0Muon::BoardUnit::BoardUnit(LHCb::MuonTileID id):L0MUnit(id){
  
}

L0Muon::BoardUnit::BoardUnit(DOMNode* pNode):L0MUnit(pNode){
  
}



L0Muon::BoardUnit::~BoardUnit() {}
