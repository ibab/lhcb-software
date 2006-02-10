#include "L0MuonKernel/CtrlUnit.h"
#include "L0MuonKernel/CrateUnit.h"

L0Muon::CtrlUnit::CtrlUnit(){

}

L0Muon::CtrlUnit::CtrlUnit(LHCb::MuonTileID id):SelectionUnit(id){
  
}

L0Muon::CtrlUnit::CtrlUnit(DOMNode* pNode):SelectionUnit(pNode){
  
}



L0Muon::CtrlUnit::~CtrlUnit() {}

void L0Muon::CtrlUnit::initialize()
{
  L0MUnit::initialize();

  char buf[4096];
  char* format ;

  // Candidate Register handlers for input candidates
  for (int iboard = 0; iboard <12 ; iboard++) {
    format = "BOARD%d";
    sprintf(buf,format,iboard);
    std::map<std::string,L0Muon::Register*>::iterator itinputs =  m_inputs.find(buf);
    if (itinputs==m_inputs.end()) continue;
    Register* reg = (*itinputs).second;
    m_candRegHandlerIn[iboard] = CandRegisterHandler(reg) ;
  }

  // Candidate Register handler for selected candidates
  format = "CAND_CTRLQ%d";
  sprintf(buf,format,m_mid.quarter()+1);
  std::map<std::string,L0Muon::Register*>::iterator itoutputs =  m_outputs.find(buf);
  if (itoutputs==m_outputs.end()) std::cout <<"L0Muon::CtrlUnit::initialize key "<<buf  <<" not found in output registers\n";
  Register* reg =(*itoutputs).second;
  m_candRegHandlerOut = CandRegisterHandler(reg) ;

}

void L0Muon::CtrlUnit::execute() {
  
  std::map<int, CandRegisterHandler>::iterator itHandlerMap;

  // Fill the board number in the input registers

  for (itHandlerMap=m_candRegHandlerIn.begin();itHandlerMap!=m_candRegHandlerIn.end();itHandlerMap++){
    for (int icand = 0;icand<2;icand++) {
      int iboard = (*itHandlerMap).first;
      if (m_candRegHandlerIn[iboard].isEmpty(icand)) continue;
      m_candRegHandlerIn[iboard].setCandBoard(iboard,icand);
    }
  }

  // Select the 2 best candidates and fill the output register
  select();


  // Call the execute method of the sub units (after the selection)
  L0Muon::Unit::execute(); 
}
