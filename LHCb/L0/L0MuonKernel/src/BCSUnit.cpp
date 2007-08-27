#include "L0MuonKernel/L0MuonStatus.h"
#include "L0MuonKernel/BCSUnit.h"
#include "L0MuonKernel/CrateUnit.h"
//#include "Event/L0Muon.h"
#include <stdio.h>
#include <cmath>

/* 3 fields:

  1. Identification field: number of bits 32 (2 words) 
     - bits  0-11: L0Event number
     - bits 12-15: free
     - bits 16-27: L0_B_Id to fill 
     - bits 28-31: free 

  2. Output field: number of bits 48 (3 words)
     - bits  0-15: for pt of candidate 1 & 2.
     - bits 16-31: for addresses of candidate 1 & 2 in M3 
     - bits 32-33: and 40-41 for pu identifier for candidates 1 and 2
     - bits 44-47: for status (0<= number of candidates <=8) 

  3. Input field: number of bits 240 (15 words) 
     - bits  0- 3: BcId of Pu1
     - bits  4- 7: status word of Pu1
     - bits  8-15: first pt of Pu1   
     - bits 16-23: second Pt of Pu1 
     - bits 23-37: first addr (M3+M2+M1) of Pu1-- erratum: bits 23-38
     - bits 38-38: free -- erratum: bit 39
     - bits 39-54: second addr (M3+M2+M1) of Pu1-- erratum: bits 40-54
     - bits 55-55: free
     - ... and so on for Pu2, Pu3, Pu4

  To do :
     - In Identification field I have to add BcId
     - In output field I have to put the absolue address in M1 (to define)

*/


L0Muon::BCSUnit::BCSUnit(LHCb::MuonTileID id):SelectionUnit(id){
  
}

L0Muon::BCSUnit::BCSUnit(DOMNode* pNode):SelectionUnit(pNode){
  
}

L0Muon::BCSUnit::~BCSUnit(){
}

void L0Muon::BCSUnit::initialize()
{

  char buf[4096];
  char* format ;

  // Candidate Register handlers for input candidates
  for (int ipu = 0; ipu <4 ; ipu++) {
    format = "PU%d";
    sprintf(buf,format,ipu);
    std::map<std::string,L0Muon::Register*>::iterator itinputs =  m_inputs.find(buf);
    if (itinputs==m_inputs.end()) continue;
    Register* reg = (*itinputs).second;
    m_candRegHandlerIn[ipu] = CandRegisterHandler(reg) ;
  }

  // Candidate Register handler for selected candidates
  format = "CAND_BCSUQ%dR%d%d%d";
  sprintf(buf,format,m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
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
