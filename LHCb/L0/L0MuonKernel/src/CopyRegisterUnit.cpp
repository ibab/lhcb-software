#include <vector>
#include "L0MuonKernel/CopyRegisterUnit.h"

L0Muon::CopyRegisterUnit::CopyRegisterUnit() {};
    
L0Muon::CopyRegisterUnit::~CopyRegisterUnit() {};  

void L0Muon::CopyRegisterUnit::execute() {

  //int inputs = m_inputs.size();
  //int outputs = m_outputs.size();
  //if (inputs != outputs ) {
  //}
  std::map<std::string,L0Muon::Register*>::iterator inr;
  std::map<std::string,L0Muon::Register*>::iterator outr;
  
  for ( inr = m_inputs.begin(), outr = m_outputs.begin(); 
        inr != m_inputs.end() && outr != m_outputs.end(); inr++, outr++ ) {
    (*outr).second = (*inr).second;
  }
  
}

