#include "L0MuonKernel/Link.h"

L0Muon::Link::Link() {}

L0Muon::Link::Link(L0Muon::Register* read, L0Muon::Register* write) {
  if ( m_inputs.empty() ) {    
    std::string iss = read->name();
    m_inputs[iss] = read ;
  } else {
    //std::cout << "Link Error: too many inputs !" << std::endl;
  }
  if ( m_outputs.empty() ) {
    std::string iss = write->name();
    m_outputs[iss] = write ;
  } else {
    //std::cout << "Link Error: too many outputs !" << std::endl;
  }
}

L0Muon::Link::~Link() {} 

void L0Muon::Link::addInputRegister(L0Muon::Register* in) {
  if ( m_inputs.empty() ) {
    std::string iss = in->name();
    m_inputs[iss] = in ;
    } else {
      //std::cout << "Link Error: too many inputs !" << std::endl;
    }
}

void L0Muon::Link::addOutputRegister(L0Muon::Register* out) {
  if ( m_outputs.empty() ) {
    std::string iss = out->name();
    m_outputs[iss] = out ;
  } else {
    //std::cout << "Link Error: too many outputs !" << std::endl;
  }
}
  


void L0Muon::Link::execute() {
  
  std::map<std::string,L0Muon::Register*>::iterator iu;
  std::map<std::string,L0Muon::Register*>::iterator ou;  
  for ( iu = m_inputs.begin(), ou = m_outputs.begin(); 
        iu != m_inputs.end() && ou != m_outputs.end(); iu++, ou++ ) {
    (*(*ou).second) = (*(*iu).second);
  }
  
}

void L0Muon::Link::initialize() {}

void L0Muon::Link::finalize() {}

