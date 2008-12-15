#include "L0MuonKernel/BCSUnit.h"
#include "L0MuonKernel/CrateUnit.h"
//#include "Event/L0Muon.h"
#include <stdio.h>
#include <cmath>

L0Muon::BCSUnit::BCSUnit(LHCb::MuonTileID id):SelectionUnit(id){
  
}

L0Muon::BCSUnit::BCSUnit(DOMNode* pNode):SelectionUnit(pNode){
  
}

L0Muon::BCSUnit::~BCSUnit(){
}

void L0Muon::BCSUnit::initialize()
{

  char buf[4096];

  // Candidate Register handlers for input candidates
  for (int ipu = 0; ipu <4 ; ipu++) {
    sprintf(buf,"PU%d",ipu);
    std::map<std::string,L0Muon::Register*>::iterator itinputs =  m_inputs.find(buf);
    if (itinputs==m_inputs.end()) continue;
    Register* reg = (*itinputs).second;
    m_candRegHandlerIn[ipu] = CandRegisterHandler(reg) ;
  }

  // Candidate Register handler for selected candidates
  sprintf(buf,"CAND_BCSUQ%dR%d%d%d",m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
  std::map<std::string,L0Muon::Register*>::iterator itoutputs =  m_outputs.find(buf);
  if (itoutputs==m_outputs.end()) std::cout <<"L0Muon::BCSUnit::initialize key "<<buf  <<" not found in output registers\n";
  Register* reg =(*itoutputs).second;
  m_candRegHandlerOut = CandRegisterHandler(reg) ;

  L0MUnit::initialize();
}


void L0Muon::BCSUnit::execute() {
  if (m_debug) std::cout<<"L0Muon::BCSUnit::execute mid: "
                        <<"Q"<<m_mid.quarter()+1<<"R"<<m_mid.region()+1<<m_mid.nX()<<m_mid.nY()<<std::endl;

  // Fill the pu number in the input registers
  std::map<int, CandRegisterHandler>::iterator itHandlerMap;
  for (itHandlerMap=m_candRegHandlerIn.begin();itHandlerMap!=m_candRegHandlerIn.end();itHandlerMap++){
    int ncand= (*itHandlerMap).second.numberOfCandidates();
    if (m_debug) std::cout<< "L0Muon::BCSUnit::execute number of cand in register: "<<ncand<<std::endl;
    ncand = ncand>2 ? 2 : ncand;
    for (int icand = 0;icand<ncand;icand++) {
      //if (m_candRegHandlerIn[iboard].isEmpty(icand)) continue;
      int ipu = (*itHandlerMap).first;
      if (m_debug) std::cout<< "L0Muon::BCSUnit::execute setting PU # "<<ipu
                            << " for candidate # "<<icand<<std::endl;
      m_candRegHandlerIn[ipu].setCandPU(ipu,icand);
      if (m_debug) m_candRegHandlerIn[ipu].dump(icand);
    }
  }
  
  // Select the 2 best candidates and fill the output register
  select();

  L0Muon::Unit::execute();

}
