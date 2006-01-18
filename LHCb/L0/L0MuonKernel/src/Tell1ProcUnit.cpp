#include "L0MuonKernel/Tell1ProcUnit.h"

L0Muon::Tell1ProcUnit::Tell1ProcUnit(){

}

L0Muon::Tell1ProcUnit::Tell1ProcUnit(LHCb::MuonTileID id):L0MUnit(id){
  
}

L0Muon::Tell1ProcUnit::Tell1ProcUnit(DOMNode* pNode):L0MUnit(pNode){
  
}



L0Muon::Tell1ProcUnit::~Tell1ProcUnit() {}
